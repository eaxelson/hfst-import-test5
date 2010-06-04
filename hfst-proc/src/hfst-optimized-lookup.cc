/*
  
  Copyright 2009 University of Helsinki
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  
*/

/*
  NOTE:
  THIS SINGLE-FILE VERSION WAS PUT TOGETHER FROM A MULTI-FILE VERSION
  SO THE CURRENT STRUCTURE IS NOT SO GREAT. TODO: FIX THIS.
  TODO: USE THE EXISTING HFST-TOOLS FRAMEWORK BETTER.
 */

#include "hfst-optimized-lookup.h"
#include "hfst-proc-extra.h"


OutputType outputType = xerox;

bool verboseFlag = false;

bool displayWeightsFlag = false;
bool displayUniqueFlag = false;
bool echoInputsFlag = false;
bool beFast = false;
int maxAnalyses = INT_MAX;
bool preserveDiacriticRepresentationsFlag = false;

bool timingFlag = false;
bool printDebuggingInformationFlag = false;

bool do_proc_flag = false;
bool do_tokenize_flag = false;


int setup(std::ifstream& is);

bool print_usage(void)
{
  std::cerr <<
    "\n" <<
    "Usage: " << PACKAGE_NAME << " [OPTIONS] TRANSDUCER\n" <<
    "Run a transducer on standard input (one word per line) and print analyses\n" <<
    "\n" <<
    "  -h, --help                  Print this help message\n" <<
    "  -V, --version               Print version information\n" <<
    "  -v, --verbose               Be verbose\n" <<
    "  -q, --quiet                 Don't be verbose (default)\n" <<
    "  -s, --silent                Same as quiet\n" <<
    "  -e, --echo                  Echo inputs\n" <<
    "                              (useful if redirecting lots of output to a file)\n" <<
    "  -w, --show-weights          Print final analysis weights (if any)\n" <<
    "  -u, --unique                Suppress duplicate analyses\n" <<
    "  -n N, --analyses=N          Output no more than N analyses\n" <<
    "                              (if the transducer is weighted, the N best analyses)\n" <<
    "  -x, --xerox                 Xerox output format (default)\n" <<
    "  -f, --fast                  Be as fast as possible.\n" <<
    "                              (with this option enabled -u and -n don't work and\n" <<
    "                              output won't be ordered by weight).\n" <<
    "  -p, --proc                  Operate in hfst-proc mode (experimental)\n" <<
    "  -t  --tokenize              Tokenize the input stream (for debugging)\n" <<
    "\n" <<
    "Report bugs to " << PACKAGE_BUGREPORT << "\n" <<
    "\n";
  return true;
}

bool print_version(void)
{
  std::cerr <<
    "\n" <<
    PACKAGE_STRING << std::endl <<
    __DATE__ << " " __TIME__ << std::endl <<
    "copyright (C) 2009 University of Helsinki\n";
  return true;
}

bool print_short_help(void)
{
  print_usage();
  return true;
}

