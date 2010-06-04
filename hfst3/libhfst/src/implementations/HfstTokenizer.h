#ifndef _HFST_TOKENIZER_H_
#define _HFST_TOKENIZER_H_
#include "SymbolDefs.h"
//#include "GlobalSymbolTable.h"
#include <iostream>
#include <climits>
#include <string>
namespace hfst
{
  using hfst::String;
  using hfst::StringSet;
  using hfst::StringPair;
  using hfst::StringPairVector;

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
  
  /** \brief A tokenizer for creating transducers from UTF-8 strings.

      An example:
\verbatim
      HfstTokenizer TOK;
      TOK.add_multichar_symbol("foo");
      TOK.add_multichar_symbol("bar");
      TOK.add_skip_symbol("fo");
      TOK.add_skip_symbol("ba");
      StringPairVector spv = tokenize("fofooo");
      // spv contains string pairs "foo:foo" and "o:o"
      HfstTransducer tr("fofooo", "babarr", TOK);
      // tr now contains the path [foo o]:[bar r]
\endverbatim

      @note The tokenizer only tokenizes utf-8 strings. */
  class HfstTokenizer
  {  
  private:
    MultiCharSymbolTrie multi_char_symbols;
    StringSet skip_symbol_set;
    int get_next_symbol_size(const char * symbol) const;
    bool is_skip_symbol(String &s) const;

  public:

    /** \brief Create a tokenizer that recognizes utf-8 symbols. */
    HfstTokenizer();
    /** \brief Add a symbol to be skipped to this tokenizer. */
    void add_skip_symbol(const std::string &symbol);
    /** \brief Add a multicharacter symbol \a symbol to this tokenizer. */
    void add_multichar_symbol(const std::string& symbol);
			      //KeyTable &key_table);

    /** \brief Tokenize the string \a input_string. */
    StringPairVector * tokenize(const std::string &input_string) const;
    //KeyTable &key_table) const;
    /** \brief Tokenize the string pair \a input_string : \a output_string. */
    StringPairVector * tokenize(const std::string &input_string,
				const std::string &output_string) const;
    //KeyTable &key_table) const;
    // Implement also a version that returns a StringSymbolPairVector (define this datatype)
    // and does not take a KeyTable argument
  };
}
#endif
