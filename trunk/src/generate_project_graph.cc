// cppgraph -- C++ call graph analyzer
//
//! @file generate_project_graph.cc
//! @brief This file implements generation of the project graph.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <iostream>
#include <fstream>
#include "Graph.h"
#include "generate_project_graph.h"

struct ProjectCallerFilter {
  static NodeType const type = project_node;
  bool operator()(Function const& caller)
  {
    return
        caller.decl().is_template() ||
        caller.decl().function_name.substr(0, 6) == "__tcf_";
  }
};

struct ProjectCalleeFilter {
  static NodeType const type = project_node;
  bool operator()(Function const& callee)
  {
    return callee.decl().function_name.substr(0, 6) == "__cxa_";
  }
};

void generate_project_graph(int verbose)
{
  if (verbose)
    std::cout << "Generating project graph" << std::endl;
  project_graph.generate_graph(ProjectCallerFilter(), ProjectCalleeFilter());
  if (verbose)
    std::cout << "Number of vertices: " << project_graph.number_of_vertices() << std::endl;
//  if (!project_graph.test_acyclic())
//    DoutFatal(dc::fatal, "Project graph is not a-cyclic!");
  std::ofstream dot_file;
  dot_file.open("projects.dot");
  project_graph.write_to(dot_file);
  dot_file.close();
  if (verbose)
    std::cout << "Wrote \"projects.dot\"." << std::endl;
}

Graph project_graph;
