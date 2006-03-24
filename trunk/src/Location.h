// cppgraph -- C++ call graph analyzer
//
//! @file Location.h
//! @brief This file contains the declaration of class Location.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef LOCATION_H
#define LOCATION_H

#include <set>
#include "LocationData.h"
#include "FileName.h"

class Location : public LocationData<std::set<Location>, FileName> {
public:
  Location(FileName const& filename, int line_nr) :
      LocationData<std::set<Location>, FileName>(filename, line_nr) { add(container, *this); }

  friend bool operator<(Location const& loc1, Location const& loc2)
  {
    return (loc1.M_KEY_line_nr < loc2.M_KEY_line_nr ||
        (loc1.M_KEY_line_nr == loc2.M_KEY_line_nr && *loc1.M_KEY_filename_ptr < *loc2.M_KEY_filename_ptr));
  }
};

#endif // LOCATION_H
