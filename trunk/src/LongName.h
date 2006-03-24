// cppgraph -- C++ call graph analyzer
//
//! @file LongName.h
//! @brief This file contains the declaration of class LongName.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef LONGNAME_H
#define LONGNAME_H

#include <string>
#include "ShortName.h"
#include "debug.h"
#include "serialization.h"

template<class Container>
class LongName {
public:
  LongName(std::string const& long_name) : M_KEY_long_name(long_name) { }

  // Accessors.
  std::string const& long_name(void) const { return M_KEY_long_name; }
  std::string const& short_name(void) const { return M_shortname->str(); }

  template<typename Iterator>
    static void init_short_name(Iterator longname_iter);

  void set_shortname(typename ShortName<Container>::container_type::iterator shortname_iter)
      { M_shortname = shortname_iter; }

  static void generate_short_names(void);

  friend bool operator<(LongName const& ln1, LongName const& ln2)
      { return ln1.M_KEY_long_name < ln2.M_KEY_long_name; }

protected:
  std::string M_KEY_long_name;		// The full name.
  typename ShortName<Container>::container_type::iterator M_shortname;

private:
  // Serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_NVP(M_KEY_long_name);
    typedef ShortName<Container> shortname_type;
    ar & SERIALIZATION_ITERATOR_NVP(shortname_type::container, M_shortname);
  }
};

#include "ShortName.tcc"

template<class Container>
  template<typename Iterator>
    void LongName<Container>::init_short_name(Iterator longname_iter)
    {
      std::string short_name = longname_iter->long_name();
      std::string::size_type pos = longname_iter->long_name().rfind(Container::value_type::seperator());
      if (pos != std::string::npos && longname_iter->process())
	short_name = longname_iter->long_name().substr(pos + 1);
      ShortName<Container>(short_name, longname_iter);
    }

template<class Container>
void LongName<Container>::generate_short_names(void)
{
  for (typename std::set<ShortName<Container> >::iterator iter =
      ShortName<Container>::container.begin();
       iter != ShortName<Container>::container.end();)
    const_cast<ShortName<Container>&>(*iter).solve_collisions(iter);
}

#endif // LONGNAME_H
