#include "RuleModifier_tests.h"

void remove_RuleString(RuleString * str)
{
  for(RuleString::iterator it = str->begin();
      it != str->end();
      ++it)
    {
      free(*it);
    }
  delete str;
}

void remove_RuleStrings(RuleStrings * strs)
{
  for(RuleStrings::iterator it = strs->begin();
      it != strs->end();
      ++it)
    {
      remove_RuleString(*it);
    }
  delete strs;
}

void display(RuleString * rule_string)
{
  for (RuleString::iterator it = rule_string->begin();
       it != rule_string->end();
       ++it)
    {
      std::cout << *it << " ";
    }
  std::cout << std::endl;
}
void display(RuleStrings &strings)
{
  for(RuleStrings::iterator it = strings.begin();
      it != strings.end();
      ++it)
    {
      display(*it);
    }
}

void display(RuleCenter &center)
{
  std::cout << center.first << ":" << center.second << " ";
}

void display(RuleRepresentationVector * representation_vector)
{
  for (RuleRepresentationVector::iterator it = representation_vector->begin();
       it != representation_vector->end();
       ++it)
    {
      RuleCenter center = (*it)->center;
      RuleStrings rule_strings = (*it)->contexts;
      display(center);
      std::cout << "OP" << std::endl;
      display(rule_strings);
    }
}

bool same_representations(RuleRepresentationVector * reps1, 
			  RuleRepresentationVector * reps2)
{
  if (reps1->size() != reps2->size())
    {
      return false;
    }
  for (size_t i = 0;
       i < reps1->size();
       ++i)
    {
      RuleRepresentation * rep1 = reps1->at(i);
      RuleRepresentation * rep2 = reps2->at(i);

      if (strcmp(rep1->center.first,rep2->center.first) != 0)
	{
	  return false;
	}
      if (strcmp(rep1->center.first,rep2->center.first) != 0)
	{
	  return false;
	}
      RuleStrings * strs1 = &(rep1->contexts);
      RuleStrings * strs2 = &(rep2->contexts);
      if (strs1->size() != strs2->size())
	{
	  std::cout << strs1->size() << " " << strs2->size() << " " << "error" << std::endl;

	  return false;
	}
      for(unsigned int i = 0;
	  i < strs1->size();
	  ++i)
	{
	  if (strs1->at(i)->size() != strs2->at(i)->size())
	    {
	      return false;
	    }
	  for(unsigned int j = 0;
	      j < strs1->at(i)->size();
	      ++j)
	    {
	      if (strcmp(strs1->at(i)->at(j),strs2->at(i)->at(j)) != 0)
		{
		  return false;
		}
	    }
	}
    }
  return true;
}

RuleString * read_symbols(const char ** symbols)
{
  RuleString * context = new RuleString;
  for (const char ** str = symbols;
       strcmp(*str,"") != 0;
       ++str)
    {
      const char * symbol = *str;
      context->push_back(string_copy(symbol));
    }
  return context;
}

