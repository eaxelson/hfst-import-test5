#ifndef _HFST_CONVERSION_TRANSDUCER_H_
#define _HFST_CONVERSION_TRANSDUCER_H_

#include <cstdio>
#include <string>
#include <vector>
#include <set>

namespace hfst {

  struct HfstConversionTransition
  {
    unsigned int target;
    unsigned int input;
    unsigned int output;
    float weight;

    HfstConversionTransition();
    HfstConversionTransition(unsigned int target,
			     unsigned int input,
			     unsigned int output,
			     float weight);
  };

  struct HfstConversionTransducer 
  {
    std::vector<std::vector<HfstConversionTransition> > states;
    std::vector<std::string> symbol_vector;
    std::set<std::string> alphabet;
   
    typedef std::vector<std::vector<HfstConversionTransition> >::iterator 
    iterator;

    HfstConversionTransducer();
    HfstConversionTransducer(unsigned int max_state_number);
    void add_transition(unsigned int source,
			unsigned int target,
			unsigned int input,
			unsigned int output,
			float weight);
    iterator begin();
    iterator end();
  };

}


#endif // _HFST_CONVERSION_TRANSDUCER_H_
