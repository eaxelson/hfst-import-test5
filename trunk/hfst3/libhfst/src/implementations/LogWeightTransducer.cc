#include "LogWeightTransducer.h"

namespace hfst { namespace implementations
{
  LogWeightInputStream::LogWeightInputStream(void):
    filename(""),i_stream(),input_stream(cin),first_read(true)
  {}
  LogWeightInputStream::LogWeightInputStream(const char * fn):
    filename(fn),i_stream(fn),input_stream(i_stream),first_read(true)
    {}

  /* Need to check if i_symbol_table and o_symbol_table are compatible! 
     That is to see that there isn't a name "x" s.t. the input symbol number
     of "x" is not the same as its output symbol number.
     Not done yet!!!! */
  /*void LogWeightInputStream::populate_key_table
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
      }*/

  /* Skip the identifier string "LOG_OFST_TYPE" */
  void LogWeightInputStream::skip_identifier_version_3_0(void)
  { input_stream.ignore(14); }

  void LogWeightInputStream::skip_hfst_header(void)
  {
    input_stream.ignore(6);
    //char c;
    //input_stream.get(c);
    //switch (c)
    //  {
    //  case 0:
    skip_identifier_version_3_0();
    //break;
    //default:
    //assert(false);
    //}
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
    //if (filename.empty())
    //  { return std::cin.eof(); }
    //else
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
    return is_fst(input_stream);
  }
  
  bool LogWeightInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == -42;
  }
  
  bool LogWeightInputStream::is_fst(istream &s)
  {
    return s.good() && (s.peek() == -42);
  }

  bool LogWeightInputStream::operator() (void) const
  { return is_good(); }

