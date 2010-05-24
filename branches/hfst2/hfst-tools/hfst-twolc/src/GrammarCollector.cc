#include "GrammarCollector.h"
void GrammarCollector::display(SymbolPairSet &alphabet)
{
  std::cout << "Alphabet" << std::endl;
  for (SymbolPairSet::iterator it = alphabet.begin();
       it != alphabet.end();
       ++it)
    {
      std::cout << it->first << ":" << it->second << std::endl;
    }
}

void GrammarCollector::insert_symbol(char * symbol)
{
  grammar_vector.push_back(symbol);
}

void GrammarCollector::insert_pair(char * input, char * output)
{
  insert_symbol(input);
  insert_symbol(string_copy(":"));
  insert_symbol(output);
}

void GrammarCollector::insert_rule_name(char * name)
{
  insert_symbol(name);
}

void GrammarCollector::insert_rule_declaration(void)
{
  insert_symbol(string_copy("Rules"));
}

void GrammarCollector::insert_star(void)
{
  insert_symbol(string_copy("*"));
}

void GrammarCollector::insert_plus(void)
{
  insert_symbol(string_copy("+"));
}

void GrammarCollector::insert_freely_insert(void)
{
  insert_symbol(string_copy("__HFST_TWOLC_FREELY_INSERT"));
}

void GrammarCollector::insert_complement(void)
{
  insert_symbol(string_copy("~"));
}

void GrammarCollector::insert_term_complement(void)
{
  insert_symbol(string_copy("\\"));
}

void GrammarCollector::insert_containment_once(void)
{
  insert_symbol(string_copy("$."));
}

void GrammarCollector::insert_containment(void)
{
  insert_symbol(string_copy("$"));
}

void GrammarCollector::insert_union(void)
{
  insert_symbol(string_copy("|"));
}

void GrammarCollector::insert_intersection(void)
{
  insert_symbol(string_copy("&"));
}

void GrammarCollector::insert_power(void)
{
  insert_symbol(string_copy("^"));
}

void GrammarCollector::insert_difference(void)
{
  insert_symbol(string_copy("-"));
}

void GrammarCollector::insert_left_square_bracket(void)
{
  insert_symbol(string_copy("["));
}

void GrammarCollector::insert_right_square_bracket(void)
{
  insert_symbol(string_copy("]"));
}

void GrammarCollector::insert_left_bracket(void)
{
  insert_symbol(string_copy("("));
}

void GrammarCollector::insert_right_bracket(void)
{
  insert_symbol(string_copy(")"));
}

void GrammarCollector::insert_left_restriction_arrow(void)
{
  insert_symbol(string_copy("/<="));
}

void GrammarCollector::insert_left_arrow(void)
{
  insert_symbol(string_copy("<="));
}

void GrammarCollector::insert_right_arrow(void)
{
  insert_symbol(string_copy("=>"));
}

void GrammarCollector::insert_left_right_arrow(void)
{
  insert_symbol(string_copy("<=>"));
}

void GrammarCollector::insert_eol(void)
{
  insert_symbol(string_copy(";"));
}

void GrammarCollector::insert_equals(void)
{
  insert_symbol(string_copy("="));
}

void GrammarCollector::insert_center_marker(void)
{
  insert_symbol(string_copy("_"));
}

bool GrammarCollector::is_first_symbol_in_pair(GrammarVector::iterator it)
{
  ++it;
  if (it == grammar_vector.end())
    {
      return false;
    }
  if (strcmp(*it,":") != 0)
    {
      return false;
    }
  ++it;
  assert(it != grammar_vector.end());
  return true;
}

void GrammarCollector::display_pairs(SymbolPairSet &symbol_pair_set)
{
  std::cout << ":[:" << std::endl;
  for (SymbolPairSet::iterator it = symbol_pair_set.begin();
       it != symbol_pair_set.end();
       ++it)
    {
      std::cout << it->first << ":" << it->second << std::endl;
    }
  std::cout << ":]:";
}

void GrammarCollector::display(void)
{
  display(alphabet_collector.get_alphabet());
  for (GrammarVector::iterator it = grammar_vector.begin();
       it != grammar_vector.end();
       ++it)
    {
      if (is_first_symbol_in_pair(it))
	{
	  if (alphabet_collector.is_set_construction(*it,*(it+2)))
	    {
	      display_pairs(alphabet_collector.get_set_construction(*it,
								    *(it+2)));
	      free(*(it+1));
	      ++(++it);
	    }
	  else
	    {
	      std::cout << *it;
	      free(*it);
	      ++it;
	      std::cout << *it;
	      free(*it);
	      ++it;
	      std::cout << *it;
	      free(*it);
	    }
	}
      else
	{
	  std::cout << *it;
	  free(*it);
	}
      std::cout << std::endl;
    }
}
