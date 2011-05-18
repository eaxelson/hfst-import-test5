#ifndef HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS
#define HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS


#include <set>
#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>


typedef std::set<std::string> StringSet;

#include "HfstDataTypes.h"

namespace hfst
{
using implementations::HfstBasicTransducer;
using implementations::HfstBasicTransition;

#ifdef TEST_HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS
#define debug_harmonize 1
#else
#define debug_harmonize 0
#endif // TEST_HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS

class HarmonizeUnknownAndIdentitySymbols
{
 public:
  static const char * identity;
  static const char * unknown;
  
  // Constructor whose side effect it is to harmonize the identity and unknown
  // symbols of its arguments.
  HarmonizeUnknownAndIdentitySymbols
    (HfstBasicTransducer &,HfstBasicTransducer &);  
 protected:

  HfstBasicTransducer &t1;
  HfstBasicTransducer &t2;
  StringSet t1_symbol_set;
  StringSet t2_symbol_set;
  
  // Collect the symbols from the transitions of the argument transducer
  // and store them in the argument set. This is needed since the alphabet
  // of HfstBasicTransducers can sometimes unexplainedly be empty...
  void populate_symbol_set(const HfstBasicTransducer &,StringSet &);

  // For every x in the set, add x:x transitions for every identity:identity 
  // transition in the argument transducer (the source and target states as 
  // well as the weights are the same as in the original identity transition.
  void harmonize_identity_symbols
    (HfstBasicTransducer &,const StringSet &);

  // For every x in the set 
  // 1. add, x:c transitions for every unknown:c transition in the argument 
  //    transducer. 
  // 2. add, c:x transitions for every c:unknown transition in the argument
  //    transducer.
  //
  // For every x and y in the set with x != y, add x:y transitions for every
  // unknown:unknown transition in the argument transducer.
  //
  // (the source and target states as well as the weights are the same as in 
  // the original identity transition)
  void harmonize_unknown_symbols
    (HfstBasicTransducer &,const StringSet &);
};

void debug_harmonize_print(const StringSet &);
void debug_harmonize_print(const std::string &);
size_t max(size_t t1,size_t t2);

}


#endif // HARMONIZE_UNKNOWN_AND_IDENTITY_SYMBOLS
