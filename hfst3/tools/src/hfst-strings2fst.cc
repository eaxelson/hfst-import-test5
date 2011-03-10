//! @file hfst-strings2fst.cc
//!
//! @brief string compiling command line tool
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

#include <vector>
#include <map>

using std::vector;
using std::pair;

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>
#include <errno.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "implementations/HfstTransitionGraph.h"
#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"

using hfst::HfstOutputStream;
using hfst::HfstTokenizer;
using hfst::HfstTransducer;
using hfst::implementations::HfstBasicTransducer;
//using hfst::HfstInternalTransducer;
//using hfst::implementations::HfstTrie;
using hfst::StringPairVector;
using hfst::StringPair;
using hfst::StringSet;
using hfst::StringVector;

static char *epsilonname=NULL; // FIX: use this
static bool has_spaces=false;
static bool disjunct_strings=false;
static bool pairstrings=false;
static char *multichar_symbol_filename=NULL;
static StringSet multichar_symbols;

static float sum_of_weights=0;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;

float divide_by_sum_of_weights(float weight) {
  if (sum_of_weights == 0)
    return 0;
  return weight/sum_of_weights;
}
float divide_by_sum_of_weights_log(float weight) {
  if (sum_of_weights == 0)
    return weight;
  return weight + log(sum_of_weights);
}
float take_negative_logarithm(float weight) {
  float result;
  if (weight == 0)
    result = INFINITY;
  else
    {
      errno = 0;
      result = -log(weight);
      if (errno != 0)
        {
          error(EXIT_FAILURE, errno, "unable to take negative logarithm");
        }
    }
  return result;
}

void
print_usage()
{
    // c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
    fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
        "Compile string pairs and pair-strings into transducer(s)\n"
        "\n", program_name); 
        print_common_program_options(message_out);
        print_common_unary_program_options(message_out); 
        fprintf(message_out, "String and format options:\n"
                "  -f, --format=FMT          Write result in FMT format\n"
                "  -j, --disjunct-strings    Disjunct all strings instead of "
                    "transforming each string into a separate transducer\n"
                /*"      --sum                 Sum weights of duplicate strings "
          "instead of taking minimum\n"*/
                "      --norm                Divide each weight by sum "
                    "of all weights (with option -j)\n"
                "      --log                 Take negative logarithm "
                    "of each weight\n"
                "  -p, --pairstring          Input is in pairstring format\n"
                "  -S, --spaces              Input has spaces between "
                    "transitions\n"
                "  -e, --epsilon=EPS         How epsilon is represented.\n"
        "  -m, --multichar-symbols=FILE   Strings that must be tokenized as one symbol.\n"
        );
        fprintf(message_out, "\n");

        fprintf(message_out, 
            "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
            "FMT must be name of a format usable by libhfst, such as "
            "openfst-tropical, sfst or foma\n"
            "If EPS is not defined, the default representation of @0@ is used.\n"
        "Option --log precedes option --norm.\n"
        "The FILE of option -m lists all multichar-symbols, each symbol on its own line.\n"   
      "The backslash '\\' is reserved for escaping a colon (\"\\:\"), \n"
		"a space (\"\\ \") or a backslash (\"\\\\\") \n"	
        "\n"
            );

        fprintf(message_out, "Examples:\n"
            "  echo \"cat:dog\" | %s        create cat:dog fst\n"
            "  echo \"c:da:ot:g\" | %s -p   same as pairstring\n"
            "  echo \"c:d a:o t:g | %s -S   same with spaces\n"
            "\n", program_name, program_name, program_name);
        print_report_bugs();
        fprintf(message_out, "\n");
        print_more_info();
        fprintf(message_out, "\n");
}

