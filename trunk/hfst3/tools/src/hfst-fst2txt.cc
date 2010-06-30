//! @file hfst-fst2txt.cc
//!
//! @brief Transducer array printing command line tool
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
using hfst::exceptions::NotTransducerStreamException;

// add tools-specific variables here

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Print transducer in AT&T tabular format\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	// fprintf(message_out, (tool-specific options and short descriptions)
	
	fprintf(message_out, "%-35s%s",	"  -o, --output=OUTFILE",    "Print transducer to OUTFILE\n");
	fprintf(message_out, "%-35s%s",	"  -i, --input=INFILE",      "Read input transducer from INFILE\n");
	fprintf(message_out, "%-35s%s",	"  -R, --read-symbols=FILE", "Read symbol table from FILE\n");
	fprintf(message_out,
		"\n"
		"If OUTFILE or INFILE is missing or -,\n"
		"standard streams will be used.\n"
		"\n"
		);
	fprintf(stderr, "\n");
	print_more_info(message_out, "Fst2Txt");
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
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sqvVR:DW:",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
		  // add tool-specific cases here
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
process_stream(HfstInputStream& instream, FILE* outf)
{
	instream.open();
		
	size_t transducer_n = 0;
	while(instream.is_good())
	{
		transducer_n++;
		if(transducer_n == 1)
		{ VERBOSE_PRINT("Converting %s...\n", inputfilename); }
		else
		{ VERBOSE_PRINT("Converting %s...%d\n", inputfilename, transducer_n); }
		
		HfstTransducer t(instream);
		if(transducer_n > 1)
			fprintf(outf, "--\n");
		t.write_in_att_format(outf);
	}
	instream.close();
	fclose(outf);
	return EXIT_SUCCESS;
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
	
	VERBOSE_PRINT("Reading from %s, writing to %s\n", 
		inputfilename, outfilename);
	// here starts the buffer handling part
	HfstInputStream* instream = NULL;
	try {
	  instream = (inputfile != stdin) ?
	    new HfstInputStream(inputfilename) : new HfstInputStream();
	} catch(NotTransducerStreamException)	{
		fprintf(stderr, "%s is not a valid transducer file\n", inputfilename);
		return EXIT_FAILURE;
	}
	
	retval = process_stream(*instream, outfile);
	delete instream;
	free(inputfilename);
	free(outfilename);
	return retval;
}

