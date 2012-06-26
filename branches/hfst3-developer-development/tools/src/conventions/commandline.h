/**
 * @file commandline.h
 *
 * @brief common practices for HFST command-line tools.
 * This file contains macros and declarations for variables and functions that
 * should be used in all command-line programs in HFST. Using same 
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

#include <cstdio>
#include <cstring>

#if HAVE_ERROR_H
#  include <error.h>
#endif

#include "portability.h"
#include "HfstDataTypes.h"


/* These variables should be used in all command line programs.
 * In some cases they may be nonsensical; just define something then.
 */
/** 
 * @brief set @c verbose when program should print before and after every
 *        non-trivial step it takes.
 */
extern bool verbose;
/** @brief set @c silent when program should not print anything at all. */
extern bool silent;
/** @brief set @c debug when program should dump all the intermediate results
 *         to terminal and/or save them to files in @c CWD.
 */
extern bool debug;
/** @brief set @c message_out to stream that is usable for non-error message
 *         print-outs.
 *         This @e should be stdout in all cases, except when transducer 
 *         binaries are being transmitted through @c stdout. Some programs 
 *         @e may have option to log these messages to a file instead.
 */
extern FILE* message_out;
/** 
 *  @brief set @a hfst_tool_version to version specific to the tool.
 *  @sa hfst_set_program_name
 */
extern char* hfst_tool_version;
/** 
 * @brief set @a hfst_tool_wikiname to name of the kitwiki page for this tool.
 */
extern char* hfst_tool_wikiname;
/**
 * @brief set @a profile_fle to target of profiling info writes
 */
extern FILE* profile_file;
/**
 * @brief set @a profile_start to @c clock() when starting profiling.
 */
extern clock_t profile_start;
/* hfst tools generic helper print functions */

/** save current transducer @c t to file @c filename if debug is @a true. */
void debug_save_transducer(hfst::HfstTransducer t, const char* name);

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
void debug_printf(const char* format, ...);

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
void verbose_printf(const char* format, ...);

/** 
 * @brief set program's name and other infos for reusable messages defined
 * below. This function must be called in beginning of main as the values are
 * used in all error messages as well.
 */
void hfst_set_program_name(const char* argv0, const char* version,
                           const char* wikipage);

/**
 * @brief set locale according to environment if UTF-8-capable or
 * print informative error message and exit on failure. This function should be
 * called in the beginning of all command-line tools that deal with corpora,
 * analysing and other linguistic data.
 */
char* hfst_setlocale();

/**
 *
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

/**
 * @brief extend the options in argv by parsing standard hfst environment
 *        variables.
 */
void extend_options_getenv(int* argc, char*** argv);

/**
 * @brief parse weight from string, or print error message and return zero
 * weight on failure.
 */
double hfst_strtoweight(const char *s);

/**
 * @brief parse number from string, or print error message on failure.
 * if @a infinite is not @c NULL, and value of string is infinity, it will
 * be set to true and sign of infinity is returned. IF @a infinite is @c NULL,
 * infinite value in string will print error message and exit.
 */
int hfst_strtonumber(const char *s, bool *infinite);

/**
 * @brief parse an unsigned long from @a s in base @a base, or print error
 * message on failure.
 */
unsigned long hfst_strtoul(char *s, int base);

/**
 * @brief parse @c long from string @a s or print error message on failure.
 */
long hfst_strtol(char* s, int base);


/** @brief parse string naming transducer format @c s or exit.
 */
hfst::ImplementationType hfst_parse_format_name(const char* s);

/** @brief allocate new string describing type of transducer format */
char* hfst_strformat(hfst::ImplementationType format);

/** @brief duplicate substring and exit cleanly on error */
char* hfst_strndup(const char* s, size_t n);

/** @brief duplicate string and exit cleanly on error */
char* hfst_strdup(const char* s);

/** @brief allocate memory and exit cleanly on error */
void* hfst_malloc(size_t size);

/** @brief allocate memory to zero and exit cleanly on error */
void* hfst_calloc(size_t nmemb, size_t size);

/** @brief reallocate memory and exit cleanly on error */
void* hfst_realloc(void* ptr, size_t size);

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
 * @brief read at most @a nmemb elements of @a size or print informative
 *        error message and exit.
 */
size_t hfst_fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
/**
 * @brief write at most @a nmemb elements of @a size or print informative
 *        error message end exit.
 */
size_t hfst_fwrite(void* ptr, size_t size, size_t nmemb, FILE* stream);
/** 
 * @brief create temporary file or print informative error message and exit
 *        on failure.
 */
FILE* hfst_tmpfile();

// same stuff for fd's
/** @brief close a file descriptor or print informative error message and exit
 *         on failure.
 */
int hfst_close(int fd);
/** @brief open a file descriptor or print informative error message and exit
 *         on failure.
 */
int hfst_open(const char* pathname, int flags);
/** @brief read at most @a count bytes from the file descriptor or print
 *         error message and exit on failure.
 */
ssize_t hfst_read(int fd, void* bug, size_t count);
/** @brief write at most @a count bytes to file descriptor or print error
 *         message and exit on failure
 */
ssize_t hfst_write(int fd, const void* buf, size_t count);
/**
 * @brief create temporary file from template name or print informative error
 *        message and exit on failrue.
 */
int hfst_mkstemp(char* templ);

/**
 * @brief remove a file or print an informative error message and exit on
 *        failure.
 */
int hfst_remove(const char* filename);

/**
 * @brief safely read one @a delim delimited char array or print informative
 * error message and exit on failure.
 */
ssize_t hfst_getdelim(char** lineptr, size_t* n, int delim, FILE* stream);

/**
 * @brief safely read one full line from file or print informative error
 * messae and exit on failure.
 */
ssize_t hfst_getline(char** lineptr, size_t* n, FILE* stream);

/**
 * @brief read one line of interactive input or print informative error
 *        message adn exit on failure.
 */
char* hfst_readline(const char* prompt);

/**
 * @brief print profiling results to @a stream in neat TSV format.
 *  The results of the profiling match the fields of struct rusage from
 *  sys/resource.h, except for first field which is accumulated @c clock() time.
 */
void hfst_print_profile_line();


#endif
// vim: set ft=cpp.doxygen:
