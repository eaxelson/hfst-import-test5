#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <fstream>
#include "hfst-proc.h"

class TransducerHeader
{
 private:
  SymbolNumber number_of_symbols;
  SymbolNumber number_of_input_symbols;
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

  void read_property(bool& property, std::istream& is)
  {
    unsigned int prop;
    is.read(reinterpret_cast<char*>(&prop), sizeof(unsigned int));
    
    property = (prop != 0);
  }
 public:
  TransducerHeader(std::istream& is)
  {
  	is.read(reinterpret_cast<char*>(&number_of_input_symbols), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&number_of_symbols), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&size_of_transition_index_table), sizeof(TransitionTableIndex));
  	is.read(reinterpret_cast<char*>(&size_of_transition_target_table), sizeof(TransitionTableIndex));
  	is.read(reinterpret_cast<char*>(&number_of_states), sizeof(StateIdNumber));
  	is.read(reinterpret_cast<char*>(&number_of_transitions), sizeof(TransitionNumber));
  	
  	read_property(weighted, is);
  	read_property(deterministic, is);
    read_property(input_deterministic, is);
    read_property(minimized, is);
    read_property(cyclic, is);
    read_property(has_epsilon_epsilon_transitions, is);
    read_property(has_input_epsilon_transitions, is);
    read_property(has_input_epsilon_cycles, is);
    read_property(has_unweighted_input_epsilon_cycles, is);
  }

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
  
#ifdef DEBUG
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
#endif
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

typedef std::vector<FlagDiacriticOperation> OperationVector;
typedef std::vector<ValueNumber> FlagDiacriticState;
typedef std::vector<FlagDiacriticState> FlagDiacriticStateStack;


class TransducerAlphabet
{
 private:
  SymbolTable symbol_table;
  OperationVector operations;
  
  /**
   * The set of symbols that are considered alphabetic. Roughly, this includes
   * symbols that are not whitespace or punctuation
   */
  std::set<SymbolNumber> alphabetic;
  
  /**
   * The symbol number for a "blank" which is here considered to be a space.
   * This symbol requires special handling because it doubles as the 
   * symbolic representation for a superblank block of text
   */
  SymbolNumber blank_symbol;
  
  void get_next_symbol(std::istream& is, SymbolNumber k);
  
  /**
   * Store the blank symbol's number in blank_symbol, adding it as a new symbol
   * if it cannot be found
   */
  void setup_blank_symbol();
  
  void calculate_alphabetic();

  std::map<std::string, SymbolNumber> feature_bucket;
  std::map<std::string, ValueNumber> value_bucket;
  ValueNumber val_num;
  SymbolNumber feat_num;
  
  SymbolNumber add_symbol(std::string string)
  {
    return add_symbol(symbol_table.size(), string);
  }
  SymbolNumber add_symbol(SymbolNumber symbol, std::string string)
  {
    return add_symbol(symbol, string, FlagDiacriticOperation()); // use a dummy flag
  }
  SymbolNumber add_symbol(SymbolNumber symbol, std::string string, FlagDiacriticOperation fdo)
  {
    operations.push_back(fdo);
    symbol_table[symbol] = string;
    return symbol;
  }
 public:
  
  TransducerAlphabet(std::istream& is, SymbolNumber symbol_count)
  {
    feat_num = 0;
    val_num = 1;
    value_bucket[std::string()] = 0; // empty value = neutral
    for(SymbolNumber k=0; k<symbol_count; k++)
      get_next_symbol(is, k);
    
    if(printDebuggingInformationFlag && get_state_size()>0)
      std::cout << "Alphabet contains " << get_state_size() << " flag diacritic feature(s)" << std::endl;
    // assume the first symbol is epsilon which we don't want to print
    symbol_table.operator[](0) = "";
    
    setup_blank_symbol();
    calculate_alphabetic();
  }
  
  TransducerAlphabet(const TransducerAlphabet& o):
    symbol_table(o.symbol_table), operations(o.operations), alphabetic(o.alphabetic),
    blank_symbol(o.blank_symbol), feature_bucket(o.feature_bucket),
    value_bucket(o.value_bucket), val_num(o.val_num), feat_num(o.feat_num) {}
  
  ~TransducerAlphabet() {}
  
  const SymbolTable& get_symbol_table(void) const { return symbol_table; }
  const OperationVector& get_operation_vector(void) const { return operations; }
  SymbolNumber get_state_size(void) const { return feature_bucket.size(); }
  SymbolNumber get_blank_symbol() const {return blank_symbol;}
  
  bool is_punctuation(const char* c) const;
  
  bool is_alphabetic(const char* c) const
  { return (c[0]!='\0' && !isspace(c[0]) && 
            !is_punctuation(c)); }
  bool is_alphabetic(SymbolNumber symbol) const 
  {return (alphabetic.find(symbol) != alphabetic.end());}
  
  std::string symbol_to_string(SymbolNumber symbol) const
  {
    SymbolTable::const_iterator it = symbol_table.find(symbol);
    return (it==symbol_table.end()) ? "" : symbol_table.find(symbol)->second;
  }
  
  /**
   * Use the symbol table to convert the given symbols into a string
   * @param symbols the symbols to convert
   * @return the string representation of the symbols
   */
  std::string symbols_to_string(const SymbolNumberVector& symbols) const;
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
  
  TransitionIndex(std::istream& is)
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
  
  Transition(std::istream& is)
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

  TransitionW(std::istream& is): Transition(is)
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
  TableFromStream(std::istream& is, TransitionTableIndex index_count)
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
  
  TransducerHeader header;
  TransducerAlphabet alphabet;
  
  //Subclasses provide the index_table and transition_table
  
  static const TransitionTableIndex START_INDEX = 0;
 public:
  AbstractTransducer(TransducerHeader h, TransducerAlphabet a):
    header(h), alphabet(a) {}  
  virtual ~AbstractTransducer() {}
  
  const TransducerHeader& get_header() const {return header;}
  const TransducerAlphabet& get_alphabet() const {return alphabet;}
  
  void tokenize(TokenIOStream& token_stream); // for testing
  void run_lookup(TokenIOStream& token_stream, OutputFormatter& output_formatter);
  
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
	            alphabet.get_operation_vector()[transition.get_input()].isFlag());
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
              alphabet.get_operation_vector()[transition.get_input()].isFlag());
  }
};

#endif
