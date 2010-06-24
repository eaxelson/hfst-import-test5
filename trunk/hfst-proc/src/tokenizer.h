#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include "hfst-proc.h"
#include "buffer.h"
#include "transducer.h"

class LetterTrie;
typedef std::vector<LetterTrie*> LetterTrieVector;
class Symbolizer;

class LetterTrie
{
 private:
  LetterTrieVector letters;
  SymbolNumberVector symbols;
  
  /**
   * Whether our symbols vector or that of any of our children contains 
   * symbol number 0
   */
  bool has_symbol_0() const;
 public:
  LetterTrie(void):
    letters(std::numeric_limits<unsigned char>::max(), (LetterTrie*) NULL),
    symbols(std::numeric_limits<unsigned char>::max(),NO_SYMBOL_NUMBER)
  {}
  
  LetterTrie(const LetterTrie& o): letters(), symbols(o.symbols)
  {
    for(LetterTrieVector::const_iterator it=o.letters.begin(); it!=o.letters.end(); it++)
      letters.push_back(((*it)==NULL) ? NULL : new LetterTrie(*(*it)));
  }
  
  ~LetterTrie()
  {
    for(LetterTrieVector::iterator it=letters.begin(); it!=letters.end(); it++)
      delete *it;
  }

  void add_string(const char * p,SymbolNumber symbol_key);

  SymbolNumber find_symbol(const char* c) const;
  
  /**
   * Read the next symbol from the stream. If the next character(s) do not form
   * a symbol, this version will put the characters back, so the stream is in
   * the same condition it was when this function was called
   * @return the number of the symbol, 0 for EOF, or NO_SYMBOL_NUMBER
   */
  SymbolNumber extract_symbol(std::istream& is) const;
  
  friend class Symbolizer;
};

class Symbolizer
{
 private:
  LetterTrie letters;
  SymbolNumberVector ascii_symbols;

  void read_input_symbols(const SymbolTable& st);

 public:
  Symbolizer(const SymbolTable& st):
    letters(), ascii_symbols(std::numeric_limits<unsigned char>::max(),NO_SYMBOL_NUMBER)
  {
    read_input_symbols(st);
    for(size_t i=0; i<ascii_symbols.size(); i++)
    {
      if(ascii_symbols[i] == 0)
      {
        ascii_symbols[i] = NO_SYMBOL_NUMBER;
        if(printDebuggingInformationFlag && i < 128)
          std::cout << "Symbolizer ignoring shortcut for ASCII character '" 
                    << (char)i << "' (" << i << ")" << std::endl;
      }
    }
    
    if(letters.has_symbol_0())
    {
      std::cerr << "!! Warning: the letter trie contains references to symbol  !!\n"
                << "!! number 0. This is almost certainly a bug and could      !!\n"
                << "!! cause certain characters to be misinterpreted as EOF    !!\n";
    }
  }
  
  SymbolNumber find_symbol(const char *c) const;
  SymbolNumber extract_symbol(std::istream& is) const;
};


/**
 * The recognized types of tokens.
 *
 * None - 'dummy' empty token, can represent EOF
 * Symbol - a known transducer symbol
 * Character - a (UTF-8) character not in the transducer alphabet
 * Superblank - an escaped string that is handled as a single blank character
 * ReservedCharacter - an Apertium reserved character not otherwise handled
 */
enum TokenType {None, Symbol, Character, Superblank, ReservedCharacter};

/**
 * A structure representing a stream token. It can be of various types and
 * uses a union to conserve memory
 */
struct Token
{
  TokenType type;
  union
  {
    SymbolNumber symbol;
    char character[5];
    unsigned int superblank_index; // see TokenIOStream::superblank_bucket
  };
  
  Token(): type(None), symbol(0) {}
  
  void set_none() {type=None;}
  void set_symbol(SymbolNumber s) {type=Symbol; symbol=s;}
  void set_character(const char* c)
  {type=Character; strncpy(character,c,4); character[4]='\0';}
  void set_character(char c) {type=Character; character[0]=c; character[1]='\0';}
  void set_superblank(unsigned int i) {type=Superblank; superblank_index=i;}
  void set_reservedcharacter(char c) {type=ReservedCharacter; character[0]=c; character[1]='\0';}
  
  static Token as_symbol(SymbolNumber s) {Token t; t.set_symbol(s); return t;}
  static Token as_character(const char* c) {Token t; t.set_character(c); return t;}
  static Token as_character(char c) {Token t; t.set_character(c); return t;}
  static Token as_superblank(unsigned int i) {Token t; t.set_superblank(i); return t;}
  static Token as_reservedcharacter(char c) {Token t; t.set_reservedcharacter(c); return t;}
  
