//! @file hfst-pair-test.cc
//!
//! @brief Transducer path testing command line tool
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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>

#include "hfst-pair-test-commandline.h"
#include "hfst-pair-test-transducers.h"
#include "hwfst-pair-test-transducers.h"

using std::cin;
using std::cout;
using std::vector;

static const char * negative_prefix = strdup("NEGATIVE ");
static const char * positive_prefix = strdup("POSITIVE ");

static FILE * output_file = stdout;

char * remove_prefix(const char * prefix, char * str)
{
  char * str_ptr = str;
  for(const char * prefix_ptr = prefix;
      *prefix_ptr != 0;
      ++prefix_ptr)
    {
      if (debug)
	{
	  fprintf(stderr,"%c\n",*prefix_ptr);
	}
      if (*str_ptr != *prefix_ptr)
	{
	  if (debug)
	    {
	      fprintf(stderr,"NO PREFIX: \"%c\" \"%c\"\n",*str_ptr,*prefix_ptr);
	    }
	  return str;
	}
      ++str_ptr;
    }
  if (debug)
    {
      fprintf(stderr,"REST: %s\n",str_ptr);
    }
  return str_ptr;
}

char * remove_negative_prefix(char * input)
{
  return remove_prefix(negative_prefix,input);
}

char * remove_positive_prefix(char * input)
{
  return remove_prefix(positive_prefix,input);
}

bool only_white_space(char * str)
{
  assert(str != NULL);
  for (char * p = str;
       *p != 0;
       ++p)
    {
      if ((*p != ' ') or (*p != '\t'))
	return false;
    }
  return true;
}

