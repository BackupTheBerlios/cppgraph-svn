// cppgraph -- C++ call graph analyzer
//
//! @file ShortName.tcc
//! @brief This file contains the implementation of class ShortName.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "debug.h"
#include "LongName.h"

template<typename Derived>
ShortName<Derived>::ShortName(std::string const& short_name, typename Derived::container_type::iterator long_name) :
    M_KEY_short_name(short_name)
{
  Dout(dc::shortname, "Creating ShortName<" << type_info_of<Derived>().demangled_name() << ">::ShortName(" <<
      "\"" << short_name << "\", \"" << long_name->long_name() << "\").");
  M_iter = short_names.insert(*this).first;
  const_cast<ShortName<Derived>&>(*M_iter).M_iter = M_iter;
  const_cast<ShortName<Derived>&>(*M_iter).M_long_name_iters.push_back(long_name);
  Derived& derived = const_cast<Derived&>(*long_name);
  static_cast<LongName<Derived>&>(derived).set_shortname(M_iter);
};

template<typename Derived>
void ShortName<Derived>::solve_collisions(typename container_type::iterator& old_iter)
{
  if (M_long_name_iters.size() == 1)
  {
    ++old_iter;
    return;
  }
  typename container_type::iterator new_iter;
  size_t current_len = M_KEY_short_name.size();
  for (typename std::vector<typename Derived::container_type::iterator>::iterator iter = M_long_name_iters.begin();
       iter != M_long_name_iters.end(); ++iter)
  {
    Dout(dc::shortname,
        "In ShortName<" << type_info_of<Derived>().demangled_name() << ">::"
	"solve_collisions((" << type_info_of(old_iter).demangled_name() << "&) " << (void*)&old_iter << "): " <<
	"calling " << type_info_of(**iter).demangled_name() << "::long_name()");
    std::string const& fullname = (*iter)->long_name();
    Dout(dc::shortname, "&fullname == " << (void*)&fullname);
    size_t pos = fullname.rfind(Derived::seperator(), fullname.size() - current_len - 2) + 1;
    std::string new_shortname = fullname.substr(pos);
    ShortName<Derived> shortname(new_shortname, *iter);
    new_iter = shortname.get_iter();
  }
  const_cast<ShortName<Derived>&>(*new_iter).solve_collisions(new_iter);
  short_names.erase(old_iter++);
}
