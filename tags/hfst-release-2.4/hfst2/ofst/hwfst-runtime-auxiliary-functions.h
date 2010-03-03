#ifndef HWFST_AUXILIARY_FUNCTIONS_H__
#define HWFST_AUXILIARY_FUNCTIONS_H__

#include "hfst.h"

typedef fst::StdArc::StateId StateId;
typedef fst::StdVectorFst TransduceR;

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef unsigned int TransitionNumber;
typedef unsigned int StateIdNumber;
typedef unsigned int ArcNumber;

bool check_finality(TransduceR * tr, StateId s);
bool is_deterministic_(TransduceR * tr);
bool is_minimised_(TransduceR * tr);

#endif
