// cppgraph -- C++ call graph analyzer
//
//! @file Project.cc
//! @brief This file contains the implementation of class Project.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Project.h"

void initialize_projects(void)
{
  for (DirTree::container_type::iterator iter = DirTree::container.begin(); iter != DirTree::container.end(); ++iter)
  {
    if (iter->nrfiles())
    {
      Project const project(iter);
    }
  }
  for (Project::container_type::iterator iter = Project::container.begin(); iter != Project::container.end(); ++iter)
    const_cast<Project&>(*iter).generate_long_name();
  Project::generate_short_names();
}

#ifdef CWDEBUG
std::ostream& operator<<(std::ostream& os, Project const& project)
{
  os << project.short_name();
  return os;
}
#endif

