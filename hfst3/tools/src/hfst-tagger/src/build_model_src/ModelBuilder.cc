#include "ModelBuilder.h"

#ifndef MAIN_TEST

using hfst::internal_epsilon;

using hfst::TROPICAL_OPENFST_TYPE;

bool ModelBuilder::verbose=false;

void ModelBuilder::verbose_printf(const std::string &message)
{
  if (ModelBuilder::verbose)
    { std::cerr << message; }
}

ModelBuilder::ModelBuilder
(const WeightedStringVectorCollection &model_weights):
  FstBuilder(TROPICAL_OPENFST_TYPE,
	     std::numeric_limits<float>::infinity(),
	     model_weights.get_name())
{
  for (WeightedStringVectorCollection::const_iterator it = 
	 model_weights.begin();
       it != model_weights.end();
       ++it)
    { 
      add_sequence(*it,
		   (model_weights.is_lexical_model ? LEXICAL : SEQUENCE)); 
    }
}

void ModelBuilder::add_sequence(const WeightedStringVector &v,
				weighted_string_type string_type)
{
  StringVector symbol_sequence = v.string_vector;
  float weight = v.weight;

  // This should only happen once with the prob of the empty suffix.
  if (symbol_sequence.empty())
    {
      model_fst.set_final_weight(START_STATE,weight);
      return;
    }

  HfstState target_state =
    FstBuilder::add_sequence(symbol_sequence.begin(),
			     symbol_sequence.end() - 1);     
  
  std::string last_output_symbol = symbol_sequence.back();
  std::string last_input_symbol = 
    ((string_type == LEXICAL) ? internal_epsilon : symbol_sequence.back());
  
  HfstState final_state = 
    ((string_type == LEXICAL) ? model_fst.add_state() : START_STATE);

  FstBuilder::add_transition(target_state,
			       final_state,
			       last_input_symbol,
			       last_output_symbol,
			       weight);

  model_fst.set_final_weight(final_state,0.0);
}

#else // MAIN_TEST

#include <iostream>
#include <cassert>

using hfst::HfstTwoLevelPaths;
using hfst::HfstTwoLevelPath;
using hfst::StringPairVector;
using hfst::StringPair;
using hfst::internal_epsilon;

int main(void)
{

  std::string line1 = "START LEX MODEL\n";
  std::string line2 = "a\tDT\t1.0\n";
  std::string line3 = "oof\tNN\t2.0\n";
  std::string line4 = "STOP LEX MODEL\n";

  std::string file1 = line1 + line2 + line3 + line4;

  std::istringstream in1(file1);

  WeightedStringVectorCollection c1(in1,LEXICAL);

  ModelBuilder model_builder1(c1);

  HfstTransducer model = model_builder1.get_model();
  assert(model.get_name() == "LEX MODEL");

  HfstTwoLevelPaths paths;
  model.extract_paths(paths);

  assert(paths.size() == 2);

  StringPairVector a_vector;
  a_vector.push_back(StringPair("a","a"));
  a_vector.push_back(StringPair(internal_epsilon,"DT"));

  StringPairVector foo_vector;
  foo_vector.push_back(StringPair("o","o"));
  foo_vector.push_back(StringPair("o","o"));
  foo_vector.push_back(StringPair("f","f"));
  foo_vector.push_back(StringPair(internal_epsilon,"NN"));

  assert(paths.count(HfstTwoLevelPath(1,a_vector)) == 1);
  assert(paths.count(HfstTwoLevelPath(2,foo_vector)) == 1);
}

#endif // MAIN_TEST