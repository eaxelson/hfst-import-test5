#ifndef VARIABLECOMPUTER_H__
#define VARIABLECOMPUTER_H__
#include "../config.h"
#include <set>
#include "VariableContainer.h"

enum Matcher 
  { MATCH, FREE, MIX };

typedef std::set<size_t> IndexSet;

class VariableComputer
{
 private:
  VariableValueMap &variable_values;
  VariableContainerVector variables;
  Matcher matcher;
  

 public:
  VariableComputer(VariableValueMap &values):
    variable_values(values),
    matcher(FREE)
    {};
  ~VariableComputer(void)
    { reset(); };
  void set_matcher(Matcher mm);
  void define_variable(SymbolNumber variable,
		       VariableValueVector &values);
  bool duplicate_indices(void);

  bool check_variables(void);
  void reset(void);
  bool last_combination(void);
  bool last_free_combination(void);
  bool end(void);
  void begin(void);
  void next(void);
};

#endif
