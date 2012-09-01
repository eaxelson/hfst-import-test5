//! @file hfst-lexc.cc
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


#include "conventions/commandline.h"
static bool start_readline = false;
static char* tempfilename = 0;

int lexc_readline_loop(ImplementationType format);

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1...]\n"
             "Compile lexc files into transducer or imitate Xerox lexc\n"
        "\n", program_name );
        print_common_program_options();
        fprintf(message_out, "Input/Output options:\n"
               "  -X, --xerox, --readline   imitate Xerox lexc user interface\n"
        );
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
          {"latin1",   optional_argument, 0, 'l'},
          {"utf8",     optional_argument, 0, 'u'},
          {"readline", no_argument,       0, 'X'},
          {"xerox",    no_argument,       0, 'X'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             "f:o:l::u::X",
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
          case 'l':
            hfst_error(EXIT_FAILURE, 0, 
                       "Latin1 encoding not supported, please use "
                "iconv, recode, uconv or similar utility to convert legacy "
                "lexicons into Unicode UTF-8 format");
            break;
          case 'u':
            if (!silent)
              {
                hfst_warning( "UTF-8 is always the default in HFST tools");
              }
            break;
          case 'X':
            start_readline = true;
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
check_options(int argc,char** argv)
  {
    if (format == hfst::FOMA_TYPE)
      {
        hfst_warning("Note that compilation in other than -f foma is SLOW");
      }
    if (outfile == stdout)
      {
        hfst_error(EXIT_FAILURE, 0, "Cannot write result to <stdout> since backend "
              "libraries will pollute it;\n"
              "Use command-line option --output meanwhile");
      }
    if (start_readline && (argc - optind > 0))
      {
        hfst_error(EXIT_FAILURE, 0, "Trailing arguments not allowed for interactive "
              "mode");
      }
    else if (!start_readline && (inputs_named == 1) && 
             (strcmp(inputfilenames[0], "<stdin>") == 0))
      {
        if (!silent)
          {
            hfst_warning( "Reading lexc script (not a lexicon) "
                "from stdin with readline");
          }
        start_readline = true;
      }
  }

void
make_lexcs()
  {
    HfstTransducer* trans = 0;
    bool delete_temporaries = false;
    if (inputs_named > 1)
      {
        if (!silent)
          {
            hfst_warning( "multiple file handling is not supported by all "
                "backends;\n"
                "concatenating to temporary file");
          }
        char* tempfilename = hfst_strdup("/tmp/hfst-lexcXXXXXX");
        int temporary_fd = hfst_mkstemp(tempfilename);
        for (unsigned int i = 0; i < inputs_named; i++)
          {
            verbose_printf("Copying data from %s to temporary file\n",
                           inputfilenames[i]);
            hfst_fseek(inputfiles[i], -1, SEEK_END);
            long file_length = hfst_ftell(inputfiles[i]);
            rewind(inputfiles[i]);
            char* fdata = static_cast<char*>(malloc(sizeof(char) * 
                                                    (file_length + 1)));
            hfst_fread(fdata, sizeof(char), file_length,
                                   inputfiles[i]);
            fdata[file_length] = '\n';
            fclose(inputfiles[i]);
            free(inputfilenames[i]);
            hfst_write(temporary_fd, fdata, file_length + 1);
          }
        hfst_close(temporary_fd);
        inputs_named = 1;
        inputfiles[0] = hfst_fopen(tempfilename, "r");
        inputfilenames[0] = hfst_strdup(tempfilename);
        delete_temporaries = true;
      }
    for (unsigned int i = 0; i < inputs_named; i++)
      {
        verbose_printf("Parsing lexc file %s\n", inputfilenames[i]);
        if (inputfiles[i] == stdin)
          {
            hfst_error(EXIT_FAILURE, 0, "Cannot read from stdin");
          }
        else
          {
            trans = HfstTransducer::read_lexc(inputfilenames[i], format);
            if (0 == trans)
              {
                hfst_error(EXIT_FAILURE, 0, "Could not parse %s correctly.\n"
                      "If there is no further info about the error, try "
                      "-v or -d.", inputfilenames[i]);
              }
          }
      }
    hfst_set_name(*trans, inputfilenames[0], "lexc");
    hfst_set_formula(*trans, inputfilenames[0], "L");
    verbose_printf("\nWriting... ");
    *outstream << *trans;
    verbose_printf("done\n");
    delete trans;
    if (delete_temporaries)
      {
        verbose_printf("Deleting temporary files on succesful exit\n");
        hfst_remove(inputfilenames[0]);
      }
  }


int main( int argc, char **argv ) {
    hfst_init_commandline(argv[0], "0.1", "HfstLexc",
                          FILE_IN_AUTOM_OUT, READ_MANY);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    if (start_readline)
      {
        lexc_readline_loop(format);
      }
    else
      {
        make_lexcs();
      }
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
  }

