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

#include <getopt.h>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <config.h>

#include "hfst-optimized-lookup.h"

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
	  {"analyses",     required_argument, 0, 'n'},
	  {0,              0,                 0,  0 }
	};
      
      int option_index = 0;
      c = getopt_long(argc, argv, "hVvqsewn:", long_options, &option_index);

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
      FILE * f = fopen(argv[(optind)], "r");
      if (f == NULL)
	{
	  std::cerr << "Could not open file " << argv[(optind)] << std::endl;
	  return 1;
	}
      return setup(f);
    }
  else
    {
      std::cerr << "No input file given\n";
      return EXIT_FAILURE;
    }
}

void TransducerAlphabet::get_next_symbol(FILE * f, SymbolNumber k)
{
  int byte;
  char * sym = line;
  while ( (byte = fgetc(f)) != 0 )
    {
      if (byte == EOF)
	{
	  std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
	  exit(1);
	}
      *sym = byte;
      ++sym;
    }
  *sym = 0;
  if (line[0] == '@' && *--sym == '@' && line[2] == '.')
    { // a flag diacritic needs to be parsed
      std::string feat;
      std::string val;
      FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
      switch (line[1]) {
      case 'P': op = P; break;
      case 'N': op = N; break;
      case 'R': op = R; break;
      case 'D': op = D; break;
      case 'C': op = C; break;
      case 'U': op = U; break;
      }
      char * c = line;
      // as long as we're working with utf-8, this should be ok
      for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
      if (*c == '.')
	{
	  for (; *c != '@'; c++) { val.append(c,1); }
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
      operations.push_back(FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]));
      kt->operator[](k) = "";
#if DEBUG_DIACRITICS
      kt->operator[](k) = strdup(line);
#endif
      return;
    }
  operations.push_back(FlagDiacriticOperation(P, NO_SYMBOL_NUMBER, 0));
  kt->operator[](k) = strdup(line);
}

