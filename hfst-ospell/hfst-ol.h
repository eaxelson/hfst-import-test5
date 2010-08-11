/* -*- Mode: C++ -*- */

/*
 * This file contains some classes, typedefs and constant common to all
 * hfst-optimized-lookup stuff. This is just to get them out of the way
 * of the actual ospell code.
 */

#ifndef HFST_OSPELL_HFST_OL_H_
#define HFST_OSPELL_HFST_OL_H_

#include <vector>
#include <map>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <set>
#include <utility>

namespace hfst_ol {

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef std::vector<SymbolNumber> SymbolVector;
typedef std::vector<std::string> KeyTable;
typedef std::map<std::string, SymbolNumber> StringSymbolMap;
typedef short ValueNumber;
typedef float Weight;

// Forward declarations to typedef some more containers
class TransitionIndex;
class Transition;
class FlagDiacriticOperation;

typedef std::vector<TransitionIndex*> TransitionIndexVector;
typedef std::vector<Transition*> TransitionVector;
typedef std::map<SymbolNumber, FlagDiacriticOperation> OperationMap;

const SymbolNumber NO_SYMBOL = USHRT_MAX;
const TransitionTableIndex NO_TABLE_INDEX = UINT_MAX;
const Weight INFINITE_WEIGHT = static_cast<float>(NO_TABLE_INDEX);
const unsigned int MAX_SYMBOL_BYTES = 1000;

// This is 2^31, hopefully equal to UINT_MAX/2 rounded up.
// For some profound reason it can't be replaced with (UINT_MAX+1)/2.
const TransitionTableIndex TARGET_TABLE = 2147483648u;

// the flag diacritic operators as given in
// Beesley & Karttunen, Finite State Morphology (U of C Press 2003)
enum FlagDiacriticOperator {P, N, R, D, C, U};

enum HeaderFlag {Weighted, Deterministic, Input_deterministic, Minimized,
                 Cyclic, Has_epsilon_epsilon_transitions,
                 Has_input_epsilon_transitions, Has_input_epsilon_cycles,
                 Has_unweighted_input_epsilon_cycles};
    
class HeaderParsingException: public std::exception
{
public:
    virtual const char* what() const throw()
	{ return("Parsing error while reading header"); }
};
    
    
class AlphabetParsingException: public std::exception
{
public:
    virtual const char* what() const throw()
	{ return("Parsing error while reading alphabet"); }
};

class IndexTableReadingException: public std::exception
{
public:
    virtual const char* what() const throw()
	{ return("Error while reading index table"); }
};

class TransitionTableReadingException: public std::exception
{
public:
    virtual const char* what() const throw()
	{ return("Error while reading transition table"); }
};

class UnweightedSpellerException: public std::exception
{
public:
    virtual const char* what() const throw()
	{ return("Transducers for use in spellers must be weighted"); }
};

class TransducerHeader
{
private:
    SymbolNumber number_of_symbols;
    SymbolNumber number_of_input_symbols;
    TransitionTableIndex size_of_transition_index_table;
    TransitionTableIndex size_of_transition_target_table;

    TransitionTableIndex number_of_states;
    TransitionTableIndex number_of_transitions;

    bool weighted;
    bool deterministic;
    bool input_deterministic;
    bool minimized;
    bool cyclic;
    bool has_epsilon_epsilon_transitions;
    bool has_input_epsilon_transitions;
    bool has_input_epsilon_cycles;
    bool has_unweighted_input_epsilon_cycles;

