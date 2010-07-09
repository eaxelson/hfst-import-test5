#include <cstdlib>
#include "tokenizer.h"
#include "transducer.h"

//////////Function definitions for LetterTrie

bool
LetterTrie::has_symbol_0() const
{
  for(size_t i=0;i<symbols.size();i++)
  {
    if(symbols[i] == 0)
      return true;
  }
  for(size_t i=0;i<letters.size();i++)
  {
    if(letters[i] != NULL && letters[i]->has_symbol_0())
      return true;
  }
  return false;
}

void
LetterTrie::add_string(const char * p, SymbolNumber symbol_key)
{
  if (*(p+1) == 0)
  {
    symbols[(unsigned char)(*p)] = symbol_key;
    return;
  }

  if (letters[(unsigned char)(*p)] == NULL)
    letters[(unsigned char)(*p)] = new LetterTrie();
  letters[(unsigned char)(*p)]->add_string(p+1,symbol_key);
}

SymbolNumber
LetterTrie::find_symbol(const char* c) const
{
  if(strlen(c) == 1)
    return symbols[(unsigned char)(c[0])];
  
  if(letters[(unsigned char)(c[0])] == NULL)
    return NO_SYMBOL_NUMBER;
  else
    return letters[(unsigned char)(c[0])]->find_symbol(c+1);
}

SymbolNumber
LetterTrie::extract_symbol(std::istream& is) const
{
  int c = is.get();
  if(c == EOF)
    return 0;
    
  if(letters[c] == NULL)
  {
    if(symbols[c] == NO_SYMBOL_NUMBER)
      is.putback(c);
    return symbols[c];
  }
  
  SymbolNumber s = letters[c]->extract_symbol(is);
  if(s == NO_SYMBOL_NUMBER)
  {
    if(symbols[c] == NO_SYMBOL_NUMBER)
      is.putback(c);
    return symbols[c];
  }
  return s;
}


//////////Function definitions for Symbolizer

void
Symbolizer::add_symbol(const SymbolProperties& symbol)
{
  std::string p = symbol.str;
  
  if(p.length() > 0)
  {
    unsigned char first = p.at(0);
    if(ascii_symbols[first] != 0)
    { // if the symbol's first character is ASCII and we're not ignoring it yet
      if(p.length() == 1)
        ascii_symbols[first] = symbol_count;
      else
        ascii_symbols[first] = 0;
    }
    letters.add_string(p.c_str(),symbol_count);
  }
  symbol_count++;
}

void
Symbolizer::add_symbols(const SymbolTable& st)
{
  for (SymbolNumber k = 0; k < st.size(); ++k)
    add_symbol(st[k]);
}

SymbolNumber
Symbolizer::find_symbol(const char* c) const
{
  if(c[0] == 0)
    return NO_SYMBOL_NUMBER;
  if(strlen(c) > 1 ||
     ascii_symbols[(unsigned char)(c[0])] == NO_SYMBOL_NUMBER ||
     ascii_symbols[(unsigned char)(c[0])] == 0)
    return letters.find_symbol(c);
  return ascii_symbols[(unsigned char)(c[0])];
}

SymbolNumber
Symbolizer::extract_symbol(std::istream& is) const
{
  int c = is.peek();
  if(c == 0)
    return NO_SYMBOL_NUMBER;
  if(ascii_symbols[c] == NO_SYMBOL_NUMBER ||
     ascii_symbols[c] == 0)
    return letters.extract_symbol(is);
  
  return ascii_symbols[is.get()];
}


//////////Function definitions for TokenIOStream

std::set<char> TokenIOStream::escaped_chars;

TokenIOStream::TokenIOStream(std::istream& i, std::ostream& o, 
                             const TransducerAlphabet& a, bool flush):
  is(i), os(o), alphabet(a), null_flush(flush), 
  symbolizer(a.get_symbolizer()), superblank_bucket(), token_buffer(1024)
{
  if(printDebuggingInformationFlag)
    std::cout << "Creating TokenIOStream" << std::endl;
  if(escaped_chars.size() == 0)
    initialize_escaped_chars();
}

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

void
TokenIOStream::do_null_flush()
{
  os.flush();
  if(os.bad())
    std::cerr << "Could not flush file" << std::endl;
}

