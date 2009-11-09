#include <map>
#include <set>
#include <vector>
#include <stdio.h>


using std::vector;
using std::map;
using std::set;

#include "fst.h"

#include "runtime_type_defs.h"

class SymbolPairNumbering;
class RuntimeState;
class RuntimeHeader;
class RuntimeTransitionIndexTable;
class RuntimeTransducerWriter;

class SymbolPairNumbering {

  struct compare_labels {
    bool operator() ( const Label &l1, const Label &l2 ) {
      if ( l1.lower_char() == l2.lower_char() )
	return l1.upper_char() < l2.upper_char();
      return l1.lower_char() < l2.lower_char();
    }
  };
 
 private:

  set<Character> input_symbols;
  map<Character,unsigned int> character_codes;
  Character next_dense;
  map<Character,Character> dense_symbol_numbers;
  map<Label,symbol_pair_number,compare_labels> symbol_pair_numbers;
  map<symbol_pair_number,Character> inputs; 
  map<Character,transition_number> transition_indices;
  symbol_pair_number next_number;

 public:

  SymbolPairNumbering( void ):
    next_number(1)
  {}

  SymbolPairNumbering(Transducer * tr ):
    next_dense(1),
    next_number(1)
  {
    input_symbols.insert(0);
    character_codes[0] = 0;
    // tentative:
    dense_symbol_numbers[0] = 0;

    for ( Alphabet::const_iterator it = tr->alphabet.begin();
	  it != tr->alphabet.end();
	  ++it )
      get_pair_number(*it);
  }

  symbol_pair_number get_pair_number( const Label &l );
  symbol_number number_of_input_symbols( void )
    { return inputs.size(); }
  symbol_number number_of_all_input_symbols( void )
    { return input_symbols.size(); }
  symbol_number number_of_symbols( void )
    { return character_codes.size(); }
  symbol_pair_number size( void ) 
    { return symbol_pair_numbers.size(); }
  Character input_symbol(symbol_pair_number pair_number) 
    { return inputs[ pair_number ]; } 
  Character input_symbol_order( symbol_pair_number pair_number ) 
    {
      Character c = inputs[pair_number];
      return dense_symbol_numbers[c];
    }
  void write( FILE * f );
};

class RuntimeState {

 private:

  Node * transducer_state;
  SymbolPairNumbering * symbol_pair_numbering;
  map<symbol_pair_number,vector<Node*>*> transitions;
  map<symbol_number,transition_number> transition_indices;
  vector<RuntimeTransitionIndex*> state_transition_indices;
 
  void read_transitions( vector<RuntimeTransition*> &transducer_transitions );

 public:

  RuntimeState( Node * n, SymbolPairNumbering * pair_numbers, vector<RuntimeTransition*> &transducer_transitions ):
    transducer_state(n),
    symbol_pair_numbering(pair_numbers)
    {
      read_transitions(transducer_transitions);
    }

  symbol_pair_number size( void )
    { return transition_indices.size(); }
  Node * get_node(void)
    { return transducer_state; }
  truth_value get_finality( void );
  void make_state_transition_index_table( void );
  vector<RuntimeTransitionIndex*> &get_transition_indices( void )
    { return state_transition_indices; }
};

class RuntimeHeader {

 private:
  byte_order_marker byte_endianness;
  version_number version;
  truth_value determinism;
  truth_value minimality;
  truth_value weightedness;
  truth_value cyclicity;
  symbol_number number_of_unicode_symbols;
  symbol_number number_of_symbols;
  symbol_pair_number number_of_pairs_in_transducer;
  transition_number size_of_index_table;
  transition_number size_of_transition_table;

 public:
  RuntimeHeader( Transducer * t ) :
    byte_endianness(BYTE_ENDIANNESS),
    version(HFST_VERSION),
    determinism(t->is_deterministic()),
    minimality(t->is_deterministic()),
    weightedness(NO),
    cyclicity(t->is_cyclic())
    {}
  
  void set_number_of_symbol_pairs( SymbolPairNumbering &s )
    { 
      number_of_unicode_symbols = s.number_of_symbols();      
      number_of_pairs_in_transducer = s.size(); 
      number_of_symbols = s.number_of_all_input_symbols();
    }  
  void set_size_of_transition_tables( transition_number index_table_size,
				      transition_number transition_table_size)
    { 
      size_of_transition_table = transition_table_size; 
      size_of_index_table = index_table_size; 
    } 
  void write( FILE * f );
};

