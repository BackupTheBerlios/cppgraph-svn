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
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "Edge.h"
#include "Function.h"

class Graph {
public:
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> graph_type;
private:
  std::vector<Node*> M_vertex_vec;
  std::set<Node*> M_vertex_set;
  std::set<std::pair<size_t, size_t> > M_edges;
  graph_type M_g;
  std::vector<int> M_distance_vec;
public:
  size_t number_of_vertices(void) const { return M_vertex_vec.size(); }
  void add_vertex(Node* node);
  void add_edge(Node* u, Node* v);
  void write_to(std::ostream& os);
  template<typename CallerFilter, typename CalleeFilter>
    void generate_graph(CallerFilter, CalleeFilter);
  bool test_acyclic(void);
  bool is_connected(size_t from, size_t to);
  int& distance_ref(size_t index) { return M_distance_vec[index]; }
};

template<typename CallerFilter, typename CalleeFilter>
void Graph::generate_graph(CallerFilter caller_filter, CalleeFilter callee_filter)
{
  for (Edge::container_type::iterator edge_iter = Edge::container.begin(); edge_iter != Edge::container.end(); ++edge_iter)
  {
    Function& caller(const_cast<Function&>(edge_iter->get_caller()));
    if (caller_filter(caller))
      continue;
    Function& callee(const_cast<Function&>(edge_iter->get_callee()));
    if (callee_filter(callee))
      continue;
    Dout(dc::notice, "Adding " << caller << " calls " << callee);
    add_edge(caller.get_node(caller_filter.type), callee.get_node(callee_filter.type));
  }

  // Add the vertices to the boost graph object.
  for (size_t i = 0; i < number_of_vertices(); ++i)
    boost::add_vertex(M_g);
  // Add the edges to the boost graph object.
  for (std::set<std::pair<size_t, size_t> >::iterator iter = M_edges.begin(); iter != M_edges.end(); ++iter)
    boost::add_edge(iter->first, iter->second, M_g);
}

#endif // GRAPH_H
