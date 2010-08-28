#ifndef _INTERNAL_TRANSDUCER_H_
#define _INTERNAL_TRANSDUCER_H_

#include <string>
#include <set>
#include "HfstAlphabet.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include <cassert>
#include <iostream>

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

      bool operator<(const InternalTransducerLine &another) const;
    };
    
    class HfstInternalTransducer;

    class HfstTransition {
    public:
      unsigned int source;
      unsigned int target;
      std::string isymbol;
      std::string osymbol;
      float weight;
      
      bool operator<(const HfstTransition &another) const;
    };
    
    class HfstInternalTransducer {
    public:
      std::set<InternalTransducerLine> lines;
      std::set<std::pair<unsigned int,float> > final_states;
      HfstAlphabet * alphabet;

      HfstInternalTransducer();
      ~HfstInternalTransducer();
      HfstInternalTransducer(const HfstInternalTransducer &transducer);

      void add_line(unsigned int final_state, float final_weight); 
      void add_line(unsigned int origin_state, unsigned int target_state,
		    unsigned int isymbol, unsigned int osymbol,
		    float weight);
      bool has_no_lines();
      std::set<InternalTransducerLine> *get_lines();
      unsigned int max_state_number();

      void add_transition(HfstTransition &transition);
      void add_final_state(unsigned int s, float weight);

      bool is_final_state(unsigned int);
      float get_final_weight(unsigned int);
      void read_number(FILE*);
      void read_symbol(FILE*);
      void print_number(FILE*);
      void print_symbol(FILE*);
      void print_number(std::ostream&);
      void print_symbol(std::ostream&);
      void substitute(const StringPair &sp, const StringPairSet &sps);
      void substitute(void (*func)(std::string &isymbol, std::string &osymbol) );   
      void substitute(const StringPair &sp, HfstInternalTransducer &transducer);
      void substitute(const std::string &old_symbol, const std::string &new_symbol,
		      bool input_side=true, bool output_side=true);
      void substitute(const StringPair &old_pair, const StringPair &new_pair); 
      void insert_freely(const StringPair &symbol_pair);

      friend class HfstStateIterator;
      friend class HfstTransitionIterator;
    };

    class HfstStateIterator {
    protected:
      std::set<unsigned int> state_set;
      std::set<unsigned int>::iterator it;
    public:
      HfstStateIterator(const HfstInternalTransducer &transducer);
      unsigned int value();
      void next();
      bool done();
    };

    class HfstTransitionIterator {
    protected:
      std::set<HfstTransition> transition_set;
      std::set<HfstTransition>::iterator it;
    public:
      HfstTransitionIterator(const HfstInternalTransducer &transducer, unsigned int s);
      HfstTransition value();
      void next();
      bool done();
    };

  }
}
#endif

