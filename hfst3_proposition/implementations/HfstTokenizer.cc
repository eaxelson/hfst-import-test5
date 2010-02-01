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

  bool HfstTokenizer::is_skip_symbol(hfst::symbols::StringSymbol &s) const
{ return (s == "") or (skip_symbol_set.find(s) != skip_symbol_set.end()); }

void

HfstTokenizer::add_multichar_symbol(const string& symbol,KeyTable &key_table)
{ key_table.add_symbol(symbol.c_str());
  multi_char_symbols.add(symbol.c_str()); }

void
HfstTokenizer::add_skip_symbol(const std::string &symbol)
{ if (symbol == "")
    { return; }
  multi_char_symbols.add(symbol.c_str()); 
  skip_symbol_set.insert(symbol.c_str()); }

KeyPairVector * HfstTokenizer::tokenize
(const string& input_string,KeyTable &key_table) const
{
  KeyPairVector * kpv = new KeyPairVector;
  const char* s = input_string.c_str();
  while (*s)
    {
      int symbol_size = get_next_symbol_size(s);
      std::string symbol(s,0,symbol_size);
      s += symbol_size;
      if (is_skip_symbol(symbol))
	{ continue; }
      Key k = key_table.add_symbol(symbol);
      kpv->push_back(KeyPair(k,k));
    }
  return kpv;
}

KeyPairVector * HfstTokenizer::tokenize
(const string& input_string,const string& output_string,KeyTable &key_table)
const
{
  KeyPairVector * kpv = new KeyPairVector;
  
  KeyPairVector * input_kpv = tokenize(input_string.c_str(),key_table);
  KeyPairVector * output_kpv = tokenize(output_string.c_str(),key_table);

  if (input_kpv->size() < output_kpv->size())
    {
      KeyPairVector::iterator jt = output_kpv->begin();
      for (KeyPairVector::iterator it = input_kpv->begin();
	   it != input_kpv->end();
	   ++it)
	{ kpv->push_back(KeyPair(it->first,
				 jt->first));
	  ++jt; }
      for ( ; jt != output_kpv->end(); ++jt)
	{ kpv->push_back(KeyPair(0,jt->first)); }
    }
  else
    {
      KeyPairVector::iterator it = input_kpv->begin();
      for (KeyPairVector::iterator jt = output_kpv->begin();
	   jt != output_kpv->end();
	   ++jt)
	{ kpv->push_back(KeyPair(it->first,
				 jt->first));
	  ++it; }
      for ( ; it != input_kpv->end(); ++it)
	{ kpv->push_back(KeyPair(it->first,0)); }
    }
  delete input_kpv;
  delete output_kpv;
  return kpv;
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
  tokenizer.add_multi_char_symbol("sha");
  tokenizer.add_multi_char_symbol("kes");
  tokenizer.add_multi_char_symbol("pea");
  tokenizer.add_multi_char_symbol("ren");
  tokenizer.add_multi_char_symbol("näy");
  tokenizer.add_multi_char_symbol("tel");
  tokenizer.add_multi_char_symbol("mi");
  tokenizer.add_multi_char_symbol("en");
  tokenizer.add_multi_char_symbol("mi");
  tokenizer.add_multi_char_symbol("tal");
  tokenizer.add_multi_char_symbol("li");
  tokenizer.add_multi_char_symbol("set");
  tokenizer.add_multi_char_symbol("o");
  tokenizer.add_multi_char_symbol("sat");
  tokenizer.add_multi_char_symbol("vat");
  tokenizer.add_multi_char_symbol("ku");
  tokenizer.add_multi_char_symbol("ten");
  tokenizer.add_multi_char_symbol("jo");
  tokenizer.add_multi_char_symbol("ai");
  tokenizer.add_multi_char_symbol("em");
  tokenizer.add_multi_char_symbol("min");
  tokenizer.add_multi_char_symbol("to");
  tokenizer.add_multi_char_symbol("det");
  tokenizer.add_multi_char_symbol("tiin");
  tokenizer.add_multi_char_symbol("si");
  tokenizer.add_multi_char_symbol("lo");
  tokenizer.add_multi_char_symbol("sä");
  tokenizer.add_multi_char_symbol("et");
  tokenizer.add_multi_char_symbol("tä");
  tokenizer.add_multi_char_symbol("e");
  tokenizer.add_multi_char_symbol("li");
  tokenizer.add_multi_char_symbol("vii");
  tokenizer.add_multi_char_symbol("si");
  tokenizer.add_multi_char_symbol("pol");
  tokenizer.add_multi_char_symbol("vis");
  tokenizer.add_multi_char_symbol("ta");
  tokenizer.add_multi_char_symbol("jam");
  tokenizer.add_multi_char_symbol("bi");
  tokenizer.add_multi_char_symbol("a");
  tokenizer.add_multi_char_symbol("tar");
  tokenizer.add_multi_char_symbol("kas");
  tokenizer.add_multi_char_symbol("tel");
  tokenizer.add_multi_char_symbol("laan");
  tokenizer.add_multi_char_symbol("a");
  tokenizer.add_multi_char_symbol("luk");
  tokenizer.add_multi_char_symbol("si");
  tokenizer.add_multi_char_symbol("mil");
  tokenizer.add_multi_char_symbol("lai");
  tokenizer.add_multi_char_symbol("nen");
  tokenizer.add_multi_char_symbol("tä");
  tokenizer.add_multi_char_symbol("mä");
  tokenizer.add_multi_char_symbol("on");
  tokenizer.add_multi_char_symbol("ja");
  tokenizer.add_multi_char_symbol("mi");
  tokenizer.add_multi_char_symbol("ten");
  tokenizer.add_multi_char_symbol("si");
  tokenizer.add_multi_char_symbol("tä");
  tokenizer.add_multi_char_symbol("on");
  tokenizer.add_multi_char_symbol("suo");
  tokenizer.add_multi_char_symbol("ma");
  tokenizer.add_multi_char_symbol("lai");
  tokenizer.add_multi_char_symbol("ses");
  tokenizer.add_multi_char_symbol("sa");
  tokenizer.add_multi_char_symbol("ru");
  tokenizer.add_multi_char_symbol("nou");
  tokenizer.add_multi_char_symbol("des");
  tokenizer.add_multi_char_symbol("sa");
  tokenizer.add_multi_char_symbol("so");
  tokenizer.add_multi_char_symbol("vel");
  tokenizer.add_multi_char_symbol("let");
  tokenizer.add_multi_char_symbol("tu");
  tokenizer.add_skip_symbol("<br/>");
  HFST::KeyPairVector * kpv;
  char input[200];
  while (std::cin >> input)
    {
      if (strcmp(input,"lopeta") == 0)
	{ break; }
      kpv = tokenizer.tokenize(input,key_table);
      for (HFST::KeyPairVector::iterator it = kpv->begin();
	   it != kpv->end();
	   ++it)
	{ std::cout << key_table[it->first] << " "; }
      std::cout << std::endl;
      delete kpv;
    }
  std::cout << "paritesti" << std::endl;
  kpv = tokenizer.tokenize("älä<br/>","ko<br/>la<br/>",key_table);
      for (HFST::KeyPairVector::iterator it = kpv->begin();
	   it != kpv->end();
	   ++it)
	{ std::cout << key_table[it->first] << ":" 
		    << key_table[it->second] << " "; }
      std::cout << std::endl;
      delete kpv;
  const char * str1 = "äläkkä";
  const char * str2 = "kälä";
  hfst::KeyPairVector * kpv = tokenizer.tokenize(str1,str2,key_table);
  delete kpv;
}

#endif
