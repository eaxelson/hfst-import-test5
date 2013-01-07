#include "SequenceModel.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
 
ConfigStringPair config_string_pair1("0101","0100");
ConfigStringPair config_string_pair2("01","00");

ConfigStringPair config_string_pairs[2] =
  {config_string_pair1, config_string_pair2};

ConfigStringPairVector config_string_pair_vector(config_string_pairs,
						 config_string_pairs + 2);

float coeffs[2] = {0.894701,0.105298};

FloatVector coefficients(coeffs,coeffs+2);

int main(int argc,char * argv[])
{
  if (argc != 3)
    { 
      std::cerr << "Usage: " << argv[0] << " input_file output_file"
		<< std::endl;
      exit(1);
    }

  std::ifstream in(argv[1]);

  SequenceModel sequence_model(config_string_pair_vector,in,coefficients);

  sequence_model.store(argv[2]);

}
