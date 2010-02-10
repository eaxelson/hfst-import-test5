#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "FOOLIB"
// Tropical: #include "ExtractStrings.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst {
  namespace implementations
{
  using namespace FOO;
  using namespace hfst::exceptions;
  // Tropical: typedef StdArc::StateId StateId;
  using namespace hfst::symbols;
  using std::ostream;
  using std::ostringstream;
  // Tropical: using std::stringstream;

  extern GlobalSymbolTable global_symbol_table;

  class FooInputStream 
  {
  private:
    std::string filename;
    // Sfst: FILE * input_file;
    // Tropical: ifstream i_stream;
    // Tropical: istream &input_stream;
    // Sfst: void add_symbol(StringSymbolMap &string_number_map,
    // Character c,
    // Alphabet &alphabet);
    void populate_key_table(KeyTable &key_table,
			    Sfst: Alphabet &alphabet,
			    Tropical: const SymbolTable * i_symbol_table,
			    Tropical: const SymbolTable * o_symbol_table,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);

  public:
    FooInputStream(void);
    FooInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    // Tropical: bool operator() (void);
    FooTransducer * read_transducer(KeyTable &key_table);
    FooTransducer * read_transducer(void);
  };

  class FooTransitionIterator;
  class FooStateIndexer;
  class FooState
  {
    protected:
      FooNode state;
      FooTransducer * t;
      friend class FooStateIndexer;
    public:
      FooState(FooNode state, FooTransducer * t);
      FooState(const FooState &s);
      Foo get_final_weight(void) const;
      bool operator< (const FooState &another) const;
      bool operator== (const FooState &another) const;
      bool operator!= (const FooState &another) const;
      typedef FooTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FooStateIndexer &indexer) const;
  };

  class FooStateIterator 
    {
    protected:
      FooTransducer * t;
      // Sfst: NodeNumbering node_numbering;
      // Tropical: StateIterator<FooTransducer> * iterator;
      int current_state;
      bool ended;
    public:
      FooStateIterator(FooTransducer * t);
      FooStateIterator(void);
      // Tropical: ~FooStateIterator(void);
      void operator= (const FooStateIterator &another);
      bool operator== (const FooStateIterator &another) const;
      bool operator!= (const FooStateIterator &another) const;
      const FooState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FooTransition
    {
    protected:
      FooArc arc;
      FooNode source_state;
      FooTransducer * t;
    public:
      // Sfst: FooTransition(FooArc arc, FooNode n, FooTransducer * t);
      // Tropical: FooTransition(const FooArc &arc, 
      //		       FooNode source_state, 
      //		       FooTransducer * t);
      // Sfst: FooTransition(const FooTransition &t);
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      FooState get_target_state(void) const;
      FooState get_source_state(void) const;
      Foo get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 FooStateIndexer &indexer) const;
    };

  class FooTransitionIterator
    {
    protected:
      FooNode state;
      FooArcsIter arc_iterator;
      FooTransducer * t;
      bool end_iterator;
    public:
      FooTransitionIterator(FooNode state,FooTransducer * t);
      FooTransitionIterator(void);
      //Sfst: ~FooTransitionIterator(void);
      void operator=  (const FooTransitionIterator &another);
      bool operator== (const FooTransitionIterator &another);
      bool operator!= (const FooTransitionIterator &another);
      const FooTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };

  class FooStateIndexer
    {
    protected:
      FooTransducer * t;
      // Sfst: NodeNumbering numbering;
    public:
      FooStateIndexer(FooTransducer * t);
      unsigned int operator[](const FooState &state);
      const FooState operator[](unsigned int number);
    };

  class FooTransducer
    {
    public:
      static FooTransducer * create_empty_transducer(void);
      static FooTransducer * create_epsilon_transducer(void);
      static FooTransducer * define_transducer(Key k);
      static FooTransducer * define_transducer(const KeyPair &kp);
      static FooTransducer * define_transducer(const KeyPairVector &kpv);
      static FooTransducer * copy(FooTransducer * t);
      static FooTransducer * determinize(FooTransducer * t);
      static FooTransducer * minimize(FooTransducer * t);
      static FooTransducer * remove_epsilons(FooTransducer * t);
      // Tropical: static FooTransducer * n_best(FooTransducer * t,int n);
      static FooTransducer * repeat_star(FooTransducer * t);
      static FooTransducer * repeat_plus(FooTransducer * t);
      static FooTransducer * repeat_n(FooTransducer * t,int n);
      static FooTransducer * repeat_le_n(FooTransducer * t,int n);
      static FooTransducer * optionalize(FooTransducer * t);
      static FooTransducer * invert(FooTransducer * t);
      static FooTransducer * reverse(FooTransducer * transducer);
      static FooTransducer * extract_input_language(FooTransducer * t);
      static FooTransducer * extract_output_language(FooTransducer * t);
      // Tropical: static void extract_strings(FooTransducer * t, KeyTable &kt,
      //	  WeightedStrings<float>::Set &results);
      static FooTransducer * substitute(FooTransducer * t,Key old_key,Key new_key);
      static FooTransducer * substitute(FooTransducer * t,
			      KeyPair old_key_pair,
			      KeyPair new_key_pair);
      static FooTransducer * compose(FooTransducer * t1,
				     FooTransducer * t2);
      static FooTransducer * concatenate(FooTransducer * t1,
					 FooTransducer * t2);
      static FooTransducer * disjunct(FooTransducer * t1,
				      FooTransducer * t2);
      static FooTransducer * intersect(FooTransducer * t1,
				       FooTransducer * t2);
      static FooTransducer * subtract(FooTransducer * t1,
				      FooTransducer * t2);
      // Tropical: static FooTransducer * set_weight(FooTransducer * t,float f);
      typedef FooStateIterator const_iterator;
      static const_iterator begin(FooTransducer * t);
      static const_iterator end(FooTransducer * t);
      static FooTransducer * harmonize(FooTransducer * t,KeyMap &key_map);
      static void print(FooTransducer * t, KeyTable &key_table, ostream &out);
    };

} }
