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
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/exception.hpp>

void Graph::add_vertex(Node* node)
{
  if (M_vertex_set.insert(node).second)
  {
    node->set_index(M_vertex_vec.size());
    M_vertex_vec.push_back(node);
    M_distance_vec.push_back(-1);
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
  os << "  rankdir = BT;\n";
  for (std::vector<Node*>::iterator vertex_iter = M_vertex_vec.begin(); vertex_iter != M_vertex_vec.end(); ++vertex_iter)
  {
    os << "  " << (*vertex_iter)->get_index() <<
        " [label=\"" << (*vertex_iter)->node_name() << "\"];\n";
  }
  for (std::set<std::pair<size_t, size_t> >::iterator edge_iter = M_edges.begin(); edge_iter != M_edges.end(); ++edge_iter)
  {
    os << "  " << edge_iter->first << " -> " << edge_iter->second << ";\n";
  }
  os << "}\n";
}

struct test_dag_visitor : public boost::dfs_visitor<> {
private:
  Graph& M_graph;
public:
  test_dag_visitor(Graph& graph) : M_graph(graph) { }
  template <typename Edge, typename Graph>
    void back_edge(Edge const&, Graph&)
    {
      throw boost::not_a_dag();
    }
  template <typename Vertex, typename Graph>
    void discover_vertex(Vertex const& u, Graph const&)
    {
      int& d(M_graph.distance_ref(u));
      if (d == -1)
        d = 0;
    }
  template <typename Edge, typename Graph>
    void examine_edge(Edge const& e, Graph const&)
    {
      int& d1(M_graph.distance_ref(e.m_source));
      int& d2(M_graph.distance_ref(e.m_target));
      if (d2 < d1 + 1)
        d2 = d1 + 1;
    }
};

bool Graph::test_acyclic(void)
{
  CGDFile::container_type::iterator some_cgd_file_iter = CGDFile::container.begin();
  FileName const& filename(some_cgd_file_iter->source_file());
  Project const& main_project(filename.get_project());

  bool result = true;
  try
  {
    boost::depth_first_search(M_g,
	boost::visitor(test_dag_visitor(*this)).
	root_vertex(main_project.get_index()));
  }
  catch (boost::not_a_dag const&)
  {
    result = false;
  }
  return result;
}

bool Graph::is_connected(size_t from, size_t to)
{
  assert(M_distance_vec[from] != -1);
  return M_distance_vec[from] < M_distance_vec[to];
}

