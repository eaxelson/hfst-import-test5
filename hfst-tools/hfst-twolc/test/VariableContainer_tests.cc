#include "VariableContainer_tests.h"

bool VariableContainer_test(void)
{
  SymbolNumber variable = 100;
  SymbolNumber value1 = 101;
  SymbolNumber value2 = 102;
  VariableValueVector values;
  values.push_back(value1);
  values.push_back(value2);
  VariableValueMap value_map;
  VariableContainer container(variable,values,value_map);
  container.set_value();
  if (container.size() != 2)
    { return false; }
  if (value_map[variable] != value1)
    { return false; }
  container.next();
  container.set_value();
  if (value_map[variable] != value2)
    { return false; }
  container.next();
  if (not container.end())
    { return false; }
  container.reset();
  if (container.size() != 2)
    { return false; }
  if (value_map[variable] != value1)
    { return false; }
  container.next();
  container.set_value();
  if (value_map[variable] != value2)
    { return false; }
  container.next();
  if (not container.end())
    { return false; }
  return true;
}

int main(void)
{
  if (VariableContainer_test())
    {
      exit(0);
    }
  exit(1);
}
