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

  using std::ostream;
  using std::ostringstream;
  using std::stringstream;

  //extern GlobalSymbolTable global_symbol_table;
  class TropicalWeightInputStream 
  {
  private:
    std::string filename;
    ifstream i_stream;
    istream &input_stream;
    /*void populate_key_table(KeyTable &key_table,
			    const SymbolTable * i_symbol_table,
			    const SymbolTable * o_symbol_table,
			    KeyMap &key_map);*/
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
    StdVectorFst * read_transducer(bool has_header);
  };

  class TropicalWeightOutputStream 
  {
  private:
    std::string filename;
    ofstream o_stream;
    ostream &output_stream;
    void write_3_0_library_header(std::ostream &out);
  public:
    TropicalWeightOutputStream(void); 
    TropicalWeightOutputStream(const char * filename);
    void open(void);
    void close(void);
    void write_transducer(StdVectorFst * transducer);
  };

  class TropicalWeightTransitionIterator;

  typedef StateId TropicalWeightState;

  class TropicalWeightStateIterator 
    {
    protected:
      StateIterator<StdVectorFst> * iterator;
    public:
      TropicalWeightStateIterator(StdVectorFst * t);
      ~TropicalWeightStateIterator(void);
      void next(void);
      bool done(void);
      TropicalWeightState value(void);
    };
 

  class TropicalWeightTransition
    {
    protected:
      StdArc arc;
      StdVectorFst * t;
    public:
      TropicalWeightTransition(const StdArc &arc, StdVectorFst *t);
      ~TropicalWeightTransition(void);
      std::string get_input_symbol(void) const;
      std::string get_output_symbol(void) const;
      TropicalWeightState get_target_state(void) const;
      TropicalWeight get_weight(void) const;
    };


  class TropicalWeightTransitionIterator
    {
    protected:
      ArcIterator<StdVectorFst> * arc_iterator;
      StdVectorFst * t;
    public:
      TropicalWeightTransitionIterator(StdVectorFst * t, StateId state);
      ~TropicalWeightTransitionIterator(void);
      void next(void);
      bool done(void);
      TropicalWeightTransition value(void);
    };
  

  class TropicalWeightTransducer
    {
    public:
      static StdVectorFst * create_empty_transducer(void);
      static StdVectorFst * create_epsilon_transducer(void);
      static StdVectorFst * define_transducer(const std::string &symbol);
      static StdVectorFst * define_transducer(const std::string &isymbol, const std::string &osymbol);
      static StdVectorFst * define_transducer(const hfst::StringPairVector &spv);
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
      static void extract_strings(StdVectorFst * t,
				  hfst::WeightedPaths<float>::Set &results);
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
      static StdVectorFst * set_final_weights(StdVectorFst * t, float weight);
      static StdVectorFst * transform_weights(StdVectorFst * t,float (*func)(float f));

      static std::pair<StdVectorFst*, StdVectorFst*> harmonize(StdVectorFst *t1, StdVectorFst *t2);

      static void write_in_att_format(StdVectorFst * t, FILE *ofile);
      static void write_in_att_format_number(StdVectorFst * t, FILE *ofile);
      
      static void test_minimize(void);

      static void write_in_att_format(StdVectorFst * t, std::ostream &os);
      static StdVectorFst * read_in_att_format(FILE *ifile);
      static bool test_equivalence(StdVectorFst *one, StdVectorFst *another);
      static StdVectorFst * substitute(StdVectorFst * t, std::string old_symbol, std::string new_symbol);
      static StdVectorFst * substitute(StdVectorFst * t,
				       StringPair old_symbol_pair,
				       StringPair new_symbol_pair);
      static StdVectorFst * substitute(StdVectorFst * t,
				       StringPair old_symbol_pair,
				       StdVectorFst *transducer);

      static StringSet get_string_set(StdVectorFst *t);
      static NumberNumberMap create_mapping(StdVectorFst * t1, StdVectorFst * t2);
      static void recode_symbol_numbers(StdVectorFst * t, hfst::NumberNumberMap &km);      
      static StdVectorFst * expand_arcs(StdVectorFst * t, hfst::StringSet &unknown);
      static StdVectorFst * substitute(StdVectorFst * t,unsigned int old_key,unsigned int new_key);
      static StdVectorFst * substitute(StdVectorFst * t,
      				       pair<unsigned int, unsigned int> old_key_pair,
      				       pair<unsigned int, unsigned int> new_key_pair);

      //static StdVectorFst * define_transducer(Key k);
      //static StdVectorFst * define_transducer(const KeyPair &kp);
      //static StdVectorFst * define_transducer(const KeyPairVector &kpv);

    private:
      static fst::SymbolTable * create_symbol_table(std::string name);
      static void initialize_symbol_tables(StdVectorFst *t);
      
    public:
      /* For HfstMutableTransducer */
      static StateId add_state(StdVectorFst *t);
      static void set_final_weight(StdVectorFst *t, StateId s, float w);
      static void add_transition(StdVectorFst *t, StateId source,
				 std::string &isymbol, std::string &osymbol, float w, StateId target);
      static float get_final_weight(StdVectorFst *t, StateId s);
      static float is_final(StdVectorFst *t, StateId s);
      static StateId get_initial_state(StdVectorFst *t);
      static void represent_empty_transducer_as_having_one_state(StdVectorFst *t);      

    };

} }
