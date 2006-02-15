// cppgraph -- C++ call graph analyzer
//
//! @file Function.h
//! @brief This file contains the declaration of class Function.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include "Edge.h"
#include "FileName.h"
#include "Functions.h"
#include "FunctionDecl.h"
#include "Class.h"
#include "Project.h"
#include "CGDFile.h"

class Function : public Node {
public:
  typedef Functions container_type;
private:
  std::string M_KEY_function_name;
  FileNames::iterator M_KEY_decl_file;
  container_type::iterator M_iter;
  bool M_definition;
  CGDFiles::const_iterator M_cgd_file;
  std::vector<Edges::iterator> M_edges_out;
  std::vector<Edges::iterator> M_edges_in;
  FunctionDecl M_decl;
  Classes::iterator M_class_iter;
  Projects::iterator M_project_iter;
public:
  Function(std::string const& function_name, FileName const& filename, CGDFiles::const_iterator cgd_file);
  Function(std::string const& function_name, FileName const& filename);
  
  std::string const& name(void) const { return M_KEY_function_name; }
  container_type::iterator get_iter(void) const { return M_iter; }
  bool has_definition(void) const { return M_definition; }
  void set_definition(CGDFiles::const_iterator cgd_file) { M_definition = true; M_cgd_file = cgd_file; }
  bool is_possibly_compiler_generated(void) const
      { return M_decl.is_destructor() || M_decl.is_assignment() || M_decl.is_copy() || M_decl.is_default(); }
  CGDFiles::const_iterator cgd_file(void) const { return M_cgd_file; }
  void add_callee(Function const& callee);
  std::vector<Edges::iterator> const& edges_out(void) const { return M_edges_out; }
  std::vector<Edges::iterator>& edges_out(void) { return M_edges_out; }
  std::vector<Edges::iterator> const& edges_in(void) const { return M_edges_in; }
  std::vector<Edges::iterator>& edges_in(void) { return M_edges_in; }
  FunctionDecl& decl(void) { return M_decl; }
  FunctionDecl const& decl(void) const { return M_decl; }
  void set_class(Classes::iterator iter) { M_class_iter = iter; }
  Class const& get_class(void) const { return *M_class_iter; }
  FileName const& get_file(void) const { return *M_KEY_decl_file; }
  void set_project(Projects::iterator iter) { M_project_iter = iter; }
  Project const& get_project(void) const { return *M_project_iter; }

  virtual std::string const& node_name(void) const { return name(); }
  virtual NodeType node_type(void) const { return function_node; }

  friend bool operator<(Function const& fn1, Function const& fn2)
  {
    return fn1.M_KEY_function_name < fn2.M_KEY_function_name ||
        (fn1.M_KEY_function_name == fn2.M_KEY_function_name &&
	 *fn1.M_KEY_decl_file < *fn2.M_KEY_decl_file);
  }
};

extern Functions functions;

#endif // FUNCTION_H
