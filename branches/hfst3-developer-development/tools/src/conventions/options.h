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
//

#ifndef GUARD_hfst_options_h
#define GUARD_hfst_options_h 1

#include <cstdio>

// all programs obey:

/**
 * @brief print the usage message part listing program options that @e should
 *  be common to all HFST command-line tools.
 *  It is very important that all HFST command-line tools have at least these
 *  options and print out this message in the beginning of the usage.
 */
void print_common_program_options();

//! for long options without corresponding short options we use reserved
//! character codes <32.
#define PROFILE_OPT 19
//! declaration of standard HFST options for getopt optstring. Must be used at
//!     the beginning of the optstring.
#define HFST_GETOPT_COMMON_SHORT ":hVvqsd"
//! declaration of standard HFST options for getopt longopts.
#define HFST_GETOPT_COMMON_LONG  {"help", no_argument, 0, 'h'},\
  {"version", no_argument, 0, 'V'},\
  {"verbose", no_argument, 0, 'v'},\
  {"quiet", no_argument, 0, 'q'},\
  {"silent", no_argument, 0, 's'},\
  {"debug", no_argument, 0, 'd'},\
  {"profile", required_argument, 0, PROFILE_OPT}


// unary fst-to-fst programs:

//! @brief print usage information snippet for the standard options of a tool
//!     of unary operator or equivalent. This message concerns the use of
//!     input and output parameters.
void print_common_unary_program_options();
//! @brief print standard usage information on how getopt handles missing or
//!     optional FILENAMEs in unary tools.
void print_common_unary_program_parameter_instructions();
//! declaration of standard HFST unary operator options for getopt optstring.
#define HFST_GETOPT_UNARY_SHORT "i:o:"
//! declaration of standard HFST unary operator options for getopt longopts.
#define HFST_GETOPT_UNARY_LONG {"input", required_argument, 0, 'i'},\
  {"output", required_argument, 0, 'o'}

// creational text-to-fst programs:

//! @brief print usage information snippet for the standard options of a tool
//!     creating automata from text data. This message concerns selecting the
//!     transducer format and input/output parameters.
void print_common_creational_program_options();
//! @brief print standard usage instructions on available transducer format
//!     names and how getopt handles missing or optional FILENAMEs in creational
//!     tools.
void print_common_creational_program_parameter_instructions();
//! declaration of standard HFST creational options for getopt optstring.
#define HFST_GETOPT_CREATIONAL_SHORT "i:o:f:"
//! declaration of standard HFST creational options for getopt longopts.
#define HFST_GETOPT_CREATIONAL_LONG {"input", required_argument, 0, 'i'},\
  {"output", required_argument, 0, 'o'},\
  {"format", required_argument, 0, 'f'}

// binary fst+fst-to-fst programs:

//! @brief print usage information snippet for the standard options of a tool
//!     of binary operator or equivalent. This message concerns on the use of
//!     two input parameters
void print_common_binary_program_options();
//! @brief print standard usage instructions on how getopt handles missing
//!     or optional FILENAMEs in binary tools
void print_common_binary_program_parameter_instructions();
//! declaration of standard HFST binary options for getopt optstring.
#define HFST_GETOPT_BINARY_SHORT "1:2:o:"
//! declaration of standard HFST binary operator options for getopt longopts.
#define HFST_GETOPT_BINARY_LONG {"input1", required_argument, 0, '1'},\
    {"input2", required_argument, 0, '2'},\
    {"output", required_argument, 0, 'o'}

#endif // GUARD_hfst_options_h
// vim: set ft=cpp.doxygen:
