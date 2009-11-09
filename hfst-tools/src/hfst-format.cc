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


#include <hfst2/hfst.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include "hfst-commandline.h"
#include "hfst-program-options.h"

using std::ifstream;
using std::ios;

char * input_file_name = NULL;
//bool verbose = false;

void
print_usage()
{
        // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
        fprintf(message_out, "Usage: hfst-format [OPTIONS...] [INFILE]\n"
                   "determine HFST transducer format\n"
                "\n");

        print_common_program_options(message_out);
        fprintf(message_out,
                   "\n"
                   "If OUTFILE or INFILE is missing or -, "
                   "standard streams will be used.\n"
                   "\n"
                   "More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstFormat>\n"
                   "\n"
                   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void
print_version()
{
        // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
        fprintf(message_out, "HFSTFormat 0.0 (%s)\n"
                   "Copyright (C) 2009 University of Helsinki,\n"
                   "License GPLv3: GNU GPL version 3 "
                   "<http://gnu.org/licenses/gpl.html>\n"
                   "This is free software: you are free to change and redistribute it.\n"
                   "There is NO WARRANTY, to the extent permitted by law.\n",
                PACKAGE_STRING);
}


int
parse_options(int argc, char** argv)
{
	// use of this function requires options are settable on global scope
	while (true)
	{
		static const struct option long_options[] =
		{
			{"help", no_argument, 0, 'h'},
			{"version", no_argument, 0, 'V'},
			{"verbose", no_argument, 0, 'v'},
			{"input", required_argument, 0, 'i'},
			{"input1", required_argument, 0, '1'},
			{"input2", required_argument, 0, '2'},
			{"lexicon", required_argument, 0, 'l'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, ":i:1:2:l:vhV",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		switch (c)
		{
		case 'h':
			print_usage();
			exit(0);
			break;
		case 'V':
			print_version();
			exit(0);
			break;
		case 'v':
		  verbose = true;
		  break;
		case 'i':
		  input_file_name = strdup(optarg);
		  break;
		case '1':
		  input_file_name = strdup(optarg);
		  break;
		case '2':
		  input_file_name = strdup(optarg);
		  break;
		case 'l':
		  input_file_name = strdup(optarg);
		  break;
		default:
		  break;
		}
	}
	if (input_file_name == NULL)
	  {
	    if ((argc - optind) == 0)
	      {
		if (verbose)
		  {
		    fprintf(stderr,"Reading input transducer from STDIN.\n");
		  }
		return HFST::read_format();
	      }
	    else
	      {
		input_file_name = argv[optind];
	      }	    
	  }
	ifstream input_file(input_file_name,ios::in);
	int exit_code = HFST::read_format(input_file);
	input_file.close();
	return exit_code;
}


int main (int argc, char * argv[])
{
  int exit_code =
    parse_options(argc,argv);
  exit(exit_code);
}
