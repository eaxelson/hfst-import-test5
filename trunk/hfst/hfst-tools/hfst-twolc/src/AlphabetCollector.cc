#include "AlphabetCollector.h"
bool AlphabetCollector::is_set_name(char * name)
{
  return set_names.find(name) != set_names.end();
}

bool AlphabetCollector::is_any_symbol(char * name)
{
  return strcmp(name,any_symbol) == 0;
}

void AlphabetCollector::insert_symbol_pair(char * input,
					   char * output)
{
  SymbolPair pair(input,output);
  if (alphabet.find(pair) == alphabet.end())
    {
      alphabet.insert(pair);
    }
}

void AlphabetCollector::insert_set_construction(char * input,
						char * output)
{
  SymbolPair pair(input,output);
  if (set_constructions.find(pair) == set_constructions.end())
    {
      set_constructions[pair];
    }
}

void AlphabetCollector::insert_pair(const char * input,
				    const char * output)
{
  char * new_input = string_copy(input);
  char * new_output = string_copy(output);
  input = output = NULL;
  if (is_any_symbol(new_input) or
      is_any_symbol(new_output) or
      is_set_name(new_input) or
      is_set_name(new_output))
    {
      insert_set_construction(new_input,
			      new_output);
    }
  else
    {
      insert_symbol_pair(new_input,
			 new_output);
    }
}

void AlphabetCollector::define_set(char * set_name,SymbolSet * set_symbols)
{
  assert(set_names.find(set_name) == set_names.end());
  set_names.insert(string_copy(set_name));
  //set_elements[set_name].insert(set_symbols->begin(),
  //				set_symbols->end());
  for (SymbolSet::iterator it = set_symbols->begin();
       it != set_symbols->end();
       ++it)
    {
      if (is_set_name(*it))
	{
	  for (SymbolSet::iterator jt = set_elements[*it].begin();
	       jt != set_elements[*it].end();
	       ++jt)
	    {
	      set_elements[set_name].insert(string_copy(*jt));
	    }
	}
      else
	{
	  set_elements[set_name].insert(*it);
	}
    }
  delete set_symbols;
}

void AlphabetCollector::define_alphabet_pair(char * input,
					     char * output)
{
  assert(input != NULL);
  assert(output != NULL);
  assert(strcmp(input,"@?@") != 0);
  assert(strcmp(output,"@?@") != 0);
  assert(set_names.find(input) == set_names.end());
  assert(set_names.find(output) == set_names.end());
  if (set_elements["@?@"].find(input)
      == set_elements["@?@"].end())
    {
      set_elements["@?@"].insert(string_copy(input));
    }
  if (set_elements["@?@"].find(output)
      == set_elements["@?@"].end())
    {
      set_elements["@?@"].insert(string_copy(output));
    }
	
  insert_symbol_pair(input,output);
}

void AlphabetCollector::compute_pairs(SymbolSet &input_set,
				      SymbolSet &output_set,
				      SymbolPair &construction_pair)
{
  SymbolPairSet possible_pairs;
  for (SymbolSet::iterator it = input_set.begin();
       it != input_set.end();
       ++it)
    {
      for (SymbolSet::iterator jt = output_set.begin();
	   jt != output_set.end();
	   ++jt)
	{
	  SymbolPair pair(string_copy(*it),
			  string_copy(*jt));
	  if (alphabet.find(pair) != alphabet.end())
	    {
	      possible_pairs.insert(pair);
	    }
	}
    }
  set_constructions[construction_pair] = possible_pairs;
}

void AlphabetCollector::compute_set_constructions(void)
{
  for (GrammarSymbolPairSets::iterator it = set_constructions.begin();
       it != set_constructions.end();
       ++it)
    {
      SymbolPair construction_pair = it->first;
      SymbolSet input_set;
      if (set_names.find(construction_pair.first) == set_names.end()) 
	{
	  input_set.insert(construction_pair.first);
	}
      else
	{
	  input_set.insert(set_elements[construction_pair.first].begin(),
			   set_elements[construction_pair.first].end());
	}

      SymbolSet output_set;
      if (set_names.find(construction_pair.second) == set_names.end()) 
	{
	  output_set.insert(construction_pair.second);
	}
      else
	{
	  output_set.insert(set_elements[construction_pair.second].begin(),
			   set_elements[construction_pair.second].end());
	}
      compute_pairs(input_set,output_set,construction_pair);
    }
  set_constructions_ready = true;
}

SymbolPairSet &AlphabetCollector::get_alphabet(void)
{
  return alphabet;
}

bool AlphabetCollector::is_set_construction(char * input,
					    char * output)
{
  return 
    is_any_symbol(input) or
    is_any_symbol(output) or
    is_set_name(input) or
    is_set_name(output);
}

SymbolPairSet &AlphabetCollector::get_set_construction(char * input,
						       char * output)
{
  assert(set_constructions_ready);
  SymbolPair construction_pair(input,output);
  assert(set_constructions.find(construction_pair) !=
	 set_constructions.end());
  return set_constructions[construction_pair];
}
