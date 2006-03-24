// cppgraph -- C++ call graph analyzer
//
//! @file generate_class_graph.cc
//! @brief This file implements generation of the class graph.
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
#include "generate_class_graph.h"

struct ClassCallerFilter {
  static NodeType const type = class_or_namespace_node;
  bool operator()(Function const& caller)
  {
    return
        caller.get_project().short_name() != "src" ||
        caller.decl().function_name.substr(0, 6) == "__tcf_";
  }
};

struct ClassCalleeFilter {
  static NodeType const type = class_or_namespace_node;
  bool operator()(Function const& callee)
  {
    return
        callee.get_project().short_name() != "src" ||
        callee.decl().function_name.substr(0, 6) == "__cxa_";
  }
};

void generate_class_graph(int verbose)
{
  if (verbose)
    std::cout << "Generating class graph" << std::endl;
  class_graph.generate_graph(ClassCallerFilter(), ClassCalleeFilter());
  if (verbose)
    std::cout << "Number of vertices: " << class_graph.number_of_vertices() << std::endl;
  std::ofstream dot_file;
  dot_file.open("class.dot");
  class_graph.write_to(dot_file);
  dot_file.close();
  if (verbose)
    std::cout << "Wrote \"class.dot\"." << std::endl;
}

Graph class_graph;
