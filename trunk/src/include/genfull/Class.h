// cppgraph -- C++ call graph analyzer
//
//! @file Class.h
//! @brief This file contains the declaration of class Class.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include "ClassData.h"
#include "Classes.h"
#include "Project.h"

class Function;

class Class : public ClassData<Classes, Class, Project> {
public:
  Class(std::string const& base_name) : ClassData<Classes, Class, Project>(base_name)
  {
    add(container, *this);
    M_is_class = M_iter->M_is_class;
    M_parent = M_iter->M_parent;
    M_is_functor = M_iter->M_is_functor;
  }
  
  void set_class(void) { M_is_class = true; }
  void set_parent(void);
  void set_functor(void) { M_is_functor = true; }
  void add_project(Project const& project);
};

extern bool operator<(Project::container_type::iterator p1, Project::container_type::iterator p2);

#endif // CLASS_H
