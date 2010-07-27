/**
 * @file hfst-commandline.h
 *
 * @brief common parts for all hfst based command line tools
 * This file contains interface specifications and some macros to implement for
 * HFST based tools. There is no common implementation for prototyped functions,
 * rather they must be defined on per program basis. For reference
 * implementation see file @c hfst-*-skeleton.cc.
 */
//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.


#ifndef GUARD_hfst_commandline_h
#define GUARD_hfst_commandline_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif
// most all HFST commandline programs will use both c++ and c io unfortunately
#include <iostream>
#include <cstdio>
#include <cstring>

#include "HfstTransducer.h"

#include "error.h"
#include "progname.h"
#include "xalloc.h"
#include "xprintf.h"
#include "xstrndup.h"
#include "xstrtod.h"
#include "xstrtol.h"

/* These variables should be used in all command line programs.
 * In some cases they may be nonsensical; just define something then.
 */
extern bool verbose;
extern bool silent;
extern bool debug;
extern FILE* message_out;
extern char* hfst_tool_version;
extern char* hfst_tool_wikiname;

/* hfst tools generic helper print functions */

/** save current transducer @c t to file @c filename if debug is @a true. */
void debug_save_transducer(hfst::HfstTransducer t, const char* name);

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
void debug_printf(const char* format, ...);

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
void verbose_printf(const char* format, ...);

/**
 * print non-fatal error message @c s with parameters @c __VA_ARGS__,
 * if @c verbose is @a true.
 * For error messages use error(int,int,char*, ...)
 */
void warning_printf(const char* format, ...);

/** @brief set program's name and other infos for reusable messages defined
 * below. This function must be called in beginning of main as the values are
 * used in all error messages as well.
 */
void hfst_set_program_name(const char* argv0, const char* version,
                           const char* wikipage);
/**
 * @brief print standard usage message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html
 */
void print_usage();

/**
 * @brief print standard version message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
 * @sa http://www.gnu.org/software/womb/gnits/Version-Output.html
 */
void print_version();

/**
 * @brief print standard short help message.
 * 
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html#Help-Output
 */
void print_short_help();

#define KITWIKI_URL "https://kitwiki.csc.fi/twiki/bin/view/KitWiki/"
/**
 * @brief print link to wiki pages.
 */
void print_more_info();

/**
 * @brief print bug reporting message.
 */
void print_report_bugs();

/* command line argument handling */

/** successful return value for argument parsing routine */
#define EXIT_CONTINUE 42

/**
 * @brief define function for parsing whole command line.
 * Each program should define this on its own, you may use includable templates
 * for standard options though.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
 * @sa http://www.gnu.org/software/womb/gnits/File-Arguments.html
 */
int parse_options(int argc, char** argv);

// the versions of standard c library functions with error handling included
/**
 * @brief parse weight from string, or print error message and return zero
 * weight on failure.
 */
double hfst_strtoweight(const char *s);
/**
 * @brief parse number from string, or print error message on failure.
 * if @a infinite is not @c NULL, and value of string is infinity, it will
 * be set to true and sign of infinity is returned.
 */
int hfst_strtonumber(const char *s, bool *infinite);

unsigned long hfst_strtoul(char *s, int base);

#define hfst_strdup xstrdup
#define hfst_strndup xstrndup
#define hfst_malloc xmalloc
#define hfst_calloc xcalloc
#define hfst_realloc xrealloc

/**
 * @brief open file, or print informative error message and exit on failure.
 */
FILE* hfst_fopen(const char* path, const char *mode);

/**
 * @brief seek file, or print informative error message and exit on failure.
 */
void hfst_fseek(FILE* stream, long offset, int whence);

/**
 * @brief tell file position, or print informative error message and exit on
 * failure.
 */
unsigned long hfst_ftell(FILE* stream);

/**
 * @brief safely read one full line from file or print informative error
 * messae and exit on failure.
 */
ssize_t hfst_getline(char** lineptr, size_t* n, FILE* stream);

/**
 * @brief determine if next transducers of given streams are compatible.
 * @return @c SFST_FORMAT, if both streams have SFST transducers available,
 * @c OPENFST_FORMAT, if both streams have Open FST transducers available,
 * @c EOF_FORMAT, if both streams have been fully consumed,
 * @c FORMAT_ERROR otherwise.
 * 
 * @deprecated all formats are compatible in HFST3
 */
int get_compatible_fst_format(std::istream& is1, std::istream& is2);

#endif
// vim: set ft=cpp.doxygen:
