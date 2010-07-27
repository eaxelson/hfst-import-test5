//! @file hfst-lookup-optimize.cc
//!
//! @brief Diff-test command line tool
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

#include <getopt.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include <hfst2/hfst.h>

#include "inc/globals-unary.h"


void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Convert a transducer to an optimized lookup format\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	fprintf(stderr, "\n");
	print_more_info(message_out, "Make Optimized Lookup Transducer");
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
		HFST_GETOPT_COMMON_LONG
		  ,
		HFST_GETOPT_UNARY_LONG
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
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
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

int process_stream(std::istream& inputstream, FILE * outstream)
{
  verbose_printf("Checking format of transducer\n");
	int format_type = HFST::read_format(inputstream);
    
	if (format_type == SFST_FORMAT)
	{
		verbose_printf("Using unweighted format\n");
		try {
		  
		  HFST::KeyTable *key_table;
		  if (read_symbols_from_filename != NULL) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HFST::read_symbol_table(is);
		    is.close();
		  }
		  else
		    key_table = HFST::create_key_table();
		  bool transducer_has_symbol_table=false;
			HFST::TransducerHandle input = NULL;
			while (true) {
				int inputformat = HFST::read_format(inputstream);
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == SFST_FORMAT)
				{
				        transducer_has_symbol_table = HFST::has_symbol_table(inputstream);
					input = HFST::read_transducer(inputstream, key_table);
					HFST::KeyTable * minimized_key_table = HFST::minimize_key_table(key_table,input);
					HFST::KeyTable * minimized_copy = new HFST::KeyTable(*minimized_key_table);
					input = HFST::harmonize_transducer(input,key_table,minimized_copy);
					delete key_table;
					delete minimized_copy;
					key_table = minimized_key_table;
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				verbose_printf("Converting to optimized lookup format...\n");
				// add your code here
				HFST::write_runtime_transducer(input,
							       key_table,
							       outstream);
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
	else if (format_type == OPENFST_FORMAT) 
	{
		verbose_printf("Using weighted format\n");
		try {
		  HWFST::KeyTable *key_table;
		  if (read_symbols_from_filename != NULL) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HWFST::read_symbol_table(is);
		    is.close();
		  }
		  else
		    key_table = HWFST::create_key_table();
		  bool transducer_has_symbol_table=false;
			HWFST::TransducerHandle input = NULL;
			while (true) 
			{
				int inputformat = HFST::read_format(inputstream);
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == OPENFST_FORMAT)
				{
				        transducer_has_symbol_table = HWFST::has_symbol_table(inputstream);
					input = HWFST::read_transducer(inputstream, key_table);
					HFST::KeyTable * minimized_key_table = HWFST::minimize_key_table(key_table,input);
					HFST::KeyTable * minimized_copy = new HFST::KeyTable(*minimized_key_table);
					input = HWFST::harmonize_transducer(input,key_table,minimized_copy);
					delete key_table;
					delete minimized_copy;
					key_table = minimized_key_table;
				}
				else {
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				// add your code here
				HWFST::write_runtime_transducer(input,
								key_table,
								outstream);

			}
			delete key_table;
		}
		catch (const char *p) {
			fprintf(message_out, "HFST lib error: %s\n", p);
			return 1;
		}
		return EXIT_SUCCESS;
	}
	else
	{
		fprintf(message_out, "ERROR: Transducer has wrong type.\n");
		return EXIT_FAILURE;
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
	verbose_printf("Reading from %s, writing to %s\n", 
		inputfilename, outfilename);
	// here starts the buffer handling part
	if (!is_input_stdin)
	{
		std::filebuf fbinput;
		fbinput.open(inputfilename, std::ios::in);
		std::istream inputstream(&fbinput);
		if (!is_output_stdout)
		{
		  FILE * outstream = fopen(outfilename,"w");
		  retval = process_stream(inputstream, outstream);
		}
		else
		{		  
		  retval = process_stream(inputstream, stdout);
		}
		return retval;
	}
	else if (is_input_stdin)
	{
		if (!is_output_stdout)
		{
			FILE * outstream = fopen(outfilename,"w");
			retval = process_stream(std::cin, outstream);
		}
		else
		{
			retval = process_stream(std::cin, stdout);
		}
		return retval;
	}
	free(inputfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}



