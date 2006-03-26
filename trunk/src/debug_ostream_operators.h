// cppgraph -- C++ call graph analyzer
//
//! @file debug_ostream_operators.h
//! @brief This file contains the declaration of debug ostream inserters.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DEBUG_OSTREAM_OPERATORS_H
#define DEBUG_OSTREAM_OPERATORS_H

#include <iosfwd>
class Function;
class FunctionDecl;
class Project;
class ClassDecl;

std::ostream& operator<<(std::ostream& os, FunctionDecl const& decl);
std::ostream& operator<<(std::ostream& os, Project const& project);
std::ostream& operator<<(std::ostream& os, ClassDecl const& class_decl);

#endif // DEBUG_OSTREAM_OPERATORS_H
