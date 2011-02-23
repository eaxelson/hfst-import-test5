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
#include "HfstTransitionGraph.h"
#include "HfstTransducer.h"

#ifndef DEBUG_MAIN
namespace hfst { namespace implementations
{


  HfstBasicTransducer * ConversionFunctions::
  hfst_transducer_to_hfst_basic_transducer
  (const hfst::HfstTransducer &t) {

#if HAVE_SFST
    if (t.type == SFST_TYPE)
      return sfst_to_hfst_basic_transducer(t.implementation.sfst); 
#endif // HAVE_SFST

#if HAVE_OPENFST
    if (t.type == TROPICAL_OPENFST_TYPE)
      return tropical_ofst_to_hfst_basic_transducer
        (t.implementation.tropical_ofst); 
    if (t.type == LOG_OPENFST_TYPE)
      return log_ofst_to_hfst_basic_transducer(t.implementation.log_ofst); 
#endif // HAVE_OPENFST

#if HAVE_FOMA
    if (t.type == FOMA_TYPE)
      return foma_to_hfst_basic_transducer(t.implementation.foma); 
#endif // HAVE_FOMA

    /* Add here your implementation. */
    //#if HAVE_MY_TRANSDUCER_LIBRARY
    //    if (t.type == MY_TRANSDUCER_LIBRARY_TYPE)
    //      return my_transducer_library_transducer_to_hfst_basic_transducer
    //        (t.implementation.my_transducer_library); 
    //#endif // HAVE_MY_TRANSDUCER_LIBRARY

    if (t.type == HFST_OL_TYPE || t.type == HFST_OLW_TYPE)
      return hfst_ol_to_hfst_basic_transducer(t.implementation.hfst_ol);

    //throw hfst::exceptions::FunctionNotImplementedException();
    HFST_THROW(FunctionNotImplementedException);
  }


  /* -----------------------------------------------------------

      Conversion functions between HfstBasicTransducer and SFST transducer. 

     ----------------------------------------------------------- */


#if HAVE_SFST

