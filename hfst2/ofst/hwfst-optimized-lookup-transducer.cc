#include "hwfst-optimized-lookup-transducer.h"
#include "hwfst-runtime-auxiliary-functions.h"

std::set<Key> HwfstFstState::flag_diacritic_set;

bool check_finality(TransduceR * tr, StateId s)
{
  return tr->Final(s) != fst::TropicalWeight::Zero();
}

bool is_deterministic_(TransduceR * tr)
{
  return false or (tr == NULL);
}

bool is_minimised_(TransduceR * tr)
{
  return false or (tr == NULL);
}

void HwfstIdNumberMap::add_node(StateId n, TransduceR *tr)
{
  if (node_to_id.find(n) == node_to_id.end())
    {
      node_to_id[n] = node_counter;
      id_to_node[node_counter] = n;
      ++node_counter;
      for (ArcIterator aiter(*tr,n); 
	   not aiter.Done();
	   aiter.Next())
	{
	  StdArc a = aiter.Value();
	  add_node(a.nextstate,tr);
	  ++arc_counter;
	}
    }
}

void HwfstIdNumberMap::set_node_maps(TransduceR * t)
{
  StateId n = t->Start();
  add_node(n,t);
}

StateIdNumber HwfstIdNumberMap::get_node_id(StateId n)
{
  if (node_to_id.find(n) == node_to_id.end())
    {
      return NO_ID_NUMBER;
    }
  return node_to_id[n];
}

StateId HwfstIdNumberMap::get_id_node(StateIdNumber i)
{
  if (id_to_node.find(i) == id_to_node.end())
    {
      return NO_STATE_ID;
    }
  return id_to_node[i];
}

void HwfstIdNumberMap::set_target_transitions(StateIdNumber i,
					 TransitionTableIndex target_index)
{
  id_to_target_index[i] = target_index;
}

HWFST::KeyTable * HwfstTransition::keys;
HWFST::KeyTable * HwfstTransitionIndex::keys;
HwfstIdNumberMap * HwfstTransition::nodes_to_id_numbers;
HwfstIdNumberMap * HwfstFstState::state_id_numbers;
HWFST::KeyTable * HwfstFstState::keys;

const char * HwfstTransition::get_symbol_string(SymbolNumber num)
{
  HWFST::Symbol s = HWFST::get_key_symbol(num,keys);
  return HWFST::get_symbol_name(s);
}

SymbolNumber HwfstTransition::get_input_symbol(const StdArc &a)
{
  return a.ilabel;
}

SymbolNumber HwfstTransition::get_output_symbol(const StdArc &a)
{
  return a.olabel;
}

StateId HwfstTransition::get_target_node(const StdArc &a)
{
  return a.nextstate;
}

void HwfstTransition::set_key_table(HWFST::KeyTable * kt)
{
  keys = kt;
}

void HwfstTransition::set_id_numbers(HwfstIdNumberMap * ids)
{
  nodes_to_id_numbers = ids;
}

#ifdef DEBUG
void HwfstTransition::display(void)
{
  const char * input_string = get_symbol_string(input_symbol);
  const char * output_string = get_symbol_string(output_symbol);
  std::cerr << input_string << " (" << input_symbol << ")" << "\t" 
	    << output_string << " (" << output_symbol << ")" << "\t"
	    <<  nodes_to_id_numbers->get_id_place(target_state_id) << std::endl;
}
#endif

bool HwfstTransition::numerical_cmp( const HwfstTransition &another_transition)
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

bool HwfstTransition::operator<( const HwfstTransition &another_transition)
  const
{
  if ((input_symbol == 0) or HwfstFstState::is_flag_diacritic(input_symbol))
    {
      if ((another_transition.input_symbol == 0) or
	  HwfstFstState::is_flag_diacritic(another_transition.input_symbol))
	{ return numerical_cmp(another_transition); }
      else
	{ return true; } 
    }
  else
    {
      if ((another_transition.input_symbol != 0) and
	 not HwfstFstState::is_flag_diacritic(another_transition.input_symbol))
	{ return numerical_cmp(another_transition); }
      else
	{ return false; } 
    }
}

