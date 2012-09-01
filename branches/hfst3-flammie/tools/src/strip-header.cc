//! @file hfst-fst2fst.cc
//!
//! @brief HFST header stripping tool
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

#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "conventions/commandline.h"

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Remove any HFST3 headers\n"
        "\n", program_name);

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
    while (true)
      {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
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
        if (parse_common_getopt_value(c))
          {
            continue;
          }
        else
          {
            parse_getopt_error_value(c);
          }
      }
  }

void
strip_headers()
  {
    hfst_error(EXIT_FAILURE, 0, "This was not right");
  }

int main(int argc, char* argv[])
  {
    hfst_init_commandline(argv[0], "0.1", "HfstStripHeader",
                          AUTOM_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    strip_headers();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

