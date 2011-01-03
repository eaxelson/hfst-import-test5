#include "HfstNet.h"

namespace hfst { 
  namespace implementations {

    TransitionData::Number2SymbolMap TransitionData::number2symbol_map;
    Number2SymbolMapInitializer dummy1(TransitionData::number2symbol_map);
    
    TransitionData::Symbol2NumberMap TransitionData::symbol2number_map;
    Symbol2NumberMapInitializer dummy2(TransitionData::symbol2number_map);
    
    unsigned int TransitionData::max_number=2;

  }
}
