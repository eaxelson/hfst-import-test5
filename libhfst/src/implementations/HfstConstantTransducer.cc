#include "HfstConstantTransducer.h"
#include "HfstSymbolDefs.h"

#ifndef MAIN_TEST

namespace hfst {
  namespace implementations {

  HfstConstantTransition::HfstConstantTransition() :
    target(0), input(0), output(0), weight(0) {}

  HfstConstantTransition::HfstConstantTransition
  (unsigned int target,
   unsigned int input,
   unsigned int output,
   float weight) : 
    target(target), input(input), output(output), weight(weight) {}

  HfstConstantTransducer::HfstConstantTransducer() {}

  HfstConstantTransducer::HfstConstantTransducer
  (unsigned int number_of_states)
  {
    states.reserve(number_of_states);
    for (unsigned int i=0; i < number_of_states; i++) {
      TransitionVector v;
      states.push_back(v);
    }
  }

  void HfstConstantTransducer::initialize_transition_vector
  (unsigned int state_number, unsigned int number_of_transitions)
  {
    states[state_number].reserve(number_of_transitions);
  }
  
  void HfstConstantTransducer::initialize_symbol_map()
  {
    symbol_map[0] = std::string(internal_epsilon);
    symbol_map[1] = std::string(internal_unknown);
    symbol_map[2] = std::string(internal_identity);
  }

  void HfstConstantTransducer::add_transition
  (unsigned int source,
   unsigned int target,
   unsigned int input,
   unsigned int output,
   float weight)
  {
    if (source >= states.size()) {
      for (unsigned int i = 0; i <= (states.size()-source); i++ ) {
	TransitionVector v;
	states.push_back(v);
      }
    }
    HfstConstantTransition tr(target, input, output, weight);
    states[source].push_back(tr);
  }

  void HfstConstantTransducer::set_final_weight
  (unsigned int state, float weight)
  {
    final_states[state] = weight;
  }

  HfstConstantTransducer::iterator HfstConstantTransducer::begin()
  {
    return states.begin();
  }

  HfstConstantTransducer::iterator HfstConstantTransducer::end()
  {
    return states.end();
  }

  HfstConstantTransducer::const_iterator HfstConstantTransducer::const_begin()
  {
    return states.begin();
  }

  HfstConstantTransducer::const_iterator HfstConstantTransducer::const_end()
  {
    return states.end();
  }

  }
}

#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST
