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
#include <string>
#include <vector>

template<typename Derived>
  class ShortName {
  public:
    typedef std::set<ShortName<Derived> > container_type;

  private:
    std::string M_KEY_short_name;
    std::vector<typename Derived::container_type::iterator> M_long_name_iters;
    typename container_type::iterator M_iter;

  public:
    ShortName(std::string const& short_name, typename Derived::container_type::iterator long_name);

    std::string const& str(void) const { return M_KEY_short_name; }
    typename container_type::iterator get_iter(void) const { return M_iter; }

    void solve_collisions(typename container_type::iterator& iter);

    static container_type short_names;

    friend bool operator<(ShortName const& sn1, ShortName const& sn2)
	{ return sn1.M_KEY_short_name < sn2.M_KEY_short_name; }
  };

template<typename Derived>
  std::set<ShortName<Derived> > ShortName<Derived>::short_names;

#endif // SHORTNAME_H