    void read_property(bool &property, FILE * f)
	{
            unsigned int prop;
            if (fread(&prop,sizeof(unsigned int),1,f) != 1) {
                throw HeaderParsingException();
            }
            if (prop == 0)
            {
                property = false;
                return;
            }
            else
            {
                property = true;
                return;
            }
	}

public:
    TransducerHeader(FILE * f)
	{
            /* The following conditional clause does all the numerical reads
               and throws an exception if any fails to return 1 */
            if (fread(&number_of_input_symbols,
                      sizeof(SymbolNumber),1,f) != 1 or
                fread(&number_of_symbols,
                      sizeof(SymbolNumber),1,f) != 1 or
                fread(&size_of_transition_index_table,
                      sizeof(TransitionTableIndex),1,f) != 1 or
                fread(&size_of_transition_target_table,
                      sizeof(TransitionTableIndex),1,f) != 1 or
                fread(&number_of_states,
                      sizeof(TransitionTableIndex),1,f) != 1 or
                fread(&number_of_transitions,
                      sizeof(TransitionTableIndex),1,f) != 1) {
                throw HeaderParsingException();
            }

            read_property(weighted,f);

            read_property(deterministic,f);
            read_property(input_deterministic,f);
            read_property(minimized,f);
            read_property(cyclic,f);
            read_property(has_epsilon_epsilon_transitions,f);
            read_property(has_input_epsilon_transitions,f);
            read_property(has_input_epsilon_cycles,f);
            read_property(has_unweighted_input_epsilon_cycles,f);

            // For ospell: demand weightedness FIXME: do this somewhere else
            if (!weighted) {
                throw UnweightedSpellerException();
            }
	}

    SymbolNumber symbol_count(void)
	{ return number_of_symbols; }

    SymbolNumber input_symbol_count(void)
	{ return number_of_input_symbols; }
  
    TransitionTableIndex index_table_size(void)
	{ return size_of_transition_index_table; }

    TransitionTableIndex target_table_size(void)
	{ return size_of_transition_target_table; }

    bool probe_flag(HeaderFlag flag)
	{
            switch (flag) {
            case Weighted:
                return weighted;
            case Deterministic:
                return deterministic;
            case Input_deterministic:
                return input_deterministic;
            case Minimized:
                return minimized;
            case Cyclic:
                return cyclic;
            case Has_epsilon_epsilon_transitions:
                return has_epsilon_epsilon_transitions;
            case Has_input_epsilon_transitions:
                return has_input_epsilon_transitions;
            case Has_input_epsilon_cycles:
                return has_input_epsilon_cycles;
            case Has_unweighted_input_epsilon_cycles:
                return has_unweighted_input_epsilon_cycles;
            }
            return false;
	}
};

class FlagDiacriticOperation
{
private:
    const FlagDiacriticOperator operation;
    const SymbolNumber feature;
    const ValueNumber value;
public:
    FlagDiacriticOperation(const FlagDiacriticOperator op,
			   const SymbolNumber feat,
			   const ValueNumber val):
	operation(op), feature(feat), value(val) {}

    // dummy constructor
    FlagDiacriticOperation():
	operation(P), feature(NO_SYMBOL), value(0) {}
  
    bool isFlag(void) const { return feature != NO_SYMBOL; }
    FlagDiacriticOperator Operation(void) const { return operation; }
    SymbolNumber Feature(void) const { return feature; }
    ValueNumber Value(void) const { return value; }

};

class TransducerAlphabet
{
private:
    KeyTable * kt;
    OperationMap * operations;
    SymbolNumber other_symbol;
    SymbolNumber flag_state_size;
    StringSymbolMap * string_to_symbol;
    
    void read(FILE * f, SymbolNumber number_of_symbols);
    
public:
    TransducerAlphabet(FILE * f, SymbolNumber number_of_symbols):
	kt(new KeyTable),
	operations(new OperationMap),
	other_symbol(NO_SYMBOL),
	string_to_symbol(new StringSymbolMap)
	{
	    read(f, number_of_symbols);
	}

    KeyTable * get_key_table(void)
	{ return kt; }
    
    OperationMap * get_operation_map(void)
	{ return operations; }
    
    SymbolNumber get_state_size(void)
	{ return flag_state_size; }
    
    SymbolNumber get_other(void)
	{
            return other_symbol;
	}

    StringSymbolMap * get_string_to_symbol(void)
	{
            return string_to_symbol;
	}

    bool is_flag(SymbolNumber symbol)
	{
            return operations->count(symbol) == 1;
	}
  
};

class LetterTrie;
typedef std::vector<LetterTrie*> LetterTrieVector;

class LetterTrie
{
private:
    LetterTrieVector letters;
    SymbolVector symbols;

public:
    LetterTrie(void):
	letters(UCHAR_MAX, static_cast<LetterTrie*>(NULL)),
	symbols(UCHAR_MAX,NO_SYMBOL)
	{}

