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

  StringVector symbol_vector = TropicalWeightTransducer::get_symbol_vector(t);
  std::vector<unsigned int> harmonization_vector 
    = HfstTropicalTransducerTransitionData::get_harmonization_vector(symbol_vector);
  
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

      unsigned int number_of_arcs = t->NumArcs(s);
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
                  HfstBasicTransition
                  (target,
                   harmonization_vector[arc.ilabel],
		   harmonization_vector[arc.olabel],
                   arc.weight.Value(),
		   false), // dummy parameter needed because numbers are used 
			  false); // do not insert symbols to alphabet
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

    assert(net != NULL);
    return net;
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

	    //assert(not tr_it->get_input_symbol().empty());
	    //assert(not tr_it->get_output_symbol().empty());

	    unsigned int in = tr_it->get_input_number(); // st.Find(tr_it->get_input_symbol());
	    unsigned int out = tr_it->get_output_number(); // st.Find(tr_it->get_output_symbol());

        if (in == -1)
          {
	    std::cerr << "ERROR: no number found for input symbol "
		      << tr_it->get_input_symbol() << std::endl;
	    assert(false);
          }
        if (out == -1)
          {
	    std::cerr << "ERROR: no number found for output symbol "
		      << tr_it->get_output_symbol() << std::endl;
	    assert(false);
          }

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
