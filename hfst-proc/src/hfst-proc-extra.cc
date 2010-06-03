#include "hfst-optimized-lookup.h"
#include "hfst-proc-extra.h"


//////////Function definitions for class LookupPath

void
LookupPath::follow(const TransitionIndex& index)
{
  this->index = index.target();
  final = index.final();
}

bool
LookupPath::follow(const Transition& transition)
{
  index = transition.target();
  final = transition.final();
  if(transition.get_output() != 0)
    output_symbols.push_back(transition.get_output());
  
  return true;
}


//////////Function definitions for class LookupPathFd

bool
LookupPathFd::evaluate_flag_diacritic(FlagDiacriticOperation op)
{
  switch (op.Operation()) {
  case P: // positive set
    fd_state[op.Feature()] = op.Value();
    return true;
    
  case N: // negative set (literally, in this implementation)
    fd_state[op.Feature()] = -1*op.Value();
    return true;
    
  case R: // require
    if (op.Value() == 0) // empty require
      return (fd_state[op.Feature()] != 0);
    else // nonempty require
      return (fd_state[op.Feature()] == op.Value());
      
  case D: // disallow
    if (op.Value() == 0) // empty disallow
       return (fd_state[op.Feature()] == 0);
    else // nonempty disallow
      return (fd_state[op.Feature()] != op.Value());
      
  case C: // clear
    fd_state[op.Feature()] = 0;
    return true;
    
  case U: // unification
    if(fd_state[op.Feature()] == 0 || // if the feature is unset or
       fd_state[op.Feature()] == op.Value() || // the feature is at this value already or
       (fd_state[op.Feature()] < 0 &&
       (fd_state[op.Feature()]*-1 != op.Value())) // the feature is negatively set to something else
       )
    {
      fd_state[op.Feature()] = op.Value();
      return true;
    }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool LookupPathFd::follow(const Transition& transition)
{
  if((*fd_operations)[transition.get_input()].isFlag())
  {
    if(evaluate_flag_diacritic((*fd_operations)[transition.get_input()]))
    {
      if(printDebuggingInformationFlag)
        std::cout << "flag diacritic [" << transition.get_input() << "] allowed" << std::endl;
      return LookupPath::follow(transition);
    }

    if(printDebuggingInformationFlag)
      std::cout << "flag diacritic [" << transition.get_input() << "] disallowed" << std::endl;
    return false;
  }
  
  return LookupPath::follow(transition);
}


//////////Function definitions for class LookupPathW

void
LookupPathW::follow_weight(const TransitionIndex& index)
{
  final_weight = static_cast<const TransitionWIndex&>(index).final_weight();
}
void
LookupPathW::follow_weight(const Transition& transition)
{
  weight += static_cast<const TransitionW&>(transition).get_weight();
  //**is this right? I'm not so sure about the precise semantics of weights
  //  and finals in this system**
  final_weight = static_cast<const TransitionW&>(transition).get_weight();
}

void
LookupPathW::follow(const TransitionIndex& index)
{
  LookupPath::follow(index);
  follow_weight(index);
}

bool
LookupPathW::follow(const Transition& transition)
{
  LookupPath::follow(transition);
  follow_weight(transition);
  
  return true;
}

//////////Function definitions for class LookupPathWFd

void
LookupPathWFd::follow_weight(const TransitionIndex& index)
{
  final_weight = static_cast<const TransitionWIndex&>(index).final_weight();
}
void
LookupPathWFd::follow_weight(const Transition& transition)
{
  weight += static_cast<const TransitionW&>(transition).get_weight();
  //**is this right? I'm not so sure about the precise semantics of weights
  //  and finals in this system**
  final_weight = static_cast<const TransitionW&>(transition).get_weight();
}

void
LookupPathWFd::follow(const TransitionIndex& index)
{
  LookupPath::follow(index);
  follow_weight(index);
}

bool
LookupPathWFd::follow(const Transition& transition)
{
  if(LookupPathFd::follow(transition))
  {
    follow_weight(transition);
    return true;
  }
  
  return false;
}


//////////Function definitions for class LookupState

void
LookupState::init(LookupPath* initial)
{
  clear_paths();
  paths.push_back(initial);
  try_epsilons();
}

void
LookupState::step(const SymbolNumber input)
{
  if(input == NO_SYMBOL_NUMBER)
  {
    clear_paths();
    return;
  }
  
  apply_input(input);
  try_epsilons();
}

  
void
LookupState::clear_paths()
{
  for(LookupPathVector::const_iterator it = paths.begin(); it!=paths.end(); it++)
    delete *it;
  paths.clear();
}

bool
LookupState::is_final() const
{
  for(LookupPathVector::const_iterator i=paths.begin(); 
      i!=paths.end(); ++i)
  {
    TransitionTableIndex index = (*i)->get_index();
    if(indexes_transition_index_table(index))
    {
      if(transducer.get_index(index).final())
        return true;
    }
    else
    {
      if(transducer.get_transition(index).final())
        return true;
    }
  }
  return false;
}

const LookupPathVector
LookupState::get_finals() const
{
  LookupPathVector finals;
  for(LookupPathVector::const_iterator i=paths.begin(); i!=paths.end(); ++i)
  {
    TransitionTableIndex index = (*i)->get_index();
    if(indexes_transition_index_table(index))
    {
      if(transducer.get_index(index).final())
        finals.push_back(*i);
    }
    else
    {
      if(transducer.get_transition(index).final())
        finals.push_back(*i);
    }
  }
  return finals;
}

void
LookupState::add_path(LookupPath& path)
{
  paths.push_back(&path);
}

void
LookupState::replace_paths(LookupPathVector new_paths)
{
  clear_paths();
  paths = new_paths;
}



void
LookupState::try_epsilons()
{
  for(size_t i=0; i<paths.size(); i++)
  {
    const LookupPath& path = *paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_epsilon_index(path);
    else // indexes transition table
      try_epsilon_transitions(path);
  }
}

void
LookupState::try_epsilon_index(const LookupPath& path)
{
  // if this path points to an entry in the transition index table
  // which indexes one or more epsilon transtions
  const TransitionIndex& index = transducer.get_index(path.get_index()+1);
  
  if(index.matches(0))
  {
    // copy the current path, follow the index, add the new path to the list
    LookupPath& epsilon_path = *path.clone();
    epsilon_path.follow(index);
    add_path(epsilon_path);
  }
}

void
LookupState::try_epsilon_transitions(const LookupPath& path)
{
  TransitionTableIndex transition_index;
  
  // if the path is pointing to the "state" entry before the transitions
  if(transducer.get_transition(path.get_index()).get_input() == NO_SYMBOL_NUMBER)
    transition_index = path.get_index()+1;
  else // the path is pointing directly to a transition
    transition_index = path.get_index();
  
  while(true)
  {
    const Transition& transition = transducer.get_transition(transition_index);
    
    if(transducer.is_epsilon(transition))
    {
      // copy the path, follow the transition, add the new path to the list
      LookupPath& epsilon_path = *path.clone();
      if(epsilon_path.follow(transition))
        add_path(epsilon_path);
      else
      {
        // destroy the new path instead of pushing it
        delete &epsilon_path;
      }
    }
    else
      return;
    
    transition_index++;
  }
}


void
LookupState::apply_input(const SymbolNumber input)
{
  LookupPathVector new_paths;
  if(input == 0)
  {
    replace_paths(new_paths);
    return;
  }
  
  for(size_t i=0; i<paths.size(); i++)
  {
    LookupPath& path = *paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_index(new_paths, path, input);
    else // indexes transition table
      try_transitions(new_paths, path, input);
  }

  replace_paths(new_paths);  
}

void
LookupState::try_index(LookupPathVector& new_paths, 
                         const LookupPath& path, 
                         const SymbolNumber input) const
{
  //??? is the +1 here correct?
  TransitionIndex index = transducer.get_index(path.get_index()+input+1);
  
  if(index.matches(input))
  {
    // copy the path, follow the index, and handle the new transitions
    LookupPath& extended_path = *path.clone();
    extended_path.follow(index);
    try_transitions(new_paths, extended_path, input);
    delete &extended_path;
  }
}

void
LookupState::try_transitions(LookupPathVector& new_paths,
                               const LookupPath& path, 
                               const SymbolNumber input) const
{
  TransitionTableIndex transition_index;
  
  // if the path is pointing to the "state" entry before the transitions
  if(transducer.get_transition(path.get_index()).get_input() == NO_SYMBOL_NUMBER)
    transition_index = path.get_index()+1;
  else // the path is pointing directly to a transition
    transition_index = path.get_index();
  
  while(true)
  {
    const Transition& transition = transducer.get_transition(transition_index);
    
    if(transition.matches(input))
    {
      // copy the path, follow the transition, add the new path to the list
      LookupPath& extended_path = *path.clone();
      extended_path.follow(transition);
      new_paths.push_back(&extended_path);
    }
    else
      return;
    
    transition_index++;
  }
}


/////////Function definitions for SymbolIOStream

std::set<char> TokenIOStream::escaped_chars;

void
TokenIOStream::initialize_escaped_chars()
{
  escaped_chars.insert('[');
  escaped_chars.insert(']');
  escaped_chars.insert('{');
  escaped_chars.insert('}');
  escaped_chars.insert('^');
  escaped_chars.insert('$');
  escaped_chars.insert('/');
  escaped_chars.insert('\\');
  escaped_chars.insert('@');
  escaped_chars.insert('<');
  escaped_chars.insert('>');
}

std::string
TokenIOStream::read_utf8_char()
{
  unsigned short u8len = 0;
  int c = is.peek();
  if(is.eof())
    return "";
  
  if (c <= 127)
    u8len = 1;
  else if ( (c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16) )
    u8len = 4;
 else if ( (c & (128 + 64 + 32 )) == (128 + 64 + 32) )
    u8len = 3;
  else if ( (c & (128 + 64 )) == (128 + 64))
    u8len = 2;
  else
    stream_error();

  char next_u8[u8len+1];
  is.get(next_u8, u8len+1, '\0');
  next_u8[u8len] = '\0';
  
  return std::string(next_u8);
}

bool
TokenIOStream::is_space(const Token& t) const
{
  switch(t.type)
  {
    case Symbol:
      return isspace(alphabet.symbol_to_string(t.symbol).at(0));
    case Character:
      return isspace(t.character[0]);
    case Superblank:
      return true;
    default:
      return false;
  }
}

bool
TokenIOStream::is_alphabetic(const Token& t) const
{
  SymbolNumber s = to_symbol(t);
  if(s != 0 && s != NO_SYMBOL_NUMBER)
    return alphabet.is_alphabetic(s);
  
  switch(t.type)
  {
    case Character:
      return alphabet.is_alphabetic(t.character);
    default:
      return false;
  }
}

size_t
TokenIOStream::first_nonalphabetic(const TokenVector& s) const
{
  for(size_t i=0; i<s.size();i++)
  {
    if(!is_alphabetic(s[i]))
      return i;
  }
  
  return string::npos;
}

int
TokenIOStream::read_escaped()
{
  if(is.eof())
    stream_error();

  int c = is.get();
  
  if(c == EOF || escaped_chars.find(c) == escaped_chars.end())
    stream_error();
  
  return c;
}

std::string
TokenIOStream::read_delimited(const char delim)
{
  std::string result;
  int c = EOF;
  
  while(is && c != delim)
  {
    c = is.get();
    if(c == EOF)
      break;
    
    result += c;
    if(c == '\\')
      result += read_escaped();
  }

  if(c != delim)
    stream_error();

  return result;
}

Token
TokenIOStream::make_token()
{
  SymbolNumber s = alphabet.extract_symbol(is);
  if(s == 0) // EOF
    return Token();
  
  if(s != NO_SYMBOL_NUMBER)
    return Token(s);
  
  // the next thing in the stream is not a symbol
  // (extract_symbol moved the stream back to before anything was read)
  return Token(read_utf8_char().c_str());
}

Token
TokenIOStream::read_token()
{
  int next_char = is.peek();
  if(is.eof())
    return Token();
  
  if(escaped_chars.find(next_char) != escaped_chars.end())
  {
    switch(next_char)
    {
      case '[':
        superblank_bucket.push_back(read_delimited(']'));
        return Token(superblank_bucket.size()-1);
     
     case '\\':
       next_char = is.get(); // get the peeked char for real
       next_char = is.get();
       if(escaped_chars.find(next_char) == escaped_chars.end())
         stream_error();
       return make_token();
      
      default:
        stream_error();
    }
  }
  return make_token();
}

SymbolNumber
TokenIOStream::to_symbol(const Token& t) const
{
  switch(t.type)
  {
    case None:
      return NO_SYMBOL_NUMBER;
    case Symbol:
      return t.symbol;
    case Character:
    case Superblank:
    default:
      return is_space(t) ? alphabet.get_blank_symbol() : NO_SYMBOL_NUMBER;
  }
}

std::string
TokenIOStream::escape(const std::string& str) const
{
  std::string res = "";
  for(std::string::const_iterator it=str.begin(); it!=str.end(); it++)
  {
    if(escaped_chars.find(*it) != escaped_chars.end())
      res += '\\';
    res += *it;
  }
  return res;
}

Token
TokenIOStream::get_token()
{
  if(!token_buffer.isEmpty())
    return token_buffer.next();
  
  Token token = read_token();
  if(token.type != None)
    token_buffer.add(token);
  return token;
}

void
TokenIOStream::put_token(Token t)
{
  switch(t.type)
  {
    case Symbol:
      os << escape(alphabet.symbol_to_string(t.symbol));
      break;
    case Character:
      os << escape(t.character);
      break;
    case Superblank:
      os << superblank_bucket[t.superblank_index];
      break;
    default:
      if(printDebuggingInformationFlag)
        std::cerr << "Tried putting a None token" << std::endl;
      break;
  }
}

void
TokenIOStream::write_escaped(const TokenVector& t) 
{
  for(TokenVector::const_iterator it=t.begin(); it!=t.end(); it++)
    put_token(*it);
}


//////////Other lookup functions


void
print_word(TokenIOStream& token_stream,
           const TokenVector& surface_form, 
           std::string const &analyzed_forms)
{
//if the surface form contains any superblanks, they need to be printed to the
//output stream somewhere!

  if(printDebuggingInformationFlag)
    std::cout << "surface_form consists of " << surface_form.size() << " tokens" << std::endl;
  token_stream.ostream() << '^';
  token_stream.write_escaped(surface_form);
  token_stream.ostream() << analyzed_forms << '$';
}
void
print_unknown_word(TokenIOStream& token_stream,
                   const TokenVector& surface_form)
{
  token_stream.ostream() << '^';
  token_stream.write_escaped(surface_form);
  token_stream.ostream() << "/*";
  token_stream.write_escaped(surface_form);
  token_stream.ostream() << '$';
}


std::string
AbstractTransducer::process_finals(TokenIOStream& token_stream, const LookupPathVector& finals) const
{
  std::string res="";
  for(LookupPathVector::const_iterator it=finals.begin(); it!=finals.end(); it++)
  {
    res += '/'+token_stream.escape(alphabet.symbols_to_string((*it)->get_output_symbols()));
  }
  
  return res;
}

void AbstractTransducer::tokenize(TokenIOStream& token_stream)
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

void AbstractTransducer::run_lookup(TokenIOStream& token_stream)
{
  LookupState state(*this);
  size_t last_stream_location = 0;
  TokenVector surface_form;
  std::string analyzed_forms;
  
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
  	  analyzed_forms = process_finals(token_stream, finals);
  	  last_stream_location = token_stream.get_pos()-1;
  	  
  	  if(printDebuggingInformationFlag)
  	    std::cout << "Final paths found and saved, stream location is " << last_stream_location << std::endl;
  	}
  	
  	SymbolNumber next_symbol = token_stream.to_symbol(next_token);
  	
  	state.step(next_symbol);
    
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
        token_stream << next_token;
      }
      else if(analyzed_forms == "" || token_stream.is_alphabetic(token_stream.at(last_stream_location)))
      {
        if(token_stream.is_alphabetic(next_token))
        {
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
          token_stream << surface_form[0];
          token_stream.move_back(surface_form.size());
        }
        else
        {
          size_t word_length = token_stream.first_nonalphabetic(surface_form);
          if(word_length == string::npos)
            word_length = surface_form.size();
          
          if(printDebuggingInformationFlag)
            std::cout << "word_length=" << word_length << ", surface_form.size()=" << surface_form.size() 
                      << ", moving back " << surface_form.size()-word_length << " characters" << std::endl;
          
          print_unknown_word(token_stream,
                             TokenVector(surface_form.begin(),
                                         surface_form.begin()+word_length));
          token_stream.move_back(surface_form.size()-word_length);
        }
      }
      else // there are one or more valid tranductions
      {
        print_word(token_stream, 
                   TokenVector(surface_form.begin(), 
                               surface_form.end()-(token_stream.get_pos()-last_stream_location-1)),
                   analyzed_forms);
        token_stream.move_back(1);
      }
      
      state.reset();
      surface_form.clear();
      analyzed_forms = "";
    }
  }
  
  if(printDebuggingInformationFlag)
    std::cout << "Got None/EOF symbol; done." << std::endl;
  
  // print any valid transductions stored
  if(analyzed_forms != "" && token_stream.get_pos() == last_stream_location)
    print_word(token_stream, surface_form, analyzed_forms);
  std::cout << std::endl;
}

