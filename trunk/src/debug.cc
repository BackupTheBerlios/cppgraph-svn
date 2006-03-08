// cppgraph -- C++ call graph analyzer
//
//! @file debug.cc
//! @brief This file contains the implementation of debug specific code.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#ifdef USE_PCH
#include "pch.h"
#endif

#include "sys.h"                        // Needed for platform-specific code

#ifdef CWDEBUG

#include <cctype>                       // Needed for std::isprint
#include <iomanip>                      // Needed for setfill
#include <map>
#include <string>
#include <sstream>
#include "debug.h"
#ifdef USE_LIBCW
#include <libcw/memleak.h>		// memleak_filter
#endif

namespace debug {
  namespace channels {	// namespace DEBUGCHANNELS
    namespace dc {

#ifndef DOXYGEN
#define DDCN(x) (x)
#endif
      // Add new debug channels here.

      channel_ct exceptions DDCN("EXCEPTIONS");	//!< This debug channel is used for debug output related to exceptions.
      channel_ct subdirs DDCN("SUBDIRS");	//!< This debug channel is used for debug output related to --subdir decoding.
      channel_ct dirtree DDCN("DIRTREE");	//!< This debug channel is used for debug output related to DirTree.
      channel_ct shortname DDCN("SHORTNAME");	//!< This debug channel is used for debug output related to ShortName.
      channel_ct decl DDCN("DECL");		//!< This debug channel is used for debug output related to declaration analysis.
      channel_ct dirvec DDCN("DIRVEC");		//!< This debug channel is used for debug output related to direcctories.

    } // namespace dc
  } // namespace DEBUGCHANNELS

  // Anonymous namespace, this map and its initialization functions are private to this file
  // for Thead-safeness reasons.
  namespace {

    /*!
     * @internal
     * @brief The type of rcfile_dc_states.
     */
    typedef std::map<std::string, bool> rcfile_dc_states_type;

    /*!
     * @internal
     * @brief Map containing the default debug channel states used at the start of each new thread.
     *
     * The first thread calls main, which calls debug::init which will initialize this
     * map with all debug channel labels and whether or not they were turned on in the
     * rcfile or not.
     */
    rcfile_dc_states_type rcfile_dc_states;

    /*!
     * @internal
     * @brief Set the default state of debug channel \a dc_label.
     *
     * This function is called once for each debug channel.
     */
    void set_state(char const* dc_label, bool is_on)
    {
      std::pair<rcfile_dc_states_type::iterator, bool> res =
          rcfile_dc_states.insert(rcfile_dc_states_type::value_type(std::string(dc_label), is_on));
      if (!res.second)
        Dout(dc::warning, "Calling set_state() more than once for the same label!");
      return;
    }

    /*!
     * @internal
     * @brief Save debug channel states.
     *
     * One time initialization function of rcfile_dc_state.
     * This must be called from debug::init after reading the rcfile.
     */
    void save_dc_states(void)
    {
      // We may only call this function once: it reflects the states as stored
      // in the rcfile and that won't change.  Therefore it is not needed to
      // lock `rcfile_dc_states', it is only written to by the first thread
      // (once, via main -> init) when there are no other threads yet.
      static bool second_time = false;
      if (second_time)
      {
        Dout(dc::warning, "Calling save_dc_states() more than once!");
	return;
      }
      second_time = true;
      ForAllDebugChannels( set_state(debugChannel.get_label(), debugChannel.is_on()) );
    }

  } // anonymous namespace

  /*!
   * @internal
   * @brief Returns the the original state of a debug channel.
   *
   * For a given \a dc_label, which must be the exact name (<tt>channel_ct::get_label</tt>) of an
   * existing debug channel, this function returns \c true when the corresponding debug channel was
   * <em>on</eM> at the startup of the application, directly after reading the libcwd runtime
   * configuration file (.libcwdrc).
   *
   * If the label/channel did not exist at the start of the application, it will return \c false
   * (note that libcwd disallows adding debug channels to modules - so this would probably
   * a bug).
   */
  bool is_on_in_rcfile(char const* dc_label)
  {
    rcfile_dc_states_type::const_iterator iter = rcfile_dc_states.find(std::string(dc_label));
    if (iter == rcfile_dc_states.end())
    {
      Dout(dc::warning, "is_on_in_rcfile(\"" << dc_label << "\"): \"" << dc_label << "\" is an unknown label!");
      return false;
    }
    return iter->second;
  }

