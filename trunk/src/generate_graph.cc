// cppgraph -- C++ call graph analyzer
//
//! @file generate_graph.cc
//! @brief This file contains function generate_graph.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include "Project.h"
#include "Function.h"
#include "Graph.h"

void generate_graph(int verbose)
{
  if (verbose)
    std::cout << "Generating graph\n";
  Graph g;
  for (Edges::iterator edge_iter = edges.begin(); edge_iter != edges.end(); ++edge_iter)
  {
    Function& caller(const_cast<Function&>(edge_iter->get_caller()));
    if (!caller.has_definition() || caller.get_project().get_iter() != caller.cgd_file()->source_file().get_project().get_iter())
      continue;
    Function& callee(const_cast<Function&>(edge_iter->get_callee()));
    if (!callee.has_definition() || callee.get_project().get_iter() != callee.cgd_file()->source_file().get_project().get_iter())
      continue;
    if (caller.decl().is_template() ||
        caller.decl().function_name.substr(0, 6) == "__tcf_" ||
	callee.decl().function_name.substr(0, 6) == "__cxa_")
      continue;
    try
    {
      g.add_edge(caller.get_node(class_or_namespace_node), callee.get_node(class_or_namespace_node));
    }
    catch (std::runtime_error& error)
    {
      std::cerr << caller << " CALLS " << callee << std::endl;
    }
  }
  if (verbose)
    std::cout << "Number of vertices: " << g.number_of_vertices() << std::endl;

  std::ofstream dot_file;
  dot_file.open("/usr/src/cppgraph/cppgraph-objdir/src/test.dot");
  g.write_to(dot_file);
  dot_file.close();
}
