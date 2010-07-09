#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <fstream>
#include <cstdlib>
#include "hfst-proc.h"
#include "tokenizer.h"

class TransducerHeader
{
 private:
  SymbolNumber number_of_input_symbols;
  SymbolNumber number_of_symbols;
  TransitionTableIndex size_of_transition_index_table;
  TransitionTableIndex size_of_transition_target_table;

  StateIdNumber number_of_states;
  TransitionNumber number_of_transitions;

  bool weighted;
  bool deterministic;
  bool input_deterministic;
  bool minimized;
  bool cyclic;
  bool has_epsilon_epsilon_transitions;
  bool has_input_epsilon_transitions;
  bool has_input_epsilon_cycles;
  bool has_unweighted_input_epsilon_cycles;

  template<class T>
  static T read_property(std::istream& is)
  {
    T p;
    is.read(reinterpret_cast<char*>(&p), sizeof(T));
    return p;
  }
  static bool read_bool_property(std::istream& is)
  {
    unsigned int prop;
    is.read(reinterpret_cast<char*>(&prop), sizeof(unsigned int));
    return (prop != 0);
  }
 public:
  TransducerHeader(std::istream& is):
    number_of_input_symbols(read_property<SymbolNumber>(is)),
    number_of_symbols(read_property<SymbolNumber>(is)),
    size_of_transition_index_table(read_property<TransitionTableIndex>(is)),
    size_of_transition_target_table(read_property<TransitionTableIndex>(is)),
    number_of_states(read_property<StateIdNumber>(is)),
    number_of_transitions(read_property<TransitionNumber>(is)),
    weighted(read_bool_property(is)),
    deterministic(read_bool_property(is)),
    input_deterministic(read_bool_property(is)),
    minimized(read_bool_property(is)),
    cyclic(read_bool_property(is)),
    has_epsilon_epsilon_transitions(read_bool_property(is)),
    has_input_epsilon_transitions(read_bool_property(is)),
    has_input_epsilon_cycles(read_bool_property(is)),
    has_unweighted_input_epsilon_cycles(read_bool_property(is)) {}

  SymbolNumber symbol_count(void) const { return number_of_symbols; }
  SymbolNumber input_symbol_count(void) const {return number_of_input_symbols;}
  
  TransitionTableIndex index_table_size(void) const
  { return size_of_transition_index_table; }
  TransitionTableIndex target_table_size(void) const
  { return size_of_transition_target_table; }

  bool probe_flag(HeaderFlag flag) const
  {
    switch (flag) {
    case Weighted: return weighted;
    case Deterministic: return deterministic;
    case Input_deterministic: return input_deterministic;
    case Minimized: return minimized;
    case Cyclic: return cyclic;
    case Has_epsilon_epsilon_transitions: return has_epsilon_epsilon_transitions;
    case Has_input_epsilon_transitions: return has_input_epsilon_transitions;
    case Has_input_epsilon_cycles: return has_input_epsilon_cycles;
    case Has_unweighted_input_epsilon_cycles: return has_unweighted_input_epsilon_cycles;
    }
    return false;
  }
  
  void print()
  {
    std::cout << "Transducer properties:" << std::endl
              << " number_of_symbols: " << number_of_symbols << std::endl
              << " number_of_input_symbols: " << number_of_input_symbols << std::endl
              << " size_of_transition_index_table: " << size_of_transition_index_table << std::endl
              << " size_of_transition_target_table: " << size_of_transition_target_table << std::endl

              << " number_of_states: " << number_of_states << std::endl
              << " number_of_transitions: " << number_of_transitions << std::endl

              << " weighted: " << weighted << std::endl
              << " deterministic: " << deterministic << std::endl
              << " input_deterministic: " << input_deterministic << std::endl
              << " minimized: " << minimized << std::endl
              << " cyclic: " << cyclic << std::endl
              << " has_epsilon_epsilon_transitions: " << has_epsilon_epsilon_transitions << std::endl
              << " has_input_epsilon_transitions: " << has_input_epsilon_transitions << std::endl
              << " has_input_epsilon_cycles: " << has_input_epsilon_cycles << std::endl
              << " has_unweighted_input_epsilon_cycles: " << has_unweighted_input_epsilon_cycles << std::endl;
  }
};

class FlagDiacriticOperation
{
 private:
  FlagDiacriticOperator operation;
  SymbolNumber feature;
  ValueNumber value;
 public:
 FlagDiacriticOperation(FlagDiacriticOperator op, SymbolNumber feat, ValueNumber val):
  operation(op), feature(feat), value(val) {}

  // dummy constructor
 FlagDiacriticOperation():
  operation(P), feature(NO_SYMBOL_NUMBER), value(0) {}
  
