/**
 * @file hfst-commandline.h
 *
 * @brief common parts for all hfst based command line tools
 * This file contains interface specifications and some macros to implement for
 * HFST based tools. There is no common implementation for prototyped functions,
 * rather they must be defined on per program basis. For reference
 * implementation see file @c hfst-skeleton.cc.
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


/* These variables should be used in all command line programs.
 * In some cases they may be nonsensical; just define something then.
 */

#ifndef GUARD_hfst_commandline_h
#define GUARD_hfst_commandline_h

#include <iostream>

#include <cstdio>
#include <cstring>

/** whether building reading or building weighted or unweighted */
extern bool weighted;
/** whether printing verbosely */
extern bool verbose;
/** whether printing debug messages and saving intermediate results */
extern bool debug ;
/**
 * destination for verbose and debug messages
 */
extern FILE *message_out;


#if DEBUG
/** save current transducer @c t to file @c filename if debug is @a true. */
#define DEBUG_SAVE(t, filename) \
	do {\
		if (debug) \
		{\
			char* fn = strdup(filename);\
			fprintf(message_out, "*** DEBUG: saving current transducer to %s\n", filename);\
			if (weighted) \
			{\
				HWFST::write_to_file(t, fn);\
			}\
			else \
			{\
				HFST::write_to_file(t, fn);\
			}\
			free(fn);\
		}\
	} while (0);\

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
#define DEBUG_PRINT(s, ...) \
	do {\
		if (debug) \
		{\
			fprintf(message_out, s , ##__VA_ARGS__);\
		}\
	} while (0);\

#else
#define DEBUG_SAVE(t, filename) 
#define DEBUG_PRINT(s, ...) 
#endif

/** print message @c s with parameters @c __VA_ARGS__ if debug is @a true. */
#define VERBOSE_PRINT(s, ...) \
	do {\
		if (verbose) \
		{\
			fprintf(message_out, s , ##__VA_ARGS__);\
		}\
	} while (0);\

/** successful return value for argument parsing routine */
#define EXIT_CONTINUE 42

/** Input for reading transducer has EOF. */
#define EOF_FORMAT -1
/** Input for reading transducer has SFST signature. */ 
#define SFST_FORMAT 0
/** Input for reading transducer has OpenFST signature. */
#define OPENFST_FORMAT 1
/** Other error reading transducer from input. */
#define FORMAT_ERROR -2

/**
 * @brief print standard usage message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html
 */
void print_usage(const char *program_name);

/**
 * @brief print standard version message.
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
 * @sa http://www.gnu.org/software/womb/gnits/Version-Output.html
 */
void print_version(const char *program_name);

/**
 * @brief print standard short help message.
 * 
 * @sa http://www.gnu.org/software/womb/gnits/Help-Output.html#Help-Output
 */
void print_short_help(const char *program_name);

/**
 * @brief define function for parsing whole command line.
 * 
 *
 *
 * @sa http://www.gnu.org/prep/standards/standards.html#Command_002dLine-Interfaces
 * @sa http://www.gnu.org/software/womb/gnits/File-Arguments.html
 */
int parse_options(int argc, char** argv);

/**
 * @brief duplicate string, or print error message and exit on failure.
 */
char* hfst_strdup(const char* s);

/**
 * @brief duplicate n first characters of string, or print error message and
 * exit on failure.
 */
char* hfst_strndup(const char* s, size_t n);

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
 * @brief allocate memory, or print informative error message and exit on
 * failure.
 */
void* hfst_malloc(size_t size);

/**
 * @brief allocate and zero memory, or print informative error message and exit
 * on failure.
 */
void* hfst_calloc(size_t nmemb, size_t size);

/**
 * @brief resize allocation, or print informative error message and exit on
 * failure.
 */
void* hfst_realloc(void* ptr, size_t size);

/**
 * @brief determine if next transducers of given streams are compatible.
 * @return @c SFST_FORMAT, if both streams have SFST transducers available,
 * @c OPENFST_FORMAT, if both streams have Open FST transducers available,
 * @c EOF_FORMAT, if both streams have been fully consumed,
 * @c FORMAT_ERROR otherwise.
 */
int get_compatible_fst_format(std::istream& is1, std::istream& is2);

#endif
// vim: set ft=cpp.doxygen:
