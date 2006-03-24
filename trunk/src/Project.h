// cppgraph -- C++ call graph analyzer
//
//! @file Project.h
//! @brief This file contains the declaration of class Project.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <set>
#include <vector>
#include "ProjectData.h"
#include "DirTree.h"

class Project : public ProjectData<std::set<Project>, DirTree> {
public:
  Project(DirTree::container_type::iterator const& dirtree_iter) :
      ProjectData<std::set<Project>, DirTree>("", dirtree_iter) { add(container, *this); }

  void generate_long_name(void) { M_KEY_long_name = M_KEY_dirtree->str(); init_short_name(M_iter); }

private:
  // Serialization.
  friend class boost::serialization::access;
  Project(void) { }		// Uninitialized object (needed for serialization).
};

void initialize_projects(void);

#endif // PROJECT_H
