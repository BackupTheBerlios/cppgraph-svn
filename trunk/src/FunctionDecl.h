// cppgraph -- C++ call graph analyzer
//
//! @file FunctionDecl.h
//! @brief This file contains the declaration of class FunctionDecl.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef FUNCTIONDECL_H
#define FUNCTIONDECL_H

#include <string>
#include <vector>
#include "ClassDecl.h"
#include <iostream>

struct FunctionDecl {
  std::string decl_specifier;
  std::string return_type;
  std::vector<std::string> class_or_namespaces;
  std::vector<ClassDecl> classes;
  std::string function_name;
  std::vector<std::string> parameter_types;
  std::string function_qualifiers;
  std::string exception_specification;

private:
  bool M_is_constructor;		// For example: N::Foo<T>::Foo(int, double)
  bool M_is_destructor;			// For example: virtual N::Foo<T>::~Foo()
  bool M_is_assignment;			// For example: N::Foo& N::Foo::operator=(const N::Foo&)
  bool M_is_copy;			// For example: N::Foo::Foo(const N::Foo&)
  bool M_is_default;			// For example: N::Foo::Foo()
  bool M_is_template;
  bool M_is_C_function;

public:
  FunctionDecl(void) : M_is_constructor(false), M_is_destructor(false), M_is_assignment(false),
      M_is_copy(false), M_is_default(false), M_is_template(false), M_is_C_function(false) { }

  void clear(void);
  void set_constructor(void) { M_is_constructor = true; }
  void set_destructor(void) { M_is_destructor = true; }
  void set_assignment(void) { M_is_assignment = true; }
  void unset_assignment(void) { M_is_assignment = false; }
  void set_copy(void) { M_is_copy = true; }
  void set_default(void) { M_is_default = true; }
  void set_is_template(void) { M_is_template = true; }
  void set_is_C_function(void) { M_is_C_function = true; }

  bool is_constructor(void) const { return M_is_constructor; }
  bool is_destructor(void) const { return M_is_destructor; }
  bool has_template_argument_list(void) const { return !classes.empty() && classes.back().has_template_argument_list(); }
  bool is_template(void) const
  {
    if (M_is_template)
      return true;
    for (std::vector<ClassDecl>::const_iterator iter = classes.begin(); iter != classes.end(); ++iter)
      if (iter->has_template_argument_list())
        return true;
    return false;
  }
  bool has_function_qualifiers(void) const { return !function_qualifiers.empty(); }
  bool is_copy(void) const { return M_is_copy; }
  bool is_assignment(void) const { return M_is_assignment; }
  bool is_default(void) const { return M_is_default; }
  bool is_C_function(void) const { return M_is_C_function; }

  std::string self_const_reference(void) const;
};

#endif // FUNCTIONDECL_H
