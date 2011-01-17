#include "HfstTransitionGraph.h"

namespace hfst { 
  namespace implementations {

    HfstTropicalTransducerTransitionData::Number2SymbolMap HfstTropicalTransducerTransitionData::number2symbol_map;
    Number2SymbolMapInitializer dummy1(HfstTropicalTransducerTransitionData::number2symbol_map);
    
    HfstTropicalTransducerTransitionData::Symbol2NumberMap HfstTropicalTransducerTransitionData::symbol2number_map;
    Symbol2NumberMapInitializer dummy2(HfstTropicalTransducerTransitionData::symbol2number_map);
    
    unsigned int HfstTropicalTransducerTransitionData::max_number=2;

  }
}