int main(int argc, char **argv)
{
  int c;
  
  while (true)
    {
      static struct option long_options[] =
	{
	  // first the hfst-mandated options
	  {"help",         no_argument,       0, 'h'},
	  {"version",      no_argument,       0, 'V'},
	  {"verbose",      no_argument,       0, 'v'},
	  {"quiet",        no_argument,       0, 'q'},
	  {"silent",       no_argument,       0, 's'},
	  // the hfst-optimized-lookup-specific options
	  {"echo-inputs",  no_argument,       0, 'e'},
	  {"show-weights", no_argument,       0, 'w'},
	  {"unique",       no_argument,       0, 'u'},
	  {"xerox",        no_argument,       0, 'x'},
	  {"fast",         no_argument,       0, 'f'},
	  {"analyses",     required_argument, 0, 'n'},
	  {"proc",         no_argument,       0, 'p'},
	  {"tokenize",     no_argument,       0, 't'},
	  {0,              0,                 0,  0 }
	};
      
      int option_index = 0;
      c = getopt_long(argc, argv, "hVvqsewuxfptn:", long_options, &option_index);

      if (c == -1) // no more options to look at
	break;

      switch (c)
	{
	case 'h':
	  print_usage();
	  return EXIT_SUCCESS;
	  break;
	  
	case 'V':
	  print_version();
	  return EXIT_SUCCESS;
	  break;
	  
	case 'v':
#ifdef DEBUG
	  printDebuggingInformationFlag = true;
	  preserveDiacriticRepresentationsFlag = true;
#endif
	  
#ifdef TIMING
	  timingFlag = true;
#endif
	  verboseFlag = true;
	  break;
	  
	case 'q':
	case 's':
#ifdef DEBUG
	  printDebuggingInformationFlag = false;
	  preserveDiacriticRepresentationsFlag = false;
#endif
	  
#ifdef TIMING
	  timingFlag = false;
#endif
	  verboseFlag = false;
	  displayWeightsFlag = true;
	  break;

	case 'e':
	  echoInputsFlag = true;
	  
	case 'w':
	  displayWeightsFlag = true;
	  break;

	case 'u':
	  displayUniqueFlag = true;
	  break;
	  
	case 'n':
	  maxAnalyses = atoi(optarg);
	  if (maxAnalyses < 1)
	    {
	      std::cerr << "Invalid or no argument for analyses count\n";
	      return EXIT_FAILURE;
	    }
	  break;

	case 'x':
	  outputType = xerox;
	  break;

	case 'f':
	  beFast = true;
	  break;
	
	case 'p':
	  do_proc_flag = true;
	  break;
	
	case 't':
	  do_tokenize_flag = true;
	  break;
	  
	default:
	  std::cerr << "Invalid option\n\n";
	  print_short_help();
	  return EXIT_FAILURE;
	  break;
	}
    }
  // no more options, we should now be at the input filename
  if ( (optind + 1) < argc)
  {
    std::cerr << "More than one input file given\n";
    return EXIT_FAILURE;
  }
  else if ( (optind + 1) == argc)
  {
    std::ifstream is(argv[(optind)], std::ios::in | std::ios::binary);
    if(!is)
    {
      std::cerr << "Could not open file " << argv[(optind)] << std::endl;
      return 1;
    }
    
    return setup(is);
  }
  else
  {
    std::cerr << "No input file given\n";
    return EXIT_FAILURE;
  }
}

void
TransducerAlphabet::setup_blank_symbol()
{
  blank_symbol = NO_SYMBOL_NUMBER;
  for(SymbolTable::const_iterator it=symbol_table.begin(); it!=symbol_table.end(); it++)
  {
    if(it->second == " ")
    {
      blank_symbol = it->first;
      break;
    }
  }
  
  if(blank_symbol == NO_SYMBOL_NUMBER)
    blank_symbol = add_symbol(" ");
}

void TransducerAlphabet::get_next_symbol(std::istream& is, SymbolNumber k)
{
  std::string str;
  std::getline(is, str, '\0');
  if(printDebuggingInformationFlag)
    std::cout << "Got next symbol: '" << str << "' (" << k << ")" << std::endl;
  
  if(!is || str.length()==0) 
  {
    std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
    exit(1);
  }

  if (str.length() >= 5 && str.at(0) == '@' && str.at(str.length()-1) == '@' && str.at(2) == '.')
  { // a flag diacritic needs to be parsed
    std::string feat;
    std::string val;
    FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
    switch (str.at(1)) {
    case 'P': op = P; break;
    case 'N': op = N; break;
    case 'R': op = R; break;
    case 'D': op = D; break;
    case 'C': op = C; break;
    case 'U': op = U; break;
    }
    const char* cstr = str.c_str();
    const char * c = cstr;
    // as long as we're working with utf-8, this should be ok
    for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
    if (*c == '.')
    {
      for (++c; *c != '@'; c++) { val.append(c,1); }
    }
    if (feature_bucket.count(feat) == 0)
    {
      feature_bucket[feat] = feat_num;
      ++feat_num;
    }
    if (value_bucket.count(val) == 0)
    {
      value_bucket[val] = val_num;
      ++val_num;
    }
    add_symbol(k, "",
               FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]));
    
#if OL_FULL_DEBUG
    std::cout << "symbol number " << k << " (flag) is " << str << std::endl;
    symbol_table[k] = str;
#endif
    
    return;
  }

#if OL_FULL_DEBUG
  std::cout << "symbol number " << k << " is " << str << std::endl;
#endif
  add_symbol(k, str);
}

