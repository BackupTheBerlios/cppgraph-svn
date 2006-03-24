// cppgraph -- C++ call graph analyzer
//
//! @file LocationData.h
//! @brief This file contains the declaration of class LocationData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef LOCATIONDATA_H
#define LOCATIONDATA_H

#include <string>
#include "ElementBase.h"
#include "serialization.h"

template<class Container, class FileName>
class LocationData : public ElementBase<Container> {
public:
  LocationData(FileName const& filename, int line_nr) :
      M_KEY_filename_ptr(filename.get_iter()), M_KEY_line_nr(line_nr) { }

  std::string const& filename(void) const { return M_KEY_filename_ptr->long_name(); }
  int line_nr(void) const { return M_KEY_line_nr; }

protected:
  typename FileName::container_type::const_iterator M_KEY_filename_ptr;
  int M_KEY_line_nr;

private:
  // Serialization.
  friend class boost::serialization::access;
  LocationData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & SERIALIZATION_ITERATOR_NVP(FileName::container, M_KEY_filename_ptr);
    ar & BOOST_SERIALIZATION_NVP(M_KEY_line_nr);
  }
};

#endif // LOCATIONDATA_H

