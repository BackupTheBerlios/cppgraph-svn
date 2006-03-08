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

// The following operators are always unary, which means they are only
// member functions when they have no arguments.
// !	unary
// ~	unary
// --	unary	but might take 'int' as argument.
// ++	unary	but might take 'int' as argument.
// []	unary
// new		unary
// new[]	unary
// delete	unary
// delete[]	unary
// TYPE		unary

// *	unary or binary
// -	unary or binary
// +	unary or binary
// &	unary or binary
// ()	unary or binary or more, must always be a member function.

// All other operators are binary, which means they are only member functions
// when they have exactly one argument.
// ==	binary
// =	binary
// *=	binary
// /	binary
// /=	binary
// %	binary
// %=	binary
// ^	binary
// ^=	binary
// !=	binary
// ~=	binary
// -=	binary
// ->	binary
// ->*	binary
// +=	binary
// <	binary
// <=	binary
// <<	binary
// <<=	binary
// >	binary
// >=	binary
// >>	binary
// >>=	binary
// &=	binary
// &&	binary
// |	binary
// |=	binary
// ||	binary
// ,	binary
bool FunctionDecl::is_class_operator(void) const
{
  if (function_name.substr(0, 8) != "operator")
    return false;
  std::string operator_r = function_name.substr(8);
  // Detect if this is operator 'new', 'delete' or a casting operator.
  if (operator_r[0] == ' ')
  {
    if (operator_r == " new" || operator_r == " new []" ||
        operator_r == " delete" || operator_r == " delete []")
    {
      // This must be a member function, unless there is no parent scope.
      return !(class_or_namespaces.empty() && classes.empty());
    }
    // Must be a casting operator.
    assert(return_type.size() == 0);
    return true;
  }
  // Otherwise, check if this is an increment or decrement operator.
  if (operator_r == "++" || operator_r == "--")
  {
    // These are only member functions if the argument is void or int.
    return (parameter_types.size() == 1 && (parameter_types[0] == "int" || parameter_types[0] == "void"));
  }
  // Otherwise, check if this is a unary operator without arguments.
  if (parameter_types.size() == 0)
    return true;
  // operator() and operator[] are always a member functions.
  if (operator_r == "()" || operator_r == "[]")
    return true;
  // Anything else with more than one parameter is not a member function (but a binary operator).
  if (parameter_types.size() > 1)
    return false;
  char c = operator_r[0];
  if (operator_r.size() == 1)
  {
    // Some operators can be unary or binary, but those need at least one class or enumerate as parameter.
    if (c == '*' || c == '&' || c == '+' || c== '-')
    {
      // FIXME: return true if the parameter is a builtin-type.
      std::string param = parameter_types[0];
      return param == "int";
    }
    // These are unary operators.
    if (c == '!' || c == '~')
      return false;
  }
  // All other operators are binary operators with one parameter, and thus member functions.
  return true;
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
