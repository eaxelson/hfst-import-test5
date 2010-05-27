#ifndef _SYMBOL_DEFS_H_
#define _SYMBOL_DEFS_H_
#include <vector>
#include <string>
#include <map>
//#include <iostream>
//#include <sstream>
#include <set>
//#include "HfstExceptions.h"

namespace hfst
{
  /* A symbol in a transition. */
  typedef std::string String;
  /* A set of strings. */
  typedef std::set<String> StringSet;
  /** \brief A symbol pair in a transition. */
  typedef std::pair<std::string, std::string> StringPair;
  /** \brief A vector of transitions that represents a path in a transducer. */
  typedef std::vector<StringPair> StringPairVector;
  /* Maps strings to numbers. */
  typedef std::map<String,unsigned int> StringNumberMap;
  /* Maps numbers to numbers. */
  typedef std::map<unsigned int,unsigned int> NumberNumberMap;

  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2);
}
#endif
