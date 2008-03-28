// cppgraph -- C++ call graph analyzer
//
//! @file FileName.h
//! @brief This file contains the declaration of class FileName.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef FILENAME_H
#define FILENAME_H

#include <string>
#include <set>
#include "FileNameData.h"
#include "Project.h"

class FileName : public FileNameData<std::set<FileName> > {
public:
  FileName(std::string const& filename) : FileNameData<std::set<FileName> >(filename)
  {
    if (add(container, *this))
    {
      init_short_name(this->M_iter);
      bool is_source_file = false;
      std::string::size_type pos = this->M_KEY_long_name.rfind('.');
      if (pos != std::string::npos)
      {
	std::string extension = this->M_KEY_long_name.substr(pos);
	is_source_file =
	  extension == ".cc" ||
	  extension == ".cxx" ||
	  extension == ".cpp" ||
	  extension == ".C" ||
	  extension == ".c";
      }
      const_cast<FileName&>(*this->M_iter).M_is_source_file = is_source_file;
    }
    M_is_source_file = this->M_iter->M_is_source_file;
  }
  void set_project(Project::container_type::iterator project_iter) { M_project = project_iter; }
};

#endif // FILENAME_H
