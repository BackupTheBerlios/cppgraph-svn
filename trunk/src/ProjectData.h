// cppgraph -- C++ call graph analyzer
//
//! @file ProjectData.h
//! @brief This file contains the declaration of class ProjectData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include <string>
#include "Node.h"
#include "ElementBase.h"
#include "LongName.h"
#include "serialization.h"
#include "debug.h"

template<class Container, class DirTree>
class ProjectData;

template<class Container, class DirTree>
bool operator<(ProjectData<Container, DirTree> const& project1, ProjectData<Container, DirTree> const& project2);

template<class Container, class DirTree>
class ProjectData : public Node, public ElementBase<Container>, public LongName<Container> {
public:
  ProjectData(std::string const& filename, typename DirTree::container_type::iterator const& dirtree_iter) :
      LongName<Container>(filename), M_KEY_dirtree(dirtree_iter) { }

  // Used by LongName
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return true; }

public:
  std::string directory(void) const { return M_KEY_dirtree->str(); }
  int nrfiles(void) const { return M_KEY_dirtree->nrfiles() ; }

  friend bool operator<(ProjectData const& project1, ProjectData const& project2)
      { return project1.M_KEY_dirtree->str() > project2.M_KEY_dirtree->str(); }

protected:
  // Used by Node.
  virtual std::string const& node_name(void) const { return this->short_name(); }
  virtual NodeType node_type(void) const { return project_node; }

protected:
  typename DirTree::container_type::iterator M_KEY_dirtree;

protected:
  // Serialization.
  friend class boost::serialization::access;
  ProjectData(void) : LongName<Container>("") { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(LongName<Container>);
    ar & SERIALIZATION_ITERATOR_NVP(DirTree::container, M_KEY_dirtree);
  }
};

#endif // PROJECTDATA_H