void HwfstTransition::write_empty_transition(FILE * f, bool final, float w)
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
      size = fwrite(&w,
		    sizeof(float),
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
      size = fwrite(&w,
		    sizeof(float),
		    1,
		    f);
    }

}
#ifdef DEBUG
void HwfstTransition::display_empty_transition(bool final)
{
  if (not final)
    {
      std::cerr << "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_TABLE_INDEX << ")" << std::endl;
    }
  else
    {
      std::cerr << "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "NaN" << " (" << NO_SYMBOL_NUMBER << ")" << "\t"
		<< "FINAL" << " (" << 1 << ")" << std::endl;

    }
}

#endif
void HwfstTransition::write(FILE * f)
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

  size = fwrite(&weight,
		sizeof(float),
		1,
		f);
}

const char * HwfstTransitionIndex::get_symbol_string(SymbolNumber num)
{
  HWFST::Symbol s = HWFST::get_key_symbol(num,keys);
  return HWFST::get_symbol_name(s);
}

void HwfstTransitionIndex::set_key_table(HWFST::KeyTable * kt)
{
  keys = kt;
}

#ifdef DEBUG
void HwfstTransitionIndex::display(void)
{
  std::cerr << get_symbol_string(input_symbol) << "\t" 
	    << first_transition_index << "\t"
	    << first_transition_place+TRANSITION_TARGET_TABLE_START << std::endl;
}
#endif

bool HwfstTransitionIndex::operator<( const HwfstTransitionIndex &another_index)
  const
{
  return input_symbol < another_index.input_symbol;
}

void HwfstTransitionIndex::write(IndexVector * transition_index_table, 
			    TransitionTableIndex start_index)
{
  transition_index &ind = 
    transition_index_table->at(start_index + input_symbol);
  ind.input_symbol = input_symbol;
  ind.target = first_transition_place + TRANSITION_TARGET_TABLE_START;
}

void HwfstFstState::set_key_table(HWFST::KeyTable * kt)
{
  keys = kt;
  HwfstTransition::set_key_table(kt);
  HwfstTransitionIndex::set_key_table(kt);
}

void HwfstFstState::set_id_numbers(HwfstIdNumberMap * ids)
{
  state_id_numbers = ids;
  HwfstTransition::set_id_numbers(ids);
}

void HwfstFstState::set_transitions(StateId n, TransduceR * tr)
{
  for (ArcIterator aiter(*tr,n);
       not aiter.Done();
       aiter.Next())
    {
      const StdArc a = aiter.Value();
      HwfstTransition * t = new HwfstTransition(n,a);
      transitions.insert(t);
    }
}

void HwfstFstState::set_transition_indices(void)
{
  SymbolNumber previous_symbol = NO_SYMBOL_NUMBER;
  SymbolNumber position = 0;
  
  bool zero_transitions = false;
  for (HwfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      HwfstTransition * t = *it;
      SymbolNumber input_symbol = t->return_input_symbol();
      if (previous_symbol != input_symbol)
	{
	  if (is_flag_diacritic(input_symbol))
	    {
	      if (not zero_transitions)
		{
		  transition_indices.insert(new HwfstTransitionIndex(0,
								    position));
		  
		  previous_symbol = input_symbol;
		  zero_transitions =true;
		}
	    }
	  else
	    {
	      transition_indices.insert(new HwfstTransitionIndex(input_symbol,
								position));
	      
	      previous_symbol = input_symbol;
	    }
	}
      if (input_symbol == 0) { zero_transitions = true; }		
      ++position;
    }
}
/*
{
  SymbolNumber previous_symbol = NO_SYMBOL_NUMBER;
  SymbolNumber position = 0;

  bool zero_transitions = false;
  for (HwfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      HwfstTransition * t = *it;
      SymbolNumber input_symbol = t->return_input_symbol();
      if (previous_symbol != input_symbol)
	{
	  transition_indices.insert(new HwfstTransitionIndex(input_symbol,
							position));
	  previous_symbol = input_symbol;
	}
      ++position;
    }
}
*/

