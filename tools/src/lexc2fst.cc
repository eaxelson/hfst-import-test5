//! @file hfst-lexc-compiler.cc
//!
//! @brief Lexc compilation command line tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;

using hfst::lexc::LexcCompiler;

#include "conventions/commandline.h"

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1...]]\n"
             "Compile lexc files into transducer\n"
        "\n", program_name );
        print_common_program_options();
        fprintf(message_out, "\n");
        print_common_parameter_instructions();
        fprintf(message_out,
            "\n"
            "Examples:\n"
            "  %s -o cat.hfst cat.lexc               Compile single-file "
            "lexicon\n"
            "  %s -o L.hfst Root.lexc 2.lexc 3.lexc  Compile multi-file "
            "lexicon\n"
            "\n",
            program_name, program_name );
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
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_CREATIONAL_SHORT,
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
make_lexcs()
  {
    LexcCompiler lexc(format);
    if (silent)
      {
        lexc.setVerbosity(false);
      }
    else
      {
        lexc.setVerbosity(verbose);
      }
    for (unsigned int i = 0; i < inputs_named; i++)
      {
        verbose_printf("Parsing lexc file %s\n", inputfilenames[i]);
        if (inputfiles[i] == stdin)
          {
            lexc.parse(stdin);
          }
        else
          {
            lexc.parse(inputfilenames[i]);
          }
      }
    verbose_printf("Compiling... ");
    HfstTransducer* res = lexc.compileLexical();
    if (0 == res)
      {
        if (inputs_named == 1)
          {
            hfst_error(EXIT_FAILURE, 0, "The file %s did not compile cleanly.\n"
                  "(if there are no error messages above, try -v or -d to "
                  "get more info)",
                  inputfilenames[0]);
          }
        else
          {
            hfst_error(EXIT_FAILURE, 0, 
                       "The files %s... did not compile cleanly.\n"
                  "(if there are no error messages above, try -v or -d to "
                  "get more info)",
                  inputfilenames[0]);
          }
      }
    hfst_set_name(*res, inputfilenames[0], "lexc");
    hfst_set_formula(*res, inputfilenames[0], "L");
    verbose_printf("\nWriting... ");
    *outstream << *res;
    verbose_printf("done\n");
    delete res;
  }


int main( int argc, char **argv ) {
    hfst_init_commandline(argv[0], "0.1", "HfstLexc2Fst",
                          FILE_IN_AUTOM_OUT, READ_MANY);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_lexcs();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}