class TransitionIndexTable {
  
 private:
  vector<RuntimeTransitionIndex*> indices;
  set<transition_number> transition_list_start_indices;

  transition_number floor;
  symbol_number alphabet_size;

  void empty_write(FILE * f)
    { 
      fwrite(&EMPTY_SYMBOL_NUMBER,sizeof(EMPTY_SYMBOL_NUMBER),1,f);
      fwrite(&EMPTY_TARGET,sizeof(EMPTY_TARGET),1,f);
    }
  void empty_display( void ) 
    {
      fprintf(stderr," 0 0 \n");
    }
  void more_space( void )
    { 
      for ( int i = 0; i < alphabet_size+2; ++i )
	indices.push_back(new RuntimeTransitionIndex() );
    }
  bool node_fits( RuntimeState * s, transition_number index );
  void insert_node( RuntimeState * s, transition_number index );
  void add_state( RuntimeState * s,   map<Node*,transition_number> &transducer_nodes);

 public:
  TransitionIndexTable( RuntimeState * root, 
			map<transition_number, vector<RuntimeState*>* > &transitions,  
			symbol_number number_of_input_symbols,
			map<Node*,transition_number> &transducer_nodes):
    floor(0), 
    alphabet_size(number_of_input_symbols)
    { 
      more_space();
      add_state(root,transducer_nodes);
      for( map<transition_number,vector<RuntimeState*>* >::reverse_iterator it = transitions.rbegin(); 
	   it != transitions.rend();
	   ++it )
	{
	  for ( vector<RuntimeState*>::iterator jt = (it->second)->begin();
		jt != (it->second)->end();
		++jt )
	    {
	      add_state(*jt,transducer_nodes);
	      delete *jt;
	    }
	  delete (it->second);
	}
    }
  transition_number size( void )
    { return indices.size(); }

  void write(FILE * f);
};

class RuntimeTransitionIndexTable {
 private:
  transition_number floor;
  SymbolPairNumbering symbol_pair_numbering;
  RuntimeState* root;
  map<transition_number,vector<RuntimeState*>* > states;
  map<Node*,transition_number> transducer_nodes;
  TransitionIndexTable * index_table;

 public:

  RuntimeTransitionIndexTable(SymbolPairNumbering &pair_numbers ):
    floor(0),
    symbol_pair_numbering(pair_numbers)
    {}
  void add_runtime_state( RuntimeState * s )
    { 
      transition_number s_size = s->size();
      if ( states.find(s_size) == states.end() )
	states[ s_size ] = new vector<RuntimeState*>;
      states[ s_size ]->push_back(s);
    }
  void add_first_state( RuntimeState * s ) 
    { root = s; }

  map<Node*,transition_number> &get_transducer_nodes( void )
    { return transducer_nodes; }
  
  transition_number make_table( void );
  void write( FILE * f );
  
};

class RuntimeTransducerWriter {

 private:
  Transducer * regular_transducer;
  FILE * output_file;
  RuntimeHeader transducer_header;
  vector<RuntimeTransition*> transition_table;
  SymbolPairNumbering symbol_pair_numbers;
  RuntimeTransitionIndexTable transition_index_table;

  void add_runtime_state( RuntimeState * s );
  void add_root( RuntimeState * s );
  void convert_states( void );
  void write_transitions( void );

 public:

  RuntimeTransducerWriter( Transducer * tr, FILE * f):
    regular_transducer(tr),
    output_file(f),
    transducer_header(tr),
    symbol_pair_numbers(tr),
    transition_index_table(symbol_pair_numbers)
    {
      convert_states();

      transducer_header.set_number_of_symbol_pairs(symbol_pair_numbers);
      
      transition_number number_of_indices = transition_index_table.make_table();
      transition_number number_of_transitions = transition_table.size();
      transducer_header.set_size_of_transition_tables(number_of_indices,number_of_transitions);

      transducer_header.write(f);

      symbol_pair_numbers.write(f);
      
      transition_index_table.write(f);
      write_transitions();

    }
  
};
