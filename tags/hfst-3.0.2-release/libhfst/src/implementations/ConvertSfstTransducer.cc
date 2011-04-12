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

#ifndef DEBUG_MAIN
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
  ( SFST::Node *node, SFST::NodeNumbering &index, 
    /*std::set<SFST::Node*> &visited_nodes,*/ 
    HfstBasicTransducer *net, SFST::Alphabet &alphabet ) {
  
    // If node has not been visited before
    //if (visited_nodes.find(node) == visited_nodes.end() ) {
    if (not node->was_visited(VMARK)) {
      //visited_nodes.insert(node);
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
				      /*visited_nodes,*/ 
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
    //std::set<SFST::Node*> visited_nodes;
    if (t->root_node()->check_visited(VMARK))
      VMARK++;
   
    sfst_to_hfst_basic_transducer(t->root_node(), index, 
				  /*visited_nodes,*/ 
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

  std::vector<SFST::Node*> state_vector;
  state_vector.push_back(t->root_node());
  for (unsigned int i=1; i <= (net->max_state); i++) {
    state_vector.push_back(t->new_node());
  }

  // Go through all states
  for (HfstBasicTransducer::const_iterator it = net->begin();
       it != net->end(); it++)
    {
      // Go through the set of transitions in each state
      for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
             = it->second.begin();
           tr_it != it->second.end(); tr_it++)
        {
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
	  state_vector[it->first]->add_arc
	    (l, state_vector[tr_it->get_target_state()], t);
					   
        }
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

  // *** THE NEW FUNCTIONS ***

  void ConversionFunctions::sfst_to_hfst_constant_transducer
  ( SFST::Node *node, SFST::NodeNumbering &index, 
    /*std::set<SFST::Node*> &visited_nodes,*/ 
    HfstConstantTransducer *net)
  {
    // If node has not been visited before
    //if (visited_nodes.find(node) == visited_nodes.end() ) { 
    if (not node->was_visited(VMARK)) {
      //visited_nodes.insert(node);
      SFST::Arcs *arcs=node->arcs();

      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;

        net->add_transition(index[node], 
                            index[arc->target_node()],
			    arc->label().lower_char(),
			    arc->label().upper_char(),
			    0);
      }
      
      if (node->is_final()) {
        net->set_final_weight(index[node],0);
      }

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
        sfst_to_hfst_constant_transducer(arc->target_node(), index, 
					 /*visited_nodes,*/ 
					 net);
      }
    }    
  }
  
  HfstConstantTransducer * ConversionFunctions::sfst_to_hfst_constant_transducer
  (SFST::Transducer * t)
  {
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);

    HfstConstantTransducer * net 
      = new HfstConstantTransducer((unsigned int)index.number_of_nodes());

    // Copy the alphabet
    net->symbol_map[0] = std::string("@_EPSILON_SYMBOL_@");

    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it 
           = cm.begin(); it != cm.end(); it++) {
      if (it->first != 0) { // The epsilon symbol "<>" is not inserted 
	net->symbol_map[it->first] = std::string(it->second);
      }
    }
    
    // The set of nodes that have been visited
    //std::set<SFST::Node*> visited_nodes
    if (t->root_node()->check_visited(VMARK))
      VMARK++;

    sfst_to_hfst_constant_transducer(t->root_node(), index, 
				     /*visited_nodes,*/ 
				     net);
    return net;
  }

  SFST::Transducer * ConversionFunctions::hfst_constant_transducer_to_sfst
  (const HfstConstantTransducer * net)
  {
    SFST::Transducer * t = new SFST::Transducer();
    t->alphabet.add_symbol("@_UNKNOWN_SYMBOL_@", 1);
    t->alphabet.add_symbol("@_IDENTITY_SYMBOL_@", 2);

    // Copy the alphabet
    for (HfstConstantTransducer::SymbolMap::const_iterator it 
	   = net->symbol_map.begin();
	 it != net->symbol_map.end(); it++) {
      if (it->second.compare("@_EPSILON_SYMBOL_@") != 0) {
	t->alphabet.add_symbol(it->second.c_str(), it->first);
      }
    }    


    // each index of state_vector is the state number in \a net
    // that corresponds to the node of \a t   
    std::vector<SFST::Node*> state_vector;
    state_vector.push_back(t->root_node());
    for (unsigned int i=1; i < ((unsigned int)net->states.size()); i++) {
      state_vector.push_back(t->new_node());
    }
    
    // Go through all states
    for (unsigned int i=0; i < net->states.size(); i++)
      {
	// Go through the set of transitions in each state
	for (HfstConstantTransducer::TransitionVector::const_iterator tr_it 
	       = net->states[i].begin();
	     tr_it != net->states[i].end(); tr_it++)
	  {	    
	    SFST::Label l(tr_it->input, tr_it->output);	    
	    state_vector[i]->add_arc
	      (l, state_vector[tr_it->target], t);
	  }
      }
    
    // Go through the final states
    for (HfstConstantTransducer::FinalStateMap::const_iterator it 
	   = net->final_states.begin();
	 it != net->final_states.end(); it++)  
      {
	state_vector[it->first]->set_final(1);
      }
    
    return t;
  }
  
#endif // HAVE_SFST

  }}
#endif // DEBUG_MAIN