int
parse_options(int argc, char** argv)
{
    // use of this function requires options are settable on global scope
    while (true)
    {
        static const struct option long_options[] =
        {
        HFST_GETOPT_COMMON_LONG,
        HFST_GETOPT_UNARY_LONG,
          {"disjunct-strings", no_argument, 0, 'j'},
          {"epsilon", required_argument, 0, 'e'},
          {"norm", no_argument, 0, '2'},
          {"log", no_argument, 0, '3'},
          {"pairstrings", no_argument, 0, 'p'},
          {"spaces", no_argument, 0, 'S'},
      {"multichar-symbols", required_argument, 0, 'm'},
          {"format", required_argument, 0, 'f'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "je:23pSm:f:",
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
            epsilonname = hfst_strdup(optarg);
            break;
        case '2':
            normalize_weights = true;
            break;
        case '3':
            logarithmic_weights = true;
            break;
        case 'j':
            disjunct_strings = true;
            break;
        case 'S':
            has_spaces = true;
            break;
        case 'p':
            pairstrings = true;
            break;
    case 'm':
        multichar_symbol_filename = hfst_strdup(optarg);
        break;
        case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
#include "inc/getopt-cases-error.h"
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    if (output_format == hfst::UNSPECIFIED_TYPE)
      {
        verbose_printf("Output format not specified, "
             "defaulting to openfst tropical\n");
        output_format = hfst::TROPICAL_OPENFST_TYPE;
      }
    if (epsilonname == 0)
      {
        epsilonname = hfst_strdup("@0@");
      }
    return EXIT_CONTINUE;
}


int
process_stream(HfstOutputStream& outstream)
{
  size_t transducer_n = 0;
  char* line = 0;
  size_t len = 0;
  HfstTokenizer tok;
  HfstBasicTransducer disjunction;
  size_t line_n = 0;

  // add multicharater symbols to tokenizer
  for(StringSet::const_iterator it=multichar_symbols.begin();
      it != multichar_symbols.end(); it++ ) 
    {
      tok.add_multichar_symbol(*it);
    }
  // add skip symbol '\\' to tokenizer
  tok.add_skip_symbol("\\");
  tok.add_multichar_symbol("\\\\");

  while (hfst_getline(&line, &len, inputfile) != -1)
    {
      transducer_n++;
      line_n++;
      verbose_printf("Parsing line %u...\n", line_n);
      // parse line end and weight
      char* tab = strstr(line, "\t");
      char* string_end = tab;
      double weight = 0.0;
      bool weighted = false;
      if (tab == NULL)
        {
          string_end = line;
          while ((*string_end != '\0') && (*string_end != '\n'))
            {
              string_end++;
            }
        }
      else
        {
      // change '\n' to '\0'
      char *p = tab;
      while (*p != '\0') {
        if (*p == '\n')
          *p = '\0';
        p++;
      }

          weight = hfst_strtoweight(tab+1);
          weighted = true;
        }
      *string_end = '\0';

      // Parse the string
      StringPairVector spv;

      // (1) of form "c:d a:o t:g"
      if (has_spaces && pairstrings)
        {
          char* pair = strtok(line, " ");
          while (pair != NULL)
            {
              char* colon = strchr(pair, ':');
              char* pair_end = pair;
              while (*pair_end != '\0')
                {
                  pair_end++;
                }
              if (colon != NULL)
                {
                  char* upper = hfst_strndup(pair, colon - pair);
                  char* lower = hfst_strndup(colon, pair_end - colon);
          spv.push_back(StringPair(std::string(upper), std::string(lower)));
                }
              else
                {
          spv.push_back(StringPair(std::string(pair), std::string(pair)));
                }
              strtok(NULL, " ");
            }
        }

      // (2) of form "c a t:d o g"
      else if (has_spaces && !pairstrings)
        {
	  StringVector input_sv;
	  StringVector output_sv;

	  // Find out whether the line has two levels
	  char * second_string = NULL;
	  for (unsigned int i=0; line[i] != '\0'; i++)
	    {
	      if (line[i] == ':') {
		line[i] = '\0';
		second_string = &line[i+1];
		break;
	      }
	    }

	  // tokenize the input string
          char* input_string = strtok(line, " ");
          while (input_string != NULL)
            {
	      input_sv.push_back(std::string(input_string));
              input_string = strtok(NULL, " ");
            }

	  // tokenize the output string
	  if (second_string != NULL)
	    {
	      char* output_string = strtok(second_string, " ");
	      while (output_string != NULL)
		{
		  output_sv.push_back(std::string(output_string));
		  output_string = strtok(NULL, " ");
		}
	    }
	  else 
	    {
	      output_sv = input_sv;
	    }

	  // convert into a string pair
	  for (unsigned int i=0; 
	       i < input_sv.size() || i < output_sv.size();
	       i++)
	    {	      
	      std::string istring;
	      std::string ostring;

	      if (i < input_sv.size())
		istring = input_sv[i];
	      else
		istring = std::string("@_EPSILON_SYMBOL_@");

	      if (i < output_sv.size())
		ostring = output_sv[i];
	      else
		ostring = std::string("@_EPSILON_SYMBOL_@");

	      spv.push_back(StringPair(istring, ostring));
	    }
        }

      // (3) of form "c:da:ot:g"
      else if (!has_spaces && pairstrings)
        {
          fprintf(stderr, "FIXME: unimplemented !has_spaces && pairstrings\n");
          return EXIT_FAILURE;
        }

      // (4) of form "cat:dog"
      else if (!has_spaces && !pairstrings)
        {
	  StringVector input_sv;
	  StringVector output_sv;

	  // Find out whether the line has two levels
	  char * second_string = NULL;
	  for (unsigned int i=0; line[i] != '\0'; i++)
	    {
	      if (line[i] == ':') {
		line[i] = '\0';
		second_string = &line[i+1];
		break;
	      }
	    }

	  // tokenize the input string
	  input_sv = tok.tokenize_one_level(std::string(line));

	  // tokenize the output string
	  if (second_string != NULL)
	    output_sv = tok.tokenize_one_level(std::string(second_string));
	  else
	    output_sv = input_sv;

	  // convert into a string pair
	  for (unsigned int i=0; 
	       i < input_sv.size() || i < output_sv.size();
	       i++)
	    {	      
	      std::string istring;
	      std::string ostring;

	      if (i < input_sv.size())
		istring = input_sv[i];
	      else
		istring = std::string("@_EPSILON_SYMBOL_@");

	      if (i < output_sv.size())
		ostring = output_sv[i];
	      else
		ostring = std::string("@_EPSILON_SYMBOL_@");

	      spv.push_back(StringPair(istring, ostring));
	    }

#ifdef FOO
          const char* colon = strstr(line, ":");
          while (colon != NULL)
            {
              if (colon == line)
                {
                  error_at_line(0, 0, inputfilename, line_n, 
                                "line may not start with unescaped colon");
                  colon = strstr(colon + 1, ":");
                }
              else if (*(colon-1) == '\\' && *(colon-2) != '\\')
                {
                  colon = strstr(colon + 1, ":");
                }
              else
                {
                  break;
                }
            }
          char* first;
          char* second;
          if (colon != NULL)
            {
              first = hfst_strndup(line, colon-line);
              second = hfst_strndup(colon+1, string_end - colon);
            }
          else
            {
              first = hfst_strndup(line, string_end-line);
              second = first;
            }

          verbose_printf("Found %s:%s...\n", first, second);
	  StringPairVector spv_tok = tok.tokenize(std::string(first), std::string(second));
	  for (StringPairVector::iterator it = spv_tok.begin(); 
	       it != spv_tok.end(); it++)
	    {
	      if (it->first.compare("\\\\") == 0)
		it->first = std::string("\\");
	      if (it->second.compare("\\\\") == 0)
		it->second = std::string("\\");
	    }
	  
	  spv = spv_tok;
#endif // FOO
        }

      // Handle the weight
      float path_weight=0;

      if (weighted)
        {
          sum_of_weights = sum_of_weights + weight;
          if (!logarithmic_weights)
            {
              path_weight=weight;
            }
          else
            {
              path_weight=take_negative_logarithm(weight);
            }
          verbose_printf("Using final weight %f...\n", weight);
        }

      if (!disjunct_strings) // each string into a transducer
        {
          HfstBasicTransducer tr;
          tr.disjunct(spv, path_weight);
          HfstTransducer res(tr, output_format);
          outstream << res;
        }
      else // disjunct all strings into a single transducer
        {
          disjunction.disjunct(spv, path_weight);
        }
    }
  if (disjunct_strings)
    {
      HfstTransducer res(disjunction, output_format);

      if (normalize_weights) 
        {
          verbose_printf("Normalising weights...\n");
          if (!logarithmic_weights)
            res.transform_weights(&divide_by_sum_of_weights);
          else
            res.transform_weights(&divide_by_sum_of_weights_log);
        }
          outstream << res;
    }
  free(line);
  return EXIT_SUCCESS;
}


int main( int argc, char **argv ) 
{
  hfst_set_program_name(argv[0], "0.1", "Strings2Fst");
  int retval = parse_options(argc, argv);
  if (retval != EXIT_CONTINUE)
    {
      return retval;
    }

  if (multichar_symbol_filename != NULL)
    {
      verbose_printf("Reading multichar stuff from %s\n", multichar_symbol_filename);
      FILE *file = hfst_fopen(multichar_symbol_filename, "r");
      char* line = 0;
      size_t len = 0;

      while (hfst_getline(&line, &len, file) != -1)
        {
            for (char* p = line; line != '\0'; p++)
              {
                if (*p == '\n') 
                  {
                    *p='\0';
                  }
              }
            multichar_symbols.insert(std::string(line));
          }
    }

  // close output buffers, we use output streams
  if (outfile != stdout)
    {
      fclose(outfile);
    }
  verbose_printf("Reading from %s, writing to %s\n", 
                 inputfilename, outfilename);
  // here starts the buffer handling part
  HfstOutputStream* outstream = (outfile != stdout) ?
        new HfstOutputStream(outfilename, output_format) :
        new HfstOutputStream(output_format);
  process_stream(*outstream);
  free(inputfilename);
  free(outfilename);
  return EXIT_SUCCESS;
}

