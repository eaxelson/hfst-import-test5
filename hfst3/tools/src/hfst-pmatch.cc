//! @file hfst-pmatch.cc
//!
//! @brief Pmatch utility for continuous matching on input streams
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
#include <string>

using std::string;
using std::vector;
using std::pair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "hfst-tool-metadata.h"
#include "implementations/optimized-lookup/pmatch.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

bool blankline_separated = true;
std::string pmatch_filename;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] TRANSDUCER\n"
            "perform matching/lookup on text streams\n"
            "\n", program_name);
    print_common_program_options(message_out);
    fprintf(message_out,
            "  -n  --newline          Newline as input separator (default is blank line)\n");
    fprintf(message_out, 
            "Use standard streams for input and output.\n"
            "\n"
        );
    
    print_report_bugs();
    fprintf(message_out, "\n");
    print_more_info();
    fprintf(message_out, "\n");
}

int process_input(hfst_ol::PmatchContainer & container,
                  std::ostream & outstream)
{
    std::string input_text;
    char * line = NULL;
    size_t len = 0;
    while (hfst_getline(&line, &len, inputfile) > 0) {
        if (!blankline_separated) {
            // newline separated
            input_text = line;
            outstream << container.match(input_text);
        } else if (line[0] == '\n') {
            outstream << container.match(input_text);
            input_text.clear();
        } else {
            input_text.append(line);
        }
        free(line);
        line = NULL;
    }
    if (blankline_separated && !input_text.empty()) {
        outstream << container.match(input_text);
    }
//         if (c == '\n') {
//             if (blankline_separated) {
//                 if (input_text[input_text.size() - 1] == '\n') {
//                     std::cerr << "Sending " << input_text << std::endl;
//                     outstream << container.match(input_text);
//                     input_text.clear();
//                     input_line.clear();
//                 } else {
//                     // Just a newline, not a blank line
//                     input_line.push_back(c);
//                     input_text.append(input_line);
//                     input_line.clear();
//                 }
//             } else {
//                 // newline separated
//                 outstream << container.match(input_line);
//                 input_line.clear();
//             }
//         } else {
//             // a regular char
//             input_line.push_back(c);
//         }
//     }
    return EXIT_SUCCESS;
}


int parse_options(int argc, char** argv)
{
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
            {
                HFST_GETOPT_COMMON_LONG,
                {"newline", no_argument, 0, 'n'},
                {0,0,0,0}
            };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT "n",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }


        switch (c)
        {
#include "inc/getopt-cases-common.h"
        case 'n':
            blankline_separated = false;
            break;
#include "inc/getopt-cases-error.h"
        }

        
        
    }

//            if (!inputNamed)
//        {
//            inputfile = stdin;
//            inputfilename = hfst_strdup("<stdin>");
//        }
        
        // no more options, we should now be at the input filename
        if ( (optind + 1) < argc)
        {
            std::cerr << "More than one input file given\n";
            return EXIT_FAILURE;
        }
        else if ( (optind + 1) == argc)
        {
            pmatch_filename = argv[(optind)];
            return EXIT_CONTINUE;
        }
        else
        {
            std::cerr << "No input file given\n";
            return EXIT_FAILURE;
        }


#include "inc/check-params-common.h"


    
    return EXIT_FAILURE;
}


int main(int argc, char ** argv)
{
    hfst_set_program_name(argv[0], "0.1", "HfstPmatch");
    hfst_setlocale();
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE) {
        return retval;
    }
    std::ifstream instream(pmatch_filename.c_str(),
                           std::ifstream::binary);
    if (!instream.good()) {
        std::cerr << "Could not open file " << pmatch_filename << std::endl;
        return EXIT_FAILURE;
    }
    hfst_ol::PmatchContainer container(instream);
//     if (outfile != stdout) {
//         std::filebuf fb;
// fb.open(outfilename, std::ios::out);
// std::ostream outstream(&fb);
// return process_input(container, outstream);
// fb.close();
//     } else {
    return process_input(container, std::cout);
}
