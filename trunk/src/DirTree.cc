// cppgraph -- C++ call graph analyzer
//
//! @file DirTree.cc
//! @brief This file contains the implementation of class DirTree.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <cassert>
#include <iostream>
#include "DirTree.h"
#include "Directory.h"
#include "debug.h"

DirTrees dirtrees;

DirTree::DirTree(std::string const& dirname) : M_KEY_parent(dirtrees.end()), M_KEY_dirname(dirname)
{
}

DirTree::DirTree(DirTrees::iterator parent, std::string const& dirname,
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
  M_iter = dirtrees.insert(*this).first;
  const_cast<DirTree&>(*M_iter).M_iter = M_iter;
  Dout(dc::dirtree, "Added DirTree with parent \"" << ((parent == dirtrees.end()) ? std::string("") : parent->str()) <<
      "\", rest \"" << dirname << "\" and totalfiles " << totalfiles << ". M_has_include = " <<
      (M_has_include ? "true" : "false") << ", M_is_general = " << (M_is_general ? "true" : "false") <<
      ", M_is_project = " << (M_is_project ? "true" : "false") <<
      ", M_is_system = " << (M_is_system ? "true" : "false"));
  if (parent != dirtrees.end())
    const_cast<DirTree&>(*parent).add_path(M_iter);
}

bool DirTree::has_include(void) const
{
  if (M_KEY_parent == dirtrees.end())
    return M_has_include;
  return M_has_include || M_KEY_parent->has_include();
}

int DirTree::nrfiles(void) const
{
  size_t count = 0;
  for (std::vector<DirTrees::iterator>::const_iterator iter = M_path_rest.begin(); iter != M_path_rest.end(); ++iter)
    count += (*iter)->totalfiles();
  return M_totalfiles - count;
}

bool DirTree::has_project(void) const
{
  if (M_is_project)
    return true;
  for (std::vector<DirTrees::iterator>::const_iterator iter = M_path_rest.begin(); iter != M_path_rest.end(); ++iter)
    if ((*iter)->has_project())
      return true;
  return false;
}

void DirTree::erase_children(void)
{
  for (int pos = M_path_rest.size() - 1; pos >= 0; --pos)
  {
    DirTree& dirtree(const_cast<DirTree&>(*M_path_rest[pos]));
    if (!M_is_general || dirtree.is_system())
    {
      dirtree.erase();
      M_path_rest.erase(M_path_rest.begin() + pos);
    }
  }
}

void DirTree::erase(void)
{
  for (std::vector<DirTrees::iterator>::const_iterator iter = M_path_rest.begin(); iter != M_path_rest.end(); ++iter)
    const_cast<DirTree&>(**iter).erase();
  dirtrees.erase(M_iter);
}

std::string DirTree::str(void) const
{
  if (M_KEY_parent == dirtrees.end())
    return M_KEY_dirname;
  return M_KEY_parent->str() + M_KEY_dirname;
}

std::ostream& operator<<(std::ostream& os, DirTree const& dirtree)
{
  return os << dirtree.str() << " (" << dirtree.totalfiles() << ")";
}

void initialize_dirtrees(
    std::vector<std::string> const& cmdline_prefixdirs,
    std::vector<std::string> const& cmdline_systemdirs)
{
  Directories::iterator iter = directories.begin();
  if (iter != directories.end())
  {
    bool at_end;
    std::string lastdir;
    Dout(dc::dirtree, "Set lastdir to \"\".");
    do
    {
      // previter points at the previous directory.
      //     iter points at the current directory.
      Directories::iterator previter = iter++;
      Dout(dc::dirtree, "previter = \"" << previter->str() << "\", iter = \"" <<
	  ((iter == directories.end()) ? std::string("<end>") : iter->str()) << "\".");
      // Determine whether or not previter is an include directory.
      bool is_prefix = false;
      for (std::vector<std::string>::const_iterator include_iter = cmdline_prefixdirs.begin();
	   include_iter != cmdline_prefixdirs.end() && !is_prefix; ++include_iter)
      {
	if (previter->str() == *include_iter)
	  is_prefix = true;
      }
      // Determine whether or not previter is a system directory.
      bool is_system = false;
      for (std::vector<std::string>::const_iterator system_iter = cmdline_systemdirs.begin();
	   system_iter != cmdline_systemdirs.end() && !is_system; ++system_iter)
      {
	if (previter->str() == *system_iter)
	  is_system = true;
      }
      // The directories are lexiographically ordered. Therefore, if the start of the current
      // directory is equal to the previous directory, the current directory is a subdirectory.
      // Moreover, if both contain the same number of files, then it has to be the only
      // subdirectory and we will always catch those (single) subdirectories this way.
      // If this is the case, then skip the parent directory (previter) and continue.
      if (!(at_end = (iter == directories.end())) &&
	  !is_prefix &&
	  iter->str().substr(0, previter->str().size()) == previter->str() &&
	  iter->count() == previter->count())
      {
	Dout(dc::dirtree, "Skipping previter.");
	continue;
      }

      // Otherwise, process 'previter' as a relevant directory and add it to dirtrees.

      // 'parent' is a parent directory of previter and will always have been added
      // before to dirtrees (again, because we process the directories lexiographically).
      // lastdir is the last directory that was actually added (or empty if none was
      // added before).
      std::string parent;
      std::string::const_iterator lastdir_iter = lastdir.begin();
      std::string::const_iterator previter_iter = previter->str().begin();
      while(1)
      {
	while (lastdir_iter != lastdir.end() && *lastdir_iter != '/')
	  ++lastdir_iter;
	while (previter_iter != previter->str().end() && *previter_iter != '/')
	  ++previter_iter;
	if (lastdir.substr(0, lastdir_iter - lastdir.begin()) !=
	    previter->str().substr(0, previter_iter - previter->str().begin()))
	  break;
	parent = lastdir.substr(0, lastdir_iter - lastdir.begin());
	if (lastdir_iter == lastdir.end() || previter_iter == previter->str().end())
	  break;
	++lastdir_iter;
	++previter_iter;
      }
      // Erase a possible last '/'.
      // Create a DirTree object (will be automatically added to dirtrees).
      Dout(dc::dirtree, "Searching for parent \"" << parent << "\".");
      DirTrees::iterator parent_iter = dirtrees.find(DirTree(parent));
      assert(parent.empty() || parent_iter != dirtrees.end());
      DirTree const dirtree(parent_iter, previter->str().substr(parent.size()),
	  previter->count(), is_prefix, previter->is_project(), is_system);
      // Keep 'lastdir' up to date.
      lastdir = previter->str();
      Dout(dc::dirtree, "Set lastdir to \"" << lastdir << "\".");
    }
    while (!at_end);
    for (DirTrees::iterator iter = dirtrees.begin(); iter != dirtrees.end(); ++iter)
    {
      if (iter->is_project() ||
	  ((iter->nrfiles() > 0 || iter->has_include()) && !iter->has_project()))
      {
#ifdef CWDEBUG
	Dout(dc::dirtree|continued_cf, "\"" << iter->str() << "\" ");
	if (iter->is_project())
	  Dout(dc::continued, "is a project directory");
	else
	  Dout(dc::continued, "has files (" << iter->nrfiles() << ") and/or include (" <<
	      (iter->has_include() ? "yes" : "no") << ")");
	Dout(dc::continued, ", erasing subdirectories... ");
#endif
	const_cast<DirTree&>(*iter).erase_children();
	Dout(dc::finish, "done.");
      }
    }
  }
}
