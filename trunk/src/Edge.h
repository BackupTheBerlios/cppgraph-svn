// cppgraph -- C++ call graph analyzer
//
//! @file Edge.h
//! @brief This file contains the declaration of class Edge.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef EDGE_H
#define EDGE_H

#include <set>
#include "Functions.h"

class Edge {
public:
  typedef std::set<Edge> container_type;
private:
  Functions::iterator M_KEY_caller;  
  Functions::iterator M_KEY_callee;
  container_type::iterator M_iter;
public:
  Edge(Function const& caller, Function const& callee);

  Function const& get_caller(void) const { return *M_KEY_caller; }
  Function const& get_callee(void) const { return *M_KEY_callee; }
  Functions::iterator get_caller_iter(void) const { return M_KEY_caller; }
  Functions::iterator get_callee_iter(void) const { return M_KEY_callee; }
  container_type::iterator get_iter(void) const { return M_iter; }
  friend bool operator<(Edge const& edge1, Edge const& edge2);
};

typedef Edge::container_type Edges;

extern Edges edges;

#endif // EDGE_H
