//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _HFST_OL_TRANSDUCER_H_
#define _HFST_OL_TRANSDUCER_H_

#include <vector>
#include <set>
#include <iostream>
#include <limits>
#include <string>
#include "../HfstExceptions.h"
#include "../FlagDiacritics.h"

namespace hfst_ol {
using namespace hfst;
using namespace hfst::exceptions;
using std::istream;
using std::ostream;
using std::vector;
using std::set;
using std::string;

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef unsigned int TransitionNumber;
typedef unsigned int StateIdNumber;
typedef short ValueNumber;
typedef float Weight;
typedef set<SymbolNumber> SymbolNumberSet;
typedef vector<SymbolNumber> SymbolNumberVector;
typedef set<TransitionTableIndex> TransitionTableIndexSet;
typedef vector<string> SymbolTable;

const SymbolNumber NO_SYMBOL_NUMBER = std::numeric_limits<SymbolNumber>::max();
const TransitionTableIndex NO_TABLE_INDEX = std::numeric_limits<TransitionTableIndex>::max();
const Weight INFINITE_WEIGHT = static_cast<float>(NO_TABLE_INDEX);

enum HeaderFlag {Weighted, Deterministic, Input_deterministic, Minimized,
                 Cyclic, Has_epsilon_epsilon_transitions,
                 Has_input_epsilon_transitions, Has_input_epsilon_cycles,
                 Has_unweighted_input_epsilon_cycles};

// This is 2^31, hopefully equal to UINT_MAX/2 rounded up.
// For some profound reason it can't be replaced with (UINT_MAX+1)/2.
const TransitionTableIndex TRANSITION_TARGET_TABLE_START = 2147483648u;

inline bool indexes_transition_table(const TransitionTableIndex i)
{
  return i >= TRANSITION_TARGET_TABLE_START;
}
inline bool indexes_transition_index_table(const TransitionTableIndex i)
{
  return i < TRANSITION_TARGET_TABLE_START;
}

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

  static void header_error()
  {
    throw TransducerHasWrongTypeException();
  }

  template<class T>
  static T read_property(std::istream& is)
  {
    T p;
    is.read(reinterpret_cast<char*>(&p), sizeof(T));
    return p;
  }
  template<class T>
  static void write_property(T prop, std::ostream& os)
    { os.write(reinterpret_cast<const char*>(&prop), sizeof(prop)); }
  static bool read_bool_property(std::istream& is)
  {
    unsigned int prop;
    is.read(reinterpret_cast<char*>(&prop), sizeof(unsigned int));
    if(prop == 0)
      return false;
    if(prop == 1)
      return true;
    header_error();
    return false;
  }
  static void write_bool_property(bool value, std::ostream& os)
  {
    unsigned int prop = (value?1:0);
    os.write(reinterpret_cast<char*>(&prop), sizeof(prop));
  }
 public:
  TransducerHeader(bool weights):
    number_of_input_symbols(0),
    number_of_symbols(1), // epsilon
    size_of_transition_index_table(1),
    size_of_transition_target_table(0),
    number_of_states(1),
    number_of_transitions(0),
    weighted(weights),
    deterministic(true),
    input_deterministic(true),
    minimized(true),
    cyclic(false),
    has_epsilon_epsilon_transitions(false),
    has_input_epsilon_transitions(false),
    has_input_epsilon_cycles(false),
    has_unweighted_input_epsilon_cycles(false)
  {}
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
    has_unweighted_input_epsilon_cycles(read_bool_property(is))
  {
    if(!is)
      throw TransducerHasWrongTypeException();
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
  
  void display() const
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
  
  void write(ostream& os) const
  {
    write_property(number_of_input_symbols, os);
    write_property(number_of_symbols, os);
    write_property(size_of_transition_index_table, os);
    write_property(size_of_transition_target_table, os);
    write_property(number_of_states, os);
    write_property(number_of_transitions, os);
    write_bool_property(weighted, os);
    write_bool_property(deterministic, os);
    write_bool_property(input_deterministic, os);
    write_bool_property(minimized, os);
    write_bool_property(cyclic, os);
    write_bool_property(has_epsilon_epsilon_transitions, os);
    write_bool_property(has_input_epsilon_transitions, os);
    write_bool_property(has_input_epsilon_cycles, os);
    write_bool_property(has_unweighted_input_epsilon_cycles, os);
  }
  
  friend class ConvertTransducerHeader;
};

class TransducerAlphabet
{
 protected:
  SymbolTable symbol_table;
  FdTable<SymbolNumber> fd_table;
  
