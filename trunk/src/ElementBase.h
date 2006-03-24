// cppgraph -- C++ call graph analyzer
//
//! @file ElementBase.h
//! @brief This file contains the declaration of class ElementBase.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef ELEMENTBASE_H
#define ELEMENTBASE_H

#include <set>
#include <list>

template<class Container>
class ElementBase {
public:
  typedef Container container_type;				// Type of container storing derived elements.
  static container_type container;				// The actual container.

public:
  static void initialize_serialization_index(void);		// Initialize M_serialization_index.

  int get_serialization_index(void) const { return M_serialization_index; }
  static typename Container::iterator get_serialization_iter(int index) { container.begin() + index; }
  typename Container::iterator get_iter(void) const { return M_iter; }

protected:
  // Add element to container and initialize M_iter.
  template<typename T>
  bool add(std::set<T> const&, typename std::set<T>::value_type const& element);

public:
  template<typename T>
  void add(std::list<T> const&, typename std::list<T>::value_type const& element);

protected:
  typename Container::iterator M_iter;				// Iterator to self.
  int M_serialization_index;
};

template<class Container>
void ElementBase<Container>::initialize_serialization_index(void)
{
  int count = 0;
  // Run over the all objects in the same order as that they will be serialized,
  // and assign an index count.
  for (typename Container::iterator iter = container.begin(); iter != container.end(); ++iter)
    const_cast<typename Container::value_type&>(*iter).M_serialization_index = count++;
}

template<class Container>
template<class T>
bool ElementBase<Container>::add(std::set<T> const&, typename std::set<T>::value_type const& element)
{
  std::pair<typename Container::iterator, bool> result = T::container.insert(element);
  M_iter = result.first;
  const_cast<T&>(*M_iter).M_iter = M_iter;
  return result.second;
}

template<class Container>
template<class T>
void ElementBase<Container>::add(std::list<T> const&, typename std::list<T>::value_type const& element)
{
  T::container.push_back(element);
  M_iter = T::container.end();
  --M_iter;
  const_cast<T&>(*M_iter).M_iter = M_iter;
}

template<class Container>
typename ElementBase<Container>::container_type ElementBase<Container>::container;

#endif // ELEMENTBASE_H