CapitalizationState
TokenIOStream::get_capitalization_state(const TokenVector& tokens) const
{
  if(tokens.size() == 0)
    return Unknown;
  
  const Token& first=tokens[0], last=tokens.size()>1?tokens[tokens.size()-1]:tokens[0];
  
  if(first.type != Symbol || last.type != Symbol)
    return Unknown;
  if(alphabet.is_lower(first.symbol) && alphabet.is_lower(last.symbol))
    return LowerCase;
  if(alphabet.is_upper(first.symbol) && alphabet.is_lower(last.symbol))
    return FirstUpperCase;
  if(alphabet.is_upper(first.symbol) && alphabet.is_upper(last.symbol))
    return UpperCase;
  return Unknown;
}

std::string
TokenIOStream::read_utf8_char()
{
  return read_utf8_char(is);
}

std::string
TokenIOStream::read_utf8_char(std::istream& is)
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
    stream_error("Invalid UTF-8 character found");

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
  int c = is.get();
  
  if(c == EOF || escaped_chars.find(c) == escaped_chars.end())
    stream_error("Found invalid character after backslash");
  
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
    if(null_flush && c == '\0')
      do_null_flush();
  }

  if(c != delim)
    stream_error(std::string("Didn't find delimiting character ")+delim);

  return result;
}

Token
TokenIOStream::make_token()
{
  SymbolNumber s = symbolizer.extract_symbol(is);
  if(s == 0) // EOF
    return Token();
  
  if(s != NO_SYMBOL_NUMBER)
    return Token::as_symbol(s);
  
  // the next thing in the stream is not a symbol
  // (extract_symbol moved the stream back to before anything was read)
  std::string ch = read_utf8_char();
  if(null_flush && ch == "")
    do_null_flush();
  return (escaped_chars.find(ch[0]) == escaped_chars.end()) ?
    Token::as_character(ch.c_str()) :
    Token::as_reservedcharacter(ch[0]);
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
        return Token::as_superblank(superblank_bucket.size()-1);
      
      case '\\':
        next_char = is.get(); // get the peeked char for real
        return Token::as_character(read_escaped());
      
      case '<':
      {
        Token t = make_token();
        if(t.type == Symbol && alphabet.is_tag(t.symbol)) // the '<' introduced a tag, that's fine
          return t;
        return Token::as_reservedcharacter('<');
      }
      
      default:
        return Token::as_reservedcharacter((char)is.get());
    }
  }
  return make_token();
}

SymbolNumber
TokenIOStream::to_symbol(const Token& t) const
{
  switch(t.type)
  {
    case Symbol:
      return t.symbol;
    case Superblank:
      return alphabet.get_blank_symbol();
    case None:
    case Character:
    case ReservedCharacter:
    default:
      return NO_SYMBOL_NUMBER;
  }
}
SymbolNumberVector
TokenIOStream::to_symbols(const TokenVector& t) const
{
  SymbolNumberVector res;
  for(TokenVector::const_iterator it=t.begin(); it!=t.end(); it++)
    res.push_back(to_symbol(*it));
  return res;
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
TokenIOStream::put_token(const Token& t)
{
    os << token_to_string(t);
}

void
TokenIOStream::put_tokens(const TokenVector& t)
{
  for(TokenVector::const_iterator it=t.begin(); it!=t.end(); it++)
    put_token(*it);
}
void
TokenIOStream::put_symbols(const SymbolNumberVector& s, CapitalizationState caps)
{
  os << alphabet.symbols_to_string(s, caps);
}

std::string
TokenIOStream::token_to_string(const Token& t, bool raw) const
{
  switch(t.type)
  {
    case Symbol:
      return alphabet.symbol_to_string(t.symbol);
    case Character:
      if(raw)
        return t.character;
      else
        return escape(t.character);
    case Superblank:
      return superblank_bucket[t.superblank_index];
    case ReservedCharacter:
      return t.character;
    default:
      return "";
  }
}

std::string
TokenIOStream::tokens_to_string(const TokenVector& t, bool raw) const
{
  std::string res;
  for(TokenVector::const_iterator it=t.begin(); it!=t.end(); it++)
    res += token_to_string(*it,raw);
  return res;
}

