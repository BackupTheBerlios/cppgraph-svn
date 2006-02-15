// cppgraph -- C++ call graph analyzer
//
//! @file exceptions.h
//! @brief This file contains the declaration of class exceptions.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "debug.h"
#include <stdexcept>		// std::runtime_error
#include <sstream>		// std::ostringstream

// The following design consideration have been made.
// * We can run out of memory about anywhere - when that happens
//   then probably nothing works anymore until we freed a little
//   memory - but the out-of-memory is not likely to be caused
//   by this application, but instead by another process that is
//   trying to allocate as much memory as possible;  therefore,
//   if we free memory - it would be eaten by the other process
//   and things would still not work.
//   Therefore we do not care much about std::bad_alloc - when
//   that exception is thrown then the application will terminate;
//   we give up.
// * That being said, if any other exception is thrown then it is
//   very unlikely that at the same moment we ALSO run out of memory.
//   Therefore we don't bother to be precious with memory in our
//   exceptions: memory is not of a concern at that moment.
// * As one can read in any C++ text book:
//   - Destructors should never throw.
//   - Exceptions should be caught by (const) reference.
//   - Exception classes should use virtual base classes.

/*! @brief Exception class used to cleanly exit the application.
 *
 * This exception is caught before leaving main().  Stack unwinding
 * will cause clean up of any objects on the stack.  When the exception
 * is caught any urgent pending business will be handled before actually
 * leaving main().
 */
class clean_exit : public std::exception {
private:
  int M_exit_code;
public:
  clean_exit(void) throw() : M_exit_code(0) { }
  clean_exit(int exit_code) throw() : M_exit_code(exit_code) { }
  virtual ~clean_exit() throw() { }
  clean_exit& operator=(clean_exit const& right) throw() { M_exit_code = right.M_exit_code; return *this; }
  int exit_code(void) const throw() { return M_exit_code; }
  virtual char const* what(void) const throw() { return "clean exit"; }
};

class no_cgd_files : public std::exception {
public:
  no_cgd_files(void) throw() { }
  ~no_cgd_files() throw() { }
};

namespace edragon {

// Declaration, specifying the noreturn attribute.
// "Indeed, 'throw' is a noreturn function" -- Richard Henderson, 2004/05/07.
// According to this, and other gcc developers, specifying that a function does NOT throw
// speeds up the code a little bit - however, there is no difference between specifying
// what a function throws and specifying nothing; except when then the function indeed
// throws at runtime, in which case it is slower because the code has to check if the
// thrown exception is one of the expected ones.
// Therefore it is best to not specify that this function throws `exception'.
template<class E>
  void throw_exception(std::string const& debug_msg, E const& exception) __attribute__ ((noreturn));

#if defined(CWDEBUG) || defined(DOXYGEN)
/*!
 * @internal
 * @brief Throw an exception.
 *
 * @param exception The exception to be thrown.
 * @param debug_msg A simple string for debugging purposes, to clarify the reason why the exception is thrown.
 *
 * Debug output has the form:
 *
 * EXCEPTIONS: &lt;debug_msg&gt;, throwing exception "<exception.what()>".
 *
 * Don't use this function directly, instead use the macro THROW_EXCEPTION.
 */
template<class E>
  void throw_exception(E const& exception, std::string const& debug_msg)
  {
#if CWDEBUG_LOCATION
    libcwd::location_ct loc((char*)__builtin_return_address(0) + libcwd::builtin_return_address_offset);
#else
    void* loc = (char*)__builtin_return_address(0) + libcwd::builtin_return_address_offset;
#endif
    Dout(dc::exceptions, loc << ": " << debug_msg << ", throwing exception \"" << exception.what() << "\".");
    throw exception; 
  }

/*! @brief Throw an exception.
 *
 * Always use this macro to throw an exception in eDragon.
 * @param exception The exception to be thrown.
 * @param debug_msg A simple string for debugging purposes, to clarify the reason why the exception is thrown.
 *
 * Debug output has the form:
 *
 * EXCEPTIONS: [&lt;addr location&gt;: ]&lt;debug_msg&gt;, throwing exception "<exception.what()>".
 */
#define THROW_EXCEPTION(exception, debug_msg...) \
    do { \
      std::ostringstream __ssbuf; \
      __ssbuf << debug_msg; \
      edragon::throw_exception(exception, __ssbuf.str()); \
    } while (0)
#else
#define THROW_EXCEPTION(exception, debug_msg...) \
    do { throw exception; } while (0)
#endif

#ifdef CWDEBUG
template<class E>
  void caught(E const& exception)
  {
#if CWDEBUG_LOCATION
    libcwd::location_ct loc((char*)__builtin_return_address(0) + libcwd::builtin_return_address_offset);
#else
    void* loc = (char*)__builtin_return_address(0) + libcwd::builtin_return_address_offset;
#endif
    Dout(dc::exceptions, loc << ": caught exception \"" << exception.what() << "\".");
  }
#endif

}

#endif // EXCEPTIONS_H