  /* Recursively copy all transitions of \a node to \a net.
     Used by function sfst_to_hfst_basic_transducer(SFST::Transducer * t).

     @param node  The current node in the SFST transducer
     @param index  A map that maps nodes to integers
     @param visited_nodes  Which nodes have already been visited
     @param net  The HfstBasicTransducer that is being created
     @param alphabet  The alphabet of the SFST transducer
  */
  void ConversionFunctions::
  sfst_to_hfst_basic_transducer( SFST::Node *node, SFST::NodeNumbering &index, 
                    std::set<SFST::Node*> &visited_nodes, 
                    HfstBasicTransducer *net, SFST::Alphabet &alphabet ) {
  
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
                            HfstBasicTransition
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
        sfst_to_hfst_basic_transducer(arc->target_node(), index, 
                         visited_nodes, 
                         net, alphabet);
      }
    }
  }


  /* Create an HfstBasicTransducer equivalent to an SFST transducer \a t. */
  HfstBasicTransducer * ConversionFunctions::
  sfst_to_hfst_basic_transducer(SFST::Transducer * t) {
  
    HfstBasicTransducer * net = new HfstBasicTransducer();
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);
    // The set of nodes that have been visited
    std::set<SFST::Node*> visited_nodes;
   
    sfst_to_hfst_basic_transducer(t->root_node(), index, 
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


  /* Create an SFST::Transducer equivalent to HfstBasicTransducer \a net. */
  SFST::Transducer * ConversionFunctions::
  hfst_basic_transducer_to_sfst(const HfstBasicTransducer * net) {

  SFST::Transducer * t = new SFST::Transducer();
  t->alphabet.add_symbol("@_UNKNOWN_SYMBOL_@", 1);
  t->alphabet.add_symbol("@_IDENTITY_SYMBOL_@", 2);

  // Map that maps states of \a net to SFST nodes
  std::map<HfstState, SFST::Node*> state_map;
  state_map[0] = t->root_node();

  // Go through all states
  for (HfstBasicTransducer::const_iterator it = net->begin();
       it != net->end(); it++)
    {
      // Go through the set of transitions in each state
      for (HfstBasicTransducer::HfstTransitionSet::iterator tr_it 
             = it->second.begin();
           tr_it != it->second.end(); tr_it++)
        {
          // Create new nodes, if needed
          if (state_map.find(it->first) == state_map.end())
            state_map[it->first] = t->new_node();

          if (state_map.find(tr_it->get_target_state()) == state_map.end())
            state_map[tr_it->get_target_state()] = t->new_node();

          std::string istring(tr_it->get_input_symbol());
          std::string ostring(tr_it->get_output_symbol());

          if (istring.compare("@_EPSILON_SYMBOL_@") == 0) {
            istring = std::string("<>");
          }

          if (ostring.compare("@_EPSILON_SYMBOL_@") == 0) {
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
  for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
         = net->final_weight_map.begin();
       it != net->final_weight_map.end(); it++) 
    {
      if (state_map.find(it->first) == state_map.end())
        state_map[it->first] = t->new_node();
      state_map[it->first]->set_final(1);
    }

  // Make sure that also symbols that occur in the alphabet of the
  // HfstBasicTransducer but not in its transitions are inserted to 
  // the SFST transducer
  for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
         = net->alphabet.begin();
       it != net->alphabet.end(); it++) {
    if (it->compare("@_EPSILON_SYMBOL_@") != 0)
      t->alphabet.add_symbol(it->c_str());
  }
  
  return t;
}

#endif // HAVE_SFST




  /* -----------------------------------------------------------

      Conversion functions between HfstBasicTransducer and foma transducer. 

      ---------------------------------------------------------- */


#if HAVE_FOMA

  /* Create an HfstBasicTransducer equivalent to foma transducer \a t. */
  HfstBasicTransducer * ConversionFunctions::
  foma_to_hfst_basic_transducer(struct fsm * t) {

  HfstBasicTransducer * net = new HfstBasicTransducer();
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
	  HFST_THROW_MESSAGE
	    (HfstFatalException,
	     "Foma transducer has more than one start state");
        }
      }

    // 2. If there are transitions leaving from the state,
    if ((fsm+i)->target != -1) 
      {
        // copy the transition.
        net->add_transition
          ((fsm+i)->state_no,
           HfstBasicTransition
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
    HFST_THROW_MESSAGE
      (HfstFatalException,
       "Foma transducer has no start state");
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


  /* Create a foma transducer equivalent to HfstBasicTransducer \a hfst_fsm. */
  struct fsm * ConversionFunctions::
    hfst_basic_transducer_to_foma(const HfstBasicTransducer * hfst_fsm) {
    
    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    // Go through all states
    for (HfstBasicTransducer::const_iterator it = hfst_fsm->begin();
         it != hfst_fsm->end(); it++)
      {
        // Go through the set of transitions in each state
        for (HfstBasicTransducer::HfstTransitionSet::iterator tr_it 
               = it->second.begin();
             tr_it != it->second.end(); tr_it++)
          {
            // Copy the transition
            fsm_construct_add_arc(h, 
                                  (int)it->first, 
                                  (int)tr_it->get_target_state(),
                                  strdup(tr_it->get_input_symbol().c_str()),
                                  strdup(tr_it->get_output_symbol().c_str()) );
          }
      }
    
    // Go through the final states
    for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
           = hfst_fsm->final_weight_map.begin();
         it != hfst_fsm->final_weight_map.end(); it++) 
      {
        // Set the state as final
        fsm_construct_set_final(h, (int)it->first);
      }
    
    /* Make sure that also the symbols that occur only in the alphabet
       but not in transitions are copied. */
    for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
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

      Conversion functions between HfstBasicTransducer and OpenFst transducers 

      ------------------------------------------------------------- */


#if HAVE_OPENFST


  /* --- Conversion between tropical OpenFst and HfstBasicTransducer --- */


  /* Create an HfstBasicTransducer equivalent to an OpenFst tropical weight
     transducer \a t. */  
  HfstBasicTransducer * ConversionFunctions::
  tropical_ofst_to_hfst_basic_transducer
  (fst::StdVectorFst * t, bool has_hfst_header) {

    const fst::SymbolTable *inputsym = t->InputSymbols();
    const fst::SymbolTable *outputsym = t->OutputSymbols();

    /* An HFST tropical transducer always has an input symbol table. */
    if (has_hfst_header && inputsym == NULL) {
      HFST_THROW(MissingOpenFstInputSymbolTableException);
    }

  HfstBasicTransducer * net = new HfstBasicTransducer();

  // An empty transducer
  if (t->Start() == fst::kNoStateId)
    {      
      /* An empty OpenFst transducer does not necessarily have to have
         an input or output symbol table. */
      if (inputsym != NULL) {
        for ( fst::SymbolTableIterator it = 
                fst::SymbolTableIterator(*(inputsym));
              not it.Done(); it.Next() ) {
          if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Symbol() );
        }    
      }
      /* If the transducer is an OpenFst transducer, it might have an output
         symbol table. If the transducer is an HFST tropical transducer, it
         can have an output symbol table, but it is equivalent to the 
         input symbol table. */
      if (not has_hfst_header && outputsym != NULL) {
        for ( fst::SymbolTableIterator it = 
                fst::SymbolTableIterator(*(outputsym));
              not it.Done(); it.Next() ) {
          if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Symbol() );
        }    
      }
      return net;
    }      

  /* A non-empty OpenFst transducer must have at least an input symbol table.
     If the output symbol table is missing, we assume that it would be 
     equivalent to the input symbol table. */
  if (inputsym == NULL) {
    HFST_THROW(MissingOpenFstInputSymbolTableException);
  }
  if (outputsym == NULL)
    outputsym = inputsym;

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
            std::string istring = inputsym->Find(arc.ilabel);
            std::string ostring = outputsym->Find(arc.olabel);
            if (arc.ilabel == 0)
              istring = std::string("@_EPSILON_SYMBOL_@");
            if (arc.olabel == 0)
              ostring = std::string("@_EPSILON_SYMBOL_@");
            net->add_transition(origin, 
                                HfstBasicTransition
                                (target,
                                 istring,
                                 ostring,
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

              std::string istring = inputsym->Find(arc.ilabel);
              std::string ostring = outputsym->Find(arc.olabel);
              if (arc.ilabel == 0)
                istring = std::string("@_EPSILON_SYMBOL_@");
              if (arc.olabel == 0)
                ostring = std::string("@_EPSILON_SYMBOL_@");
              net->add_transition(origin, 
                                  HfstBasicTransition
                                  (target,
                                   istring,
                                   ostring,
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
            fst::SymbolTableIterator(*(inputsym));
          not it.Done(); it.Next() ) {
      if (it.Value() != 0) // epsilon is not inserted
        net->alphabet.insert( it.Symbol() );
    }    
    for ( fst::SymbolTableIterator it = 
            fst::SymbolTableIterator(*(outputsym));
          not it.Done(); it.Next() ) {
      if (it.Value() != 0) // epsilon is not inserted
        net->alphabet.insert( it.Symbol() );
    }    

    return net;
}



  /* Get a state id for a state in transducer \a t that corresponds
     to HfstState s as defined in \a state_map.     
     Used by function hfst_basic_transducer_to_tropical_ofst. */
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

  /* Create an OpenFst transducer equivalent to HfstBasicTransducer \a net. */
  fst::StdVectorFst * ConversionFunctions::
  hfst_basic_transducer_to_tropical_ofst
  (const HfstBasicTransducer * net) {
    
    fst::StdVectorFst * t = new fst::StdVectorFst();
    StateId start_state = t->AddState();
    t->SetStart(start_state);
    
    // The mapping between states in HfstBasicTransducer and StdVectorFst
    std::map<HfstState, StateId> state_map;
    state_map[0] = start_state;
    
    fst::SymbolTable st("");
    st.AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st.AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st.AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    
    // Go through all states
    for (HfstBasicTransducer::const_iterator it = net->begin();
         it != net->end(); it++)
      {
        // Go through the set of transitions in each state
        for (HfstBasicTransducer::HfstTransitionSet::iterator tr_it 
               = it->second.begin();
             tr_it != it->second.end(); tr_it++)
          {
            // Copy the transition
            t->AddArc( hfst_state_to_state_id(it->first, state_map, t), 
                       fst::StdArc
                       ( st.AddSymbol(tr_it->get_input_symbol()),
                         st.AddSymbol(tr_it->get_output_symbol()),
                         tr_it->get_weight(),
                         hfst_state_to_state_id
                          (tr_it->get_target_state(), state_map, t)) );
          }
      }
    
    // Go through the final states
    for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
           = net->final_weight_map.begin();
         it != net->final_weight_map.end(); it++) 
      {
        t->SetFinal(hfst_state_to_state_id(it->first, state_map, t), 
                    it->second);
      }
    
    // Add also symbols that do not occur in transitions
    for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
           = net->alphabet.begin();
         it != net->alphabet.end(); it++) {
        st.AddSymbol(*it);
      }
    
    t->SetInputSymbols(&st);
    return t;  
  }



  /* --- Conversion between log OpenFst and HfstBasicTransducer --- */
  
  /* Create an HfstBasicTransducer equivalent to an OpenFst log weight
     transducer \a t. */  
  HfstBasicTransducer * ConversionFunctions::
  log_ofst_to_hfst_basic_transducer
  (LogFst * t, bool has_hfst_header) {

    const fst::SymbolTable *inputsym = t->InputSymbols();
    const fst::SymbolTable *outputsym = t->OutputSymbols();

    /* An HFST log transducer always has an input symbol table. */
    if (has_hfst_header && inputsym == NULL) {
      HFST_THROW(MissingOpenFstInputSymbolTableException);
    }

  HfstBasicTransducer * net = new HfstBasicTransducer();

  // An empty transducer
  if (t->Start() == fst::kNoStateId)
    {      
      /* An empty OpenFst transducer does not necessarily have to have
         an input or output symbol table. */
      if (inputsym != NULL) {
        for ( fst::SymbolTableIterator it = 
                fst::SymbolTableIterator(*(inputsym));
              not it.Done(); it.Next() ) {
          if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Symbol() );
        }    
      }
      /* If the transducer is an OpenFst transducer, it might have an output
         symbol table. If the transducer is an HFST log transducer, it
         can have an output symbol table, but it is equivalent to the 
         input symbol table. */
      if (not has_hfst_header && outputsym != NULL) {
        for ( fst::SymbolTableIterator it = 
                fst::SymbolTableIterator(*(outputsym));
              not it.Done(); it.Next() ) {
          if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Symbol() );
        }    
      }
      return net;
    }      

  /* A non-empty OpenFst transducer must have at least an input symbol table.
     If the output symbol table is missing, we assume that it would be 
     equivalent to the input symbol table. */
  if (inputsym == NULL) {
    HFST_THROW(MissingOpenFstInputSymbolTableException);
  }
  if (outputsym == NULL)
    outputsym = inputsym;

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
  for (fst::StateIterator<LogFst> siter(*t); 
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
        for (fst::ArcIterator<LogFst> aiter(*t,s); 
             !aiter.Done(); aiter.Next())
          {
            const fst::LogArc &arc = aiter.Value();
            int target;  // how target state is printed, see the first comment
            if (arc.nextstate == 0)
              target = zero_print;
            else if (arc.nextstate == initial_state)
              target = 0;
            else
              target = (int)arc.nextstate;

            // Copy the transition
            std::string istring = inputsym->Find(arc.ilabel);
            std::string ostring = outputsym->Find(arc.olabel);
            if (arc.ilabel == 0)
              istring = std::string("@_EPSILON_SYMBOL_@");
            if (arc.olabel == 0)
              ostring = std::string("@_EPSILON_SYMBOL_@");
            net->add_transition(origin, 
                                HfstBasicTransition
                                (target,
                                 istring,
                                 ostring,
                                 arc.weight.Value()
                                 ));
          }
        if (t->Final(s) != fst::LogWeight::Zero()) {
          // Set the state as final
          net->set_final_weight(origin, t->Final(s).Value());
        }
        break;
      }
    }

    for (fst::StateIterator<LogFst> siter(*t); 
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
          for (fst::ArcIterator<LogFst> aiter(*t,s); 
               !aiter.Done(); aiter.Next())
            {
              const fst::LogArc &arc = aiter.Value();
              int target;  // how target state is printed, see the first comment
              if (arc.nextstate == 0)
                target = zero_print;
              else if (arc.nextstate == initial_state)
                target = 0;
              else
                target = (int)arc.nextstate;

              std::string istring = inputsym->Find(arc.ilabel);
              std::string ostring = outputsym->Find(arc.olabel);
              if (arc.ilabel == 0)
                istring = std::string("@_EPSILON_SYMBOL_@");
              if (arc.olabel == 0)
                ostring = std::string("@_EPSILON_SYMBOL_@");
              net->add_transition(origin, 
                                  HfstBasicTransition
                                  (target,
                                   istring,
                                   ostring,
                                   arc.weight.Value()
                                   ));
            }
          if (t->Final(s) != fst::LogWeight::Zero())
            net->set_final_weight(origin, t->Final(s).Value());
        }
      }

    /* Make sure that also the symbols that occur only in the alphabet
       but not in transitions are copied. */
    for ( fst::SymbolTableIterator it = 
            fst::SymbolTableIterator(*(inputsym));
          not it.Done(); it.Next() ) {
      if (it.Value() != 0) // epsilon is not inserted
        net->alphabet.insert( it.Symbol() );
    }    
    for ( fst::SymbolTableIterator it = 
            fst::SymbolTableIterator(*(outputsym));
          not it.Done(); it.Next() ) {
      if (it.Value() != 0) // epsilon is not inserted
        net->alphabet.insert( it.Symbol() );
    }    

    return net;
}



  /* Get a state id for a state in transducer \a t that corresponds
     to HfstState s as defined in \a state_map.     
     Used by function hfst_basic_transducer_to_log_ofst. */
  StateId ConversionFunctions::hfst_state_to_state_id
  (HfstState s, std::map<HfstState, StateId> &state_map, 
   LogFst * t)
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

  /* Create an OpenFst transducer equivalent to HfstBasicTransducer \a net. */
  LogFst * ConversionFunctions::
  hfst_basic_transducer_to_log_ofst
  (const HfstBasicTransducer * net) {
    
    LogFst * t = new LogFst();
    StateId start_state = t->AddState();
    t->SetStart(start_state);
    
    // The mapping between states in HfstBasicTransducer and StdVectorFst
    std::map<HfstState, StateId> state_map;
    state_map[0] = start_state;
    
    fst::SymbolTable st("");
    st.AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st.AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st.AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    
    // Go through all states
    for (HfstBasicTransducer::const_iterator it = net->begin();
         it != net->end(); it++)
      {
        // Go through the set of transitions in each state
        for (HfstBasicTransducer::HfstTransitionSet::iterator tr_it 
               = it->second.begin();
             tr_it != it->second.end(); tr_it++)
          {
            // Copy the transition
            t->AddArc( hfst_state_to_state_id(it->first, state_map, t), 
                       fst::LogArc
                       ( st.AddSymbol(tr_it->get_input_symbol()),
                         st.AddSymbol(tr_it->get_output_symbol()),
                         tr_it->get_weight(),
                         hfst_state_to_state_id
                          (tr_it->get_target_state(), state_map, t)) );
          }
      }
    
    // Go through the final states
    for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
           = net->final_weight_map.begin();
         it != net->final_weight_map.end(); it++) 
      {
        t->SetFinal(hfst_state_to_state_id(it->first, state_map, t), 
                    it->second);
      }
    
    // Add also symbols that do not occur in transitions
    for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
           = net->alphabet.begin();
         it != net->alphabet.end(); it++) {
        st.AddSymbol(*it);
      }
    
    t->SetInputSymbols(&st);
    return t;  
  }



