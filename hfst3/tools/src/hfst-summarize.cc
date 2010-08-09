//! @file hfst-summarize.cc
//!
//! @brief Transducer information command line tool
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

#include "HfstTransducer.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

// add tools-specific variables here

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Calculate the properties of a transducer\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    // fprintf(message_out, (tool-specific options and short descriptions)
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    print_report_bugs();
    print_more_info();
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
          // add tool-specific options here 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT,
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream)
{
  size_t transducer_n = 0;
  while (instream.is_good())
    {
      if (transducer_n < 2)
        {
          verbose_printf("Summarizing...\n");
        }
      else
        {
          verbose_printf("Summarizing... %zu\n", transducer_n);
        }
      HfstTransducer trans(instream);
      if (transducer_n > 1)
        {
          fprintf(outfile, "-- \nTransducer #%zu:\n", transducer_n);
        }
      switch (trans.get_type())
        {
        case hfst::SFST_TYPE:
          fprintf(outfile, "format: SFST\n");
          break;
        case hfst::TROPICAL_OFST_TYPE:
          fprintf(outfile, "format: OpenFST tropical weights\n");
          break;
        case hfst::LOG_OFST_TYPE:
          fprintf(outfile, "format: OpenFST logarithmic weights\n");
          break;
        case hfst::FOMA_TYPE:
          fprintf(outfile, "format: Foma\n");
          break;
        case hfst::HFST_OL_TYPE:
          fprintf(outfile, "format: HFST optimized lookup\n");
          break;
        case hfst::HFST_OLW_TYPE:
          fprintf(outfile, "format: HFST optimized lookup with weights\n");
          break;
        default:
          fprintf(outfile, "format: ???\n");
          break;
        }
    }
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstSummarize");
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
    verbose_printf("Reading from %s, writing to %s\n", 
        inputfilename, outfilename);
    // here starts the buffer handling part
    HfstInputStream* instream = NULL;
    try {
        instream = (inputfile != stdin) ?
        new HfstInputStream(inputfilename) : new HfstInputStream();
    } catch (hfst::exceptions::NotTransducerStreamException)  {
        error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
        return EXIT_FAILURE;
    }
    retval = process_stream(*instream);
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

