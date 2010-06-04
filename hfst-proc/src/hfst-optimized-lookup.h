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
 */
 
#ifndef _HFST_OPTIMIZED_LOOKUP_H_
#define _HFST_OPTIMIZED_LOOKUP_H_

#include <getopt.h>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <cassert>
#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define OL_FULL_DEBUG 0

class LookupPath;

/**
 * A list of pointers to lookup paths. They are pointers for the sake of
 * polymorphism
 */
typedef std::vector<LookupPath*> LookupPathVector;

class TokenIOStream;
class Token;
typedef std::vector<Token> TokenVector;

enum OutputType {HFST, xerox};
extern OutputType outputType;

extern bool verboseFlag;

extern bool displayWeightsFlag;
extern bool displayUniqueFlag;
extern bool echoInputsFlag;
extern bool beFast;
extern int maxAnalyses;
extern bool preserveDiacriticRepresentationsFlag;

#define MAX_IO_STRING 5000

// the following flags are only meaningful with certain debugging #defines
extern bool timingFlag;
extern bool printDebuggingInformationFlag;

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef unsigned int TransitionNumber;
typedef unsigned int StateIdNumber;
typedef short ValueNumber;
typedef std::vector<SymbolNumber> SymbolNumberVector;
typedef std::map<SymbolNumber, std::string> SymbolTable;

const StateIdNumber NO_ID_NUMBER = UINT_MAX;
const SymbolNumber NO_SYMBOL_NUMBER = USHRT_MAX;
const TransitionTableIndex NO_TABLE_INDEX = UINT_MAX;

class TransitionIndex;
class Transition;

// the flag diacritic operators as given in
// Beesley & Karttunen, Finite State Morphology (U of C Press 2003)
enum FlagDiacriticOperator {P, N, R, D, C, U};

enum HeaderFlag {Weighted, Deterministic, Input_deterministic, Minimized,
		 Cyclic, Has_epsilon_epsilon_transitions,
		 Has_input_epsilon_transitions, Has_input_epsilon_cycles,
		 Has_unweighted_input_epsilon_cycles};

typedef std::vector<TransitionIndex*> TransitionIndexVector;
typedef std::vector<Transition*> TransitionVector;

// This is 2^31, hopefully equal to UINT_MAX/2 rounded up.
// For some profound reason it can't be replaced with (UINT_MAX+1)/2.
const TransitionTableIndex TRANSITION_TARGET_TABLE_START = 2147483648u;

class TransducerHeader
{
 private:
  SymbolNumber number_of_symbols;
  SymbolNumber number_of_input_symbols;
  TransitionTableIndex size_of_transition_index_table;
  TransitionTableIndex size_of_transition_target_table;

  StateIdNumber number_of_states;
  TransitionNumber number_of_transitions;

  bool weighted;
  bool deterministic;
  bool input_deterministic;
  bool minimized;
  bool cyclic;
  bool has_epsilon_epsilon_transitions;
  bool has_input_epsilon_transitions;
  bool has_input_epsilon_cycles;
  bool has_unweighted_input_epsilon_cycles;

  void read_property(bool& property, std::istream& is)
  {
    unsigned int prop;
    is.read(reinterpret_cast<char*>(&prop), sizeof(unsigned int));
    
    property = (prop != 0);
  }

