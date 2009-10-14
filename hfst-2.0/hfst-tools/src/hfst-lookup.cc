//! @file hfst-lookup.cc
//!
//! @brief Transducer lookdown command line tool
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
#include "hfst-lookup-commandline.h"
#include "hfst-lookup-transducers.h"

using std::cin;
using std::cout;
using std::vector;

static FILE * output_file = stdout;
static const size_t INPUT_BUFFER_SIZE = 1000;

int main(int argc, char * argv[])
{
  // Get all arguments.
  parse_options(argc,argv);

  if (debug) {
    display_all_arguments();
  }


  if (strings_file_name == NULL)
    {
      usage();
      exit(1);
    }
  read_input_transducer();

  //If the transducer didn't have a keytable stored with it,
  //try to trad one from a separate file.
  get_alphabet(alphabet_file_name);

  if (debug)
    {
      display_fst_keys();
    }

  if ( not spaces ) {
    if (debug)
      {
	fprintf(stderr,"Building tokenizer\n");
      }
    if (weighted)
      {
	HWFST::make_tokenizer();
      }
    else
      {
	HFST::make_tokenizer();
      }
    if (debug)
      {
	fprintf(stderr,"Done\n");
      }
  }
  
  // Read test_strings from input_file_name and write them to
  // output_file.  
  ifstream input_string_file(strings_file_name);
  FILE * output_file;
  if (output_file_name == NULL)
    {
      output_file = stdout;
    }
  else
    {
      output_file = fopen(output_file_name,
			  "w");
    }
  if (not silent)
    {
      fprintf(stderr,
	      "Read input transducer.\n"
	      "Processing input string file.\n");      
    }

  char input[INPUT_BUFFER_SIZE];
  
  // The main program loop. Read a input string,
  // Apply every transducer in turn and display the results.
  while (input_string_file.getline(input,INPUT_BUFFER_SIZE))
    {
      fprintf(output_file,"INPUT: %s\n",input);
      
      KeyVector * tokenized_input_form;
      
      if (not spaces) 
	{
	  if (weighted)
	    {
	      tokenized_input_form = HWFST::tokenize(input);
	    }
	  else
	    {
	      tokenized_input_form = HFST::tokenize(input);
	    }
	}
      else
	{
	  tokenized_input_form = split_at_spaces(input);
	}
      
      if (tokenized_input_form == NULL)
	    {
	      fprintf(output_file,"NO TOKENIZATION\n\n");
	      continue;
	    }
      
      KeyVectorVector * output_strings;

      if (weighted)
	{
	  if (HWFST::is_infinitely_ambiguous(weighted_fst,
					     true,
					     tokenized_input_form))
	    {
	      HWFST::KeyVector * first_output = HWFST::lookup_first(weighted_fst,tokenized_input_form);
	      if (first_output != NULL)
		{
		  fprintf(output_file,"WARNING! Input \"%s\" has infinitely many analyses. One of them is:\n",input);
		  output_strings = new KeyVectorVector;
		  output_strings->push_back(first_output);
		}
	      else
		{
		  output_strings = new HWFST::KeyVectorVector;
		}
	    }
	  else 
	    {
	      output_strings =
		HWFST::lookup_all(weighted_fst,
				  tokenized_input_form);
	    }
	  display_result_set(output_strings,output_file,spaces);
	  delete output_strings;
	}
      else
	{
	  if (HFST::is_infinitely_ambiguous(unweighted_fst,
					    true,
					    tokenized_input_form))
	    {
	      HFST::KeyVector * first_output = HFST::lookup_first(unweighted_fst,tokenized_input_form);
	      if (first_output != NULL)
		{
		  fprintf(output_file,"WARNING! Input \"%s\" has infinitely many analyses. One of them is:\n",input);
		  output_strings = new KeyVectorVector;
		  output_strings->push_back(first_output);
		}
	      else
		{
		  output_strings = new HFST::KeyVectorVector;
		}
	    }
	  else
	    {
	      output_strings =
		HFST::lookup_all(unweighted_fst,
				 tokenized_input_form);
	    }
	  display_result_set(output_strings,output_file,spaces);
	  delete output_strings;
	}

      delete tokenized_input_form;

      fprintf(output_file,"\n");
    }
  fprintf(output_file,"\n");
  delete unweighted_fst;
  delete weighted_fst;
}
