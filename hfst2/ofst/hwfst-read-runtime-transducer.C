#include "hwfst-read-runtime-transducer.h"

namespace HWFST {

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

void RuntimeTransitionTableReader::read_transitions(TransduceR * t,AlphabetReader &alphabet_reader) 
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
      transition_number transition = *((transition_number*)(p+sizeof(symbol_pair_number)));
      transition_penalty w = *((transition_penalty*)(p+sizeof(symbol_number)+sizeof(transition_number)));
      transitions.push_back(new RuntimeTransitionEntry(pair,transition,w));
    }
  transition_number start_state = 0;
  states[start_state] = t->Start();
  fprintf(stderr,"Start %u set to %u\n",start_state,t->Start());
  write_transitions(t,start_state);
}

void RuntimeTransitionTableReader::write_transitions(TransduceR * t,
						     transition_number i)
{
  RuntimeTransitionIndex * index = indices.at(i);
  if ( index->is_final() ) {
    fprintf(stderr,"Final State %u(%u)\n",states[i],i);
    //t->SetFinal(states[i],0); 
    t->SetFinal(states[i],fst::TropicalWeight(transitions.at(index->get_finality()-1)->get_weight()));
  }
  else {
    fprintf(stderr,"Non-final state %u(%u)\n",states[i],i);
  }
  for ( symbol_number s = 0; s < size_of_index_table; ++s ) {
    if ( indices.at(i+s+1)->matches(s) ) {
      get_transitions(t, i, indices.at(i+s+1));
    }
  }
}

bool RuntimeTransitionTableReader::matching_transition( RuntimeTransitionIndex * i, RuntimeTransitionEntry * t ){
  symbol_pair_number transition_pair = t->get_pair();
  if ( transition_pair == 0 )
    return false;
  symbol_number t_input = alpha_reader.get_input(transition_pair);
  //fprintf(stderr,"pair: %u %u\n",i->input_symbol(),t_input);
  //return (alpha_reader.symbol(i->input_symbol()) == t_input);
  return (alpha_reader.symbol(i->input_symbol()) == t_input);
}

void RuntimeTransitionTableReader::get_transitions(TransduceR * t,
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
      states[target] = t->AddState();
      write_transitions(t,target);
    }
    
    transition_penalty w = transitions.at(transition_index)->get_weight();
    fprintf(stderr,"Origin %u(%u) Lower char %u Upper char %u Weight %f Target %u(%u)\n",
	    states[origin],origin,l.lower_char(),l.upper_char(),w,states[target],target);
    t->AddArc(states[origin],StdArc(l.lower_char(),l.upper_char(),Weight(w),states[target])); // Changes when transition weights are considered.
    ++transition_index;
  }
}

};
