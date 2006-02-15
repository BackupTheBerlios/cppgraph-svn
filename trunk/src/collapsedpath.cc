// cppgraph -- C++ call graph analyzer
//
//! @file collapsedpath.cc
//! @brief This file contains the implementation of function collapsedpath.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "collapsedpath.h"

std::string collapsedpath(std::string const& filename)
{
  std::string result;
  std::string::size_type left = filename.size();
  int count = 0;			// Number of unprocessed "/..".
  for (std::string::size_type right = left; right != std::string::npos; right = left)
  {
    do
    {
      if (left == 0)
      {
        left = std::string::npos;	// This is -1.
	break;
      }
    }
    while (filename[--left] != '/');

    // Now left and right point to places like:
    //
    //  /foo/bar/xyz
    //      ^   ^    
    //
    //  /foo/bar
    //      ^   ^
    //
    //  /foo/bar/
    //          ^^
    //
    //  /foo/bar/
    //      ^   ^
    //
    //  ../bar 
    // ^  ^    
    //
    //  /foo/bar
    // ^^

    size_t len = right - left;		// Therefore this still works, even if left = npos.
    if (len == 1)
    {
      if (left == std::string::npos)
      {
        count = 0;			// Eat all remaining "/..".
	if (result.empty())
	  result = "/";			// To distinguish "." from "/", the first returns an empty result (too).
	break;
      }
      continue;				// Skip null path.
    }
    if (len == 2 && filename[right - 1] == '.')
      continue;				// Skip "/.".
    if (len == 3 && filename[right - 1] == '.' && filename[right - 2] == '.')
      ++count;				// Count number of unprocessed "/..".
    else if (count > 0)
    {
      if (--count == 0 && left == std::string::npos && !result.empty() && result[0] == '/')
	result.erase(0, 1);		// Make path relative if the left-most part (though eaten) is relative.
      continue;
    }
    else if (left != std::string::npos)
      result = filename.substr(left, len) + result;
    else
      result = filename.substr(0, len - 1) + result;
  }
  if (count > 0)
  {
    while (--count > 0)
      result = "/.." + result;
    result = ".." + result;
  }
  return result;
}
