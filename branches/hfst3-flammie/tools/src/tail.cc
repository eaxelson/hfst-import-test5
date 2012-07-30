//! @file hfst-tail.cc
//!
//! @brief Transducer archive tailing tool
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
#include <queue>

using std::queue;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include <hfst.hpp>

#include "conventions/commandline.h"
#include "conventions/options.h"

#include "conventions/globals-common.h"
#include "conventions/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;


// add tools-specific variables here
long tail_count = -1;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Get last transducers from an archive\n"
        "\n", program_name);

    print_common_program_options();
    print_common_unary_program_options();
    fprintf(message_out, "Archive options:\n"
            "  -n, --n-last=[+]K   Print the last K transducers;\n"
            "                      use +K to print transducers starting from "
            "the Kth\n");
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions();
    fprintf(message_out, "K must be an integer, as parsed by "
            "strtoul base 10, and not 0.\n"
            "if K is omitted, it defaults to +1 (all except the first)\n");
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
          {"n-last", required_argument, 0, 'n'},
          // add tool-specific options here 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "n:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "conventions/getopt-cases-common.h"
#include "conventions/getopt-cases-unary.h"
        case 'n':
          if (*optarg == '+')
            {
              // swap sign haha lol
              tail_count = -hfst_strtol(optarg, 10);
            }
          else
            {
              tail_count = hfst_strtol(optarg, 10);
            }
          break;
#include "conventions/getopt-cases-error.h"
        }
    }

#include "conventions/check-params-common.h"
#include "conventions/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
  {
    queue<HfstTransducer> last_n;
    size_t transducer_n=0;
    if (tail_count > 0)
      {
        verbose_printf("Counting last %zu transducers...\n", tail_count);
        while (instream.is_good())
          {
            transducer_n++;
            HfstTransducer trans(instream);
            last_n.push(trans);
            if (last_n.size() > tail_count)
              {
                last_n.pop();
              }
        }
        if (tail_count < transducer_n)
          {
            transducer_n -= (tail_count + 1);
          }
        else
          {
            transducer_n = 0;
          }
        while (!last_n.empty())
          {
            transducer_n++;
            verbose_printf("Forwarding %s...%zu\n", inputfilename, transducer_n);
            outstream << last_n.front();
            last_n.pop();
          }
      }
    else if (tail_count < 0)
      {
        verbose_printf("Skipping %zu transducers...\n", -tail_count);
        while (instream.is_good())
          {
            transducer_n++;
            HfstTransducer trans(instream);
            if (transducer_n >= -tail_count)
              {
                verbose_printf("Forwarding %s...%zu\n", inputfilename, transducer_n);
                outstream << trans;
              }
          }
      }
    instream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.2", "HfstTail");
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
        new HfstOutputStream(outfilename, instream->get_type()) :
        new HfstOutputStream(instream->get_type());
    
    retval = process_stream(*instream, *outstream);
    delete instream;
    delete outstream;
    free(inputfilename);
    free(outfilename);
    return retval;
}
