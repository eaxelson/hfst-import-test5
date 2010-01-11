#include "LogWeightTransducer.h"

namespace HFST_IMPLEMENTATIONS
{
  LogWeightInputStream::LogWeightInputStream(void):
    i_stream(),input_stream(cin)
  {}
  LogWeightInputStream::LogWeightInputStream(const char * filename):
    filename(filename),i_stream(filename),input_stream(i_stream)
  {}

  /* Need to check if i_symbol_table and o_symbol_table are compatible! 
     That is to see that there isn't a name "x" s.t. the input symbol number
     of "x" is not the same as its output symbol number.
     Not done yet!!!! */
  void LogWeightInputStream::populate_key_table
  (KeyTable &key_table,
   const SymbolTable * i_symbol_table,
   const SymbolTable * o_symbol_table,
   KeyMap &key_map)
  {
    KeyTable transducer_key_table;
    for (unsigned int i = 1; i < i_symbol_table->AvailableKey(); ++i)
      {
	std::string str = i_symbol_table->Find(i);
	const char * string = str.c_str(); 
	if (string != NULL)
	  { transducer_key_table.add_symbol(string); }
	else
	  { 	    
	    ostringstream oss(ostringstream::out);
	    oss << "@EMPTY@" << i;
	    transducer_key_table.add_symbol(oss.str().c_str());
	  }
      }
    for (unsigned int i = 1; i < o_symbol_table->AvailableKey(); ++i)
      {
	std::string str = i_symbol_table->Find(i);
	const char * string = str.c_str(); 
	if (string != NULL)
	  { transducer_key_table.add_symbol(string); }
	else
	  { 	    
	    ostringstream oss(ostringstream::out);
	    oss << "@EMPTY@" << i;
	    transducer_key_table.add_symbol(oss.str().c_str());
	  }
      }
    try
      { transducer_key_table.harmonize(key_map,key_table); }
    catch (const char * p)
      { throw p; }
  }

  /* Skip the identifier string "LOG_OFST_TYPE" */
  void LogWeightInputStream::skip_identifier_version_3_0(void)
  { i_stream.ignore(14); }

  void LogWeightInputStream::skip_hfst_header(void)
  {
    i_stream.ignore(6);
    char c;
    i_stream.get(c);
    switch (c)
      {
      case 0:
	skip_identifier_version_3_0();
	break;
      default:
	assert(false);
      }
  }

