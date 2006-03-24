// cppgraph -- C++ call graph analyzer
//
//! @file FileNameData.h
//! @brief This file contains the declaration of class FileNameData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef FILENAMEDATA_H
#define FILENAMEDATA_H

#include "Project.h"
#include "ElementBase.h"
#include "LongName.h"
#include "serialization.h"
#include "collapsedpath.h"

template<class Container>
class FileNameData : public ElementBase<Container>, public LongName<Container> {
public:
  FileNameData(std::string const& filename) :
    LongName<Container>((filename[0] == '/') ? collapsedpath(filename) : filename) { }

public:
  bool is_real_name(void) const { return this->M_KEY_long_name[0] == '/'; }
  Project const& get_project(void) const { return *M_project; }
  bool is_source_file(void) const { return M_is_source_file; }

public:
  // Used by LongName.
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return is_real_name(); }

protected:
  Project::container_type::iterator M_project;
  bool M_is_source_file;			// Set when filename ends on .cc, .cpp, .cxx or .C.

private:
  // Serialization.
  friend class boost::serialization::access;
  FileNameData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(LongName<Container>);
    ar & SERIALIZATION_ITERATOR_NVP(Project::container, M_project);
    ar & BOOST_SERIALIZATION_NVP(M_is_source_file);
  }
};

#endif // FILENAMEDATA_H
