#ifndef OPTIMIZE_LOOKUP_DEFS_H__
#define OPTIMIZE_LOOKUP_DEFS_H__

#include <vector>
#include <map>

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef unsigned int TransitionNumber;
typedef unsigned int StateIdNumber;
typedef unsigned int ArcNumber;

const StateIdNumber NO_ID_NUMBER = UINT_MAX;
const SymbolNumber NO_SYMBOL_NUMBER = USHRT_MAX;
const TransitionTableIndex NO_TABLE_INDEX = UINT_MAX;
const float INFINITE_WEIGHT = static_cast<float>(NO_TABLE_INDEX);


const SymbolNumber BIG_STATE_LIMIT = 1;

// This is 2^31
const TransitionTableIndex TRANSITION_TARGET_TABLE_START = 2147483648;

struct transition_index 
{
  SymbolNumber input_symbol;
  TransitionTableIndex target;
};
const transition_index EMPTY_TRANSITION_INDEX =
  { NO_SYMBOL_NUMBER, NO_TABLE_INDEX };


typedef std::vector<transition_index> IndexVector;

enum place_holder
  {
    EMPTY,
    EMPTY_START,
    OCCUPIED_START,
    OCCUPIED  
  };

typedef std::vector<place_holder> PlaceHolderVector;

#endif
