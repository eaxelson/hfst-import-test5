#include "GlobalSymbolTable.h"

namespace hfst { namespace symbols
{
  Symbol GlobalSymbolTable::define_symbol(const char * string_symbol)
  {
    assert(string_symbol != NULL);
    if (not is_symbol(string_symbol))
      {
	string_symbol_vector.push_back(string_symbol);
	string_symbol_map[string_symbol] =
	  string_symbol_vector.size() - 1;
      }
    return get_symbol(string_symbol);    
  }

  bool GlobalSymbolTable::is_symbol(const char * string_symbol)
  {
    assert(string_symbol != NULL);
    return string_symbol_map.find(string_symbol) != string_symbol_map.end();
  }

  Symbol GlobalSymbolTable::get_symbol(const char * string_symbol)
  {
    assert(string_symbol != NULL);
    if (string_symbol_map.find(string_symbol) == string_symbol_map.end())
      { throw SymbolNotDefinedException(); }
    return string_symbol_map[string_symbol];
  }

  const char * GlobalSymbolTable::get_symbol_name(Symbol symbol)
  {
    if (symbol >= string_symbol_vector.size())
      { throw SymbolNotDefinedException(); }
    return string_symbol_vector.at(symbol).c_str();
  }

  void GlobalSymbolTable::add_missing_symbols
  (KeyTable &source, KeyTable &target)
  {
    for (KeyTable::const_iterator it = source.begin(); 
	 it != source.end(); ++it)
      {
	if (not target.is_symbol(it->symbol))
	  { target.add_symbol(it->symbol); }
      }
  }

  void GlobalSymbolTable::map_keys
  (KeyMap &key_map, KeyTable &source, KeyTable &target)
  {
    for (KeyTable::const_iterator it = source.begin(); 
	 it != source.end(); ++it)
      {
	try { key_map[it->key] = target.get_key(it->symbol); }
	catch (HfstSymbolsException e) { throw e; }
      }
  }
} }

#ifdef DEBUG_MAIN
#include <iostream>
using namespace hfst::symbols;
GlobalSymbolTable KeyTable::global_symbol_table = GlobalSymbolTable();
GlobalSymbolTable &global_symbol_table = KeyTable::global_symbol_table;
int main(void) 
{
  KeyTable key_table1;
  key_table1.add_symbol("a");
  key_table1.add_symbol("b");
  assert(key_table1.number_of_keys() == 3);
  assert(key_table1.begin() != key_table1.end());
  for (KeyTable::const_iterator it = key_table1.begin(); 
       it != key_table1.end(); ++it)
    { std::cout << it->key << " " 
		<< key_table1.get_string_symbol(it->key) << std::endl; }

  KeyTable key_table2;
  key_table2.add_symbol("b");
  key_table2.add_symbol("a");
  assert(key_table2.number_of_keys() == 3);
  assert(key_table2.begin() != key_table1.end());
  for (KeyTable::const_iterator it = key_table2.begin(); 
       it != key_table2.end(); ++it)
    { std::cout << it->key << " " 
		<< key_table2.get_string_symbol(it->key) << std::endl; }
  KeyMapper key_mapper(key_table1,key_table2);

  for (KeyMapper::const_iterator it = key_mapper.begin(); 
       it != key_mapper.end(); ++it)
    { std::cout << it->first << " " << it->second << std::endl; }
}
#endif
