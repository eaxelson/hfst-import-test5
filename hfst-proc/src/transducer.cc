#include <cstring>
#include <cstdlib>
#include <sstream>
#include "transducer.h"
#include "lookup-path.h"
#include "lookup-state.h"
#include "tokenizer.h"
#include "formatter.h"

//////////Function definitions for TransducerHeader

bool TransducerHeader::check_hfst3_header(std::istream& is)
{
  const char* header1 = "HFST3";
  int header_loc = 0; // how much of the header has been found
  int c;
  for(header_loc = 0; header_loc<6; header_loc++)
  {
    c = is.get();
    if(c != header1[header_loc])
      break;
  }
  
  if(header_loc == 6) // we found it
  {
    const char* header2[] = {"HFST_OL_TYPE", "HFST_OLW_TYPE"};
    std::string h_str;
    std::getline(is, h_str, '\0');
    
    if(h_str == header2[0] || h_str == header2[1])
      return true;
    header_error();
    return false;
  }
  else // nope. put back what we've taken
  {
    is.putback(c); // first the non-matching character
    for(int i=header_loc-1; i>=0; i--) // then the characters that did match (if any)
      is.putback(header1[i]);
    
    return false;
  }
}


//////////Function definitions for TransducerAlphabet

TransducerAlphabet::TransducerAlphabet(std::istream& is, 
                                       SymbolNumber symbol_count):
  symbol_table(), symbolizer(), blank_symbol(NO_SYMBOL_NUMBER),
  feature_bucket(), value_bucket()
{
  if(symbol_count == 0)
  {
    std::cerr << "Transducer has empty alphabet; wrong or corrupt file?" << std::endl;
    exit(1);
  }
  value_bucket[std::string()] = 0; // empty value = neutral
  for(SymbolNumber k=0; k<symbol_count; k++)
    add_symbol(get_next_symbol(is));
  
  if(verboseFlag && get_state_size()>0)
    std::cout << "Alphabet contains " << get_state_size() << " flag diacritic feature(s)" << std::endl;
  // assume the first symbol is epsilon which we don't want to print
  symbol_table[0].str = "";
  
  check_for_overlapping();
  setup_blank_symbol();
  calculate_caps();
  if(printDebuggingInformationFlag)
    print_table();
}

TransducerAlphabet::~TransducerAlphabet() {}

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
    add_symbol(s);
  }
}

void
TransducerAlphabet::check_for_overlapping() const
{
  std::vector<std::string> overlapping;
  
  for(size_t i=0;i<symbol_table.size();i++)
  {
    std::string str = symbol_table[i].str;
    if(str.length() > 1 && !is_punctuation(std::string(1, str[0]).c_str()))
    {
      std::istringstream s(str);
      
      // divide the symbol into UTF8 characters
      std::vector<std::string> chars;
      while(true)
      {
        std::string ch = TokenIOStream::read_utf8_char(s);
        if(ch == "")
          break;
        else
          chars.push_back(ch);
      }
      if(chars.size() < 2)
        continue;
      
      bool overlaps = true;
      for(size_t j=0;j<chars.size();j++)
      {
        std::string ch = chars[j];
        if(!is_alphabetic(ch.c_str()) || symbolizer.find_symbol(ch.c_str()) == NO_SYMBOL_NUMBER)
        {
          overlaps = false;
          break;
        }
      }
      
      if(overlaps)
        overlapping.push_back(str);
    }
  }
  
  if(!overlapping.empty())
  {
    std::cerr << "!! Warning: Transducer contains one or more multi-character symbols made up of\n"
              << "ASCII characters which are also available as single-character symbols. The\n"
              << "input stream will always be tokenized using the longest symbols available.\n"
              << "Use the -t option to view the tokenization. The problematic symbol(s):\n";
    for(size_t i=0;i<overlapping.size();i++)
      std::cerr << (i==0?"":" ") << overlapping[i];
    std::cerr << std::endl;
  }
}

