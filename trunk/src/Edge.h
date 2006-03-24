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
#include <vector>
#include "EdgeData.h"
#include "Edges.h"
#include "Functions.h"
#include "serialization.h"

class Edge : public EdgeData<Edges, Functions, Function> {
public:
  Edge(Function const& caller, Function const& callee) :
      EdgeData<Edges, Functions, Function>(caller, callee) { add(container, *this); }
};

#endif // EDGE_H
