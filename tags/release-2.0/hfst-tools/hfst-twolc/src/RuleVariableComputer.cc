#include "RuleVariableComputer.h"

void RuleVariableComputer::insert_variable(SymbolNumber variable,
					   VariableValueVector &vals)
{
  VariableComputer * last_computer =
    *(variable_blocks.rbegin());
  last_computer->define_variable(variable,
				 vals);
}

void RuleVariableComputer::set_block_matcher(Matcher m)
{
  VariableComputer * last_computer =
    *(variable_blocks.rbegin());
  last_computer->set_matcher(m);
}

void RuleVariableComputer::new_block(void)
{
  VariableComputer * computer = new VariableComputer(values);
  variable_blocks.push_back(computer);
}

void RuleVariableComputer::clear(void)
{
  for (VariableComputerVector::iterator it = variable_blocks.begin();
       it != variable_blocks.end();
       ++it)
    {
      delete *it;
    }
  variable_blocks.clear();
  new_block();

}

bool RuleVariableComputer::end(void)
{
  for (VariableComputerVector::iterator it = variable_blocks.begin();
       it != variable_blocks.end();
       ++it)
    {
      if(not (*it)->end())
	{
	  return false;
	}
    }
  return true;
}

void RuleVariableComputer::begin(void)
{
    for (VariableComputerVector::iterator it = variable_blocks.begin();
       it != variable_blocks.end();
       ++it)
    {
      (*it)->begin();
    }
}

bool RuleVariableComputer::last_combination(void)
{
  for(VariableComputerVector::iterator it = variable_blocks.begin();
      it != variable_blocks.end();
      ++it)
    {
      if (not (*it)->last_combination())
	{
	  return false;
	}
    }
  return true;
}

void RuleVariableComputer::next(void)
{
  bool last = last_combination();

  for (VariableComputerVector::iterator it = variable_blocks.begin();
       it != variable_blocks.end();
       ++it)
    {
      (*it)->next();
      if ((*it)->end())
	{	
	  if (not last)
	    (*it)->begin();
	}
      else
	{
	  break;
	}
    }
}

size_t RuleVariableComputer::number_of_blocks(void)
{
  return variable_blocks.size();
}
