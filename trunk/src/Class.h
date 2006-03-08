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
#include <map>
#include "Classes.h"
#include "Project.h"
#include "Node.h"

class Function;

class Class : public Node {
public:
  typedef Classes container_type;
private:
  std::string M_KEY_base_name;			// For example, "std::_Rb_tree<>::_Rb_tree_impl<>",
  						// with stripped template argument list.
  container_type::iterator M_iter;
  bool M_is_class;				// Otherwise, it could be a namespace too, actually.
  container_type::iterator M_parent;		// Parent class, or namespace.
  std::map<Projects::iterator, int> M_projects;	// Related projects and count.
  bool M_is_functor;				// When used as functor at least once, between projects.

public:
  Class(std::string const& base_name);
  
  std::string const& base_name(void) const { return M_KEY_base_name; }
  container_type::iterator get_iter(void) const { return M_iter; }
  container_type::iterator parent_iter(void) const { return M_parent; }
  Class const& get_parent(void) const { return *M_parent; }
  bool is_class(void) const { return M_is_class; }
  void set_class(void) { M_is_class = true; }
  void set_parent(void);
  void set_functor(void) { M_is_functor = true; }
  bool is_functor(void) const { return M_is_functor; }
  std::map<Projects::iterator, int> const& get_projects(void) const { return M_projects; }
  std::map<Projects::iterator, int>& get_projects(void) { return M_projects; }
  void add_project(Project const& project);

  virtual std::string const& node_name(void) const { return M_KEY_base_name.empty() ? S_root_namespace : M_KEY_base_name; }
  virtual NodeType node_type(void) const { return class_or_namespace_node; }

  friend bool operator<(Class const& cl1, Class const& cl2)
  {
    return cl1.M_KEY_base_name < cl2.M_KEY_base_name;
  }

  static std::string const S_root_namespace;
};

extern bool operator<(Projects::iterator p1, Projects::iterator p2);
extern Classes classes;

#endif // CLASS_H