 public:
  TransducerAlphabet()
  {
    symbol_table.push_back("@_EPSILON_SYMBOL_@");
  }
  TransducerAlphabet(istream& is, SymbolNumber symbol_count);
  TransducerAlphabet(const SymbolTable& st);
  
  void display() const;
  
  void write(ostream& os) const
  {
    for(SymbolTable::const_iterator i=symbol_table.begin(); i!=symbol_table.end(); i++)
    {
      os << *i;
      os.put('\0');
    }
  }
  
  bool has_flag_diacritics() const
    { return fd_table.num_features() > 0; }
  bool is_flag_diacritic(SymbolNumber symbol) const
    { return fd_table.is_diacritic(symbol); }
    
  const SymbolTable& get_symbol_table() const
    { return symbol_table; }
  const FdTable<SymbolNumber>& get_fd_table() const
    { return fd_table; }
};

class TransitionIndex
{
 protected:
  SymbolNumber input_symbol;
  TransitionTableIndex first_transition_index;
 public:
  TransitionIndex(): input_symbol(NO_SYMBOL_NUMBER), first_transition_index(NO_TABLE_INDEX) {}
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
  
  void write(ostream& os) const
  {
    os.write(reinterpret_cast<const char*>(&input_symbol), sizeof(input_symbol));
    os.write(reinterpret_cast<const char*>(&first_transition_index), sizeof(first_transition_index));
  }
  
  void display() const;
  
  TransitionTableIndex get_target(void) const {return first_transition_index;}
  SymbolNumber get_input_symbol(void) const {return input_symbol;}
  
  bool matches(const SymbolNumber s) const;
  virtual bool final(void) const;
  
  static TransitionIndex create_final()
    { return TransitionIndex(NO_SYMBOL_NUMBER, 1); }
};

class TransitionWIndex : public TransitionIndex
{
 public:
  TransitionWIndex(): TransitionIndex() {}
  TransitionWIndex(SymbolNumber input,
		 TransitionTableIndex first_transition):
    TransitionIndex(input, first_transition) {}
  TransitionWIndex(std::istream& is):
    TransitionIndex(is) {}
    
  Weight final_weight(void) const;
  
  static TransitionWIndex create_final()
    { return TransitionWIndex(NO_SYMBOL_NUMBER, 0); }
};

class Transition
{
 protected:
  SymbolNumber input_symbol;
  SymbolNumber output_symbol;
  TransitionTableIndex target_index;
 public:
  Transition(SymbolNumber input, SymbolNumber output,
             TransitionTableIndex target, Weight bogus=0.0f):
    input_symbol(input), output_symbol(output), target_index(target) {bogus=0.0f;}
  Transition(bool final, Weight bogus=0.0f):
    input_symbol(NO_SYMBOL_NUMBER), output_symbol(NO_SYMBOL_NUMBER), target_index(final?1:NO_TABLE_INDEX) {bogus=0.0f;}
  Transition(std::istream& is):
    input_symbol(NO_SYMBOL_NUMBER), output_symbol(NO_SYMBOL_NUMBER), target_index(0)
  {
  	is.read(reinterpret_cast<char*>(&input_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&output_symbol), sizeof(SymbolNumber));
  	is.read(reinterpret_cast<char*>(&target_index), sizeof(target_index));
  }
  
  virtual ~Transition() {}
  
  virtual void write(ostream& os) const
  {
    os.write(reinterpret_cast<const char*>(&input_symbol), sizeof(input_symbol));
    os.write(reinterpret_cast<const char*>(&output_symbol), sizeof(output_symbol));
    os.write(reinterpret_cast<const char*>(&target_index), sizeof(target_index));
  }
  
  virtual void display() const;

  TransitionTableIndex get_target(void) const {return target_index;}
  SymbolNumber get_output_symbol(void) const {return output_symbol;}
  SymbolNumber get_input_symbol(void) const {return input_symbol;}
  
  bool matches(const SymbolNumber s) const;
  virtual bool final(void) const;
};

class TransitionW : public Transition
{
 protected:
  Weight transition_weight;
 public:
  TransitionW(SymbolNumber input, SymbolNumber output,
              TransitionTableIndex target, Weight w):
    Transition(input, output, target), transition_weight(w) {}
  TransitionW(bool final, Weight w):
    Transition(final), transition_weight(w) {}
  TransitionW(std::istream& is): Transition(is), transition_weight(0.0f)
  {is.read(reinterpret_cast<char*>(&transition_weight), sizeof(Weight));}
  
