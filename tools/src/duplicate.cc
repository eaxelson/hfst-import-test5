//! @file hfst-duplicate.cc
//!
//! @brief Transducer archive duplication tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;

// add tools-specific variables here
unsigned long dupe_count = 1;

void
print_usage()
 {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Use first transducer of an archive repeatedly\n"
        "\n", program_name);
    print_common_program_options();
    fprintf(message_out, "Archive options:\n"
            "  -n, --n-last=NUMBER   Duplicate each transducer NUMBER times\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "NUMBER must be a positive integer as parsed by "
            "strtoul base 10\n");
    fprintf(message_out, "\n");
    print_report_bugs();
    print_more_info();
}


void
parse_options(int argc, char** argv)
  {
    while (true)
      {
        static const struct option long_options[] =
        {
          HFST_GETOPT_COMMON_LONG,
          HFST_GETOPT_UNARY_LONG,
          {"n-times", required_argument, 0, 'n'},
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
            dupe_count = hfst_strtoul(optarg, 10);
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
make_duplications()
  {
    queue<HfstTransducer> last_n;
    size_t transducer_n=0;
    while (instream->is_good())
      {
        transducer_n++;
        HfstTransducer trans(*instream);
        const char* inputname = hfst_get_name(trans, inputfilename);
        verbose_printf("Duplicate %s times %zu...%zu\n", inputname, 
                       dupe_count, transducer_n); 
        for (unsigned int i = 0; i < dupe_count; i++)
          {
            *outstream << trans;
          }
      }
}


int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstDuplicate",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_duplications();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

