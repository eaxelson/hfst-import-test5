/**
 * @file commandline.h
 *
 * @brief common practices for HFST command-line tools.
 * This file contains macros and declarations for variables and functions that
 * should be used in all command-line programs in HFST.
 *  You might think that implementing command-line tools with lot of global
 *  variables is nasty or whatnot, but in fact it is quite reasonable model
 *  for a simple command-line tool that needs to hold those certain pieces of
 *  data available throughout the program, there's no need to pass the file
 *  handles for every single function call and no point in encapsulating this
 *  all to an object that could never be more than a singleton.
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
#include "pretty-printing.h"
#include "io-handling.h"
#include "profiling.h"
#include "options.h"
#include "metadata.h"
#include "HfstDataTypes.h"




/**
 * @brief set locale according to environment if UTF-8-capable or
 * print informative error message and exit on failure. This function should be
 * called in the beginning of all command-line tools that deal with corpora,
 * analysing and other linguistic data.
 */
char* hfst_setlocale();

/**
 * @brief initialise a command line program with full capabilities.
 * This command calls both @c hfst_set_program_name and @c hfst_setlocale,
 * as well as ncurses and gettext initialisation. If you want to create
 * a command line program you need to call these parts individually in the
 * very beginning of the @c main function. The @a hti determines what tool
 * does with its file arguments and @a hic determines how many inputs will
 * it accept.
 * @sa hfst_set_program_name(const char*, const char*, const char*)
 */
void hfst_init_commandline(const char* argv0, const char* version,
                           const char* wikipage,
                           hfst_tool_io hti, hfst_input_count hic);

/**
 * @brief on successful exit, free all globals and destroy everything.
 */
void hfst_uninit_commandline();

/* hfst tools generic helper print functions */

/** save current transducer @c t to file @c filename if @c debug is @c true. */
void debug_save_transducer(hfst::HfstTransducer t, const char* name);

/* command line argument handling */

/**
 * @brief define function for parsing whole command line.
 * Each program should define this on its own, although most will
 * manage with minimal standard getopt loop and cases from options header
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
 * @sa http://www.gnu.org/software/womb/gnits/File-Arguments.html
 */
void parse_options(int argc, char** argv);



/* hfst versions of other standard functions with error handling */
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

#endif
// vim: set ft=cpp.doxygen:
