// cppgraph -- C++ call graph analyzer
//
//! @file Directory.h
//! @brief This file contains the declaration of class Directory.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <set>

class Directory {
public:
  typedef std::set<Directory> container_type;
private:
  std::string const M_KEY_directory;	// The path.
  int M_count;				// Number of time this directory name occurs.
  int M_subs;				// Depth of it's path (number of slashes).
  bool M_is_project;			// True of this directory is specified as project on the command line.
public: 
  Directory(std::string const& directory, bool is_project = false) :
      M_KEY_directory(directory), M_count(0), M_subs(0), M_is_project(is_project) { }

  void increment_count(void) { ++M_count; }
  int count(void) const { return M_count; }
  std::string const& str(void) const { return M_KEY_directory; }
  int subs(void) const { return M_subs; }
  int& subs(void) { return M_subs; }
  bool is_project(void) const { return M_is_project; }

  friend bool operator<(Directory const& d1, Directory const& d2)
  {
    std::string::const_iterator p1 = d1.M_KEY_directory.begin();
    std::string::const_iterator p2 = d2.M_KEY_directory.begin();
    for(;;)
    {
      if (p1 == d1.M_KEY_directory.end())
        return p2 != d2.M_KEY_directory.end();
      if (p2 == d2.M_KEY_directory.end())
        return false;
      if (*p1 != *p2)
	return *p1 == '/' || (*p1 < *p2 && *p2 != '/');
      ++p1;
      ++p2;
    }
  }
};

typedef Directory::container_type Directories;

extern Directories directories;
void initialize_directories(std::vector<std::string> const& cmdline_projectdirs);

#endif // DIRECTORY_H
