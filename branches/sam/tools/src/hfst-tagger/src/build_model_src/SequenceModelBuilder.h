#ifndef HEADER_SEQUENCE_MODEL_BUILDER_H
#define HEADER_SEQUENCE_MODEL_BUILDER_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <vector>

#include "HfstTransducer.h"

using hfst::HfstTransducer;

#include "ModelBuilder.h"

class SequenceModelBuilder
{
 public:

  SequenceModelBuilder(std::istream &in);
  void store(const std::string &file_name);
  void store(void);
 private:

  typedef std::vector<HfstTransducer> TransducerVector;
  TransducerVector model_vector;
};

#endif // HEADER_SEQUENCE_MODEL_BUILDER_H
