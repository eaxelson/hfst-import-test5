#include "HfstTokenizer.h"
#include <string>

using std::string;
namespace hfst 
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
HfstTokenizer::add_multichar_symbol(const string& symbol,KeyTable &key_table)
const
{ /* Only partially functional */
  key_table.add_symbol(symbol.c_str()); }

KeyPairVector * HfstTokenizer::tokenize
(const string& input_string,KeyTable &key_table) const
{
  KeyPairVector * kpv = new KeyPairVector;
  const char* s = input_string.c_str();
  while (*s)
    {
      int symbol_size = get_next_symbol_size(s);
      std::string symbol(s,0,symbol_size);
      s += symbol_size;
      Key k = key_table.add_symbol(symbol);
      kpv->push_back(KeyPair(k,k));
    }
  return kpv;
}

KeyPairVector * HfstTokenizer::tokenize
(const string& input_string,const string& output_string,KeyTable &key_table)
const
{
  KeyPairVector * kpv = new KeyPairVector;
  const char* ins = input_string.c_str();
  const char* outs = output_string.c_str();
  while (*ins or *outs)
    {
      int input_symbol_size = get_next_symbol_size(ins);
      int output_symbol_size = get_next_symbol_size(outs);
      Key input_key = 0;
      Key output_key = 0;
      if (input_symbol_size != 0)
	{
	  std::string input_symbol(ins,0,input_symbol_size);
	  input_key = key_table.add_symbol(input_symbol);
	}
      if (output_symbol_size != 0)
	{
	  std::string output_symbol(outs,0,output_symbol_size);
	  output_key = key_table.add_symbol(output_symbol);
	}
      kpv->push_back(KeyPair(input_key,output_key));
      ins += input_symbol_size;
      outs += output_symbol_size;
    }
  return kpv;
}
}

#ifdef DEBUG_MAIN_TOKENIZE
hfst::symbols::GlobalSymbolTable hfst::KeyTable::global_symbol_table;
int main(void)
{
  hfst::HfstTokenizer tokenizer;
  hfst::KeyTable key_table;
  const char * str1 = "äläkkä";
  const char * str2 = "kälä";
  hfst::KeyPairVector * kpv = tokenizer.tokenize(str1,str2,key_table);
  delete kpv;
}

#endif
