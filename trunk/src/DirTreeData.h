// cppgraph -- C++ call graph analyzer
//
//! @file DirTreeData.h
//! @brief This file contains the declaration of class DirTreeData.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DIRTREEDATA_H
#define DIRTREEDATA_H

#include <string>
#include <vector>
#include "ElementBase.h"
#include "serialization.h"
#include "debug.h"

template<class Container>
class DirTreeData : public ElementBase<Container> {
protected:
  DirTreeData(std::string const& dirname);
  DirTreeData(typename Container::iterator parent, std::string const& dirname,
      int totalfiles, bool general_prefix, bool is_project, bool is_system);

public:
  std::string const& dirname(void) const { return M_KEY_dirname; }
  typename Container::iterator parent(void) const { return M_KEY_parent; }
  std::string str(void) const;
  int totalfiles(void) const { return M_totalfiles; }
  int nrfiles(void) const;
  bool has_include(void) const;
  bool is_general(void) const { return M_is_general; }
  bool is_project(void) const { return M_is_project; }
  bool is_system(void) const { return M_is_system; }
  bool has_project(void) const;
  std::vector<typename Container::iterator> const& children(void) const { return M_path_rest; }

protected:
  typename Container::iterator M_KEY_parent;			// The parent directory.
  std::string M_KEY_dirname;					// Part of the path, ie "foo/bar". Never empty.
  int M_totalfiles;						// The number of files in this directory (recursively).
  std::vector<typename Container::iterator> M_path_rest;	// Possible branches.
  bool M_has_include;						// M_KEY_long_name contains the subdirectory 'include'.
  bool M_is_general;						// Marked as general installation path.
  bool M_is_project;						// Marked as project directory;
  bool M_is_system;						// Marked as system directory;

private:
  // Serialization.
  friend class boost::serialization::access;
  DirTreeData(void) { }	// Uninitialized object (needed for serialization).
  template<class Archive>
  void serialize(Archive& ar, unsigned int const version);
};

template<class Container>
inline bool operator<(DirTreeData<Container> const& dirtree1, DirTreeData<Container> const& dirtree2)
{
  return dirtree1.str() < dirtree2.str();
}

template<class Container>
DirTreeData<Container>::DirTreeData(std::string const& dirname) :
    M_KEY_parent(ElementBase<Container>::container.end()), M_KEY_dirname(dirname)
{
}

template<class Container>
DirTreeData<Container>::DirTreeData(typename Container::iterator parent, std::string const& dirname,
    int totalfiles, bool general_prefix, bool is_project, bool is_system) :
    M_KEY_parent(parent), M_KEY_dirname(dirname), M_totalfiles(totalfiles),
    M_has_include(false), M_is_general(general_prefix), M_is_project(is_project), M_is_system(is_system)
{
  assert(dirname.size() > 0);
  std::string::size_type len = dirname.size();
  for (std::string::size_type pos = 0; pos + 6 < len && !M_has_include; ++pos)
  {
    if (dirname[pos] == 'i' && (pos == 0 || dirname[pos - 1] == '/') &&
        dirname.substr(pos, 7) == "include" && (pos == len - 7 || dirname[pos + 7] == '/'))
      M_has_include = true;
  }
}

template<class Container>
std::string DirTreeData<Container>::str(void) const
{
  if (M_KEY_parent == ElementBase<Container>::container.end())
    return M_KEY_dirname;
  return M_KEY_parent->str() + M_KEY_dirname;
}

template<class Container>
bool DirTreeData<Container>::has_project(void) const
{
  if (M_is_project)
    return true;
  for (typename std::vector<typename Container::iterator>::const_iterator iter = M_path_rest.begin();
      iter != M_path_rest.end(); ++iter)
    if ((*iter)->has_project())
      return true;
  return false;
}

template<class Container>
int DirTreeData<Container>::nrfiles(void) const
{
  size_t count = 0;
  for (typename std::vector<typename Container::iterator>::const_iterator iter = M_path_rest.begin();
      iter != M_path_rest.end(); ++iter)
    count += (*iter)->totalfiles();
  return M_totalfiles - count;
}

template<class Container>
bool DirTreeData<Container>::has_include(void) const
{
  if (M_KEY_parent == ElementBase<Container>::container.end())
    return M_has_include;
  return M_has_include || M_KEY_parent->has_include();
}

template<class Container>
template<class Archive>
void DirTreeData<Container>::serialize(Archive& ar, unsigned int const UNUSED(version))
{
  ar & SERIALIZATION_ITERATOR_NVP(ElementBase<Container>::container, M_KEY_parent);
  ar & BOOST_SERIALIZATION_NVP(M_KEY_dirname);
  ar & BOOST_SERIALIZATION_NVP(M_totalfiles);
  ar & SERIALIZATION_ITERATOR_NVP(ElementBase<Container>::container, M_path_rest);
  ar & BOOST_SERIALIZATION_NVP(M_has_include);
  ar & BOOST_SERIALIZATION_NVP(M_is_general);
  ar & BOOST_SERIALIZATION_NVP(M_is_project);
  ar & BOOST_SERIALIZATION_NVP(M_is_system);
}

template<class Container>
std::ostream& operator<<(std::ostream& os, DirTreeData<Container> const& dirtree)
{
  return os << dirtree.str() << " (" << dirtree.totalfiles() << ")";
}

#endif // DIRTREEDATA_H
