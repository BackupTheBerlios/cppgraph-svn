// cppgraph -- C++ call graph analyzer
//
//! @file CGDFileData.h
//! @brief This file contains the declaration of class CGDFileData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef CGDFILEDATA_H
#define CGDFILEDATA_H

#include <string>
#include "ElementBase.h"
#include "LongName.h"
#include "serialization.h"

template<class Container, class FileName>
class CGDFileData : public ElementBase<Container>, public LongName<Container> {
public:
  CGDFileData(std::string const& filename) :
      LongName<Container>(filename), M_source_file(FileName::container.end()) { }

public:
  // Used by LongName
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return true; }

public:
  bool has_source_file(void) const { return M_source_file != FileName::container.end(); }
  FileName const& source_file(void) const { return *M_source_file; }

protected:
  typename FileName::container_type::iterator M_source_file;

private:
  // Serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(LongName<Container>);
    ar & SERIALIZATION_ITERATOR_NVP(FileName::container, M_source_file);
  }
};

#endif // CGDFILEDATA_H

