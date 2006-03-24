// cppgraph -- C++ call graph analyzer
//
//! @file ShortNameData.h
//! @brief This file contains the declaration of class ShortNameData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef SHORTNAMEDATA_H
#define SHORTNAMEDATA_H

#include <string>
#include "ElementBase.h"
#include "serialization.h"

template<class Container, class DerivedContainer>
class ShortNameData;

template<class Container, class DerivedContainer>
bool operator<(ShortNameData<Container, DerivedContainer> const& sn1,
               ShortNameData<Container, DerivedContainer> const& sn2);

template<class Container, class DerivedContainer>
class ShortNameData : public ElementBase<Container> {
public:
  ShortNameData(std::string const& short_name) : M_KEY_short_name(short_name) { }

  std::string const& str(void) const { return M_KEY_short_name; }

  friend bool operator<(ShortNameData const& sn1, ShortNameData const& sn2)
      { return sn1.M_KEY_short_name < sn2.M_KEY_short_name; }

protected:
  std::string M_KEY_short_name;
  std::vector<typename DerivedContainer::iterator> M_long_name_iters;

private:
  // Serialization.
  friend class boost::serialization::access;
  ShortNameData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_NVP(M_KEY_short_name);
    ar & SERIALIZATION_ITERATOR_NVP(DerivedContainer::value_type::container, M_long_name_iters);
  }
};

#endif // SHORTNAMEDATA_H

