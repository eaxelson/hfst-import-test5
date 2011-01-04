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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "ConvertTransducerFormat.h"
#include "optimized-lookup/convert.h"
#include "HfstNet.h"
#include "HfstTransducer.h"

#ifndef DEBUG_MAIN
namespace hfst { namespace implementations
{


  HfstFsm * ConversionFunctions::hfst_transducer_to_hfst_net
  (const hfst::HfstTransducer &t) {

#if HAVE_SFST
    if (t.type == SFST_TYPE)
      return sfst_to_hfst_net(t.implementation.sfst); 
#endif // HAVE_SFST

#if HAVE_OPENFST
    if (t.type == TROPICAL_OFST_TYPE)
      return tropical_ofst_to_hfst_net(t.implementation.tropical_ofst); 
    if (t.type == LOG_OFST_TYPE)
      return log_ofst_to_hfst_net(t.implementation.log_ofst); 
#endif // HAVE_SFST

#if HAVE_FOMA
    if (t.type == FOMA_TYPE)
      return foma_to_hfst_net(t.implementation.foma); 
#endif // HAVE_SFST

#if HAVE_MFSTL
    if (t.type == MFSTL_TYPE)
      return mfstl_to_hfst_net(t.implementation.mfstl); 
#endif // HAVE_SFST

    if (t.type == HFST_OL_TYPE || t.type == HFST_OLW_TYPE)
      return hfst_ol_to_hfst_net(t.implementation.hfst_ol);

    throw hfst::exceptions::FunctionNotImplementedException();
  }


  /* -----------------------------------------------------------

      Conversion functions between HfstFsm and SFST transducer. 

     ----------------------------------------------------------- */


#if HAVE_SFST

