#include "RuleModifier.h"

const char * RuleModifier::END_OF_CONTEXT = ";";

void RuleModifier::insert_symbol(char * symbol)
{
  assert(current_context < contexts.size()); 
  SymbolNumber symbol_number =
    define_symbol(symbol);
  contexts.at(current_context)->insert_SymbolNumber(symbol_number);
}

void RuleModifier::define_diacritic(char * diacritic)
{
  if (diacritics_set.find(diacritic) == diacritics_set.end())
    {
      diacritics_set.insert(string_copy(diacritic));
    }
}

void RuleModifier::define_diacritics(NonDelimiterSymbolRange * diacritics)
{
  for (NonDelimiterSymbolRange::iterator it = diacritics->begin();
       it != diacritics->end();
       ++it)
    {
      if (is_alphabet_symbol(*it))
	{
	  throw(generate_warning("The diacritic %s is already declared\n"
				 "as an alphabet symbol!",*it));
	}
      define_diacritic(*it);
      free(*it);
    }
  current_rule_diacritics = diacritics_set;
  delete diacritics;
}

void RuleModifier::define_alphabet_symbol(char * symbol)
{
  if (alphabet_set.find(symbol) == alphabet_set.end())
    {
      alphabet_set.insert(string_copy(symbol));
    }
  define_symbol(symbol);
}

void RuleModifier::define_alphabet(NonDelimiterRange * alphabet_range)
{
  for (NonDelimiterRange::iterator it = alphabet_range->begin();
       it != alphabet_range->end();
       ++it)
    {
      Pair * alphabet_pair = *it;
      define_alphabet_symbol(alphabet_pair->first);
      define_alphabet_symbol(alphabet_pair->second);
    }
}

bool RuleModifier::is_internal_symbol(char * symbol)
{
  return not (twolc_internal_symbols.find(symbol) ==
	      twolc_internal_symbols.end());
}

bool RuleModifier::is_diacritic(char * symbol)
{
  return not (diacritics_set.find(symbol) == diacritics_set.end());
}

bool RuleModifier::is_alphabet_symbol(char * symbol)
{
  return not (alphabet_set.find(symbol) == alphabet_set.end());
}

bool RuleModifier::is_set_name(char * symbol)
{
  return not (set_name_set.find(symbol) == set_name_set.end());
}

bool RuleModifier::is_definition_name(char * symbol)
{
  return not (definition_name_set.find(symbol) == definition_name_set.end());
}

bool RuleModifier::is_known_symbol(char * symbol)
{
  return is_internal_symbol(symbol) or is_alphabet_symbol(symbol) or is_diacritic(symbol) or is_set_name(symbol);
}
void RuleModifier::new_context(void)
{
  assert(current_context < contexts.size());

  if ((current_context + 1) == contexts.size())
    {
      contexts.push_back(new RuleContextContainer(variable_values));
    }
  ++current_context;
}

SymbolNumber RuleModifier::define_symbol(char * symbol)
{
  SymbolMap::iterator it = symbols.find(symbol);
  if (it == symbols.end())
    {
      SymbolNumber symbol_number = symbols.size();
      symbols[string_copy(symbol)] = symbol_number;
      symbol_numbers[symbol_number] = string_copy(symbol);
      return symbol_number;
    }
  return it->second;
}

void RuleModifier::define_set(char * set_name)
{
  if (is_alphabet_symbol(set_name))
    {
      throw generate_warning("Set name %s is an alphabet symbol!",
			     set_name);
    }
  if (is_set_name(set_name))
    {
      throw generate_warning("Redefining set %s!",
			     set_name);
    }
  set_name_set.insert(string_copy(set_name));
  define_symbol(set_name);
}

void RuleModifier::define_definition(char * definition_name,
				     RuleString * definition)
{
  if (is_alphabet_symbol(definition_name))
    {
      throw generate_warning("Definition name %s is an alphabet symbol!",
			     definition_name);
    }
  if (is_set_name(definition_name))
    {
      throw generate_warning("Definition name %s is a set name!",
			     definition_name);
    }
  if (is_definition_name(definition_name))
    {
      throw generate_warning("Remaking definition %s!",
			     definition_name);
    }
  definition_name_set.insert(string_copy(definition_name));
  define_symbol(definition_name);
  definitions[definition_name] = definition;
}

