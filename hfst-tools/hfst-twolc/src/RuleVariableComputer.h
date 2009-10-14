#ifndef RULEVARIABLECOMPUTER_H__
#define RULEVARIABLECOMPUTER_H__
#include "../config.h"
#include <vector>
#include "VariableComputer.h"

typedef std::vector<VariableComputer*> VariableComputerVector;

class RuleVariableComputer
{
 private:
  VariableValueMap &values;
  VariableComputerVector variable_blocks;
 
 public:
  RuleVariableComputer(VariableValueMap &variable_values):
    values(variable_values)
    {
      variable_blocks.push_back(new VariableComputer(values));
    };
  ~RuleVariableComputer(void)
    {
      for(VariableComputerVector::iterator it = variable_blocks.begin();
	  it != variable_blocks.end();
	  ++it)
	{
	  delete *it;
	}
    };
  void insert_variable(SymbolNumber variable,
		       VariableValueVector &values);
  void set_block_matcher(Matcher m);  
  void new_block(void);
  void clear(void);
  bool end(void);
  void begin(void);
  void next(void);
  bool last_combination(void);
  size_t number_of_blocks(void);
};

#endif
