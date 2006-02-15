// cppgraph -- C++ call graph analyzer
//
//! @file parser.h
//! @brief This file contains the declaration of function parse_function_declaration.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef PARSER_H
#define PARSER_H

#include <set>
#include "FunctionDecl.h"

// Returns true when 'input' could be successfully parsed. The result is written to 'decl'.
bool parse_function_declaration(std::string const& input, FunctionDecl& decl, std::set<std::string>& types);

#endif // PARSER_H
