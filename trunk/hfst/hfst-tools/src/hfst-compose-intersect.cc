//! @file hfst-compose-intersect.cc
//!
//! @brief Interesecting composition command line tool
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
#include <cstring>
#include <ctime>
#include <iostream>
#include <getopt.h>
#include <vector>
#include <hfst2/hfst.h>

using std::vector;
using std::cin;
using std::ios;

typedef HFST::TransducerHandle UnWeightedFst;
typedef HWFST::TransducerHandle WeightedFst;

typedef vector<UnWeightedFst> UnWeightedFstVector;
typedef vector<WeightedFst> WeightedFstVector;

const int UNWEIGHTED_F = 0;
const int WEIGHTED_F = 1;
const int COMPACT_F = 2;
const int UNKNOWN_F = -2;

// Toggle, whether weighted of unweighted functions are used.
bool with_weights = false;

// Whether result is not minimized
static bool no_minimize = false;

// Program name
static char * program_name = NULL;

// Name of the lexicon transducer file.
static char * lexicon_file_name = NULL;

// Name of the file where the result of the intersecting
// composition is stored.
static char * output_file_name = NULL;

// If the option -W is used, the symbol table is stored in a
// file instead of being stored with the result. This is
// the name of that file.
static char * symbol_output_file_name = NULL;

// Read the symbol table from this file, if the
// option -S is used.
static char * symbol_input_file_name = NULL;

// output stream for messages and warnings
static FILE* message_out = stderr;

// debug mode.
static bool debug = false;

// verbose mode == normal mode.
static bool verbose = false;

// silent mode, print nothing.
static bool silent = false;

// Show help-message
static bool display_help = false;

// Display version.
static bool display_version = false;

// Store the symbols with the result, default is
// true.
static bool store_symbols_with_result = true;

// Time the intersecting composition ans minimzation.
static bool time_operation = false;

// If rules contain word boundaries, word boundaries need to be appended to 
// lexicon entries.
static bool rules_contain_word_boundary = false;

// The last commandline arguments are rule-files.
// In addition all transducer arguments after the 
// lexicon are considered rule transducers.
static vector<char*>::size_type rules_start_index = 0;
static vector<char*>::size_type rules_end_index = 0;

void message(const char * msg, const char * arg = NULL) 
{
  if (silent)
    {
      return;
    }
  if (arg != NULL)
    {
      char msg1[1000];
      snprintf(msg1,1000,msg,arg);
      fprintf(message_out,"%s\n",msg1);
    }
  else
    {
      fprintf(message_out,"%s\n",msg);
    }
}

void warn(const char * warning,
	  const char * arg1 = NULL, 
	  const char * arg2 = NULL) 
{
  fprintf(message_out,"\n");
  if (arg2==NULL) 
    {
      if (arg1==NULL)
	{
	  fprintf(message_out,"WARNING: %s\n\n",warning);
	}
      else
	{
	  char warning1[1000];
	  snprintf(warning1,1000,warning,arg1);
	  fprintf(message_out,"WARNING: %s\n\n",warning1);
	}
    }
  else
    {
      char warning1[1000];
      snprintf(warning1,1000,warning,arg1,arg2);
      fprintf(message_out,"WARNING: %s\n\n",warning1);
    }
}

void error(const char * error_msg,
	  const char * arg1 = NULL, 
	  const char * arg2 = NULL) 
{
  fprintf(stderr,"\n");
  if (arg2==NULL) 
    {
      if (arg1==NULL)
	{
	  fprintf(stderr,"ERROR: %s\n\n",error_msg);
	}
      else
	{
	  char error_msg1[1000];
	  snprintf(error_msg1,1000,error_msg,arg1);
	  fprintf(stderr,"ERROR: %s\n\n",error_msg1);
	}
    }
  else
    {
      char error_msg1[1000];
      snprintf(error_msg1,1000,error_msg,arg1,arg2);
      fprintf(stderr,"ERROR: %s\n\n",error_msg1);
    }
  exit(1);
}


HFST::KeyTable * unweighted_symbol_table;
HWFST::KeyTable * weighted_symbol_table;

vector<char*> rule_file_names;

UnWeightedFst unweighted_lexicon = NULL;
WeightedFst weighted_lexicon = NULL;

UnWeightedFstVector unweighted_rules;
WeightedFstVector weighted_rules;

UnWeightedFst unweighted_result = NULL;
WeightedFst weighted_result = NULL;