  /* Recursively copy all transitions of \a node to \a net.
     Used by function sfst_to_hfst_net(SFST::Transducer * t).

     @param node  The current node in the SFST transducer
     @param index  A map that maps nodes to integers
     @param visited_nodes  Which nodes have already been visited
     @param net  The HfstFsm that is being created
     @param alphabet  The alphabet of the SFST transducer
  */
  void ConversionFunctions::
  sfst_to_hfst_net( SFST::Node *node, SFST::NodeNumbering &index, 
		    std::set<SFST::Node*> &visited_nodes, 
		    HfstFsm *net, SFST::Alphabet &alphabet ) {
  
    // If node has not been visited before
    if (visited_nodes.find(node) == visited_nodes.end() ) { 
      visited_nodes.insert(node);
      SFST::Arcs *arcs=node->arcs();

      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	SFST::Arc *arc=p;

	std::string istring
	  (alphabet.code2symbol(arc->label().lower_char()));
	std::string ostring
	  (alphabet.code2symbol(arc->label().upper_char()));

	if (istring.compare("<>") == 0) {
	  istring = std::string("@_EPSILON_SYMBOL_@");
	}
	if (ostring.compare("<>") == 0) {
	  ostring = std::string("@_EPSILON_SYMBOL_@");
	}

	net->add_transition(index[node], 
			    HfstArc
			    (index[arc->target_node()],
			     istring,
			     ostring,
			     0));
      }

      if (node->is_final())
	net->set_final_weight(index[node],0);

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	SFST::Arc *arc=p;
	sfst_to_hfst_net(arc->target_node(), index, 
			 visited_nodes, 
			 net, alphabet);
      }
    }
  }


  /* Create an HfstFsm equivalent to an SFST transducer \a t. */
  HfstFsm * ConversionFunctions::
  sfst_to_hfst_net(SFST::Transducer * t) {
  
    HfstFsm * net = new HfstFsm();
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);
    // The set of nodes that have been visited
    std::set<SFST::Node*> visited_nodes;
   
    sfst_to_hfst_net(t->root_node(), index, 
		     visited_nodes, 
		     net, t->alphabet);
    
    // Make sure that also symbols that occur in the alphabet of the
    // transducer t but not in its transitions are inserted to net
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it 
	   = cm.begin(); it != cm.end(); it++) 
      {
	if (it->first != 0) // The epsilon symbol "<>" is not inserted
	  {
	    net->alphabet.insert(std::string(it->second));
	  }
      }
    
    return net;
  }


  /* Create an SFST::Transducer equivalent to HfstFsm \a net. */
  SFST::Transducer * ConversionFunctions::
  hfst_net_to_sfst(const HfstFsm * net) {

  SFST::Transducer * t = new SFST::Transducer();

  // Map that maps states of \a net to SFST nodes
  std::map<HfstState, SFST::Node*> state_map;
  state_map[0] = t->root_node();

  // Go through all states
  for (HfstFsm::const_iterator it = net->begin();
       it != net->end(); it++)
    {
      // Go through the set of transitions in each state
      for (HfstFsm::HfstTransitionSet::iterator tr_it = it->second.begin();
	   tr_it != it->second.end(); tr_it++)
	{
	  TransitionData data = tr_it->get_transition_data();

	  // Create new nodes, if needed
	  if (state_map.find(it->first) == state_map.end())
	    state_map[it->first] = t->new_node();

	  if (state_map.find(tr_it->get_target_state()) == state_map.end())
	    state_map[tr_it->get_target_state()] = t->new_node();

	  std::string istring(data.get_input_symbol());
	  std::string ostring(data.get_output_symbol());

	  if (data.get_input_symbol().compare("@_EPSILON_SYMBOL_@") == 0) {
	    istring = std::string("<>");
	  }

	  if (data.get_output_symbol().compare("@_EPSILON_SYMBOL_@") == 0) {
	    ostring = std::string("<>");
	  }

	  SFST::Label l
	    (t->alphabet.add_symbol(istring.c_str()),
	     t->alphabet.add_symbol(ostring.c_str()));
	  
	  // Copy transition to node
	  state_map[it->first]->add_arc(l,
					state_map[tr_it->get_target_state()],
					t);
	}
    }

  // Go through the final states
  for (HfstFsm::FinalWeightMap::const_iterator it = net->final_weight_map.begin();
       it != net->final_weight_map.end(); it++) 
    {
      if (state_map.find(it->first) == state_map.end())
	state_map[it->first] = t->new_node();
      state_map[it->first]->set_final(1);
    }

  // Make sure that also symbols that occur in the alphabet of the
  // HfstFsm but not in its transitions are inserted to the SFST transducer
  for (HfstFsm::HfstNetAlphabet::iterator it = net->alphabet.begin();
       it != net->alphabet.end(); it++) {
    if (it->compare("@_EPSILON_SYMBOL_@") != 0)
      t->alphabet.add_symbol(it->c_str());
  }
  
  return t;
}

#endif // HAVE_SFST




  /* -----------------------------------------------------------

      Conversion functions between HfstFsm and foma transducer. 

      ---------------------------------------------------------- */


