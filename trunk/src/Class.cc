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

void Class::set_parent(void)
{
  // We can be called more than once, speed that up.
  if (M_parent != container.end())
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
  if (M_KEY_base_name.substr(0, 3) == "std" && project.short_name() != "4.0.2")
    DoutFatal(dc::core, "Assigning " << project.short_name() << " to " << M_KEY_base_name);
  Project::container_type::iterator project_iter = project.get_iter();
  M_projects.insert(std::pair<Project::container_type::iterator, int>(project_iter, 0)).first->second++;
}
