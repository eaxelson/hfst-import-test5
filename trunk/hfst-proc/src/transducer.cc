#include <cstring>
#include <cstdlib>
#include "transducer.h"
#include "lookup-path.h"
#include "lookup-state.h"
#include "tokenizer.h"
#include "formatter.h"

//////////Function definitions for TransducerAlphabet

void
TransducerAlphabet::setup_blank_symbol()
{
  blank_symbol = NO_SYMBOL_NUMBER;
  for(size_t i=0;i<symbol_table.size();i++)
  {
    if(symbol_table[i].str == " ")
    {
      blank_symbol = i;
      break;
    }
  }
  
  if(blank_symbol == NO_SYMBOL_NUMBER)
  {
    blank_symbol = symbol_table.size();
    SymbolProperties s;
    s.str = " ";
    s.alphabetic = is_alphabetic(s.str.c_str());
    symbol_table.push_back(s);
  }
}

void TransducerAlphabet::get_next_symbol(std::istream& is, SymbolNumber k)
{
  SymbolProperties symbol;
  std::getline(is, symbol.str, '\0');
  if(printDebuggingInformationFlag)
    std::cout << "Got next symbol: '" << symbol.str << "' (" << k << ")" << std::endl;
  
  if(!is)
  {
    std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
    exit(1);
  }

  if (symbol.str.length() >= 5 && symbol.str.at(0) == '@' && symbol.str.at(symbol.str.length()-1) == '@' && symbol.str.at(2) == '.')
  { // a flag diacritic needs to be parsed
    std::string feat;
    std::string val;
    FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
    switch (symbol.str.at(1)) {
    case 'P': op = P; break;
    case 'N': op = N; break;
    case 'R': op = R; break;
    case 'D': op = D; break;
    case 'C': op = C; break;
    case 'U': op = U; break;
    }
    const char* cstr = symbol.str.c_str();
    const char * c = cstr;
    // as long as we're working with utf-8, this should be ok
    for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
    if (*c == '.')
    {
      for (++c; *c != '@'; c++) { val.append(c,1); }
    }
    if (feature_bucket.count(feat) == 0)
    {
      feature_bucket[feat] = feat_num;
      ++feat_num;
    }
    if (value_bucket.count(val) == 0)
    {
      value_bucket[val] = val_num;
      ++val_num;
    }
    
    symbol.fd_op = FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]);
    
#if OL_FULL_DEBUG
    std::cout << "symbol number " << k << " (flag) is " << symbol.str << std::endl;
#else
    symbol.str = "";
#endif
  }
  else // not a flag diacritic
  {
#if OL_FULL_DEBUG
    std::cout << "symbol number " << k << " is " << symbol.str << std::endl;
#endif
  }
  
  symbol.alphabetic = is_alphabetic(symbol.str.c_str());
  
  symbol_table.push_back(symbol);
}

void
TransducerAlphabet::calculate_caps()
{
  Symbolizer symbolizer(symbol_table);
  for(size_t i=0;i<symbol_table.size();i++)
  {
    int case_res;
    std::string switched = caps_helper(symbol_table[i].str.c_str(), case_res);
    
    if(case_res < 0)
    {
      symbol_table[i].lower = i;
      symbol_table[i].upper = (switched=="")?NO_SYMBOL_NUMBER:symbolizer.find_symbol(switched.c_str());
    }
    else if(case_res > 0)
    {
      symbol_table[i].lower = (switched=="")?NO_SYMBOL_NUMBER:symbolizer.find_symbol(switched.c_str());
      symbol_table[i].upper = i;
    }
    else
      symbol_table[i].lower=symbol_table[i].upper=NO_SYMBOL_NUMBER;
  }
}

