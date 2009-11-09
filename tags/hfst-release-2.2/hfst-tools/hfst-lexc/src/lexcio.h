//! @file lexcio.h
//!
//! @brief Output formatting and printing for hfst-lexc.
//! This file contains C style char arrays because I find printf style
//! a lot neater than c++ streams when formatting complex messages.
//! 
//! @author Tommi A. Pirinen


//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GUARD_hlexc_output_h
#define GUARD_hlexc_output_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef ENABLE_NLS
#  ifdef HAVE_LIBINTL_H
#    include <libintl.h>
#  endif
#else
#  define gettext(s) (s)
#endif
#define _(s) gettext(s)

#include <ctime>
#include <map>

#include "xducer.h"
#include "xymbol.h"

extern bool weighted;
extern unsigned int verbosity;
extern FILE* message_out;

extern std::map<std::string, clock_t> timings_accumulated;
extern std::map<std::string, clock_t> timings_starts;
extern std::map<std::string, clock_t> timings_ends;

#define PRINT_UNQUIET 1
#define PRINT_VERBOSE 2
#define PRINT_DEBUG 4
#define PRINT_TIMING 8
#define PRINT_QUIET 16
#define PRINT_XEROXLIKE 32
#define PRINT_TIMING_MORE 64
#define PRINT_USUALLY (PRINT_UNQUIET | PRINT_VERBOSE | PRINT_DEBUG)
#define PRINT_ALWAYS (PRINT_UNQUIET | PRINT_VERBOSE | PRINT_DEBUG | PRINT_TIMING | PRINT_QUIET)
#define PRINT_WARNING_LEADER 256
#define PRINT_ERROR_LEADER 512
#define PRINT_WARNING (PRINT_USUALLY | PRINT_WARNING_LEADER)
#define PRINT_ERROR (PRINT_ALWAYS | PRINT_ERROR_LEADER)

//! @brief Check if path is writable as file w/o creating it
bool lexc_writable(const char *path);
//! @brief Fopen with error checking
FILE* lexc_fopen(const char *path, const char *mode);

//! @brief print xducer 
bool lexc_xducer_printf(const Xducer& x, const char* format, ...);

//! @brief print xymbol set
bool lexc_xymbol_set_printf(const XymbolSet& xs, const char* format, ...);

//! @brief Print neatly formatted message if verbosity level is enough
bool lexc_printf(unsigned int level, unsigned int id,
		const char* format, ...);

//! @brief Print neatly formatted message with parser state if verbosity level
//! matches.
bool lexc_parser_printf(unsigned int level, unsigned int id,
		const char* format, ...);

//! @brief Create timer by name @a timer.
void lexc_timer_initialize(const char* timer);

//! @brief Start timer @a timer.
void lexc_timer_start(const char* timer);

//! @brief End timer @a timer and add the time to totals.
void lexc_timer_end(const char* timer);

//! @brief Print formatted time of @a timer. 
bool lexc_timing_printf(const char* timer, const char* format, ...);

//! @brief Start printing a neatly formatted list of items.
bool lexc_print_list_start(const char *text);

//! @brief Print neatly formatted item to a list.
bool lexc_list_printf(const char *format, ...);

//! @brief End current neatly formatted list of items.
bool lexc_print_list_end(const char *text);


#endif
// vim: set ft=cpp.doxygen:
