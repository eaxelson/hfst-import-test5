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
#include "HfstTransitionGraph.h"
#include "HfstTransducer.h"

#ifndef MAIN_TEST
namespace hfst { namespace implementations
{

#if HAVE_OPENFST


  /* -------------------------------------------------------
     
       Conversions between HfstBasicTransducer and OpenFst's 
       tropical transducer

     ------------------------------------------------------- */


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
	  assert(it.Symbol() != "");

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
	  assert(it.Symbol() != "");
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
  StateId initial_state = t->Start();

  /* Go through all states */
  for (fst::StateIterator<fst::StdVectorFst> siter(*t); 
       not siter.Done(); siter.Next()) 
    {
      StateId s = siter.Value();

      HfstState origin = s;
      if (origin == initial_state)
	origin = 0;
      else if (origin == 0)
	origin = initial_state;

      unsigned int number_of_arcs = fst::NumArcs(*t, s);
      net->initialize_transition_vector(s, number_of_arcs);

      /* Go through all transitions in a state */
      for (fst::ArcIterator<fst::StdVectorFst> aiter(*t,s); 
	   !aiter.Done(); aiter.Next())
	{
	  const fst::StdArc &arc = aiter.Value();

	  HfstState target = arc.nextstate;
	  if (target == initial_state)
	    target = 0;
	  else if (target == 0)
	    target = initial_state;

	  // Copy the transition
	  std::string istring = inputsym->Find(arc.ilabel);
	  std::string ostring = outputsym->Find(arc.olabel);

	  if(istring == "") {
	    std::cerr << "ERROR: arc.ilabel " << arc.ilabel << " not found" << std::endl;
	    assert(false);
	  }
	  if(ostring == "") {
	    std::cerr << "ERROR: arc.olabel " << arc.olabel << " not found" << std::endl;
	    assert(false);
	  }

	  if (arc.ilabel == 0) {
	    istring = std::string(internal_epsilon);
	  }
	  if (arc.olabel == 0) {
	    ostring = std::string(internal_epsilon);
	  }

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

    }

    /* Make sure that also the symbols that occur only in the alphabet
       but not in transitions are copied. */
    for ( fst::SymbolTableIterator it = 
            fst::SymbolTableIterator(*(inputsym));
          not it.Done(); it.Next() ) 
      {
	assert(it.Symbol() != "");
	if (it.Value() != 0) // epsilon is not inserted
	  net->alphabet.insert( it.Symbol() );
      }    
    for ( fst::SymbolTableIterator it = 
            fst::SymbolTableIterator(*(outputsym));
          not it.Done(); it.Next() ) 
      {
	assert(it.Symbol() != "");
	if (it.Value() != 0) // epsilon is not inserted
	  net->alphabet.insert( it.Symbol() );
      }    
    
    return net;
}



  //  NOT NEEDED
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
    StateId start_state = t->AddState(); // always zero
    t->SetStart(start_state);
    
    std::vector<StateId> state_vector;
    state_vector.push_back(start_state);
    for (unsigned int i = 1; i <= (net->get_max_state()); i++) {
      state_vector.push_back(t->AddState());
    }

    fst::SymbolTable st("");
    st.AddSymbol(internal_epsilon, 0);
    st.AddSymbol(internal_unknown, 1);
    st.AddSymbol(internal_identity, 2);
    
    // Add also symbols that do not occur in transitions
    for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
           = net->alphabet.begin();
         it != net->alphabet.end(); it++) {
      assert(not it->empty());
      st.AddSymbol(*it, net->get_symbol_number(*it));
    }