 public:
  TransducerHeader(std::istream& is)
  {
  	is.read(reinterpret_cast<char*>(&number_of_input_symbols), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&number_of_symbols), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&size_of_transition_index_table), sizeof(TransitionTableIndex));
  	is.read(reinterpret_cast<char*>(&size_of_transition_target_table), sizeof(TransitionTableIndex));
  	is.read(reinterpret_cast<char*>(&number_of_states), sizeof(StateIdNumber));
  	is.read(reinterpret_cast<char*>(&number_of_transitions), sizeof(TransitionNumber));
  	
  	read_property(weighted, is);
  	read_property(deterministic, is);
    read_property(input_deterministic, is);
    read_property(minimized, is);
    read_property(cyclic, is);
    read_property(has_epsilon_epsilon_transitions, is);
    read_property(has_input_epsilon_transitions, is);
    read_property(has_input_epsilon_cycles, is);
    read_property(has_unweighted_input_epsilon_cycles, is);
  }

  SymbolNumber symbol_count(void) const { return number_of_symbols; }
  SymbolNumber input_symbol_count(void) const {return number_of_input_symbols;}
  
  TransitionTableIndex index_table_size(void) const
  { return size_of_transition_index_table; }
  TransitionTableIndex target_table_size(void) const
  { return size_of_transition_target_table; }

  bool probe_flag(HeaderFlag flag) const
  {
    switch (flag) {
    case Weighted: return weighted;
    case Deterministic: return deterministic;
    case Input_deterministic: return input_deterministic;
    case Minimized: return minimized;
    case Cyclic: return cyclic;
    case Has_epsilon_epsilon_transitions: return has_epsilon_epsilon_transitions;
    case Has_input_epsilon_transitions: return has_input_epsilon_transitions;
    case Has_input_epsilon_cycles: return has_input_epsilon_cycles;
    case Has_unweighted_input_epsilon_cycles: return has_unweighted_input_epsilon_cycles;
    }
    return false;
  }
  
#ifdef DEBUG
  void print()
  {
    std::cout << "Transducer properties:" << std::endl
              << " number_of_symbols: " << number_of_symbols << std::endl
              << " number_of_input_symbols: " << number_of_input_symbols << std::endl
              << " size_of_transition_index_table: " << size_of_transition_index_table << std::endl
              << " size_of_transition_target_table: " << size_of_transition_target_table << std::endl

              << " number_of_states: " << number_of_states << std::endl
              << " number_of_transitions: " << number_of_transitions << std::endl

              << " weighted: " << weighted << std::endl
              << " deterministic: " << deterministic << std::endl
              << " input_deterministic: " << input_deterministic << std::endl
              << " minimized: " << minimized << std::endl
              << " cyclic: " << cyclic << std::endl
              << " has_epsilon_epsilon_transitions: " << has_epsilon_epsilon_transitions << std::endl
              << " has_input_epsilon_transitions: " << has_input_epsilon_transitions << std::endl
              << " has_input_epsilon_cycles: " << has_input_epsilon_cycles << std::endl
              << " has_unweighted_input_epsilon_cycles: " << has_unweighted_input_epsilon_cycles << std::endl;
  }
#endif
  
};

class FlagDiacriticOperation
{
 private:
  FlagDiacriticOperator operation;
  SymbolNumber feature;
  ValueNumber value;
 public:
 FlagDiacriticOperation(FlagDiacriticOperator op, SymbolNumber feat, ValueNumber val):
  operation(op), feature(feat), value(val) {}

  // dummy constructor
 FlagDiacriticOperation():
  operation(P), feature(NO_SYMBOL_NUMBER), value(0) {}
  
  bool isFlag(void) const { return feature != NO_SYMBOL_NUMBER; }
  FlagDiacriticOperator Operation(void) const { return operation; }
  SymbolNumber Feature(void) const { return feature; }
  ValueNumber Value(void) const { return value; }

#if OL_FULL_DEBUG
  void print(void)
  {
    std::cout << operation << "\t" << feature << "\t" << value << std::endl;
  }
#endif
};

typedef std::vector<FlagDiacriticOperation> OperationVector;

class LetterTrie;
typedef std::vector<LetterTrie*> LetterTrieVector;

class LetterTrie
{
 private:
  LetterTrieVector letters;
  SymbolNumberVector symbols;

 public:
  LetterTrie(void):
    letters(UCHAR_MAX, (LetterTrie*) NULL),
    symbols(UCHAR_MAX,NO_SYMBOL_NUMBER)
  {}
  
  LetterTrie(const LetterTrie& o): symbols(o.symbols)
  {
    for(LetterTrieVector::const_iterator it=o.letters.begin(); it!=o.letters.end(); it++)
      letters.push_back(((*it)==NULL) ? NULL : new LetterTrie(*(*it)));
  }
  
  ~LetterTrie()
  {
    for(LetterTrieVector::iterator it=letters.begin(); it!=letters.end(); it++)
      delete *it;
  }

  void add_string(const char * p,SymbolNumber symbol_key);

  SymbolNumber find_key(char ** p) const;
  
  /**
   * Read the next symbol from the stream. If the next character(s) do not form
   * a symbol, this version will put the characters back, so the stream is in
   * the same condition it was when this function was called
   * @return the number of the symbol, 0 for EOF, or NO_SYMBOL_NUMBER
   */
  SymbolNumber extract_symbol(std::istream& is) const;
};

class Encoder
{
 private:
  SymbolNumber number_of_input_symbols;
  LetterTrie letters;
  SymbolNumberVector ascii_symbols;

  void read_input_symbols(const SymbolTable& st);

 public:
 Encoder(const SymbolTable& st, SymbolNumber input_symbol_count):
  number_of_input_symbols(input_symbol_count),
    ascii_symbols(UCHAR_MAX,NO_SYMBOL_NUMBER)
  {
    read_input_symbols(st);
    for(size_t i=0; i<ascii_symbols.size(); i++)
    {
      if(ascii_symbols[i] == 0)
      {
        ascii_symbols[i] = NO_SYMBOL_NUMBER;
        if(printDebuggingInformationFlag)
          std::cout << "Encoder ignoring shortcut for ASCII character '" 
                    << (char)i << "' (" << i << ")" << std::endl;
      }
    }
  }
  
  SymbolNumber find_key(char ** p) const;
  SymbolNumber extract_symbol(std::istream& is) const;
};

class TransducerAlphabet
{
 private:
  SymbolTable symbol_table;
  OperationVector operations;
  
  /**
   * The set of symbols that are considered alphabetic. Roughly, this includes
   * symbols that are not whitespace or punctuation
   */
  std::set<SymbolNumber> alphabetic;
  
  /**
   * The symbol number for a "blank" which is here considered to be a space.
   * This symbol requires special handling because it doubles as the 
   * symbolic representation for a superblank block of text
   */
  SymbolNumber blank_symbol;
  
  void get_next_symbol(std::istream& is, SymbolNumber k);
  
  /**
   * Store the blank symbol's number in blank_symbol, adding it as a new symbol
   * if it cannot be found
   */
  void setup_blank_symbol();
  
  void calculate_alphabetic();

  std::map<std::string, SymbolNumber> feature_bucket;
  std::map<std::string, ValueNumber> value_bucket;
  ValueNumber val_num;
  SymbolNumber feat_num;
  
  SymbolNumber add_symbol(std::string string)
  {
    return add_symbol(symbol_table.size(), string);
  }
  SymbolNumber add_symbol(SymbolNumber symbol, std::string string)
  {
    return add_symbol(symbol, string, FlagDiacriticOperation()); // use a dummy flag
  }
  SymbolNumber add_symbol(SymbolNumber symbol, std::string string, FlagDiacriticOperation fdo)
  {
    operations.push_back(fdo);
    symbol_table[symbol] = string;
    return symbol;
  }
  
  Encoder* encoder;
 
 public:
  
  TransducerAlphabet(std::istream& is, SymbolNumber symbol_count)
  {
    feat_num = 0;
    val_num = 1;
    value_bucket[std::string()] = 0; // empty value = neutral
    for(SymbolNumber k=0; k<symbol_count; k++)
      get_next_symbol(is, k);
    
    if(printDebuggingInformationFlag && get_state_size()>0)
      std::cout << "Alphabet contains " << get_state_size() << " flag diacritic feature(s)" << std::endl;
    // assume the first symbol is epsilon which we don't want to print
    symbol_table.operator[](0) = "";
    
    setup_blank_symbol();
    calculate_alphabetic();
    
    encoder = new Encoder(symbol_table, symbol_table.size());
  }
  
