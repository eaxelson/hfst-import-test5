/*
   Test file for HfstTokenizer.
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

int main(int argc, char **argv) 
{

  /* From one string. */
  verbose_print("Tokenization from one string with multichar symbol "
		"\"foo\" and skip symbol \"bar\"");
  
  HfstTokenizer tok1;
  tok1.add_multichar_symbol("foo");
  tok1.add_skip_symbol("bar");
  StringPairVector tokenization1 = tok1.tokenize("fobaro");
  assert(tokenization1.size() == 3);
  assert(tokenization1[0] == StringPair("f","f"));
  assert(tokenization1[1] == StringPair("o","o"));
  assert(tokenization1[2] == StringPair("o","o"));
  
  verbose_print("Tokenization from one string with multichar symbol "
		"\"foo\" and skip symbol \"fo\"");

  HfstTokenizer tok2;
  tok2.add_multichar_symbol("foo");
  tok2.add_skip_symbol("fo");
  StringPairVector tokenization2 = tok2.tokenize("foo");
  assert(tokenization2.size() == 1);
  assert(tokenization2[0] == StringPair("foo","foo"));
  
  verbose_print("Tokenization from one string with multichar symbol "
		"\"fo\" and skip symbol \"foo\"");

  HfstTokenizer tok3;
  tok3.add_multichar_symbol("fo");
  tok3.add_skip_symbol("foo");
  StringPairVector tokenization3 = tok3.tokenize("foo");
  assert(tokenization3.size() == 0);
  
  /* From two strings. */
  verbose_print("Tokenization from two strings with multichar symbol "
		"\"foo\" and skip symbol \"bar\"");

  HfstTokenizer tok4;
  tok4.add_multichar_symbol("foo");
  tok4.add_skip_symbol("bar");
  StringPairVector tokenization4 = tok4.tokenize("fooba", "foobar");
  assert(tokenization4.size() == 3);
  assert(tokenization4[0] == StringPair("foo","foo"));
  assert(tokenization4[1] == StringPair("b","@_EPSILON_SYMBOL_@"));
  assert(tokenization4[2] == StringPair("a","@_EPSILON_SYMBOL_@"));

}