std::string
TransducerAlphabet::caps_helper(const char* c, int& case_res)
{
  static const char* parallel_ranges[5][2][2] = {{{"A","Z"},{"a","z"}}, // Basic Latin
                                                 {{"À","Þ"},{"à","þ"}}, // Latin-1 Supplement
                                                 {{"Α","Ϋ"},{"α","ϋ"}}, // Greek and Coptic
                                                 {{"А","Я"},{"а","я"}}, // Cyrillic
                                                 {{"Ѐ","Џ"},{"ѐ","џ"}}};// Cyrillic
  static const char* serial_ranges[12][3] = {{"Ā","ķ"}, // Latin Extended A
                                             {"Ĺ","ň"}, // Latin Extended A
                                             {"Ŋ","ž"}, // Latin Extended A
                                             {"Ǎ","ǜ"}, // Latin Extended B
                                             {"Ǟ","ǯ"}, // Latin Extended B
                                             {"Ǵ","ȳ"}, // Latin Extended B
                                             {"Ϙ","ϯ"}, // Greek and Coptic
                                             {"Ѡ","ҿ"}, // Cyrillic
                                             {"Ӂ","ӎ"}, // Cyrillic
                                             {"Ӑ","ӿ"}, // Cyrillic
                                             {"Ԁ","ԥ"}, // Cyrillic Supplement
                                             {"Ḁ","ỿ"}};//Latin Extended Additional
  for(int i=0;i<5;i++) // check parallel ranges
  {
    if(strcmp(c,parallel_ranges[i][0][0]) >= 0 &&
       strcmp(c,parallel_ranges[i][0][1]) <= 0) // in the uppercase section
    {
      case_res = 1;
      int diff = utf8_str_to_int(parallel_ranges[i][1][0]) -
                 utf8_str_to_int(parallel_ranges[i][0][0]);
      return utf8_int_to_str(utf8_str_to_int(c)+diff);
    }
    else if(strcmp(c,parallel_ranges[i][1][0]) >= 0 &&
            strcmp(c,parallel_ranges[i][1][1]) <= 0) // in the lowercase section
    {
      case_res = -1;
      int diff = utf8_str_to_int(parallel_ranges[i][1][0]) -
                 utf8_str_to_int(parallel_ranges[i][0][0]);
      return utf8_int_to_str(utf8_str_to_int(c)-diff);
    }
  }
  for(int i=0;i<12;i++) // check serial ranges
  {
    if(strcmp(c,serial_ranges[i][0]) >= 0 &&
       strcmp(c,serial_ranges[i][1]) <= 0)
    {
      int c_int = utf8_str_to_int(c);
      if((c_int-utf8_str_to_int(serial_ranges[i][0]))%2 == 0) // uppercase
      {
        case_res = 1;
        return utf8_int_to_str(c_int+1);
      }
      else // lowercase
      {
        case_res = -1;
        return utf8_int_to_str(c_int-1);
      }
    }
  }
  case_res = 0;
  return "";
}

int
TransducerAlphabet::utf8_str_to_int(const char* c)
{
  if ((unsigned char)c[0] <= 127)
    return (unsigned char)c[0];
  else if ( (c[0] & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) )
    return (((unsigned char)c[0])<<24)+
           (((unsigned char)c[1])<<16)+
           (((unsigned char)c[2])<<8)+
           ((unsigned char)c[3]);
  else if ( (c[0] & (128 + 64 + 32 )) == (128 + 64 + 32) )
    return (((unsigned char)c[0])<<16)+
           (((unsigned char)c[1])<<8)+
           ((unsigned char)c[2]);
  else if ( (c[0] & (128 + 64 )) == (128 + 64))
    return (((unsigned char)c[0])<<8)+
           ((unsigned char)c[1]);
  else
    stream_error("Invalid UTF-8 character found");
  return -1;
}
std::string
TransducerAlphabet::utf8_int_to_str(int c)
{
  std::string res;
  for(int i=3;i>=0;i--)
  {
    if(c & (0xFF << (8*i)))
    {
      for(;i>=0;i--)
        res.push_back((char)((c&(0xFF<<(8*i)))>>(8*i)  )&0xFF);
      return res;
    }
  }
  return "";
}

std::string
TransducerAlphabet::symbols_to_string(const SymbolNumberVector& symbols, CapitalizationState caps) const
{
  std::string str="";
  bool first=true;
  for(SymbolNumberVector::const_iterator it=symbols.begin(); it!=symbols.end(); it++, first=false)
  {
    if(caps==UpperCase || (caps==FirstUpperCase && first==true))
      str += symbol_to_string(to_upper(*it));
    else
      str += symbol_to_string(*it);
  }
  return str;
}

