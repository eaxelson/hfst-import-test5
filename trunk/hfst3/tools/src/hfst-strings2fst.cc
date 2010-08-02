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

#include "hfst-commandline.h"
#include "hfst-program-options.h"

#include "inc/globals-common.h"
#include "inc/globals-unary.h"


static char *epsilonname=NULL;
static bool has_spaces=false;
static bool disjunct_strings=false;
static bool pairstrings=false;

static unsigned int sum_of_weights=0;
static bool sum_weights=false;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

static hfst::ImplementationType output_format = hfst::UNSPECIFIED_TYPE;

float divide_by_sum_of_weights(float weight) {
  if (sum_of_weights == 0)
    return 0;
  return weight/sum_of_weights;
}
float take_negative_logarithm(float weight) {
  float result;
  if (weight == 0)
    result = INFINITY;
  else
    {
      errno = 0;
      result = -log10(weight);
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
                "      --sum                 Sum weights of duplicate strings "
                    "instead of taking minimum\n"
                "      --norm                Divide each weight by sum "
                    "of all weights\n"
                "      --log                 Take negative logarithm "
                    "of each weight\n"
                "  -p, --pairstring          Input is in pairstring format\n"
                "  -S, --spaces              Input has spaces between "
                    "transitions\n"
                "  -e, --epsilon=EPS         Map EPS as zero.\n");
        fprintf(message_out, "\n");

        fprintf(message_out, 
            "If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
            "FMT must be name of a format usable by libhfst, such as "
            "openfst-tropical, sfst, foma or hfst-optimized-weighted\n"
            "If EPS is not defined, the default representation of @0@ is used\n"
            );

        /*fprintf(message_out,
            "The input consists of strings separated by newlines. Each string is transformed into a transducer\n"
            "and written to output. If option -j is used, all resulting transducers are disjuncted instead of writing each\n"
            "transducer separately to output.\n"
            "\n"
            "The input string format is by default input_string:output_string. Both strings are tokenized separately\n"
            "and the i:th token of input string is matched against the i:th token of output string. If the strings do not\n"
            "have an equal amount of tokens, epsilon is matched against the rest of the tokens of the longer string.\n"
            "\n"
            "If the input string is in pairstring format, option -p must be used. In the pairstring format the token pairs\n"
            "are written one after another separated by a ':'.\n"
            "\n"
            "A symbol table must be defined with option -R, so the program knows how to tokenize the input\n"
            "(that might contain multicharacter symbols).\n"
            "If options -p and -S are used (i.e. the input is already tokenized), the symbol table parameter is optional,\n"
            "but the epsilon symbol must be defined with option -e in order to be correctly mapped to number zero.\n"
            "\n"
       */
        fprintf(message_out, "Examples:\n"
            "  echo \"cat:dog\" | %s -R symbols  create cat:dog fst\n"
            "  echo \"c:da:ot:g\" | %s -R symbols -p   same as pairstring\n"
            "  echo \"c:d a:o t:g\" | %s -e eps -p -S  (no need to give the symbol table)\n"   
            "\n", program_name, program_name, program_name);
        print_report_bugs();
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
          {"sum", no_argument, 0, '1'},
          {"norm", no_argument, 0, '2'},
          {"log", no_argument, 0, '3'},
          {"pairstrings", no_argument, 0, 'p'},
          {"spaces", no_argument, 0, 'S'},
          {"format", required_argument, 0, 'f'},
          {0,0,0,0}
        };
        int option_index = 0;
        char c = getopt_long(argc, argv, HFST_GETOPT_COMMON_SHORT
                             HFST_GETOPT_UNARY_SHORT "je:123pSf:",
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
        case '1':
            sum_weights = true;
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
        case 'f':
            output_format = hfst_parse_format_name(optarg);
            break;
        case '?':
            fprintf(message_out, "invalid option --%s\n",
                    long_options[option_index].name);
            print_short_help();
            return EXIT_FAILURE;
            break;
        default:
            fprintf(message_out, "invalid option -%c\n", c);
            print_short_help();
            return EXIT_FAILURE;
            break;
        }
    }

