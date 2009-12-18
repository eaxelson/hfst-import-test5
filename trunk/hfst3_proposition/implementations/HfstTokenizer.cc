#include "HfstTokenizer.h"
namespace HFST 
{
int HfstTokenizer::get_next_symbol_size(const char * symbol)
const
{
  if (not *symbol)
    { return 0; }
  if ((0b10000000 & *symbol) == 0)
    { return 1; }
  if ((0b00100000 & *symbol) == 0)
    { return 2; }
  if ((0b00010000 & *symbol) == 0)
    { return 3; }
  return 4;
}

void
HfstTokenizer::add_multichar_symbol(const char * symbol,KeyTable &key_table)
const
{ /* Only partially functional */
  key_table.add_symbol(symbol); }

KeyPairVector * HfstTokenizer::tokenize
(const char * input_string,KeyTable &key_table) const
{
  KeyPairVector * kpv = new KeyPairVector;
  while (*input_string)
    {
      int symbol_size = get_next_symbol_size(input_string);
      std::string symbol(input_string,0,symbol_size);
      input_string += symbol_size;
      Key k = key_table.add_symbol(symbol);
      kpv->push_back(KeyPair(k,k));
    }
  return kpv;
}

KeyPairVector * HfstTokenizer::tokenize
(const char * input_string,const char * output_string,KeyTable &key_table)
const
{
  KeyPairVector * kpv = new KeyPairVector;
  while (*input_string or *output_string)
    {
      int input_symbol_size = get_next_symbol_size(input_string);
      int output_symbol_size = get_next_symbol_size(output_string);
      Key input_key = 0;
      Key output_key = 0;
      if (input_symbol_size != 0)
	{
	  std::string input_symbol(input_string,0,input_symbol_size);
	  input_key = key_table.add_symbol(input_symbol);
	}
      if (output_symbol_size != 0)
	{
	  std::string output_symbol(output_string,0,output_symbol_size);
	  output_key = key_table.add_symbol(output_symbol);
	}
      kpv->push_back(KeyPair(input_key,output_key));
      input_string += input_symbol_size;
      output_string += output_symbol_size;
    }
  return kpv;
}
}

#ifdef DEBUG_MAIN_TOKENIZE
HFST_SYMBOLS::GlobalSymbolTable HFST::KeyTable::global_symbol_table;
int main(void)
{
  HFST::HfstTokenizer tokenizer;
  HFST::KeyTable key_table;
  const char * str1 = "äläkkä";
  const char * str2 = "kälä";
  HFST::KeyPairVector * kpv = tokenizer.tokenize(str1,str2,key_table);
  delete kpv;
}

#endif
