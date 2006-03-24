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
#include "Project.h"

template<class Container>
ShortName<Container>::ShortName(std::string const& short_name, typename Container::iterator long_name) :
    ShortNameData<std::set<ShortName<Container> >, Container>(short_name)
{
  Dout(dc::shortname, "Creating ShortName<" << type_info_of<Container>().demangled_name() << ">::ShortName(" <<
      "\"" << short_name << "\", \"" << long_name->long_name() << "\").");
  add(this->container, *this);
  const_cast<ShortName<Container>&>(*this->M_iter).
      M_long_name_iters.push_back(long_name);
  typename Container::value_type& derived = const_cast<typename Container::value_type&>(*long_name);
  assert(&derived == &*derived.get_iter());
  static_cast<LongName<Container>&>(derived).set_shortname(this->M_iter);
};

template<class Container>
void ShortName<Container>::
    solve_collisions(typename std::set<ShortName<Container> >::iterator& old_iter)
{
  if (this->M_long_name_iters.size() == 1)
  {
    ++old_iter;
    return;
  }
  typename std::set<ShortName<Container> >::iterator new_iter;
  size_t current_len = this->M_KEY_short_name.size();
  for (typename std::vector<typename Container::iterator>::iterator iter = this->M_long_name_iters.begin();
       iter != this->M_long_name_iters.end(); ++iter)
  {
    Dout(dc::shortname,
        "In ShortName<" << type_info_of<Container>().demangled_name() << ">::"
	"solve_collisions((" << type_info_of(old_iter).demangled_name() << "&) " << (void*)&old_iter << "): " <<
	"calling " << type_info_of(**iter).demangled_name() << "::long_name()");
    std::string const& fullname = (*iter)->long_name();
    Dout(dc::shortname, "&fullname == " << (void*)&fullname);
    size_t pos = fullname.rfind(Container::value_type::seperator(), fullname.size() - current_len - 2) + 1;
    std::string new_shortname = fullname.substr(pos);
    ShortName<Container> shortname(new_shortname, *iter);
    new_iter = shortname.get_iter();
  }
  const_cast<ShortName<Container>&>(*new_iter).solve_collisions(new_iter);
  this->container.erase(old_iter++);
}

