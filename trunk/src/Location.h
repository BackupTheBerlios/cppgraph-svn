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

#include "FileName.h"

class Location {
public:
  typedef std::set<Location> container_type;
private:
  FileNames::const_iterator M_KEY_filename_ptr;
  int M_KEY_line_nr;
  container_type::iterator M_iter;
public:
  Location(FileName const& filename, int line_nr);
  
  std::string const& filename(void) const { return M_KEY_filename_ptr->long_name(); }
  int line_nr(void) const { return M_KEY_line_nr; }
  container_type::iterator get_iter(void) const { return M_iter; }
  friend bool operator<(Location const& loc1, Location const& loc2)
  {
    return (loc1.M_KEY_line_nr < loc2.M_KEY_line_nr ||
        (loc1.M_KEY_line_nr == loc2.M_KEY_line_nr && *loc1.M_KEY_filename_ptr < *loc2.M_KEY_filename_ptr));
  }
};

typedef Location::container_type Locations;

extern Locations locations;

#endif // LOCATION_H