SymbolNumberSet * HwfstFstState::get_input_symbols(void)
{
  SymbolNumberSet * input_symbols = new SymbolNumberSet;

  for(HwfstTransitionIndexSet::iterator it = transition_indices.begin();
      it != transition_indices.end();
      ++it)
    {
      HwfstTransitionIndex * i = *it;
      input_symbols->insert(i->get_input_symbol());
    }
  
  return input_symbols;

}

TransitionTableIndex
HwfstFstState::set_transition_table_indices(TransitionTableIndex place)
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
  //    std::cerr << "root\n";
  //  }
  //#endif

  for(HwfstTransitionSet::iterator it = transitions.begin();
      it != transitions.end();
      ++it)
    {
      HwfstTransition * t = *it;
      t->set_transition_place(place);
      ++place;
    }
  ++place;


  vector<HwfstTransition*> transition_v;
  for (HwfstTransitionSet::iterator it = transitions.begin();
       it != transitions.end();
       ++it)
    {
      HwfstTransition * t = *it;
      transition_v.push_back(t);
    }

  for (HwfstTransitionIndexSet::iterator it = transition_indices.begin();
       it != transition_indices.end();
       ++it)
    {
      HwfstTransitionIndex * i = *it;
      SymbolNumber index = i->get_index();
      HwfstTransition * transition_at_index =
	transition_v.at(index);
      TransitionTableIndex place1 =
	transition_at_index->get_place();
      i->set_place(place1);
    }

  return place;
}

TransitionTableIndex
HwfstFstState::write_transitions(FILE * f,
			    TransitionTableIndex place)
{
  while (place < first_transition_index)
    {
      HwfstTransition::write_empty_transition(f,final,weight);
      //HwfstTransition::display_empty_transition(final);
      ++place;
    }
  for (HwfstTransitionSet::iterator it = transitions.begin();
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
void HwfstFstState::display(void)
{
  std::cerr << "State " << id << ":" << std::endl;
  for(HwfstTransitionIndexSet::iterator it = transition_indices.begin();
      it != transition_indices.end();
      ++it)
    {
      HwfstTransitionIndex * i = *it;
      i->display();
    }
  for(HwfstTransitionSet::iterator it = transitions.begin();
      it != transitions.end();
      ++it)
    {
      HwfstTransition * t = *it;
      t->display();
    }
  if (final)
    {
      std::cerr << id << std::endl;
    }
}
#endif

void HwfstFstState::add_state_indices(IndexVector * transition_index_table)
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
      //transition_index_table->at(i).target = 1;
      memcpy(&(transition_index_table->at(i).target),&weight,sizeof(float)); 
    }

  ++i;

  for (HwfstTransitionIndexSet::iterator it = transition_indices.begin();
       it != transition_indices.end();
       ++it)
    {
      HwfstTransitionIndex * ind = *it;
      ind->write(transition_index_table,
		 i);
    }
}

void HwfstTransitionTableIndices::get_more_space(void)
{
  for(SymbolNumber i = 0; i < number_of_input_symbols + 1; ++i)
    {
      indices.push_back(EMPTY);
    }
}