  void write(ostream& os) const
  {
    Transition::write(os);
    os.write(reinterpret_cast<const char*>(&transition_weight), sizeof(transition_weight));
  }
  
  void display() const;
  
  Weight get_weight(void) const {return transition_weight;}
};


template <class T>
class TransducerTable
{
 protected:
  vector<T> table;
 public:
  TransducerTable(): table() {}
  TransducerTable(size_t size, const T& entry): table(size, entry) {}
  TransducerTable(std::istream& is, TransitionTableIndex index_count): table()
  {
    for(size_t i=0; i<index_count; i++)
      table.push_back(T(is));
  }
  TransducerTable(const TransducerTable& t): table(t.table) {}
  
  void append(const T& v) {table.push_back(v);}
  void set(size_t index, const T& v) {table[index] = v;}
  
  const T& operator[](TransitionTableIndex i) const
  {
    return (i < TRANSITION_TARGET_TABLE_START) ? 
              table[i] : table[i-TRANSITION_TARGET_TABLE_START];
  }
  
  void display(bool transition_table) const
  {
    for(size_t i=0;i<table.size();i++)
    {
      std::cout << i;
      if(transition_table)
        std::cout << "/" << i+TRANSITION_TARGET_TABLE_START;
      std::cout << ": ";
      table[i].display();
    }
  }
  
  unsigned int size() const {return table.size();}
};

class TransducerTablesInterface
{
 public:
  virtual ~TransducerTablesInterface() {}
  
  virtual const TransitionIndex& get_index(TransitionTableIndex i) const = 0;
  virtual const Transition& get_transition(TransitionTableIndex i) const = 0;
  
  virtual void display() const {}
};

template <class T1, class T2>
class TransducerTables : public TransducerTablesInterface
{
 protected:
  TransducerTable<T1> index_table;
  TransducerTable<T2> transition_table;
 public:
  TransducerTables(istream& is, TransitionTableIndex index_table_size,
                                TransitionTableIndex transition_table_size):
    index_table(is, index_table_size), transition_table(is, transition_table_size) {}
  TransducerTables(): index_table(1, T1::create_final()), transition_table() {}
  TransducerTables(const TransducerTable<T1>& index_table,
                   const TransducerTable<T2>& transition_table):
    index_table(index_table), transition_table(transition_table) {}
  
  const TransitionIndex& get_index(TransitionTableIndex i) const
    {return index_table[i];}
  const Transition& get_transition(TransitionTableIndex i) const
    {return transition_table[i];}
  
  void display() const
  {
    std::cout << "Transition index table:" << std::endl;
    index_table.display(false);
    std::cout << "Transition table:" << std::endl;
    transition_table.display(true);
  }
};

class Transducer
{
 protected:
  TransducerHeader* header;
  TransducerAlphabet* alphabet;
  
  TransducerTablesInterface* tables;
  
  Transducer();
  Transducer(const TransducerHeader& header, const TransducerAlphabet& alphabet,
             const TransducerTable<TransitionIndex>& index_table,
             const TransducerTable<Transition>& transition_table);
  Transducer(const TransducerHeader& header, const TransducerAlphabet& alphabet,
             const TransducerTable<TransitionWIndex>& index_table,
             const TransducerTable<TransitionW>& transition_table);
  
  void load_tables(istream& is);
 public:
  Transducer(istream& is);
  Transducer(bool weighted);
  virtual ~Transducer();

  void write(ostream& os) const;
  void display() const;

  const TransducerHeader& get_header() const
    { return *header; }
  const TransducerAlphabet& get_alphabet() const
    { return *alphabet; }
  const TransitionIndex& get_index(TransitionTableIndex i) const
    { return tables->get_index(i); }
  const Transition& get_transition(TransitionTableIndex i) const
    { return tables->get_transition(i); }

  // state_index must be an index to a state which is defined as either:
  // (1) the start of a set of entries in the transition index table, or
  // (2) the boundary before a set of entries in the transition table, in
  //     which case the following entries will all have the same input symbol
  // This function will return a vector of indices to the transition table,
  // i.e. the arcs from the given state
  TransitionTableIndexSet get_transitions_from_state(TransitionTableIndex state_index) const;

  friend class ConvertTransducer;
};

}

#endif