SymbolProperties
TransducerAlphabet::get_next_symbol(std::istream& is)
{
  SymbolProperties symbol;
  std::getline(is, symbol.str, '\0');
  
  if(!is)
  {
    std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
    exit(1);
  }

  if (symbol.str.length() >= 5 && symbol.str.at(0) == '@' && symbol.str.at(symbol.str.length()-1) == '@' && symbol.str.at(2) == '.')
  { // a flag diacritic needs to be parsed
    FlagDiacriticOperator op = FlagDiacriticOperation::char_to_operator(symbol.str.at(1));
    std::string feat;
    std::string val;
    const char * c = symbol.str.c_str();
    // as long as we're working with utf-8, this should be ok
    for (c +=3; *c != '.' && *c != '@'; c++) { feat.append(c,1); }
    if (*c == '.')
    {
      for (++c; *c != '@'; c++) { val.append(c,1); }
    }
    if (feature_bucket.count(feat) == 0)
    {
      SymbolNumber next = feature_bucket.size();
      feature_bucket[feat] = next;
    }
    if (value_bucket.count(val) == 0)
      value_bucket[val] = value_bucket.size()+1;
    
    symbol.fd_op = FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val], symbol.str.c_str());
    
    symbol.str = "";
  }
  
  symbol.alphabetic = is_alphabetic(symbol.str.c_str());
  
  return symbol;
}

void
TransducerAlphabet::print_table() const
{
  std::cout << "Symbol table containing " << symbol_table.size() << " symbols:" << std::endl;
  for(SymbolNumber i=0;i<symbol_table.size();i++)
  {
    std::cout << "Symbol: #" << i << ", '" << (symbol_table[i].fd_op.isFlag()?symbol_table[i].fd_op.Name():symbol_to_string(i)) << "',"
              << (is_alphabetic(i)?" ":" not ") << "alphabetic, ";
    if(is_lower(i))
    {
      SymbolNumber s2 = to_upper(i);
      std::cout << "lowercase, upper: " << s2 << "/" << symbol_to_string(s2);
    }
    else if(is_upper(i))
    {
      SymbolNumber s2 = to_lower(i);
      std::cout << "uppercase, lower: " << s2 << "/" << symbol_to_string(s2);
    }
    else
      std::cout << "no case";
    std::cout << std::endl;
  }
}

void
TransducerAlphabet::add_symbol(const SymbolProperties& symbol)
{
  symbol_table.push_back(symbol);
  symbolizer.add_symbol(symbol);
}

void
TransducerAlphabet::calculate_caps()
{
  size_t size = symbol_table.size(); // size before any new symbols added
  for(size_t i=0;i<size;i++)
  {
    int case_res = 0;
    std::string switched;
    if(is_alphabetic(i))
      switched = caps_helper(symbol_table[i].str.c_str(), case_res);
    else
      case_res = 0;
        
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
    
    if(to_lower(i) == to_upper(i) && symbol_table[i].lower != NO_SYMBOL_NUMBER)
    {
      if(switched != "")
      {
        SymbolProperties new_symb;
        new_symb.str = switched;
        new_symb.alphabetic = symbol_table[i].alphabetic;
        if(symbol_table[i].lower == i)
        {
          symbol_table[i].upper = symbol_table.size();
          new_symb.lower = i;
          new_symb.upper = symbol_table.size();
        }
        else
        {
          symbol_table[i].lower = symbol_table.size();
          new_symb.upper = i;
          new_symb.lower = symbol_table.size();
        }
        add_symbol(new_symb);
        if(printDebuggingInformationFlag)
          std::cout << "Added new symbol '" << switched << "' (" << symbol_table.size()-1 << ") as alternate case for '" 
                    << symbol_table[i].str << "' (" << i << ")" << std::endl;
      }
      else
      {
        if(printDebuggingInformationFlag)
          std::cout << "Symbol " << i << "'s alternate case is unknown" << std::endl;
      }
    }
    
    
    if(printDebuggingInformationFlag &&
       symbol_table[i].lower != NO_SYMBOL_NUMBER && symbol_table[i].upper != NO_SYMBOL_NUMBER && 
       symbol_to_string(symbol_table[i].lower).length() != symbol_to_string(symbol_table[i].upper).length())
    {
      std::cout << "Symbol " << i << "'s alternate case has a different string length" << std::endl;
    }
  }
}

