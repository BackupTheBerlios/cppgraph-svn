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
#include "LongName.h"

class Project;

class FileName : public LongName<FileName> {
public:
  typedef std::set<FileName> container_type;
private:
  std::set<Project>::iterator M_project;
  container_type::iterator M_iter;
  bool M_is_source_file;			// Set when filename ends on .cc, .cpp, .cxx or .C.
public:
  FileName(std::string const& filename);

  container_type::iterator get_iter(void) const { return M_iter; }
  bool is_real_name(void) const { return M_KEY_long_name[0] == '/'; }
  void set_project(std::set<Project>::iterator project_iter) { M_project = project_iter; }
  Project const& get_project(void) const;
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return is_real_name(); }
  bool is_source_file(void) const { return M_is_source_file; }
};

typedef FileName::container_type FileNames;

extern FileNames filenames;

#endif // FILENAME_H
