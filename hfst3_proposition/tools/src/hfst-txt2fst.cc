//! @file hfst-txt2fst.cc
//!
//! @brief Transducer text compiling command line tool
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
#include <hfst2/hfst.h>

#include "hfst-common-unary-variables.h"
// add tools-specific variables here
// whether the result is in weighted format
static bool is_weighted=false;
// whether numbers are used instead of symbol names
static bool use_numbers=false;
// printname for epsilon
static char *epsilonname=NULL;
static const unsigned int EPSILON_KEY=0;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "Convert AT&T tabular format into a binary transducer\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	// fprintf(message_out, (tool-specific options and short descriptions)
	fprintf(message_out, "%-35s%s", "  -n, --number", "If numbers are used instead of symbol names in transitions\n");
	fprintf(message_out, "%-35s%s", "  -w, --weighted", "Write transducer in weighted format\n");
	fprintf(message_out, "%-35s%s", "  -e, --epsilon=EPS", "If no symbol table is given, map EPS as zero.\n");
	fprintf(message_out, "\n");
	fprintf(message_out, 
		"If OUTFILE or INFILE is missing or -,"
		"standard streams will be used.\n\n"
		);
	/*fprintf(message_out, 
		"If symbol names are used and no symbol table is given, each transducer constructs its own symbol table.\n"
		"In this case, the epsilon name must be given so the program knows to map it to zero." 
		"\n\n"
		);*/
	fprintf(stderr, "\n");
	print_more_info(message_out, "Txt2Fst");
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
			{"epsilon", required_argument, 0, 'e'},
			{"number", no_argument, 0, 'n'},
			{"weighted", no_argument, 0, 'w'}, 
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sqvVR:DW:e:nw",
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
		case 'e':
		        epsilonname = hfst_strdup(optarg);
			break;
		case 'n':
			use_numbers = true;
			break;
		case 'w':
			is_weighted = true;
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
process_stream(std::istream& inputstream, std::ostream& outstream)
{

	if (!read_symbols_from_filename && !use_numbers && !epsilonname) {
	  fprintf(message_out, "No alphabet file defined. Define epsilon symbol with option -e\n");
	  return EXIT_FAILURE;
	}
	if (read_symbols_from_filename && use_numbers) {
	  fprintf(message_out, "Options -R and -n are not compatible.\n");
	  return EXIT_FAILURE;
	}

	if (!is_weighted)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try {
		  HFST::KeyTable *key_table=NULL;
		  if (read_symbols_from_filename != NULL) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HFST::read_symbol_table(is);
		    is.close();
		  }
		  else if (!use_numbers) {
		    key_table = HFST::create_key_table();
		    HFST::associate_key(EPSILON_KEY, key_table, HFST::define_symbol(epsilonname));
		  }

		  while (true) {
		    HFST::TransducerHandle input;
		    if (!use_numbers) {
		      input = HFST::read_transducer_text(inputstream, key_table);
		    }
		    else {
		      input = HFST::read_transducer_number(inputstream);
		    }
		    if (input == NULL) // end of file
		      break;
		    
		    if (write_symbols && !use_numbers) {
		      HFST::write_transducer(input, key_table, outstream);
		    }
		    else {
		      HFST::write_transducer(input, outstream);
		    }
		  }

		  if (write_symbols_to_filename != NULL) {
		    ofstream os(write_symbols_to_filename);
		    HFST::write_symbol_table(key_table, os);
		    os.close();
		  }
		  delete key_table;
		}
		catch (const char *p)
		{
			printf("HFST library error: %s\n", p);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	else 
	{
		VERBOSE_PRINT("Using weighted format\n");
		try {
		  HWFST::KeyTable *key_table=NULL;
		  if (read_symbols_from_filename != NULL) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HWFST::read_symbol_table(is);
		    is.close();
		  }
		  else if (!use_numbers) {
		    key_table = HWFST::create_key_table();
		    HWFST::associate_key(EPSILON_KEY, key_table, HWFST::define_symbol(epsilonname));
		  }

		  while (true) {
		    HWFST::TransducerHandle input;
		    if (!use_numbers) {
		      input = HWFST::read_transducer_text(inputstream, key_table);
		    }
		    else
		      input = HWFST::read_transducer_number(inputstream);
		    if (input == NULL) // end of file
		      break;
		    
		    if (write_symbols && !use_numbers)
		      HWFST::write_transducer(input, key_table, outstream);
		    else
		      HWFST::write_transducer(input, outstream);
		  }

		  if (write_symbols_to_filename != NULL) {
		    ofstream os(write_symbols_to_filename);
		    HWFST::write_symbol_table(key_table, os);
		    os.close();
		  }		  
		  delete key_table;
		}
		catch (const char *p) {
			fprintf(message_out, "HFST lib error: %s\n", p);
			return 1;
		}
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
		std::filebuf fbinput;
		fbinput.open(inputfilename, std::ios::in);
		std::istream inputstream(&fbinput);
		if (!is_output_stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = process_stream(inputstream, outstream);
		}
		else
		{
			retval = process_stream(inputstream, std::cout);
		}
		return retval;
	}
	else if (is_input_stdin)
	{
		if (!is_output_stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = process_stream(std::cin, outstream);
		}
		else
		{
			retval = process_stream(std::cin, std::cout);
		}
		return retval;
	}
	free(inputfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

