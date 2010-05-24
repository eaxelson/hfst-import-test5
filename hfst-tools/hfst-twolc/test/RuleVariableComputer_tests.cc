#include "RuleVariableComputer_tests.h"

void display(VariableValueMap &m)
{
  for(VariableValueMap::iterator it = m.begin();
      it != m.end();
      ++it)
    {
      std::cout << "(" << it->first << " " << it->second << ") " ;
    }
  std::cout << std::endl;
}

bool matches(TestVector * var_values1,
	      TestVector * var_values2)
{
  if (var_values1->size() != var_values2->size())
    {
      return false;
    }

  for(size_t i = 0;
      i < var_values1->size();
      ++i)
    {
      ValueVector * val1 = var_values1->at(i);
      ValueVector * val2 = var_values2->at(i);
      if (val1->size() != val2->size())
	{
	  return false;
	}
      for (size_t j = 0;
	   j < val1->size();
	   ++j)
	{
	  if (val1->at(j) != val2->at(j))
	    {
	      return false;
	    }
	} 
    }
  return true;
}

TestVector * vectorize(unsigned int data[][3], unsigned int rows, unsigned int cols)
{
  TestVector * test_v = new TestVector;
  for (unsigned int i = 0;
       i < rows;
       ++i)
    {
      ValueVector * val = new ValueVector;
      test_v->push_back(val);
      for (unsigned int j = 0;
	   j < cols;
	   ++j)
	{
	  val->push_back(data[i][j]);
	}
    }
  return test_v;
}

void destroy(TestVector * test_v)
{
  for (TestVector::iterator it = test_v->begin();
       it != test_v->end();
       ++it)
    {
      delete (*it);
    }
  delete test_v;
}

bool matches1(TestVector * var_values)
{
  unsigned int data[8][3] =
    {
      {2, 5, 8},
      {3, 5, 8},
      {2, 6, 8},
      {3, 6, 8},
      {2, 5, 9},
      {3, 5, 9},
      {2, 6, 9},
      {3, 6, 9}
    };
  TestVector * var_values_check = vectorize(data,8,3);
  bool match = matches(var_values,var_values_check);
  destroy(var_values_check);
  return match;
}

bool matches2(TestVector * var_values)
{
  unsigned int data[4][3] =
    {
      {0, 0, 0},
      {1, 1, 0},
      {0, 0, 1},
      {1, 1, 1}
    };
  TestVector * var_values_check = vectorize(data,4,3);
  bool match = matches(var_values,var_values_check);
  destroy(var_values_check);
  return match;
}

bool matches3(TestVector * var_values)
{
  unsigned int data[4][3] =
    {
      {1, 0, 0},
      {0, 1, 0},
      {1, 0, 1},
      {0, 1, 1}
    };
  TestVector * var_values_check = vectorize(data,4,3);
  bool match = matches(var_values,var_values_check);
  destroy(var_values_check);
  return match;
}

ValueVector * gather_values(VariableValueMap &m)
{
  ValueVector * v = new ValueVector;
  for (VariableValueMap::iterator it = m.begin();
       it != m.end();
       ++it)
    {
      v->push_back(it->second);
    }
  return v;
}

bool RuleVariableComputer_test(void)
{
  VariableValueMap m;
  RuleVariableComputer r(m);
  VariableValueVector v1;
  v1.push_back(2);
  v1.push_back(3);
  r.insert_variable(1,v1);
  r.set_block_matcher(MATCH);
  r.new_block();
  VariableValueVector v4;
  v4.push_back(5);
  v4.push_back(6);
  r.insert_variable(4,v4);
  r.new_block();
  r.set_block_matcher(MIX);
  VariableValueVector v7;
  v7.push_back(8);
  v7.push_back(9);
  r.insert_variable(7,v7);
  r.set_block_matcher(FREE);
  TestVector * test_v = new TestVector;
  for (r.begin(); not r.end(); r.next())
    {
      ValueVector * vals = gather_values(m);
      test_v->push_back(vals);
    }
  if (not matches1(test_v))
    {
      return false;
    }
  destroy(test_v);
  r.clear();
  r.insert_variable(1,v1);
  r.set_block_matcher(MATCH);
  r.new_block();
  r.insert_variable(4,v4);
  r.set_block_matcher(MATCH);
  r.new_block();
  r.insert_variable(7,v7);
  r.set_block_matcher(MATCH);
  test_v = new TestVector;
  for (r.begin(); not r.end(); r.next())
    {
      ValueVector * vals = gather_values(m);
      test_v->push_back(vals);
    }
  if (not matches1(test_v))
    {
      return false;
    }
  destroy(test_v);
  r.clear();
  m.clear();
  {
  SymbolNumber var0 = 100;
  SymbolNumber var1 = 101;
  SymbolNumber var2 = 102;
  VariableValueVector var0_values;
  var0_values.push_back(0);
  var0_values.push_back(1);
  VariableValueVector var1_values;
  var1_values.push_back(0);
  var1_values.push_back(1);
  VariableValueVector var2_values;
  var2_values.push_back(0);
  var2_values.push_back(1);
  r.insert_variable(var0,var0_values);
  r.insert_variable(var1,var1_values);
  r.set_block_matcher(MATCH);
  r.new_block();
  r.insert_variable(var2,var2_values);
  test_v = new TestVector;
  for (r.begin(); not r.end(); r.next())
    {
      ValueVector * vals = gather_values(m);
      test_v->push_back(vals);
    }
  if (not matches2(test_v))
    {
      return false;
    }
  destroy(test_v);
  }
  r.clear();
  m.clear();
  {
  SymbolNumber var0 = 100;
  SymbolNumber var1 = 101;
  SymbolNumber var2 = 102;
  VariableValueVector var0_values;
  var0_values.push_back(0);
  var0_values.push_back(1);
  VariableValueVector var1_values;
  var1_values.push_back(0);
  var1_values.push_back(1);
  VariableValueVector var2_values;
  var2_values.push_back(0);
  var2_values.push_back(1);
  r.insert_variable(var0,var0_values);
  r.insert_variable(var1,var1_values);
  r.set_block_matcher(MIX);
  r.new_block();
  r.insert_variable(var2,var2_values);
  r.set_block_matcher(FREE);
  test_v = new TestVector;
  for (r.begin(); 
       not r.end(); 
       r.next())
    {
      ValueVector * vals = gather_values(m);
      test_v->push_back(vals);
    }
  if (not matches3(test_v))
    {
      return false;
    }
  destroy(test_v);
  }
  return true;

}

int main(void)
{
  if (RuleVariableComputer_test())
    {
      exit(0);
    }
  exit(1);
}