#if HAVE_FOMA

  /* Create an HfstFsm equivalent to foma transducer \a t. */
  HfstFsm * ConversionFunctions::
  foma_to_hfst_net(struct fsm * t) {

  HfstFsm * net = new HfstFsm();
  struct fsm_state *fsm;
  fsm = t->states;
  int start_state_id = -1;
  bool start_state_found=false;

  // For every line in foma transducer:
  for (int i=0; (fsm+i)->state_no != -1; i++) {    

    // 1. If the source state is an initial state in foma:
    if ((fsm+i)->start_state == 1) 
      {
	// If the start state has not yet been encountered.
	if (not start_state_found) {
	  start_state_id = (fsm+i)->state_no;
	  //if (start_state_id != 0) {
	  //  throw ErrorException();
	  //}
	  start_state_found=true;
	}
	// If the start state is encountered again, 
	else if ((fsm+i)->state_no == start_state_id) {
	  // do nothing.
	}
	// If there are several initial states in foma transducer,
	else {
	  // throw an exception.
	  throw TransducerHasMoreThanOneStartStateException();
	}
      }

    // 2. If there are transitions leaving from the state,
    if ((fsm+i)->target != -1) 
      {
	// copy the transition.
	net->add_transition
	  ((fsm+i)->state_no,
	   HfstArc
	   ((fsm+i)->target,
	    std::string (sigma_string((fsm+i)->in, t->sigma)), 
	    std::string (sigma_string((fsm+i)->out, t->sigma)),
	    0));      
      }
    
    // 3. If the source state is final in foma,
    if ((fsm+i)->final_state == 1) 
      {
	// set the state as final.
	net->set_final_weight((fsm+i)->state_no, 0);
      }
    
  }

  // If there was not an initial state in foma transducer,
  if (not start_state_found) {
    // throw an exception.
    throw TransducerHasNoStartStateException();
  }
  
  /* If start state number (N) is not zero, swap state numbers N and zero 
     in internal transducer. TODO */
  if (start_state_id != 0)
    net->swap_state_numbers(start_state_id,0);

  /* Make sure that also the symbols that occur only in the alphabet
     but not in transitions are copied. */
  struct sigma * p = t->sigma;
  while (p != NULL) {
    if (p->symbol == NULL)
      break;
    net->alphabet.insert(std::string(p->symbol));
    p = p->next;
  }

  return net;
}


  /* Create a foma transducer equivalent to HfstFsm \a hfst_fsm. */
  struct fsm * ConversionFunctions::
    hfst_net_to_foma(const HfstFsm * hfst_fsm) {
    
    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    // Go through all states
    for (HfstFsm::const_iterator it = hfst_fsm->begin();
	 it != hfst_fsm->end(); it++)
      {
	// Go through the set of transitions in each state
	for (HfstFsm::HfstTransitionSet::iterator tr_it 
	       = it->second.begin();
	     tr_it != it->second.end(); tr_it++)
	  {
	    TransitionData data = tr_it->get_transition_data();

	    // Copy the transition
	    fsm_construct_add_arc(h, 
				  (int)it->first, 
				  (int)tr_it->get_target_state(),
				  strdup(data.get_input_symbol().c_str()),
				  strdup(data.get_output_symbol().c_str()) );
	  }
      }
    
    // Go through the final states
    for (HfstFsm::FinalWeightMap::const_iterator it 
	   = hfst_fsm->final_weight_map.begin();
	 it != hfst_fsm->final_weight_map.end(); it++) 
      {
	// Set the state as final
	fsm_construct_set_final(h, (int)it->first);
      }
    
    /* Make sure that also the symbols that occur only in the alphabet
       but not in transitions are copied. */
    for (HfstFsm::HfstNetAlphabet::iterator it 
	   = hfst_fsm->alphabet.begin();
	 it != hfst_fsm->alphabet.end(); it++)
      {
	char *symbol = strdup(it->c_str());
	if ( fsm_construct_check_symbol(h,symbol) == -1 ) {
	  fsm_construct_add_symbol(h,symbol);
	}
      }
    
    fsm_construct_set_initial(h, 0);
    net = fsm_construct_done(h);
    fsm_count(net);
    net = fsm_topsort(net);
    
    return net;      
  }

#endif // HAVE_FOMA




  /* --------------------------------------------------------------

      Conversion functions between HfstFsm and OpenFst transducers 

      ------------------------------------------------------------- */


