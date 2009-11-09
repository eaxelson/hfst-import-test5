#include "VariableComputer_tests.h"

bool VariableComputer_test(void)
{
  VariableValueMap m;
  VariableComputer vc(m);
  SymbolNumber var1 = 0;
  VariableValueVector values1;
  values1.push_back(1);
  values1.push_back(2);
  vc.define_variable(var1,values1);
  if (vc.end())
    {
      return false;
    }
  if (not vc.check_variables())
    {
      return false;
    }
  vc.set_matcher(MATCH);
  if (m[0] != 1)
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (m[0] != 2)
    {
      return false;
    }
  vc.next();
  if (not vc.end())
    {
      return false;
    }
  vc.begin();
  if (vc.end())
    {
      return false;
    }
  if (not vc.check_variables())
    {
      return false;
    }
  vc.set_matcher(MATCH);
  if (m[0] != 1)
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (m[0] != 2)
    {
      return false;
    }
  vc.next();
  if (not vc.end())
    {
      return false;
    }
  vc.reset();
  vc.define_variable(var1,values1);
  if (vc.end())
    {
      return false;
    }
  if (not vc.check_variables())
    {
      return false;
    }
  vc.set_matcher(MATCH);
  if (m[0] != 1)
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (m[0] != 2)
    {
      return false;
    }
  vc.next();
  if (not vc.end())
    {
      return false;
    }
  vc.reset();
  SymbolNumber var2 = 4;
  VariableValueVector values2;
  values2.push_back(1);
  values2.push_back(2);  
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  vc.set_matcher(MATCH);
  vc.check_variables();
  vc.begin();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 1) and (m[4] == 1)))
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 2) and (m[4] == 2)))
    {
      return false;
    }
  vc.next();
  if (not vc.end())
    {
      return false;
    }
  vc.reset();
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  vc.set_matcher(FREE);
  vc.check_variables();
  vc.begin();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 1) and (m[4] == 1)))
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 2) and (m[4] == 1)))
    {
      return false;
    }
  vc.next();  
  if (not ((m[0] == 1) and (m[4] == 2)))
    {
      return false;
    }
  vc.next();
  if (not ((m[0] == 2) and (m[4] == 2)))
    {
      return false;
    }
  vc.next();
  if (not vc.end())
    {
      return false;
    }
  vc.reset();
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  vc.set_matcher(MIX);
  vc.check_variables();
  vc.begin();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 2) and (m[4] == 1)))
    {
      return false;
    }
  vc.next();
  if (vc.end())
    {
      return false;
    }
  if (not ((m[0] == 1) and (m[4] == 2)))
    {
      return false;
    }
  vc.next();  
  if (not vc.end())
    {
      return false;
    }

  vc.reset();
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  SymbolNumber var3 = 5;
  VariableValueVector values3;
  values3.push_back(1);
  values3.push_back(2);
  vc.define_variable(var3,values3);
  vc.set_matcher(MIX);
  vc.check_variables();
  vc.begin();
  if (not vc.end())
    {
      return false;
    }
  vc.reset();
  values1.push_back(3);
  values2.push_back(3);
  values3.push_back(3);
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  vc.define_variable(var3,values3);
  vc.set_matcher(FREE);
  vc.check_variables();
  vc.begin();
  if (vc.end())
    {
      return false;
    }
  size_t count = 0;
  for (vc.begin(); not vc.end(); vc.next())
    {
      ++count;
    }
  if (count != 27)
    {
      return false;
    }
  vc.reset();
  vc.define_variable(var1,values1);
  vc.define_variable(var2,values2);
  vc.define_variable(var3,values3);
  vc.set_matcher(MIX);
  vc.check_variables();
  count = 0;
  for (vc.begin(); not vc.end(); vc.next())
    {
      ++count;
    }
  if (count != 6)
    {
      return false;
    }
  return true;
}

int main(void)
{
  if (VariableComputer_test())
    {
      exit(0);
    }
  exit(1);
}
