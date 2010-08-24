//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "ConvertTransducerFormat.h"
#include "optimized-lookup/convert.h"

namespace hfst { namespace implementations
{
InternalTransducer * sfst_to_internal_format(SFST::Transducer * t)
{
  InternalTransducer * internal_transducer = new fst::StdVectorFst();
  StateId start_state = internal_transducer->AddState();
  internal_transducer->SetStart(start_state);

  SfstStateVector agenda;
  agenda.push_back(t->root_node());

  SfstToOfstStateMap state_map;
  state_map[t->root_node()] = start_state;

  if (t->root_node()->is_final())
    { internal_transducer->SetFinal(start_state,0); }

  while (not agenda.empty())
    {
      SFST::Node * current_node = agenda.back();
      agenda.pop_back();

      StateId current_state = state_map[current_node];

      for (SFST::ArcsIter it(current_node->arcs()); it; it++)
	{
	  SFST::Arc * arc = it;

	  if (state_map.find(arc->target_node()) == state_map.end())
	    {
	      state_map[arc->target_node()] = internal_transducer->AddState();
	      agenda.push_back(arc->target_node());

	      if (arc->target_node()->is_final())
		{
		  internal_transducer->SetFinal
		    (state_map[arc->target_node()],0);
		}
	    }

	  internal_transducer->AddArc
	    (current_state,
	     fst::StdArc(arc->label().lower_char(),
			 arc->label().upper_char(),
			 0,
			 state_map[arc->target_node()]));
	}
    }

  // added
  fst::SymbolTable *st = new fst::SymbolTable("anonym_hfst3_symbol_table");
  SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
  for (SFST::Alphabet::CharMap::const_iterator it = cm.begin(); it != cm.end(); it++) {
    st->AddSymbol(std::string(it->second), (int64)it->first);
  }
  // check that this works
  internal_transducer->SetInputSymbols(st);
  //internal_transducer->SetOutputSymbols(st);

  return internal_transducer;
}

// Both input and output symbol tables of internal format will contain
// all symbols in the sigma of the foma transducer
InternalTransducer * foma_to_internal_format(struct fsm * t)
{
  InternalTransducer * internal_transducer = new fst::StdVectorFst();
  FomaToOfstStateMap state_map;
  bool start_state_found=false;
  StateId start_state_id=0;
  
  struct fsm_state *fsm;
  fsm = t->states;
  
  // For every line in foma transducer:
  for (int i=0; (fsm+i)->state_no != -1; i++) {


    // 1. Find the corresponding source state in internal transducer
    //    or create it, if not found.
    StateId source_state;
    if (state_map.find((fsm+i)->state_no) == state_map.end()) {
      source_state = internal_transducer->AddState();
      state_map[(fsm+i)->state_no] = source_state; 
    }
    else
      source_state = state_map[(fsm+i)->state_no];

    // 2. If the source state is an initial state in foma:
    if ((fsm+i)->start_state == 1) {
      // If the start state has not yet been encountered,
      if (not start_state_found) {
	// mark the source state as initial.
	internal_transducer->SetStart(source_state);
	start_state_id = source_state;
	start_state_found=true;
      }
      // If the start state is encountered again, 
      else if (source_state == start_state_id) {
	// do nothing.
      }
      // If there are several initial states in foma transducer,
      else {
	// throw an exception.
	throw TransducerHasMoreThanOneStartStateException();
      }
    }


    // 3. If there are transitions leaving from the state,
    if ((fsm+i)->target != -1) {  

      // find the corresponding source state in internal transducer
      // or create it, if not found, and
      StateId target_state;
      if (state_map.find((fsm+i)->target) == state_map.end()) {
	target_state = internal_transducer->AddState();
	state_map[(fsm+i)->target] = target_state; 
      }
      else
	target_state = state_map[(fsm+i)->target];
      
      // create the transition between source and target states. 
      internal_transducer->AddArc
	(source_state,
	 fst::StdArc((fsm+i)->in,
		     (fsm+i)->out,
		     0,
		     target_state));
    }


    // 4. If the source state is final in foma,
    if ((fsm+i)->final_state == 1) {
      // mark it as final in internal transducer.
      internal_transducer->SetFinal(source_state,0); 
    }

  }

  // If there was not an initial state in foma transducer,
  if (not start_state_found) {
    // throw an exception.
    throw TransducerHasNoStartStateException();
  }


  // Convert sigma to SymbolTable
  fst::SymbolTable *st = new fst::SymbolTable("anonym_hfst3_symbol_table");
  st->AddSymbol("@_EPSILON_SYMBOL_@", 0);
  st->AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
  st->AddSymbol("@_IDENTITY_SYMBOL_@", 2);
  struct sigma * p = t->sigma;
  while (p != NULL) {
    if (p->symbol == NULL)
      break;
    st->AddSymbol(std::string(p->symbol), (int64)p->number);  // check that here are no problems
    p = p->next;
  }
  // check that this works
  internal_transducer->SetInputSymbols(st);
  //internal_transducer->SetOutputSymbols(st);

  return internal_transducer;
}


InternalTransducer * 
tropical_ofst_to_internal_format(fst::StdVectorFst * t)
{ fst::StdVectorFst *retval = new fst::StdVectorFst(*t); 
  assert (t->InputSymbols() != NULL);
  retval->SetInputSymbols(new fst::SymbolTable(*(t->InputSymbols())));
  return retval;
}

InternalTransducer * log_ofst_to_internal_format
(LogFst * t)
{ //return new fst::StdVectorFst(*dynamic_cast<fst::StdVectorFst*>(t)); }
  InternalTransducer * u = new InternalTransducer;
  fst::Cast<LogFst,InternalTransducer>(*t,u);
  assert (t->InputSymbols() != NULL);
  u->SetInputSymbols(new fst::SymbolTable(*(t->InputSymbols())));
  return u;
}


StateId hfst_ol_to_internal_add_state(hfst_ol::Transducer* t, 
                                      InternalTransducer* internal_transducer,
                                      HfstOlToOfstStateMap& state_map,
                                      bool weighted,
                                      hfst_ol::TransitionTableIndex index)
{
  StateId new_state = internal_transducer->AddState();
  state_map[index] = new_state;
  
  if(hfst_ol::indexes_transition_index_table(index))
  {
    const hfst_ol::TransitionIndex& transition_index = t->get_index(index);
    
    if(transition_index.final())
    {
      internal_transducer->SetFinal(new_state,
        weighted ? dynamic_cast<const hfst_ol::TransitionWIndex&>(transition_index).final_weight() : 0);
    }
  }
  else // indexes transition table
  {
    const hfst_ol::Transition& transition = t->get_transition(index);
    
    if(transition.final())
    {
      internal_transducer->SetFinal(new_state,
        weighted ? dynamic_cast<const hfst_ol::TransitionW&>(transition).get_weight() : 0);
    }
  }
  return new_state;
}

InternalTransducer * hfst_ol_to_internal_format(hfst_ol::Transducer * t)
{
  bool weighted = t->get_header().probe_flag(hfst_ol::Weighted);
  InternalTransducer * internal_transducer = new fst::StdVectorFst();
  
  // This contains indices to either (1) the start of a set of entries in the
  // transition index table, or (2) the boundary before a set of entries in the
  // transition table; in this case, the following entries will all have the
  // same input symbol. In either case the index represents a state and may be final
  // The will already be an entry in state_map for each value in agenda
  std::vector<hfst_ol::TransitionTableIndex> agenda;
  HfstOlToOfstStateMap state_map;
  
  internal_transducer->SetStart(hfst_ol_to_internal_add_state(t, internal_transducer, state_map, weighted, 0));
  agenda.push_back(0);
  while(!agenda.empty())
  {
    hfst_ol::TransitionTableIndex current_index = agenda.back();
    agenda.pop_back();
    
    StateId current_state = state_map[current_index];
    
    hfst_ol::TransitionTableIndexSet transitions = t->get_transitions_from_state(current_index);
    for(hfst_ol::TransitionTableIndexSet::const_iterator it=transitions.begin();it!=transitions.end();it++)
    {
      const hfst_ol::Transition& transition = t->get_transition(*it);
      
      if(state_map.find(transition.get_target()) == state_map.end())
      {
        hfst_ol_to_internal_add_state(t, internal_transducer, state_map, weighted, transition.get_target());
        agenda.push_back(transition.get_target());
      }
      
      internal_transducer->AddArc(current_state,
        fst::StdArc(transition.get_input_symbol(), 
                    transition.get_output_symbol(),
                    weighted ? dynamic_cast<const hfst_ol::TransitionW&>(transition).get_weight() : 0,
                    state_map[transition.get_target()]));
    }
  }
  
  //transfer the symbol table
  fst::SymbolTable *st = new fst::SymbolTable("anonym_hfst3_symbol_table");
  const hfst_ol::SymbolTable& symbols = t->get_alphabet().get_symbol_table();
  for(size_t i=0; i<symbols.size(); i++)
    st->AddSymbol(symbols[i], i);
  internal_transducer->SetInputSymbols(st);
  
  return internal_transducer;
}

SFST::Transducer *  internal_format_to_sfst
(InternalTransducer * internal_transducer)
{

  SFST::Transducer * t = new SFST::Transducer();
  try {
  SFST::Node * start_node = t->root_node();

  if (internal_transducer->Start() == fst::kNoStateId)
    { throw TransducerHasNoStartStateException(); }
  StateId start_state = internal_transducer->Start();

  OfstStateVector agenda;
  agenda.push_back(start_state);

  OfstToSfstStateMap state_map;
  state_map[start_state] = start_node;

  if (internal_transducer->Final(start_state) != fst::TropicalWeight::Zero())
    { start_node->set_final(1); }

  while (not agenda.empty())
    {
      StateId current_state = agenda.back();
      agenda.pop_back();

      SFST::Node * current_node = state_map[current_state];

      for (StdArcIterator it(*internal_transducer,current_state);
	   not it.Done();
	   it.Next())
	{
	  const fst::StdArc &arc = it.Value();

	  if (state_map.find(arc.nextstate) == state_map.end())
	    {
	      state_map[arc.nextstate] = t->new_node();
	      if (internal_transducer->Final(arc.nextstate) 
		  != fst::TropicalWeight::Zero())
		{ state_map[arc.nextstate]->set_final(1); }
	      agenda.push_back(arc.nextstate);
	    }

	  current_node->add_arc(SFST::Label(arc.ilabel,arc.olabel),
				state_map[arc.nextstate],
				t);
	}
    }
  // added
  /*if (internal_transducer->InputSymbols() != internal_transducer->OutputSymbols())
    throw hfst::exceptions::ErrorException();*/
  
  for ( fst::SymbolTableIterator it = fst::SymbolTableIterator(*internal_transducer->InputSymbols());
	not it.Done(); it.Next() ) {
    t->alphabet.add_symbol( it.Symbol().c_str(), (SFST::Character)it.Value() );
  }
  
  }
  catch (char *msg) {
    fprintf(stderr, "ERROR: %s \n", msg);
  }
  return t;
}

// SymbolTable is converted to sigma, but the string-to-number
// relations are changed...
struct fsm * internal_format_to_foma
(InternalTransducer * internal_transducer)
{ 
  if (internal_transducer->Start() == fst::kNoStateId)
    { throw TransducerHasNoStartStateException(); }

  /*if (internal_transducer->InputSymbols() != internal_transducer->OutputSymbols()) {
    fprintf(stderr, "internal_format_to_foma (2)\n"); 
    throw hfst::exceptions::ErrorException(); 
    }*/

  struct fsm_construct_handle *h;
  struct fsm *net;
  h = fsm_construct_init(strdup(std::string("").c_str()));


  for (fst::StateIterator<fst::StdVectorFst> siter(*internal_transducer); 
       not siter.Done(); siter.Next())
    {
      StateId source_state = siter.Value();
      for (fst::ArcIterator<fst::StdVectorFst> aiter(*internal_transducer,source_state);
	   not aiter.Done(); aiter.Next())
	{
	  const fst::StdArc &arc = aiter.Value();
	  
	  const fst::SymbolTable * isymbols = internal_transducer->InputSymbols();
	  std::string istring = isymbols->Find((int64)arc.ilabel);  // if not found, SEGFAULT
	  char *in = strdup(istring.c_str());

	  //fst::SymbolTable * osymbols = internal_transducer->OutputSymbols();
	  std::string ostring = isymbols->Find((int64)arc.olabel);
	  char *out = strdup(ostring.c_str());

	  fsm_construct_add_arc(h, (int)source_state, (int)arc.nextstate, in, out);
	  // not clear whether in and out should be freed...
	}
      if (internal_transducer->Final(source_state) != fst::TropicalWeight::Zero()) {
	fsm_construct_set_final(h, (int)source_state);
      }	  
    }

  // Add symbols that are in the symbol table but do not occur in the transducer
  for (fst::SymbolTableIterator it = fst::SymbolTableIterator(*internal_transducer->InputSymbols());
       not it.Done(); it.Next()) 
    {
      char * symbol = strdup( it.Symbol().c_str() );
      if ( fsm_construct_check_symbol(h,symbol) == -1 ) // not found
	fsm_construct_add_symbol(h,symbol);
    }

  fsm_construct_set_initial(h, (int)internal_transducer->Start());
  // not clear what happens if start state is not number zero...
  net = fsm_construct_done(h);
  fsm_count(net);
  net = fsm_topsort(net);

  return net;      
}

fst::StdVectorFst * internal_format_to_openfst(InternalTransducer * t)
{ fst::StdVectorFst *retval = new fst::StdVectorFst(*t); 
  assert (t->InputSymbols() != NULL);
  retval->SetInputSymbols(new fst::SymbolTable(*(t->InputSymbols())));
  return retval;
}


LogFst * internal_format_to_log_ofst(InternalTransducer * t)
{ LogFst * u = new LogFst;
  fst::Cast<InternalTransducer,LogFst>(*t,u);
  assert (t->InputSymbols() != NULL);
  u->SetInputSymbols(new fst::SymbolTable(*(t->InputSymbols())));
  return u; }

hfst_ol::Transducer * internal_format_to_hfst_ol(InternalTransducer * t, bool weighted)
{
  hfst_ol::ConvertTransducer conv(t, weighted);
  return conv.to_transducer();
}



/* -----------------
   The new functions.
   ----------------- */


#if HAVE_SFST
void sfst_to_internal( SFST::Node *node, SFST::NodeNumbering &index, 
		       std::set<SFST::Node*> visited_nodes, HfstInternalTransducer *internal ) {

  if (visited_nodes.find(node) == visited_nodes.end() ) { // if node has not been visited
    visited_nodes.insert(node);
    SFST::Arcs *arcs=node->arcs();
    for( SFST::ArcsIter p(arcs); p; p++ ) {
      SFST::Arc *arc=p;
      internal->add_line(index[node], index[arc->target_node()],
			 arc->label().lower_char(), arc->label().upper_char(), 0);
    }
    if (node->is_final())
      internal->add_line(index[node],0);
    for( SFST::ArcsIter p(arcs); p; p++ ) {
      SFST::Arc *arc=p;
      sfst_to_internal( arc->target_node(), index, visited_nodes, internal);
    }
  }
}

HfstInternalTransducer * sfst_to_internal_hfst_format(SFST::Transducer * t) {
  
  HfstInternalTransducer * internal_transducer = new HfstInternalTransducer();
  SFST::NodeNumbering index(*t);
  std::set<SFST::Node*> visited_nodes;
  sfst_to_internal(t->root_node(), index, visited_nodes, internal_transducer);

  HfstAlphabet * alpha = new HfstAlphabet();
  SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
  for (SFST::Alphabet::CharMap::const_iterator it = cm.begin(); it != cm.end(); it++)
    alpha->add_symbol(it->second, it->first);
  internal_transducer->alphabet = alpha;

  // testing
  //internal_transducer->print_symbol(stderr);

  return internal_transducer;
}
#endif

#if HAVE_SFST
SFST::Transducer * hfst_internal_format_to_sfst(HfstInternalTransducer * internal) {

  SFST::Transducer * t = new SFST::Transducer();
  if (internal->has_no_lines())
    return t;

  std::map<unsigned int,SFST::Node*> state_map;
  state_map[0] = t->root_node();

  std::vector<InternalTransducerLine> *lines = internal->get_lines();
  for (std::vector<InternalTransducerLine>::iterator it = lines->begin();
       it != lines->end(); it++)
    {
      if (it->final_line) 
	{
	  if (state_map.find(it->origin) == state_map.end())
	    state_map[it->origin] = t->new_node();	    
	  state_map[it->origin]->set_final(1);
	}
      else 
	{
	  if (state_map.find(it->origin) == state_map.end())
	    state_map[it->origin] = t->new_node();
	  if (state_map.find(it->target) == state_map.end())
	    state_map[it->target] = t->new_node();
	  state_map[it->origin]->add_arc(SFST::Label(it->isymbol,it->osymbol),
					 state_map[it->target],t);
	}
    }
  
  HfstAlphabet::CharMap cm = internal->alphabet->get_char_map();
  for (HfstAlphabet::CharMap::const_iterator it = cm.begin(); it != cm.end(); it++)
    t->alphabet.add_symbol(it->second, it->first);

  return t;
}
#endif



#if HAVE_FOMA
HfstInternalTransducer * foma_to_internal_hfst_format(struct fsm * t) {
  return NULL; }
#endif

#if HAVE_OPENFST
HfstInternalTransducer * tropical_ofst_to_internal_hfst_format(fst::StdVectorFst * t) {
  return NULL; }

HfstInternalTransducer * log_ofst_to_internal_hfst_format(LogFst * t) {
  return NULL; }

HfstInternalTransducer * hfst_ol_to_internal_hfst_format(hfst_ol::Transducer * t) {
  return NULL; }
#endif


#if HAVE_FOMA
struct fsm * hfst_internal_format_to_foma(InternalTransducer * t) {
  return NULL; }
#endif

#if HAVE_OPENFST
fst::StdVectorFst * hfst_internal_format_to_openfst(HfstInternalTransducer * t) {
  return NULL; }

LogFst * hfst_internal_format_to_log_ofst(HfstInternalTransducer * t) {
  return NULL; }

hfst_ol::Transducer * hfst_internal_format_to_hfst_ol(HfstInternalTransducer * t, bool weighted) {
  return NULL; }
#endif


} }
#ifdef DEBUG_CONVERT
/********************************
 *                              *
 *          TEST MAIN           *
 *                              *
 ********************************/
int main(void)
{
  hfst::KeyTable * key_table = hfst::create_key_table();
  hfst::TransducerHandle t;
  HWFST::TransducerHandle tw;
  if (hfst::read_format() == 0)
    {
      try
	{
	  t = hfst::read_transducer(std::cin,key_table);
	}
      catch (const char * p)
	{
	  std::cerr << "ERROR: " << p << std::endl;
	}
      tw = sfst_to_internal_format(t);
      HWFST::write_transducer(tw,key_table);
    }
  else
    {
      try
	{
	  tw = HWFST::read_transducer(std::cin,key_table);
	}
      catch (const char * p)
	{
	  std::cerr << "ERROR: " << p << std::endl;
	}
      t = internal_format_to_sfst(tw);
      hfst::write_transducer(t,key_table);
    }
}
#endif
