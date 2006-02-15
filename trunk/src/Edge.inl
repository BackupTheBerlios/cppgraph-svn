// cppgraph -- C++ call graph analyzer
//
//! @file Edge.inl
//! @brief This file contains the inline implementation related to class Edge.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef EDGE_INL
#define EDGE_INL

#include "Function.h"

inline bool operator<(Edge const& edge1, Edge const& edge2)
{
  return *edge1.M_KEY_caller < *edge2.M_KEY_caller ||
      (!(*edge2.M_KEY_caller < *edge1.M_KEY_caller) && *edge1.M_KEY_callee < *edge2.M_KEY_callee);
}

#endif // EDGE_INL