  bool isFlag(void) const { return feature != NO_SYMBOL_NUMBER; }
  FlagDiacriticOperator Operation(void) const { return operation; }
  SymbolNumber Feature(void) const { return feature; }
  ValueNumber Value(void) const { return value; }

#if OL_FULL_DEBUG
  void print(void)
  {
    std::cout << operation << "\t" << feature << "\t" << value << std::endl;
  }
#endif
};

typedef std::vector<ValueNumber> FlagDiacriticState;

/**
 * Various properties of a symbol, mostly used internally by TransducerAlphabet
 */
struct SymbolProperties
{
  /**
   * The string representation of the symbol
   */
  std::string str;
  
  /**
   * Whether the symbol is considered alphabetic. Roughly, this includes
   * symbols that are not whitespace or punctuation
   */
  bool alphabetic;
  
  /**
   * The symbol number of the lowercase version of the symbol. If the symbol
   * is already lowercase, this will contain the symbol's own number. If the
   * symbol has no lowercase form, it will contain NO_SYMBOL_NUMBER
   */
  SymbolNumber lower;
  
  /**
   * The symbol number of the uppercase version of the symbol. If the symbol
   * is already uppercase, this will contain the symbol's own number. If the
   * symbol has no uppercase form, it will contain NO_SYMBOL_NUMBER
   */
  SymbolNumber upper;
  
  /**
   * Flag diacritic information associated with the symbol. This may be a dummy
   * operation if the symbol is not a flag diacritic
   */
  FlagDiacriticOperation fd_op;
};

class Symbolizer;

class TransducerAlphabet
{
 private:
  SymbolTable symbol_table;
  
  Symbolizer symbolizer;
  
  /**
   * The symbol number for a "blank" which is here considered to be a space.
   * This symbol requires special handling because it doubles as the 
   * symbolic representation for a superblank block of text
   */
  SymbolNumber blank_symbol;
  
  SymbolProperties get_next_symbol(std::istream& is);
  
  /**
   * Find the upper/lower-case equivalencies for the symbols in the table
   */
  void calculate_caps();
  
  /**
   * A routine used for checking/changing a character's case
   * @param c the character to work with
   * @param case_res an output: set to <0 if c is lowercase, >0 if c is
   *                            uppercase, or 0 if neither
   * @return the equivalent character in the opposite case of c, or the empty
   *         string if nonexistent
   */
  static std::string caps_helper(const char* c, int& case_res);
  static std::string caps_helper_single(const char* c, int& case_res);
  static int utf8_str_to_int(const char* c);
  static std::string utf8_int_to_str(int c);
  
  /**
   * Store the blank symbol's number in blank_symbol, adding it as a new symbol
   * if it cannot be found
   */
  void setup_blank_symbol();
  
  /**
   * Check for and warn about the symbol table containing multi-char symbols
   * consisting ASCII characters that are available individually elsewhere in
   * the table
   */
  void check_for_overlapping() const;
  
  void print_table() const;
  
  void add_symbol(const SymbolProperties& symbol);
  
  std::map<std::string, SymbolNumber> feature_bucket;
  std::map<std::string, ValueNumber> value_bucket;
  ValueNumber val_num;
  SymbolNumber feat_num;
  
 public:
  TransducerAlphabet(std::istream& is, SymbolNumber symbol_count);
  TransducerAlphabet(const TransducerAlphabet& o);
  
  ~TransducerAlphabet();
  
  const SymbolTable& get_symbol_table(void) const { return symbol_table; }
  const Symbolizer& get_symbolizer(void) const;
  SymbolNumber get_state_size(void) const { return feature_bucket.size(); }
  SymbolNumber get_blank_symbol() const {return blank_symbol;}
  
  bool is_punctuation(const char* c) const;
  
  bool is_alphabetic(const char* c) const
  { return (c[0]!='\0' && !isspace(c[0]) && 
            !is_punctuation(c)); }
  bool is_alphabetic(SymbolNumber symbol) const 
  {return symbol_table[symbol].alphabetic;}
  
  bool is_lower(SymbolNumber symbol) const
  {return symbol_table[symbol].lower == symbol;}
  bool is_upper(SymbolNumber symbol) const
  {return symbol_table[symbol].upper == symbol;}
  bool has_case(SymbolNumber symbol) const
  {return symbol_table[symbol].lower != NO_SYMBOL_NUMBER ||
          symbol_table[symbol].upper != NO_SYMBOL_NUMBER;}
  /**
   * Returns the lowercase equivalent of symbol, or just symbol if there is no
   * lowercase equivalent
   */
  SymbolNumber to_lower(SymbolNumber symbol) const
  {return symbol_table[symbol].lower==NO_SYMBOL_NUMBER ? 
            symbol : symbol_table[symbol].lower;}
  
