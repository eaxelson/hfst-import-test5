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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <math.h>

#include "hfst-commandline.h"
#include "hfst-program-options.h"
#include <hfst2/hfst.h>

#include "inc/globals-unary.h"

static const Key EPSILON_KEY=0;

static char *epsilonname=NULL;
static bool has_spaces=false;
static bool is_weighted=false;
static bool disjunct_strings=false;
static bool pairstrings=false;

static unsigned int sum_of_weights=0;
static bool sum_weights=false;
static bool normalize_weights=false;
static bool logarithmic_weights=false;

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
    result = -log10(weight);
  return result;
}

void
print_usage(const char *program_name)
{
	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(message_out, "Usage: %s [OPTIONS...] [INFILE]\n"
		"Compile string pairs and pair-strings into transducer(s)\n"
		"\n", program_name); 
		print_common_program_options(message_out);
#               if DEBUG
		fprintf(message_out,
			"%-35s%s", "-d, --debug", "Print debugging messages and results\n"
			);
#               endif
		  
		fprintf(message_out, "%-35s%s",	"  -i, --input=INFILE",  	"Read input transducer from INFILE\n");
		fprintf(message_out, "%-35s%s",	"  -o, --output=OUTFILE", 	"Write output transducer to OUTFILE\n");
		fprintf(message_out, "%-35s%s",	"  -R, --read-symbols=FILE", 	"Read symbol table from FILE\n");
		fprintf(message_out, "%-35s%s",	"  -D, --do-not-write-symbols", "Do not write symbol table with the output transducer(s)\n");
		fprintf(message_out, "%-35s%s",	"  -W, --write-symbols-to=FILE","Write symbol table to file FILE\n");
		fprintf(message_out, "%-35s%s",	"  -w, --weighted",         "Write result in weighted format\n");
		fprintf(message_out, "\n");
		fprintf(message_out, "%-35s%s",	"  -j, --disjunct-strings", "Disjunct all strings instead of transforming each string into a separate transducer\n"); 
		fprintf(message_out, "%-35s%s",	"      --sum",              "Sum weights of duplicate strings instead of taking the minimum\n");
		fprintf(message_out, "%-35s%s",	"      --norm",             "Divide each weight by the sum of all weights\n");
		fprintf(message_out, "%-35s%s",	"      --log",              "Take the negative logarithm of each weight\n");
		fprintf(message_out, "%-35s%s",	"  -p, --pairstring",       "The input is in pairstring format\n");
		fprintf(message_out, "%-35s%s",	"  -S, --spaces",           "The input has spaces between transitions\n");
		fprintf(message_out, "%-35s%s",	"  -e, --epsilon=EPS",      "If no symbol table is given, map EPS as zero.\n");
		fprintf(message_out, "%-35s%s",	"",                         "Can be used only with option -S\n");
		fprintf(message_out, "\n");

		fprintf(message_out, 
			"If OUTFILE or INFILE is missing or -, standard streams will be used.\n"
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
			"Examples:\n\n"
			"Make a transducer that maps 'cat' to 'dog'\n"
			"  echo \"cat:dog\" | %s -R symbols\n"             
			"  echo \"c:da:ot:g\" | %s -R symbols -p   (in pairstring format)\n"        
			"  echo \"c:d a:o t:g\" | %s -e eps -p -S  (no need to give the symbol table)\n"   
			"\n", program_name, program_name, program_name); */
		fprintf(stderr, "\n");
		print_more_info(message_out, "Strings2Fst");
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
		  {"disjunct-strings", no_argument, 0, 'j'},
		  {"epsilon", required_argument, 0, 'e'},
		  {"sum", no_argument, 0, '1'},
		  {"norm", no_argument, 0, '2'},
		  {"log", no_argument, 0, '3'},
		  {"pairstrings", no_argument, 0, 'p'},
		  {"spaces", no_argument, 0, 'S'},
		  {"weighted", no_argument, 0, 'w'},
		  {0,0,0,0}
		};
		int option_index = 0;
		char c = getopt_long(argc, argv, "R:dhi:o:pqvVwW:DjsSe:F123",
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
invert_stream(std::istream& inputstream, std::ostream& outstream)
{
    size_t nth_line=0;
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
		  else if (has_spaces && epsilonname) {
		    key_table = HFST::create_key_table();
		    HFST::associate_key(EPSILON_KEY, key_table, HFST::define_symbol(epsilonname));
		  } 
		  else {
		    fprintf(message_out, "no symbol table file defined\n");
		    return EXIT_FAILURE;
		  }			  
		  
		  if (debug) {
		    fprintf(stderr, "The key table:\n");
		    HFST::print_key_table(key_table);
		  }
			     

		  HFST::TransducerHandle tok = HFST::longest_match_tokenizer2(key_table);

		  if (debug) {
		    fprintf(stderr, "The tokenization transducer:\n");
		    HFST::print_transducer(tok, key_table, cerr);
		  }

		  inputstream.peek();
		  while (!inputstream.eof()) {
		    char line[256];
		    inputstream.getline(line,256);
		    if (strcmp(line,"") == 0) // an empty line
		      break;

		    if (has_spaces && pairstrings) {

		      float weight=0;
		      vector<pair<char*,char*> > string_pairs;
		      string_pairs = parse_pairstring_with_spaces_and_weight(line, weight);

		      HFST::KeyPairVector *kpv = new HFST::KeyPairVector();
		      
		      for (unsigned int i=0; i<string_pairs.size(); i++) {
			pair<char*,char*> a_pair = string_pairs[i];
			KeyPair *kp;
			if (read_symbols_from_filename)
			  kp = HFST::define_keypair( HFST::get_key( HFST::get_symbol(a_pair.first), key_table ),
						     HFST::get_key( HFST::get_symbol(a_pair.second), key_table ) );
			else
			  kp = HFST::define_keypair( key_table->add_symbol(HFST::define_symbol(a_pair.first)),
						     key_table->add_symbol(HFST::define_symbol(a_pair.second)) );
			kpv->push_back(kp);
		      }

		      result = HFST::disjunct_as_trie(result, kpv, weight, sum_weights);
		      sum_of_weights = sum_of_weights + (unsigned int)weight;
		    }

		    else if (has_spaces && !pairstrings) {

		      float weight=0;
		      char *ostring;
		      ostring = parse_output_string_and_weight(line, weight);
		      
		      vector<char*> input = parse_identity_string_with_spaces(line);
		      vector<char*> output;
		      if (ostring != NULL)
			output = parse_identity_string_with_spaces(ostring);
		      else
			output = input;

		      HFST::KeyPairVector *kpv = new HFST::KeyPairVector();
		      
		      unsigned int i=0;  // input index
		      unsigned int j=0;  // output index
		      char *input_string=NULL;
		      char *output_string=NULL;

		      while (i<input.size() || j<output.size()) {
			if (i<input.size())
			  input_string = input[i];
			else
			  input_string = epsilonname;
			if (j<output.size())
			  output_string = output[j];
			else
			  output_string = epsilonname;
			KeyPair *kp;
			if (read_symbols_from_filename)
			  kp = HFST::define_keypair( HFST::get_key( HFST::get_symbol(input_string), key_table ),
						     HFST::get_key( HFST::get_symbol(output_string), key_table ) );
			else
			  kp = HFST::define_keypair( key_table->add_symbol(HFST::define_symbol(input_string)),
						     key_table->add_symbol(HFST::define_symbol(output_string)) );
			kpv->push_back(kp);
			i++;
			j++;
		      }
			
		      result = HFST::disjunct_as_trie(result, kpv, weight, sum_weights);
		      sum_of_weights = sum_of_weights + (unsigned int)weight;
			   
		    }   

		    else if (pairstrings) {
		      float weight=0;
		      vector<char*> strings;
		      strings = parse_pairstring_and_weight(line, weight);

		      vector<HFST::KeyVector*> strings_tok;
		      for (unsigned int i=0; i<strings.size(); i++) {
			HFST::KeyVector *kv = HFST::longest_match_tokenize(tok, strings[i], key_table);
			if (kv == NULL) {
			  fprintf(message_out, "part \"%s\" of input line could not be tokenized\n", strings[i]);
			  return EXIT_FAILURE;
			}
			strings_tok.push_back(kv);
		      }

		      HFST::KeyPairVector *kpv = new HFST::KeyPairVector();

		      HFST::Key last_key=0; 
		      for (unsigned int i=0; i<strings_tok.size(); i++) {
			HFST::KeyVector *kv = strings_tok[i];			
			if (i>0) {
			  KeyPair *kp = HFST::define_keypair(last_key, kv->at(0));
			  kpv->push_back(kp);
			}
			unsigned int start=1;
			if (i==0)
			  start=0;
			unsigned int end=1;
			if (i==strings_tok.size()-1)
			  end=0;
			for (unsigned int j=start; j<kv->size()-end; j++) {
			  KeyPair *kp = HFST::define_keypair(kv->at(j));
			  kpv->push_back(kp);
			}
			last_key = kv->at(kv->size()-1);
		      }

		      result = HFST::disjunct_as_trie(result, kpv, weight, sum_weights);
		      sum_of_weights = sum_of_weights + (unsigned int)weight;

		    }
		    else {
		      float weight=0;
		      char *ostring;
		      ostring = parse_output_string_and_weight(line, weight);
		      nth_line++;
		      
              if (nth_line < 2)
                {
                  verbose_printf("Read one line...\n");
                }
              else
                {
                  verbose_printf("Read one line... %zu\r", nth_line);
                }

		      if (true) {
			
			if (ostring == NULL) {
			  HFST::KeyVector *kv = HFST::longest_match_tokenize(tok, line, key_table);
			  if (kv == NULL) {
			    fprintf(message_out, "input line \"%s\" could not be tokenized\n", line);
			    return EXIT_FAILURE;
			  }
			  result = HFST::disjunct_as_trie(result, kv, weight, sum_weights);
			  sum_of_weights = sum_of_weights + (unsigned int)weight;  
			}
			else {
			  HFST::KeyPairVector *kpv = HFST::longest_match_tokenize_pair(tok, line, ostring, key_table);
			  if (kpv == NULL) {
			    fprintf(message_out, "input line \"%s:%s\" could not be tokenized\n", line, ostring);
			    return EXIT_FAILURE;
			  }
			  result = HFST::disjunct_as_trie(result, kpv, weight, sum_weights);
			  sum_of_weights = sum_of_weights + (unsigned int)weight;  
			}
		      }
		    }

		    if (!disjunct_strings) {
		      if (epsilonname) { // is this needed???
			HFST::Key epsilonkey = HFST::get_key(HFST::get_symbol(epsilonname), key_table);
			result = HFST::substitute_key(result, epsilonkey, HFST::Epsilon);
		      }
		      if (write_symbols)
			HFST::write_transducer(result, key_table, outstream);
		      else
			HFST::write_transducer(result, outstream);
		      result = HFST::create_empty_transducer();
		    }
		    
		    inputstream.peek();
		  }			
		  if (disjunct_strings) {
		    if (epsilonname) {
			HFST::Key epsilonkey = HFST::get_key(HFST::get_symbol(epsilonname), key_table);
			result = HFST::substitute_key(result, epsilonkey, HFST::Epsilon);
		      }
		    // normalization and taking logarithm is done here
		    if (normalize_weights) {
		      result = HFST::modify_weights(result, divide_by_sum_of_weights);
		    }
		    if (logarithmic_weights) {
		      result = HFST::modify_weights(result, take_negative_logarithm);
		    }

		    if (write_symbols)
		      HFST::write_transducer(result, key_table, outstream);
		    else
		      HFST::write_transducer(result, outstream);
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
		  else if (has_spaces && epsilonname) {
		    key_table = HWFST::create_key_table();
		    HWFST::associate_key(EPSILON_KEY, key_table, HWFST::define_symbol(epsilonname));
		  } 
		  else {
		    fprintf(message_out, "no symbol table file defined\n");
		    return EXIT_FAILURE;
		  }			  
		  
		  HWFST::TransducerHandle tok = HWFST::longest_match_tokenizer2(key_table);

		  if (debug) {
		    fprintf(stderr, "The tokenization transducer:\n");
		    HWFST::print_transducer(tok, key_table, cerr);
		  }

		  inputstream.peek();
		  while (!inputstream.eof()) {
		    char line[256];
		    inputstream.getline(line,256);
		    if (strcmp(line,"") == 0) // an empty line
		      break;

		    if (has_spaces && pairstrings) {
		      float weight=0;
		      vector<pair<char*,char*> > string_pairs;
		      string_pairs = parse_pairstring_with_spaces_and_weight(line, weight);

		      HWFST::KeyPairVector *kpv = new HWFST::KeyPairVector();
		      
		      for (unsigned int i=0; i<string_pairs.size(); i++) {
			pair<char*,char*> a_pair = string_pairs[i];
			KeyPair *kp;
			if (read_symbols_from_filename)
			  kp = HWFST::define_keypair( HWFST::get_key( HWFST::get_symbol(a_pair.first), key_table ),
						     HWFST::get_key( HWFST::get_symbol(a_pair.second), key_table ) );
			else
			  kp = HWFST::define_keypair( key_table->add_symbol(HWFST::define_symbol(a_pair.first)),
						     key_table->add_symbol(HWFST::define_symbol(a_pair.second)) );
			kpv->push_back(kp);
		      }

		      result = HWFST::disjunct_as_trie(result, kpv, weight, sum_weights);
		      sum_of_weights = sum_of_weights + (unsigned int)weight;
		    }

		    else if (has_spaces && !pairstrings) {

		      float weight=0;
		      char *ostring;
		      ostring = parse_output_string_and_weight(line, weight);
		      
		      vector<char*> input = parse_identity_string_with_spaces(line);
		      vector<char*> output;
		      if (ostring != NULL)
			output = parse_identity_string_with_spaces(ostring);
		      else
			output = input;

		      HWFST::KeyPairVector *kpv = new HWFST::KeyPairVector();
		      
		      unsigned int i=0;  // input index
		      unsigned int j=0;  // output index
		      char *input_string=NULL;
		      char *output_string=NULL;

		      while (i<input.size() || j<output.size()) {
			if (i<input.size())
			  input_string = input[i];
			else
			  input_string = epsilonname;
			if (j<output.size())
			  output_string = output[j];
			else
			  output_string = epsilonname;
			KeyPair *kp;
			if (read_symbols_from_filename)
			  kp = HWFST::define_keypair( HWFST::get_key( HWFST::get_symbol(input_string), key_table ),
						     HWFST::get_key( HWFST::get_symbol(output_string), key_table ) );
			else
			  kp = HWFST::define_keypair( key_table->add_symbol(HWFST::define_symbol(input_string)),
						     key_table->add_symbol(HWFST::define_symbol(output_string)) );
			kpv->push_back(kp);
			i++;
			j++;
		      }
			
		      result = HWFST::disjunct_as_trie(result, kpv, weight, sum_weights);
		      sum_of_weights = sum_of_weights + (unsigned int)weight;
			   
		    }   

		    else if (pairstrings) {
		      float weight=0;
		      vector<char*> strings = parse_pairstring_and_weight(line, weight);

		      vector<HWFST::KeyVector*> strings_tok;
		      for (unsigned int i=0; i<strings.size(); i++) {
			HWFST::KeyVector *kv = HWFST::longest_match_tokenize(tok, strings[i], key_table);
			if (kv == NULL) {
			  fprintf(message_out, "part \"%s\" of input line could not be tokenized\n", strings[i]);
			  return EXIT_FAILURE;
			}
			strings_tok.push_back(kv);
		      }
		      
		      HWFST::KeyPairVector *kpv = new HWFST::KeyPairVector();

		      HWFST::Key last_key=0;
		      for (unsigned int i=0; i<strings_tok.size(); i++) {
			HWFST::KeyVector *kv = strings_tok[i];			
			if (i>0) {
			  KeyPair *kp = HWFST::define_keypair(last_key, kv->at(0));
			  kpv->push_back(kp);
			}
			unsigned int start=1;
			if (i==0)
			  start=0;
			unsigned int end=1;
			if (i==strings_tok.size()-1)
			  end=0;
			for (unsigned int j=start; j<kv->size()-end; j++) {
			  KeyPair *kp = HWFST::define_keypair(kv->at(j));
			  kpv->push_back(kp);
			}
			last_key = kv->at(kv->size()-1);
		      }

		      result = HWFST::disjunct_as_trie(result, kpv, weight, sum_weights);    
		      sum_of_weights = sum_of_weights + (unsigned int)weight;
		    }

		    else {
		      float weight=0;
		      char *ostring = parse_output_string_and_weight(line, weight);
		      
		      nth_line++;
		      
              if (nth_line < 2)
                {
                  verbose_printf("Read one line...\n");
                }
              else
                {
                  verbose_printf("Read one line... %zu\r", nth_line);
                }
		      
		      if (true) {
			
			if (ostring == NULL) {
			  HWFST::KeyVector *kv = HWFST::longest_match_tokenize(tok, line, key_table);
			  if (kv == NULL) {
			    fprintf(message_out, "input line \"%s\" could not be tokenized\n", line);
			    return EXIT_FAILURE;
			  }
			  result = HWFST::disjunct_as_trie(result, kv, weight, sum_weights);
			  sum_of_weights = sum_of_weights + (unsigned int)weight;
			}
			else {
			  HWFST::KeyPairVector *kpv = HWFST::longest_match_tokenize_pair(tok, line, ostring, key_table);
			  if (kpv == NULL) {
			    fprintf(message_out, "input line \"%s:%s\" could not be tokenized\n", line, ostring);
			    return EXIT_FAILURE;
			  }
			  result = HWFST::disjunct_as_trie(result, kpv, weight, sum_weights);
			  sum_of_weights = sum_of_weights + (unsigned int)weight;				      
			}
		      }
		    }

		    if (!disjunct_strings) {
		      if (epsilonname) {
			HWFST::Key epsilonkey = HWFST::get_key(HWFST::get_symbol(epsilonname), key_table);
			result = HWFST::substitute_key(result, epsilonkey, HWFST::Epsilon);
		      }
		      if (write_symbols)
			HWFST::write_transducer(result, key_table, outstream);
		      else
			HWFST::write_transducer(result, outstream);
		      result = HWFST::create_empty_transducer();
		    }
		    
		    inputstream.peek();
		  }			
		  if (disjunct_strings) {
		    if (epsilonname) {
			HWFST::Key epsilonkey = HWFST::get_key(HWFST::get_symbol(epsilonname), key_table);
			result = HWFST::substitute_key(result, epsilonkey, HWFST::Epsilon);
		      }
		    // normalization and taking logarithm is done here
		    if (normalize_weights) {
		      result = HWFST::modify_weights(result, divide_by_sum_of_weights);
		    }
		    if (logarithmic_weights) {
		      result = HWFST::modify_weights(result, take_negative_logarithm);
		    }

		    if (write_symbols)
		      HWFST::write_transducer(result, key_table, outstream);
		    else
		      HWFST::write_transducer(result, outstream);
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


int main( int argc, char **argv ) {
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