#endif // HAVE_OPENFST



  /* -----------------------------------------------------------

      Conversion functions between HfstBasicTransducer and 
      optimized lookup transducers 

      ---------------------------------------------------------- */

/* An auxiliary function. */
unsigned int hfst_ol_to_hfst_basic_add_state
(hfst_ol::Transducer * t, 
 HfstBasicTransducer * basic,
 hfst_ol::HfstOlToBasicStateMap & state_map,
 bool weighted,
 hfst_ol::TransitionTableIndex index,
                                             unsigned int state_number)
{
  unsigned int new_state = state_number;
  state_map[index] = new_state;
  
  if(hfst_ol::indexes_transition_index_table(index))
  {
    const hfst_ol::TransitionIndex& transition_index = t->get_index(index);
    
    if(transition_index.final())
    {
        basic->add_state(new_state);
        basic->set_final_weight(new_state,
                                weighted ?
                                dynamic_cast<const hfst_ol::TransitionWIndex&>
                                (transition_index).final_weight() :
                                0.0);
    }
  }
  else // indexes transition table
  {
    const hfst_ol::Transition& transition = t->get_transition(index);
    
    if(transition.final())
    {
        basic->add_state(new_state);
        basic->set_final_weight(new_state,
                                weighted ?
                                dynamic_cast<const hfst_ol::TransitionW&>
                                (transition).get_weight() :
                                0.0);
    }
  }
  return new_state;
}


  /* Create an HfstBasicTransducer equivalent to hfst_ol::Transducer \a t . */
  HfstBasicTransducer * ConversionFunctions::
  hfst_ol_to_hfst_basic_transducer(hfst_ol::Transducer * t)
  {
      HfstBasicTransducer * basic = new HfstBasicTransducer();
      bool weighted = t->get_header().probe_flag(hfst_ol::Weighted);
      const hfst_ol::SymbolTable& symbols 
        = t->get_alphabet().get_symbol_table();
      
      
      /* This contains indices to either (1) the start of a set of entries 
         in the transition index table, or (2) the boundary before a set 
         of entries in the transition table; in this case, the following 
         entries will all have the same input symbol. In either case 
         the index represents a state and may be final The will already be 
         an entry in state_map for each value in agenda */
      std::vector<hfst_ol::TransitionTableIndex> agenda;
      hfst_ol::HfstOlToBasicStateMap state_map;
      unsigned int state_number=0;
      
      hfst_ol_to_hfst_basic_add_state
        (t, basic, state_map, weighted, 0, state_number);
      agenda.push_back(0);
      while(!agenda.empty())
      {
          hfst_ol::TransitionTableIndex current_index = agenda.back();
          agenda.pop_back();
          
          unsigned int current_state = state_map[current_index];
          
          hfst_ol::TransitionTableIndexSet transitions 
            = t->get_transitions_from_state(current_index);
          for(hfst_ol::TransitionTableIndexSet::const_iterator it
                =transitions.begin();it!=transitions.end();it++)
          {
              const hfst_ol::Transition& transition = t->get_transition(*it);
              
              if(state_map.find(transition.get_target()) == state_map.end())
              {
                  state_number++;
                  hfst_ol_to_hfst_basic_add_state
                    (t, basic, state_map, weighted, 
                     transition.get_target(), state_number);
                  agenda.push_back(transition.get_target());
              }
              basic->add_transition
                (current_state,
                 HfstBasicTransition
                 (state_map[transition.get_target()],
                  symbols[transition.get_input_symbol()],
                  symbols[transition.get_output_symbol()],
                  weighted ? dynamic_cast<const hfst_ol::TransitionW&>
                  (transition).get_weight() : 0 ));
          }
      }
      
      return basic;
      
  }

  /* Create an hfst_ol::Transducer equivalent to HfstBasicTransducer \a t.
     \a weighted defined whether the created transducer is weighted. */
  hfst_ol::Transducer * ConversionFunctions::
  hfst_basic_transducer_to_hfst_ol
  (const HfstBasicTransducer * t, bool weighted)
  {
      typedef std::set<std::string> StringSet;
      // The transition array is indexed starting from this constant
      const unsigned int TA_OFFSET = 2147483648u;
      const float packing_aggression = 0.85;
      const std::string epstr = "@_EPSILON_SYMBOL_@";

      // Symbols must be in the following order in an optimized-lookup
      // transducer:
      // 1) epsilon
      // 2) other input symbols
      // 3) symbols that aren't used as input symbols
      // Flag diacritics must be indexed as if they were symbol #0
      // (epsilon) and otherwise have a proper unique number,
      // but they can appear anywhere in the alphabet.
      
      // In this case we implement an optimisation where flag diacritics
      // appear at the end of the alphabet. This allows us to ignore
      // them for indexing purposes, which potentially makes the index
      // table smaller and faster to pack.
      
      StringSet * input_symbols = new StringSet();
      StringSet * flag_diacritics = new StringSet();
      StringSet * other_symbols = new StringSet();
    
      for (HfstBasicTransducer::const_iterator it = t->begin(); 
           it != t->end(); ++it) {
          for (HfstBasicTransducer::HfstTransitionSet::const_iterator tr_it 
                 = it->second.begin();
               tr_it != it->second.end(); ++tr_it) {
	      if (FdOperation::is_diacritic(tr_it->get_input_symbol())) {
		  flag_diacritics->insert(tr_it->get_input_symbol());
	      } else {
              input_symbols->insert(tr_it->get_input_symbol());
	      }
              other_symbols->insert(tr_it->get_output_symbol());
          }
      }
      
      hfst_ol::SymbolNumber seen_input_symbols = 1; // We always have epsilon
      hfst_ol::SymbolTable symbol_table;
      std::set<hfst_ol::SymbolNumber> flag_symbols;

      std::map<std::string, hfst_ol::SymbolNumber> * string_symbol_map =
	  new std::map<std::string, hfst_ol::SymbolNumber>();

      // 1) epsilon
      string_symbol_map->operator[](epstr) = symbol_table.size();
      symbol_table.push_back(epstr);

      // 2) input symbols
      for (std::set<std::string>::iterator it = input_symbols->begin();
           it != input_symbols->end(); ++it) {
          if (it->compare(epstr)) {
	      string_symbol_map->operator[](*it) = symbol_table.size();
	      symbol_table.push_back(*it);
	      ++seen_input_symbols;
          }
      }

      // 3) Flag diacritics
      for (std::set<std::string>::iterator it = flag_diacritics->begin();
	   it != flag_diacritics->end(); ++it) {
	  if (it->compare(epstr)) {
	      string_symbol_map->operator[](*it) = symbol_table.size();
	      flag_symbols.insert(symbol_table.size());
	      symbol_table.push_back(*it);
	      // don't increment seen_input_symbols - we use it for
	      // indexing
	  }
      }

      // 4) non-input symbols
      for (std::set<std::string>::iterator it = other_symbols->begin();
           it != other_symbols->end(); ++it) {
          if (it->compare(epstr) and input_symbols->count(*it) == 0 and
	      flag_diacritics->count(*it) == 0) {
	      string_symbol_map->operator[](*it) = symbol_table.size();
	      symbol_table.push_back(*it);
          }
      }

      delete input_symbols;
      delete flag_diacritics;
      delete other_symbols;

    std::map<unsigned int, hfst_ol::StatePlaceholder> state_placeholders;

    // first do one pass over the transitions, figuring out everything
    // about the states except starting indices

    for (HfstBasicTransducer::const_iterator it = t->begin(); 
         it != t->end(); ++it) {
        state_placeholders[it->first] = hfst_ol::StatePlaceholder(
            it->first, t->is_final_state(it->first));
        if (t->is_final_state(it->first)) {
                state_placeholders[it->first].final_weight =
                    t->get_final_weight(it->first);
        }
        for (HfstBasicTransducer::HfstTransitionSet::const_iterator tr_it 
               = it->second.begin();
             tr_it != it->second.end(); ++tr_it) {
            
            // check for previously unseen inputs
            if (state_placeholders[it->first].inputs.count(
                    string_symbol_map->operator[](
			tr_it->get_input_symbol())) == 0) {
                state_placeholders[it->first].inputs[
                    string_symbol_map->operator[](tr_it->get_input_symbol())] =
                    std::vector<hfst_ol::TransitionPlaceholder>();
            }
            hfst_ol::TransitionPlaceholder trans(
                tr_it->get_target_state(),
                string_symbol_map->operator[](tr_it->get_output_symbol()),
                tr_it->get_weight());
            state_placeholders[it->first]
                .inputs[string_symbol_map->operator[](
		    tr_it->get_input_symbol())].push_back(trans);
        }
    }

    delete string_symbol_map;

    hfst_ol::IndexPlaceholders * used_indices =
	new hfst_ol::IndexPlaceholders();

    // Now we assign starting indices (or alternatively determine a state
    // doesn't need an entry in the TIA ("is simple"). The starting state has
    // index 0. Used indices are stored in a map (at the beginning, every
    // index below or equal to the alphabet size is available except index 0).
    // For every state (in the TIA) thereafter, we check each available
    // starting index to see if it fits.

    // The starting state is special because it will have a TIA entry even if
    // it's simple, so we deal with it every time.

    unsigned int first_available_index = 0;
    std::set<unsigned int> * used_state_index = new std::set<unsigned int>();
    for (std::map<unsigned int, hfst_ol::StatePlaceholder>::iterator it =
             state_placeholders.begin();
         it != state_placeholders.end(); ++it) {
        if (it->second.is_simple() and it->first != 0) {
            continue;
        }
        unsigned int i = first_available_index;

        // While this index is not suitable for a starting index, keep looking
        while (!used_indices->fits(it->second, flag_symbols, i)) {
            ++i;
        }
        it->second.start_index = i;
        // Once we've found a starting index, mark all the used input symbols
	used_state_index->insert(i);
        for (std::map<hfst_ol::SymbolNumber,
                 std::vector<hfst_ol::TransitionPlaceholder> >
                 ::iterator sym_it = it->second.inputs.begin();
             sym_it != it->second.inputs.end(); ++sym_it) {
	    hfst_ol::SymbolNumber index_offset = sym_it->first;
	    if (flag_symbols.count(index_offset) != 0) {
		index_offset = 0;
	    }
            used_indices->operator[](i + index_offset) =
                std::pair<unsigned int, hfst_ol::SymbolNumber>
                (it->second.state_number, index_offset);
        }
        while (!used_indices->available_for_first(
		   first_available_index, used_state_index) or
	       !used_indices->available_for_something(
		   first_available_index,
		   seen_input_symbols,
		   packing_aggression)) {
            ++first_available_index;
        }
    }

    delete used_state_index;

    // Now we figure out where each state in the transition array begins.

    std::vector<unsigned int> first_transition_vector;
    first_transition_vector.push_back(0);
    for (std::map<unsigned int, hfst_ol::StatePlaceholder>::iterator it =
             state_placeholders.begin();
         it != state_placeholders.end(); ++it) {
        first_transition_vector.push_back(
            first_transition_vector[it->first] +
            it->second.number_of_transitions() + 1);
    }

    // Now for each index entry we write its input symbol and target


    hfst_ol::TransducerTable<hfst_ol::TransitionWIndex> windex_table;
    // First we note the finality of the starting state
    if (state_placeholders[0].final) {
        windex_table.append(hfst_ol::TransitionWIndex::create_final());
    } else {
        windex_table.append(hfst_ol::TransitionWIndex());
    }
    hfst_ol::TransducerTable<hfst_ol::TransitionW> wtransition_table;

    for(unsigned int i = 0; i <= (--(used_indices->end()))->first; ++i) {
        if (used_indices->count(i) == 0) { // blank entries
            windex_table.append(hfst_ol::TransitionWIndex());
        } else { // nonblank entries
	    unsigned int idx = used_indices->operator[](i).first;
	    hfst_ol::SymbolNumber sym = used_indices->operator[](i).second;
            windex_table.append(
		hfst_ol::TransitionWIndex(
		    sym,
		    first_transition_vector[idx] +
		    state_placeholders[idx].symbol_offset(sym) + TA_OFFSET));
        }
    }

    delete used_indices;
    
    for (unsigned int i = 0; i < symbol_table.size(); ++i) {
        windex_table.append(hfst_ol::TransitionWIndex()); // padding
    }

    //  For each state, write its entries in the transition array.

    hfst_ol::write_transitions_from_state_placeholders(
	wtransition_table,
	state_placeholders,
	first_transition_vector,
	flag_symbols);

    hfst_ol::TransducerAlphabet alphabet(symbol_table);
    hfst_ol::TransducerHeader header(seen_input_symbols,
                                     symbol_table.size(),
                                     windex_table.size(),
                                     wtransition_table.size(),
                                     weighted);

    return new hfst_ol::Transducer(header,
                                   alphabet,
                                   windex_table,
                                   wtransition_table);

  }


  /* Add here your conversion functions. */
  //#if HAVE_MY_TRANSDUCER_LIBRARY
  //
  //HfstBasicTransducer * ConversionFunctions::
  //my_transducer_library_transducer_to_hfst_basic_transducer
  //  (my_namespace::MyFst * t) {
  //(void)t;
  //throw hfst::exceptions::FunctionNotImplementedException();
  //}

  //my_namespace::MyFst * ConversionFunctions::
  //hfst_basic_transducer_to_my_transducer_library_transducer
  //  (const HfstBasicTransducer * t) {
  //(void)t;
  //throw hfst::exceptions::FunctionNotImplementedException();
  //}
  //#endif // HAVE_MY_TRANSDUCER_LIBRARY


} }

#else  // #ifndef DEBUG_MAIN
#include <cstdlib>
#include <cassert>

using namespace hfst;
using namespace hfst::implementations;

int main(void)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    HfstBasicTransducer net;
    net.add_transition(0, HfstBasicTransition(1, "c", "d", 1));
    net.add_transition(1, HfstBasicTransition(2, "a", "o", 2));
    net.add_transition(2, HfstBasicTransition(3, "t", "g", 3));
    net.set_final_weight(3, 4);
    std::cout << std::endl << "Conversions: ";
    std::cout << "skipped everything " <<
      "since they've disappeared into thin air" << std::endl;
    return 77;
  }
#endif // #ifndef DEBUG_MAIN

