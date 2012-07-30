//! @file hfst-fst2fst.cc
//!
//! @brief Format conversion command line tool
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

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;


// tool-specific variables

ImplementationType output_type = hfst::UNSPECIFIED_TYPE;
bool hfst_format = true;
std::string options = "";

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Convert transducers between binary formats\n"
        "\n", program_name);

    print_common_program_options();
    print_common_creational_program_options();
    fprintf(message_out, "Conversion options:\n"
    "  -b, --use-backend-format          Write result in implementation format, without any HFST wrappers\n");
    fprintf(message_out, "\n");
    print_common_creational_program_parameter_instructions();
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
          HFST_GETOPT_CREATIONAL_LONG,
          // add tool-specific options here 
          {"use-backend-format", no_argument, 0, 'b'},
          {"format",       required_argument, 0, 'f'},
          {"sfst",               no_argument, 0, 'S'},
          {"foma",               no_argument, 0, 'F'},
          {"openfst-tropical",    no_argument, 0, 't'},
          {"openfst-log",         no_argument, 0, 'l'},
          {"optimized-lookup-unweighted",   no_argument, 0, 'O'},
          {"optimized-lookup-weighted",no_argument, 0, 'w'},
          {"quick",              no_argument, 0, 'Q'},
          {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_CREATIONAL_SHORT "SFtlOwQf:b",
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
        case 'f':
          output_type=hfst_parse_format_name(optarg);
          break;
        case 'b':
          hfst_format=false;
          break;
        case 'S':
          output_type = hfst::SFST_TYPE;
          break;
        case 'F':
          output_type = hfst::FOMA_TYPE;
          break;
        case 't':
          output_type = hfst::TROPICAL_OPENFST_TYPE;
          break;
        case 'l':
          output_type = hfst::LOG_OPENFST_TYPE;
          break;
        case 'O':
          output_type = hfst::HFST_OL_TYPE;
          break;
        case 'w':
          output_type = hfst::HFST_OLW_TYPE;
          break;
    case 'Q':
        options = "quick";
        break;
#include "conventions/getopt-cases-error.h"
        }
    }
    
    if (output_type == hfst::UNSPECIFIED_TYPE)
    {
        error(EXIT_FAILURE, 0, 
              "You must specify an output format with -f");
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    return EXIT_CONTINUE;
}
int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
{
  //instream.open();
  //    outstream.open();
    
    size_t transducer_n = 0;
    while(instream.is_good())
    {
        transducer_n++;
        HfstTransducer orig(instream);
        char* inputname = hfst_get_name(orig, inputfilename);
        if (transducer_n == 1)
        {
          verbose_printf("Converting %s...\n", inputname); 
        }
        else
        {
          verbose_printf("Converting %s...%zu\n",
                         inputname, transducer_n);
        }
        try {
            orig.convert(output_type, options);
        } HFST_CATCH(HfstException)
        hfst_set_name(orig, orig, "convert");
        hfst_set_formula(orig, orig, "Id");
        outstream << orig;
    }
    instream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstFst2Fst");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    if (inputfile != stdin)
    {
        fclose(inputfile);
    }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
    char* format_description = hfst_strformat(output_type);
    if (hfst_format) 
      {
        verbose_printf("Writing %s format transducers with HFST3 headers\n",
                       format_description);
      }
    else
      {
        verbose_printf("Writing %s format transducers without HFST specific"
                       " headers\n", format_description);
      }
    free(format_description);
    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try {
      instream = (inputfile != stdin) ?
        new HfstInputStream(inputfilename) : new HfstInputStream();
    } catch(const HfstException e)  {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
        return EXIT_FAILURE;
    }
    HfstOutputStream* outstream = (outfile != stdout) ?
      new HfstOutputStream(outfilename, output_type, hfst_format) :
      new HfstOutputStream(output_type, hfst_format);

    retval = process_stream(*instream, *outstream);
    if (profile_file != 0)
      {
        hfst_print_profile_line();
      }
    delete instream;
    delete outstream;
    free(inputfilename);
    free(outfilename);
    return retval;
}

