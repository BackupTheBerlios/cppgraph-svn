// cppgraph -- C++ call graph analyzer
//
//! @file FileName.cc
//! @brief This file contains the implementation of class FileName.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "FileName.h"
#include "collapsedpath.h"
#include "Project.h"

FileNames filenames;

FileName::FileName(std::string const& filename) :
    LongName<FileName>((filename[0] == '/') ? collapsedpath(filename) : filename)
{
  std::pair<FileNames::iterator, bool> result = filenames.insert(*this);
  M_iter = result.first;
  const_cast<FileName&>(*M_iter).M_iter = M_iter;
  if (result.second)
  {
    init_short_name(M_iter);
    bool is_source_file = false;
    std::string::size_type pos = M_KEY_long_name.rfind('.');
    if (pos != std::string::npos)
    {
      std::string extension = M_KEY_long_name.substr(pos);
      is_source_file =
        extension == ".cc" ||
	extension == ".cxx" ||
	extension == ".cpp" ||
	extension == ".C" ||
	extension == ".c";
    }
    const_cast<FileName&>(*M_iter).M_is_source_file = is_source_file;
  }
  M_is_source_file = M_iter->M_is_source_file;
}

Project const& FileName::get_project(void) const
{
  return *M_project;
}
