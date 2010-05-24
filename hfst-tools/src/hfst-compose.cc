//! @file hfst-compose.cc
//!
//! @brief Transducer composition command line tool
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
// left part of composition
static char *upperfilename;
static FILE *upperfile;
static bool upperNamed = false;
// right part of composition
static char *lowerfilename;
static FILE *lowerfile;
static bool lowerNamed = false;
// result of composition
static char *outfilename;
static FILE *outfile;
static bool is_output_stdout = true;

static bool use_numbers=false;
static bool write_symbol_table=true;
static bool handle_flag_diacritics=false;
static const char *write_symbols_to;


void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
		   "Compose two transducers\n"
		"\n", program_name );
		print_common_program_options(message_out);
		fprintf(message_out, "%-35s%s", "  -F, --flag-diacritics",  "Handle flag diacritics\n");
		print_common_binary_program_options(message_out);
		fprintf(message_out, "\n");
#               if DEBUG
		fprintf(message_out,
			"%-35s%s", "  -d, --debug            Print debugging messages and results\n");
#               endif
		print_common_binary_program_parameter_instructions(message_out);
		/*fprintf(message_out,
			"\n"
			"Examples:\n"
			"  %s -o composition.hfst first.hfst second.hfst  composes "
			"first.hfst with second.hfst\n"
			"         writing results to composition.hfst\n"
			"\n",
			program_name );*/
		fprintf(stderr, "\n");
		print_more_info(message_out, "Compose");
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
			{"do-not-write-symbols", no_argument, 0, 'D'},
			{"input1", required_argument, 0, '1'},
			{"input2", required_argument, 0, '2'},
			{"number", no_argument, 0, 'n'},
			{"output", required_argument, 0, 'o'},
			{"write-symbols-to", required_argument, 0, 'W'},
		        {"flag-diacritics",no_argument, 0, 'F'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "1:2:dDhno:qsvVW:F",
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
			if (strcmp(outfilename, "-") != 0)
			{
				outfile = hfst_fopen(outfilename, "w");
				is_output_stdout = false;
				message_out = stdout;
			}
			else
			{
				free(outfilename);
				outfilename = hfst_strdup("<stdout>");
				message_out = stderr;
			}
			break;
		case '1':
			upperfilename = hfst_strdup(optarg);
			if (strcmp(upperfilename, "-") != 0)
			{
				upperfile = hfst_fopen(upperfilename, "r");
				is_input_stdin = false;
				upperNamed = true;
			}
			else
			{
				free(upperfilename);
				upperfilename = hfst_strdup("<stdin>");
				upperfile = stdin;
			}
			break;
		case '2':
			lowerfilename = hfst_strdup(optarg);
			if (strcmp(lowerfilename, "-") != 0)
			{
				lowerfile = hfst_fopen(lowerfilename, "r");
				is_input_stdin = false;
				lowerNamed = true;
			}
			else
			{
				free(lowerfilename);
				lowerfilename = hfst_strdup("<stdin>");
				lowerfile = stdin;
			}
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
		case 'F':
		        handle_flag_diacritics = true;
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

	if (upperNamed && lowerNamed)
	{
		;
	}
	else if (!upperNamed && !lowerNamed)
	{
			// neither input given in options:
			if ((argc - optind) == 2)
			{
				upperfilename = hfst_strdup(argv[optind]);
				lowerfilename = hfst_strdup(argv[optind+1]);
				if ( (strcmp(upperfilename, "-") == 0) &&
					 (strcmp(lowerfilename, "-") == 0) )
				{
					fprintf(message_out, 
						"Both files cannot be standard streams\n");
					print_short_help(argv[0]);
					return EXIT_FAILURE;
				}
				if (strcmp(upperfilename, "-") != 0)
				{
					upperfile = hfst_fopen(upperfilename, "r");
					is_input_stdin = false;
				}
				else
				{
					free(upperfilename);
					upperfilename = strdup("<stdin>");
					upperfile = stdin;
					is_input_stdin = true;
				}
				if (strcmp(lowerfilename, "-") != 0)
				{
					lowerfile = hfst_fopen(lowerfilename, "r");
					is_input_stdin = false;
				}
				else
				{
					free(lowerfilename);
					lowerfilename = strdup("<stdin>");
					lowerfile = stdin;
					is_input_stdin = true;
				}
			}
			else if ((argc - optind) == 1)
			{
				upperfilename = hfst_strdup(argv[optind]);
				if (strcmp(upperfilename, "-") == 0)
				{
					fprintf(message_out,
						"Both transducers cannot be read from standard input\n");
					print_short_help(argv[0]);
				}
				upperfile = hfst_fopen(upperfilename, "r");
				lowerfilename = hfst_strdup("<stdin>");
				lowerfile = stdin;
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
	else if (!upperNamed)
	{
		if ((argc - optind) == 1)
		{
			upperfilename = hfst_strdup(argv[optind]);
			if (strcmp(upperfilename, "-") != 0)
			{
				upperfile = hfst_fopen(upperfilename, "r");
				is_input_stdin = false;
			}
			else
			{
				free(upperfilename);
				upperfilename = hfst_strdup("<stdin>");
				is_input_stdin = true;
			}
		}
		else if ((argc - optind) == 0)
		{
			upperfilename = hfst_strdup("<stdin>");
			upperfile = stdin;
			is_input_stdin = true;
		}
	}
	else if (!lowerNamed)
	{
		if ((argc - optind) == 1)
		{
			lowerfilename = hfst_strdup(argv[optind]);
			fprintf(stderr, "lowerfilename is %s\n", lowerfilename);
			if (strcmp(lowerfilename, "-") != 0)
			{
				lowerfile = hfst_fopen(lowerfilename, "r");
				is_input_stdin = false;
			}
			else
			{
				free(lowerfilename);
				lowerfilename = strdup("<stdin>");
				is_input_stdin = true;
			}
		}
		else if ((argc - optind) == 0)
		{
			lowerfilename = hfst_strdup("<stdin>");
			lowerfile = stdin;
			is_input_stdin = true;
		}
	}
	else if (upperNamed && lowerNamed) {
	  
	}
	else
	{
		fprintf(message_out,
				"at least one transducer filename must be given\n");
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
compose_streams(std::istream& upperstream, std::istream& lowerstream, std::ostream& outstream)
{
	VERBOSE_PRINT("Checking formats of transducers\n");
	int format_type = get_compatible_fst_format(upperstream, lowerstream);
	size_t nth_stream = 0;
    
	if (handle_flag_diacritics)
	  {
	    VERBOSE_PRINT("Minding flag diacritics in argument transducers.");
	  }
	if (format_type == SFST_FORMAT)
	{
		VERBOSE_PRINT("Using unweighted format\n");
		try {
			HFST::TransducerHandle upper = NULL;
			HFST::TransducerHandle lower = NULL;
			HFST::KeyTable *key_table = HFST::create_key_table();
			bool upper_has_symbols = false;
			bool lower_has_symbols = false;

			while (true) {
				int upperformat = HFST::read_format(upperstream);
				int lowerformat = HFST::read_format(lowerstream);

				if ((upperformat == EOF_FORMAT) && (lowerformat == EOF_FORMAT))
				{
				  delete_u(upper);
				  delete_u(lower);
					break;
				}
				if (upperformat == SFST_FORMAT)
				{
				  delete_u(upper);
				  upper_has_symbols = HFST::has_symbol_table(upperstream);
				  if (upper_has_symbols && !use_numbers)
				    upper = HFST::read_transducer(upperstream, key_table);
				  else
				    upper = HFST::read_transducer(upperstream);
				}
				if (lowerformat == SFST_FORMAT)
				{
				  delete_u(lower);
				  lower_has_symbols = HFST::has_symbol_table(lowerstream);
				  if (upper_has_symbols && lower_has_symbols && !use_numbers)
				    lower = HFST::read_transducer(lowerstream, key_table);
				  else if ((!upper_has_symbols && !lower_has_symbols) || use_numbers) {
				    if (!use_numbers)
				      fprintf(stderr, "Warning: transducers do not have a symbol table, "
					              "composition done using numbers instead\n");
				    lower = HFST::read_transducer(lowerstream);
				  }
				  else {
				    fprintf(message_out, "Only one transducer has a symbol table: "
					                 "composition not well defined\n"
					                 "Use option -n if necessary\n" );
				    return EXIT_FAILURE;
				  }
				}
				nth_stream++;
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Composing...\n");
				}
				else
				{
					VERBOSE_PRINT("Composing... %zu...\r", nth_stream);
				}
				HFST::TransducerHandle comp;
				if (not handle_flag_diacritics)
				  {
				    if (verbose)
				      {
					HFST::KeyTable * flag_table =
					  HFST::gather_flag_diacritic_table
					  (key_table);
					if (flag_table->get_unused_key() 
					    !=0)
					  { VERBOSE_PRINT("Warning! "
   "Transducers contain flag diacritics which may be filtered!\n"
   "(Use option -F)\n");
					  }
					delete flag_table;
				      }
				    comp = HFST::compose(HFST::copy(upper), HFST::copy(lower), true );
				  }
				else
				  {
				    std::vector<HFST::TransducerHandle> lower_fst_vector(1,HFST::copy(lower));
				    comp = HFST::intersecting_composition(HFST::copy(upper),
									  &lower_fst_vector,key_table);
				  }
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
			HWFST::TransducerHandle lower = NULL;
			HWFST::TransducerHandle upper = NULL;
			HWFST::KeyTable *key_table = HWFST::create_key_table();
			bool upper_has_symbols = false;
			bool lower_has_symbols = false;

			while (true) 
			{
				int upperformat = HFST::read_format(upperstream);
				int lowerformat = HFST::read_format(lowerstream);

				if ((upperformat == EOF_FORMAT) && (lowerformat == EOF_FORMAT))
				{
				  delete_w(upper);
				  delete_w(lower);
					break;
				}
				if (upperformat == OPENFST_FORMAT)
				{
				  delete_w(upper);
				  upper_has_symbols = HWFST::has_symbol_table(upperstream);
				  if (upper_has_symbols && !use_numbers)
				    upper = HWFST::read_transducer(upperstream, key_table);
				  else
				    upper = HWFST::read_transducer(upperstream);
				}
				if (lowerformat == OPENFST_FORMAT)
				{
				  delete_w(lower);
				  lower_has_symbols = HWFST::has_symbol_table(lowerstream);
				  if (upper_has_symbols && lower_has_symbols && !use_numbers)
				    lower = HWFST::read_transducer(lowerstream, key_table);
				  else if ((!upper_has_symbols && !lower_has_symbols) || use_numbers) {
				    if (!use_numbers)
				      fprintf(stderr, "Warning: transducers do not have a symbol table, "
					              "composition done using numbers instead\n");
				    lower = HWFST::read_transducer(lowerstream);
				  }
				  else {
				    fprintf(message_out, "Only one transducer has a symbol table: "
					                 "composition not well defined\n"
					                 "Use option -n if necessary\n" );
				    return EXIT_FAILURE;
				  }
				}
				nth_stream++;
				if (nth_stream < 2)
				{
					VERBOSE_PRINT("Composing...\n");
				}
				else
				{
					VERBOSE_PRINT("Compos... %zu...\r", nth_stream);
				}
				HWFST::TransducerHandle comp;
				if (not handle_flag_diacritics)
				  {
				    if (verbose)
				      {
					HFST::KeyTable * flag_table =
					  HFST::gather_flag_diacritic_table
					  (key_table);
					if (flag_table->get_unused_key() 
					    !=0)
					  { VERBOSE_PRINT("Warning! "
   "Transducers contain flag diacritics which may be filtered!\n"
   "(Use option -F)\n");
					  }
					delete flag_table;
				      }
				    comp = HWFST::compose(HWFST::copy(upper), HWFST::copy(lower), true );
				  }
				else
				  {
				    std::vector<HWFST::TransducerHandle> lower_fst_vector(1,HWFST::copy(lower));
				    comp = HWFST::intersecting_composition(HWFST::copy(upper),
									  &lower_fst_vector,key_table);
				  }
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
	if (upperfile != stdin)
	{
	  // cat ab.a | ./hfst-compose -v -2 ab.a -  causes a SEGFAULT here
		fclose(upperfile);
	}
	if (lowerfile != stdin)
	{
		fclose(lowerfile);
	}
	if (outfile != stdout)
	{
		fclose(outfile);
	}
	VERBOSE_PRINT("Reading from %s and %s, writing to %s\n", 
		upperfilename, lowerfilename, outfilename);
	// here starts the buffer handling part
	if ((upperfile != stdin) && (lowerfile != stdin))
	{
		std::filebuf fbupper;
		fbupper.open(upperfilename, std::ios::in);
		std::istream upperstream(&fbupper);
		std::filebuf fblower;
		fblower.open(lowerfilename, std::ios::in);
		std::istream lowerstream(&fblower);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = compose_streams(upperstream, lowerstream, outstream);
		}
		else
		{
			retval = compose_streams(upperstream, lowerstream, std::cout);
		}
		return retval;
	}
	else if (upperfile != stdin)
	{
		std::filebuf fbupper;
		fbupper.open(upperfilename, std::ios::in);
		std::istream upperstream(&fbupper);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = compose_streams(upperstream, std::cin, outstream);
		}
		else
		{
			retval = compose_streams(upperstream, std::cin, std::cout);
		}
		return retval;
	}
	else if (lowerfile != stdin)
	{
		std::filebuf fblower;
		fblower.open(lowerfilename, std::ios::in);
		std::istream lowerstream(&fblower);
		if (outfile != stdout)
		{
			std::filebuf fbout;
			fbout.open(outfilename, std::ios::out);
			std::ostream outstream(&fbout);
			retval = compose_streams(std::cin, lowerstream, outstream);
		}
		else
		{
			retval = compose_streams(std::cin, lowerstream, std::cout);
		}
		return retval;
	}
	free(upperfilename);
	free(lowerfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