  LogFst * LogWeightInputStream::read_transducer(bool has_header)
  {
    if (this->is_eof())
      { throw FileIsClosedException(); }
    LogFst * t = NULL;
    FstHeader header;
    try 
      {
	if (has_header)
	  skip_hfst_header();
	first_read = false;

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
      { delete t; throw e; }

    try
      {
	//const SymbolTable * isymbols = t->InputSymbols();
	//const SymbolTable * osymbols = t->OutputSymbols();
	return t;
#ifdef FOO
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
#endif
      }
    catch (HfstInterfaceException e)
      { throw e; }
  }
  LogWeightOutputStream::LogWeightOutputStream(void):
    filename(std::string()),output_stream(std::cout)
  {}

  LogWeightOutputStream::LogWeightOutputStream(const char * str):
    filename(str),o_stream(str,std::ios::out),output_stream(o_stream)
  {}

  void LogWeightOutputStream::open(void) {}
  void LogWeightOutputStream::close(void) 
  {
    if (filename != string())
      { o_stream.close(); }
  }

  /*  void LogWeightOutputStream::set_symbols(LogFst * transducer, KeyTable &key_table) 
  {
    SymbolTable symbol_table("anonym_hfst3_symbol_table");
    for (KeyTable::const_iterator it = key_table.begin();
	 it != key_table.end();
	 ++it)
      { symbol_table.AddSymbol(key_table[it->key],it->key); }
    transducer->SetInputSymbols(&symbol_table);
    transducer->SetOutputSymbols(&symbol_table);
  }
  */
  void LogWeightOutputStream::write_3_0_library_header(std::ostream &out)
  {
    out.write("HFST3",6);
    //out.put(0);
    out.write("LOG_OFST_TYPE",14);
    //out.put(0);
  }
  /*
  void LogWeightOutputStream::write_transducer(LogFst * transducer, KeyTable &key_table) 
  { set_symbols(transducer,key_table);
    write_3_0_library_header(output_stream);
    transducer->Write(output_stream,FstWriteOptions()); }
  */

  void LogWeightOutputStream::write_transducer(LogFst * transducer) 
  { write_3_0_library_header(output_stream);
    transducer->Write(output_stream,FstWriteOptions()); }

  /*
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
  */
  LogFst * LogWeightTransducer::create_empty_transducer(void)
  { 
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    initialize_symbol_tables(t);
    return t;
  }

  LogFst * LogWeightTransducer::create_epsilon_transducer(void)
  { 
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    t->SetFinal(s,0);
    initialize_symbol_tables(t);
    return t;
  }
  /*
  LogFst * LogWeightTransducer::define_transducer(Key k)
  {
    LogFst * t = new LogFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(k,k,0,s2));
    initialize_symbol_tables(t);
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
    initialize_symbol_tables(t);
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
    initialize_symbol_tables(t);
    return t;
  }
  */
  LogFst * LogWeightTransducer::define_transducer(const std::string &symbol)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(t->InputSymbols()->AddSymbol(symbol),
			t->InputSymbols()->AddSymbol(symbol),0,s2));
    return t;
  }
  LogFst * LogWeightTransducer::define_transducer
    (const std::string &isymbol, const std::string &osymbol)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(t->InputSymbols()->AddSymbol(isymbol),
			t->InputSymbols()->AddSymbol(osymbol),0,s2));
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer(unsigned int number)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(number,
			number,0,s2));
    return t;
  }
  LogFst * LogWeightTransducer::define_transducer
  (unsigned int inumber, unsigned int onumber)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,LogArc(inumber,
			onumber,0,s2));
    return t;
  }

  fst::SymbolTable * LogWeightTransducer::create_symbol_table(std::string name) {
    fst::SymbolTable * st = new fst::SymbolTable(name);
    st->AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st->AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st->AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    return st;
  }

  void LogWeightTransducer::initialize_symbol_tables(LogFst *t) {
    SymbolTable *st = create_symbol_table("");
    t->SetInputSymbols(st);
    //t->SetOutputSymbols(st);
    return;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const StringPairVector &spv)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,LogArc(t->InputSymbols()->AddSymbol(it->first),t->InputSymbols()->AddSymbol(it->second),0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    return t;
  }

  LogFst * LogWeightTransducer::define_transducer
  (const StringPairSet &sps)
  {
    LogFst * t = new LogFst;
    initialize_symbol_tables(t);

    StateId s1 = t->AddState();
    t->SetStart(s1);

    if (not sps.empty()) {
      StateId s2 = t->AddState();
      for (StringPairSet::const_iterator it = sps.begin();
	   it != sps.end();
	   ++it)
	{
	  t->AddArc(s1,LogArc(t->InputSymbols()->AddSymbol(it->first),t->InputSymbols()->AddSymbol(it->second),0,s2));
	}
      s1 = s2;
    }
    t->SetFinal(s1,0);
    return t;
  }

  void LogWeightTransducer::print_test(LogFst *t) 
  {

    SymbolTable *sym = t->InputSymbols(); 
    for (fst::StateIterator<LogFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::ArcIterator<LogFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const LogArc &arc = aiter.Value();
	    fprintf(stderr, "%i\t%i\t%s\t%s\t%f\n", (int)s, (int)arc.nextstate, sym->Find(arc.ilabel).c_str(), sym->Find(arc.olabel).c_str(), arc.weight.Value());
	  }
	if (t->Final(s) != LogWeight::Zero())
	  fprintf(stderr, "%i\t%f\n", (int)s, t->Final(s).Value());
      }
  }


  LogFst * 
  LogWeightTransducer::copy(LogFst * t)
  { return new LogFst(*t); }

  LogFst * 
  LogWeightTransducer::determinize(LogFst * t)
  {
    RmEpsilonFst<LogArc> rm(*t);
    EncodeMapper<LogArc> encode_mapper(0x0001,fst::EncodeType(1));
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
    DecodeFst<LogArc> dec(fst_enc, encode_mapper);
    delete determinized_t;
    return new LogFst(dec);
  }

  LogFst * 
  LogWeightTransducer::remove_epsilons(LogFst * t)
  { return new LogFst(RmEpsilonFst<LogArc>(*t)); }

  LogFst * 
  LogWeightTransducer::n_best(LogFst * t,int n)
  { 
    if (n < 0)
      { throw ImpossibleTransducerPowerException(); }
    LogFst * n_best_fst = new LogFst(); 
    fst::ShortestPath(*t,n_best_fst,(size_t)n);
    return n_best_fst;
  }

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
      { return create_epsilon_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { Concat(repetition,*t); }
    repetition->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    return repetition;
  }

  LogFst *
  LogWeightTransducer::repeat_le_n(LogFst * t,int n)
  {
    if (n <= 0)
      { return create_epsilon_transducer(); }

    LogFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      {
	LogFst * optional_t = optionalize(t);
	Concat(repetition,*optional_t);
	delete optional_t;
      }
    return repetition;
  }

  LogFst * 
  LogWeightTransducer::optionalize(LogFst * t)
  {
    LogFst * eps = create_epsilon_transducer();
    Union(eps,*t);
    eps->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    return eps;
  }

  LogFst * 
  LogWeightTransducer::invert(LogFst * t)
  {
    assert (t->InputSymbols() != NULL);
    LogFst * inverse = copy(t);
    assert (inverse->InputSymbols() != NULL);
    Invert(inverse);
    inverse->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    assert (inverse->InputSymbols() != NULL);
    return inverse;
  }

  /* Makes valgrind angry... */
  LogFst * 
  LogWeightTransducer::reverse(LogFst * t)
  {
    LogFst * reversed = new LogFst;
    Reverse<LogArc,LogArc>(*t,reversed);
    reversed->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    return reversed;
  }

  LogFst * LogWeightTransducer::extract_input_language
  (LogFst * t)
  { LogFst * retval =  new LogFst(ProjectFst<LogArc>(*t,PROJECT_INPUT)); 
    retval->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    return retval; }

  LogFst * LogWeightTransducer::extract_output_language
  (LogFst * t)
  { LogFst * retval =  new LogFst(ProjectFst<LogArc>(*t,PROJECT_OUTPUT)); 
    retval->SetInputSymbols(new SymbolTable( *(t->InputSymbols()) ));
    return retval; }
  
  typedef std::pair<int,int> LabelPair;
  typedef std::vector<LabelPair> LabelPairVector;
  LogFst * LogWeightTransducer::substitute
  (LogFst * t,unsigned int old_key,unsigned int new_key) 
  {
    LabelPairVector v;
    v.push_back(LabelPair(old_key,new_key));
    RelabelFst<LogArc> t_subst(*t,v,v);
    return new LogFst(t_subst);
  }
  
  LogFst * LogWeightTransducer::insert_freely
  (LogFst * t, const StringPair &symbol_pair)
  {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  LogFst * LogWeightTransducer::substitute(LogFst * t,
					   pair<unsigned int, unsigned int> old_key_pair,
					   pair<unsigned int, unsigned int> new_key_pair)
  {
    EncodeMapper<LogArc> encode_mapper(0x0001,fst::EncodeType(1));
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
		 static_cast<unsigned int>(old_pair_code.ilabel),
		 static_cast<unsigned int>(new_pair_code.ilabel));

    DecodeFst<LogArc> dec(*subst,encode_mapper);
    delete subst;
    return new LogFst(dec);
  }


  LogFst * LogWeightTransducer::substitute(LogFst *t,
					   std::string old_symbol,
					   std::string new_symbol)
  {
    SymbolTable * st = t->InputSymbols();
    return substitute(t, st->AddSymbol(old_symbol), st->AddSymbol(new_symbol));
  }

  LogFst * LogWeightTransducer::substitute(LogFst *t,
					   StringPair old_symbol_pair,
					   StringPair new_symbol_pair)
  {
    SymbolTable * st = t->InputSymbols();
    pair<unsigned int, unsigned int> old_pair(st->AddSymbol(old_symbol_pair.first),
		     st->AddSymbol(old_symbol_pair.second));
    pair<unsigned int, unsigned int> new_pair(st->AddSymbol(new_symbol_pair.first),
		     st->AddSymbol(new_symbol_pair.second));
    LogFst * retval = substitute(t, old_pair, new_pair);
    retval->SetInputSymbols( new SymbolTable ( *(t->InputSymbols()) ) );
    return retval;
  }



  LogFst * LogWeightTransducer::compose(LogFst * t1,
					LogFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());
    //fst::ArcSort<LogArc,fst::OLabelCompare<LogArc> > (t1,OLabelCompare<LogArc>());
    //fst::ArcSort<LogArc,fst::ILabelCompare<LogArc> > (t2,ILabelCompare<LogArc>());
    ComposeFst<LogArc> compose(*t1,*t2);
    return new LogFst(compose);
  }

  LogFst * LogWeightTransducer::concatenate
  (LogFst * t1,LogFst * t2)
  {
    ConcatFst<LogArc> concatenate(*t1,*t2);
    LogFst * retval = new LogFst(concatenate);
    retval->SetInputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    return retval;
  }

  LogFst * LogWeightTransducer::disjunct(LogFst * t1,
			  LogFst * t2)
  {
    UnionFst<LogArc> disjunct(*t1,*t2);
    LogFst * retval = new LogFst(disjunct);
    retval->SetInputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    return retval;
  }

  void LogWeightTransducer::disjunct_as_tries(LogFst &t1,
					      const LogFst * t2)
  { HFST_IMPLEMENTATIONS::disjunct_as_tries(t1,t1.Start(),t2,t2->Start()); }
  

  LogFst * LogWeightTransducer::intersect(LogFst * t1,
			   LogFst * t2)
  {
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols( new SymbolTable( *(t2->InputSymbols()) ) );

    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());

    RmEpsilonFst<LogArc> rm1(*t1);
    RmEpsilonFst<LogArc> rm2(*t2);
    EncodeMapper<LogArc> encoder(0x0001,ENCODE);
    EncodeFst<LogArc> enc1(rm1, &encoder);
    EncodeFst<LogArc> enc2(rm2, &encoder);
    DeterminizeFst<LogArc> det1(enc1);
    DeterminizeFst<LogArc> det2(enc2);

    IntersectFst<LogArc> intersect(det1,det2);
    LogFst *foo = new LogFst(intersect);
    DecodeFst<LogArc> decode(*foo, encoder);
    delete foo;
    LogFst *result = new LogFst(decode);
    result->SetInputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    return result;
  }

  /*
  LogFst * LogWeightTransducer::intersect(LogFst * t1,
			   LogFst * t2)
  {
    IntersectFst<LogArc> intersect(*t1,*t2);
    return new LogFst(intersect);
    }*/

  LogFst * LogWeightTransducer::subtract(LogFst * t1,
			  LogFst * t2)
  {
    assert(t1->InputSymbols() != NULL);
    assert(t2->InputSymbols() != NULL);
    if (t1->OutputSymbols() == NULL)
      t1->SetOutputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    if (t2->OutputSymbols() == NULL)
      t2->SetOutputSymbols( new SymbolTable( *(t2->InputSymbols()) ) );

    ArcSort(t1, OLabelCompare<LogArc>());
    ArcSort(t2, ILabelCompare<LogArc>());

    RmEpsilonFst<LogArc> rm1(*t1);
    RmEpsilonFst<LogArc> rm2(*t2);

    EncodeMapper<LogArc> encoder(0x0003,ENCODE); // t2 must be unweighted
    EncodeFst<LogArc> enc1(rm1, &encoder);
    EncodeFst<LogArc> enc2(rm2, &encoder);
    DeterminizeFst<LogArc> det1(enc1);
    DeterminizeFst<LogArc> det2(enc2);

    LogFst *difference = new LogFst();
    Difference(det1, det2, difference);
    DecodeFst<LogArc> subtract(*difference, encoder);
    delete difference;

    //DifferenceFst<LogArc> subtract(enc1,enc2);
    LogFst *result = new LogFst(subtract);
    result->SetInputSymbols( new SymbolTable( *(t1->InputSymbols()) ) );
    return result;
  }

  /*
  LogFst * LogWeightTransducer::subtract(LogFst * t1,
			  LogFst * t2)
  {
    DifferenceFst<LogArc> subtract(*t1,*t2);
    return new LogFst(subtract);
  }*/

  LogFst * LogWeightTransducer::set_final_weights(LogFst * t,float f)
  {
    for (fst::StateIterator<LogFst> iter(*t); 
	 not iter.Done(); iter.Next())
      {
	if (t->Final(iter.Value()) != fst::LogWeight::Zero())
	  { t->SetFinal(iter.Value(),f); }
      }
    return t;
  }

  LogFst * LogWeightTransducer::transform_weights
  (LogFst * t,float(*func)(float))
  {
    LogFst * t_copy = new LogFst(*t);
    for (fst::StateIterator<LogFst> iter(*t_copy); 
	 not iter.Done(); iter.Next())
      {
	LogArc::StateId s = iter.Value();
	t_copy->SetFinal(s,func(t_copy->Final(s).Value()));
	for (fst::MutableArcIterator<LogFst> aiter(t_copy,s);
	     !aiter.Done(); aiter.Next())
	  {
	    const LogArc &a = aiter.Value();
	    aiter.SetValue(LogArc(a.ilabel,a.olabel,
				  func(a.weight.Value()),a.nextstate));
	  }
      }
    return t_copy;
  }


  bool LogWeightTransducer::are_equivalent(LogFst * t1, LogFst * t2)
  {
    LogFst * mina = minimize(t1);
    LogFst * minb = minimize(t2);
    EncodeMapper<LogArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<LogArc> enca(*mina, &encode_mapper);
    EncodeFst<LogArc> encb(*minb, &encode_mapper);
    LogFst A(enca);
    LogFst B(encb);
    return Equivalent(A, B);
  }
  
  bool LogWeightTransducer::is_cyclic(LogFst * t)
  {
    return t->Properties(kCyclic, true) & kCyclic;
  }

  /*
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
  */


  void extract_reversed_strings
  (LogFst * t, LogArc::StateId s,
   WeightedPaths<float>::Vector &reversed_results, set<StateId> &states_visited)
  {
    if (states_visited.find(s) == states_visited.end())
      states_visited.insert(s);
    else
      throw TransducerIsCyclicException();

    WeightedPaths<float>::Vector reversed_continuations;
    for (fst::ArcIterator<LogFst> it(*t,s); !it.Done(); it.Next())
      {
	const LogArc &arc = it.Value();
	extract_reversed_strings(t,arc.nextstate,reversed_continuations, states_visited);
	std::string istring;
	std::string ostring;

	if (arc.ilabel != 0)
	  { istring = t->InputSymbols()->Find(arc.ilabel); }
	if (arc.olabel != 0)
	  { ostring = t->InputSymbols()->Find(arc.olabel); }
	WeightedPath<float> 
	  arc_string(istring,ostring,arc.weight.Value());
	WeightedPaths<float>::add(arc_string,reversed_continuations);
	WeightedPaths<float>::cat(reversed_results,reversed_continuations);
	reversed_continuations.clear();
      }
    if (t->Final(s) != LogWeight::Zero()) 
      { reversed_results.push_back(WeightedPath<float>
				   ("","",t->Final(s).Value())); }

    states_visited.erase(s);
  }


  void LogWeightTransducer::extract_strings
  (LogFst * t, WeightedPaths<float>::Set &results)
  {
    if (t->Start() == -1)
      { return; }
    WeightedPaths<float>::Vector reversed_results;
    set<StateId> states_visited;
    extract_reversed_strings(t,t->Start(),reversed_results, states_visited);
    //WeightedStrings<float>::reverse_strings(reversed_results);
    results.insert(reversed_results.begin(),reversed_results.end());
  }
  
  }
}

#ifdef DEBUG_MAIN
using namespace hfst::implementations;
hfst::symbols::GlobalSymbolTable KeyTable::global_symbol_table;
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
