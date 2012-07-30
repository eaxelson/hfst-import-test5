//! @file options.cc
//! @brief  implementations of HFST standardised command-line option interfaces.

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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "conventions/commandline.h"
#include "conventions/options.h"

////////////////////////
// NB: when messing with this file please pay attention to GNU guidelines for
// commandline interfaces.
////////////////////////

void print_common_program_options() {

  fprintf(message_out, "Common options:\n"
          "  -h, --help             Print help message\n"  
          "  -V, --version          Print version info\n"  
          "  -v, --verbose          Print verbosely while processing\n"
          "  -q, --quiet            Only print fatal errors and requested "
          "output\n"  
          "  -s, --silent           Alias of --quiet\n"
          "  --profile=PFILE        Write profiling data to PFILE\n"
          "  -d, --debug            Print all debugging data "
          "(for bug reports)\n");
}

void
print_common_unary_program_options() 
  {
    fprintf(message_out, "Input/Output options:\n"
        "  -i, --input=INFILE     Read input transducer from INFILE\n"
        "  -o, --output=OUTFILE   Write output transducer to OUTFILE\n");
  }

void
print_common_unary_program_parameter_instructions() 
  {

    fprintf(message_out, 
      "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
      "Format of result depends on format of INFILE\n");

  }

void
print_common_binary_program_options() 
  {

    fprintf(message_out, "Input/Output options:\n"
          "  -1, --input1=INFILE1   Read first input transducer from INFILE1\n"
          "  -2, --input2=INFILE2   Read second input transducer from INFILE2\n"
          "  -o, --output=OUTFILE   Write results to OUTFILE\n");
  }

void
print_common_binary_program_parameter_instructions() 
  {
    fprintf(message_out, 
        "If OUTFILE, or either INFILE1 or INFILE2 is missing or -,\n"
        "standard streams will be used.\n"
        "INFILE1, INFILE2, or both, must be specified.\n"
        "Format of result depends on format of INFILE1\n"
        "and INFILE2; both should have the same format.\n");
  }

void print_common_creational_program_options() 
  {
    fprintf(message_out, "Input/Output options:\n"
          "  -f, --format=FMT       Create automaton using FMT library\n"
          "  -i, --input=INFILE     Read input data from INFILE\n"
          "  -o, --output=OUTFILE   Write results to OUTFILE\n");
  }

void
print_common_creational_program_parameter_instructions() 
  {

    fprintf(message_out, 
      "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
      "If FMT is not given, "
#   if HAVE_OPENFST
      "OpenFst's tropical format"
#   elif HAVE_SFST
      "SFST format"
#   elif HAVE_FOMA
      "foma format"
#   else
      "HFST internal format"
#   endif
      " will be used.\n"
      "The avalaible values for FMT are {"
#   if HAVE_OPENFST
      "openfst-tropical, openfst-log, "
#   endif
#   if HAVE_SFST
      "sfst, "
#   endif
#   if HAVE_FOMA
      "foma, "
#   endif
      "optimized-lookup-weighted, and optimized-lookup-unweighted }.\n");
  }

