//! @file hfst-regexp2fst.cc
//!
//! @brief Regular expression reading command line tool
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
#include <math.h>

#include <sstream>

#include <hfst2/hfst.h>

#if NESTED_BUILD
#include <hfst2/regexp/regexp.h>
#endif

#if !NESTED_BUILD
#include <hfst2/regexp.h>
#endif

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-unary.h"


static bool is_weighted=false;
static bool disjunct_expressions=false;
static char *expression=NULL;

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	printf("USAGE: %s [OPTIONS] INFILE\n"
		"Parses regular expressions and produces HFST transducers\n"
	       "\n", program_name);
		print_common_program_options(message_out);
#               if DEBUG
		fprintf(message_out,
			"%-35s%s", "-d, --debug", "Print debugging messages and results\n"
			);
#               endif
		  
		fprintf(message_out, "%-35s%s",	"  -i, --input=INFILE",  	"Read input regular expressions from INFILE\n");
		fprintf(message_out, "%-35s%s",	"  -o, --output=OUTFILE", 	"Write output transducer(s) to OUTFILE\n");
		fprintf(message_out, "%-35s%s",	"  -R, --read-symbols=FILE", 	"Read symbol table from FILE\n");
		fprintf(message_out, "%-35s%s",	"  -D, --do-not-write-symbols", "Do not write symbol table with the output transducer(s)\n");
		fprintf(message_out, "%-35s%s",	"  -W, --write-symbols-to=FILE","Write symbol table to file FILE\n");
		fprintf(message_out, "%-35s%s",	"  -w, --weighted",         "Write result in weighted format\n");
		fprintf(message_out, "\n");
		fprintf(message_out, "%-35s%s",	"  -e, --expression",         "The regular expression\n");
		fprintf(message_out, "%-35s%s",	"  -j, --disjunct-expressions", "Disjunct all regular expressions instead of transforming each of them into a separate transducer\n"); 
		fprintf(message_out, "\n");

		fprintf(message_out, 
			"If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
			);
		fprintf(stderr, "\n");
		print_more_info(message_out, "Regexp2Fst");
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
		  {"expression", required_argument, 0, 'e'},
		  {"disjunct-expressions", no_argument, 0, 'j'},
		  {"weighted", no_argument, 0, 'w'},
		  {0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "R:dhi:o:qvVwW:Djse:",
							 long_options, &option_index);
		if (-1 == c)
		{
			break;
		}

		switch (c)
		{
#include "inc/getopt-cases-common.h"
#include "inc/getopt-cases-unary.h"
		case 'e':
		  expression = hfst_strdup(optarg);
		  break;
		case 'j':
			disjunct_expressions = true;
			break;
		case 'w':
			is_weighted = true;
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
     if (!is_weighted)
                {
		verbose_printf("Using unweighted format\n");
		try {

		  HFST::TransducerHandle result = HFST::create_empty_transducer();
		  
		  HFST::KeyTable *key_table = NULL;
		  if (read_symbols_from_filename) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HFST::read_symbol_table(is);
		    is.close();
		    verbose_printf("Symbol table read\n");
		  }
		  else {
		    key_table = HFST::create_key_table();
		    HFST::associate_key(0, key_table, HFST::define_symbol("@0@"));
		  }

		  HFST::KeyPairSet *negation_pi = HFST::create_empty_keypair_set();
		  HFST::KeyPairSet *creation_pi = HFST::create_empty_keypair_set();

		  negation_pi = HFST::insert_keypair( HFST::define_keypair(0, 0),
						      negation_pi );
		  creation_pi = HFST::insert_keypair( HFST::define_keypair(0, 0),
						      creation_pi );

		  if (read_symbols_from_filename) {
	    
		    HFST::KeySet *key_set1 = HFST::get_key_set(key_table);
		    HFST::KeySet *key_set2 = HFST::get_key_set(key_table);		    
		    HFST::KeySet::iterator it1;
		    HFST::KeySet::iterator it2;	    

		    for (it1=key_set1->begin(); it1!=key_set1->end(); ++it1) {
		      for (it2=key_set2->begin(); it2!=key_set2->end(); ++it2) {
			negation_pi = HFST::insert_keypair( HFST::define_keypair(*it1, *it2),
							    negation_pi );
			creation_pi = HFST::insert_keypair( HFST::define_keypair(*it1, *it2),
							    creation_pi );
		      }
		    }
		  }

		  if (expression) {

		    char *regex_data = HFST::string_copy(expression);

		    verbose_printf("Compiling unweighted transducer\n");
		    HFST::TransducerHandle heavyDucer;
		    heavyDucer = HFST::compile_xre(regex_data, 
						   negation_pi, 
						   creation_pi, 
						   key_table);
		    free(regex_data);
		    verbose_printf("Calculating symbol mappings\n");
		    HFST::KeyTable* symbolmap = HFST::xre_get_last_key_table();
		    if (NULL == heavyDucer)
		      {
			fprintf(message_out, "ERROR: null returned");
			return 2;
		      }
		    if (HFST::is_empty(heavyDucer))
		      {
			verbose_printf("Resulting transducer is empty!\n");
		      }
		    if (verbose)
		      {
			fprintf(message_out, "The regular expression contained symbols: ");
			bool first = true;
			HFST::KeySet* keys = HFST::xre_get_last_key_set();
			for (HFST::KeyIterator k = HFST::begin_sigma_key(keys);
			     k != HFST::end_sigma_key(keys); ++k)
			  {
			    if (!first)
			      {
				fprintf(message_out, ", ");
			      }
			    fprintf(message_out, "%s", HFST::get_symbol_name(
									     HFST::get_key_symbol(*k, symbolmap)));
			    first = false;
			  }
			fprintf(message_out, "\n");
		      }
		    
		    if (write_symbols)
		      HFST::write_transducer(heavyDucer, key_table, outstream);
		    else
		      HFST::write_transducer(heavyDucer, outstream);
		  }


		  else {
		    inputstream.peek();

		    while (!inputstream.eof()) {
		      std::string input_line;
		      std::getline(inputstream,input_line);
		      const char * line = input_line.c_str();
		      if (strcmp(line,"") == 0) // an empty line
			continue;

		      char *regex_data = HFST::string_copy(line);
		      
		      verbose_printf("Compiling unweighted transducer\n");
		      HFST::TransducerHandle heavyDucer;
		      heavyDucer = HFST::compile_xre(regex_data, 
						     negation_pi, 
						     creation_pi, 
						     key_table);
		      free(regex_data);
		      verbose_printf("Calculating symbol mappings\n");
		      HFST::KeyTable* symbolmap = HFST::xre_get_last_key_table();
		      if (NULL == heavyDucer)
			{
			  fprintf(message_out, "ERROR: null returned");
			  return 2;
			}
		      if (HFST::is_empty(heavyDucer))
			{
			  verbose_printf("Resulting transducer is empty!\n");
			}
		      if (verbose)
			{
			  fprintf(message_out, "The regular expression contained symbols: ");
			  bool first = true;
			  HFST::KeySet* keys = HFST::xre_get_last_key_set();
			  for (HFST::KeyIterator k = HFST::begin_sigma_key(keys);
			       k != HFST::end_sigma_key(keys); ++k)
			    {
			      if (!first)
				{
				  fprintf(message_out, ", ");
				}
			      fprintf(message_out, "%s", HFST::get_symbol_name(
									       HFST::get_key_symbol(*k, symbolmap)));
			      first = false;
			    }
			  fprintf(message_out, "\n");
			}
		    
		      result = HFST::disjunct(result, heavyDucer);

		      if (!disjunct_expressions) {
			if (write_symbols)
			  HFST::write_transducer(result, key_table, outstream);
			else
			  HFST::write_transducer(result, outstream);
			result = HFST::create_empty_transducer();
		      }
		    
		      inputstream.peek();
		  		       
		    }

		    if (disjunct_expressions) {
			
		      if (write_symbols)
			HFST::write_transducer(result, key_table, outstream);
		      else
			HFST::write_transducer(result, outstream);
		    }
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
        else
	  {
	    verbose_printf("Using weighted format\n");
		try {

		  HWFST::TransducerHandle result = HWFST::create_empty_transducer();
		  
		  HWFST::KeyTable *key_table = NULL;
		  if (read_symbols_from_filename) {
		    ifstream is(read_symbols_from_filename);
		    key_table = HWFST::read_symbol_table(is);
		    is.close();
		    verbose_printf("Symbol table read\n");
		  }
		  else {
		    key_table = HWFST::create_key_table();
		    HWFST::associate_key(0, key_table, HWFST::define_symbol("@0@"));
		  }

		  HWFST::KeyPairSet *negation_pi = HWFST::create_empty_keypair_set();
		  HWFST::KeyPairSet *creation_pi = HWFST::create_empty_keypair_set();

		  negation_pi = HWFST::insert_keypair( HWFST::define_keypair(0, 0),
						      negation_pi );
		  creation_pi = HWFST::insert_keypair( HWFST::define_keypair(0, 0),
						      creation_pi );

		  if (read_symbols_from_filename) {
	    
		    HWFST::KeySet *key_set1 = HWFST::get_key_set(key_table);
		    HWFST::KeySet *key_set2 = HWFST::get_key_set(key_table);		    
		    HWFST::KeySet::iterator it1;
		    HWFST::KeySet::iterator it2;	    

		    for (it1=key_set1->begin(); it1!=key_set1->end(); it1++) {
		      for (it2=key_set2->begin(); it2!=key_set2->end(); it2++) {
			negation_pi = HWFST::insert_keypair( HWFST::define_keypair(*it1, *it2),
							    negation_pi );
			creation_pi = HWFST::insert_keypair( HWFST::define_keypair(*it1, *it2),
							    creation_pi );
		      }
		    }
		  }

		  if (expression) {

		    char *regex_data = HFST::string_copy(expression);

		    verbose_printf("Compiling unweighted transducer\n");
		    HWFST::TransducerHandle heavyDucer;
		    heavyDucer = HWFST::compile_xre(regex_data, 
						    negation_pi, 
						    creation_pi, 
						    key_table);
		    free(regex_data);
		    verbose_printf("Calculating symbol mappings\n");
		    HWFST::KeyTable* symbolmap = HWFST::xre_get_last_key_table();
		    if (NULL == heavyDucer)
		      {
			fprintf(message_out, "ERROR: null returned");
			return 2;
		      }
		    if (HWFST::is_empty(heavyDucer))
		      {
			verbose_printf("Resulting transducer is empty!\n");
		      }
		    if (verbose)
		      {
			fprintf(message_out, "The regular expression contained symbols: ");
			bool first = true;
			HWFST::KeySet* keys = HWFST::xre_get_last_key_set();
			for (HWFST::KeyIterator k = HWFST::begin_sigma_key(keys);
			     k != HWFST::end_sigma_key(keys); ++k)
			  {
			    if (!first)
			      {
				fprintf(message_out, ", ");
			      }
			    fprintf(message_out, "%s", HWFST::get_symbol_name(
									     HWFST::get_key_symbol(*k, symbolmap)));
			    first = false;
			  }
			fprintf(message_out, "\n");
		      }
		    
		    if (write_symbols)
		      HWFST::write_transducer(heavyDucer, key_table, outstream);
		    else
		      HWFST::write_transducer(heavyDucer, outstream);
		  }


		  else {
		    inputstream.peek();

		    while (!inputstream.eof()) {
		      std::string input_line;
		      std::getline(inputstream,input_line);
		      const char * line = input_line.c_str();

		      if (strcmp(line,"") == 0) // an empty line
			continue;
		      
		      char *regex_data = HFST::string_copy(line);
		      
		      verbose_printf("Compiling unweighted transducer\n");
		      HWFST::TransducerHandle heavyDucer;
		      heavyDucer = HWFST::compile_xre(regex_data, 
						      negation_pi, 
						      creation_pi, 
						      key_table);
		      free(regex_data);
		      verbose_printf("Calculating symbol mappings\n");
		      HWFST::KeyTable* symbolmap = HWFST::xre_get_last_key_table();
		      if (NULL == heavyDucer)
			{
			  fprintf(message_out, "ERROR: null returned");
			  return 2;
			}
		      if (HWFST::is_empty(heavyDucer))
			{
			  verbose_printf("Resulting transducer is empty!\n");
			}
		      if (verbose)
			{
			  fprintf(message_out, "The regular expression contained symbols: ");
			  bool first = true;
			  HWFST::KeySet* keys = HWFST::xre_get_last_key_set();
			  for (HWFST::KeyIterator k = HWFST::begin_sigma_key(keys);
			       k != HWFST::end_sigma_key(keys); ++k)
			    {
			      if (!first)
				{
				  fprintf(message_out, ", ");
				}
			      fprintf(message_out, "%s", HWFST::get_symbol_name(
									       HWFST::get_key_symbol(*k, symbolmap)));
			      first = false;
			    }
			  fprintf(message_out, "\n");
			}
		    
		      result = HWFST::disjunct(result, heavyDucer);

		      if (!disjunct_expressions) {
			if (write_symbols)
			  HWFST::write_transducer(result, key_table, outstream);
			else
			  HWFST::write_transducer(result, outstream);
			result = HWFST::create_empty_transducer();
		      }
		    
		      inputstream.peek();
		  		       
		    }

		    if (disjunct_expressions) {
			
		      if (write_symbols)
			HWFST::write_transducer(result, key_table, outstream);
		      else
			HWFST::write_transducer(result, outstream);
		    }
		  }

		  if (write_symbols_to_filename != NULL) {
		    ofstream os(write_symbols_to_filename);
		    HWFST::write_symbol_table(key_table, os);
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
}

extern int xredebug;

int main( int argc, char **argv ) {
  //xredebug = 1;
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

