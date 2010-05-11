//! @file hfst-fst2strings.cc
//!
//! @brief Transducer path printing command line tool
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

// whether spaces are printed between symbol pairs
static bool print_spaces;
// whether epsilon transitions are printed
static bool print_epsilons=false;
// the maximum number of strings printed for each transducer
static int nbest_strings=-1;
static int max_random_strings=-1;
// print each string at most once
static bool unique_strings=false;
static bool display_weights=false;
// print strings in pairstring format
static bool pairstrings=false;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Display the strings recognized by a transducer\n"
		"\n", program_name);
	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	
	fprintf(message_out, "%-35s%s", "  -o, --output=OUTFILE",    "Write results to OUTFILE\n");
	fprintf(message_out, "%-35s%s", "  -i, --input=INFILE",      "Read input from INFILE\n");
	fprintf(message_out, "%-35s%s", "  -R, --read-symbols=FILE", "Read symbol table from FILE\n");
	fprintf(message_out, "%-35s%s", "  -n, --nbest=INT",         "The maximum number of strings printed\n");
	fprintf(message_out, "%-35s%s", "  -w, --print-weights",     "Display the weight for each string\n");
	fprintf(message_out, "%-35s%s", "  -u, --unique",            "Print each string at most once\n");
	fprintf(message_out, "%-35s%s", "  -r, --random=INT",        "Print a maximum of INT strings randomly\n");
	fprintf(message_out, "%-35s%s", "  -S, --print-spaces",      "Print spaces between transitions\n");
	fprintf(message_out, "%-35s%s", "  -p, --pairstring",        "Print the result in pairstring format\n");
	fprintf(message_out, "%-35s%s", "  -e, --print-epsilons",    "Print epsilon transitions\n");
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	/*fprintf(message_out,
		"INFILE cannot be cyclic\n"
		"Option -n defaults to infinity, i.e. all strings are printed\n"
		"\n"
		"Examples:\n"
		"  %s  lexical.hfst  generates all forms of lexical.hfst\n"
		"\n", program_name);*/
	fprintf(stderr, "\n");
	print_more_info(message_out, "Fst2Strings");
	fprintf(stderr, "\n");
	print_report_bugs(message_out);
}

