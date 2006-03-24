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

#include <vector>
#include "CGDFileData.h"
#include "FileName.h"

class CGDFile : public CGDFileData<std::list<CGDFile>, FileName> {
public:
  CGDFile(std::string const& filename) : CGDFileData<std::list<CGDFile>, FileName>(filename) { /* No call to add() */ }

public:
  void set_source_file(FileName const& source_file) { M_source_file = source_file.get_iter(); }
};

void initialize_cgd_files(void);

#endif // CGDFILE_H
