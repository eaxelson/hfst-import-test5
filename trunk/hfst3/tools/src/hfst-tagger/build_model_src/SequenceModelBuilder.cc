#include "SequenceModelBuilder.h"

#ifndef MAIN_TEST

using hfst::HfstOutputStream;
using hfst::TROPICAL_OPENFST_TYPE;

SequenceModelBuilder::SequenceModelBuilder(std::istream &in)
{
  while (1)
    {
      try
	{
	  ModelBuilder model_builder(WeightedStringVectorCollection(in,false));
	  
	  model_vector.push_back(model_builder.get_model());
	}
      catch (const EmptyFile &e)
	{ 
	  static_cast<void>(e); 
	  break;
	}
    }
}

void SequenceModelBuilder::store(const std::string &file_name)
{
  HfstOutputStream out(file_name,TROPICAL_OPENFST_TYPE);
  
  for (TransducerVector::iterator it = model_vector.begin();
       it != model_vector.end();
       ++it)
    { out << *it; }
}

void SequenceModelBuilder::store(void)
{
  HfstOutputStream out(TROPICAL_OPENFST_TYPE);
  
  for (TransducerVector::iterator it = model_vector.begin();
       it != model_vector.end();
       ++it)
    { out << *it; }
}

#else // MAIN_TEST

int main(void)
{

}

#endif // MAIN_TEST
