#include "hfst-optimized-lookup-transducer.h"

void HfstIdNumberMap::add_node(const Node * n)
{
  if (node_to_id.find(n) == node_to_id.end())
    {
      node_to_id[n] = node_counter;
      id_to_node[node_counter] = n;
      ++node_counter;
      for (ArcsIter aiter(n->arcs()); 
	   aiter;
	   aiter++)
	{
	  Arc a = *aiter;
	  add_node(a.target_node());
	  ++arc_counter;
	}
    }
}

void HfstIdNumberMap::set_node_maps(Transducer * t)
{
  Node * n = t->root_node();
  add_node(n);
}

StateIdNumber HfstIdNumberMap::get_node_id(const Node * n)
{
  if (node_to_id.find(n) == node_to_id.end())
    {
      return NO_ID_NUMBER;
    }
  return node_to_id[n];
}

const Node * HfstIdNumberMap::get_id_node(StateIdNumber i)
{
  if (id_to_node.find(i) == id_to_node.end())
    {
      return NULL;
    }
  return id_to_node[i];
}

void HfstIdNumberMap::set_target_transitions(StateIdNumber i,
					 TransitionTableIndex target_index)
{
  id_to_target_index[i] = target_index;
}

HFST::KeyTable * HfstTransition::keys;
HFST::KeyTable * HfstTransitionIndex::keys;
HfstIdNumberMap * HfstTransition::nodes_to_id_numbers;
HfstIdNumberMap * HfstFstState::state_id_numbers;
HFST::KeyTable * HfstFstState::keys;

const char * HfstTransition::get_symbol_string(SymbolNumber num)
{
  HFST::Symbol s = HFST::get_key_symbol(num,keys);
  return HFST::get_symbol_name(s);
}

SymbolNumber HfstTransition::get_input_symbol(const Arc &a)
{
  return a.label().lower_char();
}

SymbolNumber HfstTransition::get_output_symbol(const Arc &a)
{
  return a.label().upper_char();
}

const Node * HfstTransition::get_target_node(const Arc &a)
{
  return a.target_node();
}

void HfstTransition::set_key_table(HFST::KeyTable * kt)
{
  keys = kt;
}

void HfstTransition::set_id_numbers(HfstIdNumberMap * ids)
{
  nodes_to_id_numbers = ids;
}

#ifdef DEBUG
void HfstTransition::display(void)
{
  const char * input_string = get_symbol_string(input_symbol);
  const char * output_string = get_symbol_string(output_symbol);
  std::cout << input_string << " (" << input_symbol << ")" << "\t" 
	    << output_string << " (" << output_symbol << ")" << "\t"
	    <<  nodes_to_id_numbers->get_id_place(target_state_id) << std::endl;
}
#endif

bool HfstTransition::operator<( const HfstTransition &another_transition)
  const
{
  if (input_symbol == another_transition.input_symbol)
    {
      if (output_symbol == another_transition.output_symbol)
	{
	  return target_state_id < another_transition.target_state_id;
	}
      return output_symbol < another_transition.output_symbol;
    }
  return input_symbol < another_transition.input_symbol;
}

void HfstTransition::write_empty_transition(FILE * f, bool final)
{
  unsigned int size;
  size = fwrite(&NO_SYMBOL_NUMBER,
	 sizeof(SymbolNumber),
	 1,
	 f);
  size = fwrite(&NO_SYMBOL_NUMBER,
	 sizeof(SymbolNumber),
	 1,
	 f);
  if ( not final )
    {
      size = fwrite(&NO_TABLE_INDEX,
	     sizeof(TransitionTableIndex),
	     1,
	     f);
    }
  else
    {
      TransitionTableIndex finality = 1;
      size = fwrite(&finality,
	     sizeof(TransitionTableIndex),
	     1,
	     f);
    }
}
#ifdef DEBUG
void HfstTransition::display_empty_transition(bool final)
{
  if (not final)
    {
      std::cout << "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_TABLE_INDEX << ")" << std::endl;
    }
  else
    {
      std::cout << "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "FINAL" << " (" << 1 << ")" << std::endl;

    }
}

#endif
void HfstTransition::write(FILE * f)
{
  unsigned int size;
  size = fwrite(&input_symbol,
	 sizeof(SymbolNumber),
	 1,
	 f);
  size = fwrite(&output_symbol,
	 sizeof(SymbolNumber),
	 1,
	 f);
  TransitionTableIndex target_index =
    nodes_to_id_numbers->get_id_place(target_state_id);

  // This is STEMS FROM A BUG, WHICH I'VE NOT FIXED YET.
  if ( target_index > TRANSITION_TARGET_TABLE_START)
    { --target_index; }

  size = fwrite(&target_index,
	 sizeof(TransitionTableIndex),
	 1,
	 f);
}

