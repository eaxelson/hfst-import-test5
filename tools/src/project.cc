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

#include <hfst.hpp>

#include "conventions/commandline.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;


// add tools-specific variables here
bool project_input = false;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Project (extract a level) transducer\n"
        "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Projection options:\n"
            "  -p, --project=LEVEL   project extracting tape LEVEL\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "LEVEL must be one of upper, input, first, analysis "
            "or lower, output, second, generation\n");
    fprintf(message_out, "\n");
    print_report_bugs();
    print_more_info();
  }


void
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
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
          case 'p':
            if ( (strncasecmp(optarg, "upper", 1) == 0) ||
                 (strncasecmp(optarg, "input", 1) == 0) ||
                 (strncasecmp(optarg, "first", 1) == 0) ||
                 (strncasecmp(optarg, "analysis", 1) == 0) )
              {
                project_input = true;
              }
            else if ( (strncasecmp(optarg, "lower", 1) == 0) ||
                      (strncasecmp(optarg, "output", 1) == 0) ||
                      (strncasecmp(optarg, "second", 1) == 0) ||
                      (strncasecmp(optarg, "generation", 1) == 0) )
              {
                project_input = false;
              }
            else
              {
                hfst_error(EXIT_FAILURE, 0,
                      "unknown project direction %s\n"
                      "should be one of upper, input, analysis, first, "
                      "lower, output, second or generation\n",
                      optarg);
              }
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
make_projections()
  {
    size_t transducer_n=0;
    while (instream->is_good())
    {
        transducer_n++;
        HfstTransducer trans(*instream);
        const char* inputname = hfst_get_name(trans, inputfilename);
        if (transducer_n==1)
        {
          if (project_input)
            {
              verbose_printf("Projecting first %s...\n", inputname); 
            }
          else
            {
              verbose_printf("Projecting second %s...\n", inputname);
            }
        }
        else
        {
          if (project_input)
            {
              verbose_printf("Projecting first %s... %zu\n", inputname,
                             transducer_n);
            }
          else
            {
              verbose_printf("Projecting second %s... %zu\n", inputname,
                             transducer_n);
            }
        }
        if (project_input)
          {
            trans.input_project();
            hfst_set_name(trans, trans, "project-1st");
            hfst_set_formula(trans, trans, "¹");
          }
        else
          {
            trans.output_project();
            hfst_set_name(trans, trans, "project-2nd");
            hfst_set_formula(trans, trans, "²");
          }
        *outstream << trans;
      }
  }



int main( int argc, char **argv ) {
    hfst_init_commandline(argv[0], "0.1", "HfstProject",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_projections();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

