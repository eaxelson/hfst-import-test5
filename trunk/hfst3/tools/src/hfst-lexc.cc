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


#include "hfst-commandline.h"
#include "hfst-program-options.h"


#include "inc/globals-common.h"
static char** lexcfilenames = 0;
static FILE** lexcfiles = 0;
static unsigned int lexccount = 0;
static bool is_input_stdin = true;
static ImplementationType format = hfst::UNSPECIFIED_TYPE;
static bool start_readline = false;

int lexc_readline_loop(ImplementationType format);

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1...]]\n"
             "Compile lexc files into transducer or imitate Xerox lexc\n"
        "\n", program_name );
        print_common_program_options(message_out);
        fprintf(message_out, "Input/Output options:\n"
               "  -f, --format=FORMAT       compile into FORMAT transducer\n"
               "  -o, --output=OUTFILE      write result into OUTFILE\n"
               "  -X, --xerox, --readline   imitate Xerox lexc user interface\n"
        );
        fprintf(message_out, "\n");
        fprintf(message_out,
                "If INFILE or OUTFILE are omitted or -, standard streams will "
                "be used\n"
        "The possible values for FORMAT are { sfst, openfst-tropical, "
        "openfst-log,\n"
       	"foma, optimized-lookup-unweighted, optimized-lookup-weighted }.\n");
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
    extend_options_getenv(&argc, &argv);
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
          HFST_GETOPT_COMMON_LONG,
          {"format",   required_argument, 0, 'f'},
          {"output",   required_argument, 0, 'o'},
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
        switch (c)
        {
#include "inc/getopt-cases-common.h"
        case 'f':
          format = hfst_parse_format_name(optarg);
          break;
        case 'l':
          error(EXIT_FAILURE, 0, "Latin1 encoding not supported, please use "
                "iconv, recode, uconv or similar utility to convert legacy "
                "lexicons into Unicode UTF-8 format");
          return EXIT_FAILURE;
          break;
        case 'u':
          warning(0, 0, "UTF-8 is always the default in HFST tools");
          break;
        case 'X':
          start_readline = true;
          break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
    if (format == hfst::UNSPECIFIED_TYPE)
      {
#if HAVE_FOMA
        warning(0, 0, "Defaulting to foma type "
                "(since it has native lexc support);\n"
                "Use command-line option --format to override");
        format = hfst::FOMA_TYPE;
#elif HAVE_OPENFST
        warning(0, 0, "Defaulting to using OpenFst with legacy lexc "
                "compilation scheme\n"
                "Use command-line option --format to override");
        format = hfst::TROPICAL_OPENFST_TYPE;
#else
        error(EXIT_FAILURE, 0, "Format not given and cannot deduce sensible "
              "defaults with current setup;\n"
              "Use command-line option --format to define format");
#endif
      }
    if (outfile == stdout)
      {
        error(EXIT_FAILURE, 0, "Cannot write result to <stdout> since backend "
              "libraries will pollute it;\n"
              "Use command-line option --output meanwhile");
      }
    if (start_readline && (argc - optind > 0))
      {
        error(EXIT_FAILURE, 0, "Trailing arguments not allowed for interactive "
              "mode");
        return EXIT_FAILURE;
      }
    else if (!start_readline && (argc == optind))
      {
#if AT_END_OF_DEPRECATION_PERIOD
        warning(0, 0, "Reading lexc script (not a lexicon) "
                "from stdin with readline");
        start_readline = true;
#else
        warning(0, 0, "Reading lexicons from stdin is deprecated and will "
                "be removed in next versions;\n"
                "this is not even supported by the original lexc");
        char* tempfilename = hfst_strdup("/tmp/hfst-lexcXXXXXX");
        int temporary_fd = hfst_mkstemp(tempfilename);
        verbose_printf("Copying data from <stdin> to temporary file\n");
        char* fdata = 0;
        size_t len = 0;
        while (hfst_getline(&fdata, &len, stdin) != -1)
          {
            hfst_write(temporary_fd, fdata, strlen(fdata));
          }
        hfst_close(temporary_fd);
        lexccount = 1;
        lexcfiles = static_cast<FILE**>(malloc(sizeof(FILE*) * lexccount));
        lexcfilenames = static_cast<char**>(malloc(sizeof(char*) * lexccount));
        lexcfiles[0] = hfst_fopen(tempfilename, "r");
        lexcfilenames[0] = hfst_strdup(tempfilename);
#endif
      }
    else if (argc - optind > 0)
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

    if (lexccount > 1)
      {
        warning(0, 0, "multiple file handling is not supported by all "
                "backends;\n"
                "concatenating to temporary file");
        char* tempfilename = hfst_strdup("/tmp/hfst-lexcXXXXXX");
        int temporary_fd = hfst_mkstemp(tempfilename);
        for (unsigned int i = 0; i < lexccount; i++)
          {
            verbose_printf("Copying data from %s to temporary file\n",
                           lexcfilenames[i]);
            hfst_fseek(lexcfiles[i], -1, SEEK_END);
            long file_length = hfst_ftell(lexcfiles[i]);
            rewind(lexcfiles[i]);
            char* fdata = static_cast<char*>(malloc(sizeof(char) * 
                                                    (file_length +1)));
            hfst_fread(fdata, sizeof(char), file_length,
                                   lexcfiles[i]);
            fdata[file_length] = '\0';
            fclose(lexcfiles[i]);
            free(lexcfilenames[i]);
            hfst_write(temporary_fd, fdata, file_length);
          }
        hfst_close(temporary_fd);
        lexccount = 1;
        lexcfiles[0] = hfst_fopen(tempfilename, "r");
        lexcfilenames[0] = hfst_strdup(tempfilename);
      }
    return EXIT_CONTINUE;
}

int
lexc_streams(HfstOutputStream& outstream)
{
    HfstTransducer* trans;
    for (unsigned int i = 0; i < lexccount; i++)
      {
        verbose_printf("Parsing lexc file %s\n", lexcfilenames[i]);
        if (lexcfiles[i] == stdin)
          {
            error(EXIT_FAILURE, 0, "Cannot read from stdin");
          }
        else
          {
            trans = HfstTransducer::read_lexc(lexcfilenames[i], format);
          }
      }
    char* name = static_cast<char*>(malloc(sizeof(char) * 
                                    (strlen(lexcfilenames[0]) +
                                     strlen("lexc(...)") + 1)));
    if ((sprintf(name, "lexc(%s...)", lexcfilenames[0])))
      {
        trans->set_name(name);
      }
    else
      {
        trans->set_name("lexc(sprintf failed?)");
      }
    verbose_printf("\nWriting... ");
    outstream << *trans;
    verbose_printf("done\n");
    delete trans;
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstLexc");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use filenames
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
    if (start_readline) 
      {
        lexc_readline_loop(format);
        return EXIT_FAILURE;
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
    retval = lexc_streams(*outstream);
    delete outstream;
    free(lexcfilenames);
    free(outfilename);
    return retval;
}

