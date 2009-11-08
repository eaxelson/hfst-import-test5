#include "rule_parts_tests.h"

void insert_symbols(SymbolMap &symbol_map,
		    const char ** string_symbols)
{
  for (const char ** sym = string_symbols;
       strcmp(*sym,"") != 0;
       ++sym)
    {
      char * symbol = string_copy(*sym);
      if (symbol_map.find(symbol) == symbol_map.end())
	{
	  symbol_map[symbol] = symbol_map.size();
	}
      else
	{
	  free(symbol);
	}
    }
}

RuleContextVector * make_context_vector(const char ** context_symbols,
					SymbolMap &symbol_map)
{
  RuleContextVector * context = new RuleContextVector;
  for (const char ** sym = context_symbols;
       strcmp(*sym,"") != 0;
       ++sym)
    {
      char * symbol = string_copy(*sym);
      context->push_back(symbol_map[symbol]);
      free(symbol);
    }
  return context;
}

bool RuleContextContainer_test(void)
{
  const char * context_symbols[] = 
    { "x", "_", "x", ";", "Cx", "Cy", "v", "" };

  SymbolMap symbols;
  insert_symbols(symbols,context_symbols);

  VariableValueMap variable_values;
  variable_values[symbols["Cx"]] = symbols["v"];
  variable_values[symbols["Cy"]] = symbols["x"];

  RuleContextContainer context_container(variable_values);
  
  {
  /* Test 0: Make replacements in string without variables. */
  const char * context_strings[] =
    { "x", "_", "x", ";", ""};
  RuleContextVector * context =
    make_context_vector(context_strings,
			symbols);
  context_container.insert_SymbolNumbers(context);
  delete context;

  RuleContextVector * no_variable_container =
    context_container.replace();

  const char * context_strings_check[] =
    { "x", "_", "x", ";", ""};
  RuleContextVector * no_variable_container_check =
    make_context_vector(context_strings_check,
			symbols);

  if (not equal(no_variable_container,
		no_variable_container_check))
    {
      std::cout << "RuleModifier_test,RuleContainer_test: Subtest 0 fails" << std::endl;
      return false;
    }
  delete no_variable_container;
  delete no_variable_container_check;
  context_container.reset();
  }

  /* Test 1: Make replacements in string with one variable. */
  {
  const char * context_strings[] =
    { "Cx", "_", "x", ";", ""};
  RuleContextVector * context =
    make_context_vector(context_strings,
			symbols);
  context_container.insert_SymbolNumbers(context);
  delete context;

  RuleContextVector * no_variable_container =
    context_container.replace();

  const char * context_strings_check[] =
    { "v", "_", "x", ";", ""};
  RuleContextVector * no_variable_container_check =
    make_context_vector(context_strings_check,
			symbols);

  if (not equal(no_variable_container,
		no_variable_container_check))
    {
      std::cout << "RuleModifier_test,RuleContainer_test: Subtest 1 fails" << std::endl;
      return false;
    }
  delete no_variable_container;
  delete no_variable_container_check;
  context_container.reset();
  }

  /* Test 2: Make replacements in string with two different variables. */
  {
  const char * context_strings[] =
    { "Cx", "_", "Cy", ";", ""};
  RuleContextVector * context =
    make_context_vector(context_strings,
			symbols);
  context_container.insert_SymbolNumbers(context);
  delete context;

  RuleContextVector * no_variable_container =
    context_container.replace();

  const char * context_strings_check[] =
    { "v", "_", "x", ";", ""};
  RuleContextVector * no_variable_container_check =
    make_context_vector(context_strings_check,
			symbols);

  if (not equal(no_variable_container,
		no_variable_container_check))
    {
      std::cout << "RuleModifier_test,RuleContainer_test: Subtest 2 fails" << std::endl;
      return false;
    }
  delete no_variable_container;
  delete no_variable_container_check;
  context_container.reset();
  }
  
  /* Test 3: Make replacements in string with two similar variables. */
    {
  const char * context_strings[] =
    { "Cx", "_", "Cx", ";", ""};
  RuleContextVector * context =
    make_context_vector(context_strings,
			symbols);
  context_container.insert_SymbolNumbers(context);
  delete context;

  RuleContextVector * no_variable_container =
    context_container.replace();

  const char * context_strings_check[] =
    { "v", "_", "v", ";", ""};
  RuleContextVector * no_variable_container_check =
    make_context_vector(context_strings_check,
			symbols);

  if (not equal(no_variable_container,
		no_variable_container_check))
    {
      std::cout << "RuleModifier_test,RuleContainer_test: Subtest 3 fails" << std::endl;
      return false;
    }
  
  delete no_variable_container;
  delete no_variable_container_check;
  context_container.reset();
    }
    
    for (SymbolMap::iterator it = symbols.begin();
	 it != symbols.end();
	 ++it)
      {
	free(it->first);
      }
    
    return true;
}

bool equal(RuleContextVector * v1,
	   RuleContextVector * v2)
{
  if (v1->size() != v2->size())
    {
      return false;
    }
  for (size_t index = 0;
       index < v1->size();
       ++index)
    {
      if (v1->at(index) != v2->at(index))
	{
	  return false;
	}
    }
  return true;
}

int main(void)
{
  if ((not RuleContextContainer_test()))
    {
      exit(1);
    }
  else
    {
      exit(0);
    }
}

