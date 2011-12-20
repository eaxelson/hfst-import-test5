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

#include <stdexcept>

#ifndef MAIN_TEST
namespace hfst { namespace implementations
{


  /* -----------------------------------------------------------

      Conversion functions between HfstBasicTransducer and SFST transducer. 

     ----------------------------------------------------------- */

#if HAVE_SFST

  SFST::VType VMARK = 10000;


  /* Recursively copy all transitions of \a node to \a net.
     Used by function sfst_to_hfst_basic_transducer(SFST::Transducer * t).

     @param node  The current node in the SFST transducer
     @param index  A map that maps nodes to integers
     @param visited_nodes  Which nodes have already been visited
     @param net  The HfstBasicTransducer that is being created
     @param alphabet  The alphabet of the SFST transducer
  */

  void ConversionFunctions::
  sfst_to_hfst_basic_transducer
  ( SFST::Node *node, 
    HfstBasicTransducer *net, SFST::Alphabet &alphabet,
    std::vector<unsigned int> &harmonization_vector) {
  
    // If node has not been visited before
    if (not node->was_visited(VMARK)) {
      SFST::Arcs *arcs=node->arcs();

      // Count the number of nodes and initialize the transition
      // vector of net.
      unsigned int number_of_arcs=0;
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	number_of_arcs++;
      }

      net->initialize_transition_vector(node->index, number_of_arcs);

      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;

	/*    const char *isymbol = alphabet.code2symbol(arc->label().lower_char());
    if (isymbol == NULL) {
      std::cerr << "ERROR: no string found for number " 
            << arc->label().lower_char() << std::endl;
      assert(false);
    }
        std::string istring
          (isymbol);

    const char *osymbol = alphabet.code2symbol(arc->label().upper_char());
    if (osymbol == NULL) {
      std::cerr << "ERROR: no string found for number " 
            << arc->label().upper_char() << std::endl;
      assert(false);
    }
        std::string ostring
	(osymbol);

        if (istring.compare("<>") == 0) {
          istring = std::string(internal_epsilon);
        }
        if (ostring.compare("<>") == 0) {
          ostring = std::string(internal_epsilon);
        }
	*/

        net->add_transition
	  (node->index, 
	   HfstBasicTransition
	   (arc->target_node()->index,
	    harmonization_vector.at(arc->label().lower_char()),
	    harmonization_vector.at(arc->label().upper_char()),
	    0, false), false);
      }

      if (node->is_final()) {
        net->set_final_weight(node->index,0);
      }

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
        sfst_to_hfst_basic_transducer(arc->target_node(), 
				      net, alphabet,
				      harmonization_vector);
      }
    }
  }


  /* Create an HfstBasicTransducer equivalent to an SFST transducer \a t. */
  HfstBasicTransducer * ConversionFunctions::
  sfst_to_hfst_basic_transducer(SFST::Transducer * t) {

    StringSet alphabet_before; // DEBUG
    SFST::Alphabet::CharMap CM = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it 
           = CM.begin(); it != CM.end(); it++) 
      {
    if (it->first != 0)
      alphabet_before.insert(std::string(it->second));
    else
      alphabet_before.insert(internal_epsilon);
      }  

    StringVector symbol_vector = SfstTransducer::get_symbol_vector(t);
    symbol_vector.at(0) = "@_EPSILON_SYMBOL_@";
    std::vector<unsigned int> harmonization_vector
      = HfstTropicalTransducerTransitionData::get_harmonization_vector(symbol_vector);
    
    std::vector<SFST::Node*> indexing;
    t->nodeindexing(&indexing);
    unsigned int number_of_nodes = (unsigned int)indexing.size();

    HfstBasicTransducer * net = new HfstBasicTransducer();
    net->initialize_state_vector(number_of_nodes);
    
    if (t->root_node()->check_visited(VMARK))
      VMARK++;
   
    sfst_to_hfst_basic_transducer
      (t->root_node(), 
       net, t->alphabet, harmonization_vector);
    
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
    
    // DEBUG
    StringSet alphabet_after = net->get_alphabet();
    assert(alphabet_after == alphabet_before);

    return net;
  }


  /* Create an SFST::Transducer equivalent to HfstBasicTransducer \a net. */
  SFST::Transducer * ConversionFunctions::
  hfst_basic_transducer_to_sfst(const HfstBasicTransducer * net) {

  SFST::Transducer * t = new SFST::Transducer();
  t->alphabet.add_symbol(internal_unknown.c_str(), 1);
  t->alphabet.add_symbol(internal_identity.c_str(), 2);

  // Make sure that also symbols that occur in the alphabet of the
  // HfstBasicTransducer but not in its transitions are inserted to 
  // the SFST transducer
  for (HfstBasicTransducer::HfstTransitionGraphAlphabet::iterator it 
         = net->alphabet.begin();
       it != net->alphabet.end(); it++) {
    if (not is_epsilon(*it) && not is_unknown(*it) && not is_identity(*it))
      t->alphabet.add_symbol(it->c_str(), net->get_symbol_number(*it));
  }

  std::vector<SFST::Node*> state_vector;
  state_vector.push_back(t->root_node());
  for (unsigned int i=1; i <= (net->get_max_state()); i++) {
    state_vector.push_back(t->new_node());
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
          std::string istring(tr_it->get_input_symbol());
          std::string ostring(tr_it->get_output_symbol());

          if (is_epsilon(istring)) {
            istring = std::string("<>");
          }

          if (is_epsilon(ostring)) {
            ostring = std::string("<>");
          }

          SFST::Label l
            (t->alphabet.symbol2code(istring.c_str()),
             t->alphabet.symbol2code(ostring.c_str()));
          
          // Copy transition to node
      state_vector[source_state]->add_arc
        (l, state_vector[tr_it->get_target_state()], t);
                       
        }
      source_state++;
    }

  // Go through the final states
  for (HfstBasicTransducer::FinalWeightMap::const_iterator it 
         = net->final_weight_map.begin();
       it != net->final_weight_map.end(); it++) 
    {
      if (it->first >= state_vector.size()) { // should not happen..
    state_vector.push_back(t->new_node());
      }
      state_vector[it->first]->set_final(1);
    }
  
  return t;
}
  
#endif // HAVE_SFST

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