std::string
TransducerAlphabet::symbols_to_string(const SymbolNumberVector& symbols) const
{
  std::string str="";
  for(SymbolNumberVector::const_iterator it=symbols.begin(); it!=symbols.end(); it++)
    str += symbol_to_string(*it);
  return str;
}

bool
TransducerAlphabet::is_punctuation(const char* c) const
{
  static const char* punct_ranges[6][2] = {{"!","/"},
                                           {":","@"},
                                           {"[","`"},
                                           {"{","~"},
                                           {"¡","¿"},
                                           {"‐","⁞"}};
  for(int i=0;i<6;i++)
  {
    if(strcmp(c,punct_ranges[i][0]) >= 0 && 
       strcmp(c,punct_ranges[i][1]) <= 0)
    {
      // a hack to filter out symbols (e.g. tags) that may start with punctuation
      // and then contain ASCII text. Tags should be treated as alphabetic.
      for(;*c!='\0';c++)
      {
        if(isalnum(*c))
          return false;
      }
      return true;
    }
  }
  return false;
}

void
TransducerAlphabet::calculate_alphabetic()
{
  for(SymbolTable::const_iterator it=symbol_table.begin(); it!=symbol_table.end(); it++)
    if(is_alphabetic(it->second.c_str()))
      alphabetic.insert(it->first);
}


