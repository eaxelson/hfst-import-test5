#pragma once
#include <limits.h>

typedef unsigned int version_number;
typedef unsigned int byte_order_marker;
typedef unsigned int unicode_symbol;
typedef unsigned short symbol_number; 
typedef unsigned short symbol_pair_number; 
typedef unsigned int transition_number;
typedef unsigned int truth_value;

const byte_order_marker BYTE_ENDIANNESS = 1;
const version_number HFST_VERSION = 1;

const truth_value YES = 1;
const truth_value NO = 0;

const symbol_number TRANSITION_LIST_BEGINNING = USHRT_MAX;
const symbol_pair_number DUMMY_PAIR = USHRT_MAX;
const symbol_number DUMMY_SYMBOL = USHRT_MAX;

// There are no final weights in unweighted transducers, so 
// it no index for the transition table is needed. A simple
// 1 or 0 is sufficient for markin finality resp. non-finality 
const transition_number FINALITY_INDICATOR_INDEX = 1;
const transition_number EMPTY_TARGET = 0;

// If there are no transitions with some input-symbol, 
// we leave its place empty in the transition index-table.
const symbol_number EMPTY_SYMBOL_NUMBER = 0;

const symbol_pair_number EMPTY_SYMBOL_PAIR = 0;

class RuntimeTransition {
  
 private:
  symbol_pair_number pair_number;
  Node * target_node;
  
 public:
  RuntimeTransition( symbol_pair_number pair, Node * target ):
    pair_number(pair),
    target_node(target)
    {};

  RuntimeTransition( void ):
    pair_number(DUMMY_PAIR),
    target_node(EMPTY_TARGET)
    {}

  void write( FILE * f, map<Node*,transition_number> &transducer_nodes)
    {
      fwrite(&pair_number,sizeof(pair_number),1,f);
      transition_number node_index = transducer_nodes[target_node];
      fwrite(&node_index,sizeof(node_index),1,f);
    }
  void display( map<Node*,transition_number> transducer_nodes)
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
  
 public:
  RuntimeTransitionEntry( symbol_pair_number pair, transition_number target_number ):
    pair_number(pair),
    target(target_number)
    {};

  symbol_pair_number get_pair( void )
    { return pair_number; } 
  transition_number get_target( void )
    { return target; }
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
      fwrite(&input_symbol_number,sizeof(input_symbol_number),1,f);
      fwrite(&transition_table_index,sizeof(transition_table_index),1,f);
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
    { return FINALITY_INDICATOR_INDEX == transition_table_index; }
  transition_number transition_index( void )
    { return transition_table_index; }
};

