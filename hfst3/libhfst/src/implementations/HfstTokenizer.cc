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
  
int HfstTokenizer::get_next_symbol_size(const char * symbol)
const
{
  if (not *symbol)
    { return 0; }

  const char * multi_char_symbol_end = multi_char_symbols.find(symbol);  

  /* The string begins with a multi character symbol */
  if (multi_char_symbol_end != NULL)
    { return multi_char_symbol_end - symbol; }

  if ((0b10000000 & *symbol) == 0)
    { return 1; }
  if ((0b00100000 & *symbol) == 0)
    { return 2; }
  if ((0b00010000 & *symbol) == 0)
    { return 3; }
  return 4;
}

  bool HfstTokenizer::is_skip_symbol(hfst::String &s) const
{ return (s == "") or (skip_symbol_set.find(s) != skip_symbol_set.end()); }

void

HfstTokenizer::add_multichar_symbol(const string& symbol) // ,KeyTable &key_table)
{ //key_table.add_symbol(symbol.c_str());
  multi_char_symbols.add(symbol.c_str()); }

void
HfstTokenizer::add_skip_symbol(const std::string &symbol)
{ if (symbol == "")
    { return; }
  multi_char_symbols.add(symbol.c_str()); 
  skip_symbol_set.insert(symbol.c_str()); }

//KeyPairVector * 
StringPairVector * HfstTokenizer::tokenize
(const string& input_string) const // ,KeyTable &key_table) const
{
  StringPairVector * spv = new StringPairVector;
  const char* s = input_string.c_str();
  while (*s)
    {
      int symbol_size = get_next_symbol_size(s);
      std::string symbol(s,0,symbol_size);
      s += symbol_size;
      if (is_skip_symbol(symbol))
	{ continue; }
      //Key k = key_table.add_symbol(symbol);
      spv->push_back(StringPair(symbol,symbol));
    }
  return spv;
}

//KeyPairVector *
StringPairVector * HfstTokenizer::tokenize
(const string& input_string,const string& output_string) const // ,KeyTable &key_table)
{
  StringPairVector * spv = new StringPairVector;
  
  StringPairVector * input_spv = tokenize(input_string.c_str()); //,key_table);
  StringPairVector * output_spv = tokenize(output_string.c_str()); //,key_table);

  if (input_spv->size() < output_spv->size())
    {
      StringPairVector::iterator jt = output_spv->begin();
      for (StringPairVector::iterator it = input_spv->begin();
	   it != input_spv->end();
	   ++it)
	{ spv->push_back(StringPair(it->first,
				    jt->first));
	  ++jt; }
      for ( ; jt != output_spv->end(); ++jt)
	{ spv->push_back(StringPair("@_EPSILON_SYMBOL_@",jt->first)); }
    }
  else
    {
      StringPairVector::iterator it = input_spv->begin();
      for (StringPairVector::iterator jt = output_spv->begin();
	   jt != output_spv->end();
	   ++jt)
	{ spv->push_back(StringPair(it->first,
				    jt->first));
	  ++it; }
      for ( ; it != input_spv->end(); ++it)
	{ spv->push_back(StringPair(it->first,"@_EPSILON_SYMBOL_@")); }
    }
  delete input_spv;
  delete output_spv;
  return spv;
}

}

#ifdef DEBUG_MAIN_TOKENIZE
#include <iostream>
#include <cstring>
hfst::symbols::GlobalSymbolTable hfst::KeyTable::global_symbol_table;

