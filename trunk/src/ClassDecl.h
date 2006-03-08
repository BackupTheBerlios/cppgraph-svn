// cppgraph -- C++ call graph analyzer
//
//! @file ClassDecl.h
//! @brief This file contains the declaration of class ClassDecl.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef CLASSDECL_H
#define CLASSDECL_H

#include <string>
#include <cassert>

class ClassDecl {
private:
  std::string M_name;
  std::string M_template_argument_list_opt;
  bool M_is_function;
  int M_action_count;	// Used by parser.
public:
  ClassDecl(void) : M_is_function(false), M_action_count(0) { }
  ClassDecl(std::string const& name) : M_name(name), M_is_function(false), M_action_count(0) { }
  ClassDecl(std::string const& name, std::string const& template_argument_list) :
      M_name(name), M_template_argument_list_opt(template_argument_list), M_is_function(false), M_action_count(0) { }

  void set_template_argument_list(std::string const& template_argument_list)
      { assert(!has_template_argument_list()); M_template_argument_list_opt = template_argument_list; }
  void set_name(std::string const& name)
      { M_name = name; M_template_argument_list_opt.clear(); }
  void set_is_function(void) { M_is_function = true; }

  std::string const& name(void) const { return M_name; }  
  std::string const& template_argument_list(void) const { return M_template_argument_list_opt; }
  bool has_template_argument_list(void) const { return !M_template_argument_list_opt.empty(); }
  bool is_function(void) const { return M_is_function; }

  bool action_begin(void) { return 1 == ++M_action_count; }
  bool action_end(void) { return 0 == --M_action_count; }
};

#endif // CLASSDECL_H
