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
    HfstBasicTransducer * result = new HfstBasicTransducer();

    //std::cerr << "xfsm_to_hfst_basic_transducer..." << std::endl;
    //PAGEptr p = new_page();
    //p = network_to_page(t, p);
    //std::string page(p->string);

    //std::string start("s0:");
    //std::size_t found = page.find(start);
    //assert(found != std::string::npos);
    // initial state is final
    //if (page.at(found-1) == 'f')
    //  {
    //    found = (found-1);
    //  }
    //page = page.substr(found);

    //std::cerr << "page is: '" << page << "'" << std::endl;
    //free_page(p);

    std::map<STATEptr, HfstState> state_map;

    STATEptr state_pointer = t->body.states;
    STATEptr start_pointer = t->start.state;

    while (state_pointer != NULL)
      {
        HfstState s;
        if (state_pointer == start_pointer) 
          {
            s = 0;
            state_map[state_pointer] = s; // initial state exists already in result
            //std::cerr << "state_map[" << state_pointer << "] = " << s << " (start)" << std::endl;
          }            
        else
          {
            s = result->add_state();
            state_map[state_pointer] = s;
            //std::cerr << "state_map[" << state_pointer << "] = " << s << std::endl;
          }

        if (state_pointer->final != 0)
          {
            result->set_final_weight(s, 0);
            //std::cerr << "set_final_weight: " << s << std::endl;
          }
          
        state_pointer = state_pointer->next;
      }

    state_pointer = t->body.states;

    while (state_pointer != NULL)
      {
        /*std::cerr << "state " << state_pointer << " ";
        if (state_pointer == start_pointer)
          std::cerr << "(start)";
        if (state_pointer->final != 0)
          std::cerr << "(final)";
          std::cerr << ":" << std::endl;*/

        ARCptr arc_pointer = state_pointer->arc.set;
        while (arc_pointer != NULL)
          {
            id_type label = arc_pointer->label;
            id_type upper = upper_id(label);
            id_type lower = lower_id(label);

            std::string pstru, pstrl;

            if (upper == 0)
              pstru = hfst::internal_epsilon;
            else if (upper == 1)
              pstru = hfst::internal_unknown;
            else {
              PAGEptr pp = new_page();
              label_to_page(upper, ESCAPE, DONT_WATCH_RM, pp);
              pstru = std::string(pp->string);
              free_page(pp);
            }

            if (lower == 0)
              pstrl = hfst::internal_epsilon;
            else if (lower == 1)
              pstrl = hfst::internal_unknown;
            else {
              PAGEptr pp = new_page();
              label_to_page(lower, ESCAPE, DONT_WATCH_RM, pp);
              pstrl = std::string(pp->string);
              free_page(pp);
            }
            
            STATEptr dest_ptr = arc_pointer->destination;

            //std::cerr << "'" << pstru << "':'" << pstrl << "' -> " << dest_ptr << std::endl;

            // state_pointer pstru pstrl dest_ptr
            HfstBasicTransition tr(state_map[dest_ptr], pstru, pstrl, 0);
            result->add_transition(state_map[state_pointer], tr);

            arc_pointer = arc_pointer->next;
          }
        state_pointer = state_pointer->next;
      }

    return result;
  }

  static void symbol_pair_to_id(const std::string & input, const std::string & output, 
                                id_type & input_id, id_type & output_id)
  {
    if (input == hfst::internal_epsilon)
      input_id = 0;
    else if (input == hfst::internal_unknown)
      input_id = 1;
    else
      input_id = single_to_id(input.c_str());
    
    if (output == hfst::internal_epsilon)
      output_id = 0;
    else if (output == hfst::internal_unknown)
      output_id = 1;
    else
      output_id = single_to_id(output.c_str());
  }


  /* ------------------------------------------------------------------------
     
     Create an xfsm transducer equivalent to HfstBasicTransducer \a hfst_fsm. 

     ------------------------------------------------------------------------ */

  NETptr ConversionFunctions::
    hfst_basic_transducer_to_xfsm(const HfstBasicTransducer * hfst_fsm) 
  {
    //std::cerr << "hfst_basic_transducer_to_xfsm..." << std::endl;
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
    //fsm_state = 0;
    /*for (std::vector<STATEptr>::const_iterator it = state_vector.begin(); it != state_vector.end(); it++)
      {
        std::cerr << "state_vector[" << fsm_state << "] == " << *it << std::endl;
        fsm_state++;
        }*/

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

            symbol_pair_to_id(input, output, input_id, output_id);

            //std::cerr << "input_id: " << input_id << ", output id: " << output_id << std::endl;

            STATEptr xfsm_target_state = state_vector.at(target);
            // TODO: handle special symbols
            id_type ti = id_pair_to_id(input_id, output_id);

            if( add_arc_to_state(result, xfsm_source_state, ti, xfsm_target_state, NULL, 0) == NULL )
              throw "add_arc_to_state failed";

            // DEBUG
            //std::cerr << "add_arc_to_state: " << xfsm_source_state << " <" << ti << "> " << xfsm_target_state << std::endl; 
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

