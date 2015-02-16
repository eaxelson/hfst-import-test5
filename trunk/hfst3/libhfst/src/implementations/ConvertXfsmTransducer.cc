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

      Conversion functions between HfstBasicTransducer and xfsm transducer. 

      ---------------------------------------------------------- */


#if HAVE_XFSM

  // Convert between HfstBasicTransducer and xfsm transducer one-side symbols.
  // The identity symbol must be handled separately.
  static id_type hfst_symbol_to_xfsm_symbol(const std::string & symbol)
  {
    if (symbol == hfst::internal_epsilon)
      return EPSILON;
    else if (symbol == hfst::internal_unknown)
      return OTHER;
    else if (symbol == hfst::internal_identity)
      throw "hfst_symbol_to_xfsm_symbol does not accept the identity symbol as its argument";
    else
      return single_to_id(symbol.c_str());
  }

  // Convert between xfsm transducer and HfstBasicTransducer one-side symbols.
  // The identity symbol must be handled separately.
  static std::string xfsm_symbol_to_hfst_symbol(id_type id)
  {
    if (id == EPSILON)
      return hfst::internal_epsilon;
    else if (id == OTHER)
      return hfst::internal_unknown;
    else {
      std::string retval("");
      LABELptr lptr = id_to_label(id);
      FAT_STR fs = lptr->content.name;
      while (*fs != '\0')
        {
          retval.append(1, *fs);
          ++fs;
        }
      return retval;
    }
  }

  // Convert between an xfsm label (symbol pair) and hfst transition symbols.
  void label_id_to_symbol_pair(id_type label_id, std::string & isymbol, std::string & osymbol)
  {
    // (1) atomic OTHER label -> identity pair
    if (label_id == OTHER)
      {
        isymbol = hfst::internal_identity;
        osymbol = hfst::internal_identity;
      }
    else
      {
        // (2) non-atomic OTHER label -> unknown pair
        // (3) all other cases
        id_type upperid = upper_id(label_id);
        id_type lowerid = lower_id(label_id);
        isymbol = xfsm_symbol_to_hfst_symbol(upperid);
        osymbol = xfsm_symbol_to_hfst_symbol(lowerid);
      }
  }

  id_type symbol_pair_to_label_id(const std::string & isymbol, const std::string & osymbol)
  {
    if (isymbol == hfst::internal_identity)
      {
        if (osymbol != hfst::internal_identity)
          throw "identity symbol cannot be on one side only";
        // atomic OTHER label
        return OTHER;
      }
    else
      {
        id_type input_id = hfst_symbol_to_xfsm_symbol(isymbol);
        id_type output_id = hfst_symbol_to_xfsm_symbol(osymbol);
        return id_pair_to_id(input_id, output_id);
      }
  }


  // Insert all symbols in an xfsm transducer alphabet (sigma) into
  // the alphabet of an HfstBasicTransducer.
  void copy_xfsm_alphabet_into_hfst_alphabet(NETptr t, HfstBasicTransducer * fsm)
    {
      ALPHABETptr alpha_ptr = net_sigma(t);
      ALPH_ITptr alpha_it_ptr = start_alph_iterator(NULL, alpha_ptr);
      id_type label_id = next_alph_id(alpha_it_ptr);
      
      while(label_id != ID_NO_SYMBOL)
        {
          std::string symbol = xfsm_symbol_to_hfst_symbol(label_id);
          fsm->add_symbol_to_alphabet(symbol);
          label_id = next_alph_id(alpha_it_ptr);
        }
    }

  
  /* ----------------------------------------------------------------------

     Create an HfstBasicTransducer equivalent to foma transducer \a t. 
     
     ---------------------------------------------------------------------- */

  HfstBasicTransducer * ConversionFunctions::
  xfsm_to_hfst_basic_transducer(NETptr t) 
  {
    HfstBasicTransducer * result = new HfstBasicTransducer();

    // Map states of t into states in result
    std::map<STATEptr, HfstState> xfsm_to_hfst_state;

    STATEptr state_ptr = t->body.states;
    STATEptr start_ptr = t->start.state;

    // Create states in result
    while (state_ptr != NULL)
      {
        // initial state exists already
        if (state_ptr != start_ptr) 
          {
            (void)result->add_state();
          }
        state_ptr = state_ptr->next;
      }
    state_ptr = t->body.states;

    // For each state in t, map the state into a state in result and make it final,
    // if needed. States of t are stored in a stack, so we start numbering states of
    // result from the biggest state number.
    HfstState result_state = result->get_max_state();
    while (state_ptr != NULL)
      {
        if (state_ptr == start_ptr) 
          {
            // initial state exists already in result
            xfsm_to_hfst_state.insert(std::pair<STATEptr, HfstState>(state_ptr, 0));
            if (state_ptr->final != 0) 
              {
                result->set_final_weight(0, 0);
              }
          }            
        else
          {
            xfsm_to_hfst_state.insert(std::pair<STATEptr, HfstState>(state_ptr, result_state));
            if (state_ptr->final != 0) 
              {
                result->set_final_weight(result_state, 0);
              }
            --result_state;
          }          
        state_ptr = state_ptr->next;
      }

    state_ptr = t->body.states;

    // For each state in t, go through its transitions and copy them into result.
    while (state_ptr != NULL)
      {
        ARCptr arc_ptr = state_ptr->arc.set;
        while (arc_ptr != NULL)
          {
            id_type label_id = arc_ptr->label;
            std::string isymbol, osymbol;
            label_id_to_symbol_pair(label_id, isymbol, osymbol);
            
            STATEptr target_state_ptr = arc_ptr->destination;

            HfstBasicTransition tr(xfsm_to_hfst_state[target_state_ptr], isymbol, osymbol, 0);
            result->add_transition(xfsm_to_hfst_state[state_ptr], tr);

            arc_ptr = arc_ptr->next;
          }
        state_ptr = state_ptr->next;
      }

    // Copy alphabet of t into result.
    copy_xfsm_alphabet_into_hfst_alphabet(t, result);

    return result;
  }

  /* ------------------------------------------------------------------------
     
     Create an xfsm transducer equivalent to HfstBasicTransducer \a hfst_fsm. 

     ------------------------------------------------------------------------ */

  NETptr ConversionFunctions::
    hfst_basic_transducer_to_xfsm(const HfstBasicTransducer * hfst_fsm) 
  {
    NETptr result = null_net();

    // Maps HfstBasicTransducer states (i.e. vector indices) into xfsm transducer states.
    std::vector<STATEptr> state_vector;

    // ---- Copy states -----
    unsigned int fsm_state = 0;
    for (HfstBasicTransducer::const_iterator it = hfst_fsm->begin();
         it != hfst_fsm->end(); it++)
      {
        // 'null_net()' creates the initial state
        if (fsm_state != 0)
          {
            // TODO: slow to call is_final_state each time a new state is added
            STATEptr xfsm_state = add_state_to_net(result, hfst_fsm->is_final_state(fsm_state) ? 1 : 0);
            state_vector.push_back(xfsm_state);
          }
        else
          {
            // Finality of the initial state is checked later.
            state_vector.push_back(result->start.state);
          }
        fsm_state++;
      }

    // ----- Go through all states -----
    unsigned int source_state=0;
    for (HfstBasicTransducer::const_iterator it = hfst_fsm->begin();
         it != hfst_fsm->end(); it++)
      {
        STATEptr xfsm_source_state = state_vector.at(source_state);
        // ----- Go through the set of transitions in each state -----
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it
               = it->begin();
             tr_it != it->end(); tr_it++)
          {
            // Copy the transition
            std::string isymbol = tr_it->get_input_symbol();
            std::string osymbol = tr_it->get_output_symbol();
            HfstState target_state =  tr_it->get_target_state();

            id_type ti = symbol_pair_to_label_id(isymbol, osymbol);

            if (isymbol == hfst::internal_identity)
              {
                if (osymbol != hfst::internal_identity)
                  throw "identity symbol cannot be on one side only";
                // atomic OTHER label
                ti = OTHER;
              }
            else
              {
                id_type input_id = hfst_symbol_to_xfsm_symbol(isymbol);
                id_type output_id = hfst_symbol_to_xfsm_symbol(osymbol);
                ti = id_pair_to_id(input_id, output_id);
              }

            STATEptr xfsm_target_state = state_vector.at(target_state);

            if( add_arc_to_state(result, xfsm_source_state, ti, xfsm_target_state, NULL, 0) == NULL )
              throw "add_arc_to_state failed";

          }
        // ----- transitions gone through -----
        source_state++;
      }
    // ----- all states gone through -----
    
    // If the initial state is final, make the result optional.
    if (hfst_fsm->is_final_state(0))
      {
        result = optional_net(result, 0);
      }

    // Copy alphabet
    ALPHABETptr ap = net_sigma(result);
    const HfstBasicTransducer::HfstTransitionGraphAlphabet & alpha
      = hfst_fsm->get_alphabet();
    for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it
           = alpha.begin();
         it != alpha.end(); it++)
      {
        if (hfst::is_epsilon(*it) || hfst::is_unknown(*it) || hfst::is_identity(*it))
          continue;
        (void) alph_add_to(ap, hfst_symbol_to_xfsm_symbol(it->c_str()), DONT_KEEP);
      }    

    // TESTING...
    //NETptr unk2unk = XfsmTransducer::create_xfsm_unknown_to_unknown_transducer();
    //NETptr id2id = XfsmTransducer::create_xfsm_identity_to_identity_transducer();
    //NETptr comp = compose_net(result, unk2unk, DONT_KEEP, DONT_KEEP);

    return result;
  }

#endif // HAVE_XFSM

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

