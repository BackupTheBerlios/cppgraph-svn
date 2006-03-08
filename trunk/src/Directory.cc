// cppgraph -- C++ call graph analyzer
//
//! @file Directory.cc
//! @brief This file contains the implementation of class Directory.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <vector>
#include "Directory.h"
#include "FileName.h"

Directories directories;

void initialize_directories(std::vector<std::string> const& cmdline_projectdirs)
{
  // Pre-mark directories as project directories.
  for (std::vector<std::string>::const_iterator iter = cmdline_projectdirs.begin();
       iter != cmdline_projectdirs.end(); ++iter)
  {
    Dout(dc::dirvec, "Adding directory \"" << *iter << "\", because it was passed as --projectdir.");
    directories.insert(Directory(*iter, true));
  }
  for (FileNames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
  {
    if (!iter->is_real_name())
      continue;
    std::string filename = iter->long_name();
    std::string::iterator slash = filename.begin();
    // If the first character is either a "." or a "/", we can skip it
    // because we're not supporting source files in the root.
    int subs = 0;
    do
    {
      while (++slash != filename.end() && *slash != '/');
      if (slash == filename.end())
	break;
      std::string path(filename.begin(), slash);
      Dout(dc::dirvec, "Adding directory \"" << path << "\", because of filename \"" << filename << "\".");
      Directories::iterator iter = directories.insert(Directory(path)).first;
      assert(path == iter->str());
      const_cast<Directory&>(*iter).increment_count();
      const_cast<Directory&>(*iter).subs() = ++subs;
    }
    while(1);
  }
}

