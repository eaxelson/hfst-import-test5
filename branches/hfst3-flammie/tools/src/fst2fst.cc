//! @file hfst-fst2fst.cc
//!
//! @brief Format conversion command line tool
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
using hfst::ImplementationType;

// tool-specific variables

bool hfst_format = true;
std::string options = "";

void
print_usage()
  {
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Convert transducers between binary formats\n"
        "\n", program_name);
    print_common_program_options();
    fprintf(message_out, "Conversion options:\n"
            "  -f, --format=FMT           Write result in FMT format\n"
            "  -b, --use-backend-format   Write result in native format, "
            "without HFST wrapper\n");
    fprintf(message_out, "\n");
    print_common_parameter_instructions();
    fprintf(message_out, "FMT is one of accepted format names: "
            "openfst-tropical, openfst-log, sfst, foma, olw, ol.\n"
            "If FMT is omitted, fst2fst does no conversions.");
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
          {"use-backend-format", no_argument, 0, 'b'},
          {"sfst",               no_argument, 0, 'S'},
          {"foma",               no_argument, 0, 'F'},
          {"openfst-tropical",    no_argument, 0, 't'},
          {"openfst-log",         no_argument, 0, 'l'},
          {"optimized-lookup-unweighted",   no_argument, 0, 'O'},
          {"optimized-lookup-weighted",no_argument, 0, 'w'},
          {0,0,0,0}
        };
        int option_index = 0;
        // add tool-specific options here 
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_CREATIONAL_SHORT "SFtlOwQb",
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
          case 'f':
            format = hfst_parse_format_name(optarg);
            break;
          case 'b':
            hfst_format = false;
            break;
          case 'S':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f sfst instead");
            format = hfst::SFST_TYPE;
            break;
          case 'F':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f foma instead");
            format = hfst::FOMA_TYPE;
            break;
          case 't':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f openfst-tropical instead");
            format = hfst::TROPICAL_OPENFST_TYPE;
            break;
          case 'l':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f openfst-log instead");
            format = hfst::LOG_OPENFST_TYPE;
            break;
          case 'O':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f ol instead");
            format = hfst::HFST_OL_TYPE;
            break;
          case 'w':
            hfst_warning("The format shorthands are deprecated, please use"
                         "-f olw instead");
            format = hfst::HFST_OLW_TYPE;
            break;
          default:
            parse_getopt_error_value(c);
            break;
          }
      }
  }

void
make_conversions()
  {
    size_t transducer_n = 0;
    while (instream->is_good())
      {
        transducer_n++;
        HfstTransducer orig(*instream);
        const char* inputname = hfst_get_name(orig, inputfilename);
        hfst_begin_processing(inputname, transducer_n, "Converting");
        orig.convert(format);
        hfst_set_name(orig, orig, "convert");
        hfst_set_formula(orig, orig, "Id");
        *outstream << orig;
    }
}


int main(int argc, char **argv)
  {
    hfst_init_commandline(argv[0], "0.1", "HfstFst2Fst",
                          NO_AUTOMAGIC_IO, READ_ONE);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    parse_options_getenv();
    // no hfst_open_streams() since special io
    if (inputfile != stdin)
      {
        instream = new HfstInputStream(inputfilename);
      }
    else
      {
        instream = new HfstInputStream();
      }
    if (hfst_format)
      {
        if (outfile != stdout)
          {
            outstream = new HfstOutputStream(outfilename,
                                             format);
          }
        else
          {
            outstream = new HfstOutputStream(format);
          }
      }
    else
      {
        if (outfile != stdout)
          {
            outstream = new HfstOutputStream(outfilename,
                                             format, hfst_format);
          }
        else
          {
            outstream = new HfstOutputStream(format, hfst_format);
          }
      }
    make_conversions();
    hfst_uninit_commandline();
    return EXIT_SUCCESS;
}

