// cppgraph -- C++ call graph analyzer
//
//! @file debug.h
//! @brief This file contains the declaration of debug related macros, objects and functions.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef DEBUG_H
#define DEBUG_H

#ifndef CWDEBUG

#ifndef DOXYGEN		// No need to document this.  See http://libcwd.sourceforge.net/ for more info.

#include <iostream>
#include <cstdlib>	// std::exit, EXIT_FAILURE

#define AllocTag1(p)
#define AllocTag2(p, desc)
#define AllocTag_dynamic_description(p, x)
#define AllocTag(p, x)
#define Debug(x)
#define Dout(a, b)
#define DoutFatal(a, b) LibcwDoutFatal(::std, , a, b)
#define ForAllDebugChannels(STATEMENT)
#define ForAllDebugObjects(STATEMENT)
#define LibcwDebug(dc_namespace, x)
#define LibcwDout(a, b, c, d)
#define LibcwDoutFatal(a, b, c, d) do { ::std::cerr << d << ::std::endl; ::std::exit(EXIT_FAILURE); } while (1)
#define NEW(x) new x
#define CWDEBUG_ALLOC 0
#define CWDEBUG_MAGIC 0
#define CWDEBUG_LOCATION 0
#define CWDEBUG_LIBBFD 0
#define CWDEBUG_DEBUG 0
#define CWDEBUG_DEBUGOUTPUT 0
#define CWDEBUG_DEBUGM 0
#define CWDEBUG_DEBUGT 0
#define CWDEBUG_MARKER 0
#define DebugIndent(x)

#endif // !DOXYGEN

#include <cassert>
#ifdef DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#else // CWDEBUG

#define ASSERT(x) LIBCWD_ASSERT(x)

#ifndef DEBUGCHANNELS
/*! @brief The namespace in which the \c dc namespace is declared.
 *
 * <A HREF="http://libcwd.sourceforge.net/">Libcwd</A> demands that this macro is defined
 * before <libcwd/debug.h> is included and must be the name of the namespace containing
 * the \c dc (Debug Channels) namespace.
 *
 * @sa debug::channels::dc
 */
#define DEBUGCHANNELS ::debug::channels
#endif
#include <libcwd/debug.h>

//! Debug specific code.
namespace debug {

  void init(void);		// Initialize debugging code, called once from main.
  void init_thread(void);	// Initialize debugging code, called once for each thread.

  /*! @brief Debug Channels (dc) namespace.
   *
   * @sa debug::channels::dc
   */
  namespace channels {	// namespace DEBUGCHANNELS

    //! The namespace containing the actual debug channels.
    namespace dc {
      using namespace libcwd::channels::dc;
      using libcwd::channel_ct;

#ifndef DOXYGEN		// Doxygen bug causes a warning here.
      // Add the declaration of new debug channels here
      // and add their definition in a custom debug.cc file.
      extern channel_ct exceptions;
      extern channel_ct subdirs;
      extern channel_ct dirtree;
      extern channel_ct shortname; 
#endif

    } // namespace dc

  } // namespace DEBUGCHANNELS

  void dump_hex(libcwd::channel_ct const& channel, unsigned char const* buf, size_t size);
#if CWDEBUG_LOCATION
  std::string call_location(void const* return_addr);
#endif

  /*! @brief Interface for marking scopes of invisible memory allocations.
   *
   * Creation of the object does nothing, you have to explicitly call
   * InvisibleAllocations::on.  Destruction of the object automatically
   * cancels any call to \c on of this object.  This makes it exception-
   * (stack unwinding) and recursive-safe.
   */
  struct InvisibleAllocations {
    int M_on;
    InvisibleAllocations() : M_on(0) { }
    ~InvisibleAllocations() { while (M_on > 0) off(); }
    void on(void) { libcwd::set_invisible_on(); ++M_on; }
    void off(void) { assert(M_on > 0); --M_on; libcwd::set_invisible_off(); }
  };

  struct indent {
    int M_in;
    indent(int in) : M_in(in) { Debug(libcw_do.inc_indent(M_in)); }
    ~indent() { Debug(libcw_do.dec_indent(M_in)); }
  };

} // namespace debug

#define DebugIndent(x) debug::indent __debug_indentation(x)

#endif // CWDEBUG

#include "debug_ostream_operators.h"

#endif // DEBUG_H
