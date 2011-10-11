#include "Sequencer.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
 
int main(int argc,char * argv[])
{
  if (argc != 3)
    { 
      std::cerr << "Usage: " << argv[0] << " input_file output_file"
		<< std::endl;
      exit(1);
    }
  
  std::ifstream in(argv[1]);

  Sequencer sequencer(in,"");

  sequencer.store(argv[2]);
}