  TransducerAlphabet(const TransducerAlphabet& o):
    symbol_table(o.symbol_table), operations(o.operations), alphabetic(o.alphabetic),
    blank_symbol(o.blank_symbol), feature_bucket(o.feature_bucket),
    value_bucket(o.value_bucket), val_num(o.val_num), feat_num(o.feat_num),
    encoder(new Encoder(*o.encoder)) {}
  
  ~TransducerAlphabet()
  {
    delete encoder;
  }
  
  SymbolTable& get_symbol_table(void) { return symbol_table; }
  OperationVector get_operation_vector(void) { return operations; }
  SymbolNumber get_state_size(void) const { return feature_bucket.size(); }
  SymbolNumber get_blank_symbol() const {return blank_symbol;}
  
  bool is_punctuation(const char* c) const;
  
  bool is_alphabetic(const char* c) const
  { return (strcmp(c,"")!=0 && !isspace(c[0]) && 
            !is_punctuation(c)); }
  bool is_alphabetic(SymbolNumber symbol) const 
  {return (alphabetic.find(symbol) != alphabetic.end());}
    
  std::string symbol_to_string(SymbolNumber symbol) const
  {
    SymbolTable::const_iterator it = symbol_table.find(symbol);
    return (it==symbol_table.end()) ? "" : symbol_table.find(symbol)->second;
  }
  
  /**
   * Use the symbol table to convert the given symbols into a string
   * @param symbols the symbols to convert
   * @return the string representation of the symbols
   */
  std::string symbols_to_string(const SymbolNumberVector& symbols) const;
  
  SymbolNumber find_key(char ** p) const {return encoder->find_key(p);}
  SymbolNumber extract_symbol(std::istream& is) const 
  {return encoder->extract_symbol(is);}
};

typedef std::vector<ValueNumber> FlagDiacriticState;
typedef std::vector<FlagDiacriticState> FlagDiacriticStateStack;

/*
 * BEGIN old transducer.h
 */

typedef std::vector<std::string> DisplayVector;
typedef std::set<std::string> DisplaySet;

class TransitionIndex
{
 protected:
  SymbolNumber input_symbol;
  TransitionTableIndex first_transition_index;
  
 public:
  
  // Each TransitionIndex has an input symbol and a target index.
  static const size_t SIZE = 
    sizeof(SymbolNumber) + sizeof(TransitionTableIndex);

 TransitionIndex(SymbolNumber input,
		 TransitionTableIndex first_transition):
    input_symbol(input),
    first_transition_index(first_transition)
    {}
  
  TransitionIndex(std::istream& is)
  {
  	is.read(reinterpret_cast<char*>(&input_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&first_transition_index), sizeof(TransitionTableIndex));
  }

  bool matches(const SymbolNumber s) const;
  
  TransitionTableIndex target(void) const {return first_transition_index;}
  SymbolNumber get_input(void) const {return input_symbol;}
  
  virtual bool final(void) const
  {
    return first_transition_index == 1;
  }
};

class Transition
{
 protected:
  SymbolNumber input_symbol;
  SymbolNumber output_symbol;
  TransitionTableIndex target_index;

 public:

  // Each transition has an input symbol an output symbol and 
  // a target index.
  static const size_t SIZE = 
    2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex);

  Transition(SymbolNumber input, SymbolNumber output,
             TransitionTableIndex target):
    input_symbol(input), output_symbol(output), target_index(target) {}
  
  Transition():
    input_symbol(NO_SYMBOL_NUMBER),
    output_symbol(NO_SYMBOL_NUMBER),
    target_index(NO_TABLE_INDEX)
  {}
  
  Transition(std::istream& is)
  {
  	is.read(reinterpret_cast<char*>(&input_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&output_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&target_index), sizeof(target_index));
  }

  bool matches(const SymbolNumber s) const;

  TransitionTableIndex target(void) const {return target_index;}
  SymbolNumber get_output(void) const {return output_symbol;}
  SymbolNumber get_input(void) const {return input_symbol;}
  
  virtual bool final(void) const
  {
    return target_index == 1;
  }
};


