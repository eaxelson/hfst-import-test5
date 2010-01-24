#ifndef GLOBAL_SYMBOL_TABLE_H_
#define GLOBAL_SYMBOL_TABLE_H_
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include "SymbolDefs.h"
#include "HfstExceptions.h"

namespace hfst { namespace symbols
{
  class GlobalSymbolTable
  {
  private:
    StringSymbolVector string_symbol_vector;
    StringSymbolMap string_symbol_map;
  public:
    /* 
       Define a GlobalSymbolTable with one symbol @0@. 
    */
    GlobalSymbolTable(void) { define_symbol("@0@"); }
    
    /* Assign string_symbol the next unoccupied Symbol.*/
    Symbol define_symbol(const char * string_symbol);

    /* Return whether a Symbol has been assigned for string_symbol. */
    bool is_symbol(const char * string_symbol);
    
    /* Precondition is_symbol(string_symbol) */
    Symbol get_symbol(const char * string_symbol);
    
    /* Precondition there is a string_symbol associated with symbol. */
    const char * get_symbol_name(Symbol symbol);    

    void add_missing_keys(KeyTable &source, KeyTable &target);
    void map_keys(KeyMap &key_map, 
		  KeyTable &source, KeyTable &target);
  };
} }
#endif