int main(int argc, char * argv[])
{
	bool unweighted = false;
  // Get all arguments.
  parse_options(argc,argv);
  if (debug) {
    display_all_arguments();
  }

  int format = -1;
  if ( fst_file_name == NULL ) 
  {
	  format = HFST::read_format(std::cin);
  }
  else
  {
	std::filebuf fbfsts;
    fbfsts.open(fst_file_name, std::ios::in);
    std::istream fststream(&fbfsts);
	format = HFST::read_format(fststream);
  }
#	define SFST_FORMAT 0
#	define OPENFST_FORMAT 1
  if (format == SFST_FORMAT)
  {
	  unweighted = true;
  }
  else if (format == OPENFST_FORMAT)
  {
	  unweighted = false;
  }
  else
  {
	  if (fst_file_name)
	  {
	  	fprintf(stderr, "Unrecognised file format %s\n", fst_file_name);
	  }
	  else
		 {
			 fprintf(stderr, "Unrecognised file format <stdin>\n");
		 }
	  exit(EXIT_FAILURE);
  }
  //Read input transducers
  if (unweighted)
  {
    HFST::FstVector * rules = HFST::read_rule_transducers();

  if (output_file_name != NULL)
    {
      if ( debug or verbose )
	fprintf(stderr,"Writing output to file %s\n",output_file_name);
      try 
	{
	  output_file = fopen(output_file_name,"r");
	}
      catch (const char * p)
	{
	  fprintf(stderr,"Error: %s\n",p);
	  exit(1);
	}
    }
  //Read rule_names
  if (rule_names_file_name != NULL)
    {
      size_t number_of_rules = rules->size();
      read_rule_names(number_of_rules);
    }

  //If the transducers didn't have a key table stored with them, it can be
  //given in a separate file. This does nothing, if the key table for the
  //rules  has already been set. 
  HFST::get_alphabet(alphabet_file_name);

  if (debug)
    {
		HFST::display_rule_keys();
    }

  if ( not spaces ) {
    if (debug)
      {
	fprintf(stderr,"Building tokenizer\n");
      }
	HFST::make_tokenizer();
  }
  
  // Record if all tests were passed. This is used when the program exits.
  // It exits cleanly (with code 0), only if passed_all_tests is true.
  bool passed_all_tests = true;

  // There are two main uses for the program. Test strings may either be 
  // read from STDIN or a file. The name of that file is given by 
  // char pair_file_name. If pair_file_name is NULL, test strings are read 
  // from STDIN.
  if ( pair_file_name == NULL )
    {
      if (debug or verbose)
	{
	  
	  fprintf(stderr,"Read the input rule transducers.\n"
		  "Entering main loop. Press Ctrl-d, to quit.\n");
	  fprintf(stderr,
		  "Please, give symbol-pair-strings separated by newlines.\n");
	  if (spaces)
	    {
	      fprintf(stderr,"The symbol-pairs in the symbol-pair strings need\n"
		      "to be separated by spaces. E.g. \"p r e t t y:i e r\"\n");
	    }
	}
      
      char input[1000];
      char * original_input_string;
      
      if ( debug or (not silent))
	{
	  fprintf(output_file,">> ");
	}
      // The main program loop. Read an input symbol-pair string,
      // tokenize it either using spaces or according to the KeyTable 
      // rule_keys, compile into a transducer and test against every rule. 
      // Display diagnostics
      while (cin.getline(input,1000))
	{
	  char * input_string = remove_negative_prefix(input);
	  bool negative = (input != input_string);
	  if ( not negative )
	    {
	      input_string = remove_positive_prefix(input);
	    }

	  original_input_string = strdup(input);
	  if (debug)
	    {
	      fprintf(output_file,"INPUT: %s\n",input_string);
	    }
	  //Transform the input string into a KeyVector, either by tokenizing 
	  //or splitting at spaces and coding using sppropriate keys.
	  KeyPairVector * input_vector;
	  if (not spaces)
	    {
	      input_vector = HFST::tokenize(input_string);
	    }
	  else
	    {
	      input_vector = HFST::split_at_spaces(input_string);
	    }
	  if (debug)
	    {
			HFST::display_pair_vector(input_vector);
	    }
	  if (input_vector == NULL)
	    {
	      if (debug or (not silent))
		{
		  fprintf(output_file,">> ");
		}
	      passed_all_tests=false;
	      free(original_input_string);
	      continue;
	    }
	  bool passed_test = HFST::test_correspondence(input_vector,rules,
						 rule_transducer_names,
						 negative);
	  if ( not passed_test )
	    {
	      if (not silent)
		{
		  fprintf(stderr,"%s FAILS\n",original_input_string);
		}	     
	    }
	  else
	    {
	      if (debug or verbose) 
		{
		  fprintf(stderr,"%s is OK\n",original_input_string);
		}
	    }
	  passed_all_tests = passed_all_tests and passed_test;
	  if (debug or (not silent))
	    {
	      fprintf(output_file,">> ");
	    }
	  free(original_input_string);
	}
      fprintf(output_file,"\n");
    }
  else {
      char input[1000];
      char * original_input_string;
      
      ifstream pairs_in(pair_file_name,std::ios::in);
      while (pairs_in.getline(input,1000))
	{
	  // skip lines only containing white-space.
	  if (only_white_space(input))
	    {
	      continue;
	    }
	  if (debug or verbose)
	    {
	      fprintf(output_file,"%s\n",input);
	    }
	  char * input_string;
	  input_string = remove_negative_prefix(input);
	  bool negative = (input_string != input);
	  if ( not negative)
	    {
	      input_string = remove_positive_prefix(input);
	    }
	  original_input_string = strdup(input);
	  if (debug)
	    {
	      fprintf(output_file,"INPUT STRING: %s\n",input_string);
	    }
	  //Transform the input string into a KeyVector, either by tokenizing 
	  //or splitting at spaces and coding using sppropriate keys.
	  KeyPairVector * input_vector;
	  if (not spaces)
	    {
	      input_vector = HFST::tokenize(input_string);
	    }
	  else
	    {
	      input_vector = HFST::split_at_spaces(input_string);
	    }
	  if (debug)
	    {
			HFST::display_pair_vector(input_vector);
	    }
	  if (input_vector == NULL)
	    {
	      free(original_input_string);
	      passed_all_tests = false;
	      continue;
	    }
	  bool passed_test = HFST::test_correspondence(input_vector,rules,
						 rule_transducer_names,
						 negative);
	  if ( not passed_test )
	    {
	      if (not silent)
		{
		  fprintf(stderr,"%s FAILS\n\n",original_input_string);
		}
	    }
	  else
	    {
	      if (debug or verbose) 
		{
		  fprintf(stderr,"%s is OK\n\n",original_input_string);
		}
	    }
	  passed_all_tests = passed_all_tests and passed_test;
	  free(original_input_string);
	}
  }
  for ( HFST::FstVector::iterator it = rules->begin();
	it != rules->end();
	++it )
    {
      delete *it;
    }
  delete rules;
  free(program_name);
  free(fst_file_name);
  free(alphabet_file_name);
  if ( passed_all_tests )
    {
      exit(0);
    }
  else
    {
      exit(1);
    }
  }
  else if (!unweighted)
  {
    HWFST::FstVector * rules = HFST::read_rule_transducers();

  if (output_file_name != NULL)
    {
      if ( debug or verbose )
	fprintf(stderr,"Writing output to file %s\n",output_file_name);
      try 
	{
	  output_file = fopen(output_file_name,"r");
	}
      catch (const char * p)
	{
	  fprintf(stderr,"Error: %s\n",p);
	  exit(1);
	}
    }
  //Read rule_names
  if (rule_names_file_name != NULL)
    {
      size_t number_of_rules = rules->size();
      read_rule_names(number_of_rules);
    }

  //If the transducers didn't have a key table stored with them, it can be
  //given in a separate file. This does nothing, if the key table for the
  //rules  has already been set. 
  HWFST::get_alphabet(alphabet_file_name);

  if (debug)
    {
		HWFST::display_rule_keys();
    }

  if ( not spaces ) {
    if (debug)
      {
	fprintf(stderr,"Building tokenizer\n");
      }
	HWFST::make_tokenizer();
  }
  
  // Record if all tests were passed. This is used when the program exits.
  // It exits cleanly (with code 0), only if passed_all_tests is true.
  bool passed_all_tests = true;

  // There are two main uses for the program. Test strings may either be 
  // read from STDIN or a file. The name of that file is given by 
  // char pair_file_name. If pair_file_name is NULL, test strings are read 
  // from STDIN.
  if ( pair_file_name == NULL )
    {
      if (debug or verbose)
	{
	  
	  fprintf(stderr,"Read the input rule transducers.\n"
		  "Entering main loop. Press Ctrl-d, to quit.\n");
	  fprintf(stderr,
		  "Please, give symbol-pair-strings separated by newlines.\n");
	  if (spaces)
	    {
	      fprintf(stderr,"The symbol-pairs in the symbol-pair strings need\n"
		      "to be separated by spaces. E.g. \"p r e t t y:i e r\"\n");
	    }
	}
      
      char input[1000];
      char * original_input_string;
      
      if ( debug or (not silent))
	{
	  fprintf(output_file,">> ");
	}
      // The main program loop. Read an input symbol-pair string,
      // tokenize it either using spaces or according to the KeyTable 
      // rule_keys, compile into a transducer and test against every rule. 
      // Display diagnostics
      while (cin.getline(input,1000))
	{
	  char * input_string = remove_negative_prefix(input);
	  bool negative = (input != input_string);
	  if ( not negative )
	    {
	      input_string = remove_positive_prefix(input);
	    }

	  original_input_string = strdup(input);
	  if (debug)
	    {
	      fprintf(output_file,"INPUT: %s\n",input_string);
	    }
	  //Transform the input string into a KeyVector, either by tokenizing 
	  //or splitting at spaces and coding using sppropriate keys.
	  KeyPairVector * input_vector;
	  if (not spaces)
	    {
	      input_vector = HWFST::tokenize(input_string);
	    }
	  else
	    {
	      input_vector = HWFST::split_at_spaces(input_string);
	    }
	  if (debug)
	    {
			HWFST::display_pair_vector(input_vector);
	    }
	  if (input_vector == NULL)
	    {
	      if (debug or (not silent))
		{
		  fprintf(output_file,">> ");
		}
	      passed_all_tests=false;
	      free(original_input_string);
	      continue;
	    }
	  bool passed_test = HWFST::test_correspondence(input_vector,rules,
						 rule_transducer_names,
						 negative);
	  if ( not passed_test )
	    {
	      if (not silent)
		{
		  fprintf(stderr,"%s FAILS\n",original_input_string);
		}	     
	    }
	  else
	    {
	      if (debug or verbose) 
		{
		  fprintf(stderr,"%s is OK\n",original_input_string);
		}
	    }
	  passed_all_tests = passed_all_tests and passed_test;
	  if (debug or (not silent))
	    {
	      fprintf(output_file,">> ");
	    }
	  free(original_input_string);
	}
      fprintf(output_file,"\n");
    }
  else {
      char input[1000];
      char * original_input_string;
      
      ifstream pairs_in(pair_file_name,std::ios::in);
      while (pairs_in.getline(input,1000))
	{
	  // skip lines only containing white-space.
	  if (only_white_space(input))
	    {
	      continue;
	    }
	  if (debug or verbose)
	    {
	      fprintf(output_file,"%s\n",input);
	    }
	  char * input_string;
	  input_string = remove_negative_prefix(input);
	  bool negative = (input_string != input);
	  if ( not negative)
	    {
	      input_string = remove_positive_prefix(input);
	    }
	  original_input_string = strdup(input);
	  if (debug)
	    {
	      fprintf(output_file,"INPUT STRING: %s\n",input_string);
	    }
	  //Transform the input string into a KeyVector, either by tokenizing 
	  //or splitting at spaces and coding using sppropriate keys.
	  KeyPairVector * input_vector;
	  if (not spaces)
	    {
	      input_vector = HWFST::tokenize(input_string);
	    }
	  else
	    {
	      input_vector = HWFST::split_at_spaces(input_string);
	    }
	  if (debug)
	    {
			HWFST::display_pair_vector(input_vector);
	    }
	  if (input_vector == NULL)
	    {
	      free(original_input_string);
	      passed_all_tests = false;
	      continue;
	    }
	  bool passed_test = HWFST::test_correspondence(input_vector,rules,
						 rule_transducer_names,
						 negative);
	  if ( not passed_test )
	    {
	      if (not silent)
		{
		  fprintf(stderr,"%s FAILS\n\n",original_input_string);
		}
	    }
	  else
	    {
	      if (debug or verbose) 
		{
		  fprintf(stderr,"%s is OK\n\n",original_input_string);
		}
	    }
	  passed_all_tests = passed_all_tests and passed_test;
	  free(original_input_string);
	}
  }
  for ( HWFST::FstVector::iterator it = rules->begin();
	it != rules->end();
	++it )
    {
      delete *it;
    }
  delete rules;
  free(program_name);
  free(fst_file_name);
  free(alphabet_file_name);
  if ( passed_all_tests )
    {
      exit(0);
    }
  else
    {
      exit(1);
    }
  }
}
