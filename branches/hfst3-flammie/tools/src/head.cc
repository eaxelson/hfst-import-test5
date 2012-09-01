//! @file hfst-head.cc
//!
//! @brief Transducer archive head splitting tool
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
#include <deque>

using std::deque;

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
long head_count = 1;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Get first transducers from an archive\n"
        "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Archive options:\n"
            "  -n, --n-first=[-]K   print the first K transducers;\n"
            "                       with the leading `-', print all but "
            "last K transducers\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "K must be an integer, as parsed by "
            "strtoul base 10, and not 0.\n"
            "If K is omitted default is 1.");
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
          {"n-first", required_argument, 0, 'n'},
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
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        switch (c)
          {
          case 'n':
            head_count = hfst_strtol(optarg, 10);
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
check_options()
  {
    if (head_count == 0)
      {
        hfst_warning( "Argument 0 for count is not sensible");
      }
  }


void
forward_heads()
  {
    size_t transducer_n=0;
    if (head_count > 0)
      {
        while (instream->is_good() && (transducer_n < head_count))
        {
            transducer_n++;
            HfstTransducer trans(*instream);
            const char* inputname = hfst_get_name(trans, inputfilename);
            verbose_printf("Forwarding %s...%zu\n", inputname, transducer_n); 
            *outstream << trans;
          }
      }
    else if (head_count < 0)
      {
        deque<HfstTransducer> first_but_n;
        verbose_printf("Counting all but last %zu\n", head_count);
        while (instream->is_good())
          {
            transducer_n++;
            HfstTransducer trans(*instream);
            first_but_n.push_back(trans);
           }
        if (-head_count > first_but_n.size())
          {
            hfst_warning( "Stream in %s has less than %zu automata; "
                    "Nothing will be written to output",
                    inputfilename, -head_count);
          }
        for (int i = 0; i < -head_count; i++)
          {
            if (!first_but_n.empty())
              {
                first_but_n.pop_back();
              }
          }
        while (!first_but_n.empty())
          {
            HfstTransducer trans = first_but_n.front();
            const char* inputname = hfst_get_name(trans, inputfilename);
            verbose_printf("Forwarding %s...%zu\n", inputname, transducer_n); 
            *outstream << trans;
            first_but_n.pop_front();
          }
      }
  }


int main(int argc, char **argv) 
  {
    hfst_init_commandline(argv[0], "0.2", "HfstHead",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options();
    parse_options_getenv();
    hfst_open_streams();
    forward_heads();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

