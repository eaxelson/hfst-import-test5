#ifndef _INTERNAL_TRANSDUCER_H_
#define _INTERNAL_TRANSDUCER_H_

#include <string>
#include <vector>
#include "HfstAlphabet.h"

namespace hfst {
  namespace implementations {
  
    class InternalTransducerLine {
    public:
      bool final_line;
      unsigned int origin;
      unsigned int target;
      unsigned int isymbol;
      unsigned int osymbol;
      float weight;
      
    InternalTransducerLine():
      final_line(false), origin(0), target(0), 
	isymbol(0), osymbol(0),
	weight((float)0) 
	  {};
    };
    
    class HfstInternalTransducer;

    class HfstInternalTransducer {
    public:
      std::vector<InternalTransducerLine> lines;
      HfstAlphabet * alphabet;

      HfstInternalTransducer();
      ~HfstInternalTransducer();

      void add_line(unsigned int final_state, float final_weight); 
      void add_line(unsigned int origin_state, unsigned int target_state,
		    unsigned int isymbol, unsigned int osymbol,
		    float weight);
      bool has_no_lines();
      std::vector<InternalTransducerLine> *get_lines();
      void print_number(FILE*);
      void print_symbol(FILE*);
    };

  }
}
#endif

