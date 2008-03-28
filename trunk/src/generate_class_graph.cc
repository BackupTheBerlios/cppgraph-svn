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
  static NodeType const type = function_node;
  bool operator()(Function const& caller)
  {
    Dout(dc::notice, "CALLER: " << caller);
    return caller.get_project().short_name() != "src" ||
           !(caller.decl().function_name == "init_dir_inode_to_block_cache" ||
             caller.decl().function_name == "init_directories" ||
             caller.decl().function_name == "init_files" ||
             caller.decl().function_name == "dir_inode_to_block" ||
             caller.decl().function_name == "print_directory_inode" ||
             caller.decl().function_name == "dump_names" ||
             caller.decl().function_name == "show_hardlinks" ||
             caller.decl().function_name == "restore_file" ||
             caller.decl().function_name == "filter_dir_entry" ||
             caller.decl().function_name == "init_directories_action" ||
             caller.decl().function_name == "extended_directory_action" ||
             caller.decl().function_name == "init_directories" ||
             caller.decl().function_name == "filter_dir_entry" ||
             caller.decl().function_name == "iterate_over_directory" ||
             caller.decl().function_name == "init_directories_action" ||
             caller.decl().function_name == "link_extended_directory_block_to_inode" ||
             caller.decl().function_name == "extended_directory_action");
  }
};

struct ClassCalleeFilter {
  static NodeType const type = function_node;
  bool operator()(Function const& callee)
  {
    Dout(dc::notice, "CALLEE: " << callee << "(class base_name \"" << callee.get_class().base_name() << ")");
    return callee.get_project().short_name() != "src" ||
           !(callee.decl().function_name == "init_dir_inode_to_block_cache" ||
             callee.decl().function_name == "init_directories" ||
             callee.decl().function_name == "init_files" ||
             callee.decl().function_name == "dir_inode_to_block" ||
             callee.decl().function_name == "print_directory_inode" ||
             callee.decl().function_name == "dump_names" ||
             callee.decl().function_name == "show_hardlinks" ||
             callee.decl().function_name == "restore_file" ||
             callee.decl().function_name == "filter_dir_entry" ||
             callee.decl().function_name == "init_directories_action" ||
             callee.decl().function_name == "extended_directory_action" ||
             callee.decl().function_name == "init_directories" ||
             callee.decl().function_name == "filter_dir_entry" ||
             callee.decl().function_name == "iterate_over_directory" ||
             callee.decl().function_name == "init_directories_action" ||
             callee.decl().function_name == "link_extended_directory_block_to_inode" ||
             callee.decl().function_name == "extended_directory_action");
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
