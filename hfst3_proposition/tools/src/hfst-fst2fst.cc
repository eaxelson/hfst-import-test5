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
#include <hfst2/hfst.h>

#include "hfst-common-unary-variables.h"

// tool-specific variables

// compatibility
static bool to_sfst=false;
static bool to_openfst=false;

// weightedness
static bool unweighted=false;
static bool weighteD=false;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Convert transducers between SFST, OpenFst and HFST formats\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		   "  -d, --debug            Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	// fprintf(message_out, (tool-specific options and short descriptions)
	fprintf(message_out, "%-35s%s", "  -u, --unweighted",        "Write the output in unweighted HFST format\n");
	fprintf(message_out, "%-35s%s", "  -w, --weighted",          "Write the output in weighted HFST format\n");
	fprintf(message_out, "%-35s%s", "  -f, --format=FORMAT", "Write the output in format FORMAT\n");
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	fprintf(message_out,
		"\nFORMAT is one of {sfst, openfst}\n");
	/*
		"\n"
		"If the input transducer is converted to OpenFst format, the symbol table is copied\n"
		"to input symbol table of the OpenFst transducer and vice versa (unless option -D is used).\n"
		"If the input transducer is converted to SFST format, a symbol table is always written\n"
		"with the SFST transducer, even if it is empty (i.e. option -D has no effect).\n"
		"\n");*/
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
			{"unweighted", no_argument, 0, 'u'},
			{"weighted", no_argument, 0, 'w'},
			{"format", required_argument, 0, 'f'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sqvVR:DW:f:wu",
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
		case 'u':
		        unweighted=true;
			break;
		case 'w':
		        weighteD=true;
			break;
		case 'f':
			format = hfst_strdup(optarg);
			if ( (strncasecmp(format, "sfst", 1) == 0))
			  to_sfst=true;
			else if ( (strncasecmp(format, "openfst", 1) == 0))
			  to_openfst=true;
			else
			{
			  fprintf(message_out, "unknown conversion format %s\n" \
				  "should be one of {sfst, openfst}\n",	\
				  format);
			  print_short_help(argv[0]);
			  return EXIT_FAILURE;
			}
			free(format);
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
		if (unweighted && weighteD) {
		  fprintf(message_out, "incompatible options -u and -w\n");
		  print_short_help(argv[0]);
		  return EXIT_FAILURE;
		}
		if ((unweighted || weighteD) && (to_sfst || to_openfst)) {
		  fprintf(stderr, "warning: options -u and -w have no effect when used with option -c\n");
		  //print_short_help(argv[0]);
		  //return EXIT_FAILURE;
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
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
    
	if (format_type == SFST_FORMAT)
	{
	        VERBOSE_PRINT("Reading unweighted format\n");
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
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}


				if (weighteD == false && to_openfst == false) {  // no need to convert to weighted format
				  if (to_sfst) {
				    VERBOSE_PRINT("Writing SFST transducer...\n");
				  }
				  else {
				    VERBOSE_PRINT("Writing unweighted HFST transducer...\n");
				  }

				  HFST::TransducerHandle result = input;
				  if (to_sfst)  // write a key table, even an empty one
				    HFST::write_transducer(result, key_table, outstream, to_sfst);
				  else {
				    if (!write_symbols)
				      HFST::write_transducer(result, outstream);
				    else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				      HFST::write_transducer(result, key_table, outstream);
				    else
				      HFST::write_transducer(result, outstream);
				  }
				}
				else {  // convert to weighted format
				  if (to_openfst) {
				    VERBOSE_PRINT("Writing OpenFst transducer...\n");
				  }
				  else {
				    VERBOSE_PRINT("Writing weighted HFST transducer...\n");
				  }
				  
				  const char *tempfilename="__TEMPORARY_FILE__";
				  ofstream os(tempfilename);
				  HFST::print_transducer_number(input, false, os);
				  os.close();
				  ifstream is(tempfilename);
				  HWFST::TransducerHandle result = HWFST::read_transducer_number(is);
				  is.close();
				  remove(tempfilename);

				  if (!write_symbols)
				    HWFST::write_transducer(result, outstream, to_openfst);
				  else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				    HWFST::write_transducer(result, key_table, outstream, to_openfst);
				  else
				    HWFST::write_transducer(result, outstream, to_openfst);
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
	else if (format_type == OPENFST_FORMAT) 
	{
		VERBOSE_PRINT("Reading weighted format\n");
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
				}
				else {
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}

				if (!transducer_has_symbol_table) {
				  // see if the OpenFst transducer has an input symbol table
				  // and convert it to HWFST KeyTable
				  if (HWFST::has_ofst_input_symbol_table(input)) {
				    key_table = HWFST::to_hwfst_compatible_format(input);
				    transducer_has_symbol_table=true;
				  }
				}
				
				if (unweighted == false && to_sfst == false) {  // no need to convert to unweighted format
				  if (to_openfst) {
				    VERBOSE_PRINT("Writing OpenFst transducer...\n");
				  }
				  else {
				    VERBOSE_PRINT("Writing weighted HFST transducer...\n");
				  }

				  HWFST::TransducerHandle result = input;
				  
				  if (!write_symbols)
				    HWFST::write_transducer(result, outstream, to_openfst);
				  else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				    HWFST::write_transducer(result, key_table, outstream, to_openfst);
				  else
				    HWFST::write_transducer(result, outstream, to_openfst);
				}

				else {  // convert to unweighted format
				  if (to_sfst) {
				    VERBOSE_PRINT("Writing SFST transducer...\n");
				  }
				  else {
				    VERBOSE_PRINT("Writing unweighted HFST transducer...\n");
				  }

				  const char *tempfilename="__TEMPORARY_FILE__";
				  
				  ofstream os(tempfilename);
				  HWFST::print_transducer_number(input, false, os);
				  os.close();
				  ifstream is(tempfilename);
				  HFST::TransducerHandle result = HFST::read_transducer_number(is);
				  is.close();
				  remove(tempfilename);

				  if (to_sfst)  // write a key table, even an empty one
				    HFST::write_transducer(result, key_table, outstream, to_sfst);
				  else {
				    if (!write_symbols)
				      HFST::write_transducer(result, outstream);
				    else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				      HFST::write_transducer(result, key_table, outstream);
				    else
				      HFST::write_transducer(result, outstream);
				  }

				}
				
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