void usage(void) 
{
  fprintf(message_out,"USAGE: %s [ OPTIONS ] [ RULE_FILE1 ... ]\n", "hfst-compose-intersect");
  fprintf(message_out,
      "Compute the interseting composition of a lexicon transducer\n"
       "and rule transducers.\n\n");
}

/* Display the version and legal stuff concerning this program. */
void version(void)
{
  fprintf(message_out,
	  "HFST-Compose-Intersect 0.1 (" PACKAGE_STRING ")\n"
	  "copyright (C) 2008 University of Helsinki\n"
	  "written by Miikka Silfverberg mar 2008\n"
	  "License GPLv3: GNU GPL version 3\n"
	  "\t<http://gnu.org/licences/gpl.html>\n"
"This is free software: you are free to change and redistribute it\n"
"There is NO WARRANTY, to the extent permitted by law.\n\n");
}

void help(void)
{
  usage();

  fprintf(message_out,
	  "  -h, " "--help\t\t\t" "Display this help message.\n"
	  "  -V, " "--version\t\t\t" "Display version.\n"
	  "  -v, " "--vebose\t\t\t" "Verbose mode.\n"
	  "  -q, " "--quiet\t\t\t" "Print nothing on screen.\n"
	  "  -s, " "--silent\t\t\t" "Print nothing on screen.\n"
	  "  -l, " "--lexicon=FILE\t\t" "Read lexicon from FILE.\n"
	  "  -o, " "--output=FILE\t\t" "Write result to FILE.\n"
	  "  -R, " "--read-symbols=FILE\t\t" "Read symbol table from FILE.\n"
	  "  -W, " "--write-symbols=FILE\t" "Write symbol table to FILE.\n\n"
	  "  -t, " "--time\t\t\t" "Time the operation.\n"
	  "\n"
"If the argument -l is omitted, the lexicon is read from STDIN. If\n"
"there are no RULE_FILE arguments given, the rules are read from\n"
"STDIN. If both the lexicon and the rules are read from STDIN, the\n"
"lexicon is the first transducer to be read and the rest of the\n"
"ransducers are rules.\n"
"\n"
"If the argument -o is omitted or the FILE argument is \"-\", the\n"
"result is written to STDOUT.\n"
"\n"
"If the argument, -W is omitted, the symbol table is written with\n"
"the result transducer (If you don't have a good reason to store\n"
"the symbol table in a separate file, it's probably best to store\n"
"it with the result transducer).\n"
"\n"
"More info at <https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstComposeIntersect>\n"
"\n"
"Report bugs to HFST team <hfst-bugs@helsinki.fi>\n");
}

void read_symbols(void)
{
  if (symbol_input_file_name != NULL)
    {
      message("Reading symbol table from %s.",
	      symbol_input_file_name);
      ifstream symbol_in(symbol_input_file_name);
      if (with_weights)
	{
	  weighted_symbol_table = 
	    HWFST::read_symbol_table(symbol_in);
	}
      else
	unweighted_symbol_table =
	  HFST::read_symbol_table(symbol_in);
      
    }
  else
    {
      message("Using the symbol table stored with the lexicon.");
      if (with_weights)
	weighted_symbol_table = HWFST::create_key_table();
      else
	unweighted_symbol_table = HFST::create_key_table();
    }
}

