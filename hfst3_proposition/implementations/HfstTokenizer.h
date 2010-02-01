#ifndef _HFST_TOKENIZER_H_
#define _HFST_TOKENIZER_H_
#include "SymbolDefs.h"
#include "GlobalSymbolTable.h"
#include <iostream>
#include <climits>
#include <string>
namespace hfst
{
  using hfst::symbols::Key;
  using hfst::symbols::KeyPair;
  using hfst::symbols::KeyPairVector;
  using hfst::symbols::KeyTable;

  class MultiCharSymbolTrie;
  typedef std::vector<MultiCharSymbolTrie*> MultiCharSymbolTrieVector;
  typedef std::vector<bool> SymbolEndVector;

  class MultiCharSymbolTrie
  {
  private:
    MultiCharSymbolTrieVector symbol_rests;
    SymbolEndVector is_leaf;
    bool is_end_of_string(const char * p) const ;
    void set_symbol_end(const char * p);
    void init_symbol_rests(const char * p);
    void add_symbol_rest(const char * p);
    bool is_symbol_end(const char * p) const;
    MultiCharSymbolTrie * get_symbol_rest_trie(const char * p) const;

  public:
    MultiCharSymbolTrie(void);
    ~MultiCharSymbolTrie(void);
    void add(const char * p);
    const char * find(const char * p) const;  
  };
  
  /* At the moment, this only tokenizes into utf-8 strings. */
  class HfstTokenizer
  {  
  private:
    MultiCharSymbolTrie multi_char_symbols;
    hfst::symbols::StringSymbolSet skip_symbol_set;
    int get_next_symbol_size(const char * symbol) const;
    bool is_skip_symbol(hfst::symbols::StringSymbol &s) const;

  public:
    void add_skip_symbol(const std::string &symbol);
    void add_multichar_symbol(const std::string& symbol,
			      KeyTable &key_table);

    KeyPairVector * tokenize(const std::string &input_string,
			     KeyTable &key_table) const;
    KeyPairVector * tokenize(const std::string &input_string,
			     const std::string &output_string,
			     KeyTable &key_table) const;
  };
}
#endif
