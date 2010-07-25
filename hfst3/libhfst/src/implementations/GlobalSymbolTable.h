//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GLOBAL_SYMBOL_TABLE_H_
#define GLOBAL_SYMBOL_TABLE_H_
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include "SymbolDefs.h"
#include "HfstExceptions.h"

// Symbol has 4 or 8 bytes

namespace hfst { namespace symbols
{
  class GlobalSymbolTable
  {
  private:
    /* all strings */
    StringSymbolVector string_symbol_vector;
    /* string-to-number */
    StringSymbolMap string_symbol_map;
  public:
    /* 
       Define a GlobalSymbolTable with one symbol @0@. 
    */
    GlobalSymbolTable(void) { define_symbol("@0@"); } // @ANY@, @IDENTITY@
    
    /* Assign string_symbol the next unoccupied Symbol.*/
    Symbol define_symbol(const char * string_symbol);

    /* Return whether a Symbol has been assigned for string_symbol. */
    bool is_symbol(const char * string_symbol);
    
    /* Precondition is_symbol(string_symbol) */
    Symbol get_symbol(const char * string_symbol);
    
    /* Precondition there is a string_symbol associated with symbol. */
    const char * get_symbol_name(Symbol symbol);    

    void add_missing_symbols(KeyTable &source, KeyTable &target);
    void map_keys(KeyMap &key_map, 
		  KeyTable &source, KeyTable &target);
  };
} }
#endif
