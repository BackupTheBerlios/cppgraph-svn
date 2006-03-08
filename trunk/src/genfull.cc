// cppgraph -- C++ call graph analyzer
//
//! @file genfull.cc
//! @brief This file contains the implementation of main.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#include "sys.h"
#include <iostream>
#include <vector>
#include <cerrno>
#include <set>
#include <sys/types.h>
#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <iomanip>
#include "realpath.h"
#include "exceptions.h"
#include "Subdir.h"
#include "FileName.h"
#include "Location.h"
#include "Function.h"
#include "Edge.inl"
#include "collapsedpath.h"
#include "Directory.h"
#include "DirTree.h"
#include "Project.h"
#include "parser.h"
#include "Class.h"
#include "CGDFile.h"
#include "debug.h"
#include "generate_project_graph.h"
#include "Graph.h"

void process_input_line(char const* line, CGDFiles::iterator cgd_file, int line_nr, std::string const& curdir);

int const exit_code_success = 0;
int const error_parent_dir = 1;		// --subdir contains ".."
int const error_unknown_option = 2;	// Unknown command line option.
int const error_no_input = 3;		// No input files found.
int const error_runtime_exception = 4;	// Program caught runtime-error exception.

struct find_header : public std::unary_function<FileName, void> {
  char const* M_header;
  std::map<Projects::iterator, int>& M_count_map;
  find_header(char const* header, std::map<Projects::iterator, int>& count_map) :
      M_header(header), M_count_map(count_map) { }
  void operator()(FileName const& filename)
  {
    if (!filename.is_real_name())
      return;
    if (filename.short_name() == M_header)
    {
      std::pair<std::map<Projects::iterator, int>::iterator, bool> result = 
	  M_count_map.insert(std::pair<Projects::iterator, int>(filename.get_project().get_iter(), 1));
      if (!result.second)
	++(result.first->second);
    }
  }
};

bool operator<(Projects::iterator p1, Projects::iterator p2)
{
  return p1->directory() < p2->directory();
}

