// cppgraph -- C++ call graph analyzer
//
//! @file Class.h
//! @brief This file contains the implementation of class Class.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Class.h"
#include "Function.h"
#include "debug.h"

Classes classes;

Class::Class(std::string const& base_name) : M_KEY_base_name(base_name), M_is_class(false), M_parent(classes.end())
{
  M_iter = classes.insert(*this).first;
  const_cast<Class&>(*M_iter).M_iter = M_iter;
}

void Class::set_parent(void)
{
  // We can be called more than once, speed that up.
  if (M_parent != classes.end())
    return;
  std::string::size_type pos = M_KEY_base_name.rfind("::");
  std::string parent_name;
  if (pos != std::string::npos)
    parent_name.assign(M_KEY_base_name, 0, pos);
  else if (M_KEY_base_name.empty())
    return;
  Class parent(parent_name);
  M_parent = parent.get_iter();
}

void Class::add_project(Project const& project)
{
  Projects::iterator project_iter = project.get_iter();
  M_projects.insert(std::pair<Projects::iterator, int>(project_iter, 0)).first->second++;
}

std::string const Class::S_root_namespace = "::";

