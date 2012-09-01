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


//! for long options without corresponding short options we use reserved
//! character codes <32.
#define PROFILE_OPT 19
#define COLOUR_OPT 20
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
  {"profile", required_argument, 0, PROFILE_OPT},\
  {"color", required_argument, 0, COLOUR_OPT},\
  {"colour", required_argument, 0, COLOUR_OPT}


//! declaration of standard HFST unary operator options for getopt optstring.
#define HFST_GETOPT_UNARY_SHORT "i:o:"
//! declaration of standard HFST unary operator options for getopt longopts.
#define HFST_GETOPT_UNARY_LONG {"input", required_argument, 0, 'i'},\
  {"output", required_argument, 0, 'o'}

#define HFST_GETOPT_CREATIONAL_SHORT "i:o:f:"
//! declaration of standard HFST creational options for getopt longopts.
#define HFST_GETOPT_CREATIONAL_LONG {"input", required_argument, 0, 'i'},\
  {"output", required_argument, 0, 'o'},\
  {"format", required_argument, 0, 'f'}

//! @brief print usage information snippet for the standard options of a tool
#define HFST_GETOPT_BINARY_SHORT "1:2:o:"
//! declaration of standard HFST binary operator options for getopt longopts.
#define HFST_GETOPT_BINARY_LONG {"input1", required_argument, 0, '1'},\
    {"input2", required_argument, 0, '2'},\
    {"output", required_argument, 0, 'o'}

/**
 * @brief print the usage message part listing program options that @e should
 *  be common to all HFST command-line tools.
 *  You may add a bit of more commonness by using parameters of
 *  @c hfst_init_commandline()
 */
void print_common_program_options();
/**
 * @brief print the parameter instuctionst that @e should be common to all
 * hfst tools. You may add a bit of commonness by using parameters of
 * @c hfst_init_commandline()
 */
void print_common_parameter_instructions();

/**
 * parse the default hfst options from getopt's return value @a c.
 * @return true if @a c was hfst's default option, false otherwise.
 */
bool parse_common_getopt_value(char c);
/**
 * print pretty errors and exit from getopt value @a c when all else has failed.
 */
void parse_getopt_error_value(char c);
/**
 * @brief parse environment for some common hfst options.
 */
void parse_options_getenv();

/**
 * parse rest of the command line for free parameters and check sanity of all
 * options.
 * @note You must call this after getopt loop before @c parse_options_getenv,
 * because it uses getopts global state data.
 */
void check_common_options(int argc, char** argv);

#endif // GUARD_hfst_options_h
// vim: set ft=cpp.doxygen:
