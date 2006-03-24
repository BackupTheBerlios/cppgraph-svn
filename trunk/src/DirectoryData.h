// cppgraph -- C++ call graph analyzer
//
//! @file DirectoryData.h
//! @brief This file contains the declaration of class DirectoryData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DIRECTORYDATA_H
#define DIRECTORYDATA_H

#include <string>
#include "ElementBase.h"
#include "serialization.h"

template<class Container>
class DirectoryData;

template<class Container>
bool operator<(DirectoryData<Container> const& d1, DirectoryData<Container> const& d2);

template<class Container>
class DirectoryData : public ElementBase<Container> {
public:
  DirectoryData(std::string const& directory, bool is_project = false) :
      M_KEY_directory(directory), M_subs(0), M_is_project(is_project) { }

  std::string const& str(void) const { return M_KEY_directory; }
  int subs(void) const { return M_subs; }
  int& subs(void) { return M_subs; }
  bool is_project(void) const { return M_is_project; }

  friend bool operator<(DirectoryData const& d1, DirectoryData const& d2)
  {
    typename std::string::const_iterator p1 = d1.M_KEY_directory.begin();
    typename std::string::const_iterator p2 = d2.M_KEY_directory.begin();
    for(;;)
    {
      if (p1 == d1.M_KEY_directory.end())
        return p2 != d2.M_KEY_directory.end();
      if (p2 == d2.M_KEY_directory.end())
        return false;
      if (*p1 != *p2)
	return *p1 == '/' || (*p1 < *p2 && *p2 != '/');
      ++p1;
      ++p2;
    }
  }

private:
  std::string M_KEY_directory;		// The path.
  int M_subs;				// Depth of it's path (number of slashes).
  bool M_is_project;			// True of this directory is specified as project on the command line.

private:
  // Serialization.
  friend class boost::serialization::access;
  DirectoryData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const version)
  {
    ar & BOOST_SERIALIZATION_NVP(M_KEY_directory);
    ar & BOOST_SERIALIZATION_NVP(M_subs);
    ar & BOOST_SERIALIZATION_NVP(M_is_project);
  }
};

#endif // DIRECTORYDATA_H

