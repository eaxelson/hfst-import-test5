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
#include "HfstTransducer.h"
#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstTransducer;
using hfst::HfstInputStream;
using hfst::exceptions::NotTransducerStreamException;
using hfst::WeightedPaths;
using hfst::WeightedPath;

// the maximum number of strings printed for each transducer
static int nbest_strings=-1;
// print each string at most once
static bool display_weights=false;
static bool eval_fd=false;
static bool filter_fd=false;

void
print_usage()
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
	
	fprintf(message_out, "%-35s%s", "  -o, --output=OUTFILE",        "Write results to OUTFILE\n");
	fprintf(message_out, "%-35s%s", "  -i, --input=INFILE",          "Read input from INFILE\n");
	fprintf(message_out, "%-35s%s", "  -n, --nbest=INT",             "The maximum number of strings printed\n");
	fprintf(message_out, "%-35s%s", "  -w, --print-weights",         "Display the weight for each string\n");
	fprintf(message_out, "%-35s%s", "  -e, --eval-flag-diacritics",  "Only print strings with pass flag diacritic checks\n");
	fprintf(message_out, "%-35s%s", "  -f, --filter-flag-diacritics","Don't print flag diacritic symbols (only with -e)\n");
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
	print_more_info();
	fprintf(stderr, "\n");
	print_report_bugs();
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
		HFST_GETOPT_COMMON_LONG
		  ,
		HFST_GETOPT_UNARY_LONG
		  ,
			{"nbest", required_argument, 0, 'n'},
			{"print-weights", no_argument, 0, 'w'},
			{"eval-flag-diacritics", no_argument, 0, 'e'},
			{"filter-flag-diacritics", no_argument, 0, 'f'},
			{0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "R:dhi:n:o:qsvVwef",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}
		//char *level = NULL;
		switch (c)
		{
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
		case 'n':
			nbest_strings = atoi(hfst_strdup(optarg));
			break;
		case 'w':
			display_weights = true;
			break;
		case 'e':
		  eval_fd = true;
		  break;
		case 'f':
		  if(!eval_fd)
		  {
		    fprintf(message_out, "Option -f must be used in conjunction with -e\n");
		    print_short_help();
		    return EXIT_FAILURE;
		  }
		  filter_fd = true;
		  break;
#include "inc/getopt-cases-error.h"
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
		print_short_help();
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
process_stream(HfstInputStream& instream, std::ostream& outstream)
{
  instream.open();
    
  bool first_transducer=true;
  while(instream.is_good())
  {
    if (!first_transducer)
      outstream << "\n";
    first_transducer=false;
    
    HfstTransducer t(instream);
    
    if(nbest_strings > 0)
    {
      verbose_printf("Finding a maximum of %i paths...\n", nbest_strings);
      t.n_best(nbest_strings);
    }
    else
    {
      verbose_printf("Finding all paths...\n");
      if(t.is_cyclic())
      {
        fprintf(stderr, "Transducer is cyclic. Use the -n option.\n");
        return EXIT_FAILURE;
      }
    }
    
    WeightedPaths<float>::Set results;
    if(eval_fd)
      t.extract_strings_fd(results, filter_fd);
    else
      t.extract_strings(results);
    
    for(WeightedPaths<float>::Set::const_iterator it = results.begin(); it != results.end(); it++)
	  {
	    const WeightedPath<float>& wp = *it;
	    std::cout << wp.istring;
	    if(wp.ostring != wp.istring)
	      std::cout << " : " << wp.ostring;
	    if(display_weights)
	      std::cout << "\t" << wp.weight;
	    std::cout << std::endl;
	  }
	}
	
	instream.close();
	return EXIT_SUCCESS;
}


int main( int argc, char **argv ) {
  hfst_set_program_name(argv[0], "0.1", "HfstFst2Strings");
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
	HfstInputStream* instream = NULL;
	try {
	  instream = (inputfile != stdin) ?
	    new HfstInputStream(inputfilename) : new HfstInputStream();
	} catch(NotTransducerStreamException)	{
		fprintf(stderr, "%s is not a valid transducer file\n", inputfilename);
		return EXIT_FAILURE;
	}
	
	if(!is_output_stdout)
	{
	  std::ofstream outstream(outfilename);
	  retval = process_stream(*instream, outstream);
	}
	else
	  retval = process_stream(*instream, std::cout);
	
	delete instream;
	free(inputfilename);
	free(outfilename);
	return retval;
}

