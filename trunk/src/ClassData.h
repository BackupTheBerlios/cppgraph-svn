// cppgraph -- C++ call graph analyzer
//
//! @file ClassData.h
//! @brief This file contains the declaration of class ClassData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef CLASSDATA_H
#define CLASSDATA_H

#include <string>
#include <map>
#include "Node.h"
#include "ElementBase.h"
#include "serialization.h"

template<class Container, class Class, class Project>
class ClassData : public Node, public ElementBase<Container> {
public:
  ClassData(std::string const& base_name);

public:
  std::string const& base_name(void) const { return M_KEY_base_name; }
  typename Container::iterator parent_iter(void) const { return M_parent; }
  Class const& get_parent(void) const { return *M_parent; }
  bool is_class(void) const { return M_is_class; }
  bool is_functor(void) const { return M_is_functor; }
  std::map<typename Project::container_type::iterator, int> const& get_projects(void) const { return M_projects; }
  std::map<typename Project::container_type::iterator, int>& get_projects(void) { return M_projects; }

  friend bool operator<(ClassData<Container, Class, Project> const& cl1, ClassData<Container, Class, Project> const& cl2)
      { return cl1.M_KEY_base_name < cl2.M_KEY_base_name; }

  // Used by Node:
protected:
  virtual std::string const& node_name(void) const { return M_KEY_base_name.empty() ? S_root_namespace : M_KEY_base_name; }
  virtual NodeType node_type(void) const { return class_or_namespace_node; }
private:
  static std::string const S_root_namespace;

protected:
  std::string M_KEY_base_name;			// For example, "std::_Rb_tree<>::_Rb_tree_impl<>",
  						// with stripped template argument list.
  bool M_is_class;				// Otherwise, it could be a namespace too, actually.
  typename Container::iterator M_parent;	// Parent class, or namespace.
  bool M_is_functor;				// When used as functor at least once, between projects.
  std::map<typename Project::container_type::iterator, int> M_projects;	// Related projects and count.

private:
  // Serialization.
  friend class boost::serialization::access;
  ClassData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version))
  {
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
    ar & BOOST_SERIALIZATION_NVP(M_KEY_base_name);
    ar & BOOST_SERIALIZATION_NVP(M_is_class);
    ar & SERIALIZATION_ITERATOR_NVP(this->container, M_parent);
    ar & BOOST_SERIALIZATION_NVP(M_is_functor);
    ar & SERIALIZATION_ITERATOR_NVP(Project::container, M_projects);
  }
};

template<class Container, class Class, class Project>
ClassData<Container, Class, Project>::ClassData(std::string const& base_name) :
    M_KEY_base_name(base_name), M_is_class(false), M_parent(Class::container.end()), M_is_functor(false)
{
}


template<class Container, class Class, class Project>
std::string const ClassData<Container, Class, Project>::S_root_namespace = "::";

#endif // CLASSDATA_H

