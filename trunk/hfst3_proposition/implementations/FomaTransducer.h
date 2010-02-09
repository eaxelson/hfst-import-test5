#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "foma/fomalib.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { namespace implementations
{
  using namespace hfst::symbols;
  using namespace hfst::exceptions;
  using std::ostream;
  using std::ostringstream;

  extern GlobalSymbolTable global_symbol_table;

  class FomaInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    void add_symbol(StringSymbolMap &string_number_map,
		    Character c,
		    Alphabet &alphabet);
    void populate_key_table(KeyTable &key_table,
			    Alphabet &alphabet,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);

  public:
    FomaInputStream(void);
    FomaInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    Transducer * read_transducer(KeyTable &key_table);
    Transducer * read_transducer(void);
  };
  
  class FomaTransitionIterator;
  class FomaStateIndexer;
  class FomaState
  {
    protected:
      Node * state;
      Transducer * t;
      friend class FomaStateIndexer;
    public:
      FomaState(Node * state, Transducer * t);
      FomaState(const FomaState &s);
      bool get_final_weight(void) const;
      bool operator< (const FomaState &another) const;
      bool operator== (const FomaState &another) const;
      bool operator!= (const FomaState &another) const;
      typedef FomaTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FomaStateIndexer &indexer) const;
  };
  
  class FomaStateIterator 
    {
    protected:
      NodeNumbering node_numbering;
      Transducer * t;
      int current_state;
      bool ended;
    public:
      FomaStateIterator(Transducer * t);
      FomaStateIterator(void);
      void operator= (const FomaStateIterator &another);
      bool operator== (const FomaStateIterator &another) const;
      bool operator!= (const FomaStateIterator &another) const;
      const FomaState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FomaTransition
    {
    protected:
      Arc * arc;
      Node * source_state;
      Transducer * t;
    public:
      FomaTransition(Arc * arc, Node * n, Transducer * t);
      FomaTransition(const FomaTransition &t);
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      FomaState get_target_state(void) const;
      FomaState get_source_state(void) const;
      /* Kind of weird function, since foma
	 transitions don't really carry a weight. 
	 Will always return true. */
      bool get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FomaStateIndexer &indexer) const;
    };

  class FomaTransitionIterator
    {
    protected:
      Node * state;
      ArcsIter arc_iterator;
      Transducer * t;
      bool end_iterator;
    public:
      FomaTransitionIterator(Node * state,Transducer * t);
      FomaTransitionIterator(void);
      void operator=  (const FomaTransitionIterator &another);
      bool operator== (const FomaTransitionIterator &another);
      bool operator!= (const FomaTransitionIterator &another);
      const FomaTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FomaStateIndexer
    {
    protected:
      Transducer * t;
      NodeNumbering numbering;
    public:
      FomaStateIndexer(Transducer * t);
      unsigned int operator[](const FomaState &state);
      const FomaState operator[](unsigned int number);
    };

  class FomaTransducer
    {
    public:
      static fsm * create_empty_transducer(void);
      static fsm * create_epsilon_transducer(void);
      static fsm * define_transducer(Key k);
      static fsm * define_transducer(const KeyPair &kp);
      static fsm * define_transducer(const KeyPairVector &kpv);
      static fsm * copy(fsm * t);
      static fsm * determinize(fsm * t);
      static fsm * minimize(fsm * t);
      static fsm * remove_epsilons(fsm * t);
      static fsm * repeat_star(fsm * t);
      static fsm * repeat_plus(fsm * t);
      static fsm * repeat_n(fsm * t,int n);
      static fsm * repeat_le_n(fsm * t,int n);
      static fsm * optionalize(fsm * t);
      static fsm * invert(fsm * t);
      static fsm * reverse(fsm * transducer);
      static fsm * extract_input_language(fsm * t);
      static fsm * extract_output_language(fsm * t);
      static fsm * substitute(fsm * t,Key old_key,Key new_key);
      static fsm * substitute(fsm * t,
			      KeyPair old_key_pair,
			      KeyPair new_key_pair);
      static fsm * compose(fsm * t1,
			   fsm * t2);
      static fsm * concatenate(fsm * t1,
			       fsm * t2);
      static fsm * disjunct(fsm * t1,
			    fsm * t2);
      static fsm * intersect(fsm * t1,
			     fsm * t2);
      static fsm * subtract(fsm * t1,
			    fsm * t2);
      typedef FomaStateIterator const_iterator;
      static const_iterator begin(fsm * t);
      static const_iterator end(fsm * t);
      static fsm * harmonize(fsm * t,KeyMap &key_map);
      static void print(fsm * t, KeyTable &key_table, ostream &out);
    };
} }
