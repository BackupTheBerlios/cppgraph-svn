// cppgraph -- C++ call graph analyzer
//
//! @file CGDFile.h
//! @brief This file contains the declaration of class CGDFile.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef CGDFILE_H
#define CGDFILE_H

#include "LongName.h"
#include "FileName.h"

class CGDFile : public LongName<CGDFile> {
public:
  typedef std::list<CGDFile> container_type;	// Must be list, so that push_back doesn't invalidate iterators.
  FileNames::iterator M_source_file;
public:
  CGDFile(std::string const& filename) : LongName<CGDFile>(filename), M_source_file(filenames.end()) { }
  static char const* seperator(void) { return "/"; }
  bool process(void) const { return true; }
  bool has_source_file(void) const { return M_source_file != filenames.end(); }
  FileName const& source_file(void) const { return *M_source_file; }
  void set_source_file(FileName const& source_file) { M_source_file = source_file.get_iter(); }
};

typedef CGDFile::container_type CGDFiles;
extern CGDFiles cgd_files;
void initialize_cgd_files(void);

#endif // CGDFILE_H
