#include "GrammarDisplayer.h"

const char * HFST_TWOLC_ANY_SYMBOL = "@?@";
const char * HFST_TWOLC_EPSILON_SYMBOL = "@0@";
const char * UNION_OPERATOR = "|";
const char * INTERSECTION_OPERATOR = "&";
const char * DIFFERENCE_OPERATOR = "-";
const char * CONTAINMENT_OPERATOR = "$";
const char * CONTAINMENT_ONCE_OPERATOR = "$.";
const char * TERM_COMPLEMENT_OPERATOR = "\\";
const char * COMPLEMENT_OPERATOR = "~";
const char * STAR_OPERATOR = "*";
const char * PLUS_OPERATOR = "+";
const char * POWER_OPERATOR = "^";
const char * FREELY_INSERT_OPERATOR = "__HFST_TWOLC_FREELY_INSERT";

void GrammarDisplayer::display_alphabet_declaration(void)
{
  std::cout << "Alphabet" << std::endl;
}

void GrammarDisplayer::display_definitions_declaration(void)
{
  std::cout << "Definitions" << std::endl;
}

void GrammarDisplayer::display_diacritics_declaration(void)
{
  std::cout << "Diacritics" << std::endl;
}

void GrammarDisplayer::display_rules_declaration(void)
{
  std::cout << "Rules" << std::endl;
}

void GrammarDisplayer::display_sets_declaration(void)
{
  std::cout << "Sets" << std::endl;
}

void GrammarDisplayer::display_pair(Pair * p)
{
  std::cout << p->first << ":" << p->second << std::endl;
  free(p->first);
  free(p->second);
  delete p;
}


void GrammarDisplayer::display_end_of_line(void)
{
  std::cout << ";" << std::endl;
}

void GrammarDisplayer::display_alphabet(NonDelimiterRange * alphabet_range)
{
  for(NonDelimiterRange::iterator it = alphabet_range->begin();
      it != alphabet_range->end();
      ++it)
    {
      Pair * alphabet_pair = *it;
      display_pair(alphabet_pair);
    }
  delete alphabet_range;
}

void 
GrammarDisplayer::display_set_definition(Set * set)
{
  std::cout << set->first << " " << "=" << " ";
  free(set->first);
  for(NonDelimiterSymbolRange::iterator it = set->second->begin();
      it != set->second->end();
      ++it)
    {
      /*      if (strcmp(*it,";") == 0)
	{
	  std::cout << *it << std::endl;
	}
      else
	{
	  std::cout << *it << " ";
	  }*/
      std::cout << *it << std::endl;
      free(*it);
    }
  delete set->second;
  std::cout << ";" << std::endl;
  delete set;
}

void GrammarDisplayer::display_regexp(char * regexp_name,
				      RuleString * regexp)
{
  std::cout << regexp_name << " = ";
  display(regexp);
  std::cout << ";" << std::endl;
  for (RuleString::iterator it = regexp->begin();
       it != regexp->end();
       ++it)
    {
      free(*it);
    }
  free(regexp_name);
  free(regexp);
}

char * GrammarDisplayer::get_epsilon_symbol(void)
{
  return string_copy(HFST_TWOLC_EPSILON_SYMBOL);
}

char * GrammarDisplayer::get_any_symbol(void)
{
  return string_copy(HFST_TWOLC_ANY_SYMBOL);
}

char * GrammarDisplayer::get_union_operator(void)
{
  return string_copy(UNION_OPERATOR);
}

char * GrammarDisplayer::get_freely_insert_operator(void)
{
  return string_copy(FREELY_INSERT_OPERATOR);
}

char * GrammarDisplayer::get_intersection_operator(void)
{
  return string_copy(INTERSECTION_OPERATOR);
}
char * GrammarDisplayer::get_difference_operator(void)
{
  return string_copy(DIFFERENCE_OPERATOR);
}

char * GrammarDisplayer::get_containment_operator(void)
{
  return string_copy(CONTAINMENT_OPERATOR);
}

char * GrammarDisplayer::get_containment_once_operator(void)
{
  return string_copy(CONTAINMENT_ONCE_OPERATOR);
}

char * GrammarDisplayer::get_term_complement_operator(void)
{
  return string_copy(TERM_COMPLEMENT_OPERATOR);
}

char * GrammarDisplayer::get_complement_operator(void)
{
  return string_copy(COMPLEMENT_OPERATOR);
}

char * GrammarDisplayer::get_star_operator(void)
{
  return string_copy(STAR_OPERATOR);
}

char * GrammarDisplayer::get_plus_operator(void)
{
  return string_copy(PLUS_OPERATOR);
}

char * GrammarDisplayer::get_power_operator(void)
{
  return string_copy(POWER_OPERATOR);
}

bool is_separator(RuleString::iterator it)
{
  return strcmp(":",*it) == 0;
}

bool last(RuleString::iterator it, 
	  RuleString::iterator end)
{
  return (it+1) == end;
}


void GrammarDisplayer::display(RuleString * rule_string)
{
  for (RuleString::iterator it = rule_string->begin();
       it != rule_string->end();
       ++it)
    {
      if ((not last(it,rule_string->end())) and
	  is_separator(it+1))
	{
	  std::cout << *it;
	}
      else if (is_separator(it))
	{
	  std::cout << *it;
	}
      else
	{
	  std::cout << *it << std::endl;;
	}
    }
}

void GrammarDisplayer::display(RuleCenter &center)
{
  std::cout << center.first << ":" << center.second;
}

void GrammarDisplayer::display(Operator rule_operator)
{
  switch (rule_operator)
    {
    case LEFT_OPERATOR:
      std::cout << " <= ";
      break;
    case RIGHT_OPERATOR:
      std::cout << " => ";
      break;
    case NOT_LEFT_OPERATOR:
      std::cout << " /<= ";
      break;
    default:
      assert(false);
    }
}

void GrammarDisplayer::display(RuleRepresentation * rule_representation)
{
  if (rule_representation->rule_operator == LEFT_RIGHT_OPERATOR)
    {
      rule_representation->rule_operator = LEFT_OPERATOR;
      display(rule_representation);
      rule_representation->rule_operator = RIGHT_OPERATOR;
      display(rule_representation);
    }
  else
    {
      std::cout << "\"" << rule_representation->rule_name << "\"" << std::endl;
      if (rule_representation->center_pairs->empty())
	{
	  display(rule_representation->center);
	}
      else
	{
	  display(rule_representation->center_pairs);
	}
      display(rule_representation->rule_operator);
      display(rule_representation->contexts);
    }
}

void GrammarDisplayer::display(RuleStrings &rule_strings)
{
  for (RuleStrings::iterator it = rule_strings.begin();
       it != rule_strings.end();
       ++it)
    {
      display(*it);
    }
  std::cout << std::endl << std::endl;
}

void GrammarDisplayer::display(RuleRepresentationVector * rule_representations)
{
  for (RuleRepresentationVector::iterator it = rule_representations->begin();
       it != rule_representations->end();
       ++it)
    {
      display(*it);
      delete *it;
    }
  delete rule_representations;
}