void LetterTrie::add_string(const char * p, SymbolNumber symbol_key)
{
  if (*(p+1) == 0)
  {
    symbols[(unsigned char)(*p)] = symbol_key;
    return;
  }

  if (letters[(unsigned char)(*p)] == NULL)
    letters[(unsigned char)(*p)] = new LetterTrie();
  letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber LetterTrie::find_key(char ** p) const
{
  const char * old_p = *p;
  ++(*p);
  if (letters[(unsigned char)(*old_p)] == NULL)
    return symbols[(unsigned char)(*old_p)];
  
  SymbolNumber s = letters[(unsigned char)(*old_p)]->find_key(p);
  if (s == NO_SYMBOL_NUMBER)
  {
    --(*p);
    return symbols[(unsigned char)(*old_p)];
  }
  return s;
}

SymbolNumber
LetterTrie::extract_symbol(std::istream& is) const
{
  int c = is.get();
  if(c == EOF)
    return 0;
    
  if(letters[c] == NULL)
  {
    if(symbols[c] == NO_SYMBOL_NUMBER)
      is.putback(c);
    return symbols[c];
  }
  
  SymbolNumber s = letters[c]->extract_symbol(is);
  if(s == NO_SYMBOL_NUMBER)
  {
    if(symbols[c] == NO_SYMBOL_NUMBER)
      is.putback(c);
    return symbols[c];
  }
  return s;
}

void Encoder::read_input_symbols(const SymbolTable& st)
{
  for (SymbolNumber k = 0; k < number_of_input_symbols; ++k)
  {
#if DEBUG
    assert(st.find(k) != st.end());
#endif
    std::string p = st.find(k)->second;
    
    if(p.length() > 0)
    {
      char first = p.at(0);
      if(ascii_symbols[first] != 0) 
      { // if the symbol's first character is ASCII and we're not ignoring it yet
        if(p.length() == 1)
          ascii_symbols[first] = k;
        else
          ascii_symbols[first] = 0;
      }
    }
    
    letters.add_string(p.c_str(),k);
  }
}

SymbolNumber Encoder::find_key(char ** p) const
{
  if (ascii_symbols[(unsigned char)(**p)] == NO_SYMBOL_NUMBER)
    return letters.find_key(p);

  SymbolNumber s = ascii_symbols[(unsigned char)(**p)];
  ++(*p);
  return s;
}

SymbolNumber
Encoder::extract_symbol(std::istream& is) const
{
  int c = is.peek();
  if(ascii_symbols[c] == NO_SYMBOL_NUMBER)
    return letters.extract_symbol(is);
  
  return ascii_symbols[is.get()];
}


void runTransducer(AbstractTransducer& T)
{
  SymbolNumber * input_string = (SymbolNumber*)(malloc(2000));
  for (int i = 0; i < 1000; ++i)
    input_string[i] = NO_SYMBOL_NUMBER;
  
  char * str = (char*)(malloc(MAX_IO_STRING*sizeof(char)));  
  *str = 0;
  char * old_str = str;

  while(std::cin.getline(str,MAX_IO_STRING))
  {
    if (echoInputsFlag)
      std::cout << str << std::endl;
    
    int i = 0;
    SymbolNumber k = NO_SYMBOL_NUMBER;
    bool failed = false;
    for ( char ** Str = &str; **Str != 0; )
    {
      k = T.get_alphabet().find_key(Str);
#if OL_FULL_DEBUG
      std::cout << "INPUT STRING ENTRY " << i << " IS " << k << std::endl;
#endif
      if (k == NO_SYMBOL_NUMBER)
      {
        if (echoInputsFlag)
      	  std::cout << std::endl;
    	
        failed = true;
        break;
      }
      input_string[i] = k;
      ++i;
    }
    
    str = old_str;
    if (failed)
    { // tokenization failed
      if (outputType == xerox)
        std::cout << str << "\t+?" << std::endl << std::endl;
        
      continue;
    }
    input_string[i] = NO_SYMBOL_NUMBER;
    T.analyze(input_string);
    T.printAnalyses(std::string(str));
    
    T.analyze_iteratively(input_string);
    T.printAnalyses(std::string(str));
  }
}

AbstractTransducer* load_transducer(std::istream& is)
{
  AbstractTransducer* t = NULL;
  
  TransducerHeader header(is);
  if(printDebuggingInformationFlag)
    header.print();
  TransducerAlphabet alphabet(is, header.symbol_count());

  if (header.probe_flag(Has_unweighted_input_epsilon_cycles) ||
      header.probe_flag(Has_input_epsilon_cycles))
    {
      std::cerr << "!! Warning: transducer has epsilon cycles                  !!\n"
                << "!! This is currently not handled - if they are encountered !!\n"
                << "!! program *will* segfault.                                !!\n";
    }
  
  if (alphabet.get_state_size() == 0)
  {      // if the state size is zero, there are no flag diacritics to handle
    if (header.probe_flag(Weighted) == false)
    {
      if (displayUniqueFlag)  // no flags, no weights, unique analyses only
        t = new TransducerUniq(is, header, alphabet); 
      else // no flags, no weights, all analyses
	      t = new Transducer(is, header, alphabet);
    }
    else
    {
      if (displayUniqueFlag) // no flags, weights, unique analyses only
        t = new TransducerWUniq(is, header, alphabet);
      else // no flags, weights, all analyses
        t = new TransducerW(is, header, alphabet);
    }
  }
  else // handle flag diacritics
  {
    if (header.probe_flag(Weighted) == false)
    {
      if (displayUniqueFlag) // flags, no weights, unique analyses only
        t = new TransducerFdUniq(is, header, alphabet);
      else // flags, no weights, all analyses
        t = new TransducerFd(is, header, alphabet);
    }
    else
    {
      if (displayUniqueFlag) // flags, weights, unique analyses only
        t = new TransducerWFdUniq(is, header, alphabet);
      else // flags, weights, all analyses
        t = new TransducerWFd(is, header, alphabet);
    }
  }
  
  return t;
}

int setup(std::ifstream& is)
{
  AbstractTransducer* t = load_transducer(is);
  is.close();
  
  if(do_tokenize_flag)
  {
    TokenIOStream token_stream(std::cin, std::cout, t->get_alphabet());
    t->tokenize(token_stream);
  }
  else if(do_proc_flag)
  {
    TokenIOStream token_stream(std::cin, std::cout, t->get_alphabet());
    t->run_lookup(token_stream);
  }
  else
    runTransducer(*t);
  
  delete t;
  
  return 0;
}

/**
 * BEGIN old transducer.cc
 */

LookupPath* Transducer::get_initial_path() const
{
  return new LookupPath(START_INDEX);
}
LookupPath* TransducerFd::get_initial_path() const 
{
  return new LookupPathFd(START_INDEX, alphabet.get_state_size(), operations);
}
LookupPath* TransducerW::get_initial_path() const
{
  return new LookupPathW(START_INDEX);
}
LookupPath* TransducerWFd::get_initial_path() const
{
  return new LookupPathWFd(START_INDEX, alphabet.get_state_size(), operations);
}

void
AbstractTransducer::analyze_iteratively(SymbolNumber* input_string)
{
  LookupState lookup(*this);
  
  if(printDebuggingInformationFlag)
    std::cout << "Num paths after init: " << lookup.num_active() << std::endl;
  
  while(*input_string != NO_SYMBOL_NUMBER && lookup.is_active())
  {
    if(printDebuggingInformationFlag)
      std::cout << "Num paths before stepping: " << lookup.num_active() << std::endl;
    lookup.step(*input_string);
    input_string++;
    if(printDebuggingInformationFlag)
      std::cout << "Num paths after stepping: " << lookup.num_active() << std::endl;
  }
  
  const LookupPathVector finals = lookup.get_finals();
  
  if(printDebuggingInformationFlag)
    std::cout << "Done stepping. Num finals: " << finals.size() << std::endl;
  
  LookupPathVector analyses = lookup.get_finals();
  for(LookupPathVector::const_iterator it=analyses.begin(); 
        it!=analyses.end(); it++)
  {
    note_analysis(*(*it));
  }
}

bool TransducerFd::PushState(FlagDiacriticOperation op)
{ // try to alter the flag diacritic state stack
  switch (op.Operation()) {
  case P: // positive set
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = op.Value();
    return true;
  case N: // negative set (literally, in this implementation)
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = -1*op.Value();
    return true;
  case R: // require
    if (op.Value() == 0) // empty require
      {
	if (statestack.back()[op.Feature()] == 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
        if (op.Value() == 0) // empty disallow
      {
	if (statestack.back()[op.Feature()] != 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
      {
	return false;
      }
    statestack.push_back(statestack.back());
    return true;
  case C: // clear
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = 0;
    return true;
  case U: // unification
    if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
	statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
	(statestack.back()[op.Feature()] < 0 &&
	 (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
	)
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool TransitionIndex::matches(SymbolNumber s) const
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}

bool Transition::matches(SymbolNumber s) const
{
  
  if (input_symbol == NO_SYMBOL_NUMBER)
    {
      return false;
    }
  if (s == NO_SYMBOL_NUMBER)
    {
      return true;
    }
  return input_symbol == s;
}

void Transducer::try_epsilon_transitions(SymbolNumber * input_symbol,
					 SymbolNumber * output_symbol,
					 SymbolNumber * original_output_string,
					 TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_transitions " << i << std::endl;
#endif
  while (get_transition(i).get_input() == 0)
    {
      *output_symbol = get_transition(i).get_output();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   get_transition(i).target());
      ++i;
    }
}

void TransducerFd::try_epsilon_transitions(SymbolNumber * input_symbol,
					 SymbolNumber * output_symbol,
					 SymbolNumber * original_output_string,
					 TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_transitions " << i << std::endl;
#endif
  
  while (true)
    {
    if (get_transition(i).get_input() == 0) // epsilon
	{
	  *output_symbol = get_transition(i).get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       get_transition(i).target());
	  ++i;
	} else if (get_transition(i).get_input() != NO_SYMBOL_NUMBER &&
		   operations[get_transition(i).get_input()].isFlag())
	{
	  if (PushState(operations[get_transition(i).get_input()]))
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		alphabet.symbol_to_string(get_transition(i).get_input()) << " allowed\n";
#endif
	      // flag diacritic allowed
	      *output_symbol = get_transition(i).get_output();
	      get_analyses(input_symbol,
			   output_symbol+1,
			   original_output_string,
			   get_transition(i).target());
	      statestack.pop_back();
	    }
	  else
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		alphabet.symbol_to_string(get_transition(i).get_input()) << " disallowed\n";
#endif
	    }
	  ++i;
	} else
	{
	  return;
	}
    }
}

void Transducer::try_epsilon_indices(SymbolNumber * input_symbol,
				     SymbolNumber * output_symbol,
				     SymbolNumber * original_output_string,
				     TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "try_epsilon_indices " << i << std::endl;
#endif
  if (get_index(i).get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      get_index(i).target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::find_transitions(SymbolNumber input,
				    SymbolNumber * input_symbol,
				    SymbolNumber * output_symbol,
				    SymbolNumber * original_output_string,
				    TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "find_transitions " << i << "\t" << get_transition(i).get_input() << std::endl;
#endif

  while (get_transition(i).get_input() != NO_SYMBOL_NUMBER)
    {
      if (get_transition(i).get_input() == input)
	{
	  
	  *output_symbol = get_transition(i).get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       get_transition(i).target());
	}
      else
	{
	  return;
	}
      ++i;
    }
}

void Transducer::find_index(SymbolNumber input,
			    SymbolNumber * input_symbol,
			    SymbolNumber * output_symbol,
			    SymbolNumber * original_output_string,
			    TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "find_index " << i << "\t" << get_index(i+input).get_input() << std::endl;
#endif
  if (get_index(i+input).get_input() == input)
    {
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       get_index(i+input).target() - 
		       TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::note_analysis(SymbolNumber * whole_output_string)
{
  if (beFast)
    {
      for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
	{
	  std::cout << alphabet.symbol_to_string(*num);
	}
      std::cout << std::endl;
    } else
    {
      std::string str = "";
      for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
      {
        str.append(alphabet.symbol_to_string(*num));
      }
      display_vector.push_back(str);
    }
}
void Transducer::note_analysis(const LookupPath& path)
{
  display_vector.push_back(alphabet.symbols_to_string(path.get_output_symbols()));
}

void TransducerUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(alphabet.symbol_to_string(*num));
    }
  display_vector.insert(str);
}
void TransducerUniq::note_analysis(const LookupPath& path)
{
  display_vector.insert(alphabet.symbols_to_string(path.get_output_symbols()));
}

void TransducerFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(alphabet.symbol_to_string(*num));
    }
  display_vector.insert(str);
}
void TransducerFdUniq::note_analysis(const LookupPath& path)
{
  display_vector.insert(alphabet.symbols_to_string(path.get_output_symbols()));
}

void Transducer::get_analyses(SymbolNumber * input_symbol,
			      SymbolNumber * output_symbol,
			      SymbolNumber * original_output_string,
			      TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cout << "get_analyses " << i << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i -= TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);

#if OL_FULL_DEBUG
      std::cout << "Testing input string on transition side, " << *input_symbol << " at pointer" << std::endl;
#endif

      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (get_transition(i).final())
	    {
	      note_analysis(original_output_string);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       i+1);
    }
  else
    {
      
      try_epsilon_indices(input_symbol,
			  output_symbol,
			  original_output_string,
			  i+1);
      
#if OL_FULL_DEBUG
      std::cout << "Testing input string on index side, " << *input_symbol << " at pointer" << std::endl;
#endif
      
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{ // input-string ended.
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (get_index(i).final())
	    {
	      note_analysis(original_output_string);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;

      find_index(input,
		 input_symbol,
		 output_symbol,
		 original_output_string,
		 i+1);
    }
  *output_symbol = NO_SYMBOL_NUMBER;
}

void Transducer::printAnalyses(std::string prepend)
{
  if (!beFast)
    {
      if (outputType == xerox && display_vector.size() == 0)
	{
	  std::cout << prepend << "\t+?" << std::endl;
	  std::cout << std::endl;
	  return;
	}
      int i = 0;
      DisplayVector::iterator it = display_vector.begin();
      while ( (it != display_vector.end()) && i < maxAnalyses )
	{
	  if (outputType == xerox)
	    {
	      std::cout << prepend << "\t";
	    }
	  std::cout << *it << std::endl;
	  ++it;
	  ++i;
	}
      display_vector.clear(); // purge the display vector
      std::cout << std::endl;
    }
}

void TransducerUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_vector.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
  display_vector.clear(); // purge the display set
  std::cout << std::endl;
}

void TransducerFdUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_vector.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
  display_vector.clear(); // purge the display set
  std::cout << std::endl;
}