bool
TransducerAlphabet::is_punctuation(const char* c) const
{
  static const char* punct_ranges[6][2] = {{"!","/"},
                                           {":","@"},
                                           {"[","`"},
                                           {"{","~"},
                                           {"¡","¿"},
                                           {"‐","⁞"}};
  for(int i=0;i<6;i++)
  {
    if(strcmp(c,punct_ranges[i][0]) >= 0 && 
       strcmp(c,punct_ranges[i][1]) <= 0)
    {
      // a hack to filter out symbols (e.g. tags) that may start with punctuation
      // and then contain ASCII text. Tags should be treated as alphabetic.
      for(;*c!='\0';c++)
      {
        if(isalnum(*c))
          return false;
      }
      return true;
    }
  }
  return false;
}

bool
TransducerAlphabet::is_tag(SymbolNumber symbol) const
{
  std::string str = symbol_to_string(symbol);
  if(str[0] == '<' && str[str.length()-1] == '>')
    return true;
  return false;
}

bool
TransducerAlphabet::is_compound_boundary(SymbolNumber symbol) const
{
  std::string s = symbol_to_string(symbol);
  if(s == "+" || s[s.length()-1] == '+' ||
     s == "#" || s[s.length()-1] == '#')
    return true;
  return false;
}

int
TransducerAlphabet::num_compound_boundaries(const SymbolNumberVector& symbol) const
{
  int count=0;
  for(SymbolNumberVector::const_iterator i=symbol.begin(); i!=symbol.end(); i++)
  {
    if(is_compound_boundary(*i))
      count++;
  }
  return count;
}


//////////Function definitions for TransitionIndex and Transition

bool
TransitionIndex::matches(SymbolNumber s) const
{  
  if(input_symbol == NO_SYMBOL_NUMBER)
    return false;
  if(s == NO_SYMBOL_NUMBER)
    return true;
  return input_symbol == s;
}

bool
Transition::matches(SymbolNumber s) const
{
  if(input_symbol == NO_SYMBOL_NUMBER)
    return false;
  if(s == NO_SYMBOL_NUMBER)
    return true;
  return input_symbol == s;
}


//////////Function definitions for AbstractTransducer

TransducerCreator AbstractTransducer::creators[2][2] =
    {{Transducer::create, TransducerFd::create},
     {TransducerW::create, TransducerWFd::create}};

AbstractTransducer*
AbstractTransducer::create(std::istream& is, TransducerHeader h)
{
  if(printDebuggingInformationFlag)
    h.print();

  if (h.probe_flag(Has_unweighted_input_epsilon_cycles) ||
      h.probe_flag(Has_input_epsilon_cycles))
  {
    std::cerr << "!! Warning: transducer has epsilon cycles                  !!\n"
              << "!! This is currently not handled - if they are encountered !!\n"
              << "!! program *will* segfault.                                !!\n";
  }
  
  TransducerAlphabet a(is, h.symbol_count());
  
  AbstractTransducer* t = creators[h.probe_flag(Weighted)][a.get_state_size()>0](is, h, a);
  
  if(t->check_for_blank())
  {
    std::cerr << "!! Warning: transducer accepts input strings consisting of !!\n"
              << "!! just a blank. This is probably a bug in the transducer  !!\n"
              << "!! and will cause strange behavior.                        !!\n";
  }
  
  return t;
}

bool
AbstractTransducer::check_for_blank() const
{
  if(verboseFlag)
    std::cout << "Checking whether the transducer accepts a single blank as a word..." << std::endl;
  LookupState state(*this);
  state.step(alphabet.get_blank_symbol());
  return state.is_final();
}


//////////Function definitions for transducer implementations

LookupPath* Transducer::get_initial_path() const
{
  return new LookupPath(*this, START_INDEX);
}
LookupPath* TransducerFd::get_initial_path() const
{
  return new LookupPathFd(*this, START_INDEX);
}
LookupPath* TransducerW::get_initial_path() const
{
  return new LookupPathW(*this, START_INDEX);
}
LookupPath* TransducerWFd::get_initial_path() const
{
  return new LookupPathWFd(*this, START_INDEX);
}

