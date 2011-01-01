//! @file hfst-regexp2fst.cc
//!
//! @brief regular expression compiling command line tool
//!
//! @author HFST Team


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include <iostream>
#include <fstream>

#include <vector>
#include <map>

using std::vector;
using std::pair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "parsers/XreCompiler.h"
#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstTransducer;
using hfst::xre::XreCompiler;

static char *epsilonname=NULL;
static bool disjunct_expressions=false;
static bool line_separated = true;

//static unsigned int sum_of_weights=0;
static bool sum_weights=false;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Compile string pairs and pair-strings into transducer(s)\n"
        "\n", program_name); 
        print_common_program_options(message_out);
        print_common_unary_program_options(message_out); 
        fprintf(message_out, "String and format options:\n"
                "  -f, --format=FMT          Write result in FMT format\n"
                "  -j, --disjunct-strings    Disjunct all strings instead of "
                    "transforming each string into a separate transducer\n"
                "      --sum                 Sum weights of duplicate strings "
                    "instead of taking minimum\n"
                "      --norm                Divide each weight by sum "
                    "of all weights\n"
                "      --log                 Take negative logarithm "
                    "of each weight\n"
                "  -l, --line                Input is line separated\n"
                "  -S, --semicolon           Input is semicolon separated\n"
                "  -e, --epsilon=EPS         Map EPS as zero.\n");
        fprintf(message_out, "\n");

        fprintf(message_out, 
            "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
            "FMT must be name of a format usable by libhfst, such as "
            "openfst-tropical, sfst, foma or hfst-optimized-weighted\n"
            "If EPS is not defined, the default representation of @0@ is used\n"
            );

        fprintf(message_out, "Examples:\n"
            "  echo \"c:d a:o t:g\" | %s -l      create cat:dog fst\n"
            "  echo \"c:d a:o t:g ;\" | %s -S    same as pairstring\n"
            "\n", program_name, program_name);
        print_report_bugs();
        fprintf(message_out, "\n");
        print_more_info();
        fprintf(message_out, "\n");
}

int
parse_options(int argc, char** argv)
{
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          {"disjunct-strings", no_argument, 0, 'j'},
          {"epsilon", required_argument, 0, 'e'},
          {"sum", no_argument, 0, '1'},
          {"norm", no_argument, 0, '2'},
          {"log", no_argument, 0, '3'},
          {"line", no_argument, 0, 'l'},
          {"semicolon", no_argument, 0, 'S'},
          {"format", required_argument, 0, 'f'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "je:123lSf:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
        case '1':
            sum_weights = true;
            break;
        case '2':
            normalize_weights = true;
            break;
        case '3':
            logarithmic_weights = true;
            break;
        case 'j':
            disjunct_expressions = true;
            break;
        case 'S':
            line_separated = false;
            break;
        case 'l':
            line_separated = true;
            break;
        case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    if (output_format == hfst::UNSPECIFIED_TYPE)
      {
        verbose_printf("Output format not specified, "
             "defaulting to openfst tropical\n");
        output_format = hfst::TROPICAL_OFST_TYPE;
      }
    return EXIT_CONTINUE;
}

int
process_stream(HfstOutputStream& outstream)
{
  size_t transducer_n = 0;
  char* line = 0;
  size_t len = 0;
  unsigned int line_count = 0;
  XreCompiler comp(output_format);
  HfstTransducer disjunction(output_format);
  //outstream.open();
  while (hfst_getline(&line, &len, inputfile) != -1)
    {
      transducer_n++;
      line_count++;
      HfstTransducer* compiled;
      if (line_separated)
        {
          verbose_printf("Compiling line %u\n", line_count);
          compiled = comp.compile(line);
        }
      else
        {
          return EXIT_FAILURE;
        }
      if (disjunct_expressions)
        {
          disjunction.disjunct(*compiled);
        }
      else
        {
          outstream << *compiled;
        }
      delete compiled;
    }
  if (disjunct_expressions)
    {
      outstream << disjunction;
    }
  free(line);
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "Regexp2Fst");
  int retval = parse_options(argc, argv);
  if (retval != EXIT_CONTINUE)
    {
      return retval;
    }
  // close buffers, we use streams
  if (outfile != stdout)
    {
      fclose(outfile);
    }
  verbose_printf("Reading from %s, writing to %s\n", 
                 inputfilename, outfilename);
  // here starts the buffer handling part
  HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, output_format) :
        new HfstOutputStream(output_format);
  process_stream(*outstream);
  free(inputfilename);
  free(outfilename);
  return EXIT_SUCCESS;
}

