#ifndef _HFST_PROC_EXTRA_H_
#define _HFST_PROC_EXTRA_H_

#include "hfst-optimized-lookup.h"
#include "buffer.h"

inline bool indexes_transition_table(const TransitionTableIndex i)
{
  return i >= TRANSITION_TARGET_TABLE_START;
}
inline bool indexes_transition_index_table(const TransitionTableIndex i)
{
  return i < TRANSITION_TARGET_TABLE_START;
}

/**
 * Represents a (possibly partial) path through a transducer. This is used
 * during lookup for storing lookup paths that are continued as input symbols
 * are provided.
 */
class LookupPath
{
 protected:
  /**
   * Points to the state in the transition index table or the transition table
   * where the path ends. This follows the normal semantics whereby values less
   * than TRANSITION_TARGET_TABLE_START reference the transition index table
   * while values greater than or equal to TRANSITION_TARGET_TABLE_START
   * reference the transition table
   */
  TransitionTableIndex index;
  
  /**
   * Whether the path ends at a final state
   */
  bool final;
  
  /**
   * The output symbols of the transitions this path has followed
   */
  SymbolNumberVector output_symbols;
  
 public:
  LookupPath(const TransitionTableIndex initial): index(initial) {}
  
  LookupPath(const LookupPath& o):
    index(o.index), output_symbols(o.output_symbols) {}
  
  virtual ~LookupPath() {}
  
  virtual LookupPath* clone() const {return new LookupPath(*this);}
  
  /**
   * Follow the transition index, modifying our index
   * @param index the index to follow
   */
  virtual void follow(const TransitionIndex& index);
  
  /**
   * Follow the transition, appending the output symbol
   * @param transition the transition to follow
   * @true if following the transition succeeded. This can fail because of
   *       e.g. flag diacritics in some subclasses
   */
  virtual bool follow(const Transition& transition);
  
  TransitionTableIndex get_index() const {return index;}
  bool at_final() const {return final;}
  SymbolNumberVector get_output_symbols() const {return output_symbols;}
};

/**
 * A lookup path which additionally stores the state of the flag diacritics on
 * the path
 */
class LookupPathFd : public LookupPath
{
 protected:
  /**
   * The values of the flag diacritic features at the end of the path
   */
  FlagDiacriticState fd_state;
  
  /**
   * A pointer to the list flag diacritic operations in the transducer where
   * the lookup is being done
   */
  const OperationVector* fd_operations;
  
  /**
   * Evaluates the given flag diacritic operation, possibly modifying fd_state,
   * and returning whether the operation is allowed or not
   */
  bool evaluate_flag_diacritic(FlagDiacriticOperation op);
  
 public:
  LookupPathFd(const TransitionTableIndex initial, const OperationVector& op):
    LookupPath(initial), fd_operations(&op) {}
  LookupPathFd(const LookupPathFd& o): LookupPath(o), fd_state(o.fd_state), 
                                       fd_operations(o.fd_operations) {}
  
  virtual LookupPath* clone() const {return new LookupPathFd(*this);}
  
  /**
   * As in the superclass, except also modifies the flag diacritic state if
   * appropriate, and checks whether or not the follow action is allowed
   */
  virtual bool follow(const Transition& transition);
  
  FlagDiacriticState get_fd_state() {return fd_state;}
};

/**
 * A lookup path which additionally stores the summed weight of the path
 */
class LookupPathW : public LookupPath
{
 protected:
  /**
   * The summed weight of the transitions this path has followed
   */
  Weight weight;
  
  /**
   * An extra weight that is added to the sum when the path is at a final state
   */
  Weight final_weight;
  
  void follow_weight(const TransitionIndex& index);
  void follow_weight(const Transition& transition);
 public:
  LookupPathW(const TransitionTableIndex initial): 
  	LookupPath(initial), weight(0.0f), final_weight(0.0f) {}
  LookupPathW(const LookupPathW& o): LookupPath(o), weight(o.weight),
    final_weight(o.final_weight) {}
  
  virtual LookupPath* clone() const {return new LookupPathW(*this);}

  /**
   * As in the superclass, except also modifying weight
   */
  virtual void follow(const TransitionIndex& index);
  /**
   * As in the superclass, except also modifying weight
   **/  
  virtual bool follow(const Transition& transition);
  
