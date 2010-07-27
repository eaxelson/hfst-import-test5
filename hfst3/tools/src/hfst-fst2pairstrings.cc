//! @file hfst-fst2pairstrings.cc
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
#include <hfst2/hfst.h>


// input transducer
static char *inputfilename;
static FILE *inputfile;
static bool is_input_stdin = true;
// result of inversion
static char *outfilename;
static FILE *outfile;
static bool is_output_stdout = true;

static char *symbolfilename;
// whether spaces are printed between symbol pairs
static bool print_spaces;
// the maximum number of strings printed for each transducer
static int nbest=-1;
// print each string at most once
static bool unique=false;
static bool display_weights=false;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "Display the strings recognized by a transducer\n"
		   "\n"
		   "  -h, --help              Print this help message\n"
		   "  -V, --version           Print version info\n"
		   "  -v, --verbose           Print verbosely while processing\n"
#               if DEBUG
		   "  -d, --debug             Print debugging messages and results\n"
#               endif
		   "  -q, --quiet,            Do not print anything\n"
		   "  -o, --output=OUTFILE    Write results to OUTFILE\n"
		   "  -i, --input=INFILE      Read input from INFILE\n"
		   "  -R, --read-symbols=FILE Use the symbol table in FILE\n"
		   "  -n, --nbest=INT         The maximum number of strings printed\n"
		   "  -w, --weight            Display the weight for each string\n"
		   "  -u, --unique            Print each string at most once\n"
		   "  -r, --random            Select the string randomly, not according to weight.\n"
		   "  -S, --print-spaces      Print spaces between transitions.\n"
		   "\n"
		   "If OUTFILE or INFILE is missing or -,\n"
		   "standard streams will be used.\n"
		   "If a symbol table file is defined, it is used instead "
		   "of the symbol tables of the transducers\n"
		   "\n"
		   "Examples:\n"
		   "  hfst-fst2pairstrings lexical.hfst  generates all forms of lexical.hfst\n"
		   "\n"
		   "Report bugs to HFST team <hfst-bugs@helsinki.fi>\n",
		   program_name);
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
		  	{"read-symbols", required_argument, 0, 'R'},
			{"debug", no_argument, 0, 'd'},
			{"help", no_argument, 0, 'h'},
			{"input", required_argument, 0, 'i'},
			{"nbest", required_argument, 0, 'n'},
			{"output", required_argument, 0, 'o'},
			{"quiet", no_argument, 0, 'q'},
			{"silent", no_argument, 0, 's'},
			{"print-spaces", no_argument, 0, 'S'},
			{"unique", no_argument, 0, 'u'},
			{"verbose", no_argument, 0, 'v'},
			{"version", no_argument, 0, 'V'},
			{"weight", no_argument, 0, 'w'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "R:dhi:n:o:qSsuvVw",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		//char *level = NULL;
		switch (c)
		{
		case 'h':
			print_usage(argv[0]);
			return EXIT_SUCCESS;
			break;
		case 'V':
			// See, this program name should be static since it’s for bug
			// reports, while the other must be argv[0], as it’s useful
			// for end user.
			print_version("hfst-fst2pairstrings");
			return EXIT_SUCCESS;
			break;
		case 'R':
			symbolfilename = hfst_strdup(optarg);
			break;
		case 'o':
			outfilename = hfst_strdup(optarg);
			outfile = hfst_fopen(outfilename, "w");
			is_output_stdout = false;
			message_out = stdout;
			break;
		case 'i':
			inputfilename = hfst_strdup(optarg);
			inputfile = hfst_fopen(inputfilename, "r");
			is_input_stdin = false;
			break;
		case 'n':
			nbest = atoi(hfst_strdup(optarg));
			break;
		case 'v':
			verbose = true;
			break;
		case 'S':
			print_spaces = true;
			break;
		case 'u':
			unique = true;
			break;
		case 'q':
			verbose = false;
			break;
		case 's':
			verbose = false;
			break;
		case 'd':
			debug = true;
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
		inputfile = hfst_fopen(inputfilename, "r");
		is_input_stdin = false;
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
	verbose_printf("Checking formats of transducers\n");
	int format_type = HFST::read_format(inputstream);
    
	if (format_type == SFST_FORMAT)
	{
		verbose_printf("Using unweighted format\n");
		try {
			HFST::TransducerHandle input = NULL;
			HFST::KeyTable *key_table = NULL;
			if (symbolfilename) {
			  ifstream is(symbolfilename);
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
				        if (symbolfilename) {
					  input = HFST::read_transducer(inputstream);
					}
					else if (!HFST::has_symbol_table(inputstream)) {
					  fprintf(message_out, "transducer does not have an alphabet stored with it\n");
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
				if (nbest < 1) {
				  verbose_printf("Finding all paths...\n");
				  try {
				    paths = HFST::find_all_paths(input);
				  }
				  catch (const char *p) {
				    fprintf(message_out, "Transducer is cyclic. Use option -n.\n");
				    return EXIT_FAILURE;
				  }
				}
				else {
				  verbose_printf("Finding best paths...\n");
				  HFST::TransducerHandle paths_transducer = HFST::find_best_paths(input, nbest, unique);
				  paths = HFST::find_all_paths(paths_transducer);
				}

				for (unsigned int i=0; i < paths.size(); i++) {
				  char *path = HFST::transducer_to_pairstring(paths[i], key_table, print_spaces);
				  outstream << path;
				  if (display_weights)
				    outstream << "\t" << HFST::get_weight(paths[i]);
				  outstream<< "\n";
				}
				first_transducer=false;
				//if (!alphabetfilename)
				//  delete key_table;  // not implemented
			}
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
			HWFST::TransducerHandle input = NULL;
			HWFST::KeyTable *key_table = NULL;
			if (symbolfilename) {
			  ifstream is(symbolfilename);
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
				        if (symbolfilename) {
					  input = HWFST::read_transducer(inputstream);
					}
					else if (!HWFST::has_symbol_table(inputstream)) {
					  fprintf(message_out, "transducer does not have an alphabet stored with it\n");
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
				if (nbest < 1) {
				  verbose_printf("Finding all paths...\n");
				  try {
				    paths = HWFST::find_all_paths(input);
				  }
				  catch (const char *p) {
				    fprintf(message_out, "Transducer is cyclic. Use option -n.\n");
				    return EXIT_FAILURE;
				  }
				}
				else {
				  verbose_printf("Finding best paths...\n");
				  HWFST::TransducerHandle paths_transducer = HWFST::find_best_paths(input, nbest, unique);
				  paths = HWFST::find_all_paths(paths_transducer);
				}

				for (unsigned int i=0; i < paths.size(); i++) {
				  char *path = HWFST::transducer_to_pairstring(paths[i], key_table, print_spaces);
				  outstream << path; 
				  if (display_weights)
				    outstream << "\t" << HWFST::get_weight(paths[i]);
				  outstream << "\n";
				}
				first_transducer=false;
				//if (!alphabetfilename)
				//  delete key_table;  // not implemented
			}
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

