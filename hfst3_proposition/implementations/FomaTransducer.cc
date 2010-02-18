#include "FomaTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {
    FomaInputStream::FomaInputStream(void):
      filename(std::string()), is_open_(false)
  {
  }

    FomaInputStream::FomaInputStream(const char * filename):
      filename(filename), is_open_(false)
  {
  }
  
  void FomaInputStream::open(void)
  {
    if (filename == std::string()) {  // reading from stdin 
      is_open_=true;
      // read stdin and write it to a file
      char * tempfilename = tmpnam(NULL);
      if (tempfilename == NULL) {
	fprintf(stderr, "error: could not generate a temporary filename.\n");
	throw ErrorException();
      }
      FILE * tempfile = fopen(tempfilename, "w");
      while( not feof(stdin) ) {
	int c = fgetc(stdin);
	fputc(c, tempfile);
      }
      fclose(tempfile);
      io_gz_file_to_mem(tempfilename);
      if (remove(tempfilename) != 0) {
	fprintf(stderr, "error; could not delete a temporary file.\n");
	throw ErrorException();
      }
      return;
    }
    else {
      FILE *input_file = fopen(filename.c_str(),"r");
      if (input_file == NULL)
	{ throw FileNotReadableException(); }
      fclose(input_file);  
      is_open_=true;
      io_gz_file_to_mem(strdup(filename.c_str()));    
      return;
    }
  }
  
  void FomaInputStream::close(void)
  {
    is_open_=false;
    io_free();
    return;
  }
  
  bool FomaInputStream::is_open(void)
  {
    return is_open_;
  }
  
  bool FomaInputStream::is_eof(void)
  {
    return io_buf_is_end();
  }
  
  bool FomaInputStream::is_bad(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaInputStream::is_good(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  };

  bool FomaInputStream::is_fst(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

    /*void FomaInputStream::add_symbol(StringSymbolMap &string_number_map,
				   Character c,
				   Alphabet &alphabet)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  void
  FomaInputStream::populate_key_table(KeyTable &key_table,
				      Alphabet &alphabet,
				      KeyMap &key_map)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
    }*/

  /* Skip the identifier string "FOMA_TYPE" */
    /*void FomaInputStream::skip_identifier_version_3_0(void)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  void FomaInputStream::skip_hfst_header(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }*/
  
  fsm * FomaTransducer::harmonize(fsm * t, KeyMap &key_map)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  fsm * FomaInputStream::read_transducer(KeyTable &key_table)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  fsm * FomaInputStream::read_transducer(void)
  {
    if ( (not is_open()) )
      throw FileIsClosedException();
    if (is_eof())
      return NULL;
    char *net_name;
    struct fsm * t = io_net_read(&net_name);
    if (t == NULL)
      throw NotTransducerStreamException();
    return t;
  };

    /*
  FomaOutputStream::FomaOutputStream(void):
    output_stream(std::cout)
  {}

  FomaOutputStream::FomaOutputStream(const char * str):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream)
  {}

    
  void FomaOutputStream::open(void) {}
  void FomaOutputStream::close(void) 
  {
    if (filename != string())
      { o_stream.close(); }
  }
  void FomaOutputStream::set_symbols(fsm * transducer, KeyTable &key_table) 
  {
    SymbolTable symbol_table("anonym_hfst3_symbol_table");
    for (KeyTable::const_iterator it = key_table.begin();
	 it != key_table.end();
	 ++it)
      { symbol_table.AddSymbol(key_table[it->key],it->key); }
    transducer->SetInputSymbols(&symbol_table);
    transducer->SetOutputSymbols(&symbol_table);
  }
  void FomaOutputStream::write_3_0_library_header(std::ostream &out)
  {
    out.write("HFST3",6);
    out.put(0);
    out.write("LOG_OFST_TYPE",14);
  }
  void FomaOutputStream::write_transducer(fsm * transducer, KeyTable &key_table) 
  { set_symbols(transducer,key_table);
    write_3_0_library_header(output_stream);
    transducer->Write(output_stream,FstWriteOptions()); }

  void FomaOutputStream::write_transducer(fsm * transducer) 
  { write_3_0_library_header(output_stream);
    transducer->Write(output_stream,FstWriteOptions()); }

    */
  
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
  
  /* This requires the SFST ArcsIter operator* to be
     const qualified. */
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
  }
  
  fsm * FomaTransducer::create_empty_transducer(void)
  {    
    return fsm_empty_set();
  }
  
  fsm * FomaTransducer::create_epsilon_transducer(void)
  {
    return fsm_empty_string();
  }
  
  fsm * FomaTransducer::define_transducer(Key k)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  
  fsm * FomaTransducer::define_transducer(const KeyPair &kp)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  fsm * FomaTransducer::define_transducer(const KeyPairVector &kpv)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  fsm * FomaTransducer::copy(fsm * t)
  {     
    return fsm_copy(t);
  }
  
  fsm * FomaTransducer::determinize(fsm * t)
  {     
    return fsm_determinize(t);
  }
  
  fsm * FomaTransducer::minimize(fsm * t)
  {     
    return fsm_minimize(t);
  }
  
  fsm * FomaTransducer::remove_epsilons(fsm * t)
  {
    return fsm_epsilon_remove(t);
  }
  
  fsm * FomaTransducer::repeat_star(fsm * t)
  {     
    return fsm_kleene_star(t);
  }
  
  fsm * FomaTransducer::repeat_plus(fsm * t)
  {     
    return fsm_kleene_plus(t);
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
    return fsm_optionality(t);
  }
  
  fsm * FomaTransducer::invert(fsm * t)
  {
    return fsm_invert(t);
  }
  
  fsm * FomaTransducer::reverse(fsm * t)
  { 
    return fsm_reverse(t);
  }
  
  fsm * FomaTransducer::extract_input_language(fsm * t)
  { 
    return fsm_upper(t);
  }
  
  fsm * FomaTransducer::extract_output_language(fsm * t)
  {
    return fsm_lower(t);
  }
  
  fsm * FomaTransducer::substitute(fsm * t,Key old_key,Key new_key)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  fsm * FomaTransducer::substitute
  (fsm * t,KeyPair old_key_pair,KeyPair new_key_pair)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  fsm * FomaTransducer::compose
  (fsm * t1, fsm * t2)
  {
    return fsm_compose(t1, t2);
  }

  fsm * FomaTransducer::concatenate
  (fsm * t1, fsm * t2)
  {
    return fsm_concat(t1, t2);
  }

  fsm * FomaTransducer::disjunct
  (fsm * t1, fsm * t2)
  {
    return fsm_union(t1, t2);
  }

  fsm * FomaTransducer::intersect
  (fsm * t1, fsm * t2)
  {
    return fsm_intersect(t1, t2);
  }

  fsm * FomaTransducer::subtract
  (fsm * t1, fsm * t2)
  {
    return fsm_minus(t1, t2);
  }

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
