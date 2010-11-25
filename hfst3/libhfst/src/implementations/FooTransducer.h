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

#ifndef _FOO_TRANSDUCER_H_
#define _FOO_TRANSDUCER_H_

#include "HfstExceptions.h"
#include "FlagDiacritics.h"
#include "SymbolDefs.h"
#include "ExtractStrings.h"

// all relevant header files of library Foo
//#include ...

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

/** @file FooTransducer.h
    \brief Declarations of functions and datatypes that form a bridge between
    HFST API and library Foo. */

namespace hfst { 
namespace implementations
{
  using namespace Foo;
  using namespace hfst::exceptions;
  using std::ostream;
  using std::ostringstream;

  class FooInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    // relevant variables and functions...

  public:
    FooInputStream(void);
    FooInputStream(const char * filename);
    void close(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    void ignore(unsigned int);

    char stream_get();
    void stream_unget(char c);

    //bool set_implementation_specific_header_data(StringPairVector &data, unsigned int index);
    FooTransducer * read_transducer();
    
    static bool is_fst(FILE * f);
    static bool is_fst(std::istream &s);
  };

  class FooOutputStream 
  {
  private:
    std::string filename;
    FILE *ofile;
  public:
    FooOutputStream(void); 
    FooOutputStream(const char * filename);
    void close(void);
    void write(const char &c);
    //void append_implementation_specific_header_data(std::vector<char> &header, Transducer *t);
    void write_transducer(FooTransducer * transducer);
  };
  
  class FooTransducer
    {
    public:
      /* Create a transducer that does not recognise any string. */
      static FooTransducer * create_empty_transducer(void);
      /* Create a transducer that recognises the empty string. */
      static FooTransducer * create_epsilon_transducer(void);

      // static FooTransducer * define_transducer(unsigned int number);
      // static FooTransducer * define_transducer(unsigned int inumber, unsigned int onumber);

      /* Create a transducer that recognises the symbol pair symbol:symbol */
      static FooTransducer * define_transducer(const char *symbol);
      /* Create a transducer that recognises the symbol pair isymbol:osymbol */
      static FooTransducer * define_transducer(const char *isymbol, const char *osymbol);
      /* Create a transducer that recognises the concatenation of symbol pairs in spv */
      static FooTransducer * define_transducer(const StringPairVector &spv);
      /* Create a transducer that recognises the union of symbol pairs in sps 
	 If cyclic is true, any number of consecutive symbol pairs is recognised. */
      static FooTransducer * define_transducer(const StringPairSet &sps, bool cyclic=false);
      /* Crate a transducer that recognises the concatenation of symbol pair unions in spsv. */
      static FooTransducer * define_transducer(const std::vector<StringPairSet> &spsv);

      /* Create a deep copy of transducer t. */
      static FooTransducer * copy(FooTransducer * t);
      /* Create a deterministic transducer equivalent to transducer t. */
      static FooTransducer * determinize(FooTransducer * t);
      /* Create a minimal transducer equivalent to transducer t. */
      static FooTransducer * minimize(FooTransducer * t);
      /* Create an epsilon-free transducer equivalent to transducer t. */
      static FooTransducer * remove_epsilons(FooTransducer * t);

      /* Create a transducer that accepts any number of consecutive string pairs accepted by transducer t. */
      static FooTransducer * repeat_star(FooTransducer * t);
      /* Create a transducer that accepts one or more consecutive string pairs accepted by transducer t. */
      static FooTransducer * repeat_plus(FooTransducer * t);
      /* Create a transducer that accepts n consecutive string pairs accepted by transducer t. */
      static FooTransducer * repeat_n(FooTransducer * t,int n);
      /* Create a transducer that accepts from zero to n consecutive string pairs accepted by transducer t. */
      static FooTransducer * repeat_le_n(FooTransducer * t,int n);

      /* Create a transducer that accepts string pairs accepted by transducer t or an empty string. */
      static FooTransducer * optionalize(FooTransducer * t);
      /* Create a transducer that accepts string pair string2:string1 iff transducer t accepts string pair string1:string2. */
      static FooTransducer * invert(FooTransducer * t);
      /* Create a transducer that accepts string pair 1gnirts:2gnirts iff transducer t accepts string pair string1:string2. */
      static FooTransducer * reverse(FooTransducer * transducer);

      /* Create a transducer that accepts string pair string1:string1 iff transducer t accepts string pair string1:string2. */
      static FooTransducer * extract_input_language(FooTransducer * t);
      /* Create a transducer that accepts string pair string2:string2 iff transducer t accepts string pair string1:string2. */
      static FooTransducer * extract_output_language(FooTransducer * t);
      /* A vector of transducers that each accept one string pair accepted by transducer t. t cannot be cyclic. */
      static std::vector<FooTransducer*> extract_paths(FooTransducer *t);
      /* TODO: document */
      static void extract_strings(FooTransducer * t, hfst::ExtractStringsCb& callback, int cycles=-1, FdTable<SFST::Character>* fd=NULL, bool filter_fd=false);

      /* Create a transducer that */
      static FooTransducer * insert_freely(FooTransducer *t , const StringPair &symbol_pair);
      static FooTransducer * substitute(FooTransducer * t, String old_symbol, String new_symbol);
      static FooTransducer * substitute(FooTransducer *t, const StringPair &symbol_pair, FooTransducer *tr);

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
      static std::pair<Transducer*, Transducer*> harmonize(FooTransducer *t1, FooTransducer *t2, bool unknown_symbols_in_use=true);

      static bool are_equivalent(FooTransducer * t1, FooTransducer * t2);
      static bool is_cyclic(FooTransducer * t);
      
      static FdTable<FooTransitionNumber>* get_flag_diacritics(FooTransducer * t);

      static FooTransducer * remove_from_alphabet(FooTransducer *t, const std::string &symbol);

      static FooTransducer * disjunct(FooTransducer * t, const StringPairVector &spv);

      static StringPairSet get_symbol_pairs(FooTransducer *t);

      float get_profile_seconds();
      static unsigned int number_of_states(FooTransducer *t);

    protected:
      static void initialize_alphabet(FooTransducer *t);
      static StringSet get_string_set(FooTransducer *t);
      static FooTransducer * expand_arcs(FooTransducer * t, StringSet &unknown);

      static void expand_node( FooTransducer *t, Node *origin, Label &l, 
			       Node *target, hfst::StringSet &s );
      static void expand2( FooTransducer *t, Node *node,
			    hfst::StringSet &new_symbols, std::set<Node*> &visited_nodes );
      static void expand(FooTransducer *t, hfst::StringSet &new_symbols);

    };
} }
#endif