char * 
RuleModifier::generate_warning(const char * warning,const char * symbol)
{
  char * w = (char*)(malloc(strlen(warning)+strlen(symbol)));
  sprintf(w,warning,symbol);
  return w;
}

char * 
RuleModifier::generate_warning(const char * warning,const char * symbol1,
			       const char * symbol2)
{
  char * w = (char*)(malloc(strlen(warning)+strlen(symbol1)+strlen(symbol2)));
  sprintf(w,warning,symbol1,symbol2);
  return w;
}

void RuleModifier::check_symbols(NonDelimiterSymbolRange * symbol_range)
{
  for (NonDelimiterSymbolRange::iterator it = symbol_range->begin();
       it != symbol_range->end();
       ++it)
    {
      if (not is_known_symbol(*it))
	{
	  throw generate_warning("Symbol \"%s\" not declared in alphabet,\n"
				 "as a set name or as a definition!",*it);
	}
    }
}

void RuleModifier::add_set_symbols(char * set_name, 
				   NonDelimiterSymbolRange * range)
{
  SymbolNumber set_name_number = define_symbol(set_name);
  SymbolSetVector * range_numbers = new SymbolSetVector;
  for (NonDelimiterSymbolRange::iterator it = range->begin();
       it != range->end();
       ++it)
    {
      if (is_set_name(*it))
	{
	  SymbolNumber set_num = define_symbol(*it);
	  for (SymbolSetVector::iterator jt = 
		 symbol_sets[set_num]->begin(); 
	       jt !=
		 symbol_sets[set_num]->end(); 
	       ++jt)
	    {
	      range_numbers->push_back(*jt);
	    }
	}
      else
	{
	  range_numbers->push_back(define_symbol(*it));      
	}
    }
  symbol_sets[set_name_number] = range_numbers;
}

SymbolNumber RuleModifier::get_symbol_number(char * symbol)
{
  assert(symbols.find(symbol) != symbols.end());
  return symbols[symbol];
}

void RuleModifier::insert_definition(char * definition_name)
{
  assert(definitions.find(definition_name) != definitions.end());
  RuleString * definition = definitions[definition_name];

  add_symbol_to_context(HFST_TWOLC_LEFT_BRACKET);
  for (RuleString::iterator it = definition->begin();
       it != definition->end();
       ++it)
    {
      add_symbol_to_context(*it);
    }
  add_symbol_to_context(HFST_TWOLC_RIGHT_BRACKET);
}

void RuleModifier::add_symbol_to_context(char * symbol)
{
  current_rule_diacritics.erase(symbol);
  if (is_definition_name(symbol))
    {
      insert_definition(symbol);
      return;
    }
  else
    {
      insert_symbol(symbol);
    }

  if (not is_known_symbol(symbol))
    {
      SymbolNumber symbol_number = define_symbol(symbol);
      unknown_symbols.insert(symbol_number);
    }
  if (strcmp(symbol,END_OF_CONTEXT) == 0)
    {
      new_context();
    }
}

void RuleModifier::set_center_input(char * symbol)
{
  assert(symbol != NULL);
  SymbolNumber symbol_number = define_symbol(symbol);
  if (is_definition_name(symbol))
    {
      throw(generate_warning("The definition name %s can't be a symbol in a\n"
			     "center pair of a rule!",symbol));
    }
  else if (not is_known_symbol(symbol))
    {
      unknown_symbols.insert(symbol_number);
    }
  free(symbol);
  center.set_input(symbol_number);
}

void RuleModifier::set_center_output(char * symbol)
{
  assert(symbol != NULL);
  SymbolNumber symbol_number = define_symbol(symbol);
  if (is_definition_name(symbol))
    {
      throw(generate_warning("The definition name %s can't be a symbol in a\n"
			     "center pair of a rule!",symbol));
    }
  else if (not is_known_symbol(symbol))
    {
      unknown_symbols.insert(symbol_number);
    }
  free(symbol);
  center.set_output(symbol_number);
}