void LetterTrie::add_string(const char * p,SymbolNumber symbol_key)
{
  if (*(p+1) == 0)
    {
      symbols[(unsigned char)(*p)] = symbol_key;
      return;
    }
  if (letters[(unsigned char)(*p)] == NULL)
    {
      letters[(unsigned char)(*p)] = new LetterTrie();
    }
  letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber LetterTrie::find_key(char ** p)
{
  const char * old_p = *p;
  ++(*p);
  if (letters[(unsigned char)(*old_p)] == NULL)
    {
      return symbols[(unsigned char)(*old_p)];
    }
  SymbolNumber s = letters[(unsigned char)(*old_p)]->find_key(p);
  if (s == NO_SYMBOL_NUMBER)
    {
      --(*p);
      return symbols[(unsigned char)(*old_p)];
    }
  return s;
}

void Encoder::read_input_symbols(KeyTable * kt)
{
  for (SymbolNumber k = 0; k < number_of_input_symbols; ++k)
    {
#if DEBUG
      assert(kt->find(k) != kt->end());
#endif
      const char * p = kt->operator[](k);
      if ((unsigned char)(*p) <= 127)
	{
	  ascii_symbols[(unsigned char)(*p)] = k;
	}
      letters.add_string(p,k);
    }
}

SymbolNumber Encoder::find_key(char ** p)
{
  if (ascii_symbols[(unsigned char)(**p)] == NO_SYMBOL_NUMBER)
    {
      return letters.find_key(p);
    }
  SymbolNumber s = ascii_symbols[(unsigned char)(**p)];
  ++(*p);
  return s;
}

template <class genericTransducer>
void runTransducer (genericTransducer T)
{
  SymbolNumber * input_string = (SymbolNumber*)(malloc(2000));
  for (size_t i = 0; i < 1000; ++i)
    {
      input_string[i] = NO_SYMBOL_NUMBER;
    }
  
  char * str = (char*)(malloc(MAX_IO_STRING*sizeof(char)));  
  *str = 0;
  char * old_str = str;

  size_t input_form_counter = 0;
  while(std::cin.getline(str,MAX_IO_STRING))
    {
      if (echoInputsFlag)
	{
	  std::cout << str << std::endl;
	}
      size_t i = 0;
      SymbolNumber k = NO_SYMBOL_NUMBER;
      bool failed = false;
      for ( char ** Str = &str; **Str != 0; )
	{
	  k = T.find_next_key(Str);
#if FULL_DEBUG
	  std::cout << "INPUT STRING ENTRY " << i << " IS " << k << std::endl;
#endif
	  if (k == NO_SYMBOL_NUMBER)
	    {
	      if (echoInputsFlag)
		{
		  std::cout << std::endl;
		}
	      failed = true;
	      break;
	    }
	  input_string[i] = k;
	  ++i;
	}
      str = old_str;
      if (failed)
      	{ // tokenization failed
      	  continue;
      	}
      ++input_form_counter;
      input_string[i] = NO_SYMBOL_NUMBER;
      T.analyze(input_string);
      std::cout << std::endl;
      T.printAnalyses();
    }
}

int setup(FILE * f)
{
  TransducerHeader header(f);
  TransducerAlphabet alphabet(f, header.symbol_count());

  if (alphabet.get_state_size() == 0)
    {      // if the state size is zero, there are no flag diacritics to handle
      if (header.probe_flag(Weighted) == false)
	{
	  if (displayUniqueFlag)
	    { // no flags, no weights, unique analyses only
	      TransducerUniq C(f, header, alphabet);
	      runTransducer(C);
	    } else if (!displayUniqueFlag)
	    { // no flags, no weights, all analyses
	    Transducer C(f, header, alphabet);
	    runTransducer(C);
	    }
	}
      else if (header.probe_flag(Weighted) == true)
	{
	  if (displayUniqueFlag)
	    { // no flags, weights, unique analyses only
	      TransducerWUniq C(f, header, alphabet);
	      runTransducer(C);
	    } else if (!displayUniqueFlag)
	    { // no flags, weights, all analyses
	      TransducerW C(f, header, alphabet);
	      runTransducer(C);
	    }
	}
    } else // handle flag diacritics
    {
      if (header.probe_flag(Weighted) == false)
	{
	  if (displayUniqueFlag)
	    { // flags, no weights, unique analyses only
	      TransducerFdUniq C(f, header, alphabet);
	      runTransducer(C);
	    } else
	    { // flags, no weights, all analyses
	      TransducerFd C(f, header, alphabet);
	      runTransducer(C);
	    }
	}
      else if (header.probe_flag(Weighted) == true)
	{
	  if (displayUniqueFlag)
	    { // flags, weights, unique analyses only
	      TransducerWFdUniq C(f, header, alphabet);
	      runTransducer(C);
	    } else
	    { // flags, no weights, all analyses
	      TransducerWFd C(f, header, alphabet);
	      runTransducer(C);
	    }
	}
    }
  return 0;
}

/**
 * BEGIN old transducer.cc
 */

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
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
    if (statestack.back()[op.Feature()] == op.Value())
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
    if (statestack.back()[op.Feature()] == 0 || statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool TransitionIndex::matches(SymbolNumber s)
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

bool Transition::matches(SymbolNumber s)
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


void IndexTableReader::get_index_vector(void)
{
  for (size_t i = 0;
       i < number_of_table_entries;
       ++i)
    {
      size_t j = i * TransitionIndex::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
      TransitionTableIndex * index = 
	(TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
      indices.push_back(new TransitionIndex(*input,*index));
    }
}

void TransitionTableReader::Set(TransitionTableIndex pos)
{
  if (pos >= TRANSITION_TARGET_TABLE_START)
    {
      position = pos - TRANSITION_TARGET_TABLE_START;
    }
  else
    {
      position = pos;
    }
}

void TransitionTableReader::get_transition_vector(void)
{
  for (size_t i = 0; i < number_of_table_entries; ++i)
    {
      size_t j = i * Transition::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
      SymbolNumber * output = 
	(SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
      TransitionTableIndex * target = 
       (TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
      transitions.push_back(new Transition(*input,
					   *output,
					   *target));
      
    }
}

bool TransitionTableReader::Matches(SymbolNumber s)
{
  Transition * t = transitions[position];
  return t->matches(s);
}

bool TransitionTableReader::get_finality(TransitionTableIndex i)
{
  if (i >= TRANSITION_TARGET_TABLE_START) 
    {
      return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
  else
    {
      return transitions[i]->final();
    }
}

void Transducer::set_symbol_table(void)
{
  for(KeyTable::iterator it = keys->begin();
      it != keys->end();
      ++it)
    {
      const char * key_name =
	it->second;

      symbol_table.push_back(key_name);
    }
}

void Transducer::try_epsilon_transitions(SymbolNumber * input_symbol,
					 SymbolNumber * output_symbol,
					 SymbolNumber * 
					 original_output_string,
					 TransitionTableIndex i)
{
  while (transitions[i]->get_input() == 0)
    {
      *output_symbol = transitions[i]->get_output();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      ++i;
    }
}

void TransducerFd::try_epsilon_transitions(SymbolNumber * input_symbol,
					   SymbolNumber * output_symbol,
					   SymbolNumber * 
					   original_output_string,
					   TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cout << "try_epsilon_transitions " << i << std::endl;
#endif
  while (transitions[i]->get_input() == 0)
    {
      *output_symbol = transitions[i]->get_output();
      if (operations[*output_symbol].Feature() != NO_SYMBOL_NUMBER) // this is a flag diacritic
	{ // we try to modify the state stack
	  if (PushState(operations[*output_symbol]) == false)
	    {
	      ++i;	// the new state wouldn't be legal
              continue;
	    } else {      get_analyses(input_symbol,
				       output_symbol+1,
				       original_output_string,
				       transitions[i]->target());
	    ++i;
	    statestack.pop_back();
	    continue;
	  }
	}
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      ++i;
    }
}


void  Transducer::try_epsilon_indices(SymbolNumber * input_symbol,
				      SymbolNumber * output_symbol,
				      SymbolNumber * original_output_string,
				      TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cout << "try_epsilon_indices " << i << std::endl;
#endif
  if (indices[i]->get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      indices[i]->target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}


void Transducer::find_transitions(SymbolNumber input,
				    SymbolNumber * input_symbol,
				    SymbolNumber * output_symbol,
				    SymbolNumber * original_output_string,
				    TransitionTableIndex i)
{
  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (transitions[i]->get_input() == input)
	{
	  
	  *output_symbol = transitions[i]->get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	}
      else
	{
	  return;
	}
      ++i;
    }
  
}

void TransducerFd::find_transitions(SymbolNumber input,
				    SymbolNumber * input_symbol,
				    SymbolNumber * output_symbol,
				    SymbolNumber * original_output_string,
				    TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cout << "find_transitions " << i << std::endl;
#endif
  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (transitions[i]->get_input() == input)
	{
	  *output_symbol = transitions[i]->get_output();
	  if (operations[*output_symbol].Feature() != NO_SYMBOL_NUMBER) // this is a flag diacritic
	    { // we try to modify the state stack
	      if (PushState(operations[*output_symbol]) == false)
		{
		  return; // the new state wouldn't be legal
		} else {
		get_analyses(input_symbol,
			     output_symbol+1,
			     original_output_string,
			     transitions[i]->target());
		++i;
		statestack.pop_back();
		continue;
	      }
	    }
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	}
      else
	{
	  return;
	}
      ++i;
    }
}


void 
Transducer::find_index(SymbolNumber input,
				      SymbolNumber * input_symbol,
				      SymbolNumber * output_symbol,
				      SymbolNumber * original_output_string,
				      TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cout << "find_index " << i << std::endl;
#endif
  if (indices[i+input]->get_input() == input)
    {
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       indices[i+input]->target() - 
		         TRANSITION_TARGET_TABLE_START);
    }
}

void Transducer::note_analysis(SymbolNumber * whole_output_string)
{
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    std::cout << symbol_table[*num];
  std::cout << std::endl;
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(symbol_table[*num]);
    }
    display_vector.push_back(str);
}

void TransducerUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(symbol_table[*num]);
    }
  display_vector.insert(str);
}

void TransducerFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string; *num != NO_SYMBOL_NUMBER; ++num)
    {
      str.append(symbol_table[*num]);
    }
  display_vector.insert(str);
}