const char * HfstTransitionIndex::get_symbol_string(SymbolNumber num)
{
  HFST::Symbol s = HFST::get_key_symbol(num,keys);
  return HFST::get_symbol_name(s);
}

void HfstTransitionIndex::set_key_table(HFST::KeyTable * kt)
{
  keys = kt;
}

#ifdef DEBUG
void HfstTransitionIndex::display(void)
{
  std::cout << get_symbol_string(input_symbol) << "\t" 
	    << first_transition_index << "\t"
	    << first_transition_place+TRANSITION_TARGET_TABLE_START << std::endl;
}
#endif

bool HfstTransitionIndex::operator<( const HfstTransitionIndex &another_index)
  const
{
  return input_symbol < another_index.input_symbol;
}

void HfstTransitionIndex::write(IndexVector * transition_index_table, 
			    TransitionTableIndex start_index)
{
  transition_index &ind = 
    transition_index_table->at(start_index + input_symbol);
  ind.input_symbol = input_symbol;
  ind.target = first_transition_place + TRANSITION_TARGET_TABLE_START;
}

void HfstFstState::set_key_table(HFST::KeyTable * kt)
{
  keys = kt;
  HfstTransition::set_key_table(kt);
  HfstTransitionIndex::set_key_table(kt);
}

void HfstFstState::set_id_numbers(HfstIdNumberMap * ids)
{
  state_id_numbers = ids;
  HfstTransition::set_id_numbers(ids);
}

void HfstFstState::set_transitions(const Node * n)
{
  for (ArcsIter aiter(n->arcs());
       aiter;
       aiter++)
    {
      const Arc a = *aiter;
      HfstTransition * t = new HfstTransition(n,a);
      transitions.insert(t);
    }
}

void HfstFstState::set_transition_indices(void)
{
  SymbolNumber previous_symbol = NO_SYMBOL_NUMBER;
  SymbolNumber position = 0;

  for (HfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      HfstTransition * t = *it;
      SymbolNumber input_symbol = t->return_input_symbol();
      if (previous_symbol != input_symbol)
	{
	  transition_indices.insert(new HfstTransitionIndex(input_symbol,
							position));
	  previous_symbol = input_symbol;
	}
      ++position;
    }
}

SymbolNumberSet * HfstFstState::get_input_symbols(void)
{
  SymbolNumberSet * input_symbols = new SymbolNumberSet;

  for(HfstTransitionIndexSet::iterator it = transition_indices.begin();
      it != transition_indices.end();
      ++it)
    {
      HfstTransitionIndex * i = *it;
      input_symbols->insert(i->get_input_symbol());
    }
  
  return input_symbols;

}

TransitionTableIndex
HfstFstState::set_transition_table_indices(TransitionTableIndex place)
{
  first_transition_index = place;
  // There has to be a free transition between the transitions of two
  // states.
  //if (place != 0)
  //  {
  //    ++place;
  //  }
  //#ifdef DEBUG
  //else 
  //  {
  //    std::cout << "root\n";
  //  }
  //#endif

  for(HfstTransitionSet::iterator it = transitions.begin();
      it != transitions.end();
      ++it)
    {
      HfstTransition * t = *it;
      t->set_transition_place(place);
      ++place;
    }
  ++place;


  vector<HfstTransition*> transition_v;
  for (HfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      HfstTransition * t = *it;
      transition_v.push_back(t);
    }

  for (HfstTransitionIndexSet::iterator it = transition_indices.begin();
       it != transition_indices.end();
       ++it)
    {
      HfstTransitionIndex * i = *it;
      SymbolNumber index = i->get_index();
      HfstTransition * transition_at_index =
	transition_v.at(index);
      TransitionTableIndex place1 =
	transition_at_index->get_place();
      i->set_place(place1);
    }

  return place;
}

TransitionTableIndex
HfstFstState::write_transitions(FILE * f,
			    TransitionTableIndex place)
{
  while (place < first_transition_index)
    {
      HfstTransition::write_empty_transition(f,final);
      //HfstTransition::display_empty_transition(final);
      ++place;
    }
  for (HfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      (*it)->write(f);
      //(*it)->display();
      ++place;
    }
  return place;
}

