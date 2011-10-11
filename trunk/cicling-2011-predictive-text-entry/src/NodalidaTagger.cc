#include "NodalidaTagger.h"

using hfst::TROPICAL_OPENFST_TYPE;
using hfst::HfstTwoLevelPaths;
using hfst::HfstTwoLevelPath;

ConfigStringPair nodalida_config_string_pair1("011101","010101");
ConfigStringPair nodalida_config_string_pair2("0111","0101");
ConfigStringPair nodalida_config_string_pair3("1101","0101");

ConfigStringPair nodalida_config_string_pair4("010101","010100");
ConfigStringPair nodalida_config_string_pair5("0101","0100");
ConfigStringPair nodalida_config_string_pair6("01","00");

ConfigStringPair config_string_pairs[6] =
  { nodalida_config_string_pair1, nodalida_config_string_pair2,
    nodalida_config_string_pair3, nodalida_config_string_pair4,
    nodalida_config_string_pair5, nodalida_config_string_pair6 };

ConfigStringPairVector hmm_config_string_pair_vector(config_string_pairs,
						     config_string_pairs + 6);

//float k = 1.4;
//float coeffs[3] = {k*0.28,k*0.34,k*0.38};
//float coeffs[3] = {k*0.48,k*0.24,k*0.28};

float k = 1;
float j = 0.2;

float coeffs[6] = {j*0.125,j*0.247,j*0.628,k*0.190,k*0.267,k*0.544};

FloatVector coefficients(coeffs,coeffs+6);

NodalidaTagger::NodalidaTagger(std::istream &in,const std::string &name):
  Tagger(in,name,hmm_config_string_pair_vector,coefficients)
{}

NodalidaTagger::NodalidaTagger(const std::string &lexical_model_file_name,
			       const std::string &sequence_model_file_name):
  Tagger(lexical_model_file_name,sequence_model_file_name)
{}

#ifdef HFST_TAGGER_TEST_NodalidaTagger
#include <cassert>
#include <iostream>
int main(void)
{
  std::cerr << "Unit test for: " << __FILE__ << "\t";

  // Unit test begins.



  // Unit test ends.

  std::cerr << "PASSED" << std::endl;
}
#endif // HFST_TAGGER_TEST_NodalidaTagger
