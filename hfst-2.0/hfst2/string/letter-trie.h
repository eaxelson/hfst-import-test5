
#ifndef GUARD_HFST_letter_trie_h
#define GUARD_HFST_letter_trie_h

#include <vector>
#include "../hfst.h"

typedef std::vector<HFST::Key> SymbolNumberVector;
 
const HFST::Key NO_SYMBOL_NUMBER = USHRT_MAX;


namespace HFST
{

class LetterTrie;
typedef std::vector<LetterTrie*> LetterTrieVector;

class LetterTrie
{
public:
  LetterTrie(HFST::KeyTable* kt);

  Key add_string(const char * p);

  Key find_key(char ** p, bool addUnknown = true);
private:
  LetterTrieVector letters_;
  SymbolNumberVector symbols_;
  KeyTable* kt_;

  Key add_string_r(const char* p, Key sk);
  Key find_key_r(char** p);

};

}

#endif // GUARD_h
