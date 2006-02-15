SUBDIRS = .

EXTRA_DIST =
BUILT_SOURCES =

distclean-local:

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in

maintainer-clean-local:
	rm -rf html latex

all-local: doxygen.config
	@echo "doxygen doxygen.config"
	@doxygen doxygen.config || true
	mkdir -p $(top_builddir)/doc/html/images
	$(INSTALL) -m 644 $(srcdir)/images/*.png $(top_builddir)/doc/html/images
	mv $(top_builddir)/doc/html/doxygen.png $(top_builddir)/doc/html/images
	$(INSTALL) -m 644 $(srcdir)/@PACKAGE@.css $(top_builddir)/doc/html