void RuleModifier::add_center_pair(char * input,
				   char * output)
{
  SymbolNumber input_symbol_number = define_symbol(input);
  SymbolNumber output_symbol_number = define_symbol(output);
  if (is_definition_name(input))
    {
      throw(generate_warning("The definition name %s can't be a symbol in a\n"
			     "center pair of a rule!",input));
    }
  if (is_definition_name(output))
    {
      throw(generate_warning("The definition name %s can't be a symbol in a\n"
			     "center pair of a rule!",output));
    }
  if (not is_known_symbol(input))
    {
      unknown_symbols.insert(input_symbol_number);
    }
  if (not is_known_symbol(output))
    {
      unknown_symbols.insert(output_symbol_number);
    }
  set_center_input(input);
  set_center_output(output);
  center.add_center_pair(input_symbol_number,
			 output_symbol_number);
}

void RuleModifier::add_rule_context(RuleString * rule_context)
{
  for (RuleString::iterator it = rule_context->begin();
       it != rule_context->end();
       ++it)
    {
      add_symbol_to_context(*it);
      free(*it);
    }
  delete rule_context;
}

void RuleModifier::add_rule_contexts(RuleStrings * rule_contexts)
{
  for (RuleStrings::iterator it = rule_contexts->begin();
       it != rule_contexts->end();
       ++it)
    {
      add_rule_context(*it);
    }
  delete rule_contexts;
}

void RuleModifier::reset(void)
{
  free(rule_name);
  rule_name = NULL;
  center.reset();
  rule_operator = NOT_DEFINED_OPERATOR;
  for (RuleContexts::iterator it = contexts.begin();
       it != contexts.end();
       ++it)
    {
      (*it)->reset();
    }
  current_context = 0;
  unknown_symbols.clear();
  variable_computer.clear();
  current_rule_diacritics = diacritics_set;
}

void RuleModifier::insert_variable(SymbolNumber variable,
				   VariableValueVector &values)
{
  variable_computer.insert_variable(variable,
				    values);
}

/* need to add checking that the variable has been used in tha rula and checking that values are defined symbols. */
void RuleModifier::insert_variable(char * variable,
				   NonDelimiterSymbolRange * values)
{
  if (is_known_symbol(variable) or is_definition_name(variable))
    {
      throw generate_warning("Variable %s is an alphabet symbol, set name or\n"
			     "definition!",variable);
    }
  SymbolNumber variable_number =
    define_symbol(variable);
  if (unknown_symbols.find(variable_number) == unknown_symbols.end())
    {
      throw generate_warning("Variable %s is declared in a WHERE clause,\n"
			     "but the corresponding rule has no variable %s!",
			     variable,variable);
    }
  unknown_symbols.erase(variable_number);
  VariableValueVector value_numbers;
  for (NonDelimiterSymbolRange::iterator it = values->begin();
       it != values->end();
       ++it)
    {
      if (is_alphabet_symbol(*it))
	{
	  SymbolNumber value_number = define_symbol(*it);
	  value_numbers.push_back(value_number);
	}
      else if (is_set_name(*it))
	{
	  SymbolNumber set_name_number = define_symbol(*it);
	  SymbolSetVector * set_elements = symbol_sets[set_name_number];
	  for (SymbolSetVector::iterator jt = set_elements->begin();
	       jt != set_elements->end();
	       ++jt)
	    {
	      value_numbers.push_back(*jt);
	    }
	}
      else
	{
	  throw generate_warning("Variable value %s is not an"
				 " alphabet symbol!",
				 *it);
	}
      free(*it);
    }
  free(variable);
  delete values;
  insert_variable(variable_number,
		  value_numbers);
}

void RuleModifier::set_block_matcher(unsigned int matcher)
{
  assert(matcher <= MIX);
  variable_computer.set_block_matcher((Matcher)matcher);
}

void RuleModifier::new_block(void)
{
  variable_computer.new_block();
}

