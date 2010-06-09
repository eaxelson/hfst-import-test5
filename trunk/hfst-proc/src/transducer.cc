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
  for(SymbolTable::const_iterator it=symbol_table.begin(); it!=symbol_table.end(); it++)
  {
    if(it->second == " ")
    {
      blank_symbol = it->first;
      break;
    }
  }
  
  if(blank_symbol == NO_SYMBOL_NUMBER)
    blank_symbol = add_symbol(" ");
}

void TransducerAlphabet::get_next_symbol(std::istream& is, SymbolNumber k)
{
  std::string str;
  std::getline(is, str, '\0');
  if(printDebuggingInformationFlag)
    std::cout << "Got next symbol: '" << str << "' (" << k << ")" << std::endl;
  
  if(!is)
  {
    std::cerr << "Could not parse transducer; wrong or corrupt file?" << std::endl;
    exit(1);
  }

  if (str.length() >= 5 && str.at(0) == '@' && str.at(str.length()-1) == '@' && str.at(2) == '.')
  { // a flag diacritic needs to be parsed
    std::string feat;
    std::string val;
    FlagDiacriticOperator op = P; // g++ worries about this falling through uninitialized
    switch (str.at(1)) {
    case 'P': op = P; break;
    case 'N': op = N; break;
    case 'R': op = R; break;
    case 'D': op = D; break;
    case 'C': op = C; break;
    case 'U': op = U; break;
    }
    const char* cstr = str.c_str();
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
    add_symbol(k, "",
               FlagDiacriticOperation(op, feature_bucket[feat], value_bucket[val]));
    
#if OL_FULL_DEBUG
    std::cout << "symbol number " << k << " (flag) is " << str << std::endl;
    symbol_table[k] = str;
#endif
    
    return;
  }

#if OL_FULL_DEBUG
  std::cout << "symbol number " << k << " is " << str << std::endl;
#endif
  add_symbol(k, str);
}

