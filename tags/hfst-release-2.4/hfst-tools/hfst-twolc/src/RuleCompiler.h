#ifndef _RULE_COMPILER_H_
#define _RULE_COMPILER_H_
#include "../config.h"
#include <hfst2/hfst.h>
#include "CompileUtilities.h"
#include "string_manipulation.h"
#include <iostream>
#include <map>
#include <vector>

template<class T> class RuleCompiler
{
 private:
  T center;
  T context;
  T universal;
  HFST::Key marker;
 public:
 RuleCompiler(T cen,T con,TwolCAlphabet &alphabet): 
  center(cen), context(con), 
  universal(T::universal(alphabet)), marker(alphabet.get_marker()) {};
 T operator() (void) 
 { 
   T result = center.subtract(context); 
   result = result.lose(marker);
   return universal.subtract(result);
 }
};

class Rule
{
 protected:
  HFST::Key input;
  HFST::Key output;
  char * name;

 public:
  static TwolCAlphabet alphabet;
  static bool verbose;
  static bool silent;
  static bool resolve_conflicts;
  static const char * zero_representation;
 Rule(HFST::Key input_key, HFST::Key output_key, char * rule_name):
  input(input_key), output(output_key), name(rule_name) {};
 ~Rule(void)
   {
     free(name);
   }   
 char * get_name_copy(void)
 {
   return string_copy(name);
 }
};

template<class T> 
struct CompiledRule
{
  typedef std::pair<char*,T> Pair;
  typedef std::vector<Pair*> Vector;
};


#endif