RuleString * RuleModifier::add_diacritics(RuleString * context_string)
{
  assert(context_string != NULL);
  if (current_rule_diacritics.empty())
    {
      return context_string;
    }
  RuleString * new_context_string = new RuleString; 
  for (RuleString::iterator it = context_string->begin();
       it != context_string->end();
       ++it)
    {
      if ((strcmp("_",*it) == 0) or
	  (strcmp(";",*it) == 0))
	{
	  new_context_string->
	    push_back(HFST_TWOLC_FREELY_INSERT);
	  new_context_string->
	    push_back(HFST_TWOLC_LEFT_BRACKET);
	  for (AlphabetSet::iterator jt = current_rule_diacritics.begin();
	       jt != current_rule_diacritics.end();
	       ++jt)
	    {
	      new_context_string->push_back(*jt);
	      new_context_string->push_back(HFST_TWOLC_COLON);
	      new_context_string->
		push_back(HFST_TWOLC_EPSILON);
	    }
	  new_context_string->
	    push_back(HFST_TWOLC_RIGHT_BRACKET);
	}
      new_context_string->push_back(*it);
	
    }
  delete context_string;
  return new_context_string;
}

RuleRepresentationVector * RuleModifier::replace_variables(void)
{
  assert(rule_name != NULL);
  assert(center.is_set());
  assert(not contexts.empty());
  assert(rule_operator != NOT_DEFINED_OPERATOR);

  RuleRepresentationVector * representations = new RuleRepresentationVector;

  if (not variable_computer.end())  // If variable_computer.end(), there
    {                               // aren't any variables.
      for (variable_computer.begin();
	   not variable_computer.end();
	   variable_computer.next())
	{

	  RuleRepresentation * rule_representation = new RuleRepresentation;
	  rule_representation->rule_name = string_copy(rule_name);
	  representations->push_back(rule_representation);
	  CenterPair * center_pair = center.replace();
	  RuleCenter * rule_center = stringify(center_pair);
	  rule_representation->center = *rule_center; // Leaky possibly...
	  CenterPairVector * center_pairs = center.replace_variables();
	  RuleString * rule_center_pairs = stringify(center_pairs);
	  delete center_pairs;
	  rule_representation->center_pairs = rule_center_pairs;
	  rule_representation->rule_operator = rule_operator;
	  delete rule_center;
	  delete center_pair;


	  for (RuleContexts::iterator it = contexts.begin();
	       it != contexts.end();
	       ++it)
	    {
	      RuleContextVector * context_symbol_numbers = (*it)->replace();
	      RuleString * context_string = 
		stringify(context_symbol_numbers);
	      context_string = add_diacritics(context_string);
	      rule_representation->contexts.push_back(context_string);
	      delete context_symbol_numbers;
	    }
	  
	  if (not unknown_symbols.empty())
	    {
	      const char * unknown_symbol = 
		symbol_numbers[*(unknown_symbols.begin())];
	      throw generate_warning("The symbol %s is not an alphabet\n"
				     "symbol, set name or definition!",
				     unknown_symbol);
	    }
	}
    }
  else
    {
      RuleRepresentation * rule_representation = new RuleRepresentation;
      rule_representation->rule_name = string_copy(rule_name);
      representations->push_back(rule_representation);
      CenterPair * center_pair = center.replace();
      RuleCenter * rule_center = stringify(center_pair);
      rule_representation->center = *rule_center; // Leaky possibly
      CenterPairVector * center_pairs = center.replace_variables();
      RuleString * rule_center_pairs = stringify(center_pairs);
      delete center_pairs;
      rule_representation->center_pairs = rule_center_pairs;
      rule_representation->rule_operator = rule_operator;
      delete rule_center;
      delete center_pair;

      for (RuleContexts::iterator it = contexts.begin();
	   it != contexts.end();
	   ++it)
	{
	  RuleContextVector * context_symbol_numbers = (*it)->replace();
	  RuleString * context_string = stringify(context_symbol_numbers);
	  context_string = add_diacritics(context_string);
	  rule_representation->contexts.push_back(context_string);
	  delete context_symbol_numbers;
	}
      if (not unknown_symbols.empty())
	{
	  const char * unknown_symbol = 
	    symbol_numbers[*(unknown_symbols.begin())];
	  throw generate_warning("The symbol %s is not an alphabet\n"
				 "symbol, set name or definition!",
				 unknown_symbol);
	}
    }
  return representations;
}