void
Transducer::get_analyses(SymbolNumber * input_symbol,
			 SymbolNumber * output_symbol,
			 SymbolNumber * original_output_string,
			 TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cout << "get_analyses " << i << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i = i - TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);
      
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  if (final_transition(i))
	    {
	      note_analysis(original_output_string);
	    }
	  
	  *output_symbol = NO_SYMBOL_NUMBER;
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
#if FULL_DEBUG
      std::cout << "Testing input string, " << *input_symbol << " at pointer" << std::endl;
#endif
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
#if FULL_DEBUG
	  std::cout << "At end of input string" << std::endl;
#endif
	  if (final_index(i))
	    {
	      note_analysis(original_output_string);
	    }
	  
	  *output_symbol = NO_SYMBOL_NUMBER;
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

void Transducer::printAnalyses(void)
{
  std::cout << std::endl;
  int i = 0;
  DisplayVector::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses )
    {
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
    display_vector.clear(); // purge the display vector
  std::cout << std::endl;
}

void TransducerUniq::printAnalyses(void)
{
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
      std::cout << *it << std::endl;
      ++it;
      ++i;
    }
  display_vector.clear(); // purge the display set
  std::cout << std::endl;
}

void TransducerFdUniq::printAnalyses(void)
{
  int i = 0;
  DisplaySet::iterator it = display_vector.begin();
  while ( (it != display_vector.end()) && i < maxAnalyses)
    {
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

bool TransitionWIndex::matches(SymbolNumber s)
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

bool TransitionW::matches(SymbolNumber s)
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
    if (statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	return true;
      }
    return false;
  case D: // disallow
    if (statestack.back()[op.Feature()] == op.Value())
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
    if (statestack.back()[op.Feature()] == 0 || statestack.back()[op.Feature()] == op.Value())
      {
	statestack.push_back(statestack.back());
	statestack.back()[op.Feature()] = op.Value();
	return true;
      }
    return false;
  }
  throw; // for the compiler's peace of mind
}