  Weight get_weight() const {return at_final() ? weight+final_weight : weight;}
};

/**
 * A lookup path with weight and flag diacritics
 */
class LookupPathWFd : public LookupPathFd
{
 protected:
  /**
   * The summed weight of the transitions this path has followed
   */
  Weight weight;
  
  /**
   * An extra weight that is added to the sum when the path is at a final state
   */
  Weight final_weight;
  
  void follow_weight(const TransitionIndex& index);
  void follow_weight(const Transition& transition);
 public:
  LookupPathWFd(const TransitionTableIndex initial, const OperationVector& op):
    LookupPathFd(initial, op), weight(0.0f), final_weight(0.0f)  {}
  LookupPathWFd(const LookupPathWFd& o): LookupPathFd(o), weight(o.weight),
    final_weight(o.final_weight) {}
  
  virtual LookupPath* clone() const {return new LookupPathWFd(*this);}
  
  /**
   * As in the superclass, except also modifying weight and doing Fd's
   */
  virtual void follow(const TransitionIndex& index);
  /**
   * As in the superclass, except also modifying weight and doing Fd's
   **/  
  virtual bool follow(const Transition& transition);
  
  Weight get_weight() const {return at_final() ? weight+final_weight : weight;}
};



/**
 * Represents the current state of a lookup operation
 */
class LookupState
{
 private:
  /**
   * The transducer in which the lookup is occurring
   */
  const AbstractTransducer& transducer;
  
  /**
   * The active paths in a lookup operation. At the start of a lookup this will
   * contain one path. The lookup has failed if it is ever empty.
   */
  LookupPathVector paths;
  
  
  /**
   * Delete all active paths and clear the list
   */
  void clear_paths();
  
  /**
   * Add a new path to the list of active paths
   * @param path the path to add
   */
  void add_path(LookupPath& path);
  
  /**
   * Get rid of the current paths and replace the list with a new list. Note
   * that the new list is passed by value
   * @param new_paths the new list of paths to store in the lookup state
   */
  void replace_paths(LookupPathVector new_paths);
  
  
  /**
   * Setup the state with a single initial path ending at starting state and
   * try epsilons at the initial position
   * @param initial an initial path to start the lookup
   */
  void init(LookupPath* initial);
  
  
  /**
   * Find any paths in the state that point to epsilon indices or epsilon
   * transitions, and generate additional paths by following the epsilons.
   * Because the additional paths are appended as they are generated, any
   * new epsilons generated by the epsilons will be properly handled
   */
  void try_epsilons();
  
  /**
   * If the given path points to a place in the index table with an epsilon
   * index, generate an additional path by following it
   * @param path a path pointing to the transition index table
   */
  void try_epsilon_index(const LookupPath& path);
  
  /**
   * If the given path points to one or more epsilon transitions, generate
   * additional paths by following them
   * @param path a path pointing to the beginning of a state in the transition
   *             table or directly to transitions
   */
  void try_epsilon_transitions(const LookupPath& path);
  
  
  /**
   * Find any paths in the state that have an index or transitions that match
   * the input symbol and generate a new set of paths by following the index
   * and/or transitions
   * @param input the input symbol to apply
   */
  void apply_input(const SymbolNumber input);
  
  /**
   * If the given path points to a place in the index table with an index for
   * the given input symbol, call try_transitions after having the path follow
   * the index
   * @param new_paths any new paths generated get appended here
   * @param path a path pointing to the transition index table
   * @param input the input symbol to look up in the transition index table
   */
  void try_index(LookupPathVector& new_paths, 
                 const LookupPath& path, const SymbolNumber input) const;
  
  /**
   * If the given path points to one or more transitions whose inputs match
   * the given input symbol, generate new paths by following them
   * @param new_paths any new paths generated get appended here
   * @param path a path pointing to the beginning of a state in the transition
   *             table or directly to transitions
   * @param input the input symbol to look up in the transition table
   */
  void try_transitions(LookupPathVector& new_paths,
                       const LookupPath& path, const SymbolNumber input) const;
  
