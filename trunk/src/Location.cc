// cppgraph -- C++ call graph analyzer
//
//! @file Location.cc
//! @brief This file contains the implementation of class Location.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Location.h"

Locations locations;

Location::Location(FileName const& filename, int line_nr) :
    M_KEY_filename_ptr(filename.get_iter()), M_KEY_line_nr(line_nr)
{
  M_iter = locations.insert(*this).first;
  const_cast<Location&>(*M_iter).M_iter = M_iter;
}
