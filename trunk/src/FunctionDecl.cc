// cppgraph -- C++ call graph analyzer
//
//! @file FunctionDecl.cc
//! @brief This file contains the implementation of class FunctionDecl.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "FunctionDecl.h"
#include <iostream>
#include "debug.h"

void FunctionDecl::clear(void)
{
  return_type.clear();
  class_or_namespaces.clear();
  classes.clear();
  function_name.clear();
  parameter_types.clear();
  function_qualifiers.clear();
  exception_specification.clear();
}

std::string FunctionDecl::self_const_reference(void) const
{
  std::string result = "const ";
  bool first = true;
  for (std::vector<std::string>::const_iterator iter = class_or_namespaces.begin(); iter != class_or_namespaces.end(); ++iter)
  {
    if (first)
      first = false;
    else
      result += "::";
    result += *iter;
  }
  for (std::vector<ClassDecl>::const_iterator iter = classes.begin(); iter != classes.end(); ++iter)
  {
    if (first)
      first = false;
    else
      result += "::";
    result += iter->name();
    result += iter->template_argument_list();
  }
  result += '&';
  return result;
}

#ifdef CWDEBUG
std::ostream& operator<<(std::ostream& os, FunctionDecl const& decl)
{
  if (!decl.return_type.empty())
    os << decl.return_type << ' ';
  bool first = true;
  for (std::vector<std::string>::const_iterator iter = decl.class_or_namespaces.begin();
       iter != decl.class_or_namespaces.end(); ++iter)
  {
    if (!first)
      os << "::";
    else
      first = false;
    os << *iter;
  }
  for (std::vector<ClassDecl>::const_iterator iter = decl.classes.begin(); iter != decl.classes.end(); ++iter)
  {
    if (!first)
      os << "::";
    else
      first = false;
    os << *iter;
  }
  if (!first)
    os << "::";
  os << decl.function_name << '(';
  first = true;
  for (std::vector<std::string>::const_iterator iter = decl.parameter_types.begin(); iter != decl.parameter_types.end(); ++iter)
  {
    if (!first)
      os << ", ";
    else
      first = false;
    os << *iter;
  }
  if (first && !decl.is_C_function())
    os << "void";
  os << ')';
  if (decl.has_function_qualifiers())
    os << ' ' << decl.function_qualifiers;
  return os;
}
#endif