void read_lexicon(void) 
{

  if (lexicon_file_name != NULL)
    {
      if(strcmp(lexicon_file_name,"-") == 0)
	{
	  free(lexicon_file_name);
	  lexicon_file_name = NULL;
	}
    }

  if (lexicon_file_name != NULL) 
    {
      ifstream lexicon_in(lexicon_file_name,ios::in);
      int lexicon_format = HFST::read_format(lexicon_in);
      if (lexicon_format == UNWEIGHTED_F)
	{
	  if ( (not HFST::has_symbol_table(lexicon_in)) and
	       (symbol_input_file_name == NULL))
	    {
	      error("The lexicon has no symbol table and you haven't\n"
		    "provided one in a separate file.");
	    }
	}
      else
	{
	  if ( (not HWFST::has_symbol_table(lexicon_in)) and
	       (symbol_input_file_name == NULL))
	    {
	      error("The lexicon has no symbol table and you haven't\n"
		    "provided one in a separate file.");
	    }
	}
      if (lexicon_format == UNWEIGHTED_F)
	{
	  with_weights = false;
	  if (not silent)
	    {
    message("Unweighted lexicon (all rules have to be unweighted).");
	    }
	  if (symbol_input_file_name == NULL)
	    {
	      unweighted_lexicon =
		HFST::read_transducer(lexicon_in,unweighted_symbol_table);
	    }
	  else
	    {
	      unweighted_lexicon =
		HFST::read_transducer(lexicon_in);
	    }
	}
      else if (lexicon_format == WEIGHTED_F)
	{
	  with_weights = true;
	  if (not silent)
	    {
	message("Weighted lexicon (all rules have to be weighted),");
	    }
	  if (symbol_input_file_name == NULL)
	    {
	      weighted_lexicon =
		HWFST::read_transducer(lexicon_in,weighted_symbol_table);
	    }
	  else
	    {
	      weighted_lexicon =
		HWFST::read_transducer(lexicon_in);
	    }
	}
      else if (lexicon_format == EOF)
	{
	  error("The lexicon file %s is empty.",
		lexicon_file_name);
	}
      else if (lexicon_format == COMPACT_F)
	{
	  error("The lexicon transducer in %s is compact.",
		lexicon_file_name);
	}
      else
	{
	  error("The lexicon transducer in %s has unknown format.",
		lexicon_file_name);
	}
    }
  else
    {
      if (not silent)
	{
	  message("Reading lexicon from STDIN.");
	}

      int lexicon_format = HFST::read_format();
      if (lexicon_format == UNWEIGHTED_F)
	{
	  if ( (not HFST::has_symbol_table(cin)) and
	       (symbol_input_file_name == NULL))
	    {
	   error("The lexicon has no symbol table and you haven't\n"
		 "provided one in a separate file.");
	    }

	  with_weights = false;
	  if (not silent)
	    {
      message("Unweighted lexicon (all rules need to be unweighted)");
	    }
	  if (symbol_input_file_name == NULL)
	    {
	      unweighted_lexicon =
		HFST::read_transducer(cin,unweighted_symbol_table);
	    }
	  else
	    {
	      unweighted_lexicon =
		HFST::read_transducer(cin);
	    }
	}
      else if (lexicon_format == WEIGHTED_F)
	{
	  if ( (not HWFST::has_symbol_table(cin)) and
	       (symbol_input_file_name == NULL))
	    {
	  error("The lexicon has no symbol table and you haven't\n"
		"provided one in a separate file.");
	    }

	  with_weights = true;
	  if (not silent)
	    {
	  message("Weighted lexicon (all rules need to be weighted)");
	    }
	  if (symbol_input_file_name == NULL)
	    {
	      weighted_lexicon =
		HWFST::read_transducer(cin,weighted_symbol_table);
	    }
	  else
	    {
	      weighted_lexicon =
		HWFST::read_transducer(cin);
	    }
	}
      else if (lexicon_format == EOF)
	{
	  error("The lexicon file STDIN is empty.",
		lexicon_file_name);
	}
      else if (lexicon_format == COMPACT_F)
	{
	  error("The lexicon transducer in STDIN is compact.",
		lexicon_file_name);
	}
      else
	{
	  error("The lexicon transducer in STDIN has unknown format.",
		lexicon_file_name);
	}
    }
}

