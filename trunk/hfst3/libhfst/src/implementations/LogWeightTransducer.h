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
#ifndef _LOG_WEIGHT_TRANSDUCER_H_
#define _LOG_WEIGHT_TRANSDUCER_H_


#include "SymbolDefs.h"
#include "HfstExceptions.h"
#include "FlagDiacritics.h"
#include <fst/fstlib.h>
#include "ExtractStrings.h"
#include "LogFstTrieFunctions.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
 
namespace hfst { 
namespace implementations
{
  using namespace fst;
  using namespace hfst::exceptions;

  typedef ArcTpl<LogWeight>::StateId StateId;
  typedef ArcTpl<LogWeight> LogArc;
  typedef VectorFst<LogArc> LogFst;

  using std::ostream;
  using std::ostringstream;
  using std::stringstream;

  void openfst_log_set_hopcroft(bool value);

  //extern GlobalSymbolTable global_symbol_table;
  class LogWeightInputStream 
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
    LogWeightInputStream(void);
    LogWeightInputStream(const char * filename);
    void open(void);
    void close(void);
    bool is_open(void) const;
    bool is_eof(void) const;
    bool is_bad(void) const;
    bool is_good(void) const;
    bool is_fst(void) const;
    bool operator() (void) const;
    LogFst * read_transducer(bool has_header);
    
    static bool is_fst(FILE * f);
    static bool is_fst(istream &s);
  };

  class LogWeightOutputStream 
  {
  private:
    std::string filename;
    ofstream o_stream;
    ostream &output_stream;
    void write_3_0_library_header(std::ostream &out);
  public:
    LogWeightOutputStream(void); 
    LogWeightOutputStream(const char * filename);
    void open(void);
    void close(void);
    void write_transducer(LogFst * transducer);
  };

  class LogWeightTransitionIterator;

  typedef StateId LogWeightState;

  class LogWeightStateIterator 
    {
    protected:
      StateIterator<LogFst> * iterator;
    public:
      LogWeightStateIterator(LogFst * t);
      ~LogWeightStateIterator(void);
      void next(void);
      bool done(void);
      LogWeightState value(void);
    };
 

  class LogWeightTransition
    {
    protected:
      LogArc arc;
      LogFst * t;
    public:
      LogWeightTransition(const LogArc &arc, LogFst *t);
      ~LogWeightTransition(void);
      std::string get_input_symbol(void) const;
      std::string get_output_symbol(void) const;
      LogWeightState get_target_state(void) const;
      LogWeight get_weight(void) const;
    };


  class LogWeightTransitionIterator
    {
    protected:
      ArcIterator<LogFst> * arc_iterator;
      LogFst * t;
    public:
      LogWeightTransitionIterator(LogFst * t, StateId state);
      ~LogWeightTransitionIterator(void);
      void next(void);
      bool done(void);
      LogWeightTransition value(void);
    };
  

  class LogWeightTransducer
    {
    public:
      static LogFst * create_empty_transducer(void);
      static LogFst * create_epsilon_transducer(void);
      static LogFst * define_transducer(const std::string &symbol);
      static LogFst * define_transducer(const std::string &isymbol, const std::string &osymbol);

      /* TEST */
      static LogFst * define_transducer(unsigned int number);
      static LogFst * define_transducer(unsigned int inumber, unsigned int onumber);

      static LogFst * define_transducer(const hfst::StringPairVector &spv);
      static LogFst * define_transducer(const hfst::StringPairSet &sps, bool cyclic=false);
      static LogFst * define_transducer(const std::vector<StringPairSet> &spsv);
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
      static void extract_strings(LogFst * t, hfst::ExtractStringsCb& callback,
          int cycles=-1, FdTable<int64>* fd=NULL, bool filter_fd=false);
      static LogFst * compose(LogFst * t1,
				   LogFst * t2);
      static LogFst * concatenate(LogFst * t1,
					LogFst * t2);
      static LogFst * disjunct(LogFst * t1,
			      LogFst * t2);
      static LogFst * intersect(LogFst * t1,
			     LogFst * t2);
      static LogFst * subtract(LogFst * t1,
			    LogFst * t2);
      static LogFst * set_weight(LogFst * t,float f);
      static LogFst * set_final_weights(LogFst * t, float weight);
      static LogFst * transform_weights(LogFst * t,float (*func)(float f));
      static LogFst * push_weights(LogFst * t, bool to_initial_state);

      static std::pair<LogFst*, LogFst*> harmonize(LogFst *t1, LogFst *t2, bool unknown_symbols_in_use=true);

      static void write_in_att_format(LogFst * t, FILE *ofile);
      static void write_in_att_format_number(LogFst * t, FILE *ofile);
      
      static void test_minimize(void);

      static void write_in_att_format(LogFst * t, std::ostream &os);
      static void write_in_att_format_number(LogFst * t, std::ostream &os);

      static LogFst * read_in_att_format(FILE *ifile);
      
      static bool are_equivalent(LogFst *one, LogFst *another);
      static bool is_cyclic(LogFst * t);
      
      static FdTable<int64>* get_flag_diacritics(LogFst * t);

      static LogFst * insert_freely(LogFst * t, const StringPair &symbol_pair);
      static LogFst * substitute(LogFst * t, std::string old_symbol, std::string new_symbol);
      static LogFst * substitute(LogFst * t,
				       StringPair old_symbol_pair,
				       StringPair new_symbol_pair);
      static LogFst * substitute(LogFst * t,
				       StringPair old_symbol_pair,
				       StringPairSet new_symbol_pair_set);
      static LogFst * substitute(LogFst * t,
				       const StringPair old_symbol_pair,
				       LogFst *transducer);

      static StringSet get_string_set(LogFst *t);
      static NumberNumberMap create_mapping(LogFst * t1, LogFst * t2);
      static void recode_symbol_numbers(LogFst * t, hfst::NumberNumberMap &km);      
      static LogFst * expand_arcs(LogFst * t, hfst::StringSet &unknown);
      static LogFst * substitute(LogFst * t,unsigned int old_key,unsigned int new_key);
      static LogFst * substitute(LogFst *t, void (*func)(std::string &isymbol, std::string &osymbol) );
      static LogFst * substitute(LogFst * t,
      				       pair<unsigned int, unsigned int> old_key_pair,
      				       pair<unsigned int, unsigned int> new_key_pair);
      static void disjunct_as_tries(LogFst &t1,
				    const LogFst * t2);
      //      static LogFst * compose_intersect(LogFst * t,
      //				      Grammar * grammar);
      //static LogFst * define_transducer(Key k);
      //static LogFst * define_transducer(const KeyPair &kp);
      //static LogFst * define_transducer(const KeyPairVector &kpv);

      static LogFst * remove_from_alphabet(LogFst *t, const std::string &symbol);

    private:
      static fst::SymbolTable create_symbol_table(std::string name);
      static void initialize_symbol_tables(LogFst *t);
      
    public:
      /* For HfstMutableTransducer */
      static StateId add_state(LogFst *t);
      static void set_final_weight(LogFst *t, StateId s, float w);
      static void add_transition(LogFst *t, StateId source,
				 std::string &isymbol, std::string &osymbol, float w, StateId target);
      static float get_final_weight(LogFst *t, StateId s);
      static float is_final(LogFst *t, StateId s);
      static StateId get_initial_state(LogFst *t);
      static void represent_empty_transducer_as_having_one_state(LogFst *t);      

    };

} }
#endif