  /**
   * Returns the uppercase equivalent of symbol, or just symbol if there is no
   * uppercase equivalent
   */
  SymbolNumber to_upper(SymbolNumber symbol) const
  {return symbol_table[symbol].upper==NO_SYMBOL_NUMBER ? 
            symbol : symbol_table[symbol].upper;}
  
  /**
   * Whether the symbol is an apertium-style tag (i.e. symbols starting 
   * with < and ending with > ) 
   */
  bool is_tag(SymbolNumber symbol) const;
  
  /**
   * Whether the symbol marks a compound boundary (+ or #)
   */
  bool is_compound_boundary(SymbolNumber symbol) const;
  int num_compound_boundaries(const SymbolNumberVector& symbols) const;
  
  std::string symbol_to_string(SymbolNumber symbol) const
  {return symbol_table[symbol].str;}
  
  /**
   * Use the symbol table to convert the given symbols into a string, optionally
   * modifying the case of some symbols
   * @param symbols the symbols to convert
   * @param caps how to modify case. The states are handled as follows:
   *             Unknown        - case unchanged
   *             LowerCase      - same as Unknown
   *             FirstUpperCase - first symbol forced to uppercase
   *             UpperCase      - all symbols force to uppercase
   * @return the string representation of the symbols
   */
  std::string symbols_to_string(const SymbolNumberVector& symbols, CapitalizationState caps=Unknown) const;
};

class TransitionIndex
{
 protected:
  SymbolNumber input_symbol;
  TransitionTableIndex first_transition_index;
 public:
  TransitionIndex(SymbolNumber input,
		 TransitionTableIndex first_transition):
    input_symbol(input), first_transition_index(first_transition) {}
  
  TransitionIndex(std::istream& is):
    input_symbol(NO_SYMBOL_NUMBER), first_transition_index(0)
  {
  	is.read(reinterpret_cast<char*>(&input_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&first_transition_index), sizeof(TransitionTableIndex));
  }
  
  virtual ~TransitionIndex() {}

  bool matches(const SymbolNumber s) const;
  
  TransitionTableIndex target(void) const {return first_transition_index;}
  SymbolNumber get_input(void) const {return input_symbol;}
  
  virtual bool final(void) const {return first_transition_index == 1;}
};

class TransitionWIndex : public TransitionIndex
{
 public:
  TransitionWIndex(SymbolNumber input,
		 TransitionTableIndex first_transition):
	  TransitionIndex(input, first_transition) {}

  TransitionWIndex(std::istream& is): TransitionIndex(is) {}
  
  bool final(void) const
  {return input_symbol==NO_SYMBOL_NUMBER && 
          first_transition_index != NO_TABLE_INDEX;}
  
  Weight final_weight(void) const
  {return static_cast<Weight>(first_transition_index);}
};


class Transition
{
 protected:
  SymbolNumber input_symbol;
  SymbolNumber output_symbol;
  TransitionTableIndex target_index;
 public:
  Transition(SymbolNumber input, SymbolNumber output,
             TransitionTableIndex target):
    input_symbol(input), output_symbol(output), target_index(target) {}
  
  Transition(std::istream& is):
    input_symbol(NO_SYMBOL_NUMBER), output_symbol(NO_SYMBOL_NUMBER), target_index(0)
  {
  	is.read(reinterpret_cast<char*>(&input_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&output_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&target_index), sizeof(target_index));
  }
  
  virtual ~Transition() {}

  bool matches(const SymbolNumber s) const;

  TransitionTableIndex target(void) const {return target_index;}
  SymbolNumber get_output(void) const {return output_symbol;}
  SymbolNumber get_input(void) const {return input_symbol;}
  
  virtual bool final(void) const {return target_index == 1;}
};

class TransitionW : public Transition
{
 private:
  Weight transition_weight;
 public:
  TransitionW(SymbolNumber input, SymbolNumber output,
              TransitionTableIndex target, Weight w):
      Transition(input, output, target), transition_weight(w) {}

  TransitionW(std::istream& is): Transition(is), transition_weight(0.0f)
  {is.read(reinterpret_cast<char*>(&transition_weight), sizeof(Weight));}
  
  Weight get_weight(void) const {return transition_weight;}

  bool final(void) const
  {
    if (input_symbol != NO_SYMBOL_NUMBER)
      return false;
    if (output_symbol != NO_SYMBOL_NUMBER)
      return false;
    return transition_weight != INFINITE_WEIGHT;
  }
};


template <class T>
class TableFromStream
{
 protected:
  std::vector<T> table;
  
 public:
  TableFromStream(std::istream& is, TransitionTableIndex index_count): table()
  {
    for(size_t i=0; i<index_count; i++)
      table.push_back(T(is));
  }
    
