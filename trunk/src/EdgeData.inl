// cppgraph -- C++ call graph analyzer
//
//! @file EdgeData.inl
//! @brief This file contains the inline implementation related to class EdgeData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef EDGEDATA_INL
#define EDGEDATA_INL

#include "EdgeData.h"
#include "Function.h"

template<class Container, class Functions, class Function>
EdgeData<Container, Functions, Function>::EdgeData(Function const& caller, Function const& callee) :
    M_KEY_caller(caller.get_iter()), M_KEY_callee(callee.get_iter()) { }

template<class Container, class Functions, class Function>
inline bool operator<(EdgeData<Container, Functions, Function> const& edge1,
                      EdgeData<Container, Functions, Function> const& edge2)
{
  return *edge1.M_KEY_caller < *edge2.M_KEY_caller ||
      (!(*edge2.M_KEY_caller < *edge1.M_KEY_caller) && *edge1.M_KEY_callee < *edge2.M_KEY_callee);
}

#endif // EDGEDATA_INL
