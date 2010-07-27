//! @file hfst-substitute.cc
//!
//! @brief Transducer label modification
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

#if NESTED_BUILD
#include <hfst2/string/string.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#endif

#include "inc/globals-unary.h"

static char* from_label = 0;
static char* from_file_name = 0;
static FILE* from_file = 0;
static char* to_label = 0;
static char* to_transducer_filename = 0;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "Relabel transducer arcs\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	fprintf(message_out,
			"%-35s%s", "  -f, --from-label=LABEL", "replace LABEL\n"
		   );
	fprintf(message_out,
			"%-35s%s", "  -t, --to-label=LABEL", "replace with LABEL\n"
		   );
	fprintf(message_out,
			"%-35s%s", "  -T, --to-transducer=FILE", 
			"replace with transducer read from FILE\n"
		   );
	print_common_unary_program_options(message_out);
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	/*fprintf(message_out,
		   "\n"
		   "Examples:\n"
		   "  %s -o deterministic.hfst indeterministic.hfst  determinizes "
		   "indeterministic.hfst \n"
		   "         writing results to deterministic.hfst\n"
		   "\n", program_name);*/
	fprintf(stderr, "\n");
	print_more_info(message_out, "Substitute");
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
			{"from-label", required_argument, 0, 'f'},
			{"from-file", required_argument, 0, 'F'},
			{"to-label", required_argument, 0, 't'},
			{"to-transducer", required_argument, 0, 'T'},
			{0,0,0,0}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "f:F:dhi:o:st:T:qvVR:DW:",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		FILE* f = 0;
		switch (c)
		{
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
		  // add tool-specific cases here
		case 'f':
			from_label = hfst_strdup(optarg);
			break;
		case 'F':
			from_file_name = hfst_strdup(optarg);
			from_file = hfst_fopen(from_file_name, "r");
			if (from_file == NULL)
			{
				return EXIT_FAILURE;
			}
			break;
		case 't':
			to_label = hfst_strdup(optarg);
			break;
		case 'T':
			to_transducer_filename = hfst_strdup(optarg);
			f = hfst_fopen(to_transducer_filename, "r");
			if (f == NULL)
			{
				return EXIT_FAILURE;
			}
			fclose(f);
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
	
	if ((from_label == 0) && (from_file_name == 0))
	{
		fprintf(message_out, "Must state name of labels to rewrite with -f or -F\n");
		print_short_help(argv[0]);
		return EXIT_FAILURE;
	}
	if ((to_label == 0) && (to_transducer_filename == 0) && 
			(from_file_name == 0))
	{
		fprintf(message_out, "Must give target labels with -t, -T or -F\n");
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
process_stream(std::istream& inputstream, std::ostream& outstream)
{
	verbose_printf("Checking formats of transducers\n");
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
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (from_file != 0)
				{
					char* line = 0;
                    size_t len;
					while (hfst_getline(&line, &len, from_file) != -1)
					{
						char* end = line;
						while (*end != '\0')
						{
							if (*end == '\n')
							{
								*end = '\0';
								break;
							}
							end++;
						}
						char* tabspot = strchr(line, '\t');
						if (tabspot == 0)
						{
							fprintf(message_out, "Broken relabeling line in %s\n"
									"%s", from_file_name, line);
							continue;
						}
						from_label = static_cast<char*>(calloc(
									sizeof(char), (tabspot - line + 1)));
						from_label = static_cast<char*>(memcpy(from_label,
									line, (tabspot - line)));
						to_label = static_cast<char*>(calloc(
									sizeof(char), (end - tabspot + 1)));
						to_label = static_cast<char*>(memcpy(to_label,
									tabspot+1, (end - tabspot)));
						verbose_printf("Substituting %s with %s...\n", 
								from_label, to_label);
						HFST::Key from_key = HFST::stringToKey(from_label,
								key_table);
						HFST::Key to_key = HFST::stringToKey(to_label,
								key_table);
						input = HFST::substitute_key(input, from_key, to_key);
						free(from_label);
						free(to_label);
					} // while line
                    free(line);
				}
				else if ((from_label != 0) && (to_label != 0))
				{
					verbose_printf("Substituting %s with %s...\n",
							from_label, to_label);
					HFST::Key from_key = HFST::stringToKey(from_label,
							key_table);
					HFST::Key to_key = HFST::stringToKey(to_label, key_table);
					input = HFST::substitute_key(input, from_key, to_key);
				}
				else if ((from_label != 0) && (to_transducer_filename != 0))
				{
					verbose_printf("Substituting %s with transducer "
							"from %s...\n", from_label, to_transducer_filename);
					HFST::Key from_key = HFST::stringToKey(from_label,
							key_table);
					HFST::KeyPair* from_kp = HFST::define_keypair(from_key,
							from_key);
					std::filebuf fbto;
					fbto.open(to_transducer_filename, std::ios::in);
					std::istream tostream(&fbto);
					int to_format = HFST::read_format(tostream);
					if (to_format != SFST_FORMAT)
					{
						fprintf(message_out, "Mismatching format in "
								"replacement trasnducer file %s\n",
								to_transducer_filename);
						return EXIT_FAILURE;
					}
					HFST::TransducerHandle replacement = HFST::read_transducer(
							tostream, key_table);
					input = HFST::substitute_with_transducer(input,
							from_kp, replacement);
				}
				HFST::TransducerHandle result = input;

				if (!write_symbols)
				  HFST::write_transducer(result, outstream);
				else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				  HFST::write_transducer(result, key_table, outstream);
				else
				  HFST::write_transducer(result, outstream);

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
			while (true) {
				int inputformat = HWFST::read_format(inputstream);
				if (inputformat == EOF_FORMAT)
				{
					break;
				}
				else if (inputformat == OPENFST_FORMAT)
				{
				        transducer_has_symbol_table = HWFST::has_symbol_table(inputstream);
					input = HWFST::read_transducer(inputstream, key_table);
				}
				else
				{
					fprintf(message_out, "stream format mismatch\n");
					return EXIT_FAILURE;
				}
				if (from_file != 0)
				{
					char* line = 0;
                    size_t len;
					while (hfst_getline(&line, &len, from_file) != -1)
					{
						char* end = line;
						while (*end != '\0')
						{
							if (*end == '\n')
							{
								*end = '\0';
								break;
							}
							end++;
						}
						char* tabspot = strchr(line, '\t');
						if (tabspot == 0)
						{
							fprintf(message_out, "Broken relabeling line in %s\n"
									"%s", from_file_name, line);
							continue;
						}
						from_label = static_cast<char*>(calloc(
									sizeof(char), (tabspot - line + 1)));
						from_label = static_cast<char*>(memcpy(from_label,
									line, (tabspot - line)));
						to_label = static_cast<char*>(calloc(
									sizeof(char), (end - tabspot + 1)));
						to_label = static_cast<char*>(memcpy(to_label,
									tabspot + 1, (end - tabspot)));
						verbose_printf("Substituting %s with %s...\n", 
								from_label, to_label);
						HWFST::Key from_key = HWFST::stringToKey(from_label,
								key_table);
						HWFST::Key to_key = HWFST::stringToKey(to_label,
								key_table);
						input = HWFST::substitute_key(input, from_key, to_key);
						free(from_label);
						free(to_label);
					} // while line
				    free(line);
                }
				else if ((from_label != 0) && (to_label != 0))
				{
					verbose_printf("Substituting %s with %s...\n",
							from_label, to_label);
					HWFST::Key from_key = HWFST::stringToKey(from_label,
							key_table);
					HWFST::Key to_key = HWFST::stringToKey(to_label, key_table);
					input = HWFST::substitute_key(input, from_key, to_key);
				}
				else if ((from_label != 0) && (to_transducer_filename != 0))
				{
					verbose_printf("Substituting %s with transducer "
							"from %s...\n", from_label, to_transducer_filename);
					HWFST::Key from_key = HWFST::stringToKey(from_label,
							key_table);
					HWFST::KeyPair* from_kp = HWFST::define_keypair(from_key,
							from_key);
					std::filebuf fbto;
					fbto.open(to_transducer_filename, std::ios::in);
					std::istream tostream(&fbto);
					int to_format = HWFST::read_format(tostream);
					if (to_format != OPENFST_FORMAT)
					{
						fprintf(message_out, "Mismatching format in "
								"replacement trasnducer file %s\n",
								to_transducer_filename);
						return EXIT_FAILURE;
					}
					HWFST::TransducerHandle replacement = HWFST::read_transducer(
							tostream, key_table);
					input = HWFST::substitute_with_transducer(input,
							from_kp, replacement);
				}
				HWFST::TransducerHandle result = input;

				if (!write_symbols)
				  HWFST::write_transducer(result, outstream);
				else if (transducer_has_symbol_table || read_symbols_from_filename != NULL)
				  HWFST::write_transducer(result, key_table, outstream);
				else
				  HWFST::write_transducer(result, outstream);
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

