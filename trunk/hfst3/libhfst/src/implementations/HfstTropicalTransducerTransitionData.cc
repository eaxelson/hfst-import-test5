#include "HfstTropicalTransducerTransitionData.h"

namespace hfst {

  namespace implementations {

    HfstTropicalTransducerTransitionData::Number2SymbolVector
    HfstTropicalTransducerTransitionData::number2symbol_vector;
    Number2SymbolVectorInitializer 
    dummy1(HfstTropicalTransducerTransitionData::number2symbol_vector);
    
    HfstTropicalTransducerTransitionData::Symbol2NumberMap 
    HfstTropicalTransducerTransitionData::symbol2number_map;
    Symbol2NumberMapInitializer 
    dummy2(HfstTropicalTransducerTransitionData::symbol2number_map);
    
    unsigned int HfstTropicalTransducerTransitionData::max_number=2;

  }

}
