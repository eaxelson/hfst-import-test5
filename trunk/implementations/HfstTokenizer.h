#ifndef _HFST_TOKENIZER_H_
#define _HFST_TOKENIZER_H_
#include "SymbolDefs.h"
#include "GlobalSymbolTable.h"
#include <iostream>
namespace HFST3
{
  using HFST_SYMBOLS::Key;
  using HFST_SYMBOLS::KeyPair;
  using HFST_SYMBOLS::KeyPairVector;
  using HFST_SYMBOLS::KeyTable;

  /* At the moment, this only tokenizes into utf-8 strings. */
  class HfstTokenizer
  {  
  private:
    int get_next_symbol_size(const char * symbol) const;

  public:
    /* This is useless for now... */
    void add_multichar_symbol(const char * symbol,KeyTable &key_table) const;

    KeyPairVector * tokenize(const char * input_string,KeyTable &key_table)
      const;
    KeyPairVector * tokenize(const char * input_string,
			     const char * output_string,
			     KeyTable &key_table) const;
  };
}
#endif
