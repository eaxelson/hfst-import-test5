#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "HMMTagger.h"

#define MAXBUFFER 500000

using hfst::TROPICAL_OPENFST_TYPE;
using hfst::HfstTwoLevelPaths;
using hfst::HfstTwoLevelPath;
using hfst::StringPairVector;

HfstTokenizer tokenizer;

class CodingFailed
{};

StringVector pad_and_code_to_numbers
(const std::string &input, HfstTransducer &coder)
{
  HfstTransducer input_fst(input,tokenizer,TROPICAL_OPENFST_TYPE);
  HfstTwoLevelPaths output_paths;
  input_fst.compose(coder).output_project().extract_paths(output_paths);

  if (output_paths.size() != 1)
    { throw CodingFailed(); }

  StringPairVector v = output_paths.begin()->second;
  StringVector result;
  result.push_back("||");
  for (StringPairVector::const_iterator it = v.begin();
       it != v.end();
       ++it)
    { result.push_back(it->first); }
  result.push_back("||");
  return result;
}

int main(int argc, char * argv[])
{
  if (argc != 4)
    {
      std::cerr << "Usage: cat number_input | " << argv[0] 
		<< " letter_n_gram_lexical_model letter_n_gram_sequence_model"
		<< "txt_to_nr_fst"
		<< std::endl;
      exit(1);
    }

  HMMTagger letter_n_gram_model(argv[1],argv[2]);

  HfstInputStream coder_stream(argv[3]);
  HfstTransducer coder(coder_stream);

  char line[MAXBUFFER];
  while (std::cin.peek() != EOF)
    {
      std::cin.getline(line,MAXBUFFER);

      StringVector number_input = pad_and_code_to_numbers(line,coder);

      HfstBasicTransducer n_gram_weighted_input = 
	letter_n_gram_model.get_all_taggings(number_input);

      HfstTransducer n_gram_weighted_fst
	(n_gram_weighted_input,TROPICAL_OPENFST_TYPE);

      HfstTwoLevelPaths output_paths;
      n_gram_weighted_fst.input_project().minimize().n_best(5).minimize().extract_paths(output_paths);

      std::cout << line << ": ";
      
      for (HfstTwoLevelPaths::const_iterator it = output_paths.begin();
	   it != output_paths.end();
	   ++it)
	{
	  HfstTwoLevelPath output_path = *it;
      
	  for (StringPairVector::const_iterator jt = 
		 output_path.second.begin();
	       jt != output_path.second.end();
	       ++jt)
	    { 
	      if (jt == output_path.second.begin())
		{ jt += 2; }
	      if (jt + 2 == output_path.second.end())
		{ break; }
	      ++jt;
	      std::cout << jt->first; 
	    }
	  std::cout << " ";
	}
      std::cout << std::endl;
    }
}
