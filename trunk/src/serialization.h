// cppgraph -- C++ call graph analyzer
//
//! @file serialization.h
//! @brief This file contains the declaration of the serialization interface.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <set>
#include <vector>
#include <map>
#include "ElementBase.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>

void save_state(char const* filename);
void restore_state(char const* filename);

// A wrapper for iterators.
template<class Container, typename Iterator>
class IteratorWrapper {
public:
  IteratorWrapper(Container& container, Iterator& iter) : M_container(container), M_iter(iter) { }
private:
  Container& M_container;			// The container.
  Iterator& M_iter;				// The actual iterator.
public:
  template<class Archive>
  void serialize(Archive& ar, unsigned int const UNUSED(version)) const
  {
    bool is_end = M_iter == M_container.end();
    int index = is_end ? -1 : M_iter->get_serialization_index();
    ar & boost::serialization::make_nvp("index", index);
  }
};

// A wrapper for iterators.
template<class T, typename Iterator>
class IteratorWrapper<std::vector<T>, Iterator> {
public:
  IteratorWrapper(std::vector<T>& container, Iterator& iter) : M_container(container), M_iter(iter) { }
private:
  std::vector<T>& M_container;			// The container.
  Iterator& M_iter;				// The actual iterator.
public:
  template<class Archive>
  void save(Archive& ar, unsigned int const UNUSED(version)) const
  {
    bool is_end = M_iter == M_container.end();
    int index = is_end ? -1 : M_iter->get_serialization_index();
    ar << boost::serialization::make_nvp("index", index);
  }
  template<class Archive>
  void load(Archive& ar, unsigned int const UNUSED(version))
  {
    int index;
    ar >> boost::serialization::make_nvp("index", index);
    if (index >= 0)
      M_iter = ElementBase<std::vector<T> >::get_serialization_iter(index);
    else
      M_iter = M_container.end();
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

// A wrapper for a vector of iterators.
template<class Container, typename Iterator>
class VectorIteratorWrapper {
public:
  VectorIteratorWrapper(Container& container, std::vector<Iterator>& iter) : M_container(container), M_vec(iter) { }
private:
  Container& M_container;			// The container.
  std::vector<Iterator>& M_vec;			// The actual vector of iterators.
public:
  template<class Archive>
  void save(Archive& ar, unsigned int const UNUSED(version)) const
  {
    int count = M_vec.size();
    ar << BOOST_SERIALIZATION_NVP(count);
    for (typename std::vector<Iterator>::const_iterator iter = M_vec.begin(); iter != M_vec.end(); ++iter)
    {
      IteratorWrapper<Container, Iterator> iter_functor(M_container, const_cast<Iterator&>(*iter));
      ar << boost::serialization::make_nvp("item", iter_functor);
    }
  }
  template<class Archive>
  void load(Archive& ar, unsigned int const UNUSED(version))
  {
    int count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    while(count--)
    {
      Iterator tmp_iter;
      IteratorWrapper<Container, Iterator> iter_functor(M_container, tmp_iter);
      ar >> boost::serialization::make_nvp("item", iter_functor);
      M_vec.push_back(tmp_iter);
    }
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

// A wrapper for a map of iterators.
template<class Container, typename Iterator, typename INFO>
class MapIteratorWrapper {
public:
  MapIteratorWrapper(Container& container, std::map<Iterator, INFO>& iter) : M_container(container), M_map(iter) { }
private:
  Container& M_container;			// The container.
  std::map<Iterator, INFO>& M_map;		// The actual map of iterators.
  static INFO S_dummy_info;
public:
  template<class Archive>
  void save(Archive& ar, unsigned int const UNUSED(version)) const
  {
    int count = M_map.size();
    ar << BOOST_SERIALIZATION_NVP(count);
    for (typename std::map<Iterator, INFO>::const_iterator iter = M_map.begin(); iter != M_map.end(); ++iter)
    {
      IteratorWrapper<Container, Iterator> iter_functor(M_container, const_cast<Iterator&>(iter->first));
      ar << boost::serialization::make_nvp("item", iter_functor);
    }
  }
  template<class Archive>
  void load(Archive& ar, unsigned int const UNUSED(version))
  {
    int count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    while(count--)
    {
      Iterator tmp_iter;
      IteratorWrapper<Container, Iterator> iter_functor(M_container, tmp_iter);
      ar >> boost::serialization::make_nvp("item", iter_functor);
      M_map.insert(std::pair<Iterator, INFO>(tmp_iter, S_dummy_info));
    }
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Container, typename Iterator, typename INFO>
INFO MapIteratorWrapper<Container, Iterator, INFO>::S_dummy_info;

// This macro generates the name string (from 'name').
#define SERIALIZATION_ITERATOR_NVP(container, name) \
    serialization_iterator_nvp(container, BOOST_PP_STRINGIZE(name), name)

template<typename Container>
inline boost::serialization::nvp<IteratorWrapper<Container, typename Container::iterator> > const
    serialization_iterator_nvp(Container& container, char const* name, typename Container::iterator const& t)
{
  IteratorWrapper<Container, typename Container::iterator>
      tmp(container, const_cast<typename Container::iterator&>(t));
  return boost::serialization::nvp<IteratorWrapper<Container, typename Container::iterator> >(name, tmp);
}

template<typename Container>
inline boost::serialization::nvp<IteratorWrapper<Container, typename Container::const_iterator> > const
    serialization_iterator_nvp(Container& container, char const* name, typename Container::const_iterator const& t)
{
  IteratorWrapper<Container, typename Container::const_iterator>
      tmp(container, const_cast<typename Container::const_iterator&>(t));
  return boost::serialization::nvp<IteratorWrapper<Container, typename Container::const_iterator> >(name, tmp);
}

// Specialization for std::set, because for that container, the iterator and const_iterator are the same type!
template<typename T>
inline boost::serialization::nvp<IteratorWrapper<std::set<T>, typename std::set<T>::const_iterator> > const
    serialization_iterator_nvp(std::set<T>& container, char const* name, typename std::set<T>::const_iterator const& t)
{
  IteratorWrapper<std::set<T>, typename std::set<T>::const_iterator>
      tmp(container, const_cast<typename std::set<T>::const_iterator&>(t));
  return boost::serialization::nvp<IteratorWrapper<std::set<T>, typename std::set<T>::const_iterator> >(name, tmp);
}

template<typename Container>
inline boost::serialization::nvp<VectorIteratorWrapper<Container, typename Container::iterator> > const
    serialization_iterator_nvp(Container& container, char const* name,
                               std::vector<typename Container::iterator> const& v)
{
  VectorIteratorWrapper<Container, typename Container::iterator>
      tmp(container, const_cast<std::vector<typename Container::iterator>&>(v));
  return boost::serialization::nvp<VectorIteratorWrapper<Container, typename Container::iterator> >(name, tmp);
}

template<typename Container>
inline boost::serialization::nvp<VectorIteratorWrapper<Container, typename Container::const_iterator> > const
    serialization_iterator_nvp(Container& container, char const* name,
                               std::vector<typename Container::const_iterator> const& v)
{
  VectorIteratorWrapper<Container, typename Container::const_iterator>
      tmp(container, const_cast<std::vector<typename Container::const_iterator>&>(v));
  return boost::serialization::nvp<VectorIteratorWrapper<Container, typename Container::const_iterator> >(name, tmp);
}

// Specialization for std::set, because for that container, the iterator and const_iterator are the same type!
template<typename T>
inline boost::serialization::nvp<VectorIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator> > const
    serialization_iterator_nvp(std::set<T>& container, char const* name,
                               std::vector<typename std::set<T>::const_iterator> const& v)
{
  VectorIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator>
      tmp(container, const_cast<std::vector<typename std::set<T>::const_iterator>&>(v));
  return boost::serialization::nvp<VectorIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator> >(name, tmp);
}

template<typename Container, typename INFO>
inline boost::serialization::nvp<MapIteratorWrapper<Container, typename Container::iterator, INFO> > const
    serialization_iterator_nvp(Container& container, char const* name,
                               std::map<typename Container::iterator, INFO> const& m)
{
  MapIteratorWrapper<Container, typename Container::iterator, INFO>
      tmp(container, const_cast<std::map<typename Container::iterator, INFO>&>(m));
  return boost::serialization::nvp<MapIteratorWrapper<Container, typename Container::iterator, INFO> >(name, tmp);
}

template<typename Container, typename INFO>
inline boost::serialization::nvp<MapIteratorWrapper<Container, typename Container::const_iterator, INFO> > const
    serialization_iterator_nvp(Container& container, char const* name,
                               std::map<typename Container::const_iterator, INFO> const& m)
{
  MapIteratorWrapper<Container, typename Container::const_iterator, INFO>
      tmp(container, const_cast<std::map<typename Container::const_iterator, INFO>&>(m));
  return boost::serialization::nvp<MapIteratorWrapper<Container, typename Container::const_iterator, INFO> >(name, tmp);
}

// Specialization for std::set, because for that container, the iterator and const_iterator are the same type!
template<typename T, typename INFO>
inline boost::serialization::nvp<MapIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator, INFO> > const
    serialization_iterator_nvp(std::set<T>& container, char const* name,
                               std::map<typename std::set<T>::const_iterator, INFO> const& m)
{
  MapIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator, INFO>
      tmp(container, const_cast<std::map<typename std::set<T>::const_iterator, INFO>&>(m));
  return boost::serialization::nvp<MapIteratorWrapper<std::set<T>, typename std::set<T>::const_iterator, INFO> >(name, tmp);
}

#endif // SERIALIZATION_H
