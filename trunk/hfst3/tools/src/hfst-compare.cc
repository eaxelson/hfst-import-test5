//! @file hfst-compare.cc
//!
//! @brief Transducer equality test command line tool
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

#include "HfstTransducer.h"
#include "hfst-program-options.h"
#include "hfst-commandline.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::exceptions::NotTransducerStreamException;

static bool is_input_stdin = true;
// first 
static char *firstfilename;
static FILE *firstfile;
static bool firstNamed = false;
// right part of comparison
static char *secondfilename;
static FILE *secondfile;
static bool secondNamed = false;
// result of comparition
static char *outfilename;
static FILE *outfile;
static bool is_output_stdout = true;
// setting
static bool bail_on_mismatch = false;

static bool use_numbers = false;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE1 [INFILE2]]\n"
		   "Compare two transducers for equivalence\n"
		"\n", program_name);
	print_common_program_options(message_out);

#               if DEBUG
	fprintf(message_out, "%-35s%s", "  -d, --debug", "Print debugging messages and results\n");
#               endif
	fprintf(message_out, "%-35s%s", "  -o, --output=OUTFILE",      "Write results to OUTFILE\n");
	fprintf(message_out, "%-35s%s", "  -1, --input1=INFILE1",      "Read first transducer from INFILE1\n");
	fprintf(message_out, "%-35s%s", "  -2, --input2=INFILE2",      "Read second transducer from INFILE2\n");
	fprintf(message_out, "%-35s%s", "  -n, --number",              "Use numbers instead of harmonizing transducers\n");
	fprintf(message_out, "%-35s%s", "  -e, --error-on-mismatch",   "Error if transducer are not equivalent\n");
	fprintf(message_out, "\n");
	fprintf(message_out, 
		"If OUTFILE, or either INFILE1 or INFILE2 is missing or -,\n"
		"standard streams will be used.\n"
		"INFILE1, INFILE2, or both, must be specified.\n"
		"Weightedness of result depends on weightedness of INFILE1\n"
		"and INFILE2; both must be either weighted or unweighted.\n"
		"\n");
	fprintf(stderr, "\n");
	print_more_info(message_out, "Compare");
	fprintf(stderr, "\n");
	/*"Examples:\n"
	  "  %s first.hfst second.hfst  compares first.hfst "
	  "and second.hfst\n"
	  "\n"*/
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
			{"input1", required_argument, 0, '1'},
			{"input2", required_argument, 0, '2'},
			{"output", required_argument, 0, 'o'},
                        {"number", no_argument, 0, 'n'},
			{"error-on-mismatch", no_argument, 0, 'e'},
			{"minimal", no_argument, 0, 'm'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "1:2:endho:qvVsm",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
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
			print_version("hfst-compare");
			return EXIT_SUCCESS;
			break;
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
			firstfilename = hfst_strdup(optarg);
			if (strcmp(firstfilename, "-") != 0)
			{
				firstfile = hfst_fopen(firstfilename, "r");
				is_input_stdin = false;
				firstNamed = true;
			}
			else
			{
				free(firstfilename);
				firstfilename = hfst_strdup("<stdin>");
				firstfile = stdin;
			}
			break;
		case '2':
			secondfilename = hfst_strdup(optarg);
			if (strcmp(secondfilename, "-") != 0)
			{
				secondfile = hfst_fopen(secondfilename, "r");
				is_input_stdin = false;
				secondNamed = true;
			}
			else
			{
				free(secondfilename);
				secondfilename = hfst_strdup("<stdin>");
				secondfile = stdin;
			}
			break;
		case 'v':
			verbose = true;
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
		case 'e':
			bail_on_mismatch = true;
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
	if (firstNamed && secondNamed)
	{
		;
	}
	else if (!firstNamed && !secondNamed)
	{
			// neither input given in options:
			if ((argc - optind) == 2)
			{
				firstfilename = hfst_strdup(argv[optind]);
				secondfilename = hfst_strdup(argv[optind+1]);
				if ( (strcmp(firstfilename, "-") == 0) &&
					 (strcmp(secondfilename, "-") == 0) )
				{
					fprintf(message_out, 
						"Both files cannot be standard streams\n");
					print_short_help(argv[0]);
					return EXIT_FAILURE;
				}
				if (strcmp(firstfilename, "-") != 0)
				{
					firstfile = hfst_fopen(firstfilename, "r");
					is_input_stdin = false;
				}
				else
				{
					free(firstfilename);
					firstfilename = strdup("<stdin>");
					firstfile = stdin;
					is_input_stdin = true;
				}
				if (strcmp(secondfilename, "-") != 0)
				{
					secondfile = hfst_fopen(secondfilename, "r");
					is_input_stdin = false;
				}
				else
				{
					free(secondfilename);
					secondfilename = strdup("<stdin>");
					secondfile = stdin;
					is_input_stdin = true;
				}
			}
			else if ((argc - optind) == 1)
			{
				firstfilename = hfst_strdup(argv[optind]);
				if (strcmp(firstfilename, "-") == 0)
				{
					fprintf(message_out,
						"Both transducers cannot be read from standard input\n");
					print_short_help(argv[0]);
				}
				firstfile = hfst_fopen(firstfilename, "r");
				secondfilename = hfst_strdup("<stdin>");
				secondfile = stdin;
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
	else if (!firstNamed)
	{
		if ((argc - optind) == 1)
		{
			firstfilename = hfst_strdup(argv[optind]);
			if (strcmp(firstfilename, "-") != 0)
			{
				firstfile = hfst_fopen(firstfilename, "r");
				is_input_stdin = false;
			}
			else
			{
				free(firstfilename);
				firstfilename = hfst_strdup("<stdin>");
				is_input_stdin = true;
			}
		}
		else if ((argc - optind) == 0)
		{
			firstfilename = hfst_strdup("<stdin>");
			firstfile = stdin;
			is_input_stdin = true;
		}
	}
	else if (!secondNamed)
	{
		if ((argc - optind) == 1)
		{
			secondfilename = hfst_strdup(argv[optind]);
			if (strcmp(secondfilename, "-") != 0)
			{
				secondfile = hfst_fopen(secondfilename, "r");
				is_input_stdin = false;
			}
			else
			{
				free(secondfilename);
				secondfilename = strdup("<stdin>");
				is_input_stdin = true;
			}
		}
		else if ((argc - optind) == 0)
		{
			secondfilename = hfst_strdup("<stdin>");
			secondfile = stdin;
			is_input_stdin = true;
		}
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

int
compare_streams(HfstInputStream& firststream, HfstInputStream& secondstream,
                std::ostream& outstream)
{
	firststream.open();
	secondstream.open();
	// should be is_good? 
	bool bothInputs = firststream.is_good() && secondstream.is_good();	
	size_t transducer_n = 0;
	while (bothInputs) {
		transducer_n++;
		if (transducer_n == 1)
		{
			VERBOSE_PRINT("Comparing %s and %s...\n", firstfilename, 
		                secondfilename);
		}
		else
		{
		  VERBOSE_PRINT("Comparing %s and %s... %zu\n", firstfilename,
		                secondfilename, transducer_n);
		}
		HfstTransducer first(firststream);
		HfstTransducer second(secondstream);
		
		if(first.get_type() != second.get_type())
		{
			fprintf(stderr, "Warning: %s contains a different transducer format "
				"from %s; converting to the format of %s\n", firstfilename,
				secondfilename, firstfilename);
			second.convert(first.get_type());
		}
		
		if (HfstTransducer::are_equivalent(first, second))
		{
			outstream << "Equivalent\n";
		}
		else
		{
			outstream << "Not equivalent\n";
			if (bail_on_mismatch)
			{
				return EXIT_FAILURE;
			}
		}
		bothInputs = firststream.is_good() && secondstream.is_good();
	}
	
	if (firststream.is_good())
	{
	  fprintf(stderr, "Warning: %s contains more transducers than %s; "
	          "residue skipped\n", firstfilename, secondfilename);
	}
	else if (secondstream.is_good())
	{
	  fprintf(stderr, "Warning: %s contains fewer transducers than %s; "
	          "residue skipped\n", firstfilename, secondfilename);
	}
	firststream.close();
	secondstream.close();
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
	if (firstfile != stdin)
	{
		fclose(firstfile);
	}
	if (secondfile != stdin)
	{
		fclose(secondfile);
	}
	if (outfile != stdout)
	{
		fclose(outfile);
	}
	VERBOSE_PRINT("Reading from %s and %s, writing to %s\n", 
		firstfilename, secondfilename, outfilename);
	// here starts the buffer handling part
	HfstInputStream* firststream = NULL;
	HfstInputStream* secondstream = NULL;
	try {
		firststream = (firstfile != stdin) ?
			new HfstInputStream(firstfilename) : new HfstInputStream();
	} catch(NotTransducerStreamException)	{
		fprintf(stderr, "%s is not a valid transducer file\n", firstfilename);
		return EXIT_FAILURE;
	}
	try {
		secondstream = (secondfile != stdin) ?
			new HfstInputStream(secondfilename) : new HfstInputStream();
	} catch(NotTransducerStreamException)	{
		fprintf(stderr, "%s is not a valid transducer file\n", secondfilename);
		return EXIT_FAILURE;
	}
	
	if(outfile != stdout)
	{
		std::ofstream out(outfilename);
		retval = compare_streams(*firststream, *secondstream, out);
	}
	else
	{
		retval = compare_streams(*firststream, *secondstream, std::cout);
	}
	
	delete firststream;
	delete secondstream;
	free(firstfilename);
	free(secondfilename);
	free(outfilename);
	return retval;
}
