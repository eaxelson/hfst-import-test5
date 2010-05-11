//! @file hfst-symbols.cc
//!
//! @brief Symbol table command line tool
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

static char* write_input_symbols_to_filename=NULL;
static char* write_output_symbols_to_filename=NULL;
static bool write_input_symbols=false;
static bool write_output_symbols=false;
static bool include_epsilon=false;

static const Key EPSILON_KEY=0;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		   "A tool for removing, adding and separating symbol tables.\n"
		"\n", program_name);

	print_common_program_options(message_out);
#               if DEBUG
	fprintf(message_out,
		"%-35s%s", "  -d, --debug", "Print debugging messages and results\n"
		);
#               endif
	print_common_unary_program_options(message_out);
	fprintf(message_out, "%-35s%s", "      --write-input-symbols-to=FILE", "Write input symbol table to file FILE\n");
	fprintf(message_out, "%-35s%s", "      --write-output-symbols-to=FILE", "Write output symbol table to file FILE\n");
	fprintf(message_out, "%-35s%s", "      --write-input-symbols", "Write only input symbol table with the transducer\n");
	fprintf(message_out, "%-35s%s", "      --write-output-symbols", "Write only output symbol table with the transducer\n");
	fprintf(message_out, "%-35s%s", "  -e, --include-epsilon", "Include epsilon in the input and output symbol tables\n");
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	/*fprintf(message_out,
		"\n"
		"This tool does nothing to its input transducer.\n"
		"It may be used to remove symbol table from a transducer,\n"
		"add one to a transducer or write one in a separate file.\n"
		"\n");*/
	fprintf(stderr, "\n");
	print_more_info(message_out, "Symbols");
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
		  {"write-input-symbols-to", required_argument, 0, '1'},
		  {"write-output-symbols-to", required_argument, 0, '2'},
		  {"write-input-symbols", no_argument, 0, '3'},
		  {"write-output-symbols", no_argument, 0, '4'},
		  {"include-epsilon", no_argument, 0, 'e'},
		  {0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "dhi:o:sqvVR:DW:1:2:34e",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
#include "hfst-common-cases.h"
#include "hfst-common-unary-cases.h"
		case '1':
			write_input_symbols_to_filename = hfst_strdup(optarg);
			break;
		case '2':
		        write_output_symbols_to_filename = hfst_strdup(optarg);
			break;
		case '3':
			write_input_symbols = true;
			break;
		case '4':
		        write_output_symbols = true;
			break;
		case 'e':
		        include_epsilon = true;
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


KeyTable *get_projection_key_table(KeyTable *key_table, KeySet *ks, bool include_epsilon) {

  KeyTable *result = HFST::create_key_table();
  KeySet::iterator it;
  if (include_epsilon)
    ks->insert(EPSILON_KEY);
  for ( it=ks->begin(); it!=ks->end(); ++it ) {
    if (!HFST::is_key(*it, key_table)) {
      fprintf(message_out, "ERROR: key %hu was not found in the symbol table\n", *it);
      return NULL;
    }
    Symbol s=HFST::get_key_symbol(*it, key_table);
    HFST::associate_key( *it, result, s );
  }
  return result;

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
				VERBOSE_PRINT("Forwarding...\n");

				if (!write_symbols)
				  HFST::write_transducer(input, outstream);
				else if (transducer_has_symbol_table || read_symbols_from_filename != NULL) {

				  if (write_input_symbols && !write_output_symbols) {
				    HFST::TransducerHandle input_projection = HFST::extract_input_language(HFST::copy(input));
				    HFST::KeySet *ks = HFST::define_key_set(input_projection);
				    HFST::delete_transducer(input_projection);
				    HFST::KeyTable *input_key_table = get_projection_key_table(key_table, ks, include_epsilon);
				    if (input_key_table == NULL)
				      return EXIT_FAILURE;
				    delete ks;
				    HFST::write_transducer(input, input_key_table, outstream);
				    delete input_key_table;
				  }
				  else if (write_output_symbols && !write_input_symbols) {
				    HFST::TransducerHandle output_projection = HFST::extract_output_language(HFST::copy(input));
				    HFST::KeySet *ks = HFST::define_key_set(output_projection);
				    HFST::delete_transducer(output_projection);
				    HFST::KeyTable *output_key_table = get_projection_key_table(key_table, ks, include_epsilon);
				    if (output_key_table == NULL)
				      return EXIT_FAILURE;
				    delete ks;
				    HFST::write_transducer(input, output_key_table, outstream);
				    delete output_key_table;
				  }				    
				  else
				    HFST::write_transducer(input, key_table, outstream);
				}
				else
				  HFST::write_transducer(input, outstream);

			}
			if (write_symbols_to_filename != NULL) {
			  ofstream os(write_symbols_to_filename);
			  HFST::write_symbol_table(key_table, os);
			  os.close();
			}
			if (write_input_symbols_to_filename != NULL) {
			  ofstream os(write_input_symbols_to_filename);
			  HFST::TransducerHandle input_projection = HFST::extract_input_language(HFST::copy(input));
			  HFST::KeySet *ks = HFST::define_key_set(input_projection);
			  HFST::delete_transducer(input_projection);
			  HFST::KeyTable *input_key_table = get_projection_key_table(key_table, ks, include_epsilon);
			  delete ks;
			  HFST::write_symbol_table(input_key_table, os);
			  delete input_key_table;
			  os.close();
			}
			if (write_output_symbols_to_filename != NULL) {
			  ofstream os(write_output_symbols_to_filename);
			  HFST::TransducerHandle output_projection = HFST::extract_output_language(HFST::copy(input));
			  HFST::KeySet *ks = HFST::define_key_set(output_projection);
			  HFST::delete_transducer(output_projection);
			  HFST::KeyTable *output_key_table = get_projection_key_table(key_table, ks, include_epsilon);
			  delete ks;
			  HFST::write_symbol_table(output_key_table, os);
			  delete output_key_table;
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
		VERBOSE_PRINT("Using weighted format\n");
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
				VERBOSE_PRINT("Forwarding...\n");

				if (!write_symbols)
				  HWFST::write_transducer(input, outstream);
				else if (transducer_has_symbol_table || read_symbols_from_filename != NULL) {

				  if (write_input_symbols && !write_output_symbols) {
				    HWFST::TransducerHandle input_projection = HWFST::extract_input_language(HWFST::copy(input));
				    HWFST::KeySet *ks = HWFST::define_key_set(input_projection);
				    HWFST::delete_transducer(input_projection);
				    HWFST::KeyTable *input_key_table = get_projection_key_table(key_table, ks, include_epsilon);
				    if (input_key_table == NULL)
				      return EXIT_FAILURE;
				    delete ks;
				    HWFST::write_transducer(input, input_key_table, outstream);
				    delete input_key_table;
				  }
				  else if (write_output_symbols && !write_input_symbols) {
				    HWFST::TransducerHandle output_projection = HWFST::extract_output_language(HWFST::copy(input));
				    HWFST::KeySet *ks = HWFST::define_key_set(output_projection);
				    HWFST::delete_transducer(output_projection);
				    HWFST::KeyTable *output_key_table = get_projection_key_table(key_table, ks, include_epsilon);
				    if (output_key_table == NULL)
				      return EXIT_FAILURE;
				    delete ks;
				    HWFST::write_transducer(input, output_key_table, outstream);
				    delete output_key_table;
				  }				    
				  else
				    HWFST::write_transducer(input, key_table, outstream);
				}
				else
				  HWFST::write_transducer(input, outstream);

			}
			if (write_symbols_to_filename != NULL) {
			  ofstream os(write_symbols_to_filename);
			  HWFST::write_symbol_table(key_table, os);
			  os.close();
			}
			if (write_input_symbols_to_filename != NULL) {
			  ofstream os(write_input_symbols_to_filename);
			  HWFST::TransducerHandle input_projection = HWFST::extract_input_language(HWFST::copy(input));
			  HWFST::KeySet *ks = HWFST::define_key_set(input_projection);
			  HWFST::delete_transducer(input_projection);
			  HWFST::KeyTable *input_key_table = get_projection_key_table(key_table, ks, include_epsilon);
			  delete ks;
			  HWFST::write_symbol_table(input_key_table, os);
			  delete input_key_table;
			  os.close();
			}
			if (write_output_symbols_to_filename != NULL) {
			  ofstream os(write_output_symbols_to_filename);
			  HWFST::TransducerHandle output_projection = HWFST::extract_output_language(HWFST::copy(input));
			  HWFST::KeySet *ks = HWFST::define_key_set(output_projection);
			  HWFST::delete_transducer(output_projection);
			  HWFST::KeyTable *output_key_table = get_projection_key_table(key_table, ks, include_epsilon);
			  delete ks;
			  HWFST::write_symbol_table(output_key_table, os);
			  delete output_key_table;
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

