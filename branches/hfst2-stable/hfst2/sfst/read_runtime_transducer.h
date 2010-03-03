#pragma once

#include <stdio.h>
#include "utf8.h"
#include "fst.h"
#include "runtime_type_defs.h"
#include <iostream>

using std::cerr;

class RuntimeHeaderReader {

 public:
  byte_order_marker byte_endianness;
  version_number version;
  truth_value determinism;
  truth_value minimality;
  truth_value weightedness;
  truth_value cyclicity;
  symbol_number number_of_all_symbols;
  symbol_number number_of_symbols;
  symbol_pair_number number_of_pairs_in_transducer;
  transition_number size_of_index_table;
  transition_number size_of_transition_table;
  transition_number number_of_states;


  RuntimeHeaderReader( FILE * f ) {
    size_t foo;
    foo =  fread(&number_of_symbols,sizeof(number_of_symbols),1,f);    
    fprintf(stderr,"NUMBER OF INPUT SYMBOLS %u\n",number_of_symbols);    
    foo =  fread(&number_of_all_symbols,sizeof(number_of_all_symbols),1,f);    
    fprintf(stderr,"NUMBER OF ALL SYMBOLS %u\n",number_of_all_symbols);
    foo =  fread(&size_of_index_table,sizeof(size_of_index_table),1,f);
    fprintf(stderr,"SIZE OF INDEX TABLE %u\n",size_of_index_table);    
    foo =  fread(&size_of_transition_table,sizeof(size_of_transition_table),1,f);    
    fprintf(stderr,"SIZE OF TRANSITION TABLE %u\n",size_of_transition_table);    
    foo =  fread(&number_of_states,sizeof(number_of_states),1,f);
    fprintf(stderr,"NUMBER OF STATES %u\n",number_of_states);    
    foo =  fread(&weightedness,sizeof(weightedness),1,f);
    fprintf(stderr,"WEIGHTEDNESS %u\n",weightedness);    
    foo =  fread(&determinism,sizeof(determinism),1,f);
    fprintf(stderr,"DETERMINISM %u\n",determinism);    
    foo =  fread(&version,sizeof(version),1,f);
    size_t prop;
    foo =  fread(&prop,sizeof(prop),1,f);
    fprintf(stderr,"PROP %u\n",weightedness);    
    fprintf(stderr,"MINIMALITY %u\n",minimality);    
    foo =  fread(&cyclicity,sizeof(cyclicity),1,f);
    fprintf(stderr,"CYCLICITY %u\n",cyclicity);    
    foo =  fread(&prop,sizeof(prop),1,f);
    fprintf(stderr,"PROP %u\n",weightedness);    
    foo =  fread(&prop,sizeof(prop),1,f);
    fprintf(stderr,"PROP %u\n",weightedness);    
    foo =  fread(&prop,sizeof(prop),1,f);
    fprintf(stderr,"PROP %u\n",weightedness);    
  }

};

class AlphabetReader {

 private:
  symbol_number amount_of_all_symbols;
  symbol_number amount_of_input_symbols;
  unicode_symbol * unicode_symbols;
  symbol_number * symbol_table;
  symbol_pair_number amount_of_transducer_pairs;
  symbol_number * pair_table;
  map<symbol_pair_number,symbol_number> pairs_by_input_number;
  map<symbol_pair_number,Label> labels;
  
  void display_unicode_symbols(void);
  void add_symbols(Alphabet &alpha);
  void add_pairs(Alphabet &alpha);

 public:

  AlphabetReader(Alphabet &alpha,
		 FILE * f,
		 RuntimeHeaderReader &header ):
    amount_of_all_symbols(header.number_of_all_symbols),
    amount_of_input_symbols(header.number_of_symbols),
    amount_of_transducer_pairs(header.number_of_pairs_in_transducer)
    {
      size_t foo;
      unicode_symbols = (unicode_symbol*)(malloc(amount_of_all_symbols*sizeof(unicode_symbol)));
      foo =  fread(unicode_symbols,amount_of_all_symbols*sizeof(unicode_symbol),1,f);
      symbol_table = (Character*)(malloc(amount_of_input_symbols*sizeof(Character)));
      foo =  fread(symbol_table,amount_of_input_symbols*sizeof(Character),1,f);
      pair_table = (symbol_number*)(malloc(2*amount_of_transducer_pairs*sizeof(symbol_number)));
      foo =  fread(pair_table,2*amount_of_transducer_pairs*sizeof(symbol_number),1,f);
      
      display_unicode_symbols();
      add_symbols(alpha);
      add_pairs(alpha);
    }

  ~AlphabetReader( void )
    {
    }

  symbol_number symbol(symbol_number index) { return *(symbol_table + index); }
  symbol_number number_of_input_symbols( void )
    { return amount_of_input_symbols; }
  symbol_number get_input(symbol_pair_number n) 
    { return pairs_by_input_number[n]; }
  Label get_label(symbol_pair_number num)
    { return labels[num]; }
};

class RuntimeTransitionTableReader {

  size_t index_table_element_size;
  transition_number number_of_index_table_elements;
  size_t transition_table_element_size;
  transition_number number_of_transition_table_elements;
  symbol_number size_of_index_table;					       
  void * index_table;
  void * transition_table;
  size_t index_table_size;
  size_t transition_table_size;
  vector<RuntimeTransitionIndex*> indices;
  vector<RuntimeTransitionEntry*> transitions;
  map<transition_number,Node*> states;
  AlphabetReader alpha_reader;

  bool matching_transition( RuntimeTransitionIndex * i, RuntimeTransitionEntry * t );
  void read_transitions(Transducer * t, AlphabetReader &alphabet_reader);
  void write_transitions( Transducer * t,
			  transition_number i );
  void get_transitions(Transducer * t,
		       transition_number origin,
		       RuntimeTransitionIndex * index);
    
 public:
  RuntimeTransitionTableReader( Transducer * t, 
				FILE * f,
				RuntimeHeaderReader &header,
				AlphabetReader &alphabet_reader):
    number_of_index_table_elements(header.size_of_index_table),
    number_of_transition_table_elements(header.size_of_transition_table),
    size_of_index_table(alphabet_reader.number_of_input_symbols()),
    alpha_reader(alphabet_reader)
    {
      size_t foo;
      index_table_element_size = sizeof(symbol_number)+sizeof(transition_number);
      index_table_size = number_of_index_table_elements*index_table_element_size;
      index_table = malloc(index_table_size);
      foo =  fread(index_table,index_table_size,1,f);

      transition_table_element_size = sizeof(symbol_pair_number)+sizeof(transition_number);
      transition_table_size = number_of_transition_table_elements*transition_table_element_size;
      transition_table = malloc(transition_table_size);
      foo =  fread(transition_table,transition_table_size,1,f);

      read_transitions(t,alphabet_reader);
    }
};

class RuntimeReader {

 private:
  FILE * input_file;
  Transducer * t;
  RuntimeHeaderReader header;
  Alphabet alpha;
  
 public:
  RuntimeReader( FILE * f ):
    input_file(f),
    t(new Transducer()),
    header(f)
    {
      AlphabetReader alphabet_reader(t->alphabet,input_file,header);
      //      t->alphabet.copy(alpha);
      RuntimeTransitionTableReader table_reader(t,f,header,alphabet_reader);
    }
    Transducer * get_result(void)
    { return t; }
};
