//! @file format.cc
//!
//! @brief Format checking command line tool
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
#include <cstring>
#include <getopt.h>

#include <hfst.hpp>

#include "conventions/commandline.h"

using std::ifstream;
using std::ios;

using hfst::HfstTransducer;

static bool list_formats = false;
static hfst::ImplementationType format_to_test = hfst::UNSPECIFIED_TYPE;

void
print_usage()
  {
    // c.f. 
    // http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
          "determine HFST transducer format\n"
          "\n", program_name);

    print_common_program_options();
    fprintf(message_out, 
     "Format options:\n"
     "  -l, --list-formats      List available transducer formats\n"
     "                          and print them to standard output\n"
     "  -t, --test-format FMT   Whether the format FMT is available,\n"
     "                          exits with 0 if it is, else with 1\n");
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
          {"input1", required_argument, 0, '1'},
          {"input2", required_argument, 0, '2'},
          {"list-formats", no_argument, 0, 'l'},
          {"test-format", required_argument, 0, 't'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, ":" HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "1:2:lt:",
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
          case '1':
            firstfilename = strdup(optarg);
            break;
          case '2':
            secondfilename = strdup(optarg);
            break;
          case 'l':
            list_formats=true;
            break;
          case 't':
            format_to_test= hfst_parse_format_name(optarg);
            break;
          default:
            // I suppose it's crucial for this tool to ignore other options
            break;
          }
      }
    if (inputfilename == NULL)
      {
        if ((argc - optind) == 0)
          {
            is_input_stdin = true;
            inputfilename = strdup("<stdin>");
          }
        else if ((argc - optind) == 1)
          {
            is_input_stdin = false;
            inputfilename = argv[optind];
          }
      }
  }

void
test_format()
  {
    if (format_to_test != hfst::UNSPECIFIED_TYPE)
      {
        if (HfstTransducer::is_implementation_type_available(format_to_test))
          {
            hfst_info("format %s is available", 
                           hfst_strformat(format_to_test));
            exit(0);
          }
        else
          {
            hfst_info("format %s is not available", 
                           hfst_strformat(format_to_test));
            exit(1);
          }
      }
  }

void
list_format()
  {
    hfst_info("Available format names:");
    if (HfstTransducer::is_implementation_type_available(hfst::SFST_TYPE))
      {
        hfst_info("sfst = %s", hfst_strformat(hfst::SFST_TYPE));
      }
    if (HfstTransducer::is_implementation_type_available
        (hfst::TROPICAL_OPENFST_TYPE))
      {
        hfst_info("openfst-tropical = %s",
                hfst_strformat(hfst::TROPICAL_OPENFST_TYPE));
      }
    if (HfstTransducer::is_implementation_type_available
        (hfst::LOG_OPENFST_TYPE))
      hfst_info("openfst-log = %s",
                hfst_strformat(hfst::LOG_OPENFST_TYPE));
    if (HfstTransducer::is_implementation_type_available
        (hfst::FOMA_TYPE))
      hfst_info("foma = %s",
                hfst_strformat(hfst::FOMA_TYPE));
    if (HfstTransducer::is_implementation_type_available
        (hfst::HFST_OL_TYPE))
      hfst_info("optimized-lookup-unweighted = %s",
                hfst_strformat(hfst::HFST_OL_TYPE));
    if (HfstTransducer::is_implementation_type_available
        (hfst::HFST_OLW_TYPE))
      hfst_info("optimized-lookup-weighted = %s",
                hfst_strformat(hfst::HFST_OLW_TYPE));
  }

void
read_file_format()
  {
    hfst::ImplementationType this_format = hfst::UNSPECIFIED_TYPE;
    try 
      {
        if (is_input_stdin)
          {
            hfst::HfstInputStream is;
            this_format = is.get_type();
          }
        else
          {
            hfst::HfstInputStream is(inputfilename);
            this_format = is.get_type();
          }
      } 
    catch (const NotTransducerStreamException ntse) 
      {
        hfst_error(EXIT_FAILURE, 0,
                   "The file/stream does not contain automata: %s",
                   ntse().c_str());
      }
    hfst_info("First automaton in %s is of type %s\n",
              inputfilename, hfst_strformat(this_format));
  }


int main (int argc, char * argv[])
  {
    hfst_init_commandline(argv[0], "0.1", "HfstFormat",
                          NO_AUTOMAGIC_IO, NO_AUTOMAGIC_FIlES);
    parse_options(argc, argv);
    check_common_options(argc, argv);
    if (format_to_test != hfst::UNSPECIFIED_TYPE)
      {
        test_format();
      }
    if (list_formats)
      {
        list_format();
      }
    if (inputfile)
      {
        read_file_format();
      }
    return EXIT_SUCCESS;
  }