std::string
TransducerAlphabet::caps_helper_single(const char* c, int& case_res)
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

std::string
TransducerAlphabet::caps_helper(const char* in, int& case_res)
{
  std::istringstream str(in);
  std::string out;
  case_res = 0;
  int tmp = -2; // -2 indicates first time through the loop
  
  std::vector<std::string> chars;
  while(true)
  {
    std::string c = TokenIOStream::read_utf8_char(str);
    if(c == "")
      break;
    
    std::string switched = caps_helper_single(c.c_str(), (tmp==-2?case_res:tmp));
    tmp=0;
    out.append((switched==""?c:switched));
  }
  return out;
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
  static const char* punct_ranges[8][2] = {{"!","/"},
                                           {":","@"},
                                           {"[","`"},
                                           {"{","~"},
                                           {"¡","¿"},
                                           {"‐","⁞"},
                                           {"₠","₸"},
                                           {"∀","⋿"}};
  const char* individual_chars = "×÷";
  for(int i=0;i<8;i++)
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
  
  return (strstr(individual_chars, c) != NULL);
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

const Symbolizer&
TransducerAlphabet::get_symbolizer() const { return symbolizer; }


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

bool TransitionIndex::final() const
{
  return input_symbol == NO_SYMBOL_NUMBER && first_transition_index != NO_TABLE_INDEX;
}
bool Transition::final() const
{
  return input_symbol == NO_SYMBOL_NUMBER && output_symbol == NO_SYMBOL_NUMBER && target_index == 1;
}

Weight
TransitionWIndex::final_weight(void) const
{
  union to_weight
  {
    TransitionTableIndex i;
    Weight w;
  } weight;
  weight.i = first_transition_index;
  return weight.w;
}

//////////Function definitions for Transducer

TransducerInternalCreator Transducer::creators[2][2] =
    {{TransducerInternal::create, TransducerFdInternal::create},
     {TransducerWInternal::create, TransducerWFdInternal::create}};

Transducer::Transducer(std::istream& is):
  header(is), alphabet(is, header.symbol_count()),
  transducer_internal(creators[header.probe_flag(Weighted)][alphabet.get_state_size()>0](is, header, alphabet))
{
  if (header.probe_flag(Has_unweighted_input_epsilon_cycles) ||
      header.probe_flag(Has_input_epsilon_cycles))
  {
    std::cerr << "!! Warning: transducer has epsilon cycles                  !!\n"
              << "!! This is currently not handled - if they are encountered !!\n"
              << "!! program *will* segfault.                                !!\n";
  }
  
  if(check_for_blank())
  {
    std::cerr << "!! Warning: transducer accepts input strings consisting of !!\n"
              << "!! just a blank. This is probably a bug in the transducer  !!\n"
              << "!! and will cause strange behavior.                        !!\n";
  }
}

bool
Transducer::check_for_blank() const
{
  if(verboseFlag)
    std::cout << "Checking whether the transducer accepts a single blank as a word..." << std::endl;
  LookupState state(*this);
  state.step(alphabet.get_blank_symbol());
  return state.is_final();
}


//////////Function definitions for transducer implementations

LookupPath* TransducerInternal::get_initial_path(const Transducer& t) const
{
  return new LookupPath(t, 0);
}
LookupPath* TransducerFdInternal::get_initial_path(const Transducer& t) const
{
  return new LookupPathFd(t, 0);
}
LookupPath* TransducerWInternal::get_initial_path(const Transducer& t) const
{
  return new LookupPathW(t, 0);
}
LookupPath* TransducerWFdInternal::get_initial_path(const Transducer& t) const
{
  return new LookupPathWFd(t, 0);
}

