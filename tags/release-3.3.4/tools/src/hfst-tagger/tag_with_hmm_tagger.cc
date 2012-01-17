#include <fstream>

#include "HMMTagger.h"
#include "SentenceStream.h"

int main(int argc, char * argv[])
{
  if (argc != 3)
    {
      std::cerr << "Usage: cat input_string_file | " << argv[0] 
		<< " lexical_model_file_name sequence_mode_file_name" 
		<< std::endl;
      exit(1);
    }

  HMMTagger hmm_tagger(argv[1],argv[2]);

  SentenceStream sentence_stream(std::cin);

  std::cout << "||\t||" << std::endl;

  while (sentence_stream.is_good())
    {
      StringPairVector tagged_sentence = 
	hmm_tagger[sentence_stream.get_next_sentence()];
      
      std::cout << tagged_sentence << std::endl;
    }

  std::cout << "||\t||" << std::endl;
}
