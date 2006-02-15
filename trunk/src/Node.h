// cppgraph -- C++ call graph analyzer
//
//! @file Node.h
//! @brief This file contains the declaration of class Node.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef NODE_H
#define NODE_H

#include <sys/types.h>
#include <string>

enum NodeType {
  project_node,
  class_or_namespace_node,
  function_node
};

class Node {
private:
  size_t M_index;

public:
  Node(void) { }
  virtual ~Node() { }

  void set_index(size_t index) { M_index = index; }
  size_t get_index(void) const { return M_index; }
  Node* get_node(NodeType node_type) const;

  virtual std::string const& node_name(void) const = 0;
  virtual NodeType node_type(void) const = 0;

};

#endif // NODE_H
