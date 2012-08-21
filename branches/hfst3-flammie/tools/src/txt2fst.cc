//! @file hfst-txt2fst.cc
//!
//! @brief Transducer text compiling command line tool
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include <hfst.hpp>

#include "conventions/commandline.h"
#include "conventions/options.h"
#include "conventions/metadata.h"

using hfst::HfstTransducer;
using hfst::HfstOutputStream;

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"
// add tools-specific variables here
static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;
// whether numbers are used instead of symbol names
static bool use_numbers=false; // not used
// printname for epsilon
static char *epsilonname=NULL;
static const unsigned int EPSILON_KEY=0;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Convert AT&T tabular format into a binary transducer\n"
           "\n", program_name);

    print_common_program_options();
    print_common_unary_program_options();
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "Text format options:\n"
            /*"  -n, --number        If numbers are used instead of symbol names\n"*/
            "  -f, --format=FMT    Write result using FMT as backend format\n"
            "  -e, --epsilon=EPS   Interpret string EPS as epsilon\n");
    fprintf(message_out, "\n");
    fprintf(message_out, 
        "If OUTFILE or INFILE is missing or -,"
        "standard streams will be used.\n"
        "If FMT is not given, OpenFst's tropical format will be used.\n"
        "The possible values for FMT are { sfst, openfst-tropical, "
        "openfst-log,\n"
	"foma, optimized-lookup-unweighted, optimized-lookup-weighted }.\n"
        "If EPS is not given, @0@ will be used\n\n"
        );
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
}

int
parse_options(int argc, char** argv)
{
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          // add tool-specific options here
            {"epsilon", required_argument, 0, 'e'},
            {"number", no_argument, 0, 'n'},
            {"format", required_argument, 0, 'f'}, 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "e:nf:",
                             long_options, &option_index);
        if (-1 == c)
        {
break;
        }

        switch (c)
          {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
        case 'n':
            use_numbers = true;
            break;
        case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
#include "conventions/getopt-cases-error.h"
          }
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    if (epsilonname == NULL)
      {
        epsilonname = hfst_strdup("@0@");
        verbose_printf("Using default epsilon representation %s\n", 
                       epsilonname);
      }
    if (output_format == hfst::UNSPECIFIED_TYPE)
      {
        output_format = hfst::TROPICAL_OPENFST_TYPE;
        verbose_printf("Using default output format OpenFst "
                "with tropical weight class\n");
      }

    return EXIT_CONTINUE;
}

int
process_stream(HfstOutputStream& outstream)
{
  size_t transducer_n = 0;
  //outstream.open();
  while (!feof(inputfile))
    {
      transducer_n++;
      if (transducer_n < 2)
        {
          verbose_printf("Reading transducer table...\n");
        }
      else
        {
          verbose_printf("Reading transducer table %zu...\n", transducer_n);
        }
        HfstTransducer t(inputfile,
             output_format,
             std::string(epsilonname));
        hfst_set_name(t, inputfilename, "text");
        hfst_set_formula(t, inputfilename, "T");
        outstream << t;
    }
  outstream.close();
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "HfstTxt2Fst");
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
    switch (output_format)
      {
      case hfst::SFST_TYPE:
        verbose_printf("Using SFST as output handler\n");
        break;
      case hfst::TROPICAL_OPENFST_TYPE:
        verbose_printf("Using OpenFst's tropical weights as output\n");
        break;
      case hfst::LOG_OPENFST_TYPE:
        verbose_printf("Using OpenFst's log weight output\n");
        break;
      case hfst::FOMA_TYPE:
        verbose_printf("Using foma as output handler\n");
        break;
      case hfst::HFST_OL_TYPE:
        verbose_printf("Using optimized lookup output\n");
        break;
      case hfst::HFST_OLW_TYPE:
        verbose_printf("Using optimized lookup weighted output\n");
        break;
      default:
        error(EXIT_FAILURE, 0, "Unknown format cannot be used as output\n");
        return EXIT_FAILURE;
      }
    // here starts the buffer handling part
    HfstOutputStream* outstream = (outfile != stdout) ?
                new HfstOutputStream(outfilename, output_format) :
                new HfstOutputStream(output_format);
    process_stream(*outstream);
    if (profile_file != 0)
      {
        hfst_print_profile_line();
      }
    if (inputfile != stdin)
      {
        fclose(inputfile);
      }
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}