bool RuleModifier_test(void)
{
  RuleModifier rule_modifier;
  SymbolNumber zero_number = rule_modifier.define_symbol("0");
  rule_modifier.define_symbol("0");
  if (zero_number != rule_modifier.get_symbol_number("0"))
    { 
      std::cout << "Test fails on line 136 " << std::endl;
      return false; 
    }
  SymbolNumber one_number = rule_modifier.define_symbol("1");
  if (zero_number != rule_modifier.get_symbol_number("0"))
    { 
      std::cout << "Test fails on line 136 " << std::endl;
      return false; 
    }
  if (zero_number+1 != one_number)
    { 
      std::cout << "Test fails on line 147 " << std::endl;
      return false; 
    }
  rule_modifier.define_symbol("a"); rule_modifier.define_symbol("b");
  rule_modifier.define_alphabet_symbol("a");   rule_modifier.define_alphabet_symbol("b");
  rule_modifier.define_alphabet_symbol("^I");  
  {
    RuleCenter rule_center;
    rule_center.first = "a";
    rule_center.second = "b";
    const char * context_rep[] = {"a", "^I", "_", "b", ";", ""};
    RuleString * context = read_symbols(context_rep);
    RuleString * empty_context = new RuleString;
    RuleStrings * contexts = new RuleStrings;
    contexts->push_back(context);
    contexts->push_back(empty_context);    
    RuleRepresentation representation; 
    representation.rule_name = string_copy("\"some rule\"");
    representation.center = rule_center;
    representation.contexts = *contexts;
    RuleRepresentationVector representations;
    representations.push_back(&representation);
    contexts->push_back(context);
    contexts->push_back(empty_context);
    rule_modifier.set_rule_name(string_copy("\"some rule\""));
    rule_modifier.set_rule_operator(RIGHT_OPERATOR);
    rule_modifier.set_center_input(string_copy("a"));
    rule_modifier.set_center_output(string_copy("b"));
    for (RuleString::iterator it = context->begin();
	 it != context->end();
	 ++it)
      {
	rule_modifier.add_symbol_to_context(*it);
      }
    RuleRepresentationVector * new_representations;
    try 
      {
	new_representations = rule_modifier.replace_variables();
      }
    catch (const char * str)
      {
	std::cerr << str << std::endl;
	exit(1);
      }
    if (not same_representations(&representations,new_representations))
      {
      std::cout << "Test fails on line 182 " << std::endl;
    	return false;
     }
    for (RuleRepresentationVector::iterator it = new_representations->begin();
	 it != new_representations->end();
	 ++it)
      {
	delete *it;
      }
    delete new_representations;
    rule_modifier.reset();
    for (RuleString::iterator it = context->begin();
	 it != context->end();
	 ++it)
      {
	free(*it);
      }    
      delete contexts;
  }
  {
    RuleCenter rule_center1;
    rule_center1.first = "b";
    rule_center1.second = "a";
    RuleCenter rule_center2;
    rule_center2.first = "a";
    rule_center2.second = "a";
    RuleCenter rule_center3;
    rule_center3.first = "b";
    rule_center3.second = "b";
    RuleCenter rule_center4;
    rule_center4.first = "a";
    rule_center4.second = "b";
    const char * context_rep[] = {"CX", "^I", "_", "CY", "CZ", ";", ""};
    const char * context_rep1[] = {"b", "^I", "_", "a", "a", ";", ""};
    const char * context_rep2[] = {"a", "^I", "_", "b", "a", ";", ""};
    const char * context_rep3[] = {"b", "^I", "_", "a", "b", ";", ""};
    const char * context_rep4[] = {"a", "^I", "_", "b", "b", ";", ""};
    RuleString * context = read_symbols(context_rep);
    RuleString * context1 = read_symbols(context_rep1);
    RuleString * context2 = read_symbols(context_rep2);
    RuleString * context3 = read_symbols(context_rep3);
    RuleString * context4 = read_symbols(context_rep4);
    RuleString * empty_context1 = new RuleString;
    RuleString * empty_context2 = new RuleString;
    RuleString * empty_context3 = new RuleString;
    RuleString * empty_context4 = new RuleString;
    RuleStrings * contexts = new RuleStrings;
    RuleStrings * contexts1 = new RuleStrings;
    RuleStrings * contexts2 = new RuleStrings;
    RuleStrings * contexts3 = new RuleStrings;
    RuleStrings * contexts4 = new RuleStrings;
    contexts1->push_back(context1);
    contexts2->push_back(context2);
    contexts3->push_back(context3);
    contexts4->push_back(context4);
    contexts1->push_back(empty_context1);    
    contexts2->push_back(empty_context2);    
    contexts3->push_back(empty_context3);    
    contexts4->push_back(empty_context4);    
    RuleRepresentation representation1;
    representation1.rule_name = string_copy("\"some rule 1\"");
    representation1.center = rule_center1;
    representation1.contexts = *contexts1;
    RuleRepresentation representation2;
    representation2.rule_name = string_copy("\"some rule 2\"");
    representation2.center = rule_center2;
    representation2.contexts = *contexts2;
    //{ rule_center2, *contexts2 };
    RuleRepresentation representation3;
    representation3.rule_name = string_copy("\"some rule 3\"");
    representation3.center = rule_center3;
    representation3.contexts = *contexts3;
    //{ rule_center3, *contexts3 };
    RuleRepresentation representation4; 
    representation4.rule_name = string_copy("\"some rule 4\"");
    representation4.center = rule_center4;
    representation4.contexts = *contexts4;
    //{ rule_center4, *contexts4 };
    RuleRepresentationVector representations;
    representations.push_back(&representation1);
    representations.push_back(&representation2);
    representations.push_back(&representation3);
    representations.push_back(&representation4);
    contexts->push_back(context);
    contexts->push_back(new RuleString);
    rule_modifier.set_rule_operator(RIGHT_OPERATOR);
    rule_modifier.set_rule_name(string_copy("\"some rule\""));
    rule_modifier.set_center_input(string_copy("CX"));
    rule_modifier.set_center_output(string_copy("CZ"));
    VariableValueVector values;
    rule_modifier.define_symbol("a");
    rule_modifier.define_symbol("b");
    for (RuleString::iterator it = context->begin();
	 it != context->end();
	 ++it)
      {
	rule_modifier.add_symbol_to_context(*it);
      }    
    values.push_back(rule_modifier.get_symbol_number("a"));
    values.push_back(rule_modifier.get_symbol_number("b"));
    RuleString * vals1 = new RuleString;
    vals1->push_back(string_copy("a"));
    vals1->push_back(string_copy("b"));
    RuleString * vals2 = new RuleString;
    vals2->push_back(string_copy("a"));
    vals2->push_back(string_copy("b"));
    RuleString * vals3 = new RuleString;
    vals3->push_back(string_copy("a"));
    vals3->push_back(string_copy("b"));
    rule_modifier.insert_variable(string_copy("CX"),
				  vals1);
    rule_modifier.insert_variable(string_copy("CY"),
				  vals2);
    rule_modifier.set_block_matcher(MIX);
    rule_modifier.new_block();
    rule_modifier.insert_variable(string_copy("CZ"),
				  vals3);
    RuleRepresentationVector * new_representations;
    try 
      {
	new_representations = rule_modifier.replace_variables();
      }
    catch (const char * str)
      {
	std::cerr << str << std::endl;
	return false;
      }
    if (not same_representations(&representations,new_representations))
      {
      std::cout << "Test fails on line 293 " << std::endl;
	return false;
      }
    for (RuleRepresentationVector::iterator it = new_representations->begin();
	 it != new_representations->end();
	 ++it)
      {
	delete *it;
	}
    delete new_representations;
   
    for (RuleStrings::iterator it = contexts->begin();
	 it != contexts->end();
	 ++it)
      {
	for (RuleString::iterator jt = (*it)->begin();
	     jt != (*it)->end();
	     ++jt)
	  {
	    free(*jt);
	  }
	delete *it;
	}
    for (RuleString::iterator it = context1->begin();
	 it != context1->end();
	 ++it)
      {
	free(*it);
      }
       for (RuleString::iterator it = context2->begin();
	 it != context2->end();
	 ++it)
      {
	free(*it);
      }
      
    for (RuleString::iterator it = context3->begin();
	 it != context3->end();
	 ++it)
      {
	free( *it);
      }
  
    for (RuleString::iterator it = context4->begin();
	 it != context4->end();
	 ++it)
      {
	free(*it);
	}

    delete contexts;
    delete contexts1;
    delete contexts2;
    delete contexts3;
    delete contexts4;
  }

  return true;
}

int main(void)
{
  if (RuleModifier_test())
    {
      exit(0);
    }
  exit(1);
}
