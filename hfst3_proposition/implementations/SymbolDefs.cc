#include "SymbolDefs.h"
#include "GlobalSymbolTable.h"

namespace hfst { namespace symbols
{
  KeyTable::KeyTable(void)
  { key_symbol_vector.push_back(0); symbol_key_map[0] = 0; }

  KeyTable::KeyTable(Symbol epsilon_symbol)
  { 
    key_symbol_vector.push_back(epsilon_symbol); 
    symbol_key_map[epsilon_symbol] = 0; 
  }

  KeyTable::KeyTable(const KeyTable &another):
    key_symbol_vector(another.key_symbol_vector),
    symbol_key_map(another.symbol_key_map)
  {}

  Key KeyTable::add_symbol(Symbol s)
  {
    if (symbol_key_map.find(s) != symbol_key_map.end())
      { return symbol_key_map[s]; }
    key_symbol_vector.push_back(s);
    symbol_key_map[s] = key_symbol_vector.size() - 1;
    return symbol_key_map[s];
  }

  bool KeyTable::is_symbol(Symbol s)
  { return symbol_key_map.find(s) != symbol_key_map.end(); } 

  bool KeyTable::is_key(Key k)
  { return k < key_symbol_vector.size(); }

  Key &KeyTable::get_key(Symbol s)
  { 
    if (not is_symbol(s))
      { throw SymbolNotDefinedException(); }
    return symbol_key_map[s];
  }

  const char * KeyTable::get_print_name(Key k)
  {
    if (not is_key(k))
      {
	ostringstream oss(ostringstream::out);
	oss << '\\' << k;
	return oss.str().c_str();
      }
    else
      { return get_string_symbol(k); }
  }

  KeyPair KeyTable::get_key_pair(const StringSymbolPair &p)
  { return KeyPair(add_symbol(p.first),
		   add_symbol(p.second)); }

  Key KeyTable::add_symbol(const char * string_symbol)
  {
    return add_symbol(global_symbol_table.define_symbol(string_symbol));
  }

  Key KeyTable::add_symbol(const std::string &string_symbol)
  {
    return add_symbol(string_symbol.c_str());
  }

  Key &KeyTable::get_key(const char * string_symbol)
  {
    try { return get_key(global_symbol_table.define_symbol(string_symbol)); }
    catch (HfstSymbolsException e) { throw e; }
  }

  Key KeyTable::number_of_keys(void)
  { return key_symbol_vector.size(); }

  const char * KeyTable::get_string_symbol(Key k)
  {
    try { 
      const char * str = global_symbol_table.get_symbol_name(get_symbol(k)); 
      std::string str_string(str);
      if (str_string == "\\")
	{ return "\\\\"; }
      if (str_string == " ")
	{ return "\\x20";}
      return str;
    }
    catch ( HfstSymbolsException e) { throw e; }
  }

  Symbol KeyTable::get_symbol(Key k)
  {
    if (not is_key(k))
      { throw KeyNotDefinedException(); }
    return key_symbol_vector.at(k); 
  }

  void KeyTable::harmonize(KeyMap &harmonizing_map,
			   KeyTable &another)
  {
    global_symbol_table.add_missing_keys
      (*this,another);
    global_symbol_table.map_keys(harmonizing_map,*this,another);	 
  }

  const char * KeyTable::operator[] (Key k)
  {
    return get_string_symbol(k);
  }
  Key &KeyTable::operator[] (const char * name)
  {
    return get_key(name); 
  }
  

  KeyTable::const_iterator KeyTable::begin(void) 
  { return const_iterator(symbol_key_map.begin()); }

  KeyTable::const_iterator KeyTable::end(void) 
  { return const_iterator(symbol_key_map.end());   }

  KeyTableConstIterator_::KeyTableConstIterator_
  (KeyTable::SymbolKeyMap::const_iterator it): it(it)
   {}

  KeyTableConstIterator_::KeySymbolPair * 
  KeyTableConstIterator_::operator->(void)
  { 
    ksp.key = it->second;
    ksp.symbol = it->first;
    return &ksp;
  }

  KeyTableConstIterator_::KeySymbolPair 
  KeyTableConstIterator_::operator*(void)
  { 
    ksp.key = it->second;
    ksp.symbol = it->first;
    return ksp;
  }
  bool KeyTableConstIterator_::operator==
  (const KeyTableConstIterator_ &another) const
    { return it == another.it; }

  void KeyTableConstIterator_::operator++ (void) { ++it; } 
  void KeyTableConstIterator_::operator++ (int) { ++it; }

  bool KeyTableConstIterator_::operator!= 
  (const KeyTableConstIterator_ &another) const 
  { return not (*this == another); }

  void KeyTableConstIterator_::operator= 
  (const KeyTableConstIterator_ &another)
  { 
    if (this == &another) { return; }
    it = another.it;
  }

  KeyMapper::KeyMapper(KeyTable &old_key_table,
		       KeyTable &new_key_table)
  { new_key_table.harmonize(key_map,old_key_table); }
  KeyMapper::const_iterator KeyMapper::begin(void) { return key_map.begin(); }
  KeyMapper::const_iterator KeyMapper::end(void) { return key_map.end(); }

} }
