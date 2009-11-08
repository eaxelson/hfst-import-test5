/**
 * @file letter-trie.cc
 *
 * @brief implements string parsing with simple trie.
 */

#include "../hfst.h"
#include "w_letter-trie.h"
#include "string.h"

namespace HWFST
{


LetterTrie::LetterTrie(KeyTable* kt) :
	kt_(kt)
{
    letters_ = LetterTrieVector(UCHAR_MAX);
    symbols_ = SymbolNumberVector(UCHAR_MAX,NO_SYMBOL_NUMBER);
}

HWFST::Key
LetterTrie::add_string_r(const char * p, Key symbol_key)
{
  if (*(p+1) == '\0')
    {
      symbols_[(unsigned char)(*p)] = symbol_key;
      return symbol_key;
    }
  if (letters_[(unsigned char)(*p)] == NULL)
    {
      letters_[(unsigned char)(*p)] = new LetterTrie(kt_);
    }
  return letters_[(unsigned char)(*p)]->add_string_r(p+1,symbol_key);
}

HWFST::Key
LetterTrie::add_string(const char* p)
{
  return add_string_r(p, stringToKey(p, kt_, true));
}

Key
LetterTrie::find_key_r(char** p)
{
  const char * old_p = *p;
  ++(*p);
  if (letters_[(unsigned char)(*old_p)] == NULL)
    {
      return symbols_[(unsigned char)(*old_p)];
    }
  Key s = letters_[(unsigned char)(*old_p)]->find_key_r(p);
  if (s == NO_SYMBOL_NUMBER)
    {
      --(*p);
      return symbols_[(unsigned char)(*old_p)];
    }
  return s;
}

HWFST::Key
LetterTrie::find_key(char ** p, bool addUnknown)
{
  char* old_p = *p;
  HWFST::Key s = find_key_r(p);
  if ((s == NO_SYMBOL_NUMBER) && addUnknown)
  {
    *p = old_p;
    unsigned short u8len = 0;
	unsigned char c = static_cast<unsigned char>(**p);
	if (c <= 127)
	{
	  u8len = 1;
	}
	else if ( (c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) )
	{
	  u8len = 4;
	}
	else if ( (c & (128 + 64 + 32 )) == (128 + 64 + 32) )
	{
	  u8len = 3;
	}
	else if ( (c & (128 + 64 )) == (128 + 64))
	{
	  u8len = 2;
	}
	else
	{
	  fprintf(stderr, "%s not valid UTF-8\n", *p);
	  assert(false);
	  return 0;
	}
	char* nextu8 = static_cast<char*>(malloc(sizeof(char)*u8len+1));
	memcpy(nextu8, *p, u8len);
	nextu8[u8len] = '\0';
#	if 0
	fprintf(stderr, "*** DBG: adding %s to alpha from %s and skipping to %s (+%d)\n",
		nextu8, *p, (*p)+u8len, u8len);
#	endif
	*p += u8len;
	s = add_string(nextu8);
	free(nextu8);
  }
  else if (s == NO_SYMBOL_NUMBER && !addUnknown)
  {
    assert((s != NO_SYMBOL_NUMBER) || addUnknown);
  }
  else
  {
#	if 0
		fprintf(stderr, "*** DBG: found %d from %s and skipped to %s\n",
			s, old_p, *p);
#endif
  }
  return s;
}


} // namespace HWFST
