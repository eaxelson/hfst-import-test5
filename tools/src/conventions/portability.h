//! @file portability.h
//! @brief declares functions that are known not to exist on all systems, but
//!     are used to implement hfst commandline conventions. Use
//!     portability.cc to get naive reimplementations of some of the
//!     required functions on limited systems such as Mac OS X.

#ifndef GUARD_hfst_portability_h
#define GUARD_hfst_portability_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif

/** 
 * @brief set @c program_name to program's executable name for error messages.
 */
extern char* program_name;
/**
 * @brief set @c program_short_name to program's filename without path part for
 *  error messages.
 */
extern char* program_short_name;

#ifndef HAVE_SET_PROGRAM_NAME
/**
 * @brief set program name representations for error messages from @a argv0
 */
void set_program_name(const char* argv0);
#endif

#ifndef HAVE_ERROR
/** @brief print standard formatted error message and exit if needed */
void error(int status, int error, const char* format, ...);
#endif
#ifndef HAVE_WARNING
/** @brief print standard formatted warning message and exit if needed */
void warning(int status, int error, const char* format , ...);
#endif

#ifndef HAVE_ERROR_AT_LINE
/** 
 * @brief print standard formatted error message when parsing a file and exit
 *        if needed. 
 *        The use of this function is especially important since error
 *        highlighting of vim and emacs depends on this error format.
 */
void error_at_line(int status, int errnum, const char* filename, unsigned int linenum, const char* fmt, ...);
#endif

#ifndef HAVE_STRNDUP
char* strndup(const char* s, size_t n);
#endif

#ifndef HAVE_GETDELIM
ssize_t getdelim(char** lineptr, size_t* n, int delim, FILE* stream);
#endif
#ifndef HAVE_GETLINE
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
#endif

#ifndef HAVE_READLINE
char* readline(const char* prompt);
#endif

#ifndef HAVE_GETRUSAGE
int getrusage(int who, void *usage);
#endif

#endif // GUARD_hfst_portability_h

// vim: set ft=cpp.doxygen:
