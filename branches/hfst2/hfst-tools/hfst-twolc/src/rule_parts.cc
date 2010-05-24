#include "rule_parts.h"


void RuleContextContainer::insert_SymbolNumber(SymbolNumber c)
{
  if (position + 1 > context.size())
    {
      context.push_back(c);
    }
  else
    {
      context.at(position) = c;
    }
  ++position;
}

void 
RuleContextContainer::insert_SymbolNumbers(RuleContextVector * symbol_numbers)
{
  for (RuleContextVector::iterator it = symbol_numbers->begin();
       it != symbol_numbers->end();
       ++it)
    {
      insert_SymbolNumber(*it);
    }
}

RuleContextVector * 
RuleContextContainer::replace(void)
{
  RuleContextVector * 
    context_without_variables = new RuleContextVector;
  
  for(RuleContextVector::iterator it = context.begin();
      it != context.begin() + position;
      ++it)
    {
      SymbolNumber c = *it;

      VariableValueMap::iterator v_it =
	variable_values.find(c);

      if (v_it != variable_values.end())
	{ 
	  context_without_variables->push_back(v_it->second);
	}
      else
	{
	  context_without_variables->push_back(c);
	}
    }
  return context_without_variables;
}

void RuleContextContainer::reset(void)
{
  position = 0;
}

size_t RuleContextContainer::get_position(void)
{
  return position;
}

void Center::reset(void)
{
  input_set = output_set = vector_set = false;
  center_pairs.clear();
}

CenterPair * Center::replace(void)
{
  CenterPair * center = new CenterPair;

  center->first =
    variable_values.find(input_char) != variable_values.end() ?
    variable_values[input_char] : input_char;

  center->second =
    variable_values.find(output_char) != variable_values.end() ?
    variable_values[output_char] : output_char;

  return center;
}

CenterPairVector * Center::replace_variables(void)
{
  CenterPairVector * center_pair_vector = new CenterPairVector;
  for (CenterPairVector::iterator it = center_pairs.begin();
       it != center_pairs.end();
       ++it)
    {
      center_pair_vector->push_back(*it);
      CenterPair &center_pair = center_pair_vector->back(); 

      center_pair.first =
	variable_values.find(center_pair.first) != 
	variable_values.end() ?
	variable_values[center_pair.first] : center_pair.first;

      center_pair.second =
	variable_values.find(center_pair.second) != 
	variable_values.end() ?
	variable_values[center_pair.second] : center_pair.second;
    }
  return center_pair_vector;
}

void Center::set_input(SymbolNumber new_input)
{
  input_set = true;
  input_char = new_input;
}

void Center::set_output(SymbolNumber new_output)
{
  output_set = true;
  output_char = new_output;
}
void Center::add_center_pair(SymbolNumber input,
			     SymbolNumber output)
{
  vector_set = true;
  center_pairs.push_back(CenterPair(input,output));
}

bool Center::is_set(void)
{
  return input_set and output_set;
}  

bool Center::vector_is_set(void)
{
  return vector_set;
}

RuleString * adjoin(RuleString * str1, RuleString * str2)
{
  RuleString * result_string = new RuleString;
  for (RuleString::iterator it = str1->begin();
       it != str1->end();
       ++it)
    {
      result_string->push_back(*it);
    }
  for (RuleString::iterator it = str2->begin();
       it != str2->end();
       ++it)
    {
      result_string->push_back(*it);
    }
  delete str1;
  delete str2;
  return result_string;
}

RuleString * make_power(RuleString * str, int exponent)
{
  assert(exponent >= 0);
  unsigned int exp = (unsigned int)(exponent);
  if (exponent == 0)
    {
      for (RuleString::iterator it = str->begin();
	   it != str->end();
	   ++it)
	{
	  free(*it);
	}
      str->clear();
      return str;
    }
  size_t str_len = str->size();
  for (size_t i = 1; i < exp; ++i)
    {
      for (size_t j = 0; j < str_len; ++j)
	{
	  str->push_back(string_copy(str->at(j)));
	}
    }
  return str;
}

RuleString * adjoin(RuleString * str, char * regex_operator)
{
  str->push_back(regex_operator);
  return str;
}

RuleString * adjoin(char * regex_operator, RuleString * str)
{
  str->insert(str->begin(),regex_operator);
  return str;
}

RuleString * adjoin(RuleString * str1, char * regex_operator, RuleString * str2)
{
  RuleString * result_string = new RuleString;
  for (RuleString::iterator it = str1->begin();
       it != str1->end();
       ++it)
    {
      result_string->push_back(*it);
    }
  result_string->push_back(regex_operator);
  for (RuleString::iterator it = str2->begin();
       it != str2->end();
       ++it)
    {
      result_string->push_back(*it);
    }
  delete str1;
  delete str2;
  return result_string;
}

RuleString * make_pair(char * symbol1, char * symbol2)
{
  RuleString * str = new RuleString;
  str->push_back(symbol1);
  str->push_back(string_copy(":"));
  if (strcmp(symbol1,"@#@") == 0)
    { str->push_back(string_copy("@0@")); free(symbol2); }
  else
    { str->push_back(symbol2); }
  return str;
}

RuleString * make_pair(char * symbol)
{
  RuleString * str = new RuleString;
  str->push_back(symbol);
  str->push_back(string_copy(":"));
  if (strcmp(symbol,"@#@") == 0)
    { str->push_back(string_copy("@0@")); }
  else
    { str->push_back(string_copy(symbol)); }
  return str;
}

RuleString * make_definition_pair(char * symbol)
{
  RuleString * str = new RuleString;
  str->push_back(symbol);
  return str;
}

RuleString * make_exponent(char * exponent_string)
{
  RuleString * exponent = new RuleString;
  exponent->push_back(exponent_string);
  return exponent;
}

RuleString * make_context(RuleString * regexp1, RuleString * regexp2)
{
  return adjoin(adjoin(regexp1,
		       string_copy("_"),
		       regexp2),string_copy(";"));
}

RuleString * make_empty_parenthesis(void)
{
  RuleString * expression = new RuleString;
  expression->push_back(string_copy("("));
  expression->push_back(string_copy(")"));
  return expression;
}

RuleString * make_empty_range(void)
{
  RuleString * expression = new RuleString;
  expression->push_back(string_copy("["));
  expression->push_back(string_copy("]"));
  return expression;
}

RuleString * make_parenthesis(RuleString * contents)
{
  contents->insert(contents->begin(),
		   string_copy("("));
  contents->push_back(string_copy(")"));
  return contents;
}

RuleString * make_range(RuleString * contents)
{
  contents->insert(contents->begin(),
		   string_copy("["));
  contents->push_back(string_copy("]"));
  return contents;
}

