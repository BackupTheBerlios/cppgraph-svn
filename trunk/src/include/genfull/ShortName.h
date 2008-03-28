// cppgraph -- C++ call graph analyzer
//
//! @file ShortName.h
//! @brief This file contains the declaration of class ShortName.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef SHORTNAME_H
#define SHORTNAME_H

#include <set>
#include <list>
#include <string>
#include "ShortNameData.h"

template<class Container>
class ShortName : public ShortNameData<std::set<ShortName<Container> >, Container> {
public:
  ShortName(std::string const& short_name, typename Container::iterator long_name);

  void solve_collisions(typename std::set<ShortName<Container> >::iterator& iter);
};

#endif // SHORTNAME_H
