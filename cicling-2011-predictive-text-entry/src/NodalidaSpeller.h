#ifndef HFST_TAGGER_HEADER_NODALIDA_SPELLER_H
#define HFST_TAGGER_HEADER_NODALIDA_SPELLER_H

#include <vector>
#include <utility>
#include <string>

#include "NodalidaTagger.h"
#include "SuggestionsInContext.h"

class NodalidaSpeller : public NodalidaTagger
{
 public:
  NodalidaSpeller(const std::string &lexical_model_file_name,
		  const std::string &sequence_model_file_name,
		  float scaling_coefficient = 1.0);

  std::string get_correct_spelling(const SuggestionsInContext &s);

 protected:
  float scaling_coefficient;

};

#endif // HFST_TAGGER_HEADER_NODALIDA_SPELLER_H
