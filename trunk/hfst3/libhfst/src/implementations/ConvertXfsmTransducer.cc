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

  static HfstBasicTransducer * xfsm_page_string_to_hfst_basic_transducer(char * str)
  {
    return NULL;
  }

  HfstBasicTransducer * ConversionFunctions::
  xfsm_to_hfst_basic_transducer(NETptr t) 
  {
    std::cerr << "xfsm_to_hfst_basic_transducer..." << std::endl;
    PAGEptr p = new_page();
    p = network_to_page(t, p);
    std::string page(p->string);

    std::string start("s0:");
    std::size_t found = page.find(start);
    assert(found != std::string::npos);
    // initial state is final
    if (page.at(found-1) == 'f')
      {
        found = (found-1);
      }
    page = page.substr(found);

    std::cerr << "page is: '" << page << "'" << std::endl;

    STATEptr state_pointer = t->body.states;

    while (state_pointer != NULL)
      {
        std::cerr << "a state:" << std::endl;
        ARCptr arc_pointer = state_pointer->arc.set;
        while (arc_pointer != NULL)
          {
            std::cerr << "  an arc: ";
            id_type label = arc_pointer->label;
            std::cerr << label << " ";
            (void) print_label(label, stderr, ESCAPE);
            std::cerr << std::endl;
            arc_pointer = arc_pointer->next;
          }
        state_pointer = state_pointer->next;
      }

    //HfstBasicTransducer * result = xfsm_page_string_to_hfst_basic_transducer(p->string());
    // todo: delete page?
    return NULL;
  }


  /* ------------------------------------------------------------------------
     
     Create an xfsm transducer equivalent to HfstBasicTransducer \a hfst_fsm. 

     ------------------------------------------------------------------------ */

  NETptr ConversionFunctions::
    hfst_basic_transducer_to_xfsm(const HfstBasicTransducer * hfst_fsm) 
  {
    std::cerr << "hfst_basic_transducer_to_xfsm..." << std::endl;
    NETptr result = null_net();

    // Maps HfstBasicTransducer states (i.e. vector indices) into Xfsm transducer states
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

    // DEBUG
    fsm_state = 0;
    for (std::vector<STATEptr>::const_iterator it = state_vector.begin(); it != state_vector.end(); it++)
      {
        std::cerr << "state_vector[" << fsm_state << "] == " << *it << std::endl;
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
            std::string input = tr_it->get_input_symbol();
            std::string output = tr_it->get_output_symbol();
            HfstState target =  tr_it->get_target_state();

            id_type input_id, output_id;

            if (input == hfst::internal_epsilon)
              input_id = 0;
            else
              input_id = single_to_id(input.c_str());

            if (output == hfst::internal_epsilon)
              output_id = 0;
            else
              output_id = single_to_id(output.c_str());

            STATEptr xfsm_target_state = state_vector.at(target);
            // TODO: handle special symbols
            id_type ti = id_pair_to_id(input_id, output_id);

            if( add_arc_to_state(result, xfsm_source_state, ti, xfsm_target_state, NULL, 0) == NULL )
              throw "add_arc_to_state failed";

            // DEBUG
            std::cerr << "add_arc_to_state: " << xfsm_source_state << " <" << ti << "> " << xfsm_target_state << std::endl; 
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

