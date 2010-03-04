#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "openfst-1.1/src/include/fst/fstlib.h"
#include "ExtractStrings.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { 
namespace implementations
{
  using namespace fst;
  using namespace hfst::exceptions;
  typedef StdArc::StateId StateId;

  using namespace hfst::symbols;
  using std::ostream;
  using std::ostringstream;
  using std::stringstream;

  extern GlobalSymbolTable global_symbol_table;
  class TropicalWeightInputStream 
  {
  private:
    std::string filename;
    ifstream i_stream;
    istream &input_stream;
    void populate_key_table(KeyTable &key_table,
			    const SymbolTable * i_symbol_table,
			    const SymbolTable * o_symbol_table,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
  public:
    TropicalWeightInputStream(void);
    TropicalWeightInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void) const;
    bool is_eof(void) const;
    bool is_bad(void) const;
    bool is_good(void) const;
    bool is_fst(void) const;
    bool operator() (void) const;
    StdVectorFst * read_transducer(KeyTable &key_table);
    StdVectorFst * read_transducer(void);
  };

  class TropicalWeightTransitionIterator;
  class TropicalWeightStateIndexer;
  class TropicalWeightState
  {
    protected:
      StateId state;
      StdVectorFst * t;
      friend class TropicalWeightStateIndexer;
    public:
      TropicalWeightState(StateId state, StdVectorFst * t);
      TropicalWeightState(const TropicalWeightState &s);
      TropicalWeight get_final_weight(void) const;
      bool operator< (const TropicalWeightState &another) const;
      bool operator== (const TropicalWeightState &another) const;
      bool operator!= (const TropicalWeightState &another) const;
      typedef TropicalWeightTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
      		 TropicalWeightStateIndexer &indexer) const;
  };

  class TropicalWeightStateIterator 
    {
    protected:
      StdVectorFst * t;
      StateIterator<StdVectorFst> * iterator;
      int current_state;
      bool ended;
    public:
      TropicalWeightStateIterator(StdVectorFst * t);
      TropicalWeightStateIterator(void);
      ~TropicalWeightStateIterator(void);
      void operator= (const TropicalWeightStateIterator &another);
      bool operator== (const TropicalWeightStateIterator &another) const;
      bool operator!= (const TropicalWeightStateIterator &another) const;
      const TropicalWeightState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };
 

  class TropicalWeightTransition
    {
    protected:
      StdArc arc;
      StateId source_state;
      StdVectorFst * t;
    public:
      TropicalWeightTransition(const StdArc &arc, 
			       StateId source_state, 
			       StdVectorFst * t);
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      TropicalWeightState get_target_state(void) const;
      TropicalWeightState get_source_state(void) const;
      TropicalWeight get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 TropicalWeightStateIndexer &indexer) const;
    };

  class TropicalWeightTransitionIterator
    {
    protected:
      ArcIterator<StdVectorFst> * arc_iterator;
      StateId state;
      StdVectorFst * t;
      bool end_iterator;
    public:
      TropicalWeightTransitionIterator(StateId state,StdVectorFst * t);
      TropicalWeightTransitionIterator(void);
      ~TropicalWeightTransitionIterator(void);
      void operator=  (const TropicalWeightTransitionIterator &another);
      bool operator== (const TropicalWeightTransitionIterator &another);
      bool operator!= (const TropicalWeightTransitionIterator &another);
      const TropicalWeightTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };
 
  class TropicalWeightStateIndexer
    {
    protected:
      StdVectorFst * t;
    public:
      TropicalWeightStateIndexer(StdVectorFst * t);
      unsigned int operator[](const TropicalWeightState &state);
      const TropicalWeightState operator[](unsigned int number);
    };

  class TropicalWeightTransducer
    {
    public:
      static StdVectorFst * create_empty_transducer(void);
      static StdVectorFst * create_epsilon_transducer(void);
      static StdVectorFst * define_transducer(Key k);
      static StdVectorFst * define_transducer(const KeyPair &kp);
      static StdVectorFst * define_transducer(const KeyPairVector &kpv);
      static StdVectorFst * copy(StdVectorFst * t);
      static StdVectorFst * determinize(StdVectorFst * t);
      static StdVectorFst * minimize(StdVectorFst * t);
      static StdVectorFst * remove_epsilons(StdVectorFst * t);
      static StdVectorFst * n_best(StdVectorFst * t,int n);
      static StdVectorFst * repeat_star(StdVectorFst * t);
      static StdVectorFst * repeat_plus(StdVectorFst * t);
      static StdVectorFst * repeat_n(StdVectorFst * t,int n);
      static StdVectorFst * repeat_le_n(StdVectorFst * t,int n);
      static StdVectorFst * optionalize(StdVectorFst * t);
      static StdVectorFst * invert(StdVectorFst * t);
      static StdVectorFst * reverse(StdVectorFst * transducer);
      static StdVectorFst * extract_input_language(StdVectorFst * t);
      static StdVectorFst * extract_output_language(StdVectorFst * t);
      static void extract_strings(StdVectorFst * t, KeyTable &kt,
				  WeightedStrings<float>::Set &results);
      static StdVectorFst * substitute(StdVectorFst * t,Key old_key,Key new_key);
      static StdVectorFst * substitute(StdVectorFst * t,
			      KeyPair old_key_pair,
			      KeyPair new_key_pair);
      static StdVectorFst * compose(StdVectorFst * t1,
				   StdVectorFst * t2);
      static StdVectorFst * concatenate(StdVectorFst * t1,
					StdVectorFst * t2);
      static StdVectorFst * disjunct(StdVectorFst * t1,
			      StdVectorFst * t2);
      static StdVectorFst * intersect(StdVectorFst * t1,
			     StdVectorFst * t2);
      static StdVectorFst * subtract(StdVectorFst * t1,
			    StdVectorFst * t2);
      static StdVectorFst * set_weight(StdVectorFst * t,float f);
      typedef TropicalWeightStateIterator const_iterator;
      static const_iterator begin(StdVectorFst * t);
      static const_iterator end(StdVectorFst * t);
      static StdVectorFst * harmonize(StdVectorFst * t,KeyMap &key_map);
      static void print(StdVectorFst * t, KeyTable &key_table, ostream &out);

      /* For HfstMutableTransducer */
      static StateId add_state(StdVectorFst *t);
      static void set_final_weight(StdVectorFst *t, StateId s, float w);
      static void add_transition(StdVectorFst *t, StateId source, Key ilabel, Key olabel, float w, StateId target);
      static float get_final_weight(StdVectorFst *t, StateId s);
    };

} }
