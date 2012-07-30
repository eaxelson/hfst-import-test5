#include <fstream>

#include "HMMTagger.h"

int main(int argc, char * argv[])
{
  if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " training_file  model_name" 
		<< std::endl;
      exit(1);
    }
  
  std::ifstream in(argv[1]);
  (void)in.peek();

  if (not in.good())
    {
      std::cerr << "Training file " << argv[1] << " can't be read." 
		<< std::endl;
      exit(1);
    }

  
  HMMTagger hmm_tagger(in,argv[2]);

  std::string file_name_prefix(argv[2]);

  hmm_tagger.store(file_name_prefix + ".lex",file_name_prefix + ".seq");
}
