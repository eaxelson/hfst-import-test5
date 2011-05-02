#include "HfstTransitionGraph.h"

#ifndef MAIN_TEST

namespace hfst { 
  namespace implementations {

    HfstTropicalTransducerTransitionData::Number2SymbolMap 
    HfstTropicalTransducerTransitionData::number2symbol_map;
    Number2SymbolMapInitializer 
    dummy1(HfstTropicalTransducerTransitionData::number2symbol_map);
    
    HfstTropicalTransducerTransitionData::Symbol2NumberMap 
    HfstTropicalTransducerTransitionData::symbol2number_map;
    Symbol2NumberMapInitializer 
    dummy2(HfstTropicalTransducerTransitionData::symbol2number_map);
    
    unsigned int HfstTropicalTransducerTransitionData::max_number=2;

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
