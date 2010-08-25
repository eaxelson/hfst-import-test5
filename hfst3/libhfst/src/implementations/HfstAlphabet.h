#ifndef _HFST_ALPHABET_H_
#define _HFST_ALPHABET_H_

#include <ext/hash_map>
#include <set>
#include <vector>
#include <string.h>
#include <stdio.h>

// header file would be better..
namespace hfst {
  namespace HfstBasic {
    char* fst_strdup(const char* pString);
  }
}

namespace hfst {
  namespace implementations {

    /* copied from SFST's alphabet.h|C */
    class HfstAlphabet {
      
    public:
      typedef std::pair<unsigned int,unsigned int> NumberPair;
      // hash table used to map the codes back to the symbols
      typedef __gnu_cxx::hash_map<unsigned int, char*> CharMap;      

    private:
      // string comparison operators needed to stored strings in a hash table
      struct eqstr {
	bool operator()(const char* s1, const char* s2) const {
	  return strcmp(s1, s2) == 0;
	}
      };
      
      // hash table used to map the symbols to their codes
      typedef __gnu_cxx::hash_map<const char*, unsigned int, __gnu_cxx::hash<const char*>,eqstr> SymbolMap;
      // set of symbol pairs
      typedef std::set<NumberPair> NumberPairSet;
      
      SymbolMap sm; // maps symbols to codes
      CharMap  cm; // maps codes to symbols
      
      
      // The set of string pairs
      NumberPairSet pairs;
      
    public:
      HfstAlphabet();
      ~HfstAlphabet();

      typedef NumberPairSet::const_iterator const_iterator;
      const_iterator begin() const;
      const_iterator end() const;
      size_t size() const;
      
      //bool contains_special_symbols(StringPair sp);  TODO!
      
      void insert(NumberPair sp);
      void clear_pairs();
      CharMap get_char_map();

      void add( const char *symbol, unsigned int c );
      void add_symbol( const char *symbol, unsigned int c );
      int symbol2code( const char * s ) const;
      const char *code2symbol( unsigned int c ) const;
      unsigned int add_symbol(const char * symbol);
      void complement( std::vector<unsigned int> &sym );
    };

  }
}

#endif