std::string
TransducerAlphabet::symbols_to_string(const SymbolNumberVector& symbols) const
{
  std::string str="";
  for(SymbolNumberVector::const_iterator it=symbols.begin(); it!=symbols.end(); it++)
    str += symbol_to_string(*it);
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

void
TransducerAlphabet::calculate_alphabetic()
{
  for(SymbolTable::const_iterator it=symbol_table.begin(); it!=symbol_table.end(); it++)
    if(is_alphabetic(it->second.c_str()))
      alphabetic.insert(it->first);
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
  
  return creators[h.probe_flag(Weighted)][a.get_state_size()>0](is, h, a);
}

void
AbstractTransducer::tokenize(TokenIOStream& token_stream)
{
  Token next_token;
  while((next_token=token_stream.get_token()).type != None)
  {
    if(next_token.type == Symbol)
      std::cout << "Symbol:     #" << next_token.symbol << "(" << alphabet.symbol_to_string(next_token.symbol) << ")";
    else if(next_token.type == Character)
      std::cout << "Character:  '" << next_token.character << "'" << (token_stream.is_space(next_token)?" (space)":"");
    else if(next_token.type == Superblank)
      std::cout << "Superblank: " << token_stream.get_superblank(next_token.superblank_index);
    else if(next_token.type == None)
      std::cout << "None/EOF";
    
    std::cout << " (to_symbol: " << token_stream.to_symbol(next_token) << ")" 
              << " (alphabetic: " << token_stream.is_alphabetic(next_token) << ")" << std::endl;
  }
}

void
AbstractTransducer::run_lookup(TokenIOStream& token_stream, OutputFormatter& output_formatter)
{
  LookupState state(*this);
  size_t last_stream_location = 0;
  TokenVector surface_form;
  std::vector<std::string> analyzed_forms;
  
  Token next_token;
  while((next_token = token_stream.get_token()).type != None)
  {
    if(printDebuggingInformationFlag)
    {
      if(next_token.type == Symbol)
        std::cout << "Got symbol #" << next_token.symbol << "(" << alphabet.symbol_to_string(next_token.symbol) << ")" << std::endl;
      else if(next_token.type == Character)
        std::cout << "Got non-symbolic character '" << next_token.character << "'" << (token_stream.is_space(next_token)?" (space)":"") << std::endl;
      else if(next_token.type == Superblank)
        std::cout << "Got superblank " << token_stream.get_superblank(next_token.superblank_index) << std::endl;
    }
    
  	if(state.is_final())
  	{
  	  LookupPathVector finals = state.get_finals();
  	  analyzed_forms = output_formatter.process_finals(finals);
  	  last_stream_location = token_stream.get_pos()-1;
  	  
  	  if(printDebuggingInformationFlag)
  	    std::cout << "Final paths found and saved, stream location is " << last_stream_location << std::endl;
  	}
  	
  	state.step(token_stream.to_symbol(next_token));
    
    if(printDebuggingInformationFlag)
      std::cout << "After stepping, there are " << state.num_active() << " active paths" << std::endl;
    
    if(state.is_active())
    {
      surface_form.push_back(next_token);
    }
    else
    {
      if(surface_form.empty() && !token_stream.is_alphabetic(next_token))
      {
        if(output_formatter.preserve_nonalphabetic())
          token_stream << next_token;
      }
      else if(analyzed_forms.size() == 0 || 
              token_stream.is_alphabetic(token_stream.at(last_stream_location)))
      {
        // if this is false, then we need to move the token stream back far
        // enough that next_token will be read again next iteration
        bool next_token_is_part_of_word = false;
        if(token_stream.is_alphabetic(next_token))
        {
          next_token_is_part_of_word = true;
          do
          {
            surface_form.push_back(next_token);
          }
          while((next_token = token_stream.get_token()).type != None && token_stream.is_alphabetic(next_token));
          // we overstepped the word by one token
          token_stream.move_back(1);
        }
        
        if(!token_stream.is_alphabetic(surface_form[0]))
        {
          if(output_formatter.preserve_nonalphabetic())
            token_stream << surface_form[0];
          token_stream.move_back(surface_form.size()-1);
        }
        else
        {
          size_t word_length = token_stream.first_nonalphabetic(surface_form);
          if(word_length == string::npos)
            word_length = surface_form.size();
          
          int revert_count = surface_form.size()-word_length+
                         next_token_is_part_of_word ? 0 : 1;
          
          if(printDebuggingInformationFlag)
            std::cout << "word_length=" << word_length << ", surface_form.size()=" << surface_form.size() 
                      << ", moving back " << revert_count << " characters" << std::endl;
          
          output_formatter.print_unknown_word(TokenVector(surface_form.begin(),
                                                surface_form.begin()+word_length));
          token_stream.move_back(revert_count);
        }
      }
      else // there are one or more valid tranductions
      {
        // the number of symbols on the end of surface_form that aren't a part
        // of the transduction(s) found
        int revert_count = token_stream.get_pos()-last_stream_location-1;
        output_formatter.print_word(TokenVector(surface_form.begin(), 
                                                surface_form.end()-revert_count),
                                   analyzed_forms); 
        token_stream.move_back(revert_count+1);
      }
      
      state.reset();
      surface_form.clear();
      analyzed_forms.clear();
    }
  }
  
  if(printDebuggingInformationFlag)
    std::cout << "Got None/EOF symbol; done." << std::endl;
  
  // print any valid transductions stored
  if(analyzed_forms.size() != 0)// && token_stream.get_pos() == last_stream_location)
    output_formatter.print_word(surface_form, analyzed_forms);
}


//////////Function definitions for transducer implementations

LookupPath* Transducer::get_initial_path() const
{
  return new LookupPath(START_INDEX);
}
LookupPath* TransducerFd::get_initial_path() const
{
  LookupPathFd* p = new LookupPathFd(START_INDEX, alphabet.get_state_size(), 
                           alphabet.get_operation_vector());
  return p;
}
LookupPath* TransducerW::get_initial_path() const
{
  return new LookupPathW(START_INDEX);
}
LookupPath* TransducerWFd::get_initial_path() const
{
  return new LookupPathWFd(START_INDEX, alphabet.get_state_size(), 
                            alphabet.get_operation_vector());
}

