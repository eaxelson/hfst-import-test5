//! @file portability.h
//! @brief declares portable versions of certain functions that may not exist
//!     in non-GNU environments.

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


