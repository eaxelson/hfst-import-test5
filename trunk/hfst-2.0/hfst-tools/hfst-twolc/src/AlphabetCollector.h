#ifndef _ALPHABET_COLLECTOR_H
#define _ALPHABET_COLLECTOR_H
#include "../config.h"
#include <utility>
#include <cstring>
#include <map>
#include <set>
#include "string_manipulation.h"

class SymbolPair
{
 public:
  char * first;
  char * second;
  SymbolPair(char * input,
	     char * output):
    first(input),
    second(output) 
    {};
  SymbolPair(const SymbolPair &other):
    first(string_copy(other.first)),
    second(string_copy(other.second)) 
    {};
  ~SymbolPair(void)
    {
      free(first);
      free(second);
    }
};

struct CmpSymbolPairs
{
  bool operator() (const SymbolPair &p1, 
		   const SymbolPair &p2)
  {
    if (strcmp(p1.first,p2.first) == 0)
      {
	return strcmp(p1.second,p2.second) < 0;
      }
    return strcmp(p1.first,p2.first) < 0;
  }
};

struct CmpNames
{
  bool operator() (const char * str1,
		   const char * str2) const
  {
    return strcmp(str1,str2) < 0;
  }
};

typedef std::set<SymbolPair,CmpSymbolPairs> 
  SymbolPairSet;
typedef std::set<char*,CmpNames> 
  NameSet;
typedef std::set<char*,CmpNames> 
  SymbolSet;
typedef std::map<char*,SymbolSet,CmpNames> 
  SetElementSet;
typedef std::map<SymbolPair,SymbolPairSet,CmpSymbolPairs> 
  GrammarSymbolPairSets;

class AlphabetCollector
{
  SymbolPairSet alphabet;
  NameSet set_names;
  SetElementSet set_elements;
  GrammarSymbolPairSets set_constructions;

  char * any_symbol;
  bool set_constructions_ready;

  bool is_set_name(char * name);
  bool is_any_symbol(char * symbol);

  void insert_symbol_pair(char * input,
			  char * output);
  void insert_set_construction(char * input,
			       char * output);
  void compute_pairs(SymbolSet &input_set,
		     SymbolSet &output_set,
		     SymbolPair &construction_pair);
 public:
  AlphabetCollector(void):
    any_symbol(string_copy("@?@")),
    set_constructions_ready(false)
    {
      set_names.insert(string_copy("@?@"));
      set_elements[string_copy("@?@")];
    };
  ~AlphabetCollector(void)
    {
      for (NameSet::iterator it = set_names.begin();
	   it != set_names.end();
	   ++it)
	{
	  free(*it);
	}
      for (SetElementSet::iterator it = set_elements.begin();
	   it != set_elements.end();
	   ++it)
	{
	  free(it->first);
	  for (SymbolSet::iterator jt = it->second.begin();
	       jt != it->second.end();
	       ++jt)
	    {
	      free(*jt);
	    }
	}
    }

  void insert_pair(const char * input_symbol,
		   const char * output_symbol);
  void define_set(char * set_name,SymbolSet * set_symbols);
  void define_alphabet_pair(char * input,
			    char * output);
  void compute_set_constructions(void);
  SymbolPairSet &get_alphabet(void);
  bool is_set_construction(char * input,
			   char * output);
  SymbolPairSet &get_set_construction(char * input,
				      char * output);
};

#endif
