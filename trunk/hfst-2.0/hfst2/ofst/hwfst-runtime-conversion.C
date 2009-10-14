#include "hwfst-runtime-conversion.h"

TransduceR * HWFST::RuntimeState::transducer;

namespace HWFST {

symbol_pair_number SymbolPairNumbering::get_pair_number( const Label &l ) 
{
  input_symbols.insert( l.lower_char() );
  if ( dense_symbol_numbers.find( l.lower_char() ) == dense_symbol_numbers.end() ) {
    dense_symbol_numbers[ l.lower_char() ] = next_dense;
    ++next_dense;
  }
  character_codes[l.lower_char()] = (unsigned int)(l.lower_char());
  character_codes[l.upper_char()] = (unsigned int)(l.upper_char());

  if ( symbol_pair_numbers.find(l) == symbol_pair_numbers.end() ) {
    symbol_pair_numbers[l] = next_number;
    inputs[next_number] = l.lower_char();
    ++next_number;
  }
  
  return symbol_pair_numbers[l];
}

void SymbolPairNumbering::write( FILE * f ) {

  for( map<Character,unsigned int>::iterator it = character_codes.begin();
       it != character_codes.end();
       ++it )
    {
      (void) fwrite(&(it->second),sizeof(it->second),1,f);
    }
  for( set<Character>::iterator it = input_symbols.begin();
       it != input_symbols.end();
       ++it ) {
    Character c = *it;
    (void) fwrite(&c,sizeof(c),1,f);
  }
  vector<symbol_number> input_chars(symbol_pair_numbers.size(),0);
  vector<symbol_number> output_chars(symbol_pair_numbers.size(),0);
  
  for( map<Label,symbol_pair_number,compare_labels>::iterator it = symbol_pair_numbers.begin();
       it != symbol_pair_numbers.end();
       ++it ) {
    Label symbol_pair = it->first;
    symbol_pair_number pair_index = (it->second)-1;
    input_chars.at(pair_index) = symbol_pair.lower_char();
    output_chars.at(pair_index) = symbol_pair.upper_char();
  }
  
  for ( unsigned int i = 0; i < input_chars.size(); ++i ) {
    size_t foo = fwrite(&input_chars.at(i),sizeof(symbol_number),1,f);
    foo = fwrite(&output_chars.at(i),sizeof(symbol_number),1,f);
  }
}

void RuntimeState::read_transitions( vector<RuntimeTransition*> &transducer_transitions ) 
{
  fprintf(stderr,"RuntimeState::read_transitions\n");
  // Tentative
  if ( transducer->Final(transducer_state) != fst::TropicalWeight::Zero() ) {
    fprintf(stderr,"%f",transducer->Final(transducer_state).Value());
    transducer_transitions.push_back(new RuntimeTransition(0,0,transducer->Final(transducer_state)));
    finality_target = transducer_transitions.size();
  }

  for ( ArcIterator arcs(*transducer,transducer_state); 
	not arcs.Done(); 
	arcs.Next() ) 
    {
    
      StdArc a = arcs.Value();
      Label transition_label( a.ilabel, a.olabel );
      
      TransitionTarget transition_target( a );
      //  StateId transition_target = a.nextstate;
      
      symbol_number transition_label_number = 
	symbol_pair_numbering->get_pair_number( transition_label );
      
      if (transitions.find(transition_label_number) == transitions.end() )
	transitions[transition_label_number] = new vector<TransitionTarget>;
      transitions[ transition_label_number ]->push_back(transition_target);
    }
  
  if ( not (transitions.empty() or transducer_transitions.empty()) ) {
    symbol_pair_number first_state_pair = transitions.begin()->first;
    symbol_number first_state_input = 
      symbol_pair_numbering->input_symbol(first_state_pair);

    symbol_pair_number last_table_pair = (*(transducer_transitions.end()-1))->get_pair();
    if ( last_table_pair != 0 ) {
      symbol_number last_table_input = 
	symbol_pair_numbering->input_symbol(last_table_pair);
      
      if ( first_state_input == last_table_input ) 
	transducer_transitions.push_back( new RuntimeTransition() );
    }
  }
  
  ///// HERE
  for ( map<symbol_pair_number, vector<TransitionTarget>* >::iterator jt = transitions.begin();
	jt != transitions.end();
	++jt ) {
    vector<TransitionTarget> * label_targets = jt->second;
    RuntimeTransition * rt = new RuntimeTransition(jt->first,*(label_targets->begin()));
    transducer_transitions.push_back(rt);
    if ( transition_indices.find(symbol_pair_numbering->input_symbol_order(jt->first)) == transition_indices.end() )
      transition_indices[ symbol_pair_numbering->input_symbol_order(jt->first) ] = transducer_transitions.size();
  
    for ( vector<TransitionTarget>::iterator  it=label_targets->begin()+1;
	  it != label_targets->end();
	  ++it ) {
      RuntimeTransition * rt = new RuntimeTransition(jt->first,*it);
      transducer_transitions.push_back(rt);
    }
    delete label_targets;
  }
}

truth_value RuntimeState::get_finality( void ) {
  
  fprintf(stderr,"Weight %f (ONE %f ZERO %f)\n",
	  transducer->Final(transducer_state).Value(),
	  fst::TropicalWeight::One().Value(),
	  fst::TropicalWeight::Zero().Value());
  if ( transducer->Final(transducer_state) == fst::TropicalWeight::Zero() ) {
    return NO;
  }
  else {
    return YES;
  }

}

void RuntimeState::make_state_transition_index_table( void ) 
{
  fprintf(stderr,"RuntimeState::make_state_transition_index_table\n");
  // We mark finality at the beginning of the transition index-table.
    RuntimeTransitionIndex * finality_transition_index =
      new RuntimeTransitionIndex(TRANSITION_LIST_BEGINNING,
				 finality_target);

  

  state_transition_indices.push_back(finality_transition_index);

  symbol_number counter = 0;

  for( map<symbol_number,transition_number>::iterator it = transition_indices.begin();
       it != transition_indices.end();
       ++it ) {

    while ( counter < it->first ) {
      RuntimeTransitionIndex * empty_index_marker = 
	new RuntimeTransitionIndex(EMPTY_SYMBOL_NUMBER,
				   EMPTY_TARGET);
      state_transition_indices.push_back(empty_index_marker);
      ++counter;
    }

    RuntimeTransitionIndex * transitions_index = 
      new RuntimeTransitionIndex(it->first,it->second);
    state_transition_indices.push_back(transitions_index);
    
    ++counter;
  }

  while ( counter < symbol_pair_numbering->number_of_input_symbols()+1 ) {
      RuntimeTransitionIndex * empty_index_marker = 
	new RuntimeTransitionIndex(EMPTY_SYMBOL_NUMBER,
				   EMPTY_TARGET);
      state_transition_indices.push_back(empty_index_marker);
      ++counter;
    }
 };

void RuntimeHeader::write( FILE * f ) 
{

  size_t foo = fwrite(&byte_endianness,sizeof(byte_endianness),1,f);
  foo = fwrite(&version,sizeof(version),1,f);
  foo = fwrite(&determinism,sizeof(determinism),1,f);
  foo = fwrite(&minimality,sizeof(minimality),1,f);
  foo = fwrite(&cyclicity,sizeof(cyclicity),1,f);
  foo = fwrite(&weightedness,sizeof(weightedness),1,f);
  foo = fwrite(&number_of_unicode_symbols,sizeof(number_of_symbols),1,f);
  foo = fwrite(&number_of_symbols,sizeof(number_of_symbols),1,f);
  foo = fwrite(&number_of_pairs_in_transducer,sizeof(number_of_pairs_in_transducer),1,f);
  foo = fwrite(&size_of_index_table,sizeof(size_of_index_table),1,f);
  foo = fwrite(&size_of_transition_table,sizeof(size_of_transition_table),1,f);

}

void TransitionIndexTable::add_state( RuntimeState * s,
				      map<StateId,transition_number> &transducer_nodes) 
{

  s->make_state_transition_index_table();
  transition_number old_floor = floor;
  transition_number filled = 0;
  
  transition_number limit = indices.size() - (alphabet_size+2);
  while ( floor < indices.size() ) 
    {
      if ( floor == limit ) 
	{ 
	  more_space();
	  limit = indices.size();
	}
      
      if ( not indices.at(floor)->is_empty() )
	++filled;
      
      if ( node_fits(s,floor) ) 
	{ 
	  transducer_nodes[ s->get_node() ] = floor;
	  insert_node(s,floor);
	  transition_list_start_indices.insert(floor);
	  break;
	  ++floor;
	}
      ++floor;
    }
    
  if (filled*1.0/(floor - old_floor) < 0.8)
    floor = old_floor;
  
  
  }

bool TransitionIndexTable::node_fits( RuntimeState * s, transition_number index ) {

  if ( transition_list_start_indices.find(index) != transition_list_start_indices.end() )
    return false;
  
  vector<RuntimeTransitionIndex*> s_transition_indices = s->get_transition_indices();

  for( size_t i = 0; i < s_transition_indices.size(); ++i ) {
      if ( not s_transition_indices.at(i)->is_empty() )
	if ( not indices.at(index+i)->is_empty() )
	  return false;
  }
  
  return true;
};
 
void TransitionIndexTable::insert_node( RuntimeState * s, transition_number index ) {

  vector<RuntimeTransitionIndex*> s_transition_indices = s->get_transition_indices();

  for( size_t i = 0; i < s_transition_indices.size(); ++i ) {
    if ( not s_transition_indices.at(i)->is_empty()) {
      indices.at(index+i) = s_transition_indices.at(i);
    }
  }
  
};

void TransitionIndexTable::write(FILE * f) {
  for ( vector<RuntimeTransitionIndex*>::iterator it = indices.begin();
	it != indices.end();
	++it ) {
    if (*it != NULL) {
      (*it)->write(f);
    }
    else {
      empty_write(f);
    }
  }
    
}

transition_number RuntimeTransitionIndexTable::make_table( void ) {

  index_table = new TransitionIndexTable (root,
					  states,
					  symbol_pair_numbering.number_of_input_symbols(),
					  transducer_nodes);
  return index_table->size();
}

void RuntimeTransitionIndexTable::write( FILE * f ) {

  index_table->write(f);
}

void RuntimeTransducerWriter::add_runtime_state( RuntimeState * s ) 
{
  transition_index_table.add_runtime_state(s);
}

void RuntimeTransducerWriter::add_root( RuntimeState * s ) 
{
  transition_index_table.add_first_state(s);
}

void RuntimeTransducerWriter::convert_states( void ) 
{
  add_root( new RuntimeState(regular_transducer->Start(),
			     &symbol_pair_numbers,
			     transition_table) );

  // We need to skip state 0 i.e. the the root
  // regular_transducer->Start() which was just added.
  StateIterator state_numbers(*regular_transducer);
  state_numbers.Next();
  for ( ;
	not state_numbers.Done();
	state_numbers.Next() ) 
    {
      StateId state = state_numbers.Value();
      add_runtime_state( new RuntimeState(state,&symbol_pair_numbers,transition_table) ); 

    }
}

void RuntimeTransducerWriter::write_transitions( void ) {

  map<StateId,transition_number> transducer_nodes = 
    transition_index_table.get_transducer_nodes();

  for ( vector<RuntimeTransition*>::iterator it = transition_table.begin();
	it != transition_table.end();
	++it ) {
    (*it)->write(output_file,transducer_nodes);
  }

}

};
