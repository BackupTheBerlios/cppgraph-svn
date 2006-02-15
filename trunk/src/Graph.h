// cppgraph -- C++ call graph analyzer
//
//! @file Graph.h
//! @brief This file contains the declaration of class Graph.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include <vector>
#include <set>
#include <iosfwd>

class Graph {
private:
  std::vector<Node*> M_vertex_vec;
  std::set<Node*> M_vertex_set;
  std::set<std::pair<size_t, size_t> > M_edges;
public:
  size_t number_of_vertices(void) const { return M_vertex_vec.size(); }
  void add_vertex(Node* node);
  void add_edge(Node* u, Node* v);
  void write_to(std::ostream& os);
};

#endif // GRAPH_H
