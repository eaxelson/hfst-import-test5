//! @file hfst-name.cc
//!
//! @brief Transducer naming command line tool
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


static char* transducer_name = 0;
static bool name_option_given = false;
static bool print_name = false;
static unsigned long truncate_length = 0;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Name a transducer\n"
        "\n", program_name);
    fprintf(message_out, "Name options:\n"
            "  -n, --name=NAME      Name the transducer NAME\n"
            "  -p, --print-name     Only print the current name\n"
            "  -t, --truncate_length=LEN   Truncate name length to LEN\n");
    print_common_program_options();
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
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
          {"name", required_argument, 0, 'n'},
          {"print-name", no_argument, 0, 'p'},
          {"truncate_length", required_argument, 0, 't'},
          {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "n:pt:",
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
          case 'n':
            transducer_name = hfst_strdup(optarg);
            name_option_given=true;
            break;
          case 'p':
            print_name = true;
            break;
          case 't':
            truncate_length = hfst_strtoul(optarg, 10);
            break;
          default:
            parse_getopt_error_value(c);
          }
      }
  }

void
name_automata()
  {
    size_t transducer_n=0;
    while (instream->is_good())
      {
        transducer_n++;
        hfst_begin_processing(inputfilename, transducer_n, "Naming");
        HfstTransducer trans(*instream);
        if (!print_name) 
          {
            if (truncate_length > 0)
              {
                trans.set_name(hfst_strndup(transducer_name, truncate_length));
              }
            else
              {
                trans.set_name(transducer_name);
              }
          *outstream << trans;
          }
        else
          {
            hfst_info("Name: %s", trans.get_name().c_str());
          }
      }
  }

void
check_options(int, char**)
  {
    if (!print_name && !name_option_given) 
      {
        hfst_error(EXIT_FAILURE, 0, "use either option --print-name "
                   " or --name");
      }
    if (print_name && name_option_given) 
      {
        hfst_warning("option --print-name overrides option --name\n");
      }
  }

int main(int argc, char **argv) {
    hfst_init_commandline(argv[0], "0.1", "HfstName",
                          AUTOM_IN_FILE_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    name_automata();
    hfst_warning("%s is deprecated, use hfst-edit-metadata instead",
                 program_short_name);
    hfst_uninit_commandline();
    return EXIT_FAILURE;
  }

