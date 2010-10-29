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
#ifndef _TROPICAL_WEIGHT_TRANSDUCER_H_
#define _TROPICAL_WEIGHT_TRANSDUCER_H_

#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "FlagDiacritics.h"
#include <fst/fstlib.h>
#include "ExtractStrings.h"
#include "TropicalWeightComposeIntersect.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include "HfstAlphabet.h"

namespace hfst { 
namespace implementations
{
  using namespace fst;
  using namespace hfst::exceptions;
  typedef StdArc::StateId StateId;

  using std::ostream;
  using std::ostringstream;
  using std::stringstream;

  void openfst_tropical_set_hopcroft(bool value);
  void openfst_tropical_set_use_symbol_tables(bool value);

  class TropicalWeightInputStream 
  {
  private:
    std::string filename;
    ifstream i_stream;
    istream &input_stream;
    void skip_identifier_version_3_0(void);
    void skip_hfst_header(void);
  public:
    TropicalWeightInputStream(void);
    TropicalWeightInputStream(const char * filename);
    void close(void);
    bool is_eof(void) const;
    bool is_bad(void) const;
    bool is_good(void) const;
    bool is_fst(void) const;
    bool operator() (void) const;
    void ignore(unsigned int);
    StdVectorFst * read_transducer();
    
    static bool is_fst(FILE * f);
    static bool is_fst(istream &s);
  };

  class TropicalWeightOutputStream 
  {
  private:
    std::string filename;
    ofstream o_stream;
    ostream &output_stream;
    //void write_3_0_library_header(std::ostream &out);
  public:
    TropicalWeightOutputStream(void); 
    TropicalWeightOutputStream(const char * filename);
    void close(void);
    void write(const char &c);
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

      // string versions
      static StdVectorFst * define_transducer(const std::string &symbol);
      static StdVectorFst * define_transducer(const std::string &isymbol, const std::string &osymbol);
      static StdVectorFst * define_transducer(const hfst::StringPairVector &spv);
      static StdVectorFst * define_transducer(const hfst::StringPairSet &sps, bool cyclic=false);
      static StdVectorFst * define_transducer(const std::vector<StringPairSet> &spsv);

      // number versions
      static StdVectorFst * define_transducer(unsigned int number);
      static StdVectorFst * define_transducer(unsigned int inumber, unsigned int onumber);
      static StdVectorFst * define_transducer(const hfst::NumberPairVector &npv);
      static StdVectorFst * define_transducer(const hfst::NumberPairSet &nps, bool cyclic=false);
      static StdVectorFst * define_transducer(const std::vector<NumberPairSet> &npsv);

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
      static void extract_strings(StdVectorFst * t, hfst::ExtractStringsCb& callback,
          int cycles=-1, FdTable<int64>* fd=NULL, bool filter_fd=false);
      static StdVectorFst * compose(StdVectorFst * t1,
				   StdVectorFst * t2);
      static StdVectorFst * concatenate(StdVectorFst * t1,
					StdVectorFst * t2);
      static StdVectorFst * disjunct(StdVectorFst * t1,
			      StdVectorFst * t2);

      static StdVectorFst * disjunct(StdVectorFst * t, const StringPairVector &spv);
      static StdVectorFst * disjunct(StdVectorFst * t, const NumberPairVector &npv);

      static StdVectorFst * intersect(StdVectorFst * t1,
			     StdVectorFst * t2);
      static StdVectorFst * subtract(StdVectorFst * t1,
			    StdVectorFst * t2);
      static StdVectorFst * set_weight(StdVectorFst * t,float f);
      static StdVectorFst * set_final_weights(StdVectorFst * t, float weight);
      static StdVectorFst * transform_weights(StdVectorFst * t,float (*func)(float f));
      static StdVectorFst * push_weights(StdVectorFst * t, bool to_initial_state);

      static std::pair<StdVectorFst*, StdVectorFst*> harmonize(StdVectorFst *t1, StdVectorFst *t2, bool unknown_symbols_in_use=true);

      static void write_in_att_format(StdVectorFst * t, FILE *ofile);
      static void write_in_att_format_number(StdVectorFst * t, FILE *ofile);
      
      static void test_minimize(void);

      static void write_in_att_format(StdVectorFst * t, std::ostream &os);
      static void write_in_att_format_number(StdVectorFst * t, std::ostream &os);

      static StdVectorFst * read_in_att_format(FILE *ifile);
      
      static bool are_equivalent(StdVectorFst *one, StdVectorFst *another);
      static bool is_cyclic(StdVectorFst * t);
      
      static FdTable<int64>* get_flag_diacritics(StdVectorFst * t);

      // string versions
      static StdVectorFst * insert_freely(StdVectorFst * t, const StringPair &symbol_pair);
      static StdVectorFst * substitute(StdVectorFst * t, std::string old_symbol, std::string new_symbol);
      static StdVectorFst * substitute(StdVectorFst * t,
				       StringPair old_symbol_pair,
				       StringPair new_symbol_pair);
      static StdVectorFst * substitute(StdVectorFst * t,
				       StringPair old_symbol_pair,
				       StringPairSet new_symbol_pair_set);
      static StdVectorFst * substitute(StdVectorFst * t,
				       const StringPair old_symbol_pair,
				       StdVectorFst *transducer);
      static StdVectorFst * substitute(StdVectorFst *t, void (*func)(std::string &isymbol, std::string &osymbol) );

      // number versions
      static StdVectorFst * insert_freely(StdVectorFst * t, const NumberPair &number_pair);
      static StdVectorFst * substitute(StdVectorFst * t, unsigned int, unsigned int);
      static StdVectorFst * substitute(StdVectorFst * t,
				       NumberPair old_number_pair,
				       NumberPair new_number_pair);
      static StdVectorFst * substitute(StdVectorFst * t,
				       const NumberPair old_number_pair,
				       StdVectorFst *transducer);

      static StringSet get_string_set(StdVectorFst *t);
      static NumberNumberMap create_mapping(StdVectorFst * t1, StdVectorFst * t2);
      //static std::vector<unsigned int> create_mapping(StdVectorFst * t1, StdVectorFst * t2);
      static void recode_symbol_numbers(StdVectorFst * t, hfst::NumberNumberMap &km);      
      //static void recode_symbol_numbers(StdVectorFst * t, std::vector<unsigned int> &km);      
      static StdVectorFst * expand_arcs(StdVectorFst * t, hfst::StringSet &unknown, bool unknown_symbols_in_use);

      static StdVectorFst * compose_intersect(StdVectorFst * t,
					      Grammar * grammar);

      static StdVectorFst * remove_from_alphabet(StdVectorFst *t, const std::string &symbol);

    private:
      static fst::SymbolTable create_symbol_table(std::string name);
      static void initialize_symbol_tables(StdVectorFst *t);
      static void add_symbol_table(StdVectorFst *t, HfstAlphabet &alpha); 
      static void remove_symbol_table(StdVectorFst *t);      

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
#endif