/**
 * BEGIN old transducer-weighted.cc
 */

bool TransducerWFd::PushState(FlagDiacriticOperation op)
{
  switch (op.Operation()) {
  case P: // positive set
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = op.Value();
    return true;
  case N: // negative set (literally, in this implementation)
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = -1*op.Value();
    return true;
  case R: // require
    if (op.Value() == 0) // empty require
      {
	if (statestack.back()[op.Feature()] == 0)
	  {
	    return false;
	  }
	statestack.push_back(statestack.back());
	return true;
      }
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
    if (op.Value() == 0) // empty disallow
      {
	if (statestack.back()[op.Feature()] != 0)
	  {
	    return false;
	  }
	else
	  {
	    statestack.push_back(statestack.back());
	    return true;
	  }
      }
    if (statestack.back()[op.Feature()] == op.Value()) // nonempty disallow
      {
	return false;
      }
    statestack.push_back(statestack.back());
    return true;
  case C: // clear
    statestack.push_back(statestack.back());
    statestack.back()[op.Feature()] = 0;
    return true;
  case U: // unification
    if (statestack.back()[op.Feature()] == 0 || // if the feature is unset or
	statestack.back()[op.Feature()] == op.Value() || // the feature is at this value already or
	(statestack.back()[op.Feature()] < 0 &&
	 (statestack.back()[op.Feature()] * -1 != op.Value())) // the feature is negatively set to something else
	)
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}


void TransducerW::try_epsilon_transitions(SymbolNumber * input_symbol,
					  SymbolNumber * output_symbol,
					  SymbolNumber * 
					  original_output_string,
					  TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "try epsilon transitions " << i << " " << current_weight << std::endl;
#endif

  if (transition_table.size() <= i) 
    {
      return;
    }

  while (get_transition(i).get_input() == 0)
    {
      *output_symbol = get_transition(i).get_output();
      current_weight += static_cast<const TransitionW&>(get_transition(i)).get_weight();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   get_transition(i).target());
      current_weight -= static_cast<const TransitionW&>(get_transition(i)).get_weight();
      ++i;
    }
  *output_symbol = NO_SYMBOL_NUMBER;
}

