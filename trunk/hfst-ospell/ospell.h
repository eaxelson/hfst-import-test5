/* -*- Mode: C++ -*- */
// Copyright 2010 University of Helsinki
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef HFST_OSPELL_OSPELL_H_
#define HFST_OSPELL_OSPELL_H_ 1

#include <string>
#include <deque>
#include <queue>
#include <stdexcept>
#include "hfst-ol.h"

namespace hfst_ol {

class STransition{
public:
    TransitionTableIndex index;
    SymbolNumber symbol;
    Weight weight;

    STransition(TransitionTableIndex i,
                SymbolNumber s):
        index(i),
        symbol(s),
        weight(0.0)
        {}

    STransition(TransitionTableIndex i,
                SymbolNumber s,
                Weight w):
        index(i),
        symbol(s),
        weight(w)
        {}

};
typedef std::pair<std::string, std::string> StringPair;
typedef std::pair<std::string, Weight> StringWeightPair;
typedef std::pair<std::pair<std::string, std::string>, Weight>
                                                        StringPairWeightPair;
class StringWeightComparison
/* results are reversed by default because greater weights represent
   worse results - to reverse the reversal, give a true argument*/

{
    bool reverse;
public:
    StringWeightComparison(bool reverse_result=false):
        reverse(reverse_result)
        {}
    
    bool operator() (StringWeightPair lhs, StringWeightPair rhs);
};

class StringPairWeightComparison
{
    bool reverse;
public:
    StringPairWeightComparison(bool reverse_result=false):
        reverse(reverse_result)
        {}
    
    bool operator() (StringPairWeightPair lhs, StringPairWeightPair rhs);
};

typedef std::priority_queue<StringWeightPair,
                            std::vector<StringWeightPair>,
                            StringWeightComparison> CorrectionQueue;
typedef std::priority_queue<StringWeightPair,
                            std::vector<StringWeightPair>,
                            StringWeightComparison> AnalysisQueue;
typedef std::priority_queue<StringWeightPair,
                            std::vector<StringWeightPair>,
                            StringWeightComparison> HyphenationQueue;
typedef std::priority_queue<StringPairWeightPair,
                            std::vector<StringPairWeightPair>,
                            StringPairWeightComparison> AnalysisCorrectionQueue;
typedef std::priority_queue<Weight> WeightQueue;


class Transducer
{
protected:
    TransducerHeader header;
    TransducerAlphabet alphabet;
    KeyTable * keys;
    Encoder encoder;

    static const TransitionTableIndex START_INDEX = 0;
  
    std::vector<const char*> symbol_table;
  
    void set_symbol_table(void);

 
public:
    Transducer(FILE * f);
    Transducer(char * raw);
    AnalysisQueue lookup(char * line);
    IndexTable indices;
    TransitionTable transitions;
    bool final_transition(TransitionTableIndex i);
    bool final_index(TransitionTableIndex i);
    KeyTable * get_key_table(void);
    SymbolNumber find_next_key(char ** p);
    Encoder * get_encoder(void);
    unsigned int get_state_size(void);
    std::vector<const char*> * get_symbol_table(void);
    SymbolNumber get_other(void);
    TransducerAlphabet * get_alphabet(void);
    OperationMap * get_operations(void);
    STransition take_epsilons(const TransitionTableIndex i) const;
    STransition take_epsilons_and_flags(const TransitionTableIndex i);
    STransition take_non_epsilons(const TransitionTableIndex i,
                                  const SymbolNumber symbol) const;
    TransitionTableIndex next(const TransitionTableIndex i,
                              const SymbolNumber symbol) const;
    TransitionTableIndex next_e(const TransitionTableIndex i) const;
    bool has_transitions(const TransitionTableIndex i,
                         const SymbolNumber symbol) const;
    bool has_epsilons_or_flags(const TransitionTableIndex i);
    bool is_final(const TransitionTableIndex i);
    Weight final_weight(const TransitionTableIndex i) const;
    bool is_flag(const SymbolNumber symbol);
    bool is_weighted(void);

};

class TreeNode
{
public:
    SymbolVector string;
    unsigned int input_state;
    TransitionTableIndex mutator_state;
    TransitionTableIndex lexicon_state;
    FlagDiacriticState flag_state;
    Weight weight;

    TreeNode(SymbolVector prev_string,
             unsigned int i,
             TransitionTableIndex mutator,
             TransitionTableIndex lexicon,
             FlagDiacriticState state,
             Weight w):
        string(prev_string),
        input_state(i),
        mutator_state(mutator),
        lexicon_state(lexicon),
        flag_state(state),
        weight(w)
        { }

    TreeNode(FlagDiacriticState start_state): // starting state node
    string(SymbolVector()),
    input_state(0),
    mutator_state(0),
    lexicon_state(0),
    flag_state(start_state),
    weight(0.0)
        { }

    bool try_compatible_with(FlagDiacriticOperation op);

    TreeNode update_lexicon(SymbolNumber next_symbol,
                            TransitionTableIndex next_lexicon,
                            Weight weight);

    TreeNode update_mutator(SymbolNumber next_symbol,
                            TransitionTableIndex next_mutator,
                            Weight weight);

    void increment_mutator(void);

    TreeNode update(SymbolNumber next_symbol,
                    unsigned int next_input,
                    TransitionTableIndex next_mutator,
                    TransitionTableIndex next_lexicon,
                    Weight weight);

    TreeNode update(SymbolNumber next_symbol,
                    TransitionTableIndex next_mutator,
                    TransitionTableIndex next_lexicon,
                    Weight weight);


};

typedef std::deque<TreeNode> TreeNodeQueue;

int nByte_utf8(unsigned char c);

class InputString
{
  
private:
    SymbolVector s;

public:
    InputString():
        s(SymbolVector())
        { }

    bool initialize(Encoder * encoder, char * input, SymbolNumber other);
    
    unsigned int len(void);

    SymbolNumber operator[](unsigned int i)
        {
            return s[i];
        }

};

class AlphabetTranslationException: public std::runtime_error
{ // "what" should hold the first untranslatable symbol
public:
    
    AlphabetTranslationException(const std::string what):
        std::runtime_error(what)
        { }
};

class Speller
{
public:
    Transducer * mutator;
    Transducer * lexicon;
    InputString input;
    TreeNodeQueue queue;
    TreeNode next_node;
    SymbolVector alphabet_translator;
    OperationMap * operations;
    std::vector<const char*> * symbol_table;
    
    Speller(Transducer * mutator_ptr, Transducer * lexicon_ptr);
    bool init_input(char * str, Encoder * encoder, SymbolNumber other);
    SymbolNumber get_state_size(void);
    void build_alphabet_translator(void);
    void lexicon_epsilons(void);
    void mutator_epsilons(void);
    void consume_input(void);
    void lexicon_consume(void);
    bool check(char * line);
    CorrectionQueue correct(char * line, int nbest = 0);
    AnalysisQueue analyse(char * line, int nbest = 0);
};

std::string stringify(std::vector<const char *> * symbol_table,
                      SymbolVector & symbol_vector);


} // namespace hfst_ol

// Some platforms lack strndup
char* hfst_strndup(const char* s, size_t n);
    
#endif // HFST_OSPELL_OSPELL_H_
