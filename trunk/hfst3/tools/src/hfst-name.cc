//! @file hfst-name.cc
//!
//! @brief Transducer naming command line tool
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

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::exceptions::NotTransducerStreamException;

// add tools-specific variables here

static char * transducer_name= strdup("");
static bool name_option_given=false;
static bool print_name=false;

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    // Usage line
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
           "Name a transducer\n"
        "\n", program_name);
    fprintf(message_out, "Name options:\n");
    fprintf(message_out, "  -n, --name=NAME        Name the transducer NAME.\n");
    fprintf(message_out, "  -p, --print-name       Only print the current name to standard error stream.\n");
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
		  // add tool-specific options here
		  {"name", required_argument, 0, 'n'},
		  {"print-name", no_argument, 0, 'p'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "n:p",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
#include "inc/getopt-cases-error.h"
		case 'n':
		  transducer_name = hfst_strdup(optarg);
		  name_option_given=true;
		  break;
		case 'p':
		  print_name = true;
		  break;
		}
	}

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
	return EXIT_CONTINUE;
}

int
process_stream(HfstInputStream& instream, HfstOutputStream& outstream)
{
  //instream.open();
  //outstream.open();
	
	size_t transducer_n=0;
	while(instream.is_good())
	{
		transducer_n++;

		if ((transducer_n > 1) && print_name) {
		  std::cerr << "---\n";
		}

		if (transducer_n==1)
		{
		  verbose_printf("Naming %s...\n", inputfilename); 
		}
		else
		  {
		    verbose_printf("Naming %s...%zu\n", 
				   inputfilename, transducer_n); 
		  }
		
		HfstTransducer trans(instream);
		if (not print_name) {
		  trans.set_name(std::string(transducer_name));
		  outstream << trans;
		}
		else
		  std::cerr << "\"" << trans.get_name() << "\"\n";
	}
	instream.close();
	outstream.close();
	return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
	hfst_set_program_name(argv[0], "0.1", "HfstName");
	int retval = parse_options(argc, argv);
	if (retval != EXIT_CONTINUE)
	{
		return retval;
	}

	if (not print_name && not name_option_given) {
	  fprintf(stderr, "Error: hfst-name: use either option --print-name "
		  " or --name\n");
	  exit(1);
	}
	if (print_name && name_option_given) {
	  fprintf(stderr, "Warning: option --print-name overrides option "
		  "--name\n");
	}

	// close buffers, we use streams
	if (inputfile != stdin)
	{
		fclose(inputfile);
	}
	if (outfile != stdout)
	{
		fclose(outfile);
	}
	verbose_printf("Reading from %s, writing to %s\n", 
		inputfilename, outfilename);
	// here starts the buffer handling part
	HfstInputStream* instream = NULL;
	try {
	  instream = (inputfile != stdin) ?
	    new HfstInputStream(inputfilename) : new HfstInputStream();
	} catch (NotTransducerStreamException)	{
		error(EXIT_FAILURE, 0, "%s is not a valid transducer file",
              inputfilename);
		return EXIT_FAILURE;
	}
	HfstOutputStream* outstream = (outfile != stdout) ?
		new HfstOutputStream(outfilename, instream->get_type()) :
		new HfstOutputStream(instream->get_type());
	
	retval = process_stream(*instream, *outstream);
	delete instream;
	delete outstream;
	free(inputfilename);
	free(outfilename);
	return retval;
}

