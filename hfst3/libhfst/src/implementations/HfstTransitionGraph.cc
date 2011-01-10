#include "HfstTransitionGraph.h"

namespace hfst { 
  namespace implementations {

    HfstNameThis::Number2SymbolMap HfstNameThis::number2symbol_map;
    Number2SymbolMapInitializer dummy1(HfstNameThis::number2symbol_map);
    
    HfstNameThis::Symbol2NumberMap HfstNameThis::symbol2number_map;
    Symbol2NumberMapInitializer dummy2(HfstNameThis::symbol2number_map);
    
    unsigned int HfstNameThis::max_number=2;

  }
}
