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
  
  /* ----------------------------------------------------------------------

     Create an HfstBasicTransducer equivalent to foma transducer \a t. 
     
     ---------------------------------------------------------------------- */

  HfstBasicTransducer * ConversionFunctions::
  xfsm_to_hfst_basic_transducer(NETptr t) 
  {
    return NULL;
  }


  /* ------------------------------------------------------------------------
     
     Create an xfsm transducer equivalent to HfstBasicTransducer \a hfst_fsm. 

     ------------------------------------------------------------------------ */

  NETptr ConversionFunctions::
    hfst_basic_transducer_to_xfsm(const HfstBasicTransducer * hfst_fsm) 
  {
    NETptr result = null_net();

    // Maps HfstBasicTransducer states (i.e. vector indices) into Xfsm transducer states
    std::vector<STATEptr> state_vector;

    // ---- Copy states -----
    unsigned int fsm_state = 0;
    for (HfstBasicTransducer::const_iterator it = hfst_fsm->begin();
         it != hfst_fsm->end(); it++)
      {
        // TODO: slow to call is_final_state each time a new state is added
        STATEptr xfsm_state = add_state_to_net(result, hfst_fsm->is_final_state(fsm_state));
        state_vector.push_back(xfsm_state);
        fsm_state++;
      }

    // ----- Go through all states -----
    unsigned int source_state=0;
    for (HfstBasicTransducer::const_iterator it = hfst_fsm->begin();
         it != hfst_fsm->end(); it++)
      {
        STATEptr xfsm_source_state = state_vector[source_state];
        // ----- Go through the set of transitions in each state -----
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it
               = it->begin();
             tr_it != it->end(); tr_it++)
          {
            // Copy the transition
            const char * input = tr_it->get_input_symbol().c_str();
            const char * output = tr_it->get_output_symbol().c_str();
            HfstState target =  tr_it->get_target_state();

            STATEptr xfsm_target_state = state_vector[target];
            // TODO: handle special symbols
            id_type ti = pair_to_id(input, output);

            if( add_arc_to_state(result, xfsm_source_state, ti, xfsm_target_state, NULL, 0) == NULL )
              throw "add_arc_to_state failed";
          }
        // ----- transitions gone through -----
        source_state++;
      }
    // ----- all states gone through -----


    // ----- Go through the final states -----
    /*for (HfstBasicTransducer::FinalWeightMap::const_iterator it
           = hfst_fsm->final_weight_map.begin();
         it != hfst_fsm->final_weight_map.end(); it++)
      {
        // Set the state as final, ignore the weight
        HfstState final = it->first;
        }*/
    // ----- final states gone through -----

    // TODO: copy alphabet

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