#if HAVE_OPENFST


  /* Create an HfstFsm equivalent to an OpenFst tropical weight
     transducer \a t. */  
  HfstFsm * ConversionFunctions::
  tropical_ofst_to_hfst_net
  (fst::StdVectorFst * t) {

  HfstFsm * net = new HfstFsm();

  // An empty transducer
  if (t->Start() == fst::kNoStateId)
    {      
      for ( fst::SymbolTableIterator it = 
	      fst::SymbolTableIterator(*(t->InputSymbols()));
	    not it.Done(); it.Next() ) {
	net->alphabet.insert( it.Symbol() );
      }    
      return net;
    }      

  /* This takes care that initial state is always number zero
     and state number zero (if it is not initial) is some other number
     (basically as the number of the initial state in that case, i.e.
     the numbers of initial state and state number zero are swapped) */
  StateId zero_print=0;
  StateId initial_state = t->Start();
  if (initial_state != 0) {
    zero_print = initial_state;
  }

  /* Go through all states */
  for (fst::StateIterator<fst::StdVectorFst> siter(*t); 
       not siter.Done(); siter.Next()) 
    {
      StateId s = siter.Value();
      if (s == initial_state) {
	int origin;  // how origin state is printed, see the first comment
	if (s == 0)
	  origin = zero_print;
	else if (s == initial_state)
	  origin = 0;
	else
	  origin = (int)s;

	/* Go through all transitions in a state */
	for (fst::ArcIterator<fst::StdVectorFst> aiter(*t,s); 
	     !aiter.Done(); aiter.Next())
	  {
	    const fst::StdArc &arc = aiter.Value();
	    int target;  // how target state is printed, see the first comment
	    if (arc.nextstate == 0)
	      target = zero_print;
	    else if (arc.nextstate == initial_state)
	      target = 0;
	    else
	      target = (int)arc.nextstate;

	    // Copy the transition
	    net->add_transition(origin, 
				HfstArc
				(target,
				 t->InputSymbols()->Find(arc.ilabel),
				 t->InputSymbols()->Find(arc.olabel),
				 arc.weight.Value()
				 ));
	  }
	if (t->Final(s) != fst::TropicalWeight::Zero()) {
	  // Set the state as final
	  net->set_final_weight(origin, t->Final(s).Value());
	}
	break;
      }
    }

    for (fst::StateIterator<fst::StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	if (s != initial_state) {
	  int origin;  // how origin state is printed, see the first comment
	  if (s == 0)
	    origin = zero_print;
	  else if (s == initial_state)
	    origin = 0;
	  else
	    origin = (int)s;
	  for (fst::ArcIterator<fst::StdVectorFst> aiter(*t,s); 
	       !aiter.Done(); aiter.Next())
	    {
	      const fst::StdArc &arc = aiter.Value();
	      int target;  // how target state is printed, see the first comment
	      if (arc.nextstate == 0)
		target = zero_print;
	      else if (arc.nextstate == initial_state)
		target = 0;
	      else
		target = (int)arc.nextstate;

	      net->add_transition(origin, 
				  HfstArc
				  (target,
				   t->InputSymbols()->Find(arc.ilabel),
				   t->InputSymbols()->Find(arc.olabel),
				   arc.weight.Value()
				   ));
	    }
	  if (t->Final(s) != fst::TropicalWeight::Zero())
	    net->set_final_weight(origin, t->Final(s).Value());
	}
      }

    /* Make sure that also the symbols that occur only in the alphabet
       but not in transitions are copied. */
    for ( fst::SymbolTableIterator it = 
	    fst::SymbolTableIterator(*(t->InputSymbols()));
	  not it.Done(); it.Next() ) {
      net->alphabet.insert( it.Symbol() );
    }    

    return net;
}



  /* Get a state id for a state in transducer \a t that corresponds
     to HfstState s as defined in \a state_map.     
     Used by function hfst_net_to_tropical_ofst. */
  StateId ConversionFunctions::hfst_state_to_state_id
  (HfstState s, std::map<HfstState, StateId> &state_map, 
   fst::StdVectorFst * t)
  {
    std::map<HfstState, StateId>::iterator it = state_map.find(s);
    if (it == state_map.end())
      {
	// If not found, add a state
	StateId retval = t->AddState();
	state_map[s] = retval;
	return retval;
      }
    return it->second;
  }

  /* Create an OpenFst transducer equivalent to HfstFsm \a net. */
  fst::StdVectorFst * ConversionFunctions::hfst_net_to_tropical_ofst
  (const HfstFsm * net) {
    
    fst::StdVectorFst * t = new fst::StdVectorFst();
    StateId start_state = t->AddState();
    t->SetStart(start_state);
    
    // The mapping between states in HfstFsm and StdVectorFst
    std::map<HfstState, StateId> state_map;
    state_map[0] = start_state;
    
    fst::SymbolTable st("");
    st.AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st.AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st.AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    
    // Go through all states
    for (HfstFsm::const_iterator it = net->begin();
	 it != net->end(); it++)
      {
	// Go through the set of transitions in each state
	for (HfstFsm::HfstTransitionSet::iterator tr_it = it->second.begin();
	     tr_it != it->second.end(); tr_it++)
	  {
	    TransitionData data = tr_it->get_transition_data();
	    
	    // Copy the transition
	    t->AddArc( hfst_state_to_state_id(it->first, state_map, t), 
		       fst::StdArc
		       ( st.AddSymbol(data.get_input_symbol()),
			 st.AddSymbol(data.get_output_symbol()),
			 data.get_weight(),
			 hfst_state_to_state_id
 			 (tr_it->get_target_state(), state_map, t)) );
	  }
      }
    
    // Go through the final states
    for (HfstFsm::FinalWeightMap::const_iterator it 
	   = net->final_weight_map.begin();
	 it != net->final_weight_map.end(); it++) 
      {
	t->SetFinal(hfst_state_to_state_id(it->first, state_map, t), 
		    it->second);
      }
    
    // Add also symbols that do not occur in transitions
    for (HfstFsm::HfstNetAlphabet::iterator it = net->alphabet.begin();
	 it != net->alphabet.end(); it++)
      {
	st.AddSymbol(*it);
      }
    
    t->SetInputSymbols(&st);
    return t;  
  }


  /* Create an HfstFsm equivalent to an OpenFst log weight
     transducer \a t. TODO */  
  HfstFsm * ConversionFunctions::log_ofst_to_hfst_net
  (LogFst * t) 
  {
    (void)t;
    throw hfst::exceptions::FunctionNotImplementedException(); 
  }

  /* Create an OpenFst log weight transducer equivalent to
     HfstFsm \a t. TODO */    
  LogFst * ConversionFunctions::hfst_net_to_log_ofst
  (const HfstFsm * t) 
  {
    (void)t;
    throw hfst::exceptions::FunctionNotImplementedException(); 
  }
  

