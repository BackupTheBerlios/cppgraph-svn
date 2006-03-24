// cppgraph -- C++ call graph analyzer
//
//! @file DirTree.h
//! @brief This file contains the declaration of class DirTree.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DIRTREE_H
#define DIRTREE_H

#include <set>
#include <vector>
#include <string>
#include <iosfwd>
#include "DirTreeData.h"

class DirTree : public DirTreeData<std::set<DirTree> > {
public:
  DirTree(std::string const& dirname) : DirTreeData<std::set<DirTree> >(dirname) { }
  DirTree(container_type::iterator parent, std::string const& dirname,
      int totalfiles, bool general_prefix, bool is_project, bool is_system) :
      DirTreeData<std::set<DirTree> >(parent, dirname, totalfiles, general_prefix, is_project, is_system)
  {
    add(container, *this);
    Dout(dc::dirtree, "Added DirTree with parent \"" <<
	((parent == container.end()) ? std::string("") : parent->str()) <<
	"\", rest \"" << dirname << "\" and totalfiles " << totalfiles << ". M_has_include = " <<
	(M_has_include ? "true" : "false") << ", M_is_general = " << (M_is_general ? "true" : "false") <<
	", M_is_project = " << (M_is_project ? "true" : "false") <<
	", M_is_system = " << (M_is_system ? "true" : "false"));
    if (parent != container.end())
      const_cast<DirTree&>(*parent).add_path(M_iter);
  }

public:
  void add_path(container_type::iterator path_rest) { M_path_rest.push_back(path_rest); }
  void erase(void);
  void erase_children(void);
};

void initialize_dirtrees(
    std::vector<std::string> const& cmdline_prefixdirs,
    std::vector<std::string> const& cmdline_systemdirs);

extern std::ostream& operator<<(std::ostream& os, DirTree const& dirtree);

#endif // DIRTREE_H
