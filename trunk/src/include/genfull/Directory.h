// cppgraph -- C++ call graph analyzer
//
//! @file Directory.h
//! @brief This file contains the declaration of class Directory.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <set>
#include "DirectoryData.h"
#include "serialization.h"

class Directory : public DirectoryData<std::set<Directory> > {
public: 
  Directory(std::string const& directory, bool is_project = false) :
      DirectoryData<std::set<Directory> >(directory, is_project), M_count(0) { add(container, *this); }

  int count(void) const { return M_count; }
  void increment_count(void) { ++M_count; }

private:
  int M_count;				// Number of time this directory name occurs.
};

void initialize_directories(std::vector<std::string> const& cmdline_projectdirs);

#endif // DIRECTORY_H
