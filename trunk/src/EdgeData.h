// cppgraph -- C++ call graph analyzer
//
//! @file EdgeData.h
//! @brief This file contains the declaration of class EdgeData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef EDGEDATA_H
#define EDGEDATA_H

#include "ElementBase.h"
#include "serialization.h"

template<class Container, class Functions, class Function>
class EdgeData;

template<class Container, class Functions, class Function>
bool operator<(EdgeData<Container, Functions, Function> const& edge1,
               EdgeData<Container, Functions, Function> const& edge2);

template<class Container, class Functions, class Function>
class EdgeData : public ElementBase<Container> {
public:
  EdgeData(Function const& caller, Function const& callee);

  Function const& get_caller(void) const { return *M_KEY_caller; }
  Function const& get_callee(void) const { return *M_KEY_callee; }
  typename Functions::iterator get_caller_iter(void) const { return M_KEY_caller; }
  typename Functions::iterator get_callee_iter(void) const { return M_KEY_callee; }
  friend bool operator< <>(EdgeData const& edge1, EdgeData const& edge2);

private:
  typename Functions::iterator M_KEY_caller;  
  typename Functions::iterator M_KEY_callee;

private:
  // Serialization.
  friend class boost::serialization::access;
  EdgeData() { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & SERIALIZATION_ITERATOR_NVP(Function::container, M_KEY_caller);
    ar & SERIALIZATION_ITERATOR_NVP(Function::container, M_KEY_callee);
  }
};

#endif // EDGEDATA_H