int main(void)
{
  hfst::HfstTokenizer tokenizer;
  hfst::KeyTable key_table;
  tokenizer.add_multichar_symbol("sha",key_table);
  tokenizer.add_multichar_symbol("kes",key_table);
  tokenizer.add_multichar_symbol("pea",key_table);
  tokenizer.add_multichar_symbol("ren",key_table);
  tokenizer.add_multichar_symbol("näy",key_table);
  tokenizer.add_multichar_symbol("tel",key_table);
  tokenizer.add_multichar_symbol("mi",key_table);
  tokenizer.add_multichar_symbol("en",key_table);
  tokenizer.add_multichar_symbol("mi",key_table);
  tokenizer.add_multichar_symbol("tal",key_table);
  tokenizer.add_multichar_symbol("li",key_table);
  tokenizer.add_multichar_symbol("set",key_table);
  tokenizer.add_multichar_symbol("o",key_table);
  tokenizer.add_multichar_symbol("sat",key_table);
  tokenizer.add_multichar_symbol("vat",key_table);
  tokenizer.add_multichar_symbol("ku",key_table);
  tokenizer.add_multichar_symbol("ten",key_table);
  tokenizer.add_multichar_symbol("jo",key_table);
  tokenizer.add_multichar_symbol("ai",key_table);
  tokenizer.add_multichar_symbol("em",key_table);
  tokenizer.add_multichar_symbol("min",key_table);
  tokenizer.add_multichar_symbol("to",key_table);
  tokenizer.add_multichar_symbol("det",key_table);
  tokenizer.add_multichar_symbol("tiin",key_table);
  tokenizer.add_multichar_symbol("si",key_table);
  tokenizer.add_multichar_symbol("lo",key_table);
  tokenizer.add_multichar_symbol("sä",key_table);
  tokenizer.add_multichar_symbol("et",key_table);
  tokenizer.add_multichar_symbol("tä",key_table);
  tokenizer.add_multichar_symbol("e",key_table);
  tokenizer.add_multichar_symbol("li",key_table);
  tokenizer.add_multichar_symbol("vii",key_table);
  tokenizer.add_multichar_symbol("si",key_table);
  tokenizer.add_multichar_symbol("pol",key_table);
  tokenizer.add_multichar_symbol("vis",key_table);
  tokenizer.add_multichar_symbol("ta",key_table);
  tokenizer.add_multichar_symbol("jam",key_table);
  tokenizer.add_multichar_symbol("bi",key_table);
  tokenizer.add_multichar_symbol("a",key_table);
  tokenizer.add_multichar_symbol("tar",key_table);
  tokenizer.add_multichar_symbol("kas",key_table);
  tokenizer.add_multichar_symbol("tel",key_table);
  tokenizer.add_multichar_symbol("laan",key_table);
  tokenizer.add_multichar_symbol("a",key_table);
  tokenizer.add_multichar_symbol("luk",key_table);
  tokenizer.add_multichar_symbol("si",key_table);
  tokenizer.add_multichar_symbol("mil",key_table);
  tokenizer.add_multichar_symbol("lai",key_table);
  tokenizer.add_multichar_symbol("nen",key_table);
  tokenizer.add_multichar_symbol("tä",key_table);
  tokenizer.add_multichar_symbol("mä",key_table);
  tokenizer.add_multichar_symbol("on",key_table);
  tokenizer.add_multichar_symbol("ja",key_table);
  tokenizer.add_multichar_symbol("mi",key_table);
  tokenizer.add_multichar_symbol("ten",key_table);
  tokenizer.add_multichar_symbol("si",key_table);
  tokenizer.add_multichar_symbol("tä",key_table);
  tokenizer.add_multichar_symbol("on",key_table);
  tokenizer.add_multichar_symbol("suo",key_table);
  tokenizer.add_multichar_symbol("ma",key_table);
  tokenizer.add_multichar_symbol("lai",key_table);
  tokenizer.add_multichar_symbol("ses",key_table);
  tokenizer.add_multichar_symbol("sa",key_table);
  tokenizer.add_multichar_symbol("ru",key_table);
  tokenizer.add_multichar_symbol("nou",key_table);
  tokenizer.add_multichar_symbol("des",key_table);
  tokenizer.add_multichar_symbol("sa",key_table);
  tokenizer.add_multichar_symbol("so",key_table);
  tokenizer.add_multichar_symbol("vel",key_table);
  tokenizer.add_multichar_symbol("let",key_table);
  tokenizer.add_multichar_symbol("tu",key_table);
  tokenizer.add_skip_symbol("<br/>");
  hfst::KeyPairVector * kpv;
  char input[200];
  while (std::cin >> input)
    {
      if (strcmp(input,"lopeta") == 0)
	{ break; }
      kpv = tokenizer.tokenize(input,key_table);
      for (hfst::KeyPairVector::iterator it = kpv->begin();
	   it != kpv->end();
	   ++it)
	{ std::cout << key_table[it->first] << " "; }
      std::cout << std::endl;
      delete kpv;
    }
  std::cout << "paritesti" << std::endl;
  kpv = tokenizer.tokenize("älä<br/>","ko<br/>la<br/>",key_table);
      for (hfst::KeyPairVector::iterator it = kpv->begin();
	   it != kpv->end();
	   ++it)
	{ std::cout << key_table[it->first] << ":" 
		    << key_table[it->second] << " "; }
      std::cout << std::endl;
      delete kpv;
  const char * str1 = "äläkkä";
  const char * str2 = "kälä";
  kpv = tokenizer.tokenize(str1,str2,key_table);
  delete kpv;
}

#endif
