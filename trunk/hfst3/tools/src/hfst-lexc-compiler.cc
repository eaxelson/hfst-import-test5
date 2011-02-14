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

#include "HfstTransducer.h"
#include "HfstOutputStream.h"
#include "parsers/LexcCompiler.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;
using hfst::exceptions::NotTransducerStreamException;
using hfst::lexc::LexcCompiler;

#include "hfst-commandline.h"
#include "hfst-program-options.h"


#include "inc/globals-common.h"
static char** lexcfilenames = 0;
static FILE** lexcfiles = 0;
static unsigned int lexccount = 0;
static bool is_input_stdin = true;
static ImplementationType format = hfst::UNSPECIFIED_TYPE;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1...]]\n"
             "Compile lexc files into transducer\n"
        "\n", program_name );
        print_common_program_options(message_out);
        fprintf(message_out, "Input/Output options:\n"
               "  -f, --format=FORMAT     compile into FORMAT transducer\n"
               "  -o, --output=OUTFILE    write result into OUTFILE\n");
        fprintf(message_out, "\n");
        fprintf(message_out,
                "If INFILE or OUTFILE are omitted or -, standard streams will "
                "be used\n"
                "FORMAT must be one of supported transducer formats, such as "
                "openfst-tropical, sfst, foma, etc.\n");
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
        fprintf(message_out, "\n");
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
          {"format", required_argument, 0, 'f'},
          {"output", required_argument, 0, 'o'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             "f:o:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        switch (c)
        {
#include "inc/getopt-cases-common.h"
        case 'f':
          format = hfst_parse_format_name(optarg);
          break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
    if (format == hfst::UNSPECIFIED_TYPE)
      {
        verbose_printf("Defaulting to OpenFst tropical type\n");
        format = hfst::TROPICAL_OPENFST_TYPE;
      }
    if (argc - optind > 0)
      {
        lexcfilenames = static_cast<char**>(malloc(sizeof(char*)*(argc-optind)));
        lexcfiles = static_cast<FILE**>(malloc(sizeof(char*)*(argc-optind)));
        while (optind < argc)
          {
            lexcfilenames[lexccount] = hfst_strdup(argv[optind]);
            lexcfiles[lexccount] = hfst_fopen(argv[optind], "r");
            lexccount++;
            optind++;
          }
        is_input_stdin = false;
      }
    else
      {
        lexcfilenames = static_cast<char**>(malloc(sizeof(char*)));
        lexcfiles = static_cast<FILE**>(malloc(sizeof(FILE*)));
        lexcfilenames[0] = hfst_strdup("<stdin>");
        lexcfiles[0] = stdin;
        is_input_stdin = true;
        lexccount++;
      }
    return EXIT_CONTINUE;
}

int
lexc_streams(LexcCompiler& lexc, HfstOutputStream& outstream)
{
    for (unsigned int i = 0; i < lexccount; i++)
      {
        verbose_printf("Parsing lexc file %s\n", lexcfilenames[i]);
        if (lexcfiles[i] == stdin)
          {
            lexc.parse(stdin);
          }
        else
          {
            lexc.parse(lexcfilenames[i]);
          }
      }
    verbose_printf("Compiling... ");
    HfstTransducer* res = lexc.compileLexical();
    verbose_printf("\nWriting... ");
    outstream << *res;
    verbose_printf("done\n");
    delete res;
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstLexcCompiler");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    for (unsigned int i = 0; i < lexccount; i++)
      {
        if (lexcfiles[i] != stdin)
          {
            fclose(lexcfiles[i]);
          }
      }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from ");
    for (unsigned int i = 0; i < lexccount; i++)
      {
        verbose_printf("%s, ", lexcfilenames[i]);
      }
    verbose_printf("writing to %s\n", outfilename);
    // here starts the buffer handling part
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, format) :
        new HfstOutputStream(format);
    LexcCompiler lexc(format);
    if (silent)
      {
        lexc.setVerbosity(false);
      }
    else
      {
        lexc.setVerbosity(verbose);
      }
    retval = lexc_streams(lexc, *outstream);
    delete outstream;
    free(lexcfilenames);
    free(outfilename);
    return retval;
}

