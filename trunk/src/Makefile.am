## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS = include .

bin_PROGRAMS = genfull
# If you change this, also update DEFS in all other Makefile.am.
DEFS = -DHAVE_CONFIG_H
CXXFLAGS = @CXXFLAGS@ @CWD_FLAGS@

LIBS = @LIBS@ @CWD_LIBS@
genfull_SOURCES = \
	serialization.cc \
        generate_class_graph.cc \
	generate_project_graph.cc \
	Graph.cc \
	Node.cc \
	genfull.cc \
	realpath.cc \
	collapsedpath.cc \
	Subdir.cc \
	CGDFile.cc \
	Function.cc \
	Directory.cc \
	DirTree.cc \
	Project.cc \
	parser.cc \
	FunctionDecl.cc \
	Class.cc \
	ClassDecl.cc \
	debug.cc

genfull_CXXFLAGS = -I$(srcdir)/include/genfull

# --------------- Maintainer's Section

#dist-hook:

all-local:
	@if test -e $(top_srcdir)/../.libcwdrc-src -a ! -e .libcwdrc; then \
	  echo "ln -s $(top_srcdir)/../.libcwdrc-src .libcwdrc"; \
	  ln -s $(top_srcdir)/../.libcwdrc-src .libcwdrc; \
	fi
	@if test -e $(top_srcdir)/../.gdbinit-src -a ! -e .gdbinit; then \
	  echo "ln -s $(top_srcdir)/../.gdbinit-src .gdbinit"; \
	  ln -s $(top_srcdir)/../.gdbinit-src .gdbinit; \
	fi

distclean-local:
	rm -rf $(srcdir)/autom4te.cache
	@MAINTAINER_MODE_TRUE@rm -f .all_sources *.s *.ii

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in tags troep*

if MAINTAINER_MODE
# Usage: You have to set the CTAGS_ROOT_SRCDIRS environment variable yourself,
# for example: export CTAGS_ROOT_SRCDIRS="/usr/src/gtk/glib-current /usr/src/gtk/gtk+-current"
# Then run 'vim -u /some/path/.vimrc' where the .vimrc is the normal .vimrc, but with the
# additional lines: set tags=/full/path/to/top_builddir/tags
#                   nmap <C-N> :tnext<CR>
ctags:
	rm -f .all_sources
	touch .all_sources
	for d in $(CTAGS_ROOT_SRCDIRS) $(top_srcdir); do \
	  find $$d -follow -type f -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.C' >> .all_sources; \
	done
	ctags -L .all_sources
endif