template <class T>
class Table
{
 protected:
  std::vector<T> table;
  
 public:
  Table(std::istream& is, TransitionTableIndex index_count)
  {
    for(size_t i=0; i<index_count; i++)
      table.push_back(T(is));
  }
    
  const T& operator[](unsigned int i) const
  {
    return (i < TRANSITION_TARGET_TABLE_START) ? 
              table[i] : table[i-TRANSITION_TARGET_TABLE_START];
  }
  
  unsigned int size() const {return table.size();}
};


class AbstractTransducer
{
 protected:
  TransducerHeader header;
  TransducerAlphabet alphabet;
  
  //Subclasses provide the index_table and transition_table
  
  static const TransitionTableIndex START_INDEX = 0;
  
  SymbolNumber* output_string;
  
  virtual std::string process_finals(TokenIOStream& token_stream, const LookupPathVector& finals) const;
  
  virtual void note_analysis(SymbolNumber * whole_output_string) = 0;
  virtual void note_analysis(const LookupPath& path) = 0;
  
  virtual void get_analyses(SymbolNumber * input_symbol,
			    SymbolNumber * output_symbol,
			    SymbolNumber * original_output_string,
			    TransitionTableIndex i) = 0;
  
 public:
  AbstractTransducer(TransducerHeader h, TransducerAlphabet a):
    header(h), alphabet(a),
    output_string((SymbolNumber*)(malloc(2000)))
  {
    for (int i = 0; i < 1000; ++i)
      output_string[i] = NO_SYMBOL_NUMBER;
  }
  
  virtual ~AbstractTransducer()
  {
    free(output_string);
  }
  
  const TransducerAlphabet& get_alphabet() const {return alphabet;}
  
  void analyze(SymbolNumber * input_string)
  {
    get_analyses(input_string,output_string,output_string,START_INDEX);
  }
  
  void analyze_iteratively(SymbolNumber* input_string);
  
  void tokenize(TokenIOStream& token_stream); // for testing
  void run_lookup(TokenIOStream& token_stream);

  virtual void printAnalyses(const std::string prepend) = 0;
  
  virtual bool is_epsilon(const Transition& transition) const
  {
    return transition.matches(0);
  }
  
  /**
   * Create a new lookup path appropriate for initializing a lookup opeation.
   * Subclasses should override this to return the appropriate subclass of
   * LookupPath
   * @return a new lookup path pointing to the beginning of the transducer
   */
  virtual LookupPath* get_initial_path() const = 0;
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const = 0;
  virtual const Transition& get_transition(TransitionTableIndex i) const = 0;
};

class Transducer : public AbstractTransducer
{
 protected:
  Table<TransitionIndex> index_table;
  Table<Transition> transition_table;

  DisplayVector display_vector;
  
  virtual LookupPath* get_initial_path() const;

  virtual void note_analysis(SymbolNumber * whole_output_string);
  virtual void note_analysis(const LookupPath& path);
  
  void try_epsilon_indices(SymbolNumber * input_symbol,
			   SymbolNumber * output_symbol,
			   SymbolNumber * original_output_string,
			   TransitionTableIndex i);
  
  virtual void try_epsilon_transitions(SymbolNumber * input_symbol,
			       SymbolNumber * output_symbol,
			       SymbolNumber * original_output_string,
			       TransitionTableIndex i);
  
  void find_index(SymbolNumber input,
		  SymbolNumber * input_symbol,
		  SymbolNumber * output_symbol,
		  SymbolNumber * original_output_string,
		  TransitionTableIndex i);
  
  void find_transitions(SymbolNumber input,
			SymbolNumber * input_symbol,
			SymbolNumber * output_symbol,
			SymbolNumber * original_output_string,
			TransitionTableIndex i);
  
