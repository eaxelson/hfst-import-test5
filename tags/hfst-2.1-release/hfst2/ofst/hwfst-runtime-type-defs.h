#pragma once
#include <limits.h>

typedef fst::StdArc::StateId StateId;
typedef fst::StdVectorFst TransduceR;
typedef fst::StdArc StdArc;
typedef fst::ArcIterator<TransduceR> ArcIterator;
typedef fst::StateIterator<TransduceR> StateIterator;
typedef fst::TropicalWeight Weight;

const Weight ZERO_WEIGHT = fst::TropicalWeight::One();

typedef unsigned int version_number;
typedef unsigned int byte_order_marker;
typedef unsigned short symbol_number; 
typedef unsigned short symbol_pair_number; 
typedef unsigned int transition_number;
typedef unsigned int truth_value;
typedef unsigned int unicode_symbol;
typedef float transition_penalty;

const byte_order_marker BYTE_ENDIANNESS = 1;
const version_number HFST_VERSION = 1;

const truth_value YES = 1;
const truth_value NO = 0;

const symbol_number TRANSITION_LIST_BEGINNING = USHRT_MAX;
const symbol_pair_number DUMMY_PAIR = 0;
const symbol_number DUMMY_SYMBOL = USHRT_MAX;

const fst::TropicalWeight ZERO = fst::TropicalWeight::Zero();
// There are no final weights in unweighted transducers, so 
// it no index for the transition table is needed. A simple
// 1 or 0 is sufficient for markin finality resp. non-finality 
const transition_number FINALITY_INDICATOR_INDEX = 1;
const transition_number INFINITE_WEIGHT_INDICATOR = 0;
const transition_number EMPTY_TARGET = 0;

// If there are no transitions with some input-symbol, 
// we leave its place empty in the transition index-table.
const symbol_number EMPTY_SYMBOL_NUMBER = 0;

const symbol_pair_number EMPTY_SYMBOL_PAIR = 0;

class TransitionTarget {
 private:
  StateId target_state;
  Weight transition_weight; 
 public:
  TransitionTarget( StdArc a ) :
    target_state(a.nextstate),
    transition_weight(a.weight)
    {

    }
  TransitionTarget( void ) :
    target_state(EMPTY_TARGET),
    transition_weight(ZERO_WEIGHT.Value())
    {

    }

  friend class RuntimeTransition;

};

class RuntimeTransition {
  
 private:
  symbol_pair_number pair_number;
  StateId target_node;
  float transition_weight;

 public:
  RuntimeTransition( symbol_pair_number pair, StateId target, Weight weight = ZERO_WEIGHT):
    pair_number(pair),
    target_node(target),
    transition_weight(weight.Value())
    {};

  RuntimeTransition( symbol_pair_number pair, TransitionTarget t):
    pair_number(pair),
    target_node(t.target_state),
    transition_weight(t.transition_weight.Value())
    {};
  
  RuntimeTransition( void ):
    pair_number(DUMMY_PAIR),
    target_node(EMPTY_TARGET),
    transition_weight(ZERO_WEIGHT.Value())
    {};

  void write( FILE * f, map<StateId,transition_number> &transducer_nodes)
    {
      size_t foo = fwrite(&pair_number,sizeof(pair_number),1,f);
      transition_number node_index = transducer_nodes[target_node];
      foo = fwrite(&node_index,sizeof(node_index),1,f);
      foo = fwrite(&transition_weight,sizeof(transition_weight),1,f);
    }
  void display( map<StateId,transition_number> transducer_nodes)
    {
      fprintf(stderr,"  %u %u\n",pair_number,transducer_nodes[target_node]); 
    }
  symbol_pair_number get_pair(void)
    { return pair_number; }
};

class RuntimeTransitionEntry {
  
 private:
  symbol_pair_number pair_number;
  transition_number target;
  transition_penalty w;

 public:
  RuntimeTransitionEntry( symbol_pair_number pair, transition_number target_number, transition_penalty weight):
    pair_number(pair),
    target(target_number),
    w(weight)
    {};

  symbol_pair_number get_pair( void )
    { return pair_number; } 
  transition_number get_target( void )
    { return target; }
  transition_penalty get_weight( void )
    { return w; }
};

class  RuntimeTransitionIndex {

 private:
  symbol_number input_symbol_number;
  transition_number transition_table_index;

 public:
  RuntimeTransitionIndex( symbol_number symbol, transition_number transition):
    input_symbol_number(symbol),
    transition_table_index(transition)
    {};

  RuntimeTransitionIndex( void ) :
    input_symbol_number(EMPTY_SYMBOL_NUMBER),
    transition_table_index(EMPTY_TARGET)
    {};

  void write(FILE * f) 
    {
      size_t foo = fwrite(&input_symbol_number,sizeof(input_symbol_number),1,f);
      foo = fwrite(&transition_table_index,sizeof(transition_table_index),1,f);
    }
  void display( void ) {
    fprintf(stderr,"  %u %u\n",input_symbol_number,transition_table_index);
  }
  bool is_empty(void)
    { 
      return   (input_symbol_number == EMPTY_SYMBOL_NUMBER) and
	       (transition_table_index == EMPTY_TARGET ); 
    }
  bool matches( symbol_number s )
    { 
      return s == input_symbol_number; 
    }
  symbol_number input_symbol( void ) 
    { return input_symbol_number; }
  bool is_final( void ) 
    { return INFINITE_WEIGHT_INDICATOR != transition_table_index; }
  transition_number get_finality( void ) {
    return transition_table_index;
  }
  transition_number transition_index( void )
    { return transition_table_index; }
};