void IndexTableReaderW::get_index_vector(void)
{
  for (size_t i = 0;
       i < number_of_table_entries;
       ++i)
    {
      size_t j = i * TransitionWIndex::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableIndices + j);
      TransitionTableIndex * index = 
	(TransitionTableIndex*)(TableIndices + j + sizeof(SymbolNumber));
      indices.push_back(new TransitionWIndex(*input,*index));
    }
}

void TransitionTableReaderW::Set(TransitionTableIndex pos)
{
  if (pos >= TRANSITION_TARGET_TABLE_START)
    {
      position = pos - TRANSITION_TARGET_TABLE_START;
    }
  else
    {
      position = pos;
    }
}

void TransitionTableReaderW::get_transition_vector(void)
{
  for (size_t i = 0; i < number_of_table_entries; ++i)
    {
      size_t j = i * TransitionW::SIZE;
      SymbolNumber * input = (SymbolNumber*)(TableTransitions + j);
      SymbolNumber * output = 
	(SymbolNumber*)(TableTransitions + j + sizeof(SymbolNumber));
      TransitionTableIndex * target = 
	(TransitionTableIndex*)(TableTransitions + j + 2 * sizeof(SymbolNumber));
      Weight * weight =
	(Weight*)(TableTransitions + j + 2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex));
      transitions.push_back(new TransitionW(*input,
					    *output,
					    *target,
					    *weight));
      
    }
  transitions.push_back(new TransitionW());
  transitions.push_back(new TransitionW());
}