void TransducerWFd::try_epsilon_transitions(SymbolNumber * input_symbol,
					    SymbolNumber * output_symbol,
					    SymbolNumber * 
					    original_output_string,
					    TransitionTableIndex i)
{
  if (transition_table.size() <= i)
    { return; }
  
  while (true)
    {
    if (get_transition(i).get_input() == 0) // epsilon
	{
	  *output_symbol = get_transition(i).get_output();
	  current_weight += static_cast<const TransitionW&>(get_transition(i)).get_weight();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       get_transition(i).target());
	  current_weight += static_cast<const TransitionW&>(get_transition(i)).get_weight();
	  ++i;
	} else if (get_transition(i).get_input() != NO_SYMBOL_NUMBER &&
		   operations[get_transition(i).get_input()].isFlag())
	{
	  if (PushState(operations[get_transition(i).get_input()]))
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		alphabet.symbol_to_string(get_transition(i).get_input()) << " allowed\n";
#endif
	      // flag diacritic allowed
	      *output_symbol = get_transition(i).get_output();
	      current_weight += static_cast<const TransitionW&>(get_transition(i)).get_weight();
	      get_analyses(input_symbol,
			   output_symbol+1,
			   original_output_string,
			   get_transition(i).target());
	      current_weight -= static_cast<const TransitionW&>(get_transition(i)).get_weight();
	      statestack.pop_back();
	    }
	  else
	    {
#if OL_FULL_DEBUG
	      std::cout << "flag diacritic " <<
		alphabet.symbol_to_string(get_transition(i).get_input()) << " disallowed\n";
#endif
	    }
	  ++i;
	} else
	{
	  return;
	}
    }
}

