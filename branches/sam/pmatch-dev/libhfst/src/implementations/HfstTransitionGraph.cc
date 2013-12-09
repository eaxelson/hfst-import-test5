#include "HfstTransitionGraph.h"

#ifndef MAIN_TEST

namespace hfst { 
  namespace implementations {


  }
}

#else // MAIN_TEST was defined
#include <iostream>

int main(int argc, char * argv[]) 
{
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
}

#endif // MAIN_TEST
