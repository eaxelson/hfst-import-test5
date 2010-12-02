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

/** @file FooTransducer.h
    \brief Declarations of functions and datatypes that form a bridge between
    HFST API and library Foo. 

    This file lists the classes and functions that a new library must implement
    before added under the HFST API. Write implementations in file 
    FooTransducer.cc */

#include "HfstExceptions.h"
#include "FlagDiacritics.h"
#include "SymbolDefs.h"
#include "ExtractStrings.h"

/* Include all relevant header files of library Foo here */
//#include ...

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

namespace hfst { 
namespace implementations
{
  //using namespace Foo;

  /* A class for reading binary FooTransducers from a stream. 

     If your transducer read function uses C FILEs, see class SfstInputStream
     or FomaInputStream for an example how to implement the functions
     in class FooInputStream. If your transducer read function uses C++ streams,
     see class TropicalWeightInputStream or LogWeightInputStream.
   */
  class FooInputStream 
  {
  private:
    // you probably need
    //std::string filename;

    // if the library uses C FILEs, you probably need this
    //FILE * input_file;

    // if the library uses C++ streams, you probably need this
    //ifstream i_stream;
    //istream &input_stream;

  public:
    /* Create and open a stream to standard in. */
    FooInputStream(void);
    /* Create and open a stream to file filename. */
    FooInputStream(const char * filename);
    /* Close the stream. If it points to standard in, nothing is done. */
    void close(void);
    /* */
    bool is_eof(void);
    /* */
    bool is_bad(void);
    /* */
    bool is_good(void);
    /* */
    bool is_fst(void);

    /* Extract a char from the stream. */
    char stream_get();
    /* Put back a char to the stream. */
    void stream_unget(char c);

    /* Optional: if you want to extract implementation specific data from the header. */
    bool set_implementation_specific_header_data(StringPairVector &data, unsigned int index);

    /* Read a FooTransducer from the stream. */
    FooTransducer * read_transducer();
    
  };

  /* A class for writing binary FooTransducers to a stream. 

     If your transducer write function uses C FILEs, see class SfstOutputStream
     or FomaOuputStream for an example how to implement the functions
     in class FooOutputStream. If your transducer write function uses C++ streams,
     see class TropicalWeightOutputStream or LogWeightOutputStream.
  */
  class FooOutputStream 
  {
  private:
    // you probably need this
    //std::string filename;

    // if the library uses C FILEs, you probably need this
    //FILE *ofile;

    // if the library uses C++ streams, you probably need this
    //ofstream o_stream;
    //ostream &output_stream;

  public:
    /* Create and open a stream to standard out. */
    FooOutputStream(void); 
    /* Create and open a stream to file filename. */
    FooOutputStream(const char * filename);
    /* Close the stream. If it points to standard out, nothig is done. */
    void close(void);
    /* Write a char to the stream. */
    void write(const char &c);

    /* Optional: if you want to store implementation specific data to the header. */
    void append_implementation_specific_header_data(std::vector<char> &header, Transducer *t);

    /* Write a FooTransducer to the stream. */
    void write_transducer(FooTransducer * transducer);
  };
  
  /* A library class that contains operations for FooTransducers. */
  class FooTransducer
    {
    public:
      /* Create a transducer that does not recognise any string. */
      static FooTransducer * create_empty_transducer(void);
      /* Create a transducer that recognises the empty string. */
      static FooTransducer * create_epsilon_transducer(void);
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
      static FooTransducer * reverse(FooTransducer * t);

      /* Create a transducer that accepts string pair string1:string1 iff transducer t accepts string pair string1:string2. */
      static FooTransducer * extract_input_language(FooTransducer * t);
      /* Create a transducer that accepts string pair string2:string2 iff transducer t accepts string pair string1:string2. */
      static FooTransducer * extract_output_language(FooTransducer * t);
      /* A vector of transducers that each accept one string pair accepted by transducer t. t cannot be cyclic. */
      static std::vector<FooTransducer*> extract_paths(FooTransducer *t);
      /* TODO: document */
      static void extract_strings(FooTransducer * t, hfst::ExtractStringsCb& callback, int cycles=-1, FdTable<SFST::Character>* fd=NULL, bool filter_fd=false);

      /* Create a transducer that accepts string pair of [ A:B* s A:B* t A:B* r A:B* i A:B* n A:B* g A:B* 1:2 A:B* ] 
	 (where A and B are input and output symbol of symbol_pair) iff transducer t accepts string pair string1:string2. */
      static FooTransducer * insert_freely(FooTransducer *t , const StringPair &symbol_pair);
      /* Create a transducer equivalent to transducer t but where all symbols old_symbol are substituted with new_symbol. */
      static FooTransducer * substitute(FooTransducer * t, String old_symbol, String new_symbol);
      /* Create a transducer equivalent to transducer t but where all symbol pairs symbol_pair are substituted with
	 a copy of transducer tr. */
      static FooTransducer * substitute(FooTransducer *t, const StringPair &symbol_pair, FooTransducer *tr);

      /* Create a transducer that accepts string pair string1:string3 iff t1 accepts string pair string1:string2
	 and t2 accepts string pair string2:string3, where string2 is any string. */
      static FooTransducer * compose(FooTransducer * t1,
			   FooTransducer * t2);
      /* Create a transducer that accepts a concatenation of any string pair accepted by t1
	 and any string pair accepted by t2. */
      static FooTransducer * concatenate(FooTransducer * t1,
			       FooTransducer * t2);
      /* Create a transducer that accepts any string pair accepted by t1 or t2. */
      static FooTransducer * disjunct(FooTransducer * t1,
			    FooTransducer * t2);
      /* Create a transducer that accepts any string pair accepted by both t1 and t2. */
      static FooTransducer * intersect(FooTransducer * t1,
			     FooTransducer * t2);
      /* Create a transducer that accepts any string pair accepted by t1 but not t2. */
      static FooTransducer * subtract(FooTransducer * t1,
			    FooTransducer * t2);

      /* */
      static std::pair<Transducer*, Transducer*> harmonize(FooTransducer *t1, FooTransducer *t2, bool unknown_symbols_in_use=true);

      /* Whether transducers t1 an t2 are equivalent. */
      static bool are_equivalent(FooTransducer * t1, FooTransducer * t2);
      /* Whether transducer t is cyclic. */
      static bool is_cyclic(FooTransducer * t);
      
      /* A table of FooTransitionNumbers that represent flag diacritics in transducer t. */
      static FdTable<FooTransitionNumber>* get_flag_diacritics(FooTransducer * t);

      /* Remove symbol symbol from the alphabet of transducer t. */
      static FooTransducer * remove_from_alphabet(FooTransducer *t, const std::string &symbol);

      /* */
      static FooTransducer * disjunct(FooTransducer * t, const StringPairVector &spv);

      /* Get all symbol pairs that occur in transitions of transducer t. */
      static StringPairSet get_symbol_pairs(FooTransducer *t);

      /* The number of states in transducer t. */
      static unsigned int number_of_states(FooTransducer *t);

      // you probably need also these functions
    protected:
      /* Add the following number-to-symbol correspondencies to the alphabet of transducer t: 
	 0 : "@_EPSILON_SYMBOL_@"
	 1 : "@_UNKNOWN_SYMBOL_@"
	 2 : "@_IDENTITY_SYMBOL_@"  */
      static void initialize_alphabet(FooTransducer *t);
      /* Get all symbols that occur in transitions of transducer t. */
      static StringSet get_string_set(FooTransducer *t);

    };
} }
#endif