void TransducerW::try_epsilon_indices(SymbolNumber * input_symbol,
				      SymbolNumber * output_symbol,
				      SymbolNumber * original_output_string,
				      TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "try indices " << i << " " << current_weight << std::endl;
#endif
  if (get_index(i).get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      get_index(i).target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::find_transitions(SymbolNumber input,
				   SymbolNumber * input_symbol,
				   SymbolNumber * output_symbol,
				   SymbolNumber * original_output_string,
				   TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "find transitions " << i << " " << current_weight << std::endl;
#endif

  if (transition_table.size() <= i) 
    {
      return;
    }
  while (get_transition(i).get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (get_transition(i).get_input() == input)
	{
	  current_weight += static_cast<const TransitionW&>(get_transition(i)).get_weight();
	  *output_symbol = get_transition(i).get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       get_transition(i).target());
	  current_weight -= static_cast<const TransitionW&>(get_transition(i)).get_weight();
	}
      else
	{
	  return;
	}
      ++i;
    }
  
}

void TransducerW::find_index(SymbolNumber input,
			     SymbolNumber * input_symbol,
			     SymbolNumber * output_symbol,
			     SymbolNumber * original_output_string,
			     TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "find index " << i << " " << current_weight << std::endl;
#endif
  if (index_table.size() <= i) 
    {
      return;
    }
  
  if (get_index(i+input).get_input() == input)
    {
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       get_index(i+input).target() - 
		       TRANSITION_TARGET_TABLE_START);
    }
}

