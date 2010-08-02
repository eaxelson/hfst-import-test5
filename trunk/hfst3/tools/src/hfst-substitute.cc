//! @file hfst-substitute.cc
//!
//! @brief Transducer label modification
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

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

static char* from_label = 0;
static char* from_file_name = 0;
static FILE* from_file = 0;
static char* to_label = 0;
static char* to_transducer_filename = 0;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Relabel transducer arcs\n"
        "\n", program_name);

    print_common_program_options(message_out);
    print_common_unary_program_options(message_out);
    fprintf(message_out, "Relabeling options:\n"
            "  -f, --from-label=LABEL     replace LABEL\n"
            "  -t, --to-label=LABEL       replace with LABEL\n"
            "  -T, --to-transducer=FILE   replace with transducer "
            "read from FILE\n"
           );
    fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "LABEL must be a symbol name of single arc in "
            "transducer\n");
    /*fprintf(message_out,
           "\n"
           "Examples:\n"
           "  %s -o deterministic.hfst indeterministic.hfst  determinizes "
           "indeterministic.hfst \n"
           "         writing results to deterministic.hfst\n"
           "\n", program_name);*/
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
            {"from-label", required_argument, 0, 'f'},
            {"from-file", required_argument, 0, 'F'},
            {"to-label", required_argument, 0, 't'},
            {"to-transducer", required_argument, 0, 'T'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "f:F:t:T:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        FILE* f = 0;
        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
          // add tool-specific cases here
        case 'f':
            from_label = hfst_strdup(optarg);
            break;
        case 'F':
            from_file_name = hfst_strdup(optarg);
            from_file = hfst_fopen(from_file_name, "r");
            if (from_file == NULL)
            {
                return EXIT_FAILURE;
            }
            break;
        case 't':
            to_label = hfst_strdup(optarg);
            break;
        case 'T':
            to_transducer_filename = hfst_strdup(optarg);
            f = hfst_fopen(to_transducer_filename, "r");
            if (f == NULL)
            {
                return EXIT_FAILURE;
            }
            fclose(f);
            break;
#include "inc/getopt-cases-error.h"
        }
    }
    
    if ((from_label == 0) && (from_file_name == 0))
    {
        error(EXIT_FAILURE, 0,
              "Must state name of labels to rewrite with -f or -F");
        return EXIT_FAILURE;
    }
    if ((to_label == 0) && (to_transducer_filename == 0) && 
            (from_file_name == 0))
    {
        error(EXIT_FAILURE, 0,
              "Must give target labels with -t, -T or -F");
        return EXIT_FAILURE;
    }
#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}

int
process_stream()
{
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "HfstSubstitute");
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
    free(inputfilename);
    free(outfilename);
    return EXIT_SUCCESS;
}