int main(int argc, char* const argv[])
{
  Debug(debug::init());

  // Make a copy of this.
  std::string program_name(argv[0]);

  // Parse command line arguments.
  bool print_usage = false;
  bool print_version = false;
  int verbose = 0;
  int exit_code = exit_code_success;
  std::string builddir = ".";
  std::vector<std::string> cmdline_subdirs;
  std::vector<std::string> cmdline_projectdirs;
  std::vector<std::string> cmdline_prefixdirs;
  std::vector<std::string> cmdline_systemdirs;

  cmdline_prefixdirs.push_back(std::string("/usr/include"));
  cmdline_prefixdirs.push_back(std::string("/usr/local/include"));

  cmdline_systemdirs.push_back(std::string("/usr/include/sys"));
  cmdline_systemdirs.push_back(std::string("/usr/include/asm"));
  cmdline_systemdirs.push_back(std::string("/usr/include/bits"));

  while (1)
  {
    int option_index = 0;
    static struct option long_options[] = {
      { "builddir", 1, 0, 'b' },
      { "subdir", 1, 0, 's' },
      { "projectdir", 1, 0, 'p' },
      { "prefix", 1, 0, 'g' },
      { "system", 1, 0, 'y' },
      { "verbose", 0, 0, 'v' },
      { "help", 0, 0, 'h' },
      { "version", 0, 0, 'V' },
      { 0, 0, 0, 0 }
    };

    int c = getopt_long(argc, argv, "b:hs:p:g:y:vV", long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
      case 'b':
        builddir = optarg;
	break;
      case 's':
        if (*optarg == '/' || !strcmp(optarg, "..") || std::string(optarg).find("../") != std::string::npos)
	{
	  std::cerr << program_name << ": --subdir \"" << optarg << "\":\n";
	  std::cerr << "Subdirectories are relative to the builddir (--builddir)\n"
	               "and can not be '..', start with a '/' or contain the string '../'." << std::endl;
	  exit_code = error_parent_dir;
	  break;
	}
        cmdline_subdirs.push_back(std::string(optarg));
	break;
      case 'p':
        cmdline_projectdirs.push_back(collapsedpath(std::string(optarg)));
        break;
      case 'g':
        cmdline_prefixdirs.push_back(collapsedpath(std::string(optarg) + "/include"));
        break;
      case 'y':
        cmdline_systemdirs.push_back(collapsedpath(std::string(optarg)));
        break;
      case 'v':
        ++verbose;
	break;
      case 'V':
	print_version = true;
	break;
      case 'h':
        print_usage = true;
        break;
      case '?':
        print_usage = true;
	exit_code = error_unknown_option;
        break;
      default:
        std::cerr << "?? getopt returned character code " << c << " ??\n";
    }
  }

  // Print version info if requested.
  if (print_version)
    std::cout << "cppgraph v1.0." << std::endl;

  // Print usage message when necessary.
  if (print_usage)
  {
    std::ostream* out = (exit_code == 0) ? &std::cout : &std::cerr;
    *out << "Usage: " << program_name << " [options]" << std::endl;
    *out << "Options:\n";
    *out << "\t--version, -V\t\t\tPrint version and exit successfully.\n";
    *out << "\t--help, -h\t\t\tPrint this help and exit successfully.\n";
    *out << "\t--builddir, -b <builddir>\tThe build directory [default: current directory].\n";
    *out << "\t--subdir, -s <subdir>\t\tSubdirectories to scan.\n";
    *out << "\t--projectdir, -p <dir>\t\tDirectories that contains files of a single project [default: auto].\n";
    *out << "\t--prefix, -g <dir>\t\tGeneral install prefix used for MORE than one project.\n";
    *out << "\t--system, -y <dir>\t\tSystem directories [default: /usr/include/{sys|asm|bits}].\n";
    *out << "\t--verbose, -v\t\t\tIncrease verbosity.\n";
    *out << "\nEach directory is scanned recursively unless a subdirectory\n";
    *out << "of that (sub)directory is specified with --subdir (-s).\n";
    *out << "By default, /usr and /usr/local are considered to be general install prefixes;\n";
    *out << "additional ones can be specified with the --prefix option." << std::endl;
  }

  // Exit if appropriate.
  if (print_version || print_usage || exit_code != 0)
    return exit_code;

  try
  {
    //-----------------------------------------------------------------------------------------------
    Dout(dc::subdirs, "builddir is \"" << builddir << "\".");

    //-----------------------------------------------------------------------------------------------
    // Get subdirectories to search for .cgd files.
    initialize_subdirs(builddir, cmdline_subdirs);
    if (verbose)
    {
      std::cout << "Searching the following subdirectories:" << std::endl;
      for (SubdirSet::const_iterator iter = subdirs.begin(); iter != subdirs.end(); ++iter)
        std::cout << iter->realpath() << (iter->is_recursive() ? " (recursive)" : "") << std::endl;
    }

    //-----------------------------------------------------------------------------------------------
    // Determine which files contain the call graph information.
    initialize_cgd_files();
    if (verbose)
    {
      std::cout << "Found " << cgd_files.size() << " \".cgd\" input files.\n";
      if (verbose > 1)
      {
	for (CGDFiles::iterator iter = cgd_files.begin(); iter != cgd_files.end(); ++iter)
	  std::cout << "  " << iter->long_name() << " [" << iter->short_name() << "]\n";
      }
    }

    //---------------------------------------------------------------------------------------------
    // Actually read and process the cgd files.
    // Initialize filenames, functions, locations and edges and assign source files to cgd files.
    if (verbose)
    {
      std::cout << "Scanning \".cgd\" input files" << std::flush;
      if (verbose > 1)
	std::cout << '\n';
    }
    int const buflen = 4096;
    char* buf = new char [buflen];
    AllocTag(buf, "Buffer used for storing input lines while reading .cgd files.");
    for (CGDFiles::iterator iter = cgd_files.begin(); iter != cgd_files.end(); ++iter)
    {
      if (verbose > 1)
	std::cout << "  " << iter->long_name() << std::flush;
      int line_nr = 0;
      std::string input_file = iter->long_name();
      std::ifstream input_stream;
      input_stream.open(input_file.c_str());
      std::string::size_type pos = input_file.rfind('/');
      std::string curdir = input_file.substr(0, pos);
      while (input_stream.getline(buf, buflen))
	process_input_line(buf, iter, ++line_nr, curdir);
      input_stream.close();
      if (verbose == 1)
	std::cout << '.' << std::flush;
      else if (verbose > 1)
      {
        if (iter->has_source_file())
	  std::cout << " [" << iter->source_file().short_name() << "]\n";
        else
	  std::cout << '\n';
      }
    }
    delete [] buf;
    FileName::generate_short_names();
    if (verbose)
    {
      if (verbose == 1)
	std::cout << " done.\n";
      std::cout << "Found " << filenames.size() << " different source files.\n";
      if (verbose > 2)
      {
	for (FileNames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
	  std::cout << "  \"" << iter->long_name() << "\" [" << iter->short_name() << "]\n";
      }
      std::cout << "Found " << functions.size() << " different functions (vertices).\n";
      if (verbose > 2)
      {
	for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
	  std::cout << "  \"" << iter->name() << "\"\n";
      }
      std::cout << "Found " << locations.size() << " different source locations at which functions are defined or called from.\n";
      std::cout << "Found " << edges.size() << " different caller/callee function pairs (edges).\n";
    }

    //---------------------------------------------------------------------------------------------
    // Find all directories involved.
    if (verbose)
      std::cout << "Analyzing source file directories... " << std::flush;
    initialize_directories(cmdline_projectdirs);
    if (verbose > 2)
    {
      std::cout << "\nDirectories found:\n";
      for (Directories::iterator dir_iter = directories.begin(); dir_iter != directories.end(); ++dir_iter)
	std::cout << "  " << dir_iter->str() << '\n';
    }

    //---------------------------------------------------------------------------------------------
    // Make tree of all directories.
    if (verbose > 2)
      std::cout << "Making a directory tree..." << std::flush;
    initialize_dirtrees(cmdline_prefixdirs, cmdline_systemdirs);
    if (verbose > 2)
      std::cout << " done.\n";
      
    //---------------------------------------------------------------------------------------------
    // Fill the projects container.
    if (verbose > 2)
      std::cout << "Finding possible project directories..." << std::flush;
    initialize_projects();
    if (verbose)
    {
      std::cout << "done." << std::endl;
      std::cout << "Found " << directories.size() << " different directories.\n";
      if (verbose > 1)
	for (Directories::const_iterator iter = directories.begin(); iter != directories.end(); ++iter)
	  std::cout << "  " << iter->str() << " (files: " << iter->count() << ")" << std::endl;
      std::cout << "Found " << dirtrees.size() << " possible project candidates.\n";
      if (verbose > 1)
	for (DirTrees::const_iterator iter = dirtrees.begin(); iter != dirtrees.end(); ++iter)
	{
	  std::cout << "  " << iter->str();
	  int nrfiles = iter->nrfiles();
	  if (nrfiles)
	    std::cout << " (files: " << nrfiles << ", has_include: " << (iter->has_include() ? "true)" : "false)");
	  else
	    std::cout << " is not a project directory (no files).";
	  if (iter->is_general())
	    std::cout << " General include directory.";
	  std::cout << '\n';
	}
      std::cout << "Found " << projects.size() << " different project directories.\n";
      std::cout << "  Files\tDirectory\n";
      for (Projects::const_iterator iter = projects.begin(); iter != projects.end(); ++iter)
	std::cout << "  " << iter->nrfiles() << '\t' << iter->directory() << " [" << iter->short_name() << "]\n";
    }

    //---------------------------------------------------------------------------------------------
    if (verbose)
      std::cout << "Determining which project each file belongs to..." << std::flush;
    for (FileNames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
    {
      if (!iter->is_real_name())
	continue;
      // Assign Projects to FileNames.
      for (Projects::iterator project_iter = projects.begin(); project_iter != projects.end(); ++project_iter)
      {
	if (iter->long_name().compare(0, project_iter->directory().size(), project_iter->directory()) == 0)
	{
	  const_cast<FileName&>(*iter).set_project(project_iter);
	  break;
	}
      }
    }
    if (verbose)
    {
      std::cout << " done.\n";
      if (verbose > 2)
      {
	for (Projects::iterator project_iter = projects.begin(); project_iter != projects.end(); ++project_iter)
	{
	  std::cout << "  Project: " << project_iter->directory() << '\n';
	  for (FileNames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
	  {
	    if (!iter->is_real_name())
	      continue;
	    if (iter->get_project().get_iter() == project_iter)
	      std::cout << "    " << iter->long_name() << '\n';
	  }
	}
      }
    }

    //---------------------------------------------------------------------------------------------
    // Determine which project is related to gcc itself.
    if (verbose)
      std::cout << "Determining which project corresponds to g++..." << std::flush;
    Projects::iterator gcc_iter = projects.end();
    {
      char const* const std_headers[] = { "cstdlib", "cstring", "exception", "fstream", "iomanip",
	  "iostream", "istream", "limits", "memory", "new", "ostream", "sstream", "stdexcept",
	  "streambuf", "typeinfo", NULL };
      std::map<Projects::iterator, int> count_map;
      for (char const* const* header_ptr = std_headers; *header_ptr; ++header_ptr)
	for_each(filenames.begin(), filenames.end(), find_header(*header_ptr, count_map));
      int count = 0;
      for (std::map<Projects::iterator, int>::iterator iter = count_map.begin(); iter != count_map.end(); ++iter)
	if (iter->second > count)
	{
	  count = iter->second;
	  gcc_iter = iter->first->get_iter();
	}
      for (FileNames::iterator iter = filenames.begin(); iter != filenames.end(); ++iter)
      {
	if (!iter->is_real_name())
	  const_cast<FileName&>(*iter).set_project(gcc_iter);
      }
    }
    if (verbose)
    {
      std::cout << " done.\n";
      if (gcc_iter != projects.end())
	std::cout << "g++ resides in project " << gcc_iter->directory() << std::endl;
    }

    //---------------------------------------------------------------------------------------------
    if (verbose)
    {
      std::cout << "Parsing function declarations" << std::flush;
      if (verbose > 2)
	std::cout << "..." << std::endl;
    }
    int count = 0;			// Number of functions parsed so far.
    std::set<std::string> types;	// All types, seen so far.
    for (Functions::iterator next = functions.begin(); next != functions.end();)
    {
      Functions::iterator iter = next++;
      FunctionDecl& decl(const_cast<Function&>(*iter).decl());
      if (iter->name()[0] == '(')
      {
	if (verbose > 2)
	  std::cout << "  Special: \'" << iter->name() << "'" << std::endl;
	// FIXME: add special 'declaration'.
      }
      else
      {
	if (verbose > 2)
	  std::cout << "  Parsing: " << iter->name() << std::endl;
	if (!parse_function_declaration(iter->name(), decl, types))
	{
	  if (verbose)
	    std::cout << "\nWARNING: Parsing failed for '" << iter->name() << "'!" << std::endl;
	  DoutFatal(dc::fatal, "Parsing failed for '" << iter->name() << "'!");
	  functions.erase(iter);
	  continue;
	}
      }
      if (verbose && verbose < 3 && ++count % 10 == 0)
	std::cout << '.' << std::flush;
    }
    if (verbose)
    {
      if (verbose < 3)
	std::cout << " done." << std::endl;
    }

    //---------------------------------------------------------------------------------------------
    // Initialize classes.
    if (verbose)
      std::cout << "Analyzing function declarations... " << std::flush;
    for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
      if (iter->name()[0] == '(')	// Skip 'special' functions, they don't have a declaration.
      {
        // Associate them with the root namespace.
        Class const a_class("");
	const_cast<Function&>(*iter).set_class(a_class.get_iter());
        continue;
      }
      FunctionDecl& decl(const_cast<Function&>(*iter).decl());
      Dout(dc::decl, "Analyzing function " << decl);
      Debug(libcw_do.inc_indent(2));
      // Construct a uniq key for the class of this function, if any.
      std::string scopename;	// Namespace and class scope thus far.
      bool is_class = false;
      std::vector<ClassDecl>::iterator cd_iter = decl.classes.begin();
      std::vector<std::string>::iterator cn_iter = decl.class_or_namespaces.begin();
      bool first = true;
      bool last = (cn_iter == decl.class_or_namespaces.end());
      while (!last)
      {
	if (!first)
	  scopename += "::";
	else
	  first = false;
	scopename += *cn_iter;
	// Advance iterator.
	++cn_iter;
        last = cn_iter == decl.class_or_namespaces.end();
#ifdef CWDEBUG
	Dout(dc::decl, "scopename set to \"" << scopename << "\" (is_class == " << (is_class ? "true" : "false") << ")");
	if (last)
	  Dout(dc::decl, "This is the last scopename of decl.class_or_namespaces");
#endif
	Class const a_class(scopename);
	if (!is_class && a_class.is_class())
	{
	  is_class = true;
	  Dout(dc::decl, "Setting is_class, because it was detected before");
	}
	if (is_class && !a_class.is_class())
	{
	  Dout(dc::decl, "Calling set_class() for scopename \"" << a_class.base_name() << "\"");
	  const_cast<Class&>(*a_class.get_iter()).set_class();
	}
	if (!is_class)
	{
	  is_class =
	      (last && cd_iter == decl.classes.end() &&
	          (decl.has_function_qualifiers() ||
		   decl.is_constructor() ||
		   decl.is_destructor() ||
		   decl.is_class_operator())) ||
	      (types.find(scopename) != types.end());
	  if (is_class)
	  {
#ifdef CWDEBUG
	    if (DEBUGCHANNELS::dc::decl.is_on())
	    {
	      std::string reason("Setting is_class, because ");
	      do	// So we can use break;
	      {
		if (last && cd_iter == decl.classes.end())
		{
		  if (decl.has_function_qualifiers())
		  {
		    reason += "decl.has_function_qualifiers() is true";
		    break;
		  }
		  else if (decl.is_constructor())
		  {
		    reason += "decl.is_constructor() is true";
		    break;
		  }
		  else if (decl.is_destructor())
		  {
		    reason += "decl.is_destructor() is true";
		    break;
		  }
		  else if (decl.is_class_operator())
		  {
		    reason += "decl.is_class_operator() is true";
		    break;
		  }
		}
		reason += scopename + " is part of 'types'";
	      }
	      while(0);
	      Dout(dc::decl, reason);
	    }
	    Dout(dc::decl, "Calling set_class() for scopename \"" << a_class.base_name() << "\"");
#endif
	    const_cast<Class&>(*a_class.get_iter()).set_class();
	  }
	}
	// Is this the very last?
        if (last && cd_iter == decl.classes.end())
	  const_cast<Function&>(*iter).set_class(a_class.get_iter());
      }
      last = (cd_iter == decl.classes.end());
      is_class = true;
      while (!last)
      {
	if (!first)
	  scopename += "::";
	else
	  first = false;
	scopename += cd_iter->name();
	if (cd_iter->has_template_argument_list())
	  scopename += "<>";
	last = (++cd_iter == decl.classes.end());
#ifdef CWDEBUG
	Dout(dc::decl, "scopename set to \"" << scopename << "\" (is_class == " << (is_class ? "true" : "false") << ")");
	if (last)
	  Dout(dc::decl, "This is the last scopename of decl.classes");
#endif
	Class const a_class(scopename);
#ifdef CWDEBUG
        if (!a_class.is_class())
	  Dout(dc::decl, "Calling set_class() for scopename \"" << a_class.base_name() << "\"");
#endif
	const_cast<Class&>(*a_class.get_iter()).set_class();
        if (last)
	  const_cast<Function&>(*iter).set_class(a_class.get_iter());
      }
      if (first)
      {
        // scopename is empty.
        Class const a_class(scopename);
	const_cast<Function&>(*iter).set_class(a_class.get_iter());
      }
      Debug(libcw_do.dec_indent(2));
    }
    size_t classes_size;
    do
    {
      classes_size = 0;
      for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
      {
	++classes_size;
	const_cast<Class&>(*iter).set_parent();
      }
    }
    while(classes.size() > classes_size);	// It could be that set_parent() created
    						// namespaces that weren't in classes before.
    // Perhaps we saw a type later than the first time it appeared as scope, check again.
    for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
    {
      if (!iter->is_class() && types.find(iter->base_name()) != types.end())
      {
	Dout(dc::decl, "Calling set_class() for scopename \"" << iter->base_name() << "\", because it was seen as type.");
	const_cast<Class&>(*iter).set_class();
      }
    }
    // It might happen that some parent class was marked as class, but we didn't notice that for the child.
    for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
    {
      if (!iter->is_class() && iter->parent_iter() != classes.end() && iter->parent_iter()->is_class())
      {
	Dout(dc::decl, "Calling set_class() for scopename \"" << iter->base_name() << "\", because parent is a class.");
	const_cast<Class&>(*iter).set_class();
      }
    }
    if (verbose)
    {
      std::cout << "done." << std::endl;
      std::cout << "Found " << classes.size() << " different class- or namespaces.\n";
      int nr_classes = 0;
      for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
	if (iter->is_class())
	  ++nr_classes;
      std::cout << "Found " << nr_classes << " classes with template arguments, de- or constructor, or qualified member functions.\n";
      if (verbose > 2)
	for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
	  if (iter->is_class())
	  {
	    std::string parent;
	    Classes::iterator parent_iter = iter->parent_iter();
	    if (parent_iter != classes.end())
	      parent = parent_iter->base_name();
	    std::cout << "  \"" << iter->base_name() << "\" [parent: \"" << parent << "\"]" << '\n';
	  }
      if (verbose > 1)
      {
	std::cout << "That leaves " << classes.size() - nr_classes << ", possible, namespaces.\n";
	for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
	  if (!iter->is_class())
	    std::cout << "  \"" << iter->base_name() << "\"\n";
      }
      if (verbose > 2)
      {
	std::cout << "Below follows a list of each function and the class/namespace it belongs to:\n";
	for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
	{
	  std::cout << "  " << iter->name() << '\n';
	  std::cout << "  `-> \"" << iter->get_class().base_name() << "\"\n";
	}
      }
    }

    //---------------------------------------------------------------------------------------------
    if (verbose)
      std::cout << "Determining the relationship between classes/namespaces and projects..." << std::flush;
    for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
      if (!iter->has_definition())
	continue;
      Class const& a_class = const_cast<Function&>(*iter).get_class();
      Classes::iterator class_iter = a_class.get_iter();
      Project const& project(iter->get_file().get_project());
      // The location of a (possibly) compiler generated function can be wrong (bug in compiler).
      // In that case the location refers to location of instantiation of an object that caused
      // the method to be generated. This will usually be the current project (which is the project
      // that belongs to the source file of the 'current' compilation unit).
      // Therefore, if the function at hand is possibly compiler generated, and the file of
      // it's 'definition' is part of the project that corresponds to that of the compilation
      // unit, skip it - just in case it is wrong.
      if (iter->is_possibly_compiler_generated() &&
          project.get_iter() == iter->cgd_file()->source_file().get_project().get_iter())
        continue;
      do
      {
	const_cast<Class&>(*class_iter).add_project(project);
	class_iter = class_iter->parent_iter();
      }
      while (class_iter != classes.end());
    }
    bool assigned_empty;
    bool first = true;
    do
    {
      // Inherit project(s) from parent class/namespace (but not ::), for those classes
      // that are not assigned to a project yet.
      for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
      {
	if (iter->base_name().empty())
	  continue;
	std::map<Projects::iterator, int>& project_map = const_cast<Class&>(*iter).get_projects();
	if (project_map.empty())
	{
	  Classes::iterator parent_iter = iter->parent_iter();
	  while (parent_iter != classes.end() && !parent_iter->base_name().empty())
	  {
	    if (!parent_iter->get_projects().empty())
	    {
	      project_map = parent_iter->get_projects(); 
	      break;
	    }
	    parent_iter = parent_iter->parent_iter();
	  }
	}
      }
      assigned_empty = false;
      // Assign the project that belongs to the compilation unit containing the definition
      // of classes (and their still unassigned namespace) that (apparently) only contain
      // a default constructor, copy constructor, destructor and/or assignment operator,
      // and therefore are still unassigned.
      for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
      {
        if (!iter->has_definition())
	  continue;
	Class const& a_class = iter->get_class();
	std::map<Projects::iterator, int> const& project_map = a_class.get_projects();
	if (project_map.empty() && iter->cgd_file()->has_source_file())
	{
	  Project const& project = iter->cgd_file()->source_file().get_project();
	  if (verbose)
	  {
	    if (first)
	    {
	      std::cout << '\n';
	      first = false;
	    }
	    std::cout << "  Assigning class" << (a_class.is_class() ? "" : " or namespace") <<
	        " \"" << a_class.base_name() << "\" to project " <<
		project.long_name() << std::endl;
	  }
	  Classes::iterator class_iter = a_class.get_iter();
	  do
	  {
	    const_cast<Class&>(*class_iter).add_project(project);
	    class_iter = class_iter->parent_iter();
	  }
	  while (class_iter != classes.end());
	  assigned_empty = true;
	}
      }
    }
    while (assigned_empty);
    if (verbose)
    {
      std::cout << " done.\n";
      if (verbose > 1)
      {
	for (Projects::iterator project_iter = projects.begin(); project_iter != projects.end(); ++project_iter)
	{
	  std::cout << "  Project: " << project_iter->directory() << '\n';
	  std::cout << "    Refs\tClass/Namespace\n";
	  for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
	  {
	    if (iter->base_name().empty())
	      continue;
	    std::map<Projects::iterator, int> const& project_map = iter->get_projects();
	    for (std::map<Projects::iterator, int>::const_iterator project_iter2 = project_map.begin();
		project_iter2 != project_map.end(); ++project_iter2)
	      if (project_iter2->first == project_iter)
		std::cout << std::setw(8) << project_iter2->second << "\t\"" << iter->base_name() << "\"\n";
	  }
	}
      }
      bool any_empty = false;
      bool any_ambigious = false;
      for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
      {
	if (iter->base_name().empty())
	  continue;
	std::map<Projects::iterator, int> const& project_map = iter->get_projects();
	if (project_map.empty())
	{
	  std::cout << "Class or namespace \"" << iter->base_name() << "\" is not related to any project!\n";
	  any_empty = true;
	}
	if (project_map.size() > 1)
	{
	  std::cout << '"' << iter->base_name() << "\" is related to more than one project!\n";
	  any_ambigious = true;
	  for (std::map<Projects::iterator, int>::const_iterator project_iter2 = project_map.begin();
	      project_iter2 != project_map.end(); ++project_iter2)
	    std::cout << "  " << project_iter2->first->directory() << '\n';
	}
      }
      if (!any_empty)
	std::cout << "Successfully asscociated every class and namespace with a project.\n";
      if (!any_ambigious)
	std::cout << "Good! No class or namespace is related to more than one project.\n";
      std::cout << std::flush;
    }

    //---------------------------------------------------------------------------------------------
    if (verbose)
      std::cout << "Determining the relationship between functions and projects..." << std::flush;
    for (Functions::iterator iter = functions.begin(); iter != functions.end(); ++iter)
    {
      Class const& a_class = iter->get_class();
      if (!a_class.base_name().empty())
      {
        const_cast<Function&>(*iter).set_project(a_class.get_projects().begin()->first);
        continue;
      }
      std::string function_name_prefix = iter->decl().function_name.substr(0, 6);
      if (function_name_prefix == "__cxa_" || function_name_prefix == "__tcf_")
      {
        const_cast<Function&>(*iter).set_project(gcc_iter);
        continue;
      }
      const_cast<Function&>(*iter).set_project(iter->get_file().get_project().get_iter());
    }
    if (verbose)
      std::cout << " done." << std::endl;

    //---------------------------------------------------------------------------------------------
    generate_project_graph(verbose);

    //---------------------------------------------------------------------------------------------
    if (verbose)
      std::cout << "Trying to determine which classes are functors..." << std::flush;
    for (std::set<Edge>::const_iterator iter = edges.begin(); iter != edges.end(); ++iter)
    {
      Edge const& edge(*iter);
      Function const& caller(edge.get_caller());
      if (!caller.decl().is_template())
        continue;	// Not a functor.
      Function const& callee(edge.get_callee());
      Project const& caller_project(caller.get_project());
      Project const& callee_project(callee.get_project());
      size_t caller_project_index = caller_project.get_index();
      size_t callee_project_index = callee_project.get_index();
      if (caller_project_index == callee_project_index)
        continue;	// We cannot determine if this is a functor.
      if (!project_graph.is_connected(caller_project_index, callee_project_index))
      {
        // Must be an upstream call, and therefore a functor.
	Class const& a_class(callee.get_class());
	const_cast<Class&>(a_class).set_functor();
	Dout(dc::notice, caller << " (" << caller_project_index << ") calls " << callee << " (" << callee_project_index << ')');
      }
    }
    if (verbose)
    {
      std::cout << " done." << std::endl;
      if (verbose > 1)
      {
	for (Classes::iterator iter = classes.begin(); iter != classes.end(); ++iter)
	  if (iter->is_functor())
	    std::cout << "  " << iter->base_name() << std::endl;
      }
    }
  }
  catch (clean_exit const& error)
  {
    Debug(edragon::caught(error));
    exit_code = error.exit_code();
  }
  catch (std::runtime_error const& error)
  {
    Debug(edragon::caught(error));
    std::cerr << program_name << ": " << error.what() << std::endl;
    exit_code = error_runtime_exception;
  }
  catch (no_cgd_files const& error)
  {
    Debug(edragon::caught(error));
    std::cerr << program_name << ": No \".cgd\" input files found." << std::endl;
    exit_code = error_no_input;
  }
  catch (std::exception const& exception)
  {
    std::cerr << "main: Warning: Uncaught exception \"" << exception.what() << "\"." << std::endl;
  }

  // Flush all remaining debug output.
  Dout(dc::always|noprefix_cf|nonewline_cf|flush_cf, "");
  return exit_code;
}

void process_input_line(
    char const* line, CGDFiles::iterator cgd_file, int input_line_nr, std::string const& curdir)
{
  if (line[0] != 'F' && line[0] != 'C')
  {
    std::ostringstream ss;
    ss << cgd_file->long_name() << ':' << input_line_nr << ": syntax error: Lines are expected to start with either F or C.";
    THROW_EXCEPTION(std::runtime_error(ss.str()),
	"process_input_line(\"" << line << "\", \"" << cgd_file->long_name() <<
	"\", " << input_line_nr << "): Input doesn't start with F or C");
  }
  bool parse_error = false;
  char const* ptr = line;
  std::string function_str;
  Functions::iterator caller;
  for (int field = 0; field < (line[0] == 'F' ? 2 : 4); ++field)
  {
    if (ptr[1] != ' ' || ptr[2] != '{')
    {
      parse_error = true;
      break;
    }
    int n = 2;
    while (ptr[++n] && ptr[n] != '}');
    if (ptr[n] != '}')
    {
      parse_error = true;
      break;
    }
    if (field == 0)
    {
      function_str.assign(ptr + 3, n - 3);
      std::string::size_type pos = 0;
      do
      {
	pos = function_str.find("<unnamed>::", pos); 
	if (pos == std::string::npos)
	  break;
        pos += 8;
	function_str.insert(pos, "@");
	function_str.insert(pos + 1, cgd_file->short_name());
      }
      while(1);
    }
    else if (field == 1)
    {
      std::string file_name_str(ptr + 3, n - 3);
      std::string::size_type pos = file_name_str.find(':');
      if (pos == std::string::npos)
      {
        parse_error = true;
	break;
      }
      int line_number = atoi(ptr + 4 + pos);
      if (line_number == 0)
      {
        parse_error = true;
	break;
      }
      file_name_str.erase(pos);
      bool relative = file_name_str[0] != '/' && file_name_str[0] != '<';
      if (relative)
        file_name_str = curdir + "/" + file_name_str;
      FileName const file_name(file_name_str);
      Location const location(file_name, line_number);
      if (line[0] == 'F')	// Declarion and not call location?
      {
	if (file_name.is_source_file())
	  const_cast<CGDFile&>(*cgd_file).set_source_file(file_name);
        // Add new function declaration.
	Function const function(function_str, file_name, cgd_file);
      }
      else
      {
        Function const function(function_str, file_name);
	caller = function.get_iter();
      }
    }
    else if (field == 2)
    {
      function_str.assign(ptr + 3, n - 3);
      std::string::size_type pos = 0;
      do
      {
	pos = function_str.find("<unnamed>::", pos); 
	if (pos == std::string::npos)
	  break;
        pos += 8;
	function_str.insert(pos, "@");
	function_str.insert(pos + 1, cgd_file->short_name());
      }
      while(1);
    }
    else if (field == 3)
    {
      std::string file_name_str(ptr + 3, n - 3);
      bool relative = file_name_str[0] != '/' && file_name_str[0] != '<';
      if (relative)
        file_name_str = curdir + "/" + file_name_str;
      FileName const file_name(file_name_str);
      Function const callee(function_str, file_name);
      const_cast<Function&>(*caller).add_callee(callee);
    }
    ptr += n;
  }
  if (parse_error)
  {
    std::ostringstream ss;
    ss << cgd_file->long_name() << ':' << input_line_nr << ": Parse error.";
    THROW_EXCEPTION(std::runtime_error(ss.str()),
	"process_input_line(\"" << line << "\", \"" << cgd_file->long_name() <<
	"\", " << input_line_nr << "): Parse error");
  }
}
