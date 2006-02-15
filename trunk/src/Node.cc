// cppgraph -- C++ call graph analyzer
//
//! @file Node.cc
//! @brief This file contains the implementation of class Node.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include "Node.h"
#include "Function.h"
#include "Class.h"
#include "Project.h"

Node* Node::get_node(NodeType node_type) const
{
  NodeType my_type = this->node_type();
  if (node_type == my_type)
    return const_cast<Node*>(this);
  switch (my_type)
  {
    case function_node:
    {
      Function const& me = static_cast<Function const&>(*this);
      if (node_type == class_or_namespace_node)
        return const_cast<Class*>(&me.get_class());
      if (node_type == project_node)
        return const_cast<Project*>(&me.get_project());
      break;
    }
    case class_or_namespace_node:
    {
      Class const& me = static_cast<Class const&>(*this);
      if (node_type == project_node)
        return const_cast<Project*>(&*me.get_projects().begin()->first);
      break;
    }
    case project_node:
    {
      // Project const& me = static_cast<Project const&>(*this);
      break;
    }
  }
  return NULL;
}
