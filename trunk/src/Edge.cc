// cppgraph -- C++ call graph analyzer
//
//! @file Edge.cc
//! @brief This file contains the implementation of class Edge.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Edge.h"
#include "Function.h"
#include "Edge.inl"

Edges edges;

Edge::Edge(Function const& caller, Function const& callee) :
    M_KEY_caller(caller.get_iter()), M_KEY_callee(callee.get_iter())
{
  M_iter = edges.insert(*this).first;
  const_cast<Edge&>(*M_iter).M_iter = M_iter;
}
