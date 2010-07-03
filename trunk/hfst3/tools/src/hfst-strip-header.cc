//! @file hfst-fst2fst.cc
//!
//! @brief HFST header stripping tool
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

#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "hfst-common-unary-variables.h"

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Remove any HFST3 headers\n"
		"\n", program_name);

	print_common_program_options(message_out);
	fprintf(message_out,	"%-35s%s", "  -i, --input=INFILE",          "Read input transducer from INFILE\n");
  fprintf(message_out, "%-35s%s", "  -o, --output=OUTFILE",        "Write output transducer to OUTFILE\n");
	fprintf(message_out, "\n");
	print_common_unary_program_parameter_instructions(message_out);
	fprintf(message_out, "\n");
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
      {"input", required_argument, 0, 'i'},
      {"output", required_argument, 0, 'o'}
		};
		int option_index = 0;
		// add tool-specific options here 
		char c = getopt_long(argc, argv, "dhi:o:sqvV",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		char *format=NULL;

		switch (c)
		{
#include "hfst-common-cases.h"
    case 'i':
      inputfilename = hfst_strdup(optarg);
      if (strcmp(inputfilename, "-") == 0) {
        inputfilename = hfst_strdup("<stdin>");
        inputfile = stdin;
        is_input_stdin = true;
      }
      else {
        inputfile = hfst_fopen(inputfilename, "r");
        is_input_stdin = false;
      }
      break;
      case 'o':
      outfilename = hfst_strdup(optarg);
      if (strcmp(outfilename, "-") == 0) {
        outfilename = hfst_strdup("<stdout>");
        outfile = stdout;
        is_output_stdout = true;
      }
      else {
        outfile = hfst_fopen(outfilename, "w");
        is_output_stdout = false;
      }
      message_out = stdout;
      break;
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

int
process_stream(FILE* f_in, FILE* f_out)
{
  const char* header = "HFST3";
  int header_loc = 0; // how much of the header has been found
  while(true)
  {
    int c = getc(f_in);
    if(c == EOF)
      return EXIT_SUCCESS;
    
    if(c == header[header_loc])
    {
      if(header_loc == 5) // we've found the whole header (incl. null terminator)
      {
        // eat text until the next null terminator
        do
        {
          c = getc(f_in);
        } while (c != '\0' && c != EOF);
        header_loc = 0;
      }
      else
        header_loc++; // look for the next character now
    }
    else
    {
      if(header_loc > 0)
      { // flush the characters that could have been header but turned out not to be
        for(int i=0; i<header_loc; i++)
          putc(header[i], f_out);
        header_loc = 0;
        
        ungetc(c, f_in); // the character we just grabbed could be the start of the header, so put it back
      }
      else
        putc(c, f_out);
    }
  }
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  message_out = stdout;
  verbose = false;
  int retval = parse_options(argc, argv);
  if (retval != EXIT_CONTINUE)
  {
    return retval;
  }
  VERBOSE_PRINT("Reading from %s, writing to %s\n", 
    inputfilename, outfilename);
  
  retval = process_stream(inputfile, outfile);
	
  free(inputfilename);
  free(outfilename);
  return retval;
}

