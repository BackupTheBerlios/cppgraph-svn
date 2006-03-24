// cppgraph -- C++ call graph analyzer
//
//! @file Function.cc
//! @brief This file contains the implementation of class Function.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Function.h"
#include "Edge.h"
#include "EdgeData.inl"		// For instantiation of Edge objects.
#include "debug.h"

Function::Function(std::string const& function_name, FileName const& filename,
    CGDFile::container_type::const_iterator cgd_file) :
    FunctionData<Functions, CGDFile, FileName, Project, Classes, FunctionDecl, Edges>(function_name, filename, cgd_file)
{
  add(container, *this);
  const_cast<Function&>(*M_iter).set_definition(cgd_file);
}

void Function::add_callee(Function const& callee)
{
  Edge edge(*this, callee);
  this->edges_out().push_back(edge.get_iter());
  const_cast<Function&>(*callee.get_iter()).edges_in().push_back(edge.get_iter());
}

#ifdef CWDEBUG
std::ostream& operator<<(std::ostream& os, Function const& function)
{
  os << function.decl() << " [" << function.get_project() << "]";
  return os;
}
#endif
