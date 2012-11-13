#ifndef HEADER_MODEL_BUILDER_H
#define HEADER_MODEL_BUILDER_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <limits>
#include <string>

#include "FstBuilder.h"

#include "WeightedStringVectorCollection.h"

class ModelBuilder : public FstBuilder
{
 public:

  static bool verbose;
  static void verbose_printf(const std::string &message);

  ModelBuilder(const WeightedStringVectorCollection &model_weights);

 protected:

  void add_sequence(const WeightedStringVector &v, bool is_lexical_model);
};

#endif // HEADER_MODEL_BUILDER_H
