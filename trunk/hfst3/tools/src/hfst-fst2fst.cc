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

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include "HfstTransducer.h"

#include "hfst-common-unary-variables.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;

// tool-specific variables

ImplementationType output_type = hfst::UNSPECIFIED_TYPE;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Convert transducers between SFST, OpenFst and foma formats\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		   "  -d, --debug            Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	// fprintf(message_out, (tool-specific options and short descriptions)
	fprintf(message_out, "%-35s%s", "  -S, --sfst",           "Write the output for HFST's SFST implementation\n");
	fprintf(message_out, "%-35s%s", "  -f, --foma",           "Write the output in HFST's foma implementation\n");
	fprintf(message_out, "%-35s%s", "  -t, --tropical-weight","Write the output in HFST's tropical weight (OpenFST) implementation\n");
	fprintf(message_out, "%-35s%s", "  -l, --log-weight",     "Write the output in HFST's log weight (OpenFST) implementation\n");
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	fprintf(stderr, "\n");
	print_more_info(message_out, "Fst2Fst");
	fprintf(stderr, "\n");
	print_report_bugs(message_out);
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.1 (" PACKAGE_STRING ")\n"
		   "Copyright (C) 2008 University of Helsinki,\n"
		   "License GPLv3: GNU GPL version 3 "
		   "<http://gnu.org/licenses/gpl.html>\n"
		   "This is free software: you are free to change and redistribute it.\n"
		   "There is NO WARRANTY, to the extent permitted by law.\n",
		program_name);
}

int
parse_options(int argc, char** argv)
{
	// use of this function requires options are settable on global scope
	while (true)
	{
		static const struct option long_options[] =
		{
#include "hfst-common-options.h"
		  ,
#include "hfst-common-unary-options.h"
		  ,
		  // add tool-specific options here 
		  {"sfst",            no_argument, 0, 'S'},
		  {"foma",            no_argument, 0, 'f'},
		  {"tropical-weight", no_argument, 0, 't'},
		  {"log-weight",      no_argument, 0, 'l'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sqvVR:DW:Sftl",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		char *format=NULL;

		switch (c)
		{
#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
		  // add tool-specific cases here
		case 'S':
		  output_type = hfst::SFST_TYPE;
		  break;
		case 'f':
		  output_type = hfst::FOMA_TYPE;
		  break;
		case 't':
		  output_type = hfst::TROPICAL_OFST_TYPE;
		  break;
		case 'l':
		  output_type = hfst::LOG_OFST_TYPE;
		  break;
		case '?':
			fprintf(message_out, "invalid option --%s\n",
					long_options[option_index].name);
			print_short_help(argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			fprintf(message_out, "invalid option -%c\n", c);
			print_short_help(argv[0]);
			return EXIT_FAILURE;
			break;
		}
	}
	
	if(output_type == hfst::UNSPECIFIED_TYPE)
	{
		fprintf(message_out, "You must specify an output type (one of -S, -f, -t, or -l)\n");
		print_short_help(argv[0]);
		return EXIT_FAILURE;
	}

	if (is_output_stdout)
	{
			outfilename = hfst_strdup("<stdout>");
			outfile = stdout;
			message_out = stderr;
	}
	// rest of arguments are files...
	if (is_input_stdin && ((argc - optind) == 1))
	{
		inputfilename = hfst_strdup(argv[optind]);
		if (strcmp(inputfilename, "-") == 0) {
		  inputfilename = hfst_strdup("<stdin>");
		  inputfile = stdin;
		  is_input_stdin = true;
		}
		else {
		  inputfile = hfst_fopen(inputfilename, "r");
		  is_input_stdin = false;
		}
	}
	else if (inputfile) {

	}
	else if ((argc - optind) == 0)
	{
		inputfilename = hfst_strdup("<stdin>");
		inputfile = stdin;
		is_input_stdin = true;
	}
	else if ((argc - optind) > 1)
	{
		fprintf(message_out, "Exactly one input transducer file must be given\n");
		print_short_help(argv[0]);
		return EXIT_FAILURE;
	}
	else
	{
		fprintf(message_out, "???\n");
		return 73;
	}
	return EXIT_CONTINUE;
}

int
process_stream(const char* infilename, const char* outfilename)
{
	HfstInputStream in(infilename);
	in.open();
  
	HfstOutputStream out(outfilename, output_type);
	out.open();
	
	size_t transducer_n = 0;
	while(in.is_good())
	{
		transducer_n++;
		if(transducer_n == 1)
		{ VERBOSE_PRINT("Reading transducer from %s...\n", infilename); }
		else
		{ VERBOSE_PRINT("Reading transducer from %s...%d\n", infilename, transducer_n); }
		
		try {
			HfstTransducer orig(in);
			VERBOSE_PRINT("Converting...\n");
			orig.convert(output_type);
			VERBOSE_PRINT("Writing...\n");
			out << orig;
		}
		catch (const char *p)
		{
			printf("HFST library error: %s\n", p);
			return EXIT_FAILURE;
		}
		
		in.close();
		out.close();
		return EXIT_SUCCESS;
	}
}


int main( int argc, char **argv ) {
	message_out = stdout;
	verbose = false;
	int retval = parse_options(argc, argv);
	if (retval != EXIT_CONTINUE)
	{
		return retval;
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
	VERBOSE_PRINT("Reading from %s, writing to %s\n", 
		inputfilename, outfilename);
	// here starts the buffer handling part
	if (!is_input_stdin)
	{
		retval = process_stream(inputfilename, outfilename);
	}
	else if (is_input_stdin)
	{
		retval = process_stream(NULL, outfilename);
	}
	free(inputfilename);
	free(outfilename);
	return retval;
}

