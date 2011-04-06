#include "HfstConstantTransducer.h"

namespace hfst {

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
  (unsigned int max_state_number)
  {
    for (unsigned int i=0; i < max_state_number; i++) {
      TransitionVector v;
      states.push_back(v);
    }
  }
  
  void HfstConstantTransducer::initialize_symbol_map()
  {
    symbol_map[0] = std::string("@_EPSILON_SYMBOL_@");
    symbol_map[1] = std::string("@_UNKNOWN_SYMBOL_@");
    symbol_map[2] = std::string("@_IDENTITY_SYMBOL_@");
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
