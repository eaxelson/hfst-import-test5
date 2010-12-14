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

/** @file MfstlTransducer.cc
    \brief Implementations of functions and datatypes that form a bridge between
    HFST API and library MFSTL. */

#include "MfstlTransducer.h"

#ifndef DEBUG_MAIN
namespace hfst { 
namespace implementations
{
  using namespace hfst::exceptions;

  /* Create and open a stream to standard in. */
  MfstlInputStream::MfstlInputStream(void) {
    throw FunctionNotImplementedException(); }    
  
  /* Create and open a stream to file filename. */
  MfstlInputStream::MfstlInputStream(const std::string &filename) {
    (void)filename;
    throw FunctionNotImplementedException(); }      
  
  /* Close the stream. If it points to standard in, nothing is done. */
  void MfstlInputStream::close(void) {
    throw FunctionNotImplementedException(); }
  
  /* */
  bool MfstlInputStream::is_eof(void) {
    throw FunctionNotImplementedException(); }
  
  /* */
  bool MfstlInputStream::is_bad(void) {
    throw FunctionNotImplementedException(); }
  
  /* */
  bool MfstlInputStream::is_good(void) {
    throw FunctionNotImplementedException(); }
  
  /* */
  bool MfstlInputStream::is_fst(void) {
    throw FunctionNotImplementedException(); }
    
  /* Extract a char from the stream. */
  char MfstlInputStream::stream_get() {
    throw FunctionNotImplementedException(); }
  
  /* Put back a char to the stream. */
  void MfstlInputStream::stream_unget(char c) {
    (void)c;
    throw FunctionNotImplementedException(); }

  /* Extract n chars and ignore them. */
  void MfstlInputStream::ignore(unsigned int n) {
    (void)n;
    throw FunctionNotImplementedException(); }
  
  /* Optional: if you want to extract implementation specific data from the header. */
  bool MfstlInputStream::set_implementation_specific_header_data(StringPairVector &data, unsigned int index) {
    (void)data;
    (void)index;
    throw FunctionNotImplementedException(); }
  
  /* Read a MyFst from the stream. */
  MyFst * MfstlInputStream::read_transducer() {
    throw FunctionNotImplementedException(); }
  


  /* Create and open a stream to standard out. */
  MfstlOutputStream::MfstlOutputStream(void) {
    throw FunctionNotImplementedException(); }

  /* Create and open a stream to file filename. */
  MfstlOutputStream::MfstlOutputStream(const std::string &filename) {
    (void)filename;
    throw FunctionNotImplementedException(); }
  
  /* Close the stream. If it points to standard out, nothing is done. */
  void MfstlOutputStream::close(void) {
    throw FunctionNotImplementedException(); }
  
  /* Write a char to the stream. */
  void MfstlOutputStream::write(const char &c) {
    (void)c;
    throw FunctionNotImplementedException(); }  
  
  /* Optional: if you want to store implementation specific data to the header. */
  void MfstlOutputStream::append_implementation_specific_header_data(std::vector<char> &header, MyFst *t) {
    (void)header;
    (void)t;
    throw FunctionNotImplementedException(); }
  
  /* Write a MyFst to the stream. */
  void MfstlOutputStream::write_transducer(MyFst * transducer) {
    (void)transducer;
    throw FunctionNotImplementedException(); }
  

  
  /* Create a transducer that does not recognise any string. */
  MyFst * MfstlTransducer::create_empty_transducer(void) {
    throw FunctionNotImplementedException(); }

  /* Create a transducer that recognises the empty string. */
  MyFst * MfstlTransducer::create_epsilon_transducer(void) {
    throw FunctionNotImplementedException(); }
    
  /* Create a transducer that recognises the symbol pair symbol:symbol */
  MyFst * MfstlTransducer::define_transducer(const std::string &symbol) {
    (void)symbol;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that recognises the symbol pair isymbol:osymbol */
  MyFst * MfstlTransducer::define_transducer(const std::string &isymbol, const std::string &osymbol) {
    (void)isymbol;
    (void)osymbol;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that recognises the concatenation of symbol pairs in spv */
  MyFst * MfstlTransducer::define_transducer(const StringPairVector &spv) {
    (void)spv;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that recognises the union of symbol pairs in sps 
     If cyclic is true, any number of consecutive symbol pairs is recognised. */
  MyFst * MfstlTransducer::define_transducer(const StringPairSet &sps, bool cyclic) {
    (void)sps;
    (void)cyclic;
    throw FunctionNotImplementedException(); }

  /* Crate a transducer that recognises the concatenation of symbol pair unions in spsv. */
  MyFst * MfstlTransducer::define_transducer(const std::vector<StringPairSet> &spsv) {
    (void)spsv;
    throw FunctionNotImplementedException(); }

  /* Create a deep copy of transducer t. */
  MyFst * MfstlTransducer::copy(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a deterministic transducer equivalent to transducer t. */
  MyFst * MfstlTransducer::determinize(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a minimal transducer equivalent to transducer t. */
  MyFst * MfstlTransducer::minimize(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create an epsilon-free transducer equivalent to transducer t. */
  MyFst * MfstlTransducer::remove_epsilons(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts any number of consecutive string pairs accepted by transducer t. */
  MyFst * MfstlTransducer::repeat_star(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts one or more consecutive string pairs accepted by transducer t. */
  MyFst * MfstlTransducer::repeat_plus(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts n consecutive string pairs accepted by transducer t. */
  MyFst * MfstlTransducer::repeat_n(MyFst * t,int n) {
    (void)t;
    (void)n;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts from zero to n consecutive string pairs accepted by transducer t. */
  MyFst * MfstlTransducer::repeat_le_n(MyFst * t,int n) {
    (void)t;
    (void)n;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts string pairs accepted by transducer t or an empty string. */
  MyFst * MfstlTransducer::optionalize(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts string pair string2:string1 iff transducer t accepts string pair string1:string2. */
  MyFst * MfstlTransducer::invert(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts string pair 1gnirts:2gnirts iff transducer t accepts string pair string1:string2. */
  MyFst * MfstlTransducer::reverse(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts string pair string1:string1 iff transducer t accepts string pair string1:string2. */
  MyFst * MfstlTransducer::extract_input_language(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts string pair string2:string2 iff transducer t accepts string pair string1:string2. */
  MyFst * MfstlTransducer::extract_output_language(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* A vector of transducers that each accept one string pair accepted by transducer t. t cannot be cyclic. */
  std::vector<MyFst*> MfstlTransducer::extract_paths(MyFst *t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* TODO: document */
  void MfstlTransducer::extract_strings(MyFst * t, hfst::ExtractStringsCb& callback, 
				      int cycles, FdTable<unsigned int>* fd, bool filter_fd) {
    (void)t;
    (void)callback;
    (void)cycles;
    (void)fd;
    (void)filter_fd;
    throw FunctionNotImplementedException(); }  

  /* Create a transducer that accepts string pair of [ A:B* s A:B* t A:B* r A:B* i A:B* n A:B* g A:B* 1:2 A:B* ] 
     (where A and B are input and output symbol of symbol_pair) iff transducer t accepts string pair string1:string2. */
  MyFst * MfstlTransducer::insert_freely(MyFst *t , const StringPair &symbol_pair) {
    (void)t;
    (void)symbol_pair;
    throw FunctionNotImplementedException(); }

  /* Create a transducer equivalent to transducer t but where all symbols old_symbol are substituted with new_symbol. */
  MyFst * MfstlTransducer::substitute(MyFst * t, String old_symbol, String new_symbol) {
    (void)t;
    (void)old_symbol;
    (void)new_symbol;
    throw FunctionNotImplementedException(); }

  /* Create a transducer equivalent to transducer t but where all symbol pairs symbol_pair are substituted with
     a copy of transducer tr. */
  MyFst * MfstlTransducer::substitute(MyFst *t, const StringPair &symbol_pair, MyFst *tr) {
    (void)t;
    (void)symbol_pair;
    (void)tr;
    throw FunctionNotImplementedException(); }
  
  /* Create a transducer that accepts string pair string1:string3 iff t1 accepts string pair string1:string2
     and t2 accepts string pair string2:string3, where string2 is any string. */
  MyFst * MfstlTransducer::compose(MyFst * t1,
					 MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts a concatenation of any string pair accepted by t1
     and any string pair accepted by t2. */
  MyFst * MfstlTransducer::concatenate(MyFst * t1,
					     MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts any string pair accepted by t1 or t2. */
  MyFst * MfstlTransducer::disjunct(MyFst * t1,
					  MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts any string pair accepted by both t1 and t2. */
  MyFst * MfstlTransducer::intersect(MyFst * t1,
					   MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }

  /* Create a transducer that accepts any string pair accepted by t1 but not t2. */
  MyFst * MfstlTransducer::subtract(MyFst * t1,
					  MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }
  
  /* See file MyFst.h */
  std::pair<MyFst*, MyFst*> MfstlTransducer::harmonize(MyFst *t1, MyFst *t2, bool unknown_symbols_in_use) {
    (void)t1;
    (void)t2;
    (void)unknown_symbols_in_use;
    throw FunctionNotImplementedException(); }

  /* Whether transducers t1 an t2 are equivalent. */
  bool MfstlTransducer::are_equivalent(MyFst * t1, MyFst * t2) {
    (void)t1;
    (void)t2;
    throw FunctionNotImplementedException(); }

  /* Whether transducer t is cyclic. */
  bool MfstlTransducer::is_cyclic(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }  

  /* A table of MfstlTransitionNumbers that represent flag diacritics in transducer t. */
  FdTable<unsigned int>* MfstlTransducer::get_flag_diacritics(MyFst * t) {
    (void)t;
    throw FunctionNotImplementedException(); }  

  /* Remove symbol symbol from the alphabet of transducer t. */
  MyFst * MfstlTransducer::remove_from_alphabet(MyFst *t, const std::string &symbol) {
    (void)t;
    (void)symbol;
    throw FunctionNotImplementedException(); }  

  /* Disjunct t with a path transducer spv. */
  MyFst * MfstlTransducer::disjunct(MyFst * t, const StringPairVector &spv) {
    (void)t;
    (void)spv;
    throw FunctionNotImplementedException(); }  

  /* Get all symbol pairs that occur in transitions of transducer t. */
  StringPairSet MfstlTransducer::get_symbol_pairs(MyFst *t) {
    (void)t;
    throw FunctionNotImplementedException(); }  

  /* The number of states in transducer t. */
  unsigned int MfstlTransducer::number_of_states(MyFst *t) {
    (void)t;
    throw FunctionNotImplementedException(); }
  
  /* Add the following number-to-symbol correspondencies to the alphabet of transducer t: 
     0 : "@_EPSILON_SYMBOL_@"
     1 : "@_UNKNOWN_SYMBOL_@"
     2 : "@_IDENTITY_SYMBOL_@"  */
  void MfstlTransducer::initialize_alphabet(MyFst *t) {
    (void)t;
    throw FunctionNotImplementedException(); }

  /* Get all symbols that occur in transitions of transducer t. */
  StringSet MfstlTransducer::get_string_set(MyFst *t) {
    (void)t;
    throw FunctionNotImplementedException(); }

}
}

#else
#include <cstdlib>
#include <cassert>
#include <iostream>

int
main(void)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
  }
#endif