/* Convert a SymbolNumberVector into a RuleString for displaying. */
RuleString * 
RuleModifier::stringify(RuleContextVector * symbol_number_vector)
{
  RuleString * symbol_vector = new RuleString;
  for (RuleContextVector::iterator it = symbol_number_vector->begin();
       it != symbol_number_vector->end();
       ++it)
    {
      assert(symbol_numbers.find(*it) != symbol_numbers.end());
      symbol_vector->push_back(symbol_numbers[*it]); // No copy made. 
                                                     // Shouldn't be a problem.
    }
  return symbol_vector;
}

RuleCenter * 
RuleModifier::stringify(CenterPair * center_pair)
{
  RuleCenter * rule_center = new RuleCenter;
  assert(symbol_numbers.find(center_pair->first) != symbol_numbers.end());
  assert(symbol_numbers.find(center_pair->second) != symbol_numbers.end());
  rule_center->first = symbol_numbers[center_pair->first];   // No copy made. Shouldn't be
  rule_center->second = symbol_numbers[center_pair->second]; // a problem.
  return rule_center;
}

RuleString * 
RuleModifier::stringify(CenterPairVector * center_pairs)
{
  RuleString * rule_string = new RuleString;
  for (CenterPairVector::iterator it = center_pairs->begin();
       it != center_pairs->end();
       ++it)
    {
      assert(symbol_numbers.find(it->first) != symbol_numbers.end());
      assert(symbol_numbers.find(it->second) != symbol_numbers.end());
      if (it != center_pairs->begin())
	{
	  rule_string->push_back(HFST_TWOLC_UNION);
	}
      rule_string->push_back(symbol_numbers[it->first]);
      rule_string->push_back(HFST_TWOLC_COLON);
      rule_string->push_back(symbol_numbers[it->second]);
    }
  return rule_string;
}

void RuleModifier::set_rule_operator(unsigned int op)
{
  assert(op <= NOT_LEFT_OPERATOR);
  
  rule_operator = (Operator)op;
}

void RuleModifier::set_rule_name(char * name)
{
  rule_name = name;
}

void RuleModifier::insert_internal_symbol(char * symbol)
{
  //define_symbol(symbol);
  twolc_internal_symbols.insert(symbol);
}

void RuleModifier::init_internal_symbols(void)
{
  insert_internal_symbol(string_copy("["));
  insert_internal_symbol(string_copy("]"));
  insert_internal_symbol(string_copy("("));
  insert_internal_symbol(string_copy(")"));
  insert_internal_symbol(string_copy("{"));
  insert_internal_symbol(string_copy("}"));
  insert_internal_symbol(string_copy(";"));
  insert_internal_symbol(string_copy("*"));
  insert_internal_symbol(string_copy("+"));
  insert_internal_symbol(string_copy("?"));
  insert_internal_symbol(string_copy("$"));
  insert_internal_symbol(string_copy("$."));
  insert_internal_symbol(string_copy("\\"));
  insert_internal_symbol(string_copy("&"));
  insert_internal_symbol(string_copy("|"));
  insert_internal_symbol(string_copy("_"));
  insert_internal_symbol(string_copy("-"));
  insert_internal_symbol(string_copy(":"));
  insert_internal_symbol(string_copy("@?@"));
  insert_internal_symbol(string_copy("@0@"));
  insert_internal_symbol(string_copy("__HFST_TWOLC_FREELY_INSERT"));
}

NonDelimiterRange * RuleModifier::get_diacritics(void)
{
  NonDelimiterRange * diacritics = new NonDelimiterRange;
  for (AlphabetSet::iterator it = diacritics_set.begin();
       it != diacritics_set.end();
       ++it)
    {
      diacritics->
	push_back(new Pair(string_copy(*it),
			   string_copy("@0@")));
    }
  return diacritics;
}
