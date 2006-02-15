// cppgraph -- C++ call graph analyzer
//
//! @file realpath.cc
//! @brief This file contains the implementation of function realpath.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <string>
#include <cerrno>
#include "exceptions.h"

int const path_max = 4096;

//! \brief Return canonical path name of \a path.
//
// realpath expands all symbolic links and resolves references to '/./', '/../' and extra '/' characters in
// the string named by path and returns the canonicalized absolute pathname.
// The resulting path will have no symbolic link, '/./' or '/../' components, also no trailing ones.
// Nor will it  end on a slash: if the result is the root then the returned path is empty,
// and unless the result is empty, it will always start with a slash.
//
// @throws std::runtime_error

std::string realpath(std::string const& path)
{
  // Buffer for getcwd and readlink.
  char buf[path_max];

  std::string full_path;
  if (path.size() > 0 && path[0] == '/')
    full_path = path;
  else
  {
    if (getcwd(buf, sizeof(buf)) == NULL)
      THROW_EXCEPTION(std::runtime_error(std::string("realpath: getcwd failed: ") + strerror(errno)),
	  "getcwd(" << (void*)buf << ", " << sizeof(buf) << ") == NULL: " << strerror(errno));
    full_path = buf;
    if (path.size() > 0 && full_path[full_path.size() - 1] != '/')
      full_path += '/';
    full_path += path;
  }

  std::string result;
  std::string::iterator slash1 = full_path.begin();
  for (std::string::iterator slash2 = slash1; slash1 != full_path.end(); slash1 = slash2)
  {
    while (*++slash2 != '/' && slash2 != full_path.end());	// Find the next slash.
    // Now we have:
    // /usr/src/cppgraph/cppgraph-objdir/src/lll
    //         ^        ^
    //      slash1    slash2
    // or
    // /usr/src/cppgraph/cppgraph-objdir/src/lll
    //                                      ^   ^
    //                                  slash1  slash2
    // result is either empty, or starts with a slash.
    size_t dirlen = slash2 - slash1;
    // Ignore trailing slashes, sequences of '//', sequences of '/./' and a trailing '/.'.
    if (dirlen == 1 || (dirlen == 2 && slash1[1] == '.'))
      continue;
    // Process sequences of '/../' or a trailing '/..'.
    if (dirlen == 3 && slash1[1] == '.' && slash1[2] == '.')
    {
      if (result.size() > 0)		// "/.." is the same as "/".
      {
	std::string::iterator iter = result.end();
	while (*--iter != '/');
	result.erase(iter, result.end());
      }
      continue;
    }
    result += full_path.substr(slash1 - full_path.begin(), dirlen);
    int len = readlink(result.c_str(), buf, sizeof(buf));
    if (len >= (int)sizeof(buf))
    {
      errno = ERANGE;
      len = -1;
    }
    else if (len != -1)
    {
      if (buf[0] == '/')
      {
        result.clear();
	full_path.replace(full_path.begin(), slash2, buf, len);
	slash2 = full_path.begin();
      }
      else
      {
        result.erase(result.end() - dirlen, result.end());
	size_t len1 = slash1 - full_path.begin();
	full_path.replace(slash1 + 1, slash2, buf, len);
	slash2 = full_path.begin() + len1;
      }
      continue;
    }
    if (len == -1 && errno != EINVAL)
      THROW_EXCEPTION(std::runtime_error(result + ": " + strerror(errno)),
	  "readlink(\"" << result << "\", " << (void*)buf << ", " << sizeof(buf) << ") == -1: " << strerror(errno));
  }
  return result;
}
