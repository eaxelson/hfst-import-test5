#include "FomaTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {
  FomaInputStream::FomaInputStream(void)
  {

  }
  
  FomaInputStream::FomaInputStream(const char * filename)
  {

  }
  
  void FomaInputStream::open(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  void FomaInputStream::close(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaInputStream::is_open(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaInputStream::is_eof(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
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
    throw hfst::exceptions::FunctionNotImplementedException();
  };
  
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

