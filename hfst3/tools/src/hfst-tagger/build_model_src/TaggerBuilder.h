#ifndef HEADER_TaggerBuilder_h
#define HEADER_TaggerBuilder_h

#include <fstream>
#include <string>

#include "LexicalModelBuilder.h"
#include "SequenceModelBuilder.h"

class TaggerBuilder
{
 public:
  TaggerBuilder(void);
  TaggerBuilder(const std::string &filename);
  void store(const std::string &file_name_prefix);
  void store(void);

 private:
  std::ifstream * in;
  LexicalModelBuilder lexical_model_builder;
  SequenceModelBuilder sequence_model_builder;
};

#endif // HEADER_TaggerBuilder_h