void
print_version(const char* program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(message_out, "%s 0.1 (" PACKAGE_STRING ")\n"
		   "Copyright (C) 2009 University of Helsinki,\n"
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
			{"print-epsilons", no_argument, 0, 'e'},
			{"pairstring", no_argument, 0, 'p'},
			{"random", required_argument, 0, 'r'},
			{"nbest", required_argument, 0, 'n'},
			{"print-spaces", no_argument, 0, 'S'},
			{"unique", no_argument, 0, 'u'},
			{"print-weights", no_argument, 0, 'w'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "SR:dhi:n:o:pqr:suvVwe",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		//char *level = NULL;
		switch (c)
		{
#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
		case 'r':
		        max_random_strings = atoi(hfst_strdup(optarg));
		        break;
		case 'e':
			print_epsilons = true;
			break;
		case 'n':
			nbest_strings = atoi(hfst_strdup(optarg));
			break;
		case 'p':
		        pairstrings = true;
			break;
		case 'S':
			print_spaces = true;
			break;
		case 'u':
			unique_strings = true;
			break;
		case 'w':
			display_weights = true;
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
invert_stream(std::istream& inputstream, std::ostream& outstream)
{
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
    
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try {
			HFST::TransducerHandle input = NULL;
			HFST::KeyTable *key_table = NULL;
			if (read_symbols_from_filename != NULL) {
			  ifstream is(read_symbols_from_filename);
			  key_table = HFST::read_symbol_table(is);
			  is.close();
			}
			bool first_transducer=true;
			while (true) {
				int inputformat = HFST::read_format(inputstream);
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == SFST_FORMAT)
				{				  
				        if (read_symbols_from_filename) {
					  input = HFST::read_transducer(inputstream);
					}
					else if (!HFST::has_symbol_table(inputstream)) {
					  fprintf(message_out, "transducer does not have a symbol table stored with it\n");
					  return EXIT_FAILURE;
					}
					else {
					  key_table = HFST::create_key_table();
					  input = HFST::read_transducer(inputstream, key_table);
					}
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (!first_transducer)
				  outstream << "\n";
				vector<TransducerHandle> paths;
				if (nbest_strings < 1 && max_random_strings < 1) {
				  VERBOSE_PRINT("Finding all paths...\n");
				  try {
				    paths = HFST::find_all_paths(input);
				  }
				  catch (const char *p) {
				    fprintf(message_out, "Transducer is cyclic. Use option -n.\n");
				    return EXIT_FAILURE;
				  }
				}
				else if (max_random_strings > 0) {
				  VERBOSE_PRINT("Finding a maximum of %i random paths...\n", max_random_strings);
				  HFST::TransducerHandle paths_transducer = HFST::find_random_paths(input, max_random_strings);
				  paths = HFST::find_all_paths(paths_transducer);
				  HFST::delete_transducer(paths_transducer);
				}
				else {
				  VERBOSE_PRINT("Finding a maximum of %i paths...\n", nbest_strings);
				  HFST::TransducerHandle paths_transducer = HFST::find_best_paths(input, nbest_strings, unique_strings);
				  paths = HFST::find_all_paths(paths_transducer);
				}

				for (unsigned int i=0; i < paths.size(); i++) {
				  if (pairstrings) {
				    char *path = HFST::transducer_to_pairstring(paths[i], key_table, print_spaces, print_epsilons);
				    outstream << path;
				  }
				  else {
				    HFST::TransducerHandle input_language = HFST::extract_input_language(HFST::copy(paths[i]));
				    HFST::TransducerHandle output_language = HFST::extract_output_language(HFST::copy(paths[i]));
				    char *input_path = HFST::transducer_to_pairstring(input_language, key_table, print_spaces, print_epsilons);
				    char *output_path = HFST::transducer_to_pairstring(output_language, key_table, print_spaces, print_epsilons);
				    HFST::delete_transducer(input_language);
				    HFST::delete_transducer(output_language);
				    outstream << input_path;
				    if (strcmp(input_path, output_path) != 0)
				      outstream << ':' << output_path;
				  }
				  if (display_weights)
				    outstream << "\t" << HFST::get_weight(paths[i]);
				  outstream<< "\n";
				  HFST::delete_transducer(paths[i]);
				}
				first_transducer=false;
				HFST::delete_transducer(input);
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
		VERBOSE_PRINT("Using weighted format\n");
		try {
			HWFST::TransducerHandle input = NULL;
			HWFST::KeyTable *key_table = NULL;
			if (read_symbols_from_filename != NULL) {
			  ifstream is(read_symbols_from_filename);
			  key_table = HWFST::read_symbol_table(is);
			  is.close();
			}
			bool first_transducer=true;
			while (true) {
			        
				int inputformat = HWFST::read_format(inputstream);
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == OPENFST_FORMAT)
				{				  
				        if (read_symbols_from_filename) {
					  input = HWFST::read_transducer(inputstream);
					}
					else if (!HWFST::has_symbol_table(inputstream)) {
					  fprintf(message_out, "transducer does not have a symbol table stored with it\n");
					  return EXIT_FAILURE;
					}
					else {
					  key_table = HWFST::create_key_table();
					  input = HWFST::read_transducer(inputstream, key_table);
					}
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (!first_transducer)
				  outstream << "\n";

				vector<TransducerHandle> paths;
				if (nbest_strings < 1 && max_random_strings < 1) {
				  VERBOSE_PRINT("Finding all paths...\n");
				  try {
				    paths = HWFST::find_all_paths(input);
				  }
				  catch (const char *p) {
				    fprintf(message_out, "Transducer is cyclic. Use option -n.\n");
				    return EXIT_FAILURE;
				  }
				}
				else if (max_random_strings > 0) {
				  VERBOSE_PRINT("Finding a maximum of %i random paths...\n", max_random_strings);
				  HWFST::TransducerHandle paths_transducer = HWFST::find_random_paths(input, max_random_strings, false);
				  paths = HWFST::find_all_paths(paths_transducer);
				}
				else {
				  VERBOSE_PRINT("Finding a maximum of %i paths...\n", nbest_strings);
				  HWFST::TransducerHandle paths_transducer = HWFST::find_best_paths(input, nbest_strings, unique_strings);
				  paths = HWFST::find_all_paths(paths_transducer);
				  HWFST::delete_transducer(paths_transducer);
				}

				for (unsigned int i=0; i < paths.size(); i++) {
				  if (pairstrings) {
				    char *path = HWFST::transducer_to_pairstring(paths[i], key_table, print_spaces, print_epsilons);
				    outstream << path;
				  }
				  else {
				    HWFST::TransducerHandle input_language = HWFST::extract_input_language(HWFST::copy(paths[i]));
				    HWFST::TransducerHandle output_language = HWFST::extract_output_language(HWFST::copy(paths[i]));
				    char *input_path = HWFST::transducer_to_pairstring(input_language, key_table, print_spaces, print_epsilons);
				    char *output_path = HWFST::transducer_to_pairstring(output_language, key_table, print_spaces, print_epsilons);
				    HWFST::delete_transducer(input_language);
				    HWFST::delete_transducer(output_language);
				    outstream << input_path;
				    if (strcmp(input_path, output_path) != 0)
				      outstream << ':' << output_path;
				  }

				  if (display_weights)
				    outstream << "\t" << HWFST::get_weight(paths[i]);
				  outstream << "\n";
				  HWFST::delete_transducer(paths[i]);
				}
				first_transducer=false;
				//if (!alphabetfilename)
				//  delete key_table;  // not implemented
				HWFST::delete_transducer(input);
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
		fprintf(message_out, "ERROR: Transducer has wrong type: %i\n", format_type);
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
			retval = invert_stream(inputstream, outstream);
		}
		else
		{
			retval = invert_stream(inputstream, std::cout);
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
			retval = invert_stream(std::cin, outstream);
		}
		else
		{
			retval = invert_stream(std::cin, std::cout);
		}
		return retval;
	}
	free(inputfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