bool TransitionTableReaderW::Matches(SymbolNumber s)
{
  TransitionW * t = transitions[position];
  return t->matches(s);
}

bool TransitionTableReaderW::get_finality(TransitionTableIndex i)
{
  if (i >= TRANSITION_TARGET_TABLE_START) 
    {
      return transitions[i - TRANSITION_TARGET_TABLE_START]->final();
    }
  else
    {
      return transitions[i]->final();
    }
}


void TransducerW::set_symbol_table(void)
{
  for(KeyTable::iterator it = keys->begin();
      it != keys->end();
      ++it)
    {
      const char * key_name =
	it->second;

      symbol_table.push_back(key_name);
    }
}

void TransducerW::try_epsilon_transitions(SymbolNumber * input_symbol,
					  SymbolNumber * output_symbol,
					  SymbolNumber * 
					  original_output_string,
					  TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cerr << "try epsilon transitions " << i << " " << current_weight << std::endl;
#endif

  if (transitions.size() <= i) 
    {
      return;
    }

  while ((transitions[i] != NULL) and (transitions[i]->get_input() == 0))
    {
      *output_symbol = transitions[i]->get_output();
      current_weight += transitions[i]->get_weight();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      current_weight -= transitions[i]->get_weight();
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
  if (transitions.size() <= i) 
    {
      return;
    }

  while ((transitions[i] != NULL) and (transitions[i]->get_input() == 0))
    {
      *output_symbol = transitions[i]->get_output();
      if (operations[*output_symbol].Feature() != NO_SYMBOL_NUMBER) // this is a flag diacritic
	{ // we try to modify the state stack
	  if (PushState(operations[*output_symbol]) == false)
	    {
	      return; // the new state wouldn't be legal
	    } else {
	    current_weight += transitions[i]->get_weight();
	    get_analyses(input_symbol,
			 output_symbol+1, // output_symbol points to pre-symbol zero entry
			 original_output_string,
			 transitions[i]->target());
	    ++i;
	    statestack.pop_back();
	    current_weight -= transitions[i]->get_weight();
	    continue;
	  }
	}
      
      current_weight += transitions[i]->get_weight();
      get_analyses(input_symbol,
		   output_symbol+1,
		   original_output_string,
		   transitions[i]->target());
      current_weight -= transitions[i]->get_weight();
      ++i;
    }
  *output_symbol = NO_SYMBOL_NUMBER;
}

void TransducerW::try_epsilon_indices(SymbolNumber * input_symbol,
				      SymbolNumber * output_symbol,
				      SymbolNumber * original_output_string,
				      TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cerr << "try indices " << i << " " << current_weight << std::endl;
#endif

  if (indices[i]->get_input() == 0)
    {
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      indices[i]->target() - 
			      TRANSITION_TARGET_TABLE_START);
    }
}


void TransducerW::find_transitions(SymbolNumber input,
				   SymbolNumber * input_symbol,
				   SymbolNumber * output_symbol,
				   SymbolNumber * original_output_string,
				   TransitionTableIndex i)
{
#if FULL_DEBUG
  std::cerr << "find transitions " << i << " " << current_weight << std::endl;
#endif

  if (transitions.size() <= i) 
    {
      return;
    }
  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (transitions[i]->get_input() == input)
	{
	  current_weight += transitions[i]->get_weight();
	  *output_symbol = transitions[i]->get_output();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	  current_weight -= transitions[i]->get_weight();
	}
      else
	{
	  return;
	}
      ++i;
    }
  
}

