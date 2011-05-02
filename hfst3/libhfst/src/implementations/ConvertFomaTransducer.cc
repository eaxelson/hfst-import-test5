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
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
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

  // *** THE NEW FUNCTIONS ***

  HfstConstantTransducer * ConversionFunctions::foma_to_hfst_constant_transducer
  (struct fsm * t)
  {
    fsm_count(t);
    HfstConstantTransducer * net 
      = new HfstConstantTransducer(t->statecount);
    struct fsm_state *fsm;
    fsm = t->states;
    int start_state_id = -1;
    bool start_state_found=false;

    // Copy the alphabet
    struct sigma * p = t->sigma;
    while (p != NULL) {
      if (p->symbol == NULL)
	break;
      net->symbol_map[p->number] = std::string(p->symbol);
      p = p->next;
    }

    
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
	     (fsm+i)->target,
	     (fsm+i)->in, 
	     (fsm+i)->out,
	     0);
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
    if (start_state_id != 0) {
      //net->swap_state_numbers(start_state_id,0);
      HFST_THROW_MESSAGE
	(HfstFatalException,
	 "Start state of a foma transducer is not numbered as zero");
    }
        
    return net;
  }

  struct fsm * ConversionFunctions::hfst_constant_transducer_to_foma
  (const HfstConstantTransducer * t)
  {
    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    std::vector<std::string> symbols;

    // Copy the alphabet
    for (HfstConstantTransducer::SymbolMap::const_iterator it 
           = t->symbol_map.begin();
         it != t->symbol_map.end(); it++)
      {
	symbols.push_back(it->second);
        char *symbol = strdup(it->second.c_str());
        if ( fsm_construct_check_symbol(h,symbol) == -1 ) {
          fsm_construct_add_symbol(h,symbol);
        }
      }

    // Go through all states
    for (unsigned int i=0; i < (unsigned int)t->states.size(); i++)
      {
        // Go through the set of transitions in each state
        for (HfstConstantTransducer::TransitionVector::const_iterator tr_it 
               = t->states[i].begin();
             tr_it != t->states[i].end(); tr_it++)
          {
            // Copy the transition
            fsm_construct_add_arc(h, 
                                  (int)i, 
                                  (int)tr_it->target,
                                  strdup(symbols[tr_it->input].c_str()),
                                  strdup(symbols[tr_it->output].c_str()) );
          }
      }
    
    // Go through the final states
    for (HfstConstantTransducer::FinalStateMap::const_iterator it 
           = t->final_states.begin();
         it != t->final_states.end(); it++) 
      {
        // Set the state as final
        fsm_construct_set_final(h, (int)it->first);
      }
    
    fsm_construct_set_initial(h, 0);
    net = fsm_construct_done(h);
    fsm_count(net);
    net = fsm_topsort(net);
    
    return net;      
  }

#endif // HAVE_FOMA



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

