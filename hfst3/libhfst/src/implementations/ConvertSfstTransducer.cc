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
    // NODE_NUMBERING
  void ConversionFunctions::
  sfst_to_hfst_basic_transducer
  ( SFST::Node *node, 
#ifdef FOO
    SFST::NodeNumbering &index, 
#endif
    HfstBasicTransducer *net, SFST::Alphabet &alphabet ) {
  
    // If node has not been visited before
    if (not node->was_visited(VMARK)) {
      SFST::Arcs *arcs=node->arcs();

      // Count the number of nodes and initialize the transition
      // vector of net.
      unsigned int number_of_arcs=0;
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	number_of_arcs++;
      }
      // NODE_NUMBERING
      net->initialize_transition_vector(node->index, number_of_arcs);

      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;

    const char *isymbol = alphabet.code2symbol(arc->label().lower_char());
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

    // NODE_NUMBERING
        net->add_transition(node->index, 
                            HfstBasicTransition
                            (arc->target_node()->index,
                             istring,
                             ostring,
                             0));
      }

      if (node->is_final())     // NODE_NUMBERING
        net->set_final_weight(node->index,0);

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
        sfst_to_hfst_basic_transducer(arc->target_node(), 
#ifdef FOO
				      index,  
#endif
				      net, alphabet);
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
    
    std::vector<SFST::Node*> indexing;
    t->nodeindexing(&indexing);
    unsigned int number_of_nodes = (unsigned int)indexing.size();

    HfstBasicTransducer * net = new HfstBasicTransducer();
    net->initialize_state_vector(number_of_nodes);
    
#ifdef FOO
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);     // NODE_NUMBERING
#endif

    if (t->root_node()->check_visited(VMARK))
      VMARK++;
   
    sfst_to_hfst_basic_transducer
      (t->root_node(), 
#ifdef FOO
       index,
#endif
       //visited_nodes,
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




  /* Recursively copy all transitions of \a node to \a net.
     Used by function sfst_to_hfst_fast_transducer(SFST::Transducer * t).

     @param node  The current node in the SFST transducer
     @param index  A map that maps nodes to integers
     @param visited_nodes  Which nodes have already been visited
     @param net  The HfstFastTransducer that is being created
  */
    // NODE_NUMBERING
  void ConversionFunctions::
  sfst_to_hfst_fast_transducer
  ( SFST::Node *node, 
#ifdef FOO
    SFST::NodeNumbering &index,
#endif 
    HfstFastTransducer *net, NumberVector &harmonization_vector) {
  
    // If node has not been visited before
    if (not node->was_visited(VMARK)) {
      SFST::Arcs *arcs=node->arcs();

      // Count the number of nodes and initialize the transition
      // vector of net.
      unsigned int number_of_arcs=0;
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	number_of_arcs++;
      }
      // NODE_NUMBERING
      net->initialize_transition_vector(node->index, number_of_arcs);

      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
    
    unsigned int in, out;
    try {
      in = harmonization_vector.at(arc->label().lower_char());
      out = harmonization_vector.at(arc->label().upper_char());
    } catch (std::out_of_range e)
      {
        fprintf(stderr, "no index for %i or %i\n",
            arc->label().lower_char(),
            arc->label().upper_char());
        fprintf(stderr, "the size of harmonization_vector is %i\n",
            (unsigned int)harmonization_vector.size());
        assert(false);
      }

    // NODE_NUMBERING
        net->add_transition(node->index, 
                            HfstFastTransition
                            (arc->target_node()->index,
                             in,
                 out,
                             0));
      }

      if (node->is_final())     // NODE_NUMBERING
        net->set_final_weight(node->index,0);

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
        sfst_to_hfst_fast_transducer(arc->target_node(), 
#ifdef FOO
				     index, 
#endif
                      net, harmonization_vector);
      }
    }
  }


  /* Create an HfstFastTransducer equivalent to an SFST transducer \a t. */
  HfstFastTransducer * ConversionFunctions::
  sfst_to_hfst_fast_transducer(SFST::Transducer * t) {

    HfstFastTransducer * net = new HfstFastTransducer();

    std::vector<SFST::Node*> indexing;
    t->nodeindexing(&indexing);
    unsigned int number_of_nodes = (unsigned int)indexing.size();

    net->initialize_state_vector(number_of_nodes);

    // Handle the alphabet
    StringVector coding_vector;
    coding_vector.push_back(internal_epsilon);

    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it 
           = cm.begin(); it != cm.end(); it++) 
      {
        if (it->first != 0) { 
      // it is possible that there are gaps in numbering
      while (coding_vector.size() < it->first) {
        // empty space if no symbol at this index
        coding_vector.push_back(std::string(""));
      }
      coding_vector.push_back(std::string(it->second));
    }
      }
    NumberVector harmonization_vector 
      = get_harmonization_vector(coding_vector);

