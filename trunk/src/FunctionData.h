// cppgraph -- C++ call graph analyzer
//
//! @file FunctionData.h
//! @brief This file contains the declaration of class FunctionData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef FUNCTIONDATA_H
#define FUNCTIONDATA_H

#include <vector>
#include "Node.h"
#include "ElementBase.h"
#include "serialization.h"

template<class Container, class CGDFile, class FileName, class Project, class Classes, class FunctionDecl, class Edges>
class FunctionData;

template<class Container, class CGDFile, class FileName, class Project, class Classes, class FunctionDecl, class Edges>
bool operator<(FunctionData<Container, CGDFile, FileName, Project, Classes, FunctionDecl, Edges> const& fn1,
               FunctionData<Container, CGDFile, FileName, Project, Classes, FunctionDecl, Edges> const& fn2);

template<class Container, class CGDFile, class FileName, class Project, class Classes, class FunctionDecl, class Edges>
class FunctionData : public Node, public ElementBase<Container> {
public:
  FunctionData(std::string const& function_name, FileName const& filename,
           typename CGDFile::container_type::const_iterator cgd_file) :
      M_KEY_function_name(function_name), M_KEY_decl_file(filename.get_iter()), M_definition(true), M_cgd_file(cgd_file) { }
  FunctionData(std::string const& function_name, FileName const& filename) :
      M_KEY_function_name(function_name), M_KEY_decl_file(filename.get_iter()), M_definition(false) { }

  std::string const& name(void) const { return M_KEY_function_name; }
  bool has_definition(void) const { return M_definition; }
  bool is_possibly_compiler_generated(void) const
      { return M_decl.is_destructor() || M_decl.is_assignment() || M_decl.is_copy() || M_decl.is_default(); }
  typename CGDFile::container_type::const_iterator cgd_file(void) const { return M_cgd_file; }
  std::vector<typename Edges::iterator> const& edges_out(void) const { return M_edges_out; }
  std::vector<typename Edges::iterator>& edges_out(void) { return M_edges_out; }
  std::vector<typename Edges::iterator> const& edges_in(void) const { return M_edges_in; }
  std::vector<typename Edges::iterator>& edges_in(void) { return M_edges_in; }
  FunctionDecl& decl(void) { return M_decl; }
  FunctionDecl const& decl(void) const { return M_decl; }
  Class const& get_class(void) const { return *M_class_iter; }
  FileName const& get_file(void) const { return *M_KEY_decl_file; }
  Project const& get_project(void) const { return *M_project_iter; }

  friend bool operator<(FunctionData const& fn1, FunctionData const& fn2)
  {
    return fn1.M_KEY_function_name < fn2.M_KEY_function_name ||
        (fn1.M_KEY_function_name == fn2.M_KEY_function_name &&
	 *fn1.M_KEY_decl_file < *fn2.M_KEY_decl_file);
  }

protected:
  // Called from Node.
  virtual std::string const& node_name(void) const { return name(); }
  virtual NodeType node_type(void) const { return function_node; }

protected:
  std::string M_KEY_function_name;
  typename FileName::container_type::iterator M_KEY_decl_file;
  bool M_definition;
  typename CGDFile::container_type::const_iterator M_cgd_file;
  std::vector<typename Edges::iterator> M_edges_out;
  std::vector<typename Edges::iterator> M_edges_in;
  FunctionDecl M_decl;
  typename Classes::iterator M_class_iter;
  typename Project::container_type::iterator M_project_iter;

private:
  // Serialization.
  friend class boost::serialization::access;
  FunctionData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const version);
};

#endif // FUNCTIONDATA_H

#ifndef FUNCTIONDATA2_H
#define FUNCTIONDATA2_H

#include "Edge.h"

template<class Container, class CGDFile, class FileName, class Project, class Classes, class FunctionDecl, class Edges>
template<class Archive>
void FunctionData<Container, CGDFile, FileName, Project, Classes, FunctionDecl, Edges>::
    serialize(Archive& ar, unsigned int const UNUSED(version))
{
  ar & BOOST_SERIALIZATION_NVP(M_KEY_function_name);
  ar & SERIALIZATION_ITERATOR_NVP(FileName::container, M_KEY_decl_file);
  ar & BOOST_SERIALIZATION_NVP(M_definition);
  if (M_definition)
    ar & SERIALIZATION_ITERATOR_NVP(CGDFile::container, M_cgd_file);
  ar & BOOST_SERIALIZATION_NVP(M_decl);
  ar & SERIALIZATION_ITERATOR_NVP(Class::container, M_class_iter);
  ar & SERIALIZATION_ITERATOR_NVP(Project::container, M_project_iter);
  ar & SERIALIZATION_ITERATOR_NVP(Edge::container, M_edges_out);
  ar & SERIALIZATION_ITERATOR_NVP(Edge::container, M_edges_in);
}

#endif // FUNCTIONDATA2_H