bool HwfstTransitionTableIndices::state_fits(SymbolNumberSet * input_symbols,
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

void HwfstTransitionTableIndices::insert_state(SymbolNumberSet * input_symbols,
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

PlaceHolderVector::size_type HwfstTransitionTableIndices::last_full_index(void)
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
HwfstTransitionTableIndices::add_state(HwfstFstState * state)
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

  //  if (lower_bound_test_count >= 10)
  //  {
  //    lower_bound_test_count = 0;
  //    ++lower_bound;
  //  }
#ifdef DEBUG
  std::cerr << lower_bound << " " << lower_bound_test_count << " " << indices.size() << " " << state->number_of_input_symbols() << "\r";
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

void HwfstFst::read_nodes(void)
{
  for(StateIdNumber id = 0;
      id < fst_state_id_numbers->get_number_of_nodes();
      ++id)
    {
      StateId n = fst_state_id_numbers->get_id_node(id);
      HwfstFstState * state = new HwfstFstState(n,fst);
      states.push_back(state);
    }
}

void HwfstFst::set_transition_table_indices(void)
{
  TransitionTableIndex place = 0;
  for (HwfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HwfstFstState * state = *it;
      place = state->set_transition_table_indices(place);
    }
}

struct fst_state_compare {
  bool operator() (const HwfstFstState * s1,
		   const HwfstFstState * s2) const
  {
    if (s1->transition_indices.size() < s2->transition_indices.size())
      {
	return true;
      }
    return s1->id < s2->id;
  }
};

typedef std::set<HwfstFstState*,fst_state_compare> StateSet ;

void HwfstFst::set_target_node_places(void)
{
  StateSet state_set;

  for (HwfstStateVector::iterator it = states.begin()+1;
       it != states.end();
       ++it)
    {
      state_set.insert(*it);
    }

  HwfstFstState * start_state = *states.begin(); 
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
      HwfstFstState * state = *it;
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

void HwfstFst::add_input_symbols(StateId n,
			    SymbolNumberSet &input_symbols,
			    StateIdSet &visited_nodes)
{
  for(ArcIterator aiter(*fst,n);
      not aiter.Done();
      aiter.Next())
    {
      StdArc a = aiter.Value();
      input_symbols.insert(a.ilabel);
      if (visited_nodes.find(a.nextstate) == visited_nodes.end())
	{
	  visited_nodes.insert(a.nextstate);
	  add_input_symbols(a.nextstate,
			    input_symbols,
			    visited_nodes);
	}
    }
}

SymbolNumber HwfstFst::number_of_input_symbols(void)
{
  SymbolNumberSet input_symbol_set;
  input_symbol_set.insert(0);
  StateIdSet visited_nodes;
  add_input_symbols(fst->Start(),
		    input_symbol_set,
		    visited_nodes);
  #ifdef DEBUG
  std::cerr << "Input symbol set size " << input_symbol_set.size() << std::endl;
  #endif
  return input_symbol_set.size();
}

#ifdef DEBUG
void HwfstFst::display(void)
{
  for (HwfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HwfstFstState * s = *it;
      s->display();
    }
}
#endif

void HwfstFst::build_index_table(TransitionTableIndex index_table_size)
{
  transition_index_table = new IndexVector(index_table_size,
					   EMPTY_TRANSITION_INDEX);
  
  for (HwfstStateVector::iterator it = states.begin();
       it != states.end();
       ++it)
    {
      HwfstFstState * state = *it;
      state->add_state_indices(transition_index_table);
    }
  
  //#ifdef DEBUG
  //std::cerr << "transition index table" << std::endl;
  //for(IndexVector::iterator it = transition_index_table->begin();
  //    it != transition_index_table->end();
  //    ++it)
  //  {
  //    std::cerr << it->input_symbol << " " << it->target << std::endl;
  //  }
  //std::cerr << std::endl;
  //#endif
}

void HwfstFst::write_index_table(void)
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

TransitionTableIndex HwfstFst::count_transitions(void)
{
  TransitionTableIndex transition_count = 0;
  for (HwfstStateVector::iterator it = states.begin();
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

void HwfstFst::write_transition_targets()
{
  TransitionTableIndex place = 0;
  for(HwfstStateVector::iterator it = states.begin();
      it != states.end();
      ++it)
    {
      HwfstFstState * state = *it;
      place =
      state->write_transitions(output_file,place);
    }
  HwfstTransition::write_empty_transition(output_file,false,INFINITE_WEIGHT);
}

#ifdef DEBUG
void test_1(void);
void test_2(void);
void test_3(char * p);
#endif

#ifdef DEBUG
int main(int argc, char * argv[])
{
  //  std::cerr << "test_1: Make a transducer with one transition from" 
  //	    << std::endl
  //	    << "root to root with pair a:b. Create the HwfstTransition * and"
  //	    << std::endl
  //	    << "display it." << std::endl << std::endl;
  //test_1();
  //std::cerr << std::endl;
  //std::cerr << "test_2: Same as test_1 except do everything through"
  //	    << std::endl
  //	    << "Stste." << std::endl << std::endl;
  //test_2();
  
  if (argc != 2)
    {
      fprintf(stderr,"Usage: %s FILE\n",argv[0]);
      exit(1);
    }
  std::cerr << "Read a transducer from a file and display it.";
  std::cerr << std::endl;
  
  test_3(argv[1]);

}
#endif

#ifdef DEBUG 
void test_1(void)
{
  HWFST::KeyTable * kt = HWFST::create_key_table();
  HWFST::Symbol epsilon = HWFST::define_symbol("<>");
  HWFST::Symbol sym_a = HWFST::define_symbol("a");
  HWFST::Symbol sym_b = HWFST::define_symbol("b");
  HWFST::associate_key(0,kt,epsilon);
  HWFST::associate_key(1,kt,sym_a);
  HWFST::associate_key(2,kt,sym_b);
  HwfstTransition::set_key_table(kt);
  TransduceR * t = new TransduceR();

  StateId root = t->Start();
  t->SetFinal(root,1);
  t->AddArc(root,StdArc(1,2,0,root));
  //root->add_arc(transition_label(1,2),root,t);
  t->AddArc(root,StdArc(2,2,0,root));
  ///root->add_arc(transition_label(2,2),root,t);
  HwfstIdNumberMap * ids = new HwfstIdNumberMap(t);
  HwfstTransition::set_id_numbers(ids);
  for (ArcIterator aiter(*t,root);
       not aiter.Done();
       aiter.Next())
    {
      StdArc a = aiter.Value();
      HwfstTransition tr(root,a);
      tr.display();
    }
  delete kt;
  delete ids;
  delete t;
}

void test_2(void)
{
  HWFST::KeyTable * kt = HWFST::create_key_table();
  HWFST::Symbol epsilon = HWFST::define_symbol("<>");
  HWFST::Symbol sym_a = HWFST::define_symbol("A");
  HWFST::Symbol sym_b = HWFST::define_symbol("B");
  HWFST::associate_key(0,kt,epsilon);
  HWFST::associate_key(1,kt,sym_a);
  HWFST::associate_key(2,kt,sym_b);
  HwfstFstState::set_key_table(kt);

  TransduceR * t = new TransduceR();

  StateId root = t->Start();
  t->SetFinal(root,1);
  //root->set_final(1);
  t->AddArc(root,StdArc(1,1,0,root));
  //root->add_arc(transition_label(1,1),root,t);
  t->AddArc(root,StdArc(1,2,0,root));
  //root->add_arc(transition_label(1,2),root,t);
  t->AddArc(root,StdArc(2,0,0,root));
  //root->add_arc(transition_label(2,0),root,t);
  t->AddArc(root,StdArc(2,2,0,root));
  //root->add_arc(transition_label(2,2),root,t);
  HwfstIdNumberMap * ids = new HwfstIdNumberMap(t);
  HwfstFstState::set_id_numbers(ids);
  HwfstFstState state(root,t);
  state.display();
}


void test_3(char * p)
{
  HWFST::KeyTable * kt = HWFST::create_key_table();
  std::cerr << "Reading transducer" << std::endl;
  HWFST::TransducerHandle th = 
    HWFST::read_transducer(p,kt);
  //HWFST::print_transducer(th,kt);
  std::cerr << "Done." << std::endl;
  TransduceR * t = HANDLE_TO_PINSTANCE(TransduceR,th); 
  HWFST::KeyTable * new_kt = reorder_key_table(kt,t);
  HWFST::TransducerHandle new_th =
  HWFST::harmonize_transducer(th,kt,new_kt);
  HWFST::delete_transducer(th);
  t = HANDLE_TO_PINSTANCE(TransduceR,new_th); 
  char line[1000];
  line[0] = 0;
  strcat(line,p);
  strcat(line,".rt");
  FILE * out_file = fopen(line,"w");
  t->SetStart(0);
  HwfstFst fst(t,
  	  new_kt,
  	  out_file);
  fclose(out_file);
  //  fst.display();
}
#endif

