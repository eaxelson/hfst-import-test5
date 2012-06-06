#ifndef HEADER_WEIGHTED_STRING_VECTOR_COLLECTION_H
#define HEADER_WEIGHTED_STRING_VECTOR_COLLECTION_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>

#include "string_handling.h"

// Throw, if file end is reached unexpectedly.
struct EmptyFile
{};

class WeightedStringVectorCollection
{
 public:
  typedef std::vector<WeightedStringVector> WeightedStringVectorVector;
  typedef WeightedStringVectorVector::const_iterator const_iterator;
  typedef WeightedStringVectorVector::iterator iterator;

  const bool is_lexical_model;

  static size_t get_line_number(void);
  static size_t init_line_number(void);

  WeightedStringVectorCollection(std::istream &in, bool lexical_model);
  
  const_iterator begin(void) const;
  const_iterator end(void) const;

  iterator begin(void);
  iterator end(void);

  const std::string &get_name(void) const;

 private:
  static size_t line_number;

  HfstTokenizer tokenizer;
  WeightedStringVectorVector data;
  std::string name;

  std::string getline(std::istream &in);
};

#endif // HEADER_WEIGHTED_STRING_VECTOR_COLLECTION_H
