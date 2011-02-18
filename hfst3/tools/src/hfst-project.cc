//! @file hfst-project.cc
//!
//! @brief Transducer projection tool
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

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;


// add tools-specific variables here
bool project_upwards = false;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Project (extract a level) transducer\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    fprintf(message_out, "Projection options:\n"
            "  -p, --project=LEVEL   project extracting tape LEVEL\n");
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "LEVEL must be one of upper, input, first, analysis "
            "or lower, output, second, generation\n");
    fprintf(message_out, "\n");
    print_report_bugs();
    fprintf(message_out, "\n");
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
          {"project", required_argument, 0, 'p'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "p:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }

        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case 'p':
            if ( (strncasecmp(optarg, "upper", 1) == 0) ||
                 (strncasecmp(optarg, "input", 1) == 0) ||
                 (strncasecmp(optarg, "first", 1) == 0) ||
                 (strncasecmp(optarg, "analysis", 1) == 0) )
            {
                project_upwards = true;
            }
            else if ( (strncasecmp(optarg, "lower", 1) == 0) ||
                      (strncasecmp(optarg, "output", 1) == 0) ||
                      (strncasecmp(optarg, "second", 1) == 0) ||
                      (strncasecmp(optarg, "generation", 1) == 0) )
            {
                project_upwards = false;
            }
            else
            {
                error(EXIT_FAILURE, 0,
                      "unknown project direction %s\n"
                      "should be one of upper, input, analysis, first, "
                      "lower, output, second or generation\n",
                      optarg);
                return EXIT_FAILURE;
            }
            break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
{
  //instream.open();
  //outstream.open();
    
    size_t transducer_n=0;
    while(instream.is_good())
    {
        transducer_n++;
        if (transducer_n==1)
        {
          if (project_upwards)
            {
              verbose_printf("Projecting second %s...\n", inputfilename); 
            }
          else
            {
              verbose_printf("Projecting first %s...\n", inputfilename);
            }
        }
        else
        {
          if (project_upwards)
            {
              verbose_printf("Projecting second %s... %zu\n", inputfilename,
                             transducer_n);
            }
          else
            {
              verbose_printf("Projecting first %s... %zu\n", inputfilename,
                             transducer_n);
            }
        }
        
        HfstTransducer trans(instream);
        if (project_upwards)
          {
            outstream << trans.output_project();
          }
        else
          {
            outstream << trans.input_project();
          }
    }
    instream.close();
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstProject");
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