    // Go through all states
    unsigned int source_state=0;
    for (HfstBasicTransducer::const_iterator it = net->begin();
         it != net->end(); it++)
      {
        // Go through the set of transitions in each state
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
               = it->begin();
             tr_it != it->end(); tr_it++)
          {
            // Copy the transition

	    assert(not tr_it->get_input_symbol().empty());
	    assert(not tr_it->get_output_symbol().empty());

	    unsigned int in = st.Find(tr_it->get_input_symbol());
	    unsigned int out = st.Find(tr_it->get_output_symbol());

	    if (in == -1)
	      {
		// FIXME: a temporary solution
		st.AddSymbol(tr_it->get_input_symbol(), 
			     net->get_symbol_number
			       (tr_it->get_input_symbol()));
		in = st.Find(tr_it->get_input_symbol());
		/* std::cerr << "ERROR: no number found for input symbol "
			  << tr_it->get_input_symbol() << std::endl;
			  assert(false); */
	      }
	    if (out == -1)
	      {
		// FIXME: a temporary solution
		st.AddSymbol(tr_it->get_output_symbol(), 
			     net->get_symbol_number
			       (tr_it->get_output_symbol()));
		out = st.Find(tr_it->get_output_symbol());
		/* std::cerr << "ERROR: no number found for output symbol "
			  << tr_it->get_output_symbol() << std::endl;
			  assert(false); */
	      }

	    // DEBUG
	    /*std::cerr << "adding arc "
		      << state_vector[source_state] << "\t"
		      << state_vector[tr_it->get_target_state()] << "\t"
		      << in << "\t"
		      << out << std::endl;*/

            t->AddArc(
		      state_vector[source_state],
                       fst::StdArc
		      ( in,
			out,
			tr_it->get_weight(),
			state_vector[tr_it->get_target_state()]));
          }
	source_state++;
      }
    
    // Go through the final states
    for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
           = net->final_weight_map.begin();
         it != net->final_weight_map.end(); it++) 
      {
        t->SetFinal(
		    state_vector[it->first],
                    it->second);
      }
    
    t->SetInputSymbols(&st);
    return t;  
  }







  /* -------------------------------------------------------
     
       Conversions between HfstFastTransducer and OpenFst's 
       tropical transducer

     ------------------------------------------------------- */


  /* Create an HfstFastTransducer equivalent to an OpenFst tropical weight
     transducer \a t. */  
  HfstFastTransducer * ConversionFunctions::
  tropical_ofst_to_hfst_fast_transducer
  (fst::StdVectorFst * t, bool has_hfst_header) {

    const fst::SymbolTable *inputsym = t->InputSymbols();
    const fst::SymbolTable *outputsym = t->OutputSymbols();

    /* An HFST tropical transducer always has an input symbol table. */
    if (has_hfst_header && inputsym == NULL) {
      HFST_THROW(MissingOpenFstInputSymbolTableException);
    }

  HfstFastTransducer * net = new HfstFastTransducer();

  // An empty transducer
  if (t->Start() == fst::kNoStateId)
    {      
      /* An empty OpenFst transducer does not necessarily have to have
         an input or output symbol table. */
      if (inputsym != NULL) {
        for ( fst::SymbolTableIterator it = 
                fst::SymbolTableIterator(*(inputsym));
              not it.Done(); it.Next() ) {
	  assert(it.Symbol() != "");

          //if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Value() );
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
	  assert(it.Symbol() != "");
          //if (it.Value() != 0) // epsilon is not inserted
            net->alphabet.insert( it.Value() );
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


  // Handle the alphabet

  StringVector input_coding_vector;
  StringVector output_coding_vector;

  for ( fst::SymbolTableIterator it = 
	  fst::SymbolTableIterator(*(inputsym));
	not it.Done(); it.Next() ) {
    assert(it.Symbol() != "");
    //if (it.Value() != 0) // epsilon is not inserted
    // it is possible that there are gaps in numbering
    while (input_coding_vector.size() < it.Value()) {
      // empty space if no symbol at this index
      input_coding_vector.push_back(std::string(""));
    }
    net->alphabet.insert( it.Value() );
    input_coding_vector.push_back( it.Symbol() );
  }    
  for ( fst::SymbolTableIterator it = 
	  fst::SymbolTableIterator(*(outputsym));
	not it.Done(); it.Next() ) {
    assert(it.Symbol() != "");
    //if (it.Value() != 0) // epsilon is not inserted
    // it is possible that there are gaps in numbering
    while (output_coding_vector.size() < it.Value()) {
      // empty space if no symbol at this index
      output_coding_vector.push_back(std::string(""));
    }
    net->alphabet.insert( it.Value() );
    output_coding_vector.push_back( it.Symbol() );
  }    

    NumberVector input_harmonization_vector 
      = get_harmonization_vector(input_coding_vector);
    NumberVector output_harmonization_vector 
      = get_harmonization_vector(output_coding_vector);

  /* This takes care that initial state is always number zero
     and state number zero (if it is not initial) is some other number
     (basically as the number of the initial state in that case, i.e.
     the numbers of initial state and state number zero are swapped) */
  StateId initial_state = t->Start();

  /* Go through all states */
  for (fst::StateIterator<fst::StdVectorFst> siter(*t); 
       not siter.Done(); siter.Next()) 
    {
      StateId s = siter.Value();

      HfstState origin = s;
      if (origin == initial_state)
	origin = 0;
      else if (origin == 0)
	origin = initial_state;

      unsigned int number_of_arcs = fst::NumArcs(*t, s);
      net->initialize_transition_vector(s, number_of_arcs);

      /* Go through all transitions in a state */
      for (fst::ArcIterator<fst::StdVectorFst> aiter(*t,s); 
	   !aiter.Done(); aiter.Next())
	{
	  const fst::StdArc &arc = aiter.Value();

	  HfstState target = arc.nextstate;
	  if (target == initial_state)
	    target = 0;
	  else if (target == 0)
	    target = initial_state;

	  net->add_transition(origin, 
			      HfstFastTransition
			      (target,
			       input_harmonization_vector.at(arc.ilabel),
			       output_harmonization_vector.at(arc.olabel),
			       arc.weight.Value()
			       ));
	} 

      if (t->Final(s) != fst::TropicalWeight::Zero()) {
	// Set the state as final
	net->set_final_weight(origin, t->Final(s).Value());
      }

    }

    return net;
}



  /* Create an OpenFst transducer equivalent to HfstBasicTransducer \a net. */
  fst::StdVectorFst * ConversionFunctions::
  hfst_fast_transducer_to_tropical_ofst
  (const HfstFastTransducer * net) {
    
    fst::StdVectorFst * t = new fst::StdVectorFst();
    StateId start_state = t->AddState(); // always zero
    t->SetStart(start_state);
    
    std::vector<StateId> state_vector;
    state_vector.push_back(start_state);
    for (unsigned int i = 1; i <= (net->get_max_state()); i++) {
      state_vector.push_back(t->AddState());
    }

    fst::SymbolTable st("");
    st.AddSymbol(internal_epsilon, 0);
    st.AddSymbol(internal_unknown, 1);
    st.AddSymbol(internal_identity, 2);
    
    // Handle the alphabet
    for (HfstFastTransducer::HfstTransitionGraphAlphabet::iterator it 
           = net->alphabet.begin();
         it != net->alphabet.end(); it++) {
      //assert(not it->empty());
      st.AddSymbol(get_string(*it), *it);
    }


    // Go through all states
    unsigned int source_state=0;
    for (HfstFastTransducer::const_iterator it = net->begin();
         it != net->end(); it++)
      {
        // Go through the set of transitions in each state
        for (HfstFastTransducer::HfstTransitions::const_iterator tr_it 
               = it->begin();
             tr_it != it->end(); tr_it++)
          {
            // Copy the transition
            t->AddArc(
		      state_vector[source_state],
                       fst::StdArc
		      ( tr_it->get_input_symbol(),
			tr_it->get_output_symbol(),
			tr_it->get_weight(),
			state_vector[tr_it->get_target_state()]));
          }
	source_state++;
      }
    
    // Go through the final states
    for (HfstFastTransducer::FinalWeightMap::const_iterator it 
           = net->final_weight_map.begin();
         it != net->final_weight_map.end(); it++) 
      {
        t->SetFinal(
		    state_vector[it->first],
                    it->second);
      }
        
    t->SetInputSymbols(&st);
    return t;  
  }












  /* ------------------------------------------------------------
     
       Conversions between HfstConstantTransducer and OpenFst's 
       tropical transducer

     ------------------------------------------------------------ */


  /* Create an HfstConstantTransducer equivalent to an OpenFst tropical weight
     transducer \a t. 

     @pre has_hfst_header = true */  
  HfstConstantTransducer * ConversionFunctions::
  tropical_ofst_to_hfst_constant_transducer
  (fst::StdVectorFst * t, bool has_hfst_header) {

    const fst::SymbolTable *inputsym = t->InputSymbols();
    const fst::SymbolTable *outputsym = t->OutputSymbols();

    /* An HFST tropical transducer always has an input symbol table. */
    if (has_hfst_header && inputsym == NULL) {
      HFST_THROW(MissingOpenFstInputSymbolTableException);
    }

    HfstConstantTransducer * net 
      = new HfstConstantTransducer(t->NumStates());

    // An empty transducer
    if (t->Start() == fst::kNoStateId) {      
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
    
    /* Copy the alphabet */
    for ( fst::SymbolTableIterator it = 
	    fst::SymbolTableIterator(*(inputsym));
	  not it.Done(); it.Next() ) {
      //if (it.Value() != 0) // epsilon is not inserted
	net->symbol_map[it.Value()] = it.Symbol();
    }    
    for ( fst::SymbolTableIterator it = 
	    fst::SymbolTableIterator(*(outputsym));
	  not it.Done(); it.Next() ) {
      //if (it.Value() != 0) // epsilon is not inserted
	net->symbol_map[it.Value()] = it.Symbol();
    }    
    
    
    /* This takes care that initial state is always number zero
       and state number zero (if it is not initial) is some other number
       (basically as the number of the initial state in that case, i.e.
       the numbers of initial state and state number zero are swapped) */
    StateId initial_state = t->Start();
    
    /* Go through all states */
    for (fst::StateIterator<fst::StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next()) 
      {
	StateId s = siter.Value();
	
	HfstState origin = s;
	if (origin == initial_state)
	  origin = 0;
	else if (origin == 0)
	  origin = initial_state;
	
	unsigned int number_of_arcs = fst::NumArcs(*t, s);
	net->initialize_transition_vector(s, number_of_arcs);

	/* Go through all transitions in a state */
	for (fst::ArcIterator<fst::StdVectorFst> aiter(*t,s); 
	     !aiter.Done(); aiter.Next())
	  {
	    const fst::StdArc &arc = aiter.Value();
	    
	    HfstState target = arc.nextstate;
	    if (target == initial_state)
	      target = 0;
	    else if (target == 0)
	      target = initial_state;
	    
	    // Copy the transition
	    net->add_transition(origin,
				target,
				arc.ilabel,
				arc.olabel,
				arc.weight.Value()
				);
	  } 
	
	if (t->Final(s) != fst::TropicalWeight::Zero()) {
	  // Set the state as final
	  net->set_final_weight(origin, t->Final(s).Value());
	}
	
      }
    
    return net;
  }


  /* Create an OpenFst transducer equivalent to 
     HfstConstantTransducer \a net. */
  fst::StdVectorFst * ConversionFunctions::
  hfst_constant_transducer_to_tropical_ofst
  (const HfstConstantTransducer * net) {
    
    fst::StdVectorFst * t = new fst::StdVectorFst();

    // (0) Copy the alphabet
    fst::SymbolTable st("");
    st.AddSymbol(internal_epsilon, 0);  // these should
    st.AddSymbol(internal_unknown, 1);  // already be in
    st.AddSymbol(internal_identity, 2); // the alphabet..

    for (HfstConstantTransducer::SymbolMap::const_iterator it 
           = net->symbol_map.begin();
         it != net->symbol_map.end(); it++) {
      st.AddSymbol(it->second, it->first);
    }
    t->SetInputSymbols(&st);

    // (1) Go through all states
    for (unsigned int i=0; i < net->states.size(); i++)
      {
	// Create the state
	t->AddState();

        // Go through the set of transitions in each state
        for (HfstConstantTransducer::TransitionVector::const_iterator tr_it 
               = net->states[i].begin();
             tr_it != net->states[i].end(); tr_it++)
          {
            // Copy the transition
            t->AddArc( i,
                       fst::StdArc
		       (
			tr_it->input,
			tr_it->output,
			tr_it->weight,
			tr_it->target
			 ));
          }
      }
    
    // (2) Go through the final states
    for (HfstConstantTransducer::FinalStateMap::const_iterator it 
           = net->final_states.begin();
         it != net->final_states.end(); it++) 
      {
        t->SetFinal(it->first,
                    it->second);
      }
    
    // Set state number 0 as start state
    t->SetStart(0);

    return t;  
  }


#endif // HAVE_OPENFST

  }}
#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST
