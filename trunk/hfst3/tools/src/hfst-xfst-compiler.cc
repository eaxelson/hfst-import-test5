//! @file hfst-xfst-compiler.cc
//!
//! @brief Xfst compilation command line tool
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
#include "implementations/XfstCompiler.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;
using hfst::exceptions::NotTransducerStreamException;
using hfst::xfst::XfstCompiler;

#include "hfst-commandline.h"
#include "hfst-program-options.h"


#include "inc/globals-common.h"
static char** xfstfilenames = 0;
static FILE** xfstfiles = 0;
static unsigned int xfstcount = 0;
static char* outfilename = 0;
static FILE* outfile = 0;
static bool is_input_stdin = true;
static bool is_output_stdout = true;
static ImplementationType format = hfst::UNSPECIFIED_TYPE;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1...]]\n"
             "Compile xfst files into transducer\n"
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
            "  %s -o cat.hfst cat.xfst               Compile single-file "
            "lexicon\n"
            "  %s -o L.hfst Root.xfst 2.xfst 3.xfst  Compile multi-file "
            "lexicon\n"
            "\n",
            program_name, program_name );
        print_report_bugs();
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
        case 'o':
          outfilename = hfst_strdup(optarg);
          if (strcmp(outfilename, "-") == 0) {
            free(outfilename);
            outfilename = hfst_strdup("<stdout>");
            outfile = stdout;
            is_output_stdout = true;
            message_out = stderr;
          }
          else {
            outfile = hfst_fopen(outfilename, "w");
            is_output_stdout = false;
            message_out = stdout;
          }
          break;
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
        format = hfst::TROPICAL_OFST_TYPE;
      }
    if (argc - optind > 0)
      {
        xfstfilenames = static_cast<char**>(malloc(sizeof(char*)*(argc-optind)));
        xfstfiles = static_cast<FILE**>(malloc(sizeof(char*)*(argc-optind)));
        while (optind < argc)
          {
            xfstfilenames[xfstcount] = hfst_strdup(argv[optind]);
            xfstfiles[xfstcount] = hfst_fopen(argv[optind], "r");
            xfstcount++;
            optind++;
          }
        is_input_stdin = false;
      }
    else
      {
        xfstfilenames = static_cast<char**>(malloc(sizeof(char*)));
        xfstfiles = static_cast<FILE**>(malloc(sizeof(FILE*)));
        xfstfilenames[0] = hfst_strdup("<stdin>");
        xfstfiles[0] = stdin;
        is_input_stdin = true;
        xfstcount++;
      }
    return EXIT_CONTINUE;
}

int
xfst_streams(XfstCompiler& xfst, HfstOutputStream& outstream)
{
  //outstream.open();
    for (unsigned int i = 0; i < xfstcount; i++)
      {
        verbose_printf("Parsing xfst file %s\n", xfstfilenames[i]);
        if (xfstfiles[i] == stdin)
          {
            verbose_printf("hfst[0]: ");
            xfst.parse(stdin);
          }
        else
          {
            xfst.parse(xfstfilenames[i]);
          }
      }
    outstream.close();
    return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
    hfst_set_program_name(argv[0], "0.1", "HfstXfstCompiler");
    int retval = parse_options(argc, argv);
    if (retval != EXIT_CONTINUE)
    {
        return retval;
    }
    // close buffers, we use streams
    for (unsigned int i = 0; i < xfstcount; i++)
      {
        if (xfstfiles[i] != stdin)
          {
            fclose(xfstfiles[i]);
          }
      }
    if (outfile != stdout)
    {
        fclose(outfile);
    }
    verbose_printf("Reading from ");
    for (unsigned int i = 0; i < xfstcount; i++)
      {
        verbose_printf("%s, ", xfstfilenames[i]);
      }
    verbose_printf("writing to %s\n", outfilename);
    // here starts the buffer handling part
    HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, format) :
        new HfstOutputStream(format);
    XfstCompiler xfst(format);
    if (silent)
      {
        xfst.setVerbosity(false);
      }
    else
      {
        xfst.setVerbosity(verbose);
      }
    retval = xfst_streams(xfst, *outstream);
    delete outstream;
    free(xfstfilenames);
    free(outfilename);
    return retval;
}

