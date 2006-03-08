// cppgraph -- C++ call graph analyzer
//
//! @file Subdir.h
//! @brief This file contains the declaration of class Subdir.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef SUBDIR_H
#define SUBDIR_H

#include <set>
#include <string>
#include <vector>
#include <functional>

class Subdir {
private:
  std::string M_realpath;
  bool M_recursive;
public:
  Subdir(void) { }
  Subdir(std::string const& realpath) : M_realpath(realpath), M_recursive(true) { }

  std::string const& realpath(void) const { return M_realpath; }

  bool is_recursive(void) const { return M_recursive; }
  void reset_recursive(void) { M_recursive = false; }
};

struct SubdirLess : public std::binary_function<Subdir, Subdir, bool> {
  bool operator()(Subdir const& lhs, Subdir const& rhs) const { return lhs.realpath() < rhs.realpath(); }
};

typedef std::set<Subdir, SubdirLess> SubdirSet;
extern SubdirSet subdirs;
void initialize_subdirs(std::string builddir, std::vector<std::string> const& cmdline_subdirs);

#endif // SUBDIR_H
