//! @file hfst-format.cc
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

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using std::ifstream;
using std::ios;

//using hfst::ImplementationType;
//using hfst::HfstInputStream;

//bool verbose = false;

void
print_usage()
{
        // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
        fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
                   "determine HFST transducer format\n"
                "\n", program_name);

        print_common_program_options(message_out);
        print_common_unary_program_options(message_out);
        fprintf(message_out, "\n");
    print_common_unary_program_parameter_instructions(message_out);
    fprintf(message_out, "\n");
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
          HFST_GETOPT_UNARY_LONG,
          {"input1", required_argument, 0, '1'},
          {"input2", required_argument, 0, '2'},
          {"lexicon", required_argument, 0, 'l'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "1:2:l:",
                             long_options, &option_index);
        if (-1 == c)
        {
            break;
        }
        switch (c)
        {
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
        case '1':
          inputfilename = strdup(optarg);
          break;
        case '2':
          inputfilename = strdup(optarg);
          break;
        case 'l':
          inputfilename = strdup(optarg);
          break;
        default:
          // I suppose it's crucial for this tool to ignore other options
          break;
        }
    }


    (void)inputfilename;
    (void)inputNamed;

    try {

      if (inputfilename == NULL)
	{
	  if ((argc - optind) == 0)
	    {
	      inputfilename = strdup("<stdin>");
	      hfst::HfstInputStream is("");
	      return is.get_type();
	    }
	  else if ((argc - optind) == 1)
	    {
	      inputfilename = argv[optind];
	    }     
	}
      hfst::HfstInputStream is(inputfilename);
      return is.get_type();
    } catch (hfst::exceptions::NotTransducerStreamException e) {
      fprintf(stderr, "ERROR: The file/stream does not contain transducers.\n");
      exit(1);
    }
}


int main (int argc, char * argv[])
{
  hfst_set_program_name(argv[0], "0.1", "HfstFormat");
  verbose = true;
  hfst::ImplementationType type = static_cast<hfst::ImplementationType>(parse_options(argc,argv));
  verbose_printf("Transducers in %s are of type %s\n", inputfilename,
                 hfst_strformat(type));
}
