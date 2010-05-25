#ifndef _SYMBOL_DEFS_H_
#define _SYMBOL_DEFS_H_
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <set>
#include "HfstExceptions.h"

namespace hfst
{
  /** \brief A namespace for symbol datatypes. */
namespace symbols
{
  using namespace hfst::exceptions;
  using std::ostringstream;

  /*
  typedef std::string StringSymbol;
  typedef std::vector<StringSymbol> StringSymbolVector;
  typedef StringSymbolVector::size_type Symbol;
  typedef unsigned short Key;
  typedef std::pair<Key,Key> KeyPair;
  typedef std::pair<StringSymbol,StringSymbol> StringSymbolPair;
  typedef std::map<StringSymbol,Symbol> StringSymbolMap;
  typedef std::set<String> StringSet;
  typedef std::map<Key,Key> KeyMap;
  typedef std::vector<KeyPair> KeyPairVector;
  //typedef std::set<StringSymbolPair> StringSymbolPairSet;
  typedef std::set<Symbol> SymbolSet;
  typedef std::pair<Symbol,Symbol> SymbolPair;
  typedef std::set<SymbolPair> SymbolPairSet;
  */

  /* A symbol in a transition. */
  typedef std::string String;
  /* A set of strings. */
  typedef std::set<String> StringSet;
  /** \brief A symbol pair in a transition. */
  typedef std::pair<std::string, std::string> StringPair;
  /** \brief A vector of transitions that represents a path in a transducer. */
  typedef std::vector<StringPair> StringPairVector;
  /* Maps strings to numbers. */
  typedef std::map<String,unsigned int> StringNumberMap;
  /* Maps numbers to numbers. */
  typedef std::map<unsigned int,unsigned int> NumberNumberMap;


  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2);

  //class KeyTableConstIterator_;

  //class GlobalSymbolTable;


  // new stuff begins...

  // ...new stuff ends


  /*
  class KeyTable
  {
  public:
    typedef std::vector<Key> KeySymbolVector;
    typedef std::map<Symbol,Key> SymbolKeyMap;
  protected:
    KeySymbolVector key_symbol_vector;
    SymbolKeyMap symbol_key_map;
  public:
    static GlobalSymbolTable global_symbol_table;
    KeyTable(void);    
    KeyTable(Symbol epsilon_symbol);
    KeyTable(const KeyTable &another);
    //KeyTable(struct sigma *s);
    //KeyTable(SFST::Alphabet &alpha);
    bool is_symbol(Symbol s);
    bool is_key(Key k);
    Key add_symbol(Symbol s);
    Key add_symbol(const char * string_symbol);
    Key add_symbol(const std::string &string_symbol);
    Key &get_key(Symbol s);
    Key &get_key(const char * string_symbol);
    Symbol get_symbol(Key k);
    KeyPair get_key_pair(const StringSymbolPair &p);
    const char * get_print_name(Key k);
    const char * get_string_symbol(Key k);
    const char * operator[] (Key k);
    Key &operator[] (const char * name);
    Key number_of_keys(void);
    void harmonize(KeyMap &harmonizing_map,
		   KeyTable &another);
    typedef KeyTableConstIterator_ const_iterator;
    const_iterator begin();
    const_iterator end();
    static void collect_unknown_sets(StringSymbolSet &s1, StringSymbolSet &unknown1,
				     StringSymbolSet &s2, StringSymbolSet &unknown2);
    // a set of non-identity symbol pairs from  S x S, where
    // S = the union of unknown_symbols and unknown_symbol_string and
    // x = cross-product operator
    static SymbolPairSet non_identity_cross_product(
      const SymbolSet &unknown_symbols,
      Symbol unknown_symbol_string);
  };

  class KeyTableConstIterator_
  {
  protected:
    KeyTable::SymbolKeyMap::const_iterator it;
  public:
    struct KeySymbolPair
    {
      Key key;
      Symbol symbol;
    };
    KeySymbolPair ksp;
    KeyTableConstIterator_(KeyTable::SymbolKeyMap::const_iterator it);
    KeySymbolPair * operator->(void);
    KeySymbolPair operator*(void);
    bool operator== (const KeyTableConstIterator_ &another) const ;
    bool operator!= (const KeyTableConstIterator_ &another) const ;
    void operator++ (void);
    void operator++ (int);
    void operator= (const KeyTableConstIterator_ &another);
  };
  typedef KeyTableConstIterator_::KeySymbolPair KeySymbolPair;

  class KeyMapper 
  {
  protected:
    KeyMap key_map;
  public:
    KeyMapper(KeyTable &old_key_table,
	      KeyTable &new_key_table);
    typedef KeyMap::const_iterator const_iterator;
    const_iterator begin(void);
    const_iterator end(void);
    };*/
} }
#endif
