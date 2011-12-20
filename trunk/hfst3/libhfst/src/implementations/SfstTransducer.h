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

#ifndef _SFST_TRANSDUCER_H_
#define _SFST_TRANSDUCER_H_

#include "HfstExceptionDefs.h"
#include "HfstFlagDiacritics.h"
#include "HfstSymbolDefs.h"
#include "HfstExtractStrings.h"
#include "sfst/interface.h"
#include "sfst/fst.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

/** @file SfstTransducer.h
    \brief Declarations of functions and datatypes that form a bridge between
    HFST API and SFST. */

namespace hfst { 
namespace implementations
{
  using namespace SFST;
  ;
  using std::ostream;
  using std::ostringstream;

  void sfst_set_hopcroft(bool);

  class SfstInputStream 
  {
  private:
    std::string filename;
    FILE * input_file;
    bool is_minimal;  // whether the next transducer in the stream is minimal
                      // this can be said in the header
    void add_symbol(StringNumberMap &string_number_map,
                    Character c,
                    Alphabet &alphabet);

  public:
    SfstInputStream(void);
    SfstInputStream(const std::string &filename);
    void close(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    void ignore(unsigned int);

    char stream_get();
    short stream_get_short();
    void stream_unget(char c);

    bool set_implementation_specific_header_data
      (StringPairVector &data, unsigned int index);
    Transducer * read_transducer();
    
    static bool is_fst(FILE * f);
    static bool is_fst(std::istream &s);
  };

  class SfstOutputStream 
  {
  private:
    std::string filename;
    FILE *ofile;
    //void write_3_0_library_header(FILE *file, bool is_minimal);
  public:
    SfstOutputStream(void); 
    SfstOutputStream(const std::string &filename);
    void close(void);
    void write(const char &c);
    void append_implementation_specific_header_data
      (std::vector<char> &header, Transducer *t);
    void write_transducer(Transducer * transducer);
  };

  class HfstNode2Int {
    
    struct hashf {
      size_t operator()(const Node *node) const {
    return (size_t)node;
      }
    };
    struct equalf {
      int operator()(const Node *n1, const Node *n2) const {
    return (n1 == n2);
      }
    };
    typedef hash_map<Node*, int, hashf, equalf> NL;
    
  private:
    NL number;
    
  public:
    int &operator[]( Node *node ) {
      NL::iterator it=number.find(node);
      if (it == number.end())
    return number.insert(NL::value_type(node, 0)).first->second;
      return it->second;
    };
  };
  
  
  class SfstTransducer
    {
    public:
      static Transducer * create_empty_transducer(void);
      static Transducer * create_epsilon_transducer(void);

      static Transducer * define_transducer(unsigned int number);
      static Transducer * define_transducer
        (unsigned int inumber, unsigned int onumber);

      static Transducer * define_transducer(const std::string &symbol);
      static Transducer * define_transducer
        (const std::string &isymbol, const std::string &osymbol);
      static Transducer * define_transducer
        (const StringPairVector &spv);
      static Transducer * define_transducer
        (const StringPairSet &sps, bool cyclic=false);
      static Transducer * define_transducer
        (const std::vector<StringPairSet> &spsv);
      static Transducer * copy(Transducer * t);
      static Transducer * determinize(Transducer * t);
      static Transducer * minimize(Transducer * t);
      static Transducer * remove_epsilons(Transducer * t);
      static Transducer * repeat_star(Transducer * t);
      static Transducer * repeat_plus(Transducer * t);
      static Transducer * repeat_n(Transducer * t,unsigned int n);
      static Transducer * repeat_le_n(Transducer * t,unsigned int n);
      static Transducer * optionalize(Transducer * t);
      static Transducer * invert(Transducer * t);
      static Transducer * reverse(Transducer * transducer);
      static Transducer * extract_input_language(Transducer * t);
      static Transducer * extract_output_language(Transducer * t);
      static std::vector<Transducer*> extract_path_transducers
        (Transducer *t);
      static void extract_paths
        (Transducer * t, hfst::ExtractStringsCb& callback, int cycles=-1,
         FdTable<SFST::Character>* fd=NULL, bool filter_fd=false);

      static void extract_random_paths
    (Transducer *t, HfstTwoLevelPaths &results, int max_num);

      static Transducer * insert_freely
        (Transducer *t , const StringPair &symbol_pair);
      static Transducer * substitute
        (Transducer * t, String old_symbol, String new_symbol);
      static Transducer * substitute
        (Transducer *t, const StringPair &symbol_pair, Transducer *tr);

      static Transducer * compose(Transducer * t1,
                           Transducer * t2);
      static Transducer * concatenate(Transducer * t1,
                               Transducer * t2);
      static Transducer * disjunct(Transducer * t1,
                            Transducer * t2);
      static Transducer * intersect(Transducer * t1,
                             Transducer * t2);
      static Transducer * subtract(Transducer * t1,
                            Transducer * t2);
      static std::pair<Transducer*, Transducer*> harmonize
        (Transducer *t1, Transducer *t2, bool unknown_symbols_in_use=true);

      static bool are_equivalent(Transducer * t1, Transducer * t2);
      static bool is_cyclic(Transducer * t);
      
      static FdTable<SFST::Character>* get_flag_diacritics(Transducer * t);

      static void print_test(Transducer *t);
      static void print_alphabet(Transducer *t);

      static unsigned int get_biggest_symbol_number(Transducer * t);

      static StringVector get_symbol_vector(Transducer * t);


      static Transducer * disjunct(Transducer * t, const StringPairVector &spv);

      static StringPairSet get_symbol_pairs(Transducer *t);

      float get_profile_seconds();
      static unsigned int number_of_states(Transducer *t);

      static StringSet get_alphabet(Transducer *t);
      static void insert_to_alphabet(Transducer *t, const std::string &symbol);
      static void remove_from_alphabet
    (Transducer *t, const std::string &symbol);
      static unsigned int get_symbol_number(Transducer *t, 
                        const std::string &symbol);

    protected:
      static void initialize_alphabet(Transducer *t);
      static Transducer * expand_arcs(Transducer * t, StringSet &unknown);

      static void expand_node( Transducer *t, Node *origin, Label &l, 
                               Node *target, hfst::StringSet &s );
      static void expand2
        ( Transducer *t, Node *node,
          hfst::StringSet &new_symbols, std::set<Node*> &visited_nodes );
      static void expand(Transducer *t, hfst::StringSet &new_symbols);

    };
} }
#endif