  /*! @brief Initialize debugging code from new threads.
   *
   * This function needs to be called at the start of each new thread,
   * because a new thread starts in a completely reset state.
   *
   * The function turns on all debug channels that were turned on
   * after reading the rcfile at the start of the application.
   * Furthermore it initializes the debug ostream, its mutex and the
   * margin of the default debug object (Dout).
   */
  void init_thread(void)
  {
    // Turn on all debug channels that are turned on as per rcfile configuration.
    ForAllDebugChannels(
        if (!debugChannel.is_on() && is_on_in_rcfile(debugChannel.get_label()))
	  debugChannel.on();
    );

    // Turn on debug output.
    Debug( libcw_do.on() );
#if LIBCWD_THREAD_SAFE
    Debug( libcw_do.set_ostream(&std::cout, &cout_mutex) );
#else
    Debug( libcw_do.set_ostream(&std::cout) );
#endif

    static bool first_thread = true;
    if (!first_thread)			// So far, the application has only one thread.  So don't add a thread id.
    {
      // Set the thread id in the margin.
      char margin[12];
      sprintf(margin, "%-10lu ", pthread_self());
      Debug( libcw_do.margin().assign(margin, 11) );
    }
  }

  /*! @brief Initialize debugging code from main.
   *
   * This function initializes the debug code.
   */
  void init(void)
  {
#if CWDEBUG_ALLOC && defined(USE_LIBCW)
    // Tell the memory leak detector which parts of the code are
    // expected to leak so that we won't get an alarm for those.
    {
      std::vector<std::pair<std::string, std::string> > hide_list;
      hide_list.push_back(std::pair<std::string, std::string>("libdl.so.2", "_dlerror_run"));
      hide_list.push_back(std::pair<std::string, std::string>("libstdc++.so.6", "__cxa_get_globals"));
      // The following is actually necessary because of a bug in glibc
      // (see http://sources.redhat.com/bugzilla/show_bug.cgi?id=311).
      hide_list.push_back(std::pair<std::string, std::string>("libc.so.6", "dl_open_worker"));
      memleak_filter().hide_functions_matching(hide_list);
    }
    {
      std::vector<std::string> hide_list;
      // Also because of http://sources.redhat.com/bugzilla/show_bug.cgi?id=311
      hide_list.push_back(std::string("ld-linux.so.2"));
      memleak_filter().hide_objectfiles_matching(hide_list);
    }
    memleak_filter().set_flags(libcwd::show_objectfile|libcwd::show_function);
#endif

    // You want this, unless you mix streams output with C output.
    // Read  http://gcc.gnu.org/onlinedocs/libstdc++/27_io/howto.html#8 for an explanation.
    //std::ios::sync_with_stdio(false);

    // This will warn you when you are using header files that do not belong to the
    // shared libcwd object that you linked with.
    Debug( check_configuration() );

    Debug(
      libcw_do.on();		// Show which rcfile we are reading!
      ForAllDebugChannels(
        while (debugChannel.is_on())
	  debugChannel.off()	// Print as little as possible though.
      );
      read_rcfile();		// Put 'silent = on' in the rcfile to suppress most of the output here.
      libcw_do.off()
    );
    save_dc_states();

    init_thread();
  }

  /*! @brief Write a hexadecimal and ascii dump of a buffer to a given debug channel.
   *
   * This function can be used to print a binary block of data of \a size bytes, starting at \a buf.
   * The output is written to debug channel \a channel.
   */
  void dump_hex(libcwd::channel_ct const& channel, unsigned char const* buf, size_t size)
  {
    for (size_t addr = 0; addr < size; addr += 16)
    {
      LibcwDoutScopeBegin(DEBUGCHANNELS, libcwd::libcw_do, channel)
      LibcwDoutStream << std::hex << std::setfill('0') << std::setw(4) << addr << " |";
      int offset;
      for (offset = 0; offset < 16 && addr + offset < size; ++offset)
        LibcwDoutStream << ' ' << std::hex << std::setfill('0') << std::setw(2) << (int)buf[addr + offset];
      for (; offset < 16; ++offset)
        LibcwDoutStream << "   ";
      LibcwDoutStream << " | ";
      for (int offset = 0; offset < 16 && addr + offset < size; ++offset)
      {
	unsigned char c = buf[addr + offset];
	if (!std::isprint(c))
	  c = '.';
        LibcwDoutStream << c;
      }
      LibcwDoutScopeEnd;
    }
  }

#if CWDEBUG_LOCATION
  /*! @brief Return call location.
   *
   * @param return_addr The return address of the call.
   */
  std::string call_location(void const* return_addr)
  {
    libcwd::location_ct loc((char*)return_addr + libcwd::builtin_return_address_offset);
    std::ostringstream convert;
    convert << loc;
    return convert.str();
  }
#endif

} // namespace debug

#endif // CWDEBUG
