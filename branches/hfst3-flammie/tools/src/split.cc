//! @file hfst-split.cc
//!
//! @brief Transducer archive exploding tool
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


// add tools-specific variables here
char *prefix = 0;
char *extension = 0;

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Extract transducers from archive with systematic file names\n"
        "\n", program_name);
    print_common_program_options();
    fprintf(message_out, "Input/Output options:\n"
            "  -i, --input=INFILE    Read input transducer from INFILE\n"
            "  -p, --prefix=PRE      Use the prefix PRE in "
            "naming output files\n"
            "  -e, --extension=EXT   Use the extension EXT in "
            "naming output files\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, 
        "If INFILE is omitted or -, stdin is used.\n"
            "If PRE is omitted, no prefix is used.\n"
        "If EXT is omitted, .hfst is used.\n"
        "The extracted files are named \"PRE\" + N + \"EXT\",\n"
        "where N is the number of the transducer in the archive.\n\n"
        "An example:\n"
        "   cat transducer_a transducer_b | hfst-split -p \"rule\" -e \".tr\"\n\n"
        "This command creates files \"rule1.tr\" (equivalent to transducer_a)\n"
        "and \"rule2.tr\" (equivalent to transducer_b). \n");
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
          {"input", required_argument, 0, 'i'},
          {"prefix", required_argument, 0, 'p'},
          {"extension", required_argument, 0, 'e'},
          // add tool-specific options here 
            {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT "i:p:e:",
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
          case 'p':
            prefix = hfst_strdup(optarg);
            break;
          case 'e':
            free(extension);
            extension = hfst_strdup(optarg);
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
check_options(int, char**)
  {
    if (NULL == extension)
      {
        hfst_warning("Extension not set, defaulting to .hfst");
        extension = hfst_strdup(".hfst");
      }
    if (NULL == prefix)
      {
        hfst_verbose("Prefix not set, leaving empty");
        prefix = hfst_strdup("");
      }
  }

void
make_splits()
  {
    size_t transducer_n=0;
    while (instream->is_good())
      {
        transducer_n++;
        outfilename = static_cast<char*>(hfst_malloc(sizeof(char) *
                             strlen(prefix) + strlen(extension) +
                             strlen("123456789012345678901234567890")));
        sprintf(outfilename, "%s%zu%s", prefix, transducer_n,
                              extension);
        verbose_printf("Writing %zu of %s to %s...\n", transducer_n,
                       inputfilename, outfilename); 
        outstream = new HfstOutputStream(outfilename,
                                         instream->get_type());
        HfstTransducer trans(*instream);
        *outstream << trans;
        free(outfilename);
     }
  }


int main( int argc, char **argv ) {
    hfst_init_commandline(argv[0], "0.1", "HfstSplit",
                          AUTOM_IN_FILE_OUT, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    check_options(argc, argv);
    parse_options_getenv();
    hfst_open_streams();
    make_splits();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}