#endif // HAVE_OPENFST



  /* -----------------------------------------------------------

      Conversion functions between HfstFsm and 
      optimized lookup transducers 

      ---------------------------------------------------------- */

  /* Create an HfstFsm equivalent to hfst_ol::Transducer \a t . */
  HfstFsm * ConversionFunctions::
  hfst_ol_to_hfst_net(hfst_ol::Transducer * t)
  {
    (void)t;
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  /* Create an hfst_ol::Transducer equivalent to HfstFsm \a t.
     \a weighted defined whether the created transducer is weighted. */
  hfst_ol::Transducer * ConversionFunctions::
  hfst_net_to_hfst_ol
  (HfstFsm * t, bool weighted)
  {
    (void)t;
    throw hfst::exceptions::FunctionNotImplementedException();
  }




#if HAVE_MFSTL

  HfstFsm * ConversionFunctions::
  mfstl_to_hfst_net(mfstl::MyFst * t) {
  (void)t;
  throw hfst::exceptions::FunctionNotImplementedException();
}

  mfstl::MyFst * ConversionFunctions::
  hfst_net_to_mfstl(const HfstFsm * t) {
  (void)t;
  throw hfst::exceptions::FunctionNotImplementedException();
}

#endif // HAVE_MFSTL


} }

#else  // #ifndef DEBUG_MAIN
#include <cstdlib>
#include <cassert>

using namespace hfst;
using namespace hfst::implementations;

int main(void)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    HfstFsm net;
    net.add_transition(0, HfstArc(1, "c", "d", 1));
    net.add_transition(1, HfstArc(2, "a", "o", 2));
    net.add_transition(2, HfstArc(3, "t", "g", 3));
    net.set_final_weight(3, 4);
    std::cout << std::endl << "Conversions: ";
    std::cout << "skipped everything " <<
      "since they've disappeared into thin air" << std::endl;
    return 77;
  }
#endif // #ifndef DEBUG_MAIN