  virtual void get_analyses(SymbolNumber * input_symbol,
			    SymbolNumber * output_symbol,
			    SymbolNumber * original_output_string,
			    TransitionTableIndex i);

 public:
  Transducer(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    AbstractTransducer(h, a),
    index_table(is, header.index_table_size()),
    transition_table(is, header.target_table_size()),
    display_vector()
  {
  }
  
  virtual void printAnalyses(const std::string prepend);
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const
  {
    return index_table[i];
  }
  virtual const Transition& get_transition(TransitionTableIndex i) const
  {
    return transition_table[i];
  }
};

class TransducerUniq: public Transducer
{
 private:
  DisplaySet display_vector;
  void note_analysis(SymbolNumber * whole_output_string);
  void note_analysis(const LookupPath& path);
 public:
  TransducerUniq(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    Transducer(is, h, a), display_vector() {}
  
  void printAnalyses(std::string prepend);
};

class TransducerFd: public Transducer
{
  FlagDiacriticStateStack statestack;
  OperationVector operations;
  
  virtual LookupPath* get_initial_path() const;

  void try_epsilon_transitions(SymbolNumber * input_symbol,
			       SymbolNumber * output_symbol,
			       SymbolNumber * original_output_string,
			       TransitionTableIndex i);
  
  bool PushState(FlagDiacriticOperation op);

 public:
  TransducerFd(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    Transducer(is, h, a),
    statestack(1, FlagDiacriticState(a.get_state_size(), 0)),
    operations(a.get_operation_vector()) {}
	
	virtual bool is_epsilon(const Transition& transition) const
	{
	  return transition.matches(0) ||
	         (transition.get_input() != NO_SYMBOL_NUMBER &&
	            operations[transition.get_input()].isFlag());
	}
};

class TransducerFdUniq: public TransducerFd
{
 private:
  DisplaySet display_vector;
  void note_analysis(SymbolNumber * whole_output_string);
  void note_analysis(const LookupPath& path);
 public:
  TransducerFdUniq(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    TransducerFd(is, h, a), display_vector() {}
  
  void printAnalyses(std::string prepend);

};

/*
 * BEGIN old transducer-weighted.h
 */

typedef float Weight;
const Weight INFINITE_WEIGHT = static_cast<float>(NO_TABLE_INDEX);

typedef std::multimap<Weight, std::string> DisplayMultiMap;
typedef std::map<std::string, Weight> DisplayMap;

class TransitionWIndex : public TransitionIndex
{
 public:
  TransitionWIndex(SymbolNumber input,
		 TransitionTableIndex first_transition):
	  TransitionIndex(input, first_transition) {}

  TransitionWIndex(std::istream& is): TransitionIndex(is) {}
  
  bool final(void) const
  {
    if (input_symbol != NO_SYMBOL_NUMBER)
      {
        return false;
      }
    return first_transition_index != NO_TABLE_INDEX;
  }
  
  Weight final_weight(void) const
  {
    return static_cast<Weight>(first_transition_index);
  }
};

class TransitionW : public Transition
{
 private:
  Weight transition_weight;

 public:

  // Each transition has an input symbol an output symbol and 
  // a target index, as well as a weight.
  static const size_t SIZE = 
    2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex) + sizeof(Weight);

  TransitionW(SymbolNumber input, SymbolNumber output,
              TransitionTableIndex target, Weight w):
      Transition(input, output, target), transition_weight(w)
  {}

  TransitionW(): Transition(), transition_weight(INFINITE_WEIGHT) {}
  
  TransitionW(std::istream& is): Transition(is)
  {
  	is.read(reinterpret_cast<char*>(&transition_weight), sizeof(Weight));
  }
  
  Weight get_weight(void) const {return transition_weight;}