    void add_string(const char * p,SymbolNumber symbol_key);

    SymbolNumber find_key(char ** p);

};

class Encoder {

private:
    LetterTrie letters;
    SymbolVector ascii_symbols;

    void read_input_symbols(KeyTable * kt, SymbolNumber number_of_input_symbols);

public:
    Encoder(KeyTable * kt, SymbolNumber number_of_input_symbols):
	ascii_symbols(UCHAR_MAX,NO_SYMBOL)
	{
            read_input_symbols(kt, number_of_input_symbols);
	}
  
    SymbolNumber find_key(char ** p);
};

typedef std::vector<ValueNumber> FlagDiacriticState;

class TransitionIndex
{
protected:
    SymbolNumber input_symbol;
    TransitionTableIndex first_transition_index;
  
public:
  
    // Each TransitionIndex has an input symbol and a target index.
    static const size_t SIZE = 
	sizeof(SymbolNumber) + sizeof(TransitionTableIndex);

    TransitionIndex(const SymbolNumber input,
		    const TransitionTableIndex first_transition):
	input_symbol(input),
	first_transition_index(first_transition)
	{}

    TransitionTableIndex target(void) const
	{
            return first_transition_index;
	}
  
    bool final(void) const
	{
            if (input_symbol != NO_SYMBOL)
            {
                return false;
            }
            return first_transition_index != NO_TABLE_INDEX;
	}

    Weight final_weight(void) const
	{
            return static_cast<Weight>(first_transition_index);
	}
  
    SymbolNumber get_input(void) const
	{
            return input_symbol;
	}
};

class Transition
{
protected:
    SymbolNumber input_symbol;
    SymbolNumber output_symbol;
    TransitionTableIndex target_index;
    Weight transition_weight;

public:

    // Each transition has an input symbol, an output symbol and 
    // a target index.
    static const size_t SIZE = 
	2 * sizeof(SymbolNumber) + sizeof(TransitionTableIndex) + sizeof(Weight);

    Transition(const SymbolNumber input,
	       const SymbolNumber output,
	       const TransitionTableIndex target,
	       const Weight w):
	input_symbol(input),
	output_symbol(output),
	target_index(target),
	transition_weight(w)
	{}

    Transition():
	input_symbol(NO_SYMBOL),
	output_symbol(NO_SYMBOL),
	target_index(NO_TABLE_INDEX),
	transition_weight(INFINITE_WEIGHT)
	{}

    TransitionTableIndex target(void) const
	{
            return target_index;
	}

    SymbolNumber get_output(void) const
	{
            return output_symbol;
	}

    SymbolNumber get_input(void) const
	{
            return input_symbol;
	}

    Weight get_weight(void) const
	{
            return transition_weight;
	}

    bool final(void) const
	{
            if (input_symbol != NO_SYMBOL)
                return false;
            if (output_symbol != NO_SYMBOL)
                return false;
            return transition_weight != INFINITE_WEIGHT;
	}
};

class IndexTableReader
{
private:
    TransitionIndexVector indices;
  
    void read(FILE * f,
	      TransitionTableIndex number_of_table_entries);
public:
    IndexTableReader(FILE * f,
		     TransitionTableIndex number_of_table_entries)
	{
	    read(f, number_of_table_entries);
	}
    
    TransitionIndexVector &operator() (void)
	{ return indices; }
};

class TransitionTableReader
{
protected:
    TransitionTableIndex number_of_table_entries;
    char * TableTransitions;
    TransitionVector transitions;
    size_t table_size;
    size_t transition_size;
  
    void get_transition_vector(void);
public:
    TransitionTableReader(FILE * f,
                          TransitionTableIndex transition_count):
	number_of_table_entries(transition_count)
	{
            table_size = number_of_table_entries*Transition::SIZE;
            TableTransitions = (char*)(malloc(table_size));
            if (fread(TableTransitions,table_size,1,f) != 1) {
                throw TransitionTableReadingException();
            }
            get_transition_vector();
	    free(TableTransitions);
	}
  
    TransitionVector &operator() (void)
	{ 
            return transitions; 
	}
};

} // namespace hfst_ol
    
#endif // HFST_OSPELL_HFST_OL_H_
