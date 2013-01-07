#ifndef HFST_TAGGER_HEADER_NODALIDA_TAGGER_H
#define HFST_TAGGER_HEADER_NODALIDA_TAGGER_H

#include "Tagger.h"

class NodalidaTagger : public Tagger
{
 public:
  NodalidaTagger(std::istream &in,const std::string &name);
  NodalidaTagger(const std::string &lexical_model_file_name,
		 const std::string &sequence_model_file_name);
};

#endif // HFST_TAGGER_HEADER_NODALIDA_TAGGER_H