void TransducerWFd::find_transitions(SymbolNumber input,
				     SymbolNumber * input_symbol,
				     SymbolNumber * output_symbol,
				     SymbolNumber * original_output_string,
				     TransitionTableIndex i)
{
  if (transitions.size() <= i) 
    {
      return;
    }
  while (transitions[i]->get_input() != NO_SYMBOL_NUMBER)
    {
      
      if (transitions[i]->get_input() == input)
	{
	  *output_symbol = transitions[i]->get_output();
	  if (operations[*output_symbol].Feature() != NO_SYMBOL_NUMBER) // this is a flag diacritic
	    { // we try to modify the state stack
	      if (PushState(operations[*output_symbol]) == false)
		{
		  return; // the new state wouldn't be legal
		} else {
		current_weight += transitions[i]->get_weight();
		get_analyses(input_symbol,
			     output_symbol+1, // output_symbol points to pre-symbol zero entry
			     original_output_string,
			     transitions[i]->target());
		++i;
		statestack.pop_back();
		current_weight -= transitions[i]->get_weight();
		continue;
	      }
	    }
	  
	  current_weight += transitions[i]->get_weight();
	  get_analyses(input_symbol,
		       output_symbol+1,
		       original_output_string,
		       transitions[i]->target());
	  current_weight -= transitions[i]->get_weight();
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
#if FULL_DEBUG
  std::cerr << "find index " << i << " " << current_weight << std::endl;
#endif
  if (transitions.size() <= i) 
    {
      return;
    }
  
  if (indices[i+input]->get_input() == input)
    {
      
      find_transitions(input,
		       input_symbol,
		       output_symbol,
		       original_output_string,
		       indices[i+input]->target() - 
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
      str.append(symbol_table[*num]);
    }
  display_map.insert(std::pair<Weight, std::string>(current_weight, str));
}

void TransducerWUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(symbol_table[*num]);
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}

void TransducerWFdUniq::note_analysis(SymbolNumber * whole_output_string)
{
  std::string str = "";
  for (SymbolNumber * num = whole_output_string;
       *num != NO_SYMBOL_NUMBER;
       ++num)
    {
      str.append(symbol_table[*num]);
    }
  if ((display_map.count(str) == 0) || (display_map[str] > current_weight))
    { // if there isn't an entry yet or we've found a lower weight
      display_map.insert(std::pair<std::string, Weight>(str, current_weight));
    }
}

void TransducerW::printAnalyses(void)
{
  int i = 0;
  DisplayMultiMap::iterator it = display_map.begin();
  while ( (it != display_map.end()) && (i < maxAnalyses))
    {
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

void TransducerWUniq::printAnalyses(void)
{
  int i = 0;
  std::map<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it = display_map.begin();
  while (it != display_map.end())
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
      ++it;
    }
  std::map<Weight, std::string>::iterator display_it = weight_sorted_map.begin();
  while ( (display_it != weight_sorted_map.end()) && (i < maxAnalyses))
    {
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

void TransducerWFdUniq::printAnalyses(void)
{
  int i = 0;
  std::map<Weight, std::string> weight_sorted_map;
  DisplayMap::iterator it;
  for (it = display_map.begin(); it != display_map.end(); it++)
    {
      weight_sorted_map.insert(std::pair<Weight, std::string>((*it).second, (*it).first));
    }
  std::map<Weight, std::string>::iterator display_it;
  for (display_it = weight_sorted_map.begin();
       display_it != weight_sorted_map.end(), i < maxAnalyses;
       display_it++, i++)
    {
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
#if FULL_DEBUG
  std::cerr << "get analyses " << i << " " << current_weight << std::endl;
#endif
  if (i >= TRANSITION_TARGET_TABLE_START )
    {
      i = i - TRANSITION_TARGET_TABLE_START;
      
      try_epsilon_transitions(input_symbol,
			      output_symbol,
			      original_output_string,
			      i+1);
      
      // input-string ended.
      if (*input_symbol == NO_SYMBOL_NUMBER)
	{
	  if (transitions.size() <= i) 
	    {
	      return;
	    }
	  if (final_transition(i))
	    {
	      current_weight += get_final_transition_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_transition_weight(i);
	    }
	  
	  *output_symbol = NO_SYMBOL_NUMBER;
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
	  if (final_index(i))
	    {
	      current_weight += get_final_index_weight(i);
	      note_analysis(original_output_string);
	      current_weight -= get_final_index_weight(i);
	    }
	  
	  *output_symbol = NO_SYMBOL_NUMBER;
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
