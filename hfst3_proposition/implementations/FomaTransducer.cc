#include "FomaTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {
  FomaInputStream::FomaInputStream(void)
  {

  }
  
  FomaInputStream::FomaInputStream(const char * filename):
  filename(filename)
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

  void FomaInputStream::add_symbol(StringSymbolMap &string_number_map,
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
  }

  /* Skip the identifier string "FOMA_TYPE" */
  void FomaInputStream::skip_identifier_version_3_0(void)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  void FomaInputStream::skip_hfst_header(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::harmonize(Transducer * t, KeyMap &key_map)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  Transducer * FomaInputStream::read_transducer(KeyTable &key_table)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  Transducer * FomaInputStream::read_transducer(void)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  };
  
  FomaState::FomaState(Node * state, Transducer * t) 
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  FomaState::FomaState(const FomaState &s)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  bool FomaState::get_final_weight(void) const
  {    throw hfst::exceptions::FunctionNotImplementedException();};
  
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

  FomaTransition::FomaTransition(Arc * arc, Node * n, Transducer * t)
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

  FomaTransitionIterator::FomaTransitionIterator
  (Node * state, Transducer * t) : 
    state(state), arc_iterator(state->arcs()), t(t), end_iterator(false) 
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
  
  Transducer * FomaTransducer::create_empty_transducer(void)
  {    throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::create_epsilon_transducer(void)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::define_transducer(Key k)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  
  Transducer * FomaTransducer::define_transducer(const KeyPair &kp)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::define_transducer(const KeyPairVector &kpv)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  Transducer * FomaTransducer::copy(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::determinize(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::minimize(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::remove_epsilons(Transducer *)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::repeat_star(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::repeat_plus(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::repeat_n(Transducer * t,int n)
  {     throw hfst::exceptions::FunctionNotImplementedException();}
  
  Transducer * FomaTransducer::repeat_le_n(Transducer * t,int n)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::optionalize(Transducer * t)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::invert(Transducer * t)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::reverse(Transducer * t)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::extract_input_language(Transducer * t)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::extract_output_language(Transducer * t)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::substitute(Transducer * t,Key old_key,Key new_key)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::substitute
  (Transducer * t,KeyPair old_key_pair,KeyPair new_key_pair)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }
  
  Transducer * FomaTransducer::compose
  (Transducer * t1, Transducer * t2)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  Transducer * FomaTransducer::concatenate
  (Transducer * t1, Transducer * t2)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  Transducer * FomaTransducer::disjunct
  (Transducer * t1, Transducer * t2)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  Transducer * FomaTransducer::intersect
  (Transducer * t1, Transducer * t2)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  Transducer * FomaTransducer::subtract
  (Transducer * t1, Transducer * t2)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  FomaStateIterator::FomaStateIterator(Transducer * t):
  node_numbering(*t),t(t),current_state(0),ended(false)
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

  FomaStateIterator FomaTransducer::begin(Transducer * t)
  {     throw hfst::exceptions::FunctionNotImplementedException();}

  FomaStateIterator FomaTransducer::end(Transducer * t)
  { 
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  FomaStateIndexer::FomaStateIndexer(Transducer * t):
    t(t),numbering(*t)
  {    throw hfst::exceptions::FunctionNotImplementedException();}

  unsigned int FomaStateIndexer::operator[] (const FomaState &state)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  const FomaState FomaStateIndexer::operator[] (unsigned int number)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  void FomaTransducer::print(Transducer * t, 
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
  SfstTransducer sfst;
  SfstInputStream i(argv[1]);
  i.open();
  KeyTable key_table;
  Transducer * t = i.read_transducer(key_table);
  i.close();
  std::cerr << "Begin: transducer read from " << argv[1] << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;

  t = sfst.create_empty_transducer();
  std::cerr << "Begin: empty transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  t = sfst.create_epsilon_transducer();
  std::cerr << "Begin: epsilon transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  std::cerr << "Begin: a:a transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;

  t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("j")));
  std::cerr << "Begin: a:j transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  Transducer * s = sfst.copy(t);
  std::cerr << "Begin: copy of a:a transducer" << std::endl;
  sfst.print(s,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_star(t);
  std::cerr << "Begin: a:a* transducer" << std::endl;
  sfst.print(s,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_star(t);
  Transducer * u = sfst.determinize(s); 
  std::cerr << "Begin: a:a* transducer determinized" << std::endl;
  sfst.print(u,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;
  delete u;

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_star(t);
  u = sfst.minimize(s); 
  std::cerr << "Begin: a:a* transducer minimized" << std::endl;
  sfst.print(u,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;
  delete u;

  try {
    t = sfst.define_transducer(key_table.get_key("a"));
    s = sfst.repeat_star(t);
    u = sfst.remove_epsilons(s); 
    std::cerr << "Begin: a:a* transducer epsilons removed" << std::endl;
    sfst.print(u,key_table,std::cout);
    std::cerr << "End" << std::endl << std::endl;
    delete s;
    delete t;
    delete u;
  }
  catch ( HfstInterfaceException e )
    { 
      delete t;
      delete s;
      std::cerr << "Exception caught while attempting remove_epsilons!" 
		<< std::endl << std::endl; 
    }

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_plus(t);
  std::cerr << "Begin: a:a+ transducer" << std::endl;
  sfst.print(s,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_n(t,2);
  std::cerr << "Begin: a:a^2 transducer" << std::endl;
  sfst.print(s,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(key_table.get_key("a"));
  s = sfst.repeat_le_n(t,3);
  delete t;
  t = sfst.minimize(s);
  std::cerr << "Begin: a:a^{0,1,2} transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("j")));
  s = sfst.extract_input_language(t);
  delete t;
  t = sfst.minimize(s);
  std::cerr << "Begin: input side of a:j transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("j")));
  s = sfst.extract_output_language(t);
  delete t;
  t = sfst.minimize(s);
  std::cerr << "Begin: output side of a:j transducer" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("j")));
  s = sfst.invert(t);
  delete t;
  t = sfst.minimize(s);
  std::cerr << "Begin: a:j inverted" << std::endl;
  sfst.print(t,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("j")));
  s = sfst.substitute(t,key_table.get_key("a"),key_table.get_key("b"));
  std::cerr << "Begin: a:j where a replace by b" << std::endl;
  sfst.print(s,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete s;
  delete t;

  try 
    {
      t = sfst.define_transducer(KeyPair(key_table.get_key("a"),
					 key_table.get_key("j")));
      s = sfst.substitute(t,
			  KeyPair(key_table.get_key("a"),
				  key_table.get_key("j")),
			  KeyPair(key_table.get_key("a"),
				  key_table.get_key("a")));
      std::cerr << "Begin: a:j where a:j replace by a:a" << std::endl;
      sfst.print(s,key_table,std::cout);
      std::cerr << "End" << std::endl << std::endl;
      delete s;
      delete t;
    }
  catch ( HfstInterfaceException e )
    { 
      delete t;
      std::cerr << "Exception caught while attempting remove_epsilons!" 
		<< std::endl << std::endl; 
    }

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.compose(t,s);
  std::cerr << "Begin: c:a .o. a:b transducer" << std::endl;
  sfst.print(u,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.concatenate(t,s);
  std::cerr << "Begin: c:a a:b transducer" << std::endl;
  sfst.print(u,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.concatenate(t,s);
  Transducer * v = sfst.reverse(u);
  std::cerr << "Begin: c:a a:b transducer reversed" << std::endl;
  sfst.print(v,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;
  delete v;

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.disjunct(t,s);
  std::cerr << "Begin: c:a | a:b transducer" << std::endl;
  sfst.print(u,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.disjunct(t,s);
  v = sfst.intersect(u,t);
  std::cerr << "Begin: (c:a | a:b) & c:a transducer" << std::endl;
  sfst.print(v,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;
  delete v;

  t = sfst.define_transducer(KeyPair(key_table.get_key("c"),
				     key_table.get_key("a")));
  s = sfst.define_transducer(KeyPair(key_table.get_key("a"),
				     key_table.get_key("b")));
  u = sfst.disjunct(t,s);
  v = sfst.subtract(u,t);
  std::cerr << "Begin: (c:a | a:b) - c:a transducer" << std::endl;
  sfst.print(v,key_table,std::cout);
  std::cerr << "End" << std::endl << std::endl;
  delete t;
  delete s;
  delete u;
  delete v;

}
#endif
