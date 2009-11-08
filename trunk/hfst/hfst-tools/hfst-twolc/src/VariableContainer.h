
#ifndef VARIABLECONTAINER_H__
#define VARIABLECONTAINER_H__
#include "../config.h"
#include <cassert>
#include <map>
#include <vector>

typedef unsigned int SymbolNumber;
typedef std::map<SymbolNumber,SymbolNumber> VariableValueMap;
typedef std::vector<SymbolNumber> VariableValueVector;

class VariableContainer
{
 private:
  SymbolNumber variable;
  VariableValueVector values;
  VariableValueVector::iterator value_it; 
  VariableValueMap &variable_values;


 public:
  VariableContainer(SymbolNumber var,
		    VariableValueVector &var_values,
		    VariableValueMap &value_map):
    variable(var),
    values(var_values),
    value_it(values.begin()),
    variable_values(value_map) 
      { set_value(); };
  ~VariableContainer(void) {};
  void reset(void);
  size_t size(void);
  size_t index(void);
  void next(void);
  void set_value(void);
  bool last(void);
  bool nth_last(size_t offset);
  bool end(void);
};

typedef std::vector<VariableContainer*> VariableContainerVector;

#endif
