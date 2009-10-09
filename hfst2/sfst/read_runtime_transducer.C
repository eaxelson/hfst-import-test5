#include "read_runtime_transducer.h"

void AlphabetReader::display_unicode_symbols(void) {
  for( unsigned int i = 0; i < amount_of_all_symbols; ++i ) {
    //fprintf(stderr,"UNICODE SYMBOL %u\n",*(unicode_symbols+i));
  }
}

void AlphabetReader::add_symbols(Alphabet &alpha) 
{
  alpha.add_symbol("<>");
  for ( size_t s = 0; 
	s < amount_of_input_symbols;
	++s )
    {
      char * symbol = (char*)(malloc(10));
      sprintf(symbol,"%u",symbol_table[s]);
      alpha.add_symbol( symbol );
    }
}

void AlphabetReader::add_pairs(Alphabet &alpha) 
{
  symbol_pair_number counter = 1;
  for ( symbol_number s = 0;
	s < amount_of_transducer_pairs*2;
	s += 2 ) {
    Character c1 = pair_table[s];
    Character c2 = pair_table[s+1];
    //fprintf(stderr,"%u %u\n",c1,c2);
    alpha.insert( Label(c1,c2) );
    pairs_by_input_number[counter] = c1;
    labels[counter] = Label(c1,c2);
    //labels[counter] = Label(symbol(c1),c2);
    ++counter;
    }
}

void RuntimeTransitionTableReader::read_transitions(Transducer * t,AlphabetReader &alphabet_reader) 
{


  for ( char * p = (char*)(index_table); 
	p != (char*)(index_table) + index_table_size; 
	p += index_table_element_size ) {

    symbol_number input_symbol = *((symbol_number*)(p));
    transition_number index = *((transition_number*)(p+sizeof(symbol_number)));
    indices.push_back(new RuntimeTransitionIndex(input_symbol,index));
    }

  for ( char * p = (char*)(transition_table); 
	p != (char*)(transition_table) + transition_table_size; 
	p += transition_table_element_size ) 
    {      
      symbol_pair_number pair = *((symbol_pair_number*)(p));
      transition_number transition = *((transition_number*)(p+sizeof(symbol_number)));
      transitions.push_back(new RuntimeTransitionEntry(pair,transition));
    }
  transition_number start_state = 0;
  states[start_state] = t->root_node();
  
  write_transitions(t,start_state);
}

void RuntimeTransitionTableReader::write_transitions(Transducer * t,
						     transition_number i)
{
  RuntimeTransitionIndex * index = indices.at(i);
  if ( index->is_final() )
    states[i]->set_final(1);
  for ( symbol_number s = 0; s < size_of_index_table; ++s ) {
    if ( indices.at(i+s+1)->matches(s) ) {
      get_transitions(t, i, indices.at(i+s+1));
    }
  }
}

bool RuntimeTransitionTableReader::matching_transition( RuntimeTransitionIndex * i, RuntimeTransitionEntry * t ){
  symbol_pair_number transition_pair = t->get_pair();
  symbol_number t_input = alpha_reader.get_input(transition_pair);
  //fprintf(stderr,"pair: %u %u\n",i->input_symbol(),t_input);
  //return (alpha_reader.symbol(i->input_symbol()) == t_input);
  return (alpha_reader.symbol(i->input_symbol()) == t_input);
}

void RuntimeTransitionTableReader::get_transitions(Transducer * t,
						   transition_number origin,
						   RuntimeTransitionIndex * index)
{
  //fprintf(stderr,"origin %u %u %u\n",origin,index->input_symbol(),index->transition_index());

  if (index->transition_index() == 0 )
    return;

  transition_number transition_index = index->transition_index()-1;
  
  while ( transition_index < transitions.size() and 
	  matching_transition(index,transitions.at(transition_index) ) and 
	  (transitions.at(transition_index)->get_pair() != DUMMY_PAIR)) {

    //fprintf(stderr,"found one\n");
    Label l = alpha_reader.get_label(transitions.at(transition_index)->get_pair());
    transition_number target = transitions.at(transition_index)->get_target();
    if ( states.find(target) == states.end() ) {      
      states[target] = t->new_node();
      write_transitions(t,target);
    }
    
    states[origin]->add_arc(l,states[target],t);

    ++transition_index;
  }
}
