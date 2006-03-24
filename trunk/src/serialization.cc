// cppgraph -- C++ call graph analyzer
//
//! @file serialization.cc
//! @brief This file implements serialization of all global data.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <cassert>
#include <fstream>
#define BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#include <boost/archive/tmpdir.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include "serialization.h"
#include "Function.h"

void save_state(char const* filename)
{
  // Make an archive.
  std::ofstream ofs(filename);
  assert(ofs.good());
  boost::archive::xml_oarchive oa(ofs);
  // Store the state in the archive.
  oa << BOOST_SERIALIZATION_NVP(DirTree::container);
  oa << BOOST_SERIALIZATION_NVP(Project::container);
  oa << BOOST_SERIALIZATION_NVP(Class::container);
  oa << BOOST_SERIALIZATION_NVP(Function::container);
  oa << BOOST_SERIALIZATION_NVP(Edge::container);
  assert(ofs.good());
  Dout(dc::notice, "Successfully wrote \"" << filename << '"');
  ofs.close();
}

void restore_state(char const* filename)
{
  // Open the archive.
  std::ifstream ifs(filename);
  assert(ifs.good());
  boost::archive::xml_iarchive ia(ifs);
  // Restore the state from the archive.
  ia >> BOOST_SERIALIZATION_NVP(Project::container);
  ifs.close();
}