  bool final(void) const
  {
    if (input_symbol != NO_SYMBOL_NUMBER)
      return false;
    if (output_symbol != NO_SYMBOL_NUMBER)
      return false;
    return transition_weight != INFINITE_WEIGHT;
  }
};


class TransducerW : public AbstractTransducer
{
 protected:
  Table<TransitionWIndex> index_table;
  Table<TransitionW> transition_table;
  
  DisplayMultiMap display_map;
  
  Weight current_weight;
  
  virtual LookupPath* get_initial_path() const;

  virtual void note_analysis(SymbolNumber * whole_output_string);
  virtual void note_analysis(const LookupPath& path);
  
  virtual void try_epsilon_transitions(SymbolNumber * input_symbol,
				       SymbolNumber * output_symbol,
				       SymbolNumber * original_output_string,
				       TransitionTableIndex i);
  
  void try_epsilon_indices(SymbolNumber * input_symbol,
				   SymbolNumber * output_symbol,
				   SymbolNumber * original_output_string,
				   TransitionTableIndex i);

  void find_transitions(SymbolNumber input,
			SymbolNumber * input_symbol,
			SymbolNumber * output_symbol,
			SymbolNumber * original_output_string,
			TransitionTableIndex i);

  void find_index(SymbolNumber input,
		  SymbolNumber * input_symbol,
		  SymbolNumber * output_symbol,
		  SymbolNumber * original_output_string,
		  TransitionTableIndex i);

  void get_analyses(SymbolNumber * input_symbol,
		    SymbolNumber * output_symbol,
		    SymbolNumber * original_output_string,
		    TransitionTableIndex i);

  Weight get_final_index_weight(TransitionTableIndex i) {
    return static_cast<const TransitionWIndex&>(get_index(i)).final_weight();
  }

  Weight get_final_transition_weight(TransitionTableIndex i) {
    return static_cast<const TransitionW&>(get_transition(i)).get_weight();
  }

 public:
  //NOTE: the old TransitionTableReaderW appended two blank transitions to the
  // end of the transitions list. Is that really necessary to recreate here?
  TransducerW(std::istream& is, TransducerHeader h, TransducerAlphabet a) :
    AbstractTransducer(h, a),
    index_table(is, header.index_table_size()),
    transition_table(is, header.target_table_size()),
    display_map(), current_weight(0.0)
  {
  }

  virtual void printAnalyses(const std::string prepend);
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const
  {
    return index_table[i];
  }
  virtual const Transition& get_transition(TransitionTableIndex i) const
  {
    return transition_table[i];
  }
};

class TransducerWUniq: public TransducerW
{
 private:
  DisplayMap display_map;
  void note_analysis(SymbolNumber * whole_output_string);
  void note_analysis(const LookupPath& path);
 public:
  TransducerWUniq(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    TransducerW(is, h, a), display_map() {}
  
  void printAnalyses(std::string prepend);
};

class TransducerWFd: public TransducerW
{
  FlagDiacriticStateStack statestack;
  OperationVector operations;
  
  virtual LookupPath* get_initial_path() const;

  void try_epsilon_transitions(SymbolNumber * input_symbol,
			       SymbolNumber * output_symbol,
			       SymbolNumber * original_output_string,
			       TransitionTableIndex i);

  bool PushState(FlagDiacriticOperation op);

  
 public:
  TransducerWFd(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    TransducerW(is, h, a),
    statestack(1, FlagDiacriticState(a.get_state_size(), 0)),
    operations(a.get_operation_vector()) {}
  
	virtual bool is_epsilon(const Transition& transition) const
	{
	  return transition.matches(0) ||
	         (transition.get_input() != NO_SYMBOL_NUMBER &&
	            operations[transition.get_input()].isFlag());
	}
};

class TransducerWFdUniq: public TransducerWFd
{
 private:
  DisplayMap display_map;
  void note_analysis(SymbolNumber * whole_output_string);
  void note_analysis(const LookupPath& path);
 public:
  TransducerWFdUniq(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    TransducerWFd(is, h, a), display_map() {}
  
  void printAnalyses(std::string prepend);

};

#endif