  bool operator<(const Token& rhs) const
  {
    if(type != rhs.type)
      return type < rhs.type;
    switch(type)
    {
      case Symbol:
        return symbol < rhs.symbol;
      case Character:
      case ReservedCharacter:
        return strcmp(character, rhs.character) < 0;
      case Superblank:
        return superblank_index < rhs.superblank_index;
      case None:
      default:
        return false;
    }
  }
};

/**
 * Wrapper class around an istream and an ostream for reading and writing
 * tokens, with additional buffering functionality. Input and output
 * are combined here for superblank functionality
 */
class TokenIOStream
{
  /**
   * The set of characters that need to be backslash-escaped in the stream
   */
  static std::set<char> escaped_chars;
  static void initialize_escaped_chars();
  
  std::istream& is;
  std::ostream& os;
  const TransducerAlphabet& alphabet;
  bool null_flush;
  
  Symbolizer symbolizer;
  
  /**
   * All superblanks found in the input stream all stored here, and are
   * indexed by Token objects
   */
  std::vector<std::string> superblank_bucket;
  
  Buffer<Token> token_buffer;
  
  void do_null_flush();
  
  /**
   * Reads a UTF-8 char (1-4 bytes) from the input stream, returning it as a
   * character string
   */
  std::string read_utf8_char();
  
  /**
   * Called after a backslash has been found in the stream to read an escaped
   * character. Fails on stream error or if the next character isn't a proper
   * escaped character
   */
  int read_escaped();
  
  /**
   * Generate an escaped copy of the given string
   */
  std::string escape(const std::string& str) const;
  
  /**
   * Make a token from the next character(s) in the stream by attempting to
   * get a symbol, and reverting to reading a character if that fails
   */
  Token make_token();
  
  /**
   * Read the next token in the stream, handling escaped characters
   */
  Token read_token();
 public:
  TokenIOStream(std::istream& i, std::ostream& o, const TransducerAlphabet& a,
                bool flush):
    is(i), os(o), alphabet(a), null_flush(flush), 
    symbolizer(a.get_symbol_table()), superblank_bucket(), token_buffer(1024)
  {
    if(printDebuggingInformationFlag)
      std::cout << "Creating TokenIOStream" << std::endl;
    if(escaped_chars.size() == 0)
      initialize_escaped_chars();
  }
  
  size_t get_pos() const {return token_buffer.getPos();}
  size_t diff_prev(size_t pos) const {return token_buffer.diffPrevPos(pos);}
  Token at(size_t pos) const {return token_buffer.get(pos);}
  void move_back(size_t count) {token_buffer.back(count);}
  
  const TransducerAlphabet& get_alphabet() const {return alphabet;}
  
  bool is_space(const Token& t) const;
  bool is_alphabetic(const Token& t) const;
  
  /**
   * Get a symbol representation of the token. The conversion depends on the
   * token type
   * 
   * None - NO_SYMBOL_NUMBER
   * Symbol - the unmodified symbol
   * Character - blank_symbol if is_space returns true for the character,
   *             otherwise NO_SYMBOL_NUMBER
   * Superblank - blank_symbol
   * ReservedCharacter - NO_SYMBOL_NUMBER
   */
  SymbolNumber to_symbol(const Token& t) const;
  SymbolNumberVector to_symbols(const TokenVector& t) const;
  
  /**
   * Calculate the capitalization properties of the given word, which should
   * contain all symbols
   */
  CapitalizationState get_capitalization_state(const TokenVector& tokens) const;
  
  size_t first_nonalphabetic(const TokenVector& s) const;
  
  /**
   * Read into the the stream until the delimiting character is found. The
   * delimiting character is read and included in the string. Charater escaping
   * is handled. Fails on stream error
   * @return the string from the stream's current point up to and including
   *         the delimiting character
   */
  std::string read_delimited(const char delim);
  
  /**
   * Read the next token from the input stream/buffer
   */
  Token get_token();
  
  /**
   * Write a token to the output stream
   */
  void put_token(const Token& t);
  
  void put_tokens(const TokenVector& t);
  void put_symbols(const SymbolNumberVector& s, CapitalizationState caps=Unknown);
  
  /**
   * Get the string representation of the given token
   * @param raw if true, then don't do any character escaping
   */
  std::string token_to_string(const Token& t, bool raw=false) const;
  
  std::string tokens_to_string(const TokenVector& t, bool raw=false) const;
    
  /**
   * Read the next token from the input stream/buffer
   */
  TokenIOStream& operator>>(Token& t) {t=get_token(); return *this;}
  
  /**
   * Write the string representation of a token to the output stream
   */
  TokenIOStream& operator<<(const Token& t) {put_token(t); return *this;}
  
  std::ostream& ostream() {return os;}
  
  void write_escaped(const std::string str) {os << escape(str);}
  void write_escaped(const TokenVector& t) {os << tokens_to_string(t);}
  
  std::string get_superblank(size_t i) const {return superblank_bucket[i];}
};

#endif
