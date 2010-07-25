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

#include "FomaTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {


  // ---------- FomaInputStream functions ----------

    /** Create a FomaInputStream that reads from stdin. */
  FomaInputStream::FomaInputStream(void)
  {
    this->input_file = stdin;
  }
    /** Create a FomaInputStream that reads from file \a filename. */
  FomaInputStream::FomaInputStream(const char * filename):
  filename(filename)
  {
    input_file = NULL;
  }
    /** Open the stream. */
  void FomaInputStream::open(void)
  {
    if (filename == std::string())
      { return; }
    input_file = fopen(filename.c_str(),"r");
    if (input_file == NULL)
      { throw FileNotReadableException(); }
  }
    /** Close the stream. */
  void FomaInputStream::close(void)
  {
    if (input_file == NULL)
      { return; }
    if (filename.c_str()[0] != 0)
      {
	fclose(input_file);
	input_file = NULL;
      }
  }
    /** Whether the stream is open. */
  bool FomaInputStream::is_open(void)
  {
    return input_file != NULL;
  }
  
  bool FomaInputStream::is_eof(void)
  {
    if (not is_open())
      { return true; }
    int c = getc(input_file);
    bool retval = (feof(input_file) != 0);
    ungetc(c, input_file);
    return retval;
  }
  
  bool FomaInputStream::is_bad(void)
  {
    return is_eof();
  }
  
  bool FomaInputStream::is_good(void)
  {
    return not is_bad();
  };
  
  bool FomaInputStream::is_fst(void)
  {
    return is_fst(input_file);
  }
  
  bool FomaInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == 31 || c == (int)'#';
  }
  
  bool FomaInputStream::is_fst(std::istream &s)
  {
    int c = s.peek();
    return s.good() && (c == 31 || c == (int)'#');
  }

  /* Skip the identifier string "FOMA_TYPE" */
  void FomaInputStream::skip_identifier_version_3_0(void)
  { 
    char foma_identifier[10];
    int foma_id_count = fread(foma_identifier,10,1,input_file);
    if (foma_id_count != 1)
      { throw NotTransducerStreamException(); }
    if (0 != strcmp(foma_identifier,"FOMA_TYPE"))
      { throw NotTransducerStreamException(); }
  }
  
  void FomaInputStream::skip_hfst_header(void)
  {
    char hfst_header[6];
    int header_count = fread(hfst_header,6,1,input_file);
    if (header_count != 1)
      { throw NotTransducerStreamException(); }
    //int c = fgetc(input_file);
    //switch (c)
    //{
    // case 0:
    try { skip_identifier_version_3_0(); }
    catch (NotTransducerStreamException e) { throw e; }
    //break;
    //default:
    //assert(false);
    //}
  }

  fsm * FomaInputStream::read_transducer(bool has_header)
  {
    if ( (not is_open()) )
      throw FileIsClosedException();
    if (is_eof())
      return NULL;
    if (has_header)
      skip_hfst_header();
    struct fsm * t = read_net_hfst(input_file);
    if (t == NULL)
      throw NotTransducerStreamException();
    return t;
  };

  // ------------------------------------------------


    /*  
  fsm * FomaTransducer::harmonize(fsm * t, KeyMap &key_map)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
    */
    /*fsm * FomaInputStream::read_transducer(KeyTable &key_table)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
    }*/




  // ---------- FomaOutputStream functions ----------

  FomaOutputStream::FomaOutputStream(void)
  {}
  FomaOutputStream::FomaOutputStream(const char * str):
    filename(str)
  {}
  void FomaOutputStream::open(void) {
    if (filename != std::string()) {
      ofile = fopen(filename.c_str(), "wb");
      if (ofile == NULL)
	throw FileNotReadableException();
    } 
    else {
      ofile = stdout;
    }
  }
  void FomaOutputStream::close(void) 
  {
    if (filename != std::string())
      { fclose(ofile); }
  }
  void FomaOutputStream::write_3_0_library_header(FILE *file)
  {
    fputs("HFST3",file);
    fputc(0, file);
    fputs("FOMA_TYPE",file);
    fputc(0, file);
  }
  void FomaOutputStream::write_transducer(struct fsm * transducer) 
  { 
    write_3_0_library_header(ofile);
    if (1 != write_net_hfst(transducer, ofile))
      throw hfst::exceptions::HfstInterfaceException();
  }

  // ------------------------------------------------


    /*  
  FomaState::FomaState(FomaNode state, fsm * t) 
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  FomaState::FomaState(const FomaState &s)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaState::get_final_weight(void) const
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaState::operator< (const FomaState &another) const
  {    throw hfst::exceptions::FunctionNotImplementedException();};
  
  bool FomaState::operator== (const FomaState &another) const 
  {    throw hfst::exceptions::FunctionNotImplementedException();};
  
  bool FomaState::operator!= (const FomaState &another) const 
  {    throw hfst::exceptions::FunctionNotImplementedException();};

  //TransitionIterator<Transducer,bool> FomaState::begin(void) const 
  FomaTransitionIterator FomaState::begin(void) const 
  {   throw hfst::exceptions::FunctionNotImplementedException();};
  
  //TransitionIterator<Transducer*,bool> FomaState::end(void) const 
  FomaTransitionIterator FomaState::end(void) const 
  {    throw hfst::exceptions::FunctionNotImplementedException();} 

  void FomaState::print(KeyTable &key_table, ostream &out,
			FomaStateIndexer &indexer) const
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  FomaTransition::FomaTransition(FomaArc arc, FomaNode n, fsm * t)
  {  }

  
  FomaTransition::FomaTransition(const FomaTransition &t)
  {  }

  
  Key FomaTransition::get_input_key(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  
  Key FomaTransition::get_output_key(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  
  FomaState FomaTransition::get_target_state(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  
  FomaState FomaTransition::get_source_state(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();}


  bool FomaTransition::get_weight(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  void FomaTransition::print(KeyTable &key_table, ostream &out,
			     FomaStateIndexer &indexer) const
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  FomaTransitionIterator::FomaTransitionIterator(FomaNode state, fsm * t) : 
    state(state), arc_iterator(0), t(t), end_iterator(false) 
  { }
  
  FomaTransitionIterator::FomaTransitionIterator (void) :
    state(NULL), arc_iterator(), t(NULL), end_iterator(true)
  {  }
  
  void FomaTransitionIterator::operator= 
  (const FomaTransitionIterator &another)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
    */  
  /* This requires the SFST ArcsIter operator* to be
     const qualified. */
    /*
  bool FomaTransitionIterator::operator== 
  (const FomaTransitionIterator &another)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaTransitionIterator::operator!= 
  (const FomaTransitionIterator &another)
  {    throw hfst::exceptions::FunctionNotImplementedException();}
  
  
  const FomaTransition 
  FomaTransitionIterator::operator* (void)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  void FomaTransitionIterator::operator++ (void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  void FomaTransitionIterator::operator++ (int)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
    }*/
  
  fsm * FomaTransducer::create_empty_transducer(void)
  {    
    struct fsm * retval = fsm_empty_set();
  }
  
  fsm * FomaTransducer::create_epsilon_transducer(void)
  {
    return fsm_empty_string();
  }
  
  void FomaTransducer::delete_foma(struct fsm * net)
  {
    fsm_destroy(net);
  }

  // remove?
  /*fsm * FomaTransducer::define_transducer(Key k)
    {     throw hfst::exceptions::FunctionNotImplementedException();}*/

  fsm * FomaTransducer::define_transducer(char *symbol)
  {     
    return fsm_symbol(symbol);
  }

  
  // remove?
  /*fsm * FomaTransducer::define_transducer(const KeyPair &kp)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
    }*/

  fsm * FomaTransducer::define_transducer(char *isymbol, char *osymbol)
  { 
    return fsm_cross_product( fsm_symbol(isymbol), fsm_symbol(osymbol) );
    // should either argument be deleted?
  }
  
    /*
  fsm * FomaTransducer::define_transducer(const KeyPairVector &kpv)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
    }*/

    // This could be much more efficient...
  fsm * FomaTransducer::define_transducer(const StringPairVector &spv)
  {
    struct fsm * retval = fsm_empty_string();
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	retval = fsm_concat( retval, fsm_cross_product( fsm_symbol(strdup(it->first.c_str())), fsm_symbol(strdup(it->second.c_str())) ) );
      }
    return retval;
  }

  fsm * FomaTransducer::define_transducer(const StringPairSet &sps)
  {
    if (sps.empty())
      return fsm_empty_string();

    struct fsm * retval = NULL;
    for (StringPairSet::const_iterator it = sps.begin();
	 it != sps.end();
	 ++it)
      {
	if (retval == NULL)
	  retval = fsm_cross_product( fsm_symbol(strdup(it->first.c_str())), fsm_symbol(strdup(it->second.c_str())) );
	else
	  retval = fsm_union( retval, fsm_cross_product( fsm_symbol(strdup(it->first.c_str())), fsm_symbol(strdup(it->second.c_str())) ) );
      }
    return retval;
  }


  fsm * FomaTransducer::copy(fsm * t)
  {     
    return fsm_copy(t);
  }
  
  fsm * FomaTransducer::determinize(fsm * t)
  {     
    // returns the argument, so a copy is taken here
    return fsm_determinize(fsm_copy(t));
  }
  
  fsm * FomaTransducer::minimize(fsm * t)
  {     
    // returns the argument, so a copy is taken here
    return fsm_minimize(fsm_copy(t));
  }
  
  fsm * FomaTransducer::remove_epsilons(fsm * t)
  {
    // returns the argument, so a copy is taken here
    return fsm_epsilon_remove(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_star(fsm * t)
  {     
    return fsm_kleene_star(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_plus(fsm * t)
  {     
    return fsm_kleene_plus(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_n(fsm * t,int n)
  {     
    return fsm_concat_n(t, n);
  }
  
  fsm * FomaTransducer::repeat_le_n(fsm * t,int n)
  { 
    return fsm_concat_m_n(t,0,n);
  }
  
  fsm * FomaTransducer::optionalize(fsm * t)
  { 
    return fsm_optionality(fsm_copy(t));
  }
  
  fsm * FomaTransducer::invert(fsm * t)
  {
    return fsm_invert(fsm_copy(t));
  }
  
  fsm * FomaTransducer::reverse(fsm * t)
  { 
    return fsm_reverse(fsm_copy(t));
  }
  
  fsm * FomaTransducer::extract_input_language(fsm * t)
  { 
    return fsm_upper(fsm_copy(t));
  }
  
  fsm * FomaTransducer::extract_output_language(fsm * t)
  {
    return fsm_lower(fsm_copy(t));
  }
  
  fsm * FomaTransducer::substitute(fsm * t,String old_symbol,String new_symbol)
  {
    return fsm_substitute_symbol(t, strdup(old_symbol.c_str()), strdup(new_symbol.c_str()));
  }
  
  
  fsm * FomaTransducer::compose
  (fsm * t1, fsm * t2)
  {
    return fsm_compose(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::concatenate
  (fsm * t1, fsm * t2)
  {
    return fsm_concat(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::disjunct
  (fsm * t1, fsm * t2)
  {
    return fsm_union(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::intersect
  (fsm * t1, fsm * t2)
  {
    return fsm_intersect(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::subtract
  (fsm * t1, fsm * t2)
  {
    return fsm_minus(fsm_copy(t1), fsm_copy(t2));
  }

  bool FomaTransducer::are_equivalent
  (fsm * t1, fsm * t2)
  {
    return fsm_isempty(fsm_union(fsm_minus(fsm_copy(t1),fsm_copy(t2)),fsm_minus(fsm_copy(t1),fsm_copy(t2))));    
  }

  bool FomaTransducer::is_cyclic(fsm * t)
  {
    return (t->pathcount == PATHCOUNT_CYCLIC);
  }

    /*
  FomaStateIterator::FomaStateIterator(fsm * t):
  node_numbering(0),t(t),current_state(0),ended(false)
  {

  }

  FomaStateIterator::FomaStateIterator(void):
  node_numbering(),t(NULL),current_state(0),ended(true)
  { }

  void FomaStateIterator::operator= (const FomaStateIterator &another)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  bool FomaStateIterator::operator== (const FomaStateIterator &another) const
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  bool FomaStateIterator::operator!= (const FomaStateIterator &another) const
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  const FomaState FomaStateIterator::operator* (void)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  void FomaStateIterator::operator++ (void)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  void FomaStateIterator::operator++ (int)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  FomaStateIterator FomaTransducer::begin(fsm * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  FomaStateIterator FomaTransducer::end(fsm * t)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  FomaStateIndexer::FomaStateIndexer(fsm * t):
    t(t),numbering(0)
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  unsigned int FomaStateIndexer::operator[] (const FomaState &state)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  const FomaState FomaStateIndexer::operator[] (unsigned int number)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  void FomaTransducer::print(fsm * t, 
			     KeyTable &key_table, ostream &out)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
    */
  void FomaTransducer::print_test(fsm * t)
  {
    net_print_att(t, stdout);
  }

  } }

#ifdef DEBUG_MAIN
using namespace hfst::implementations;
#include <iostream>
hfst::symbols::GlobalSymbolTable KeyTable::global_symbol_table;
int main(int argc, char * argv[]) 
{
  fprintf(stderr, "Testing FomaTransducer.cc functions:\n");

  // reading from file
  const char *filename = "foo.foma";
  FomaInputStream in(filename);
  in.open();
  assert(in.is_open());
  in.close();
  in.open();

  // reading from stdin
  FomaInputStream in2;
  in2.open();
  assert(in2.is_open());
  struct fsm * t2;
  while (not in2.is_eof()) {
    t2 = in2.read_transducer();
    fprintf(stderr, "  One transducer read from stdin.\n");
  }
  in2.close();
  assert(not in2.is_open());

  struct fsm * t;
  assert(not in.is_eof());
  while (not in.is_eof()) {
    t = in.read_transducer();
    fprintf(stderr, "  One transducer read from file.\n");
  }
  in.close();
  assert(not in.is_open());

  fprintf(stderr, "Test ends.\n");
  return 0;
}
#endif