  const T& operator[](unsigned int i) const
  {
    return (i < TRANSITION_TARGET_TABLE_START) ? 
              table[i] : table[i-TRANSITION_TARGET_TABLE_START];
  }
  
  unsigned int size() const {return table.size();}
};


class AbstractTransducer;
typedef AbstractTransducer* (*TransducerCreator)(std::istream&, const TransducerHeader&, const TransducerAlphabet&);

class AbstractTransducer
{
 protected:
  static TransducerCreator creators[2][2];
  static AbstractTransducer* create(std::istream& is, TransducerHeader h);
  
  static const TransitionTableIndex START_INDEX = 0;
  
  TransducerHeader header;
  TransducerAlphabet alphabet;
  
  //Subclasses provide the index_table and transition_table
  
  /**
   * Check if the transducer accepts an input string consisting of just a blank
   */
  bool check_for_blank() const;
 public:
  AbstractTransducer(TransducerHeader h, TransducerAlphabet a):
    header(h), alphabet(a) {}  
  virtual ~AbstractTransducer() {}
  
  const TransducerHeader& get_header() const {return header;}
  const TransducerAlphabet& get_alphabet() const {return alphabet;}
    
  virtual bool is_epsilon(const Transition& transition) const
  {return transition.matches(0);}
  
  /**
   * Create a new lookup path appropriate for initializing a lookup opeation.
   * Subclasses should override this to return the appropriate subclass of
   * LookupPath
   * @return a new lookup path pointing to the beginning of the transducer
   */
  virtual LookupPath* get_initial_path() const = 0;
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const = 0;
  virtual const Transition& get_transition(TransitionTableIndex i) const = 0;
  
  /**
   * Create an appropriate transducer instance from the given input stream
   */
  static AbstractTransducer* load_transducer(std::istream& is)
  {return create(is, TransducerHeader(is));}
};

class Transducer : public AbstractTransducer
{
 protected:
  TableFromStream<TransitionIndex> index_table;
  TableFromStream<Transition> transition_table;
  
  virtual LookupPath* get_initial_path() const;
 public:
  Transducer(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    AbstractTransducer(h, a),
    index_table(is, header.index_table_size()),
    transition_table(is, header.target_table_size()) {}
  
  static AbstractTransducer* create(std::istream& is,
                                    const TransducerHeader& h,
                                    const TransducerAlphabet& a)
  {return new Transducer(is, h, a);}
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const
  {return index_table[i];}
  virtual const Transition& get_transition(TransitionTableIndex i) const
  {return transition_table[i];}
};

class TransducerFd: public Transducer
{
  virtual LookupPath* get_initial_path() const;
 public:
  TransducerFd(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    Transducer(is, h, a) {}
  
  static AbstractTransducer* create(std::istream& is,
                                    const TransducerHeader& h,
                                    const TransducerAlphabet& a)
  {return new TransducerFd(is, h, a);}
  	
	virtual bool is_epsilon(const Transition& transition) const
	{
	  return transition.matches(0) ||
	         (transition.get_input() != NO_SYMBOL_NUMBER &&
	            alphabet.get_symbol_table()[transition.get_input()].fd_op.isFlag());
	}
};

class TransducerW : public AbstractTransducer
{
 protected:
  TableFromStream<TransitionWIndex> index_table;
  TableFromStream<TransitionW> transition_table;
  
  virtual LookupPath* get_initial_path() const;
 public:
  //NOTE: the old TransitionTableReaderW appended two blank transitions to the
  // end of the transitions list. Is that really necessary to recreate here?
  TransducerW(std::istream& is, TransducerHeader h, TransducerAlphabet a) :
    AbstractTransducer(h, a),
    index_table(is, header.index_table_size()),
    transition_table(is, header.target_table_size()) {}
  
  static AbstractTransducer* create(std::istream& is,
                                    const TransducerHeader& h,
                                    const TransducerAlphabet& a)
  {return new TransducerW(is, h, a);}
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const
  {return index_table[i];}
  virtual const Transition& get_transition(TransitionTableIndex i) const
  {return transition_table[i];}
};

class TransducerWFd: public TransducerW
{
  virtual LookupPath* get_initial_path() const;
 public:
  TransducerWFd(std::istream& is, TransducerHeader h, TransducerAlphabet a):
    TransducerW(is, h, a) {}
  
  static AbstractTransducer* create(std::istream& is,
                                    const TransducerHeader& h,
                                    const TransducerAlphabet& a)
  {return new TransducerWFd(is, h, a);}
  
  virtual bool is_epsilon(const Transition& transition) const
  {
    return transition.matches(0) ||
           (transition.get_input() != NO_SYMBOL_NUMBER &&
              alphabet.get_symbol_table()[transition.get_input()].fd_op.isFlag());
  }
};

#endif
