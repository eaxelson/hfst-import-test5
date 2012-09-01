//! @file hfst-txt2fst.cc
//!
//! @brief Transducer text compiling command line tool
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
using hfst::HfstOutputStream;

// whether numbers are used instead of symbol names
static bool use_numbers=false; // not used
// printname for epsilon
static char *epsilonname=NULL;
static const unsigned int EPSILON_KEY=0;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Convert some text formats into binary automata\n"
           "\n", program_name);

    print_common_program_options();
    fprintf(message_out, "Text format options:\n"
            "  -e, --epsilon=EPS   Interpret string EPS as epsilon\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, 
        "If EPS is not given, @0@ will be used\n"
        );
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
        HFST_GETOPT_CREATIONAL_LONG,
          // add tool-specific options here
            {"epsilon", required_argument, 0, 'e'},
            {"number", no_argument, 0, 'n'},
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "e:nf:",
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
          case 'e':
            epsilonname = hfst_strdup(optarg);
            break;
          case 'n':
            use_numbers = true;
            break;
          default:
            parse_getopt_error_value(c);
          }
      }
  }

void
check_options(int, char**)
  {
    if (epsilonname == NULL)
      {
        epsilonname = hfst_strdup("@0@");
        verbose_printf("Using default epsilon representation %s\n", 
                       epsilonname);
      }
  }

void
make_texts()
  {
    size_t transducer_n = 0;
    while (!feof(inputfile))
      {
        transducer_n++;
        if (transducer_n < 2)
          {
            verbose_printf("Reading transducer table...\n");
          }
        else
          {
            verbose_printf("Reading transducer table %zu...\n", transducer_n);
          }
          HfstTransducer t(inputfile,
               format,
               std::string(epsilonname));
          hfst_set_name(t, inputfilename, "text");
          hfst_set_formula(t, inputfilename, "T");
          *outstream << t;
      }
  }


int main( int argc, char **argv ) 
  {
    hfst_init_commandline(argv[0], "0.1", "HfstTxt2Fst",
                          FILE_IN_AUTOM_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_texts();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

