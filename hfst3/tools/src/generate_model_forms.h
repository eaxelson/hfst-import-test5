#ifndef HEADER_GENERATE_MODEL_FORMS_H
#define HEADER_GENERATE_MODEL_FORMS_H

#include <vector>

#include "HfstTransducer.h"

using hfst::HfstTransducer;
using hfst::StringVector;
using hfst::HfstTokenizer;
using hfst::StringPair;
using hfst::HfstOneLevelPaths;

#define MODEL_FORM_PREFIX "ANALYSIS_SUFFIX="

#define MAX_ANALYSES 5
#define MAX_GENERATED_FORMS 1

typedef std::vector<StringVector> StringVectorVector;

struct InvalidModelLine
{
  InvalidModelLine(const std::string &line);
  std::string line;
};

struct InvalidModelFile
{};

// Return a tokenizer, which knows the multi character symbols in the
// alphabet of fst.
HfstTokenizer get_alphabet_string_tokenizer(HfstTransducer &fst);

// Return the prefix of reversed_analysis which precedes the first
// symbol of the form "[GUESS_CATHEGORY=...".
StringVector get_analysis_prefix(StringVector &analysis);

// Return a vector which contains all elemenrs of sv1 in order
// followed by all elements of sv2 in order.
StringVector join(StringVector sv1, const StringVector &sv2);

// Read a model form from stream @a in.
StringVector read_model_form(std::istream &in,
			     HfstTokenizer &tokenizer);

// Read a model form file with name @a model_form_filename.
StringVectorVector read_model_forms(const std::string &model_form_filename,
				    HfstTokenizer &tokenizer);

// Return a string vector which contains the tokenized word forms
//which can be generated from @a analysis using @a form_generator. The
//word forms are tokenized and spearated by ", " tokens.
StringVector generate_word_forms(const StringVector &analysis,
				 HfstTransducer &form_generator);

bool contains_analysis_symbols(const StringVector &word_form);

StringVectorVector get_guesses(const std::string &word_form,
			       HfstTransducer &guesser,
			       size_t number_of_guesses,
			       HfstTokenizer &tokenizer);

StringVectorVector get_model_forms(const StringVector &reversed_analysis,
				   const StringVectorVector &model_forms,
				   HfstTransducer &form_generator);

StringVectorVector get_paradigms(const std::string &word_form,
				 const StringVectorVector &guesses,
				 HfstTransducer &generator,
				 const StringVectorVector &model_forms,
				 size_t number_of_generated_forms);

std::ostream &operator<<(std::ostream &out,const StringVector &v);

#endif // HEADER_GENERATE_MODEL_FORMS_H