  void LogWeightInputStream::open(void) {}
  void LogWeightInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }
  bool LogWeightInputStream::is_open(void) const
  { 
    if (filename != string())
      { return i_stream.is_open(); }
    return true;
  }
  bool LogWeightInputStream::is_eof(void) const
  {
    if (filename == string())
      { return std::cin.eof(); }
    else
      { return input_stream.peek() == EOF; }
  }
  bool LogWeightInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool LogWeightInputStream::is_good(void) const
  {
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }

  bool LogWeightInputStream::is_fst(void) const
  {
    if (not is_good())
      { return false; }
    if (filename == string())
      {
	if (std::cin.peek() == -42)
	  { return true; }
      }
    else
      {
	if (input_stream.peek() == -42)
	  { return true; }
      }
    return false;
  }

  bool LogWeightInputStream::operator() (void) const
  { return is_good(); }

  LogFst * LogWeightInputStream::read_transducer(KeyTable &key_table)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    LogFst * t;
    FstHeader header;
    try 
      {
	skip_hfst_header();
	if (filename == string())
	  {
	    header.Read(input_stream,"STDIN");			    
	    t = static_cast<LogFst*>
	      (LogFst::Read(input_stream,
				  FstReadOptions("STDIN",
						 &header)));
	  }
	else
	  {
	    header.Read(input_stream,filename);			    
	    t = static_cast<LogFst*>
	      (LogFst::Read(input_stream,
				  FstReadOptions(filename,
						 &header)));
	  }
	if (t == NULL)
	  { throw TransducerHasWrongTypeException(); }
      }
    catch (TransducerHasWrongTypeException e)
      { throw e; }

    try
      {
	const SymbolTable * isymbols = t->InputSymbols();
	const SymbolTable * osymbols = t->OutputSymbols();
	if ((isymbols == NULL) and (osymbols == NULL))
	  { return t; }
	KeyMap key_map;
	if (isymbols != NULL)
	  {
	    populate_key_table(key_table,
			       isymbols,
			       osymbols,
			       key_map);
	  }
	LogFst * t_harmonized = 
	  LogWeightTransducer::harmonize(t,key_map);
	delete t;
	return t_harmonized;
      }
    catch (HfstInterfaceException e)
      { throw e; }
  }
  LogWeightState::LogWeightState(StateId state,
					   LogFst * t):
    state(state), t(t) {}

  LogWeightState::LogWeightState(const LogWeightState &s)
  { this->state = s.state; this->t = s.t; }

  LogWeight LogWeightState::get_final_weight(void) const
  { return t->Final(state); }
    
  bool LogWeightState::operator< 
  (const LogWeightState &another) const
  { return state < another.state; }
  
   bool LogWeightState::operator== 
  (const LogWeightState &another) const
   { return (t == another.t) and (state == another.state); }
  
  bool LogWeightState::operator!= 
  (const LogWeightState &another) const
  { return not (*this == another); }

  LogWeightState::const_iterator 
  LogWeightState::begin(void) const
  { return LogWeightState::const_iterator(state,t); }

  LogWeightState::const_iterator 
  LogWeightState::end(void) const
  { return LogWeightState::const_iterator(); }

  void LogWeightState::print(KeyTable &key_table, ostream &out,
				  LogWeightStateIndexer &indexer) const
  {
    for (LogWeightState::const_iterator it = begin(); it != end(); ++it)
      { 
	LogWeightTransition tr = *it;
	tr.print(key_table,out,indexer);
      }
    if (get_final_weight() != LogWeight::Zero())
      { out << state << "\t" << get_final_weight() << std::endl; }
  }

  LogWeightStateIndexer::LogWeightStateIndexer(LogFst * t):
    t(t) {}

  unsigned int LogWeightStateIndexer::operator[]
  (const LogWeightState &state)
  { return state.state; }

  const LogWeightState LogWeightStateIndexer::operator[]
  (unsigned int number)
  { return LogWeightState(number,t); }

  LogWeightStateIterator::LogWeightStateIterator(LogFst * t):
    t(t), iterator(new StateIterator<LogFst>(*t)), 
    current_state(iterator->Value()),ended(iterator->Done())
  {}

  LogWeightStateIterator::LogWeightStateIterator(void):
    t(NULL),iterator(NULL),current_state(0),ended(true) {}

  LogWeightStateIterator::~LogWeightStateIterator(void)
  { delete iterator; }

  void LogWeightStateIterator::operator= 
  (const LogWeightStateIterator &another)
  {
    if (*this == another) {return; }
    delete iterator;
    if (another.ended)
      {
	t = NULL;
	iterator = NULL;
	current_state = 0;
	ended = true;
	return;
      }
    ended = false;
    t = another.t;
    iterator = new StateIterator<LogFst>(*t);
    current_state = another.current_state;
    while (iterator->Value() != current_state)
      { iterator->Next(); }
  }

  bool LogWeightStateIterator::operator== 
  (const LogWeightStateIterator &another) const
  {
    if (ended and another.ended)
      { return true; }
    if (ended or another.ended)
      { return false; }
    return 
      (t == another.t) and
      (current_state == another.current_state);
  }

  bool LogWeightStateIterator::operator!= 
  (const LogWeightStateIterator &another) const
  { return not (*this == another); }

  const LogWeightState 
  LogWeightStateIterator::operator* (void)
  { return LogWeightState(current_state,t); }

  void LogWeightStateIterator::operator++ (void)
  {
    if (ended) { return; }
    iterator->Next();
    if (iterator->Done()) { ended = true; }
    else { current_state = iterator->Value(); }
  }

  void LogWeightStateIterator::operator++ (int)
  {
    if (ended) { return; }
    iterator->Next();
    if (iterator->Done()) { ended = true; }
    else { current_state = iterator->Value(); }
  }

  LogWeightTransition::LogWeightTransition
  (const LogArc &arc,StateId source_state,LogFst * t):
    arc(arc), source_state(source_state), t(t) {}
  
  Key LogWeightTransition::get_input_key(void) const
  { return arc.ilabel; }

  Key LogWeightTransition::get_output_key(void) const
  { return arc.olabel; }

  LogWeightState LogWeightTransition::get_target_state(void) const
  { return LogWeightState(arc.nextstate,t); }

  LogWeightState LogWeightTransition::get_source_state(void) const
  { return LogWeightState(source_state,t); }

  LogWeight LogWeightTransition::get_weight(void) const
  { return arc.weight; }

  void LogWeightTransition::print
  (KeyTable &key_table, ostream &out, LogWeightStateIndexer &indexer) 
    const
  {
    (void)indexer;
    out << source_state << "\t"
	<< arc.nextstate << "\t"
	<< key_table[arc.ilabel] << "\t"
	<< key_table[arc.olabel] << "\t"
	<< arc.weight << std::endl;				       
  }

  LogWeightTransitionIterator::LogWeightTransitionIterator
  (StateId state,LogFst * t):
    arc_iterator(new ArcIterator<LogFst>(*t,state)), state(state), t(t)
  { end_iterator = arc_iterator->Done(); }

  LogWeightTransitionIterator::LogWeightTransitionIterator(void):
    arc_iterator(NULL),state(0), t(NULL), end_iterator(true)
  {}
  
  LogWeightTransitionIterator::~LogWeightTransitionIterator(void)
  { delete arc_iterator; }

  void LogWeightTransitionIterator::operator=  
  (const LogWeightTransitionIterator &another)
  {
    if (this == &another) { return; }
    delete arc_iterator;
    if (another.end_iterator)
      {
	end_iterator = true;
	arc_iterator = NULL;
	state = 0;
	t = NULL;
	return;
      }
    arc_iterator = new ArcIterator<LogFst>(*(another.t),another.state);
    arc_iterator->Seek(another.arc_iterator->Position());
    t = another.t;
    end_iterator = false;
  }

  bool LogWeightTransitionIterator::operator== 
  (const LogWeightTransitionIterator &another)
  { if (end_iterator and another.end_iterator)
      { return true; }
    if (end_iterator or another.end_iterator)
      { return false; }
    return 
      (t == another.t) and
      (state == another.state) and
      (arc_iterator->Position() == another.arc_iterator->Position());
  }

  bool LogWeightTransitionIterator::operator!= 
  (const LogWeightTransitionIterator &another)
  { return not (*this == another); }

  const LogWeightTransition LogWeightTransitionIterator::operator* 
  (void)
  { return LogWeightTransition(arc_iterator->Value(),state,t); }

  void LogWeightTransitionIterator::operator++ (void)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }

  void LogWeightTransitionIterator::operator++ (int)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }

  LogFst * LogWeightTransducer::create_empty_transducer(void)
  { 
    LogFst * t = new LogFst;
    StateId s = t->AddState();
    t->SetStart(s);
    return t;
  }

  LogFst * LogWeightTransducer::create_epsilon_transducer(void)
  { 
    LogFst * t = new LogFst;
    StateId s = t->AddState();
    t->SetStart(s);
    t->SetFinal(s,0);
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer(Key k)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(k,k,0,s2));
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const KeyPair &key_pair)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(key_pair.first,key_pair.second,0,s2));
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const KeyPairVector &kpv)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (KeyPairVector::const_iterator it = kpv.begin();
	 it != kpv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,LogArc(it->first,it->second,0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    return t;
  }

  LogFst * 
  LogWeightTransducer::copy(LogFst * t)
  { return new LogFst(*t); }

  LogFst * 
  LogWeightTransducer::determinize(LogFst * t)
  {
    RmEpsilonFst<LogArc> rm(*t);
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(rm,
			  &encode_mapper);
    DeterminizeFst<LogArc> det(enc);
    DecodeFst<LogArc> dec(det,
			  encode_mapper);
    return new LogFst(dec);
  }
  
  LogFst * LogWeightTransducer::minimize
  (LogFst * t)
  {
    LogFst * determinized_t = determinize(t);
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(*determinized_t,
			  &encode_mapper);
    LogFst fst_enc(enc);
    Minimize<LogArc>(&fst_enc);
    DeterminizeFst<LogArc> minimized_t(enc);
    DecodeFst<LogArc> dec(minimized_t,
			  encode_mapper);
    delete determinized_t;
    return new LogFst(dec);
  }

  LogFst * 
  LogWeightTransducer::remove_epsilons(LogFst * t)
  { return new LogFst(RmEpsilonFst<LogArc>(*t)); }

  LogFst * 
  LogWeightTransducer::repeat_star(LogFst * t)
  { return new LogFst(ClosureFst<LogArc>(*t,CLOSURE_STAR)); }

  LogFst * 
  LogWeightTransducer::repeat_plus(LogFst * t)
  { return new LogFst(ClosureFst<LogArc>(*t,CLOSURE_PLUS)); }

  LogFst *
  LogWeightTransducer::repeat_n(LogFst * t,int n)
  {
    if (n <= 0)
      { return create_empty_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { Concat(repetition,*t); }
    return repetition;
  }

  LogFst *
  LogWeightTransducer::repeat_le_n(LogFst * t,int n)
  {
    if (n <= 0)
      { return create_empty_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    LogFst * t_i_times = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { 
	Union(repetition,*t_i_times); 
	Concat(t_i_times,*t);
      }
    delete t_i_times;
    return repetition;
  }

  LogFst * 
  LogWeightTransducer::optionalize(LogFst * t)
  {
    LogFst * eps = create_epsilon_transducer();
    Union(eps,*t);
    return eps;
  }

  LogFst * 
  LogWeightTransducer::invert(LogFst * t)
  {
    LogFst * inverse = copy(t);
    Invert(inverse);
    return inverse;
  }

  /* Makes valgrind angry... */
  LogFst * 
  LogWeightTransducer::reverse(LogFst * t)
  {
    LogFst * reversed = new LogFst;
    Reverse<LogArc,LogArc>(*t,reversed);
    return reversed;
  }

  LogFst * LogWeightTransducer::extract_input_language
  (LogFst * t)
  { return new LogFst(ProjectFst<LogArc>(*t,PROJECT_INPUT)); }

  LogFst * LogWeightTransducer::extract_output_language
  (LogFst * t)
  { return new LogFst(ProjectFst<LogArc>(*t,PROJECT_OUTPUT)); }
  
  typedef std::pair<int,int> LabelPair;
  typedef std::vector<LabelPair> LabelPairVector;
  LogFst * LogWeightTransducer::substitute
  (LogFst * t,Key old_key,Key new_key) 
  {
    LabelPairVector v;
    v.push_back(LabelPair(old_key,new_key));
    RelabelFst<LogArc> t_subst(*t,v,v);
    return new LogFst(t_subst);
  }
  
  LogFst * LogWeightTransducer::substitute(LogFst * t,
			    KeyPair old_key_pair,
			    KeyPair new_key_pair)
  {
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enc(*t,&encode_mapper);

    LogArc old_pair_code = 
      encode_mapper(LogArc(old_key_pair.first,old_key_pair.second,0,0));
    LogArc new_pair_code =
      encode_mapper(LogArc(new_key_pair.first,new_key_pair.second,0,0));

    // First cast up, then cast down... For some reason dynamic_cast<LogFst*>
    // doesn't work although both EncodeFst<LogArc> and LogFst extend Fst<LogArc>. 
    // reinterpret_cast worked, but that is apparently unsafe...
    LogFst * subst = 
      substitute(static_cast<LogFst*>(static_cast<Fst<LogArc>*>(&enc)),
		 static_cast<Key>(old_pair_code.ilabel),
		 static_cast<Key>(new_pair_code.ilabel));

    DecodeFst<LogArc> dec(*subst,encode_mapper);
    delete subst;
    return new LogFst(dec);
  }

  LogFst * LogWeightTransducer::compose(LogFst * t1,
			 LogFst * t2)
  {
    ComposeFst<LogArc> compose(*t1,*t2);
    return new LogFst(compose);
  }

  LogFst * LogWeightTransducer::concatenate
  (LogFst * t1,LogFst * t2)
  {
    ConcatFst<LogArc> concatenate(*t1,*t2);
    return new LogFst(concatenate);
  }

  LogFst * LogWeightTransducer::disjunct(LogFst * t1,
			  LogFst * t2)
  {
    UnionFst<LogArc> disjunct(*t1,*t2);
    return new LogFst(disjunct);
  }

  LogFst * LogWeightTransducer::intersect(LogFst * t1,
			   LogFst * t2)
  {
    IntersectFst<LogArc> intersect(*t1,*t2);
    return new LogFst(intersect);
  }

  LogFst * LogWeightTransducer::subtract(LogFst * t1,
			  LogFst * t2)
  {
    DifferenceFst<LogArc> subtract(*t1,*t2);
    return new LogFst(subtract);
  }

  LogFst * LogWeightTransducer::set_weight(LogFst * t,float f)
  {
    LogFst * t_copy = new LogFst(*t);
    for (fst::StateIterator<LogFst> iter(*t); 
	 not iter.Done(); iter.Next())
      {
	if (t_copy->Final(iter.Value()) != fst::LogWeight::Zero())
	  { t_copy->SetFinal(iter.Value(),f); }
      }
    return t_copy;
  }

  LogWeightTransducer::const_iterator 
  LogWeightTransducer::begin(LogFst * t)
  { return LogWeightStateIterator(t); }

  LogWeightTransducer::const_iterator 
  LogWeightTransducer::end(LogFst * t)
  { (void)t;
    return LogWeightStateIterator(); }

  void LogWeightTransducer::print
  (LogFst * t, KeyTable &key_table, ostream &out) 
  {
    LogWeightStateIndexer indexer(t);
    for (LogWeightTransducer::const_iterator it = begin(t);
	 it != end(t);
	 ++it)
      { 
	LogWeightState s = *it;
	s.print(key_table,out,indexer);
      }
  }

  LogFst * LogWeightTransducer::harmonize
  (LogFst * t,KeyMap &key_map)
  {
    LabelPairVector v;
    for (KeyMap::iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      {
	v.push_back(LabelPair(it->first,it->second));
      }
    RelabelFst<LogArc> t_subst(*t,v,v);
    return new LogFst(t_subst);
  }
}

#ifdef DEBUG_MAIN
using namespace HFST_IMPLEMENTATIONS;
HFST_SYMBOLS::GlobalSymbolTable KeyTable::global_symbol_table;
int main(void) 
{
  LogWeightTransducer ofst;
  LogFst * t = ofst.create_empty_transducer();
  for (LogWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      LogWeightState s = *it;
    }
  delete t;
  t = ofst.create_epsilon_transducer();
  for (LogWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      LogWeightState s = *it;
    }
  delete t;
  KeyTable key_table;
  key_table.add_symbol("a");
  key_table.add_symbol("b");
  t = ofst.define_transducer(key_table["a"]);
  ofst.print(t,key_table,std::cerr);
  delete t;
  t = ofst.define_transducer(KeyPair(key_table["a"],
				     key_table["b"]));
  ofst.print(t,key_table,std::cerr);
  LogFst * tt;
  tt = ofst.copy(t);
  tt->AddArc(0,LogArc(0,0,0,1));
  ofst.print(tt,key_table,std::cerr);
  LogFst * tt_det = ofst.minimize(tt);
  ofst.print(tt_det,key_table,std::cerr);
  delete t;
  t = ofst.invert(tt);
  delete t;
  delete tt;
  delete tt_det;
  std::cerr << "Test substitution" << std::endl;
  LogFst * fst = ofst.define_transducer(KeyPair(key_table["a"],
						      key_table["b"]));
  std::cerr << "Print a:b" << std::endl;
  ofst.print(fst,key_table,std::cerr);
  key_table.add_symbol("c");
  LogFst * fst1 = ofst.substitute(fst,key_table["a"],key_table["c"]);
  std::cerr << "Print c:b" << std::endl;
  ofst.print(fst1,key_table,std::cerr);
  LogFst * fst2 = 
    ofst.substitute(fst,
		    KeyPair(key_table["a"],key_table["b"]),
		    KeyPair(key_table["c"],key_table["c"]));
  std::cerr << "Print c:c" << std::endl;
  ofst.print(fst2,key_table,std::cerr);
  delete fst;
  delete fst1;
  delete fst2;
  LogWeightInputStream input;
  LogFst * input_fst = input.read_transducer(key_table);
  ofst.print(input_fst,key_table,std::cerr);
  delete input_fst;
}
#endif
