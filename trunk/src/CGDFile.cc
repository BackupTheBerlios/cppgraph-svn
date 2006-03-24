// cppgraph -- C++ call graph analyzer
//
//! @file CGDFile.cc
//! @brief This file contains the implementation of class CGDFile.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <sys/types.h>
#include <dirent.h>
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>
#include "CGDFile.h"
#include "Subdir.h"
#include "exceptions.h"
#include "debug.h"

static void process_directory(std::string const& path, bool recursive)
{
  Dout(dc::subdirs, path << (recursive ? " (recursive)" : " (not recursive)"));
  DebugIndent(2);
  DIR* dir = opendir(path.c_str());
  if (!dir)
    THROW_EXCEPTION(std::runtime_error(path + ": " + strerror(errno)),
	"process_directory(\"" << path << "\", " << (recursive ? "true" : "false") << "): opendir failed");
  struct dirent* dirent;
  while ((dirent = readdir(dir)))
  {
    struct stat statbuf;
    std::string direntname = path + "/" + dirent->d_name;
    if (stat(direntname.c_str(), &statbuf) == -1)
      THROW_EXCEPTION(std::runtime_error("stat: " + direntname + ": " + strerror(errno)),
	  "process_directory(\"" << path << "\", " << (recursive ? "true" : "false") << "): stat failed");
    if (S_ISREG(statbuf.st_mode))
    {
      size_t len = strlen(dirent->d_name);
      if (len >= 4 && !strcmp(dirent->d_name + len - 4, ".cgd"))
      {
	Dout(dc::subdirs, "Found: " << direntname);
	CGDFile cgd_file(direntname);
	cgd_file.add(CGDFile::container, cgd_file);
	CGDFile::container_type::iterator cgd_iter = cgd_file.get_iter();
	CGDFile::init_short_name(cgd_iter);
      }
    }
    else if (recursive && dirent->d_name[0] != '.' && S_ISDIR(statbuf.st_mode))
      process_directory(direntname, true);
  }
  if (errno == EBADF)
    THROW_EXCEPTION(std::runtime_error(std::string("readdir: ") + strerror(errno)),
	"process_directory(\"" << path << "\", " << (recursive ? "true" : "false") << "): readdir(" << dir << ") failed");
  closedir(dir);
}

void initialize_cgd_files(void)
{
  for (SubdirSet::iterator iter = subdirs.begin(); iter != subdirs.end(); ++iter)
    process_directory(iter->realpath(), iter->is_recursive());
  if (CGDFile::container.empty())
    THROW_EXCEPTION(no_cgd_files(), "initialize_cgd_files(): No \".cgd\" input files found");
  CGDFile::generate_short_names();
}

