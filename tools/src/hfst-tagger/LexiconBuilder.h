#ifndef HFST_TAGGER_HEADER_LEXICON_BUILDER_H
#define HFST_TAGGER_HEADER_LEXICON_BUILDER_H

#include "TrieBuilder.h"

class LexiconBuilder : public TrieBuilder
{
 public:
  LexiconBuilder(std::istream &in);
};

#endif // HFST_TAGGER_HEADER_LEXICON_BUILDER_H
