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
#include "DirTree.h"
#include "LongName.h"
#include "Node.h"

class Project : public Node, public LongName<Project> {
public:
  typedef std::set<Project> container_type;
private:
  DirTrees::iterator M_KEY_dirtree;
  container_type::iterator M_iter;
public:
  Project(DirTrees::iterator const& dirtree_iter);

  std::string directory(void) const { return M_KEY_dirtree->str(); }
  int nrfiles(void) const { return M_KEY_dirtree->nrfiles() ; }
  container_type::iterator get_iter(void) const { return M_iter; }
  void generate_long_name(void) { M_KEY_long_name = M_KEY_dirtree->str(); init_short_name(M_iter); }
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return true; }

  virtual std::string const& node_name(void) const { return short_name(); }
  virtual NodeType node_type(void) const { return project_node; }

  friend bool operator<(Project const& project1, Project const& project2)
      { return project1.M_KEY_dirtree->str() > project2.M_KEY_dirtree->str(); }
};

typedef Project::container_type Projects;

extern Projects projects;
void initialize_projects(void);

#endif // PROJECT_H
