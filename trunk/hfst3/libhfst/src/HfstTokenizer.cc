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

#include "HfstTokenizer.h"
#include <string>

using std::string;
namespace hfst 
{
  bool MultiCharSymbolTrie::is_end_of_string(const char * p) const
  { return *(p+1) == 0; }

  void MultiCharSymbolTrie::set_symbol_end(const char * p)
  { is_leaf[(unsigned char)(*p)] = true; }

  bool MultiCharSymbolTrie::is_symbol_end(const char * p) const
  { return is_leaf[(unsigned char)(*p)]; }
  
  void MultiCharSymbolTrie::init_symbol_rests(const char * p)
  { 
    if (symbol_rests[(unsigned char)(*p)] == NULL)
      { symbol_rests[(unsigned char)(*p)] = new MultiCharSymbolTrie(); }
  }
  
  void MultiCharSymbolTrie::add_symbol_rest(const char * p)
  { symbol_rests[(unsigned char)(*p)]->add(p+1); }

  MultiCharSymbolTrie * MultiCharSymbolTrie::get_symbol_rest_trie
  (const char * p) const
  { return symbol_rests[(unsigned char)(*p)]; }
  
  MultiCharSymbolTrie::MultiCharSymbolTrie(void):
    symbol_rests(UCHAR_MAX, (MultiCharSymbolTrie*) NULL),
    is_leaf(UCHAR_MAX, false)
  {}

  MultiCharSymbolTrie::~MultiCharSymbolTrie(void)
  {
    for (MultiCharSymbolTrieVector::iterator it = symbol_rests.begin();
         it != symbol_rests.end();
         ++it)
      { delete *it; }
  }

  void MultiCharSymbolTrie::add(const char * p)
  {
    if (is_end_of_string(p))
      { set_symbol_end(p); }
    else
      { init_symbol_rests(p);
        add_symbol_rest(p); }
  }
  
  const char * MultiCharSymbolTrie::find(const char * p) const
  {
    MultiCharSymbolTrie * symbol_rest_trie = get_symbol_rest_trie(p);
    if (symbol_rest_trie == NULL)
      { 
        if (is_symbol_end(p))
          { return p+1; }
        return NULL; 
      }
    const char * symbol_end = symbol_rest_trie->find(p+1);
    if (symbol_end == NULL)
      { 
        if (is_symbol_end(p))
          { return p+1; }
      }
    return symbol_end;
  }
  
  HfstTokenizer::HfstTokenizer() {}


int HfstTokenizer::get_next_symbol_size(const char * symbol)
const
{
  if (not *symbol)
    { return 0; }

  const char * multi_char_symbol_end = multi_char_symbols.find(symbol);  

  /* The string begins with a multi character symbol */
  if (multi_char_symbol_end != NULL)
    { return multi_char_symbol_end - symbol; }
  if ((128 & *symbol) == 0)
    { return 1; }
  else if ((32 & *symbol) == 0)
    { return 2; }
  else if ((16 & *symbol) == 0)
    { return 3; }
  else
    {
      return 4;
    }
}

  bool HfstTokenizer::is_skip_symbol(hfst::String &s) const
{ return (s == "") or (skip_symbol_set.find(s) != skip_symbol_set.end()); }

void

HfstTokenizer::add_multichar_symbol(const string& symbol)
{  multi_char_symbols.add(symbol.c_str()); }

void
HfstTokenizer::add_skip_symbol(const std::string &symbol)
{ if (symbol == "")
    { return; }
  multi_char_symbols.add(symbol.c_str()); 
  skip_symbol_set.insert(symbol.c_str()); }

StringPairVector HfstTokenizer::tokenize
(const string& input_string) const
{
  StringPairVector spv;
  const char* s = input_string.c_str();
  while (*s)
    {
      int symbol_size = get_next_symbol_size(s);
      std::string symbol(s,0,symbol_size);
      s += symbol_size;
      if (is_skip_symbol(symbol))
        { continue; }
      spv.push_back(StringPair(symbol,symbol));
    }
  return spv;
}

StringVector HfstTokenizer::tokenize_one_level
(const string& input_string) const
{
  StringVector sv;
  const char* s = input_string.c_str();
  while (*s)
    {
      int symbol_size = get_next_symbol_size(s);
      std::string symbol(s,0,symbol_size);
      s += symbol_size;
      if (is_skip_symbol(symbol))
        { continue; }
      sv.push_back(symbol);
    }
  return sv;
}


StringPairVector HfstTokenizer::tokenize
(const string& input_string,const string& output_string) const
{
  StringPairVector spv;
  
  StringPairVector input_spv = tokenize(input_string.c_str());
  StringPairVector output_spv = tokenize(output_string.c_str());

  if (input_spv.size() < output_spv.size())
    {
      StringPairVector::iterator jt = output_spv.begin();
      for (StringPairVector::iterator it = input_spv.begin();
           it != input_spv.end();
           ++it)
        { spv.push_back(StringPair(it->first,
                                    jt->first));
          ++jt; }
      for ( ; jt != output_spv.end(); ++jt)
        { spv.push_back(StringPair("@_EPSILON_SYMBOL_@",jt->first)); }
    }
  else
    {
      StringPairVector::iterator it = input_spv.begin();
      for (StringPairVector::iterator jt = output_spv.begin();
           jt != output_spv.end();
           ++jt)
        { spv.push_back(StringPair(it->first,
                                   jt->first));
          ++it; }
      for ( ; it != input_spv.end(); ++it)
        { spv.push_back(StringPair(it->first,"@_EPSILON_SYMBOL_@")); }
    }
  return spv;
}

}
