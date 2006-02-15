// cppgraph -- C++ call graph analyzer
//
//! @file Subdir.cc
//! @brief This file contains the implementation of class Subdir.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <string>
#include <vector>
#include "realpath.h"
#include "Subdir.h"
#include "debug.h"

SubdirSet subdirs;

void initialize_subdirs(std::string const& cwd, std::vector<std::string> const& cmdline_subdirs)
{
  subdirs.insert(cwd);
  Dout(dc::subdirs, "Added: " << cwd);
  for (std::vector<std::string>::const_iterator cmdline_subdir_iter = cmdline_subdirs.begin();
       cmdline_subdir_iter != cmdline_subdirs.end(); ++cmdline_subdir_iter)
  {
    std::string canonical_path = realpath(*cmdline_subdir_iter);
    bool recursive = true;
    for (SubdirSet::iterator iter = subdirs.begin(); iter != subdirs.end(); ++iter)
    {
      if (iter->realpath().size() > canonical_path.size() &&
	  iter->realpath().substr(0, canonical_path.size()) == canonical_path)
      {
	recursive = false;
	break;
      }
    }
    std::pair<SubdirSet::iterator, bool> res = subdirs.insert(canonical_path);
    if (!res.second)
      continue;
    Dout(dc::subdirs, "Added: " << canonical_path << (recursive ? "" : " (not recursive)"));
    if (!recursive)
    {
      const_cast<Subdir&>(*res.first).reset_recursive();
      continue;
    }
    while (!canonical_path.empty())
    {
      canonical_path.erase(canonical_path.find_last_of('/'));
      Dout(dc::subdirs, "Search:" << canonical_path);
      SubdirSet::iterator iter = subdirs.find(canonical_path);
      if (iter != subdirs.end())
      {
	if (!iter->is_recursive())
	{
	  Dout(dc::subdirs, "Already not recursive");
	  break;
	}
	const_cast<Subdir&>(*iter).reset_recursive();
	Dout(dc::subdirs, "Made not recursive");
      }
    }
  }
}

