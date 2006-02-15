// cppgraph -- C++ call graph analyzer
//
//! @file Graph.cc
//! @brief This file contains the implementation of class Graph.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <iostream>
#include <stdexcept>
#include "Graph.h"

void Graph::add_vertex(Node* node)
{
  if (M_vertex_set.insert(node).second)
  {
    node->set_index(M_vertex_vec.size());
    M_vertex_vec.push_back(node);
  }
}

void Graph::add_edge(Node* u, Node* v)
{
  add_vertex(u);
  add_vertex(v);
  size_t ui = u->get_index();
  size_t vi = v->get_index();
  if (ui != vi)
  {
    if (M_vertex_vec[ui]->node_name() == "libcw" &&
        M_vertex_vec[vi]->node_name() == "src")
      throw std::runtime_error("weird relationship");
    M_edges.insert(std::pair<size_t, size_t>(ui, vi));
  }
}

void Graph::write_to(std::ostream& os)
{
  os << "digraph G {\n";
  os << "  size = \"12,12\";\n";
  os << "  rankdir = BT;\n";
  for (std::vector<Node*>::iterator vertex_iter = M_vertex_vec.begin(); vertex_iter != M_vertex_vec.end(); ++vertex_iter)
  {
    os << "  \"" << (*vertex_iter)->node_name() << "\"\n";
  }
  for (std::set<std::pair<size_t, size_t> >::iterator edge_iter = M_edges.begin(); edge_iter != M_edges.end(); ++edge_iter)
  {
    os << "  \"" << M_vertex_vec[edge_iter->first]->node_name() << "\" -> \"" <<
        M_vertex_vec[edge_iter->second]->node_name() << "\";\n";
  }
  os << "}\n";
}

