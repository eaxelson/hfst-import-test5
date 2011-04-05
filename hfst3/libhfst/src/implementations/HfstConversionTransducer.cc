#include "HfstConversionTransducer.h"

namespace hfst {

  HfstConversionTransition::HfstConversionTransition() :
    target(0), input(0), output(0), weight(0) {}

  HfstConversionTransition::HfstConversionTransition
  (unsigned int &target,
   unsigned int &input,
   unsigned int &output,
   float &weight) : 
    target(target), input(input), output(output), weight(weight) {}

  HfstConversionTransducer::HfstConversionTransducer() {}

  HfstConversionTransducer::HfstConversionTransducer
  (unsigned int max_state_number)
  {
    for (unsigned int i=0; i < max_state_number) {
      std::vector<HfstConversionTransition> v;
      states.push_back(v);
    }
  }
  
  void HfstConversionTransducer::add_transition
  (unsigned int &source,
   unsigned int &target,
   unsigned int &input,
   unsigned int &output,
   float &weight)
  {
    if (source >= states.size()) {
      for (unsigned int i = 0; i <= (states.size()-source) ) {
	std::vector<HfstConversionTransition> v;
	states.push_back(v);
      }
    }
    HfstConversionTransition tr(source, target, input, output, weight)
    states[source].push_back(tr);
  }

  HfstConversionTransducer::iterator HfstConversionTransducer::begin()
  {
    return states.begin();
  }

  HfstConversionTransducer::iterator HfstConversionTransducer::end()
  {
    return states.end();
  }

}