#ifdef DEBUG
void HfstFstState::display(void)
{
  std::cout << "State " << id << ":" << std::endl;
  for(HfstTransitionIndexSet::iterator it = transition_indices.begin();
      it != transition_indices.end();
      ++it)
    {
      HfstTransitionIndex * i = *it;
      i->display();
    }
  for(HfstTransitionSet::iterator it = transitions.begin();
      it != transitions.end();
      ++it)
    {
      HfstTransition * t = *it;
      t->display();
    }
  if (final)
    {
      std::cout << id << std::endl;
    }
}
#endif

void HfstFstState::add_state_indices(IndexVector * transition_index_table)
{
  if (not is_big_state())
    {
      if (not is_start_state())
	{
	  return;
	}
    }
  TransitionTableIndex i = state_id_numbers->get_id_place(id);

  if (final)
    {
      transition_index_table->at(i).target = 1;
    }
  ++i;

  for (HfstTransitionIndexSet::iterator it = transition_indices.begin();
       it != transition_indices.end();
       ++it)
    {
      HfstTransitionIndex * ind = *it;
      ind->write(transition_index_table,
		 i);
    }
}

void HfstTransitionTableIndices::get_more_space(void)
{
  for(SymbolNumber i = 0; i < number_of_input_symbols + 1; ++i)
    {
      indices.push_back(EMPTY);
    }
}

bool HfstTransitionTableIndices::state_fits(SymbolNumberSet * input_symbols,
					bool final_state,
					PlaceHolderVector::size_type index)
{
  if ((indices.at(index) == EMPTY_START) or
      (indices.at(index) == OCCUPIED_START))
    {
      return false;
    }
  
  if (final_state and (indices.at(index) == OCCUPIED))
    {
      return false;
    }

  // The input symbols start after the finality indicator.
  PlaceHolderVector::size_type input_symbol_start = index+1;

  // The node fits, if every one of its input symbols goes on 
  // an EMPTY or EMPTY_START index.
  for (SymbolNumberSet::iterator it = input_symbols->begin();
       it != input_symbols->end();
       ++it)
    {
      SymbolNumber input_symbol = *it;

      if ((indices.at(input_symbol_start + input_symbol) ==
	   OCCUPIED) or
	  (indices.at(input_symbol_start + input_symbol) ==
	   OCCUPIED_START))
	{
	  return false;
	}
    }

  return true;
}

void HfstTransitionTableIndices::insert_state(SymbolNumberSet * input_symbols,
					  bool final_state,
					  PlaceHolderVector::size_type index)
{
  if (final_state or (indices.at(index) == OCCUPIED))
    {
      indices.at(index) = OCCUPIED_START;
    }
  else
    {
      indices.at(index) = EMPTY_START;
    }
  
  // The input symbols start after the finality indicator.
  PlaceHolderVector::size_type input_symbol_start = index+1;

  for (SymbolNumberSet::iterator it = input_symbols->begin();
       it != input_symbols->end();
       ++it)
    {
      SymbolNumber input_symbol = *it;
      if (indices.at(input_symbol_start + input_symbol) == EMPTY)
	{
	  indices.at(input_symbol_start + input_symbol) = OCCUPIED;
	}
      else
	{
	  indices.at(input_symbol_start + input_symbol) = OCCUPIED_START;
	}
    }
}

PlaceHolderVector::size_type HfstTransitionTableIndices::last_full_index(void)
{
  for(PlaceHolderVector::size_type i = indices.size()-1;
      i != 0;
      --i)
    {
      if (indices.at(i) != EMPTY)
	{
	  return i;
	}
    }
  return 0;
}
PlaceHolderVector::size_type
HfstTransitionTableIndices::add_state(HfstFstState * state)
{
  if (lower_bound_test_count >= 1)
    {
      lower_bound_test_count = 0;
      if (indices.size() > 2000)
	{
	  if (lower_bound < (indices.size() - 2000))
	    {
	      lower_bound = indices.size() - 1000;
	    }
	}
      ++lower_bound;
    }

#ifdef DEBUG
  std::cout << lower_bound << " " << lower_bound_test_count << " " << indices.size() << " " << state->number_of_input_symbols() << "\r";
#endif
  bool final_state = state->is_final();

  SymbolNumberSet * state_input_symbols = 
    state->get_input_symbols();

  ++lower_bound_test_count;

  for (PlaceHolderVector::size_type index = lower_bound;
       index < indices.size();
       ++index)
    {
      // Only try the first 100 indices.
      //    if (index > lower_bound+100000)
      //	{
      //	  index = last_full_index()+1;
      //	}
      if ((index + number_of_input_symbols + 1) >= indices.size())
	{
	  get_more_space();
	}
      if (state_fits(state_input_symbols,final_state,index))
	{
	  insert_state(state_input_symbols,final_state,index);
	  delete state_input_symbols;
	  return index;
	}
    }
  return UINT_MAX;
}

