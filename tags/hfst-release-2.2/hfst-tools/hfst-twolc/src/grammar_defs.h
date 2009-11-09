#ifndef GRAMMAR_DEFS_H__
#define GRAMMAR_DEFS_H__
#include "../config.h"
#include <utility>
#include <vector>

typedef std::pair<char *, char*> Pair;
typedef std::vector<char *> NonDelimiterSymbolRange;
typedef std::vector<Pair*>  NonDelimiterRange;
typedef std::pair<char *,NonDelimiterSymbolRange*> Set;
#endif