#include "inc/check-params-common.h"
#include "inc/check-params-unary.h"
    return EXIT_CONTINUE;
}




char *parse_output_string_and_weight(char *line, float& weight) {
  
  char *ostring=NULL;

  for (int i=0; line[i] != '\0'; i++) {
    // ':' does not need to be escaped anymore
    if (line[i] == '\\' && line[i+1] == ':') {
      int j=i;
      while (line[i] != '\0') {
	line[i] = line[i+1];
	i++;
      }
      i=j;
    }				    
    else if (line[i] == ':') {
      line[i] = '\0';
      ostring = &line[i+1];
    }
    else if (line[i] == '\t') {
      line[i] = '\0';
      i++;
      while (line[i] == '\t' || line[i] == ' ')
	i++;
      weight = (float)atof(&line[i]);
      break;
    }
  }
  return ostring;
}

// if input is "c:da:ot:g" returns c, da, ot, g
vector<char*> parse_pairstring_and_weight(char *line, float& weight) {

  vector<char*> res;
  int last_start=0;

  int i;
  for (i=0; line[i] != '\0' && line[i] != '\t'; i++) {
    if (line[i] == '\\') {
      int j=i;
      while (line[i] != '\0') {
	line[i] = line[i+1];
	i++;
      }
      i=j;
    }				    
    else if (line[i] == ':') {
      line[i] = '\0';
      res.push_back(&line[last_start]);
      last_start = i+1;
    }
  }
  res.push_back(&line[last_start]);
  if (line[i] == '\0')
    weight=0;
  else {
    line[i] = '\0';
    i++;
    while (line[i] == '\t' || line[i] == ' ')
      i++;
    weight = (float)atof(&line[i]);
  }
  return res;
}

// if input is "a:b c:d e:f" returns pairs (a,b); (c,d) and (e,f)
vector<pair<char*,char*> > parse_pairstring_with_spaces_and_weight(char *line, float& weight) {

  vector<pair<char*,char*> > res;
  int last_start=0;
  char *input=NULL;

  int i;
  for (i=0; line[i] != '\0' && line[i] != '\t'; i++) {
    if (line[i] == '\\') {
      int j=i;
      while (line[i] != '\0') {
	line[i] = line[i+1];
	i++;
      }
      i=j;
    }
    else if (line[i] == ':') {
      line[i] = '\0';
      input = &line[last_start]; // input of pair
      last_start = i+1;
    }
    else if (line[i] == ' ') {
      line[i] = '\0';
      if (input != NULL) // output of pair
	res.push_back( pair<char*,char*>(input, &line[last_start]) );
      else // identity pair
	res.push_back( pair<char*,char*>(&line[last_start], &line[last_start]) );
      last_start = i+1;
      input=NULL;
    }
  }
  if (input != NULL) // output of last pair
    res.push_back( pair<char*,char*>(input, &line[last_start]) ); 
  else // last identity pair
    res.push_back( pair<char*,char*>(&line[last_start], &line[last_start]) );
  input=NULL;

  if (line[i] == '\0')
    weight=0;
  else {
    line[i] = '\0';
    i++;
    while (line[i] == '\t' || line[i] == ' ')
      i++;
    weight = (float)atof(&line[i]);
  }
  return res;
}

vector<char*> parse_identity_string_with_spaces(char *line) {
  vector<char*> res;
  int last_start=0;

  int i;
  for (i=0; line[i] != '\0' && line[i] != '\t'; i++) {
    if (line[i] == '\\') {
      int j=i;
      while (line[i] != '\0') {
	line[i] = line[i+1];
	i++;
      }
      i=j;
    }
    else if (line[i] == ' ') {
      line[i] = '\0';
      res.push_back( &line[last_start] );
      last_start = i+1;
    }
  }
  res.push_back( &line[last_start] );
  return res;
}



int
process_stream()
{
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
	free(inputfilename);
	free(outfilename);
	return EXIT_SUCCESS;
}

