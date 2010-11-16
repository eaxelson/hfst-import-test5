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

#ifndef _HFST_TOKENIZER_H_
#define _HFST_TOKENIZER_H_
#include "SymbolDefs.h"
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

      Strings are tokenized from left to right using longest match tokenization.
      For example, if the tokenizer contains a multicharacter symbol "foo" and a skip symbol "fo",
      the string "foo" is tokenized as "foo:foo".
      If the tokenizer contains a multicharacter symbol "fo" and a skip symbol "foo",
      the string "foo" is tokenized as an empty string (check this).

      An example:
\verbatim
      HfstTokenizer TOK;
      TOK.add_multichar_symbol("<br />");
      TOK.add_skip_symbol("<p>");
      TOK.add_skip_symbol("</p>");
      StringPairVector spv = TOK.tokenize("<p>A<br />paragraph!</p>");
      // spv now contains
      //    A:A <br />:<br /> p:p a:a r:r a:a g:g r:r a:a p:p h:h !:!
\endverbatim

      @note The tokenizer only tokenizes utf-8 strings. 
      <a href="classhfst_1_1HfstTransducer.html#symbols">Special symbols</a> 
      are not included in the tokenizer unless added to it.*/
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
    /** \brief Tokenize the string \a input_string. */
    StringPairVector * tokenize(const std::string &input_string) const;
    /** \brief Tokenize the string pair \a input_string : \a output_string. */
    StringPairVector * tokenize(const std::string &input_string,
				const std::string &output_string) const;

  };
}
#endif
