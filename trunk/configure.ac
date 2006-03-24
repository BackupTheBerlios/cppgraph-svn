dnl Process this file with autoconf to produce a configure script.

dnl Define package version here.
define(TE_VERSION_MAJOR, 1)
define(TE_VERSION_MINOR, 0)
define(TE_VERSION_REVISION, 0)
define(TE_PACKAGE_NAME, cppgraph)
define(TE_BUGREPORT, timmy1992@gmail.com)

dnl Package name and version
AC_INIT(TE_PACKAGE_NAME, TE_VERSION_MAJOR.TE_VERSION_MINOR.TE_VERSION_REVISION, TE_BUGREPORT)

dnl Put resulting configuration defines in this header file.
AC_CONFIG_HEADERS([config.h])

dnl Put revision of configure.ac in configure script.
AC_REVISION($Revision: 1.0 $)dnl

dnl Ensure that the directory specified with --srcdir was correct
dnl by demanding that it contains LICENSE.QPL.
AC_CONFIG_SRCDIR([LICENSE.QPL])

dnl Automake options.
AM_INIT_AUTOMAKE

dnl Include maintainer mode targets
AM_MAINTAINER_MODE

dnl Demand at least version 2.57 of autoconf.
AC_PREREQ(2.57)

dnl Because we use cwautomacros.
CW_AUTOMACROS

dnl Workaround
true=
AC_SUBST(true)

dnl Check for compiler and preprocessor
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP

# Add --enable-debug (DEBUG, DOXYGEN_DEBUG), --enable-libcwd (CWDEBUG, DOXYGEN_CWDEBUG),
# --enable-optimise and --enable-profile options. Update USE_LIBCWD, LIBS and CXXFLAGS accordingly.
CW_OPG_FLAGS

dnl Checks for other programs.
AC_PROG_INSTALL

# Check if we have graphviz's 'dot'.
AC_PATH_PROG(PROG_DOT, [dot],)
AC_SUBST(PROG_DOT)
HAVE_DOT=NO
if test -n "$PROG_DOT"; then
HAVE_DOT=YES
fi

# Find boost.
CW_BOOST("no", "no", [serialization])

# Used in sys.h to force recompilation.
CW_PROG_CXX_FINGER_PRINTS
CC_FINGER_PRINT="$cw_prog_cc_finger_print"
AC_SUBST([CC_FINGER_PRINT])
CXX_FINGER_PRINT="$cw_prog_cxx_finger_print"
AC_SUBST([CXX_FINGER_PRINT])
CXXCPP_FINGER_PRINT="$cw_prog_cxxcpp_finger_print"
AC_SUBST([CXXCPP_FINGER_PRINT])

dnl Generate sys.h.
AC_CONFIG_FILES(src/timestamp-sys.h:src/sys.h.in, [
cd src
if cmp -s sys.h timestamp-sys.h 2> /dev/null; then
  echo "config.status: sys.h is unchanged"
else
  echo "config.status: creating sys.h"
  cp timestamp-sys.h sys.h
fi
touch timestamp-sys.h
cd ..
])

dnl Output the other configuration files.
AC_CONFIG_FILES(
	[Makefile]
	[src/Makefile]
	[doc/Makefile]
	[doc/doxygen.config])

AC_OUTPUT

