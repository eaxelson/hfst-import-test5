#ifndef _HFST_CONSTANT_TRANSDUCER_H_
#define _HFST_CONSTANT_TRANSDUCER_H_

#include <cstdio>
#include <string>
#include <vector>
#include <map>

namespace hfst {
  namespace implementations {

  struct HfstConstantTransition
  {
    unsigned int target;
    unsigned int input;
    unsigned int output;
    float weight;

    HfstConstantTransition();
    HfstConstantTransition(unsigned int target,
			   unsigned int input,
			   unsigned int output,
			   float weight);
  };

  struct HfstConstantTransducer 
  {
    typedef std::vector<HfstConstantTransition> TransitionVector;
    typedef std::vector<TransitionVector> StateVector;
    typedef std::map<unsigned int, float> FinalStateMap;
    typedef std::map<unsigned int, std::string> SymbolMap;

    StateVector states;
    FinalStateMap final_states;
    SymbolMap symbol_map;
   
    typedef StateVector::iterator iterator;
    typedef StateVector::const_iterator const_iterator;

    HfstConstantTransducer();
    HfstConstantTransducer(unsigned int number_of_states);
    void initialize_symbol_map();

    void add_transition(unsigned int source,
			unsigned int target,
			unsigned int input,
			unsigned int output,
			float weight);
    void set_final_weight(unsigned int state, float weight);
    iterator begin();
    iterator end();
    const_iterator const_begin();
    const_iterator const_end();
  };

  }
}


#endif // _HFST_CONSTANT_TRANSDUCER_H_
