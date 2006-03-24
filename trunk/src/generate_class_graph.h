// cppgraph -- C++ call graph analyzer
//
//! @file generate_class_graph.h
//! @brief This file contains the declaration of function generate_class_graph.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef GENERATE_CLASS_GRAPH_H
#define GENERATE_CLASS_GRAPH_H

#include "Graph.h"

void generate_class_graph(int verbose);

extern Graph class_graph;

#endif // GENERATE_CLASS_GRAPH_H