void read_rules(istream &in,
		const char * file_name) 
{
  if (HFST::read_format(in) == EOF)
    {
      error("Rule file %s is empty",file_name);
    }
  if (HFST::read_format(in) == COMPACT_F)
    {
      error("Rule file %s has compact transducers",file_name);
    }
  if (HFST::read_format(in) == UNKNOWN_F)
    {
      error("Rule file %s has unknown transducers",file_name);
    }

  size_t number_of_rules = 0;
  if (with_weights)
    {
      if (HFST::read_format(in) == UNWEIGHTED_F)
	{
	  error("Rule file %s has unweighted transducers.",file_name);
	}
      while(HFST::read_format(in) == WEIGHTED_F)
	{
	  WeightedFst rule = NULL;
	  ++number_of_rules;
	  if (HWFST::has_symbol_table(in))
	    {
	      HFST::KeyTable * rule_table = HFST::create_key_table();
	      rule = 
		HWFST::read_transducer(in,rule_table);
	      if (HFST::is_symbol("@#@"))
		{ 
		  try {
		  if (HFST::is_symbol(HFST::get_symbol("@#@"),
				      rule_table))
		    {
		      rules_contain_word_boundary = true;
		    }
		  }
		  catch (const char * p)
		    { std::cerr << p << std::endl; }
		  try {
		    if (not HFST::is_symbol(HFST::get_symbol("@#@"),
					    weighted_symbol_table))	  
		      { HFST::associate_key
			  (weighted_symbol_table->get_unused_key(),
			   weighted_symbol_table,HFST::get_symbol("@#@")); }
		  }
		  catch (const char * p)
		    { std::cerr << p << std::endl; }
		  }
		
	      rule = HWFST::harmonize_transducer(rule,rule_table,
						 weighted_symbol_table);
	    }
	  else
	    {
	      rule = HWFST::read_transducer(in);	      
	    }
	  weighted_rules.push_back(rule);
	  if (not silent)
	    {
	      fprintf(message_out,"Rule number %zu.\r",number_of_rules);
	    }
	}
    }
  else
    {
      if (HFST::read_format(in) == WEIGHTED_F)
	{
	  error("Rule file %s has weighted transducers.",file_name);
	}
      while(HFST::read_format(in) == UNWEIGHTED_F)
	{
	  UnWeightedFst rule = NULL;
	  ++number_of_rules;
	  if (HFST::has_symbol_table(in))
	    {
	      HFST::KeyTable * rule_table = HFST::create_key_table();
	      rule = 
		HFST::read_transducer(in,rule_table);
	      if (HFST::is_symbol("@#@"))
		{ 
		  if (HFST::is_symbol(HFST::get_symbol("@#@"),
				      rule_table))
		    {
		      rules_contain_word_boundary = true;
		    }
		  if (not HFST::is_symbol(HFST::get_symbol("@#@"),
					  unweighted_symbol_table))	  
		    { HFST::associate_key
			(unweighted_symbol_table->get_unused_key(),
			 unweighted_symbol_table,HFST::get_symbol("@#@")); }

		}
	      rule = HFST::harmonize_transducer(rule,rule_table,
						unweighted_symbol_table);
	    }
	  else
	    {
	      rule = HFST::read_transducer(in);	      
	    }
	  unweighted_rules.push_back(rule);
	  if (not silent)
	    {
	      fprintf(message_out,"Rule number %zu.\r",number_of_rules);
	    }
	}
    }
  if (not silent)
    {
      fprintf(message_out,"\n");
    }
}

void read_rules(void) 
{
  if (rule_file_names.empty())
    {
      message("Reading rules from STDIN.");
      read_rules(cin,"STDIN");
    }
  else 
    {
      for(vector<char*>::iterator it = rule_file_names.begin();
	  it != rule_file_names.end();
	  ++it)
	{
	  char * rule_file_name = *it;
	  message("Reading rules from %s.",rule_file_name);
	  ifstream rule_file_in(rule_file_name);
	  read_rules(rule_file_in,rule_file_name);
	  rule_file_in.close();
	}
    }

};

void set_with_weight(void)
{
  if (lexicon_file_name == NULL)
    {
      message("Expecting to find transducer in STDIN.");
      if (HFST::read_format() == WEIGHTED_F)
	with_weights = true;
      else
	with_weights = false;
    }
  else
    {
      ifstream test_stream(lexicon_file_name,ios::in);
      if (HFST::read_format(test_stream) == WEIGHTED_F)
	with_weights = true;
      else
	with_weights = false;
      test_stream.close();
    }
}

char * time_to_string(clock_t interval)
{
  float interval_in_ms =
    interval*1000.0/CLOCKS_PER_SEC;
  char * str = (char*)(malloc(100));
  str[0] = 0;
  sprintf(str,"%.2f",interval_in_ms);
  return str;
}

