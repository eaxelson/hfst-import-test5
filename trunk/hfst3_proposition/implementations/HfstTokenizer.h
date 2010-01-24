#ifndef _HFST_TOKENIZER_H_
#define _HFST_TOKENIZER_H_
#include "SymbolDefs.h"
#include "GlobalSymbolTable.h"
#include <string>
namespace hfst
{
  using hfst::symbols::Key;
  using hfst::symbols::KeyPair;
  using hfst::symbols::KeyPairVector;
  using hfst::symbols::KeyTable;

  /* At the moment, this only tokenizes into utf-8 strings. */
  class HfstTokenizer
  {  
  private:
    int get_next_symbol_size(const char * symbol) const;

  public:
    /* This is useless for now... */
    void add_multichar_symbol(const std::string& symbol,KeyTable &key_table) const;

    KeyPairVector * tokenize(const std::string& input_string,KeyTable &key_table)
      const;
    KeyPairVector * tokenize(const std::string& input_string,
			     const std::string& output_string,
			     KeyTable &key_table) const;
  };
}
#endif
