//! @file hfst-concatenate.cc
//!
//! @brief Transducer concatenation command line tool
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

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::HfstOutputStream;
using hfst::ImplementationType;

static bool is_input_stdin = true;
// left part of concatenation
static char *leftfilename;
static FILE *leftfile;
static bool leftNamed = false;
// right part of concatenation
static char *rightfilename;
static FILE *rightfile;
static bool rightNamed = false;
// result of concatenation
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
		   "Concatenate two transducers\n"
		"\n", program_name );
		print_common_program_options(message_out);
		print_common_binary_program_options(message_out);
		fprintf(message_out, "\n");
#               if DEBUG
		fprintf(message_out,
			"%-35s%s", "  -d, --debug            Print debugging messages and results\n");
#               endif
		print_common_binary_program_parameter_instructions(message_out);
		fprintf(stderr, "\n");
		print_more_info(message_out, "Concatenate");
		fprintf(stderr, "\n");
		/*fprintf(message_out,
			"\n"
			"Examples:\n"
			"  %s -o catdog.hfst cat.hfst dog.hfst  concatenates "
			"cat.hfst with dog.hfst\n"
			"         writing results to catdog.hfst\n"
			"\n",
			program_name );*/
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
		  {"write-symbols-to", required_argument, 0, 'W'},
		        {"input1", required_argument, 0, '1'},
			{"input2", required_argument, 0, '2'},
			{"number", no_argument, 0, 'n'},
			{"output", required_argument, 0, 'o'},
			{"version", no_argument, 0, 'V'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "1:2:Ddhno:qvVsW:",
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
		case 'W':
			write_symbols_to = hfst_strdup(optarg);
			break;
		case 'D':
			write_symbol_table = false;
			break;
		case '?':
			fprintf(message_out, "%s: invalid option --%s\n", argv[0], 
					long_options[option_index].name);
			fprintf(message_out, "Try ``%s --help'' for more information\n",
					argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			fprintf(message_out, "%s: invalid option -%c\n", argv[0], c);
			fprintf(message_out, "Try ``%s --help'' for more information\n",
					argv[0]);
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
						"%s: exactly two transducer files must be given\n",
						argv[0]);
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
	else if (leftNamed && rightNamed) {

	}
	else
	{
		fprintf(message_out,
				"%s: at least one transducer filename must be given\n",
				argv[0]);
		return EXIT_FAILURE;
	}
	return EXIT_CONTINUE;
}

int
concatenate_streams(const char* leftfilename, const char* rightfilename,
                    const char* outfilename)
{
    HfstInputStream leftInput(leftfilename);
    HfstInputStream rightInput(rightfilename);
    leftInput.open();
    rightInput.open();
    // should be is_good? 
    bool bothInputs = leftInput.is_good() && rightInput.is_good();
    HfstTransducer* left = new HfstTransducer(leftInput);
    HfstTransducer* right = new HfstTransducer(rightInput);
    ImplementationType leftType = left->get_type();
    ImplementationType rightType = right->get_type();
    if (leftType != rightType)
      {
        fprintf(stderr, "Warning: tranducer type mismatch in %s and %s; "
                "using former type as output\n",
                leftfilename, rightfilename);
      }
    HfstOutputStream outstream(outfilename, leftType);
    size_t transducer_n = 0;
    while (bothInputs) {
        transducer_n++;
        if (transducer_n == 1)
          {
            VERBOSE_PRINT("Concatenating %s and %s...\n", leftfilename, 
                          rightfilename);
          }
        else
          {
            VERBOSE_PRINT("Concatenating %s and %s... %zu\n", leftfilename,
                          rightfilename, transducer_n);
          }
        outstream << left->concatenate(*right);
        bothInputs = leftInput.is_good() && rightInput.is_good();
        delete left;
        left = NULL;
        delete right;
        right = NULL;
        if (leftInput.is_good())
          {
            left = new HfstTransducer(leftInput);
          }
        if (rightInput.is_good())
          {
            right = new HfstTransducer(rightInput);
          }
      } 
    if (leftInput.is_good())
      {
        fprintf(stderr, "Warning: %s contains more transducers than %s; "
                "residue skipped\n", leftfilename, rightfilename);
      }
    else if (rightInput.is_good())
      {
        fprintf(stderr, "Warning: %s contains fewer transducers than %s; "
                "residue skipped\n", leftfilename, rightfilename);
      }
    leftInput.close();
    rightInput.close();
    delete left;
    delete right;
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
        retval = concatenate_streams(leftfilename, rightfilename, outfilename);
      }
	else if (leftfile == stdin)
      {
        retval = concatenate_streams(NULL, rightfilename, outfilename);
      }
	else if (rightfile == stdin)
      {
        retval = concatenate_streams(leftfilename, NULL, outfilename);
      }
	free(leftfilename);
	free(rightfilename);
	free(outfilename);
    return retval;
}

