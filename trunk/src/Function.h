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
#include "Functions.h"
#include "CGDFile.h"
#include "FileName.h"
#include "Project.h"
#include "Class.h"
#include "FunctionDecl.h"
#include "Edges.h"
#include "FunctionData.h"

class Function : public FunctionData<Functions, CGDFile, FileName, Project, Classes, FunctionDecl, Edges> {
public:
  Function(std::string const& function_name, FileName const& filename, CGDFile::container_type::const_iterator cgd_file);
  Function(std::string const& function_name, FileName const& filename) :
      FunctionData<Functions, CGDFile, FileName, Project, Classes, FunctionDecl, Edges>(function_name, filename)
      { add(container, *this); }

  void set_project(Project::container_type::iterator iter) { M_project_iter = iter; }
  void set_class(Classes::iterator iter) { M_class_iter = iter; }
  void add_callee(Function const& callee);
  void set_definition(CGDFile::container_type::const_iterator cgd_file) { M_definition = true; M_cgd_file = cgd_file; }
};

#endif // FUNCTION_H
