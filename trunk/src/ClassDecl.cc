// cppgraph -- C++ call graph analyzer
//
//! @file ClassDecl.cc
//! @brief This file contains the implementation of class ClassDecl.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "ClassDecl.h"
#include "debug.h"

#ifdef CWDEBUG
std::ostream& operator<<(std::ostream& os, ClassDecl const& class_decl)
{
  os << class_decl.name() << class_decl.template_argument_list();
  return os;
}
#endif
