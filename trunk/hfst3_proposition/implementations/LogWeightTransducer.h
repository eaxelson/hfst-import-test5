#include "GlobalSymbolTable.h"
#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "openfst-1.1/src/include/fst/fstlib.h"
#include "LogFstTrieFunctions.h"
#include "ExtractStrings.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { namespace implementations
{
  using namespace fst;
  using namespace hfst::exceptions;
  typedef ArcTpl<LogWeight>::StateId StateId;
  typedef ArcTpl<LogWeight> LogArc;
  typedef VectorFst<LogArc> LogFst;

  using namespace hfst::symbols;
  using std::ostream;
  using std::ostringstream;

  extern GlobalSymbolTable global_symbol_table;
  class LogWeightInputStream 
  {
  private:
    std::string filename;
    ifstream i_stream;
    istream &input_stream;
    bool first_read;
    void populate_key_table(KeyTable &key_table,
			    const SymbolTable * i_symbol_table,
			    const SymbolTable * o_symbol_table,
			    KeyMap &key_map);
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
  public:
    LogWeightInputStream(void);
    LogWeightInputStream(const char * fn);
    void open(void);
    void close(void);
    bool is_open(void) const;
    bool is_eof(void) const;
    bool is_bad(void) const;
    bool is_good(void) const;
    bool is_fst(void) const;
    bool operator() (void) const;
    //LogFst * read_transducer(KeyTable &key_table);
    LogFst * read_transducer(bool has_header);
  };

  class LogWeightOutputStream 
  {
  private:
    std::string filename;
    ofstream o_stream;
    ostream &output_stream;
    void set_symbols(LogFst * transducer, KeyTable &key_table);
    void write_3_0_library_header(std::ostream &out);
  public:
    LogWeightOutputStream(void); 
    LogWeightOutputStream(const char * filename);
    void open(void);
    void close(void);
    void write_transducer(LogFst * transducer, KeyTable &key_table);
    void write_transducer(LogFst * transducer);
  };


  class LogWeightTransitionIterator;
  class LogWeightStateIndexer;
  class LogWeightState
  {
    protected:
      StateId state;
      LogFst * t;
      friend class LogWeightStateIndexer;
    public:
      LogWeightState(StateId state, LogFst * t);
      LogWeightState(const LogWeightState &s);
      LogWeight get_final_weight(void) const;
      bool operator< (const LogWeightState &another) const;
      bool operator== (const LogWeightState &another) const;
      bool operator!= (const LogWeightState &another) const;
      typedef LogWeightTransitionIterator const_iterator; 
      const_iterator begin(void) const;
      const_iterator end(void) const;
      void print(KeyTable &key_table, ostream &out,
      		 LogWeightStateIndexer &indexer) const;
  };

  class LogWeightStateIterator 
    {
    protected:
      LogFst * t;
      StateIterator<LogFst> * iterator;
      int current_state;
      bool ended;
    public:
      LogWeightStateIterator(LogFst * t);
      LogWeightStateIterator(void);
      ~LogWeightStateIterator(void);
      void operator= (const LogWeightStateIterator &another);
      bool operator== (const LogWeightStateIterator &another) const;
      bool operator!= (const LogWeightStateIterator &another) const;
      const LogWeightState operator* (void);
      void operator++ (void);
      void operator++ (int);
    };
 

  class LogWeightTransition
    {
    protected:
      LogArc arc;
      StateId source_state;
      LogFst * t;
    public:
      LogWeightTransition(const LogArc &arc, 
			       StateId source_state, 
			       LogFst * t);
      Key get_input_key(void) const;
      Key get_output_key(void) const;
      LogWeightState get_target_state(void) const;
      LogWeightState get_source_state(void) const;
      LogWeight get_weight(void) const;
      void print(KeyTable &key_table, ostream &out,
		 LogWeightStateIndexer &indexer) const;
    };

  class LogWeightTransitionIterator
    {
    protected:
      ArcIterator<LogFst> * arc_iterator;
      StateId state;
      LogFst * t;
      bool end_iterator;
    public:
      LogWeightTransitionIterator(StateId state,LogFst * t);
      LogWeightTransitionIterator(void);
      ~LogWeightTransitionIterator(void);
      void operator=  (const LogWeightTransitionIterator &another);
      bool operator== (const LogWeightTransitionIterator &another);
      bool operator!= (const LogWeightTransitionIterator &another);
      const LogWeightTransition operator* (void);
      void operator++ (void);
      void operator++ (int);
    };
 
  class LogWeightStateIndexer
    {
    protected:
      LogFst * t;
    public:
      LogWeightStateIndexer(LogFst * t);
      unsigned int operator[](const LogWeightState &state);
      const LogWeightState operator[](unsigned int number);
    };

  class LogWeightTransducer
    {
    public:
      static LogFst * create_empty_transducer(void);
      static LogFst * create_epsilon_transducer(void);
      static LogFst * define_transducer(Key k);
      static LogFst * define_transducer(const KeyPair &kp);
      static LogFst * define_transducer(const KeyPairVector &kpv);
      static LogFst * copy(LogFst * t);
      static LogFst * determinize(LogFst * t);
      static LogFst * minimize(LogFst * t);
      static LogFst * remove_epsilons(LogFst * t);
      static LogFst * n_best(LogFst * t,int n);
      static LogFst * repeat_star(LogFst * t);
      static LogFst * repeat_plus(LogFst * t);
      static LogFst * repeat_n(LogFst * t,int n);
      static LogFst * repeat_le_n(LogFst * t,int n);
      static LogFst * optionalize(LogFst * t);
      static LogFst * invert(LogFst * t);
      static LogFst * reverse(LogFst * transducer);
      static LogFst * extract_input_language(LogFst * t);
      static LogFst * extract_output_language(LogFst * t);
      static LogFst * substitute(LogFst * t,Key old_key,Key new_key);
      static LogFst * substitute(LogFst * t,
			      KeyPair old_key_pair,
			      KeyPair new_key_pair);
      static LogFst * compose(LogFst * t1,
				   LogFst * t2);
      static LogFst * concatenate(LogFst * t1,
					LogFst * t2);
      static LogFst * disjunct(LogFst * t1,
			      LogFst * t2);
      static void disjunct_as_tries(LogFst &t1,
				    const LogFst * t2);
      static LogFst * intersect(LogFst * t1,
			     LogFst * t2);
      static LogFst * subtract(LogFst * t1,
			    LogFst * t2);
      static LogFst * set_weight(LogFst * t,float f);
      static LogFst * transform_weights(LogFst * t,float(*func)(float));
      typedef LogWeightStateIterator const_iterator;
      static const_iterator begin(LogFst * t);
      static const_iterator end(LogFst * t);
      static LogFst * harmonize(LogFst * t,KeyMap &key_map);
      static void print(LogFst * t, KeyTable &key_table, ostream &out);
      static void extract_strings(LogFst * t, KeyTable &kt,
				  WeightedStrings<float>::Set &results);
    };

} }
