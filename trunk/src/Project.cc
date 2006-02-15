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

Projects projects;

Project::Project(DirTrees::iterator const& dirtree): LongName<Project>(""), M_KEY_dirtree(dirtree)
{
  M_iter = projects.insert(*this).first;
  const_cast<Project&>(*M_iter).M_iter = M_iter;
}

void initialize_projects(void)
{
  for (DirTrees::iterator iter = dirtrees.begin(); iter != dirtrees.end(); ++iter)
  {
    if (iter->nrfiles())
    {
      Project const project(iter);
    }
  }
  for (Projects::iterator iter = projects.begin(); iter != projects.end(); ++iter)
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