 public:
  /**
   * Set up a new lookup state, initializing it for doing a lookup in the
   * given transducer
   * @param t the transducer in which the lookup will occur
   */
  LookupState(const AbstractTransducer& t): transducer(t)
  {
    reset();
  }
  
  LookupState(const LookupState& o): transducer(o.transducer)
  {
    for(LookupPathVector::const_iterator it=o.paths.begin(); it!=o.paths.end(); it++)
      paths.push_back((*it)->clone());
  }
  
  ~LookupState()
  {
    clear_paths();
  }
  
  /**
   * Clear any current lookup paths and prepare it for a new lookup
   */
  void reset()
  {
    init(transducer.get_initial_path());
  }
  
  /**
   * Determine whether there are any active paths
   * @true if there are any active paths
   */
  bool is_active() const
  {
    return !paths.empty();
  }
  
  int num_active() {return paths.size();}
  
  /**
   * Determine whether any active paths are at a final state
   * @true if an active path is at a final state
   */
  bool is_final() const;
  
  /**
   * Get a list of active paths that are at a final state
   * @return the active paths that are at a final state
   */
  const LookupPathVector get_finals() const;
  
  
  /**
   * Apply a new input symbol to the state's active paths, and then follow any
   * epsilon transitions. If NO_SYMBOL_NUMBER is given, it has the effect of
   * "killing" all active paths
   * @param input the new input symbol
   */
  void step(const SymbolNumber input);
};


/**
 * The recognized types of tokens.
 *
 * None - 'dummy' empty token, can represent EOF
 * Symbol - a known transducer symbol
 * Character - a (UTF-8) character not in the transducer alphabet
 * Superblank - an escaped string that is handled as a single blank character
 */
enum TokenType {None, Symbol, Character, Superblank};

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
  
  Token(): type(None) {}
  Token(SymbolNumber s): type(Symbol), symbol(s) {}
  Token(const char* c): type(Character)
  { strncpy(character, c, 4); character[4]='\0'; }
  Token(unsigned int i): type(Superblank), superblank_index(i) {}
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
  
  /**
   * All superblanks found in the input stream all stored here, and are
   * indexed by Token objects
   */
  std::vector<std::string> superblank_bucket;
  
  Buffer<Token> token_buffer;
  
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
   * Read into the the stream until the delimiting character is found. The
   * delimiting character is read and included in the string. Charater escaping
   * is handled. Fails on stream error
   * @return the string from the stream's current point up to and including
   *         the delimiting character
   */
  std::string read_delimited(const char delim);
  
  /**
   * Make a token from the next character(s) in the stream by attempting to
   * get a symbol, and reverting to reading a character if that fails
   */
  Token make_token();
  
  /**
   * Read the next token in the stream, handling escaped characters
   */
  Token read_token();
  
  void stream_error() const 
  {
    throw std::ios_base::failure("Error: malformed input stream");
  }
 public:
  TokenIOStream(std::istream& i, std::ostream& o, const TransducerAlphabet& a):
    is(i), os(o), alphabet(a)
  {
    if(escaped_chars.size() == 0)
      initialize_escaped_chars();
  }
  
  size_t get_pos() const {return token_buffer.getPos();}
  Token at(size_t pos) const {return token_buffer.get(pos);}
  void move_back(size_t count) {token_buffer.back(count);}
  
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
   */
  SymbolNumber to_symbol(const Token& t) const;
  
  size_t first_nonalphabetic(const TokenVector& s) const;
  
  /**
   * Generate an escaped copy of the given string
   */
  std::string escape(const std::string& str) const;
  
  /**
   * Read the next token from the input stream/buffer
   */
  Token get_token();
  
  /**
   * Write a token to the output stream
   */
  void put_token(Token t);
  
  /**
   * Read the next token from the input stream/buffer
   */
  TokenIOStream& operator>>(Token& t) {t=get_token(); return *this;}
  
  /**
   * Write the string representation of a token to the output stream
   */
  TokenIOStream& operator<<(const Token& t) {put_token(t); return *this;}
  
  std::istream& istream() {return is;}
  std::ostream& ostream() {return os;}
  
  void write_escaped(std::string& str) {os << escape(str);}
  void write_escaped(const TokenVector& t);
  
  std::string get_superblank(size_t i) const {return superblank_bucket[i];}
};

#endif