void TransducerW::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(alphabet.symbol_to_string(*num));
    }
  display_map.insert(std::pair<Weight, std::string>(current_weight, str));
}
void TransducerW::note_analysis(const LookupPath& path)
{
  display_map.insert(std::pair<Weight, std::string>(
    static_cast<const LookupPathW&>(path).get_weight(),
    alphabet.symbols_to_string(path.get_output_symbols())));
}

void TransducerWUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(alphabet.symbol_to_string(*num));
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}
void TransducerWUniq::note_analysis(const LookupPath& path)
{
  std::string str = alphabet.symbols_to_string(path.get_output_symbols());
  const LookupPathW& p = static_cast<const LookupPathW&>(path);
  
  if((display_map.count(str) == 0) || (display_map[str] > p.get_weight()))
    display_map.insert(std::pair<std::string, Weight>(str, p.get_weight()));
}

void TransducerWFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(alphabet.symbol_to_string(*num));
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}
void TransducerWFdUniq::note_analysis(const LookupPath& path)
{
  std::string str = alphabet.symbols_to_string(path.get_output_symbols());
  const LookupPathWFd& p = static_cast<const LookupPathWFd&>(path);
  
  if((display_map.count(str) == 0) || (display_map[str] > p.get_weight()))
    display_map.insert(std::pair<std::string, Weight>(str, p.get_weight()));
}


void TransducerW::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  DisplayMultiMap::iterator it = display_map.begin();
  while ( (it != display_map.end()) && (i < maxAnalyses))
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << (*it).second;
      if (displayWeightsFlag)
	{
	  std::cout << '\t' << (*it).first;
	}
      std::cout << std::endl;
      ++it;
      ++i;
    }
  display_map.clear();
  std::cout << std::endl;
}

void TransducerWUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  std::multimap<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it = display_map.begin();
  while (it != display_map.end())
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
      ++it;
    }
  std::multimap<Weight, std::string>::iterator display_it = weight_sorted_map.begin();
  while ( (display_it != weight_sorted_map.end()) && (i < maxAnalyses))
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << (*display_it).second;
      if (displayWeightsFlag)
	{
	  std::cout << '\t' << (*display_it).first;
	}
      std::cout << std::endl;
      ++display_it;
      ++i;
    }
  display_map.clear();
  std::cout << std::endl;
}

void TransducerWFdUniq::printAnalyses(std::string prepend)
{
  if (outputType == xerox && display_map.size() == 0)
    {
      std::cout << prepend << "\t+?" << std::endl;
      std::cout << std::endl;
      return;
    }
  int i = 0;
  std::multimap<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it;
  for (it = display_map.begin(); it != display_map.end(); it++)
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
    }
  std::multimap<Weight, std::string>::iterator display_it;
  for (display_it = weight_sorted_map.begin();
       display_it != weight_sorted_map.end(), i < maxAnalyses;
       display_it++, i++)
    {
      if (outputType == xerox)
	{
	  std::cout << prepend << "\t";
	}
      std::cout << (*display_it).second;
      if (displayWeightsFlag)
	{
	  std::cout << '\t' << (*display_it).first;
	}
      std::cout << std::endl;
    }
  display_map.clear();
  std::cout << std::endl;
}

void TransducerW::get_analyses(SymbolNumber * input_symbol,
			       SymbolNumber * output_symbol,
			       SymbolNumber * original_output_string,
			       TransitionTableIndex i)
{
#if OL_FULL_DEBUG
  std::cerr << "get analyses " << i << " " << current_weight << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i -= TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);
      
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (transition_table.size() <= i) 
	    {
	      return;
	    }
	  if (get_transition(i).final())
	    {
	      current_weight += get_final_transition_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_transition_weight(i);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       i+1);
    }
  else
    {
      
      try_epsilon_indices(input_symbol,
			  output_symbol,
			  original_output_string,
			  i+1);
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  *output_symbol = NO_SYMBOL_NUMBER;
	  if (get_index(i).final())
	    {
	      current_weight += get_final_index_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_index_weight(i);
	    }
	  return;
	}
      
      SymbolNumber input = *input_symbol;
      ++input_symbol;
      
      find_index(input,
		 input_symbol,
		 output_symbol,
		 original_output_string,
		 i+1);
    }
}

