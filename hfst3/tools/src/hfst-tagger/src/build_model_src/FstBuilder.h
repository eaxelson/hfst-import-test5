#ifndef HFST_TAGGER_HEADER_FST_BUILDER_H
#define HFST_TAGGER_HEADER_FST_BUILDER_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <vector>
#include <map>
#include <string>

#include <hfst/HfstTransducer.h>

using hfst::implementations::HfstBasicTransducer;
using hfst::HfstTransducer;
using hfst::ImplementationType;
using hfst::implementations::HfstState;

#define START_STATE 0
#define NO_STATE    UINT_MAX

#include "string_handling.h"

class FstBuilder
{
 public:

  FstBuilder(ImplementationType type,
	     float default_final_weight,
	     const std::string &name);
  HfstTransducer get_model(void);

 protected:

  typedef std::vector<std::string> StringVector;
  typedef std::map<std::string,HfstState> SymbolTargetStateMap;

  ImplementationType type;
  float default_final_weight;
  std::string name;
  HfstBasicTransducer model_fst;
  SymbolTargetStateMap start_state_targets;
  
  void add_transition(HfstState initial_state,
		      HfstState target_state,
		      const std::string &symbol,
		      float weight);

  void add_transition(HfstState initial_state,
		      HfstState target_state,
		      const std::string &isymbol,
		      const std::string &osymbol,
		      float weight);

  HfstState get_target(HfstState s,
		       const std::string &symbol, 
		       bool &new_transition_required);
  StringVector split_at_tabs(const std::string &str);
  virtual void add_sequence(const WeightedStringVector &c,
			    bool is_lexical_model) = 0;
  HfstState add_sequence(StringVector::const_iterator it,
			 StringVector::const_iterator end);
};

#endif // HFST_TAGGER_HEADER_FST_BUILDER_H