void HfstFst::read_nodes(void)
{
  for(StateIdNumber id = 0;
      id < fst_state_id_numbers->get_number_of_nodes();
      ++id)
    {
      const Node * n = fst_state_id_numbers->get_id_node(id);
      HfstFstState * state = new HfstFstState(n);
      states.push_back(state);
    }
}

void HfstFst::set_transition_table_indices(void)
{
  TransitionTableIndex place = 0;
  for (HfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HfstFstState * state = *it;
      place = state->set_transition_table_indices(place);
    }
}

struct fst_state_compare {
  bool operator() (const HfstFstState * s1,
		   const HfstFstState * s2) const
  {
    if (s1->transition_indices.size() < s2->transition_indices.size())
      {
	return true;
      }
    return s1->id < s2->id;
  }
};

typedef std::set<HfstFstState*,fst_state_compare> StateSet ;

void HfstFst::set_target_node_places(void)
{
  StateSet state_set;

  for (HfstStateVector::iterator it = states.begin()+1;
       it != states.end();
       ++it)
    {
      state_set.insert(*it);
    }

  HfstFstState * start_state = *states.begin(); 
  TransitionTableIndex start_state_index = 
    fst_indices->add_state(start_state);
#ifdef DEBUG 
  assert(start_state_index == 0);
#endif
  fst_state_id_numbers->set_target_transitions(start_state->get_id(),
					       start_state_index);

  for (StateSet::reverse_iterator it = state_set.rbegin();
       it != state_set.rend();
       ++it)
    {
      HfstFstState * state = *it;
      TransitionTableIndex state_index;
      if (state->is_big_state())
	{
	  state_index =
	    fst_indices->add_state(state);
	}
      else
	{
	  state_index = 
	    TRANSITION_TARGET_TABLE_START + 
	    state->get_first_transition_index();	    
	}
      fst_state_id_numbers->set_target_transitions(state->get_id(),
						   state_index);
    }
}

void HfstFst::add_input_symbols(Node * n,
			    SymbolNumberSet &input_symbols,
			    NodeSet &visited_nodes)
{
  for(ArcsIter aiter(n->arcs());
      aiter;
      aiter++)
    {
      Arc a = *aiter;
      input_symbols.insert(a.label().lower_char());
      if (visited_nodes.find(a.target_node()) == visited_nodes.end())
	{
	  visited_nodes.insert(a.target_node());
	  add_input_symbols(a.target_node(),
			    input_symbols,
			    visited_nodes);
	}
    }
}

SymbolNumber HfstFst::number_of_input_symbols(void)
{
  SymbolNumberSet input_symbol_set;
  input_symbol_set.insert(0);
  NodeSet visited_nodes;
  add_input_symbols(fst->root_node(),
		    input_symbol_set,
		    visited_nodes);
  #ifdef DEBUG
  std::cout << "Input symbol set size " << input_symbol_set.size() << std::endl;
  #endif
  return input_symbol_set.size();
}

#ifdef DEBUG
void HfstFst::display(void)
{
  for (HfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HfstFstState * s = *it;
      s->display();
    }
}
#endif

void HfstFst::build_index_table(TransitionTableIndex index_table_size)
{
  transition_index_table = new IndexVector(index_table_size,
					   EMPTY_TRANSITION_INDEX);
  
  for (HfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HfstFstState * state = *it;
      state->add_state_indices(transition_index_table);
    }
  
  //#ifdef DEBUG
  //std::cout << "transition index table" << std::endl;
  //for(IndexVector::iterator it = transition_index_table->begin();
  //    it != transition_index_table->end();
  //    ++it)
  //  {
  //    std::cout << it->input_symbol << " " << it->target << std::endl;
  //  }
  //std::cout << std::endl;
  //#endif
}

void HfstFst::write_index_table(void)
{
  unsigned int size;

  for(IndexVector::iterator it = transition_index_table->begin();
      it != transition_index_table->end();
      ++it)
    {
      size = fwrite(&(it->input_symbol),
	     sizeof(SymbolNumber),
	     1,
	     output_file);
      size = fwrite(&(it->target),
	     sizeof(TransitionTableIndex),
	     1,
	     output_file);
    }
}

TransitionTableIndex HfstFst::count_transitions(void)
{
  TransitionTableIndex transition_count = 0;
  for (HfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      // Separator between states;
      ++transition_count;

      transition_count +=
	(*it)->number_of_transitions();
    }
  return transition_count;
}

void HfstFst::write_transition_targets()
{
  TransitionTableIndex place = 0;
  for(HfstStateVector::iterator it = states.begin();
      it != states.end();
      ++it)
    {
      HfstFstState * state = *it;
      place =
      state->write_transitions(output_file,place);
    }
  HfstTransition::write_empty_transition(output_file,false);
}

#ifdef DEBUG
void test_1(void);
void test_2(void);
void test_3(char * p);
#endif

#ifdef DEBUG
int main(int argc, char * argv[])
{
  //  std::cout << "test_1: Make a transducer with one transition from" 
  //	    << std::endl
  //	    << "root to root with pair a:b. Create the HfstTransition * and"
  //	    << std::endl
  //	    << "display it." << std::endl << std::endl;
  //test_1();
  //std::cout << std::endl;
  //std::cout << "test_2: Same as test_1 except do everything through"
  //	    << std::endl
  //	    << "Stste." << std::endl << std::endl;
  //test_2();
  
  if (argc != 2)
    {
      fprintf(stderr,"Usage: %s FILE\n",argv[0]);
      exit(1);
    }
  std::cout << "Read a transducer from a file and display it.";
  std::cout << std::endl;
  
  test_3(argv[1]);

}
#endif


#ifdef DEBUG 
void test_1(void)
{
  HFST::KeyTable * kt = HFST::create_key_table();
  HFST::Symbol epsilon = HFST::define_symbol("<>");
  HFST::Symbol sym_a = HFST::define_symbol("a");
  HFST::Symbol sym_b = HFST::define_symbol("b");
  HFST::associate_key(0,kt,epsilon);
  HFST::associate_key(1,kt,sym_a);
  HFST::associate_key(2,kt,sym_b);
  HfstTransition::set_key_table(kt);
  Transducer * t = new Transducer();

  Node * root = t->root_node();
  root->set_final(1);
  root->add_arc(Label(1,2),root,t);
  root->add_arc(Label(2,2),root,t);
  HfstIdNumberMap * ids = new HfstIdNumberMap(t);
  HfstTransition::set_id_numbers(ids);
  for (ArcsIter aiter(root->arcs());
       aiter;
       aiter++)
    {
      Arc a = *aiter;
      HfstTransition tr(root,a);
      tr.display();
    }
  delete kt;
  delete ids;
  delete t;
}

void test_2(void)
{
  HFST::KeyTable * kt = HFST::create_key_table();
  HFST::Symbol epsilon = HFST::define_symbol("<>");
  HFST::Symbol sym_a = HFST::define_symbol("A");
  HFST::Symbol sym_b = HFST::define_symbol("B");
  HFST::associate_key(0,kt,epsilon);
  HFST::associate_key(1,kt,sym_a);
  HFST::associate_key(2,kt,sym_b);
  HfstFstState::set_key_table(kt);

  Transducer * t = new Transducer();

  Node * root = t->root_node();
  root->set_final(1);
  root->add_arc(Label(1,1),root,t);
  root->add_arc(Label(1,2),root,t);
  root->add_arc(Label(2,0),root,t);
  root->add_arc(Label(2,2),root,t);
  HfstIdNumberMap * ids = new HfstIdNumberMap(t);
  HfstFstState::set_id_numbers(ids);
  HfstFstState state(root);
  state.display();
}


void test_3(char * p)
{
  HFST::KeyTable * kt = HFST::create_key_table();
  std::cout << "Reading transducer" << std::endl;
  HFST::TransducerHandle th = 
    HFST::read_transducer(p,kt);
  //HFST::print_transducer(th,kt);
  std::cout << "Done." << std::endl;
  Transducer * t = HANDLE_TO_PINSTANCE(Transducer,th); 
  HFST::KeyTable * new_kt = reorder_key_table(kt,t);
  HFST::TransducerHandle new_th =
    HFST::harmonize_transducer(th,kt,new_kt);
  HFST::delete_transducer(th);
  t = HANDLE_TO_PINSTANCE(Transducer,new_th); 
  char line[1000];
  line[0] = 0;
  strcat(line,p);
  strcat(line,".rt");
  FILE * out_file = fopen(line,"w");
  HfstFst fst(t,
  	  new_kt,
  	  out_file);
  fclose(out_file);
  //  fst.display();
}
#endif