void parse_arguments(int argc, char * argv[])
{
  program_name = strdup(argv[0]);
  while (true) 
    {
      static const struct option long_options[] =
	{
	  {"help",no_argument,NULL,'h'},
	  {"version",no_argument,NULL,'V'},
	  {"verbose",no_argument,NULL,'v'},
	  {"quiet",no_argument,NULL,'q'},
	  {"silent",no_argument,NULL,'s'},
	  {"lexicon",required_argument,NULL,'l'},
	  {"output",required_argument,NULL,'o'},
	  {"read-symbols",required_argument,NULL,'R'},
	  {"write-symbols",required_argument,NULL,'W'},
	  {"time",no_argument,NULL,'t'},
	  {"no-minimize",no_argument,NULL,'X'},
	  {0,0,0,0}
	};
      int option_index = 0;
      int c = getopt_long(argc,argv,":htdVvqsl:o:R:W:X",
			  long_options, &option_index);
      if (c == EOF) 
	{
	  break;
	}
      switch (c)
	{
	case 'h':
	  display_help = true;
	  break;
	case 'd':
	  debug = true;
	  break;
	case 'V':
	  display_version = true;
	  break;
	case 'v':
	  verbose = true;
	  break;
	case 'q':
	  silent = true;
	  break;
	case 's':
	  silent = true;
	  break;
	case 'l':
	  lexicon_file_name = strdup(optarg);
	  break;
	case 'o':
	  output_file_name = strdup(optarg);
	  break;
	case 'R':
	  symbol_input_file_name = strdup(optarg);
	  break;
	case 'W':
	  store_symbols_with_result = false;
	  symbol_output_file_name = strdup(optarg);
	  break;
	case 't':
	  time_operation = true;
	  break;
	case 'X':
	  no_minimize = true;
	  break;
	case '?':
	  error("Option is unknown.");
	  break;
	case ':':
	  error("The option %s is missing argument\n",
		long_options[option_index].name);
	}
    }

  if (NULL == output_file_name)
  {
	  message_out = stderr;
  }
  rules_start_index = optind; 
  rules_end_index = argc;

  for (vector<char*>::size_type i = rules_start_index;
       i < rules_end_index;
       ++i) {
    rule_file_names.push_back(argv[i]);
  }

  if (display_help)
    {
      help();
      exit(0);
    }
  if (display_version)
    {
      version();
      exit(0);
    }


  set_with_weight();

  read_symbols();
  message("Symbol table read.");

  time_t LEXICON_READ_START = clock();
  read_lexicon();
  time_t LEXICON_READ_STOP = clock();
  char * lexicon_read_time = 
    time_to_string(LEXICON_READ_STOP-LEXICON_READ_START);
  if (time_operation)
    {
      message("Lexicon read in %s ms.",lexicon_read_time);
    }
  
  time_t RULES_READ_START = clock();
  read_rules();
  time_t RULES_READ_STOP = clock();
  char * rules_read_time = 
    time_to_string(RULES_READ_STOP-RULES_READ_START);
  if (time_operation)
    {
      message("Rules read in %s ms.",rules_read_time);
    }

}

void store_symbols(void)
{
  if (store_symbols_with_result)
    {
      return;
    }
  else
    {
      ofstream symbol_out(symbol_output_file_name,
			  ios::out);
      if (with_weights) 
	{
	  HWFST::write_symbol_table(weighted_symbol_table,
				    symbol_out);
	}
      else
	{
	  HFST::write_symbol_table(unweighted_symbol_table,
				   symbol_out);
	}
    }
}

void store_result(void)
{
  time_t STORE_RESULT_START = clock();
  if (output_file_name == NULL)
    {
      if (with_weights) 
	{
	  if (store_symbols_with_result)
	    {
	      HWFST::write_transducer(weighted_result,
				      weighted_symbol_table,
				      cout);
	    }
	  else
	    {
	      HWFST::write_transducer(weighted_result,
				      cout);
	    }
	}
      else
	{
	  if (store_symbols_with_result)
	    {
	      HFST::write_transducer(unweighted_result,
				     unweighted_symbol_table,
				      cout);
	    }
	  else
	    {
	      HFST::write_transducer(unweighted_result,
				      cout);
	    }

	}
    }
  else
    {
      if (with_weights) 
	{
	  if (store_symbols_with_result)
	    {
	      HWFST::write_transducer(weighted_result,
				      output_file_name,
				      weighted_symbol_table);
	    }
	  else
	    {
	      HWFST::write_transducer(weighted_result,
				      output_file_name);
	    }
	}
      else
	{
	  if (store_symbols_with_result)
	    {
	      HFST::write_transducer(unweighted_result,
				     output_file_name,
				     unweighted_symbol_table);
	    }
	  else
	    {
	      HFST::write_transducer(unweighted_result,
				     output_file_name);
	    }

	}
    }
  time_t STORE_RESULT_STOP = clock();
  char * store_time = 
    time_to_string(STORE_RESULT_STOP-STORE_RESULT_START);
  if (time_operation)
    {
      message("Stored result in %s ms.",
	      store_time);
    }


}


