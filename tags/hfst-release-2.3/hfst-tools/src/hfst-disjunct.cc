//! @file hfst-diff-test.cc
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


#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include <hfst2/hfst.h>


static bool is_input_stdin = true;
// left part of disjunction
static char *leftfilename;
static FILE *leftfile;
static bool leftNamed = false;
// right part of disjunction
static char *rightfilename;
static FILE *rightfile;
static bool rightNamed = false;
// result of disjunction
static char *outfilename;
static FILE *outfile;
static bool is_output_stdout = true;

static bool use_numbers = false;
static bool write_symbol_table = true;
static const char *write_symbols_to;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
		   "Disjunct two transducers\n"
		"\n", program_name);
		print_common_program_options(message_out);
		print_common_binary_program_options(message_out);
		fprintf(message_out, "\n");
#               if DEBUG
		fprintf(message_out,
			"%-35s%s", "  -d, --debug            Print debugging messages and results\n");
#               endif
		print_common_binary_program_parameter_instructions(message_out);
		fprintf(stderr, "\n");
		print_more_info(message_out, "Disjunct");
		fprintf(stderr, "\n");
		/*fprintf(message_out,
		  "\n"
		  "Examples:\n"
		  "  %s -o 1_2.hfst first.hfst second.hfst  disjuncts "
		   "first.hfst with second.hfst\n"
		   "         writing results to 1_2.hfst\n"
		   "\n" */
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
			{"do-not-write-symbols", no_argument, 0, 'D'},
		  {"write-symbols-to", required_argument, 0, 'W'},
			{"input1", required_argument, 0, '1'},
			{"input2", required_argument, 0, '2'},
			{"number", no_argument, 0, 'n'},
			{"output", required_argument, 0, 'o'},
			{"version", no_argument, 0, 'V'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "1:2:Ddhno:qvVW:s",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		switch (c)
		{
#include "hfst-common-cases.h"
		case 'o':
			outfilename = hfst_strdup(optarg);
			outfile = hfst_fopen(outfilename, "w");
			is_output_stdout = false;
			message_out = stdout;
			break;
		case '1':
			leftfilename = hfst_strdup(optarg);
			leftfile = hfst_fopen(leftfilename, "r");
			is_input_stdin = false;
			leftNamed = true;
			break;
		case '2':
			rightfilename = hfst_strdup(optarg);
			rightfile = hfst_fopen(rightfilename, "r");
			is_input_stdin = false;
			rightNamed = true;
			break;
		case 'n':
			use_numbers = true;
			break;
		case 'D':
			write_symbol_table = false;
			break;
		case 'W':
			write_symbols_to = hfst_strdup(optarg);
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
	if (leftNamed && rightNamed)
	{
		;
	}
	else if (!leftNamed && !rightNamed)
	{
			// neither input given in options:
			if ((argc - optind) == 2)
			{
				leftfilename = hfst_strdup(argv[optind]);
				leftfile = hfst_fopen(leftfilename, "r");
				rightfilename = hfst_strdup(argv[optind+1]);
				rightfile = hfst_fopen(rightfilename, "r");
				is_input_stdin = false;
			}
			else if ((argc - optind) == 1)
			{
				leftfilename = hfst_strdup(argv[optind]);
				leftfile = hfst_fopen(leftfilename, "r");
				rightfilename = hfst_strdup("<stdin>");
				rightfile = stdin;
				is_input_stdin = true;
			}
			else
			{
				fprintf(message_out, 
						"Exactly two transducer files must be given\n");
				print_short_help(argv[0]);
				return EXIT_FAILURE;
			}
	}
	else if (!leftNamed)
	{
		if ((argc - optind) == 1)
		{
			leftfilename = hfst_strdup(argv[optind]);
			leftfile = hfst_fopen(leftfilename, "r");
			is_input_stdin = false;
		}
		else if ((argc - optind) == 0)
		{
			leftfilename = hfst_strdup("<stdin>");
			leftfile = stdin;
			is_input_stdin = true;
		}
	}
	else if (!rightNamed)
	{
		if ((argc - optind) == 1)
		{
			rightfilename = hfst_strdup(argv[optind]);
			rightfile = hfst_fopen(rightfilename, "r");
			is_input_stdin = false;
		}
		else if ((argc - optind) == 0)
		{
			rightfilename = hfst_strdup("<stdin>");
			rightfile = stdin;
			is_input_stdin = true;
		}
	}
	else if (rightNamed && leftNamed) {
	  
	}
	else
	{
		fprintf(message_out,
				"At least one transducer must be read from a file\n");
		print_short_help(argv[0]);
		return EXIT_FAILURE;
	}
	return EXIT_CONTINUE;
}

void delete_u(HFST::TransducerHandle t) {
  if (t != NULL)
    HFST::delete_transducer(t);
}
void delete_w(HWFST::TransducerHandle t) {
  if (t != NULL)
    HWFST::delete_transducer(t);
}

int
disjunct_streams(std::istream& leftstream, std::istream& rightstream, std::ostream& outstream)
{
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = get_compatible_fst_format(leftstream, rightstream);
	size_t nth_stream = 0;
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try {
		        HFST::KeyTable *key_table = HFST::create_key_table();
			HFST::TransducerHandle left = NULL;
			HFST::TransducerHandle right = NULL;
			bool left_has_symbols = false;
			bool right_has_symbols = false;

			while (true) {
				int leftformat = HFST::read_format(leftstream);
				int rightformat = HFST::read_format(rightstream);
				if ((leftformat == EOF_FORMAT) && (rightformat == EOF_FORMAT))
				{
				  delete_u(left);
				  delete_u(right);
					break;
				}
				if (leftformat == SFST_FORMAT)
				{
				  delete_u(left);
				  left_has_symbols = HFST::has_symbol_table(leftstream);
				  if (left_has_symbols && !use_numbers)
				    left = HFST::read_transducer(leftstream, key_table);
				  else
				    left = HFST::read_transducer(leftstream);
				}
				if (rightformat == SFST_FORMAT)
				{
				  delete_u(right);
				  right_has_symbols = HFST::has_symbol_table(rightstream);
				  if (left_has_symbols && right_has_symbols && !use_numbers)
				    right = HFST::read_transducer(rightstream, key_table);
				  else if ((!left_has_symbols && !right_has_symbols) || use_numbers) {
				    if (!use_numbers)
				      fprintf(stderr, "Warning: transducers do not have a symbol table, "
					              "disjunction done using numbers instead\n");
				    right = HFST::read_transducer(rightstream);
				  }
				  else {
				    fprintf(message_out, "Only one transducer has a symbol table: "
					                 "disjunction not well defined\n"
					                 "Use option -n if necessary\n" );
				    return EXIT_FAILURE;
				  }
				}
				nth_stream++;
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Disjuncting...\n");
				}
				else
				{
					VERBOSE_PRINT("Disjuncting... %zu\r", nth_stream);
				}
				HFST::TransducerHandle comp = HFST::disjunct(HFST::copy(left), HFST::copy(right));
				VERBOSE_PRINT("Disjuncted... \r");
				if (write_symbol_table && !use_numbers)
				  HFST::write_transducer(comp, key_table, outstream);
				else
				  HFST::write_transducer(comp, outstream);
				HFST::delete_transducer(comp);
			}
			if (write_symbols_to != NULL) {
			  ofstream os(write_symbols_to);
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
		VERBOSE_PRINT("Using weighted format\n");
		try {
		        HWFST::KeyTable *key_table = HWFST::create_key_table();
			HWFST::TransducerHandle right = NULL;
			HWFST::TransducerHandle left = NULL;
			bool left_has_symbols = false;
			bool right_has_symbols = false;

			while (true) 
			{
				int leftformat = HFST::read_format(leftstream);
				int rightformat = HFST::read_format(rightstream);
				if ((leftformat == EOF_FORMAT) && (rightformat == EOF_FORMAT))
				{
				  delete_w(left);
				  delete_w(right);
					break;
				}
				if (leftformat == OPENFST_FORMAT)
				{
				  delete_w(left);
				  left_has_symbols = HWFST::has_symbol_table(leftstream);
				  if (left_has_symbols && !use_numbers)
				    left = HWFST::read_transducer(leftstream, key_table);
				  else
				    left = HWFST::read_transducer(leftstream);
				}
				if (rightformat == OPENFST_FORMAT)
				{
				  delete_w(right);
				  right_has_symbols = HWFST::has_symbol_table(rightstream);
				  if (left_has_symbols && right_has_symbols && !use_numbers)
				    right = HWFST::read_transducer(rightstream, key_table);
				  else if ((!left_has_symbols && !right_has_symbols) || use_numbers) {
				    if (!use_numbers)
				      fprintf(stderr, "Warning: transducers do not have a symbol table, "
					              "disjunction done using numbers instead\n");
				    right = HWFST::read_transducer(rightstream);
				  }
				  else {
				    fprintf(message_out, "Only one transducer has a symbol table: "
					                 "disjunction not well defined\n"
					                 "Use option -n if necessary\n" );
				    return EXIT_FAILURE;
				  }
				}
				nth_stream++;
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Disjuncting...\n");
				}
				else
				{
					VERBOSE_PRINT("Disjuncting... %zu\r", nth_stream);
				}
				HWFST::TransducerHandle comp = 
				  HWFST::disjunct(HWFST::copy(left), HWFST::copy(right));
				if (write_symbol_table && !use_numbers)
				  HWFST::write_transducer(comp, key_table, outstream);
				else
				  HWFST::write_transducer(comp, outstream);
				HWFST::delete_transducer(comp);
			}
			if (write_symbols_to != NULL) {
			  ofstream os(write_symbols_to);
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
	if (leftfile != stdin)
	{
		fclose(leftfile);
	}
	if (rightfile != stdin)
	{
		fclose(rightfile);
	}
	if (outfile != stdout)
	{
		fclose(outfile);
	}
	VERBOSE_PRINT("Reading from %s and %s, writing to %s\n", 
		leftfilename, rightfilename, outfilename);
	// here starts the buffer handling part
	if ((leftfile != stdin) && (rightfile != stdin))
	{
		std::filebuf fbleft;
		fbleft.open(leftfilename, std::ios::in);
		std::istream leftstream(&fbleft);
		std::filebuf fbright;
		fbright.open(rightfilename, std::ios::in);
		std::istream rightstream(&fbright);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = disjunct_streams(leftstream, rightstream, outstream);
		}
		else
		{
			retval = disjunct_streams(leftstream, rightstream, std::cout);
		}
		return retval;
	}
	else if (leftfile != stdin)
	{
		std::filebuf fbleft;
		fbleft.open(leftfilename, std::ios::in);
		std::istream leftstream(&fbleft);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = disjunct_streams(leftstream, std::cin, outstream);
		}
		else
		{
			retval = disjunct_streams(leftstream, std::cin, std::cout);
		}
		return retval;
	}
	else if (rightfile != stdin)
	{
		std::filebuf fbright;
		fbright.open(rightfilename, std::ios::in);
		std::istream rightstream(&fbright);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = disjunct_streams(std::cin, rightstream, outstream);
		}
		else
		{
			retval = disjunct_streams(std::cin, rightstream, std::cout);
		}
		return retval;
	}
	free(leftfilename);
	free(rightfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

