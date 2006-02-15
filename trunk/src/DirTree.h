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

class DirTree {
public:
  typedef std::set<DirTree> container_type;
private:
  container_type::iterator const M_KEY_parent;			// The parent directory.
  std::string M_KEY_dirname;					// Part of the path, ie "foo/bar". Never empty.
  int M_totalfiles;						// The number of files in this directory (recursively).
  std::vector<container_type::iterator> M_path_rest;		// Possible branches.
  bool M_has_include;						// M_KEY_long_name contains the subdirectory 'include'.
  bool M_is_general;						// Marked as general installation path.
  bool M_is_project;						// Marked as project directory;
  bool M_is_system;						// Marked as system directory;
  container_type::iterator M_iter;
public:
  DirTree(std::string const& dirname);
  DirTree(container_type::iterator parent, std::string const& dirname,
      int totalfiles, bool general_prefix, bool is_project, bool is_system);

  std::string const& dirname(void) const { return M_KEY_dirname; }
  container_type::iterator parent(void) const { return M_KEY_parent; }
  std::string str(void) const;
  int totalfiles(void) const { return M_totalfiles; }
  int nrfiles(void) const;
  bool has_include(void) const;
  bool is_general(void) const { return M_is_general; }
  bool is_project(void) const { return M_is_project; }
  bool is_system(void) const { return M_is_system; }
  bool has_project(void) const;
  std::vector<container_type::iterator> const& children(void) const { return M_path_rest; }
  container_type::iterator get_iter(void) const { return M_iter; }

  void add_path(container_type::iterator path_rest) { M_path_rest.push_back(path_rest); }
  void erase(void);
  void erase_children(void);

  static char const* seperator(void) { return "/"; }
  bool process(void) const { return true; }
};

typedef DirTree::container_type DirTrees;

extern DirTrees dirtrees;
void initialize_dirtrees(
    std::vector<std::string> const& cmdline_prefixdirs,
    std::vector<std::string> const& cmdline_systemdirs);

inline bool operator<(DirTree const& dirtree1, DirTree const& dirtree2)
{
  return dirtree1.str() < dirtree2.str();
}

extern std::ostream& operator<<(std::ostream& os, DirTree const& dirtree);

#endif // DIRTREE_H