void compute_result(void)
{
  message("Computing intersecting composition.\n");
  if (with_weights)
    {
      /*HWFST::TransducerHandle lexicon_output_language =
      	HWFST::extract_output_language(HWFST::copy(weighted_lexicon));
      HWFST::TransducerHandle rule_input_language =
	HWFST::extract_input_language(HWFST::copy(weighted_rules[0]));
      KeySet *lexicon_ks =
      	HWFST::define_key_set(lexicon_output_language);
      KeySet *rule_ks =
	HWFST::define_key_set(rule_input_language);
      HWFST::delete_transducer(lexicon_output_language);
      HWFST::delete_transducer(rule_input_language);

      for (KeySet::iterator it=lexicon_ks->begin(); it!=lexicon_ks->end(); it++) {
	if (rule_ks->find(*it) == rule_ks->end()) {
	  warn("WARNING! The output symbol \"%s\" in the lexicon is not an "
	       "output symbol in the rules. The form will be filtered out!",
	       HWFST::get_symbol_name( HWFST::get_key_symbol(*it, weighted_symbol_table) ) );
	}
	}*/
    
      time_t INTERSECTING_COMPOSITION_START = clock();
      // Check whether word-boundar @#@ has been mentioned in the rules.
      if (rules_contain_word_boundary)
	{
	  HWFST::KeyPair * word_boundary_pair;
	  try {
	    word_boundary_pair =
	      HWFST::define_keypair(HWFST::Epsilon,
				    HWFST::get_key(HWFST::get_symbol("@#@"),
						   weighted_symbol_table));
	  }
	  catch (const char * p)
	    {
	      std::cerr << p << std::endl;
	      exit(1);
	    }
	  weighted_lexicon =
	    HWFST::concatenate
	    (HWFST::define_transducer(word_boundary_pair),
	     HWFST::concatenate
	     (weighted_lexicon,
	      HWFST::define_transducer(word_boundary_pair)));
	  weighted_lexicon = HWFST::minimize(weighted_lexicon);
	  delete word_boundary_pair;
	}
      if (HFST::is_symbol("@?@") and (HFST::is_symbol(HFST::get_symbol("@?@"),
						      weighted_symbol_table)))
	{
	  try {
	  HFST::KeyPair 
	    unknown_keypair(HFST::get_key(HFST::get_symbol("@?@"),
					  weighted_symbol_table));
	  HFST::KeySet * lexicon_keys = 
	    HWFST::define_key_set(HWFST::extract_output_language
				 (HWFST::copy(weighted_lexicon)));
	  HFST::KeySet * rule_keys =
	    HWFST::define_key_set(HWFST::extract_input_language
				 (HWFST::copy(weighted_rules.at(0))));
	  rule_keys->insert(0);
	  for (HFST::KeySet::iterator it = rule_keys->begin();
	       it != rule_keys->end();
	       ++it)
	    { lexicon_keys->erase(*it); }
	  lexicon_keys->insert(HFST::get_key(HFST::get_symbol("@?@"),
					     weighted_symbol_table));
	  delete rule_keys;
	  HWFST::TransducerHandle unknown_transducer =
	    HWFST::define_transducer(lexicon_keys);
	  for (vector<HWFST::TransducerHandle>::iterator it = 
		 weighted_rules.begin();
	       it != weighted_rules.end();
	       ++it)
	    {
	      *it = HWFST::substitute_with_transducer
		(*it,&unknown_keypair,HWFST::copy(unknown_transducer));
	      *it = HWFST::minimize(*it);
	    }
	  delete unknown_transducer;
	  }
	  catch (const char * p)
	    { std::cerr << p << std::endl; exit(1); }
	}

      HWFST::TransducerHandle weighted_lexicon_phonology_level =
	HWFST::minimize
	(HWFST::extract_output_language(HWFST::copy(weighted_lexicon)));

      weighted_result =
	HWFST::intersecting_composition(weighted_lexicon_phonology_level,
					&weighted_rules,
					weighted_symbol_table);

      weighted_result = HWFST::minimize(weighted_result);
      
      weighted_result = 
	HWFST::compose(weighted_lexicon,weighted_result);

      time_t INTERSECTING_COMPOSITION_STOP = clock();
      char * intersecting_composition_time = 
 time_to_string(INTERSECTING_COMPOSITION_STOP-INTERSECTING_COMPOSITION_START);
      if (time_operation)
	{
	  message("Intersecting composition done in %s ms.",
		  intersecting_composition_time);
	}

	if (!no_minimize)
	{
		message("Minimizing.\n");
		time_t MINIMIZE_START = clock_t();
		weighted_result =
			HWFST::minimize(weighted_result);
		time_t MINIMIZE_STOP = clock_t();
		char * minimize_time = 
		time_to_string(MINIMIZE_STOP-MINIMIZE_START);
		if (time_operation)
		{
			message("Minimized result in %s ms.",
				    minimize_time);
		}
	}
    }
  else
    {
      time_t INTERSECTING_COMPOSITION_START = clock();
      if (rules_contain_word_boundary)
	{
	  HFST::KeyPair * word_boundary_pair =
	    HFST::define_keypair(HFST::Epsilon,
				 HFST::get_key(HFST::get_symbol("@#@"),
					       unweighted_symbol_table));
	  unweighted_lexicon =
	    HFST::concatenate
	    (HFST::define_transducer(word_boundary_pair),
	     HFST::concatenate
	     (unweighted_lexicon,
	      HFST::define_transducer(word_boundary_pair)));
	  unweighted_lexicon = HFST::minimize(unweighted_lexicon);
	  delete word_boundary_pair;
	}
      // Handle unknown lexicon symbols.
      if (HFST::is_symbol("@?@") and (HFST::is_symbol(HFST::get_symbol("@?@"),
						      unweighted_symbol_table)))
	{
	  HFST::KeyPair 
	    unknown_keypair(HFST::get_key(HFST::get_symbol("@?@"),
					  unweighted_symbol_table));
	  HFST::KeySet * lexicon_keys = 
	    HFST::define_key_set(HFST::extract_output_language
				 (HFST::copy(unweighted_lexicon)));
	  HFST::KeySet * rule_keys =
	    HFST::define_key_set(HFST::extract_input_language
				 (HFST::copy(unweighted_rules.at(0))));
	  rule_keys->insert(0);
	  for (HFST::KeySet::iterator it = rule_keys->begin();
	       it != rule_keys->end();
	       ++it)
	    { lexicon_keys->erase(*it); }
	  lexicon_keys->insert(HFST::get_key(HFST::get_symbol("@?@"),
					     unweighted_symbol_table));
	  delete rule_keys;
	  HFST::TransducerHandle unknown_transducer =
	    HFST::define_transducer(lexicon_keys);
	  for (vector<HFST::TransducerHandle>::iterator it = 
		 unweighted_rules.begin();
	       it != unweighted_rules.end();
	       ++it)
	    {
	      *it = HFST::substitute_with_transducer
		(*it,&unknown_keypair,HFST::copy(unknown_transducer));
	      *it = HFST::minimize(*it);
	    }
	  delete unknown_transducer;
	}

      HFST::TransducerHandle unweighted_lexicon_phonology_level =
	HFST::minimize
	(HFST::extract_output_language(HFST::copy(unweighted_lexicon)));

      unweighted_result =
	HFST::intersecting_composition(unweighted_lexicon_phonology_level,
				       &unweighted_rules,
				       unweighted_symbol_table);

      unweighted_result = HFST::minimize(unweighted_result);
      
      unweighted_result = 
	HFST::compose(unweighted_lexicon,unweighted_result);

      /*      unweighted_result =
	HFST::intersecting_composition(unweighted_lexicon,
				       &unweighted_rules,
				       unweighted_symbol_table);
      */
      time_t INTERSECTING_COMPOSITION_STOP = clock();
      char * intersecting_composition_time = 
 time_to_string(INTERSECTING_COMPOSITION_STOP-INTERSECTING_COMPOSITION_START);
      if (time_operation)
	{
	  message("Intersecting composition done in %s ms.",
		  intersecting_composition_time);
	}
	if (!no_minimize)
	{
      message("Minimizing.\n");
      time_t MINIMIZE_START = clock();
      unweighted_result =
	HFST::minimize(unweighted_result);
      time_t MINIMIZE_STOP = clock();
      char * minimize_time = 
	time_to_string(MINIMIZE_STOP-MINIMIZE_START);
      if (time_operation)
	{
	  message("Minimized result in %s ms.",
		  minimize_time);
	}
	}
    }
  
}

int main(int argc, char * argv[]) {
  parse_arguments(argc,argv);
  compute_result();
  store_symbols();
  store_result();
}
