#include "Speller.h"

using hfst::TROPICAL_OPENFST_TYPE;

Speller::Speller(const std::string &lexical_model_file_name,
		 const std::string &sequence_model_file_name,
		 float scaling_coefficient):
  HMMTagger(lexical_model_file_name,sequence_model_file_name),
  scaling_coefficient(scaling_coefficient)
{}

std::string Speller::get_correct_spelling
(const SuggestionsInContext &s)
{
  LexicalModel::WeightStringVectorVector left_vector;
  for (StringVector::const_iterator it = s.left_context.begin();
       it != s.left_context.end();
       ++it)
    { left_vector.push_back(lexical_model[*it]); }
  HfstTransducer left_fst(get_fst(s.left_context,left_vector),
			  TROPICAL_OPENFST_TYPE);

  HfstTransducer center_fst(TROPICAL_OPENFST_TYPE);
  for (WeightedStringVector::const_iterator it = s.suggestions.begin();
       it != s.suggestions.end();
       ++it)
    {
      StringVector center_word(1,it->second);

      LexicalModel::WeightStringVectorVector
      center_word_vector(1,lexical_model[it->second]);

      HfstTransducer center_word_fst(get_fst(center_word,
					     center_word_vector),
				     TROPICAL_OPENFST_TYPE);      
      HfstTransducer eps(hfst::internal_epsilon,TROPICAL_OPENFST_TYPE);
      eps.set_final_weights(scaling_coefficient*it->first);
      center_word_fst.concatenate(eps).minimize();
      center_fst.disjunct(center_word_fst);
    }
  center_fst.minimize();

  LexicalModel::WeightStringVectorVector right_vector;
  for (StringVector::const_iterator it = s.right_context.begin();
       it != s.right_context.end();
       ++it)
    { right_vector.push_back(lexical_model[*it]); }
  HfstTransducer right_fst(get_fst(s.right_context,right_vector),
			   TROPICAL_OPENFST_TYPE);

  left_fst.concatenate(center_fst).concatenate(right_fst).minimize();

  HfstBasicTransducer sentence_with_lexical_weights(left_fst);

  HfstBasicTransducer sentence_with_weights = 
    sequence_model.tag_input(sentence_with_lexical_weights);

  StringPairVector result = get_vector(sentence_with_weights);

  StringPair suggestion_pair = result.at(left_vector.size());

  return suggestion_pair.first;
}


#ifdef HFST_TAGGER_TEST_Speller
#include <cassert>
#include <iostream>
int main(void)
{
  std::cerr << "Unit test for: " << __FILE__ << "\t";

  // Unit test begins.



  // Unit test ends.

  std::cerr << "PASSED" << std::endl;
}
#endif // HFST_TAGGER_TEST_Speller