#ifdef FOO
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);     // NODE_NUMBERING
#endif

    if (t->root_node()->check_visited(VMARK))
      VMARK++;
   
    sfst_to_hfst_fast_transducer
      (t->root_node(), 
#ifdef FOO
       index,
#endif 
       net, harmonization_vector);
    
    return net;
  }


  /* Create an SFST::Transducer equivalent to HfstFastTransducer \a net. */
  SFST::Transducer * ConversionFunctions::
  hfst_fast_transducer_to_sfst(const HfstFastTransducer * net) {

  SFST::Transducer * t = new SFST::Transducer();
  t->alphabet.add_symbol(internal_unknown.c_str(), 1);
  t->alphabet.add_symbol(internal_identity.c_str(), 2);

  // Handle the alphabet
  for (HfstFastTransducer::HfstTransitionGraphAlphabet::iterator it 
         = net->alphabet.begin();
       it != net->alphabet.end(); it++) {
    if (*it != 0) { // The epsilon is "<>" in SFST
      t->alphabet.add_symbol(get_string(*it).c_str(), *it);
    }
  }


  std::vector<SFST::Node*> state_vector;
  state_vector.push_back(t->root_node());
  for (unsigned int i=1; i <= (net->get_max_state()); i++) {
    state_vector.push_back(t->new_node());
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
          SFST::Label l
            (tr_it->get_input_symbol(),
         tr_it->get_output_symbol());             
          
          // Copy transition to node
      state_vector[source_state]->add_arc
        (l, state_vector[tr_it->get_target_state()], t);
                       
        }
      source_state++;
    }

  // Go through the final states
  for (HfstFastTransducer::FinalWeightMap::const_iterator it 
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










  // *** THE NEW FUNCTIONS ***

    // NODE_NUMBERING
  void ConversionFunctions::sfst_to_hfst_constant_transducer
  ( SFST::Node *node, 
#ifdef FOO
    SFST::NodeNumbering &index,
 #endif
    /*std::set<SFST::Node*> &visited_nodes,*/ 
    HfstConstantTransducer *net)
  {
    // If node has not been visited before
    //if (visited_nodes.find(node) == visited_nodes.end() ) { 
    if (not node->was_visited(VMARK)) {
      //visited_nodes.insert(node);
      SFST::Arcs *arcs=node->arcs();

      // Count the number of nodes and initialize the transition
      // vector of net.
      unsigned int number_of_arcs=0;
      for( SFST::ArcsIter p(arcs); p; p++ ) {
	number_of_arcs++;
      }
      // NODE_NUMBERING
      net->initialize_transition_vector(node->index, number_of_arcs);
      
      // Go through all transitions and copy them to \a net
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;

    // NODE_NUMBERING
        net->add_transition(node->index, 
                            arc->target_node()->index,
                arc->label().lower_char(),
                arc->label().upper_char(),
                0);
      }
      
      if (node->is_final()) {     // NODE_NUMBERING
        net->set_final_weight(node->index,0);
      }

      // Call this function recursively for all target nodes
      // of the transitions
      for( SFST::ArcsIter p(arcs); p; p++ ) {
        SFST::Arc *arc=p;
        sfst_to_hfst_constant_transducer
	  (arc->target_node(), 
#ifdef FOO
	   index, 
#endif
	   net);
      }
    }    
  }
  
  HfstConstantTransducer * ConversionFunctions::sfst_to_hfst_constant_transducer
  (SFST::Transducer * t)
  {
#ifdef FOO
    // A map that maps nodes to integers
    SFST::NodeNumbering index(*t);     // NODE_NUMBERING
    HfstConstantTransducer * net 
      = new HfstConstantTransducer((unsigned int)index.number_of_nodes());
#endif
    std::vector<SFST::Node*> indexing;
    t->nodeindexing(&indexing);
    unsigned int number_of_nodes = (unsigned int)indexing.size();
    HfstConstantTransducer * net 
      = new HfstConstantTransducer(number_of_nodes);

    // Copy the alphabet
    net->symbol_map[0] = std::string(internal_epsilon);

    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it 
           = cm.begin(); it != cm.end(); it++) {
      if (it->first != 0) { // The epsilon symbol "<>" is not inserted 
    net->symbol_map[it->first] = std::string(it->second);
      }
    }
    
    if (t->root_node()->check_visited(VMARK))
      VMARK++;

    sfst_to_hfst_constant_transducer
      (t->root_node(), 
#ifdef FOO
       index,
#endif 
       net);

    return net;
  }

  SFST::Transducer * ConversionFunctions::hfst_constant_transducer_to_sfst
  (const HfstConstantTransducer * net)
  {
    SFST::Transducer * t = new SFST::Transducer();
    t->alphabet.add_symbol(internal_unknown.c_str(), 1);
    t->alphabet.add_symbol(internal_identity.c_str(), 2);

    // Copy the alphabet
    for (HfstConstantTransducer::SymbolMap::const_iterator it 
       = net->symbol_map.begin();
     it != net->symbol_map.end(); it++) {
      if (not is_epsilon(it->second)) {
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
#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST

