#include "TropicalWeightTransducer.h"

namespace hfst { namespace implementations
{

  void initialize_symbol_tables(StdVectorFst *t);

  TropicalWeightInputStream::TropicalWeightInputStream(void):
    i_stream(),input_stream(cin)
  {}
  TropicalWeightInputStream::TropicalWeightInputStream(const char * filename):
    filename(filename),i_stream(filename),input_stream(i_stream)
  {}

  StringSymbolSet TropicalWeightTransducer::get_string_symbol_set(fst::StdVectorFst *t)
  {
    StringSymbolSet s;
    for ( fst::SymbolTableIterator it = fst::SymbolTableIterator(*(t->InputSymbols()));
	  not it.Done(); it.Next() ) {
      s.insert( std::string(it.Symbol()) );
    }
    return s;
  }

  /* Find the number-to-number mappings needed to be performed to t1 so that it will follow 
     the same symbol-to-number encoding as t2.
     @pre t2's symbol table must contain all symbols in t1's symbol table. 
  */
  KeyMap TropicalWeightTransducer::create_mapping(fst::StdVectorFst *t1, fst::StdVectorFst *t2)
  {
    KeyMap km;
    // find the number-to-number mappings for transducer t1
    for ( fst::SymbolTableIterator it = fst::SymbolTableIterator(*(t1->InputSymbols()));
	  not it.Done(); it.Next() ) {    
      km [ (Key)it.Value() ] = (Key) t2->InputSymbols()->Find( it.Symbol() );
    }
    return km;
  }

  /* Recode the symbol numbers in this transducer as indicated in KeyMap km. */
  void TropicalWeightTransducer::recode_symbol_numbers(StdVectorFst *t, KeyMap &km) 
  {
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	StateId s = siter.Value();
	for (fst::MutableArcIterator<StdVectorFst> aiter(t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();
	    StdArc new_arc;
	    new_arc.ilabel = km[arc.ilabel];
	    new_arc.olabel = km[arc.olabel];
	    new_arc.weight = arc.weight;
	    new_arc.nextstate = arc.nextstate;
	    aiter.SetValue(new_arc);
	  }
      }
    return;
  }

  /* 
     Create a copy of this transducer where all transitions of type "?:?", "?:x" and "x:?"
     are expanded according to the StringSymbolSet 'unknown' that lists all symbols previously
     unknown to this transducer.
  */
  StdVectorFst * TropicalWeightTransducer::expand_arcs(StdVectorFst * t, StringSymbolSet &unknown)
  {
    (void)unknown;  // REMOVE THIS !!!
    StdVectorFst * result = new StdVectorFst();
    std::map<StateId,StateId> state_map;   // maps states of this to states of result

    // go through all states in this
    for (fst::StateIterator<StdVectorFst> siter(*t); 
	 not siter.Done(); siter.Next())
      {
	// create new state in result
	StateId s = siter.Value();
	StateId result_s = result->AddState();
	state_map[s] = result_s;

	// make the new state initial, if needed
	if (t->Start() == s)
	  result->SetStart(result_s);

	// make the new state final, if needed
	if (t->Final(s) != TropicalWeight::Zero())
	  result->SetFinal(result_s, t->Final(s).Value());


	// go through all the arcs in this
	for (fst::ArcIterator<StdVectorFst> aiter(*t,s); !aiter.Done(); aiter.Next())
	  {
	    const StdArc &arc = aiter.Value();

	    // find the corresponding target state in result or, if not found, create a new state
	    StateId result_nextstate;
	    map<StateId,StateId>::const_iterator it = state_map.find(arc.nextstate); 
	    if ( it == state_map.end() )
	      {
		result_nextstate = result->AddState();
		state_map[arc.nextstate] = result_nextstate;
	      }
	    else 
	      result_nextstate = it->second;

	    // expand the transitions, if needed
	    
#ifdef foo
	    if ( arc.ilabel == 1 &&       // cross-product "?:?"
		 arc.olabel == 1 )
	      {
	      }
	    else if (arc.ilabel == 2 &&   // identity "?:?"
		     arc.olabel == 2 )       
	      {
	      }
	    else if (false)  // "?:x"
	      {
	      }
	    else if (false)  // "x:?"
	      {
	      }
	    else             // "x:y", no need to expand
	      {
#endif
	      result->AddArc(result_s, StdArc(arc.ilabel, arc.olabel, arc.weight, result_nextstate));		
		//}
	  }
      }
    return result;
  }

  void TropicalWeightTransducer::harmonize 
  (StdVectorFst * t1, StdVectorFst * t2)
  {
    // 1. Calculate the set of unknown symbols for transducers t1 and t2.

    StringSymbolSet unknown_t1;    // symbols known to another but not this
    StringSymbolSet unknown_t2;    // and vice versa
    StringSymbolSet t1_symbols = get_string_symbol_set(t1);
    StringSymbolSet t2_symbols = get_string_symbol_set(t2);
    KeyTable::collect_unknown_sets(t1_symbols, unknown_t1,
				   t2_symbols, unknown_t2);


    // 2. Add new symbols from transducer t1 to the symbol table of transducer t2...

    for ( StringSymbolSet::const_iterator it = unknown_t2.begin();
	  it != unknown_t2.end(); it++ ) {
	t2->InputSymbols()->AddSymbol(*it);
    }
    // ...calculate the number mappings needed in harmonization...
    KeyMap km = create_mapping(t1, t2);

    // ... replace the symbol table of t1 with a copy of t2's symbol table
    delete t1->InputSymbols();
    t1->SetInputSymbols( new fst::SymbolTable(*(t2->InputSymbols())) );

    // ...and recode the symbol numbers of transducer t1 so that
    //    it follows the new symbol table.
    recode_symbol_numbers(t1, km);


    // 3. Calculate the set of symbol pairs to which a non-identity "?:?"
    //    transition is expanded for both transducers.
    
    fst::StdVectorFst *temp;

    temp = expand_arcs(t1, unknown_t1);
    delete t1;
    t1 = temp;

    temp = expand_arcs(t2, unknown_t2);
    delete t2;
    t2 = temp;

    return;
  }


  /* Need to check if i_symbol_table and o_symbol_table are compatible! 
     That is to se that there isn't a name "x" s.t. the input symbol number
     of "x" is not the same as its output symbol number.
     Not done yet!!!! */
  void TropicalWeightInputStream::populate_key_table
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
	std::string str = o_symbol_table->Find(i);
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

  /* Skip the identifier string "TROPICAL_OFST_TYPE" */
  void TropicalWeightInputStream::skip_identifier_version_3_0(void)
  { i_stream.ignore(19); }

  void TropicalWeightInputStream::skip_hfst_header(void)
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
  
  void TropicalWeightInputStream::open(void) {}
  void TropicalWeightInputStream::close(void)
  {
    if (filename != string())
      { i_stream.close(); }
  }
  bool TropicalWeightInputStream::is_open(void) const
  { 
    if (filename != string())
      { return i_stream.is_open(); }
    return true;
  }
  bool TropicalWeightInputStream::is_eof(void) const
  {
    if (filename == string())
      { return std::cin.eof(); }
    else
      { return input_stream.peek() == EOF; }
  }
  bool TropicalWeightInputStream::is_bad(void) const
  {
    if (filename == string())
      { return std::cin.bad(); }
    else
      { return input_stream.bad(); }    
  }
  bool TropicalWeightInputStream::is_good(void) const
  {
    if (filename == string())
      { return std::cin.good(); }
    else
      { return input_stream.good(); }
  }

  bool TropicalWeightInputStream::is_fst(void) const
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

  bool TropicalWeightInputStream::operator() (void) const
  { return is_good(); }

  StdVectorFst * TropicalWeightInputStream::read_transducer(KeyTable &key_table)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    StdVectorFst * t;
    FstHeader header;
    try 
      {
	skip_hfst_header();
	if (filename == string())
	  {
	    header.Read(input_stream,"STDIN");			    
	    t = static_cast<StdVectorFst*>
	      (StdVectorFst::Read(input_stream,
				  FstReadOptions("STDIN",
						 &header)));
	  }
	else
	  {
	    header.Read(input_stream,filename);			    
	    t = static_cast<StdVectorFst*>
	      (StdVectorFst::Read(input_stream,
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
	StdVectorFst * t_harmonized = NULL;  // FIX THIS
	  //TropicalWeightTransducer::harmonize(t,key_map);
	delete t;
	return t_harmonized;
      }
    catch (HfstInterfaceException e)
      { throw e; }
  }

  /*
  TropicalWeightState::TropicalWeightState(StateId state,
					   StdVectorFst * t):
    state(state), t(t) {}

  TropicalWeightState::TropicalWeightState(const TropicalWeightState &s)
  { this->state = s.state; this->t = s.t; }

  TropicalWeight TropicalWeightState::get_final_weight(void) const
  { return t->Final(state); }
    
  bool TropicalWeightState::operator< 
  (const TropicalWeightState &another) const
  { return state < another.state; }
  
   bool TropicalWeightState::operator== 
  (const TropicalWeightState &another) const
   { return (t == another.t) and (state == another.state); }
  
  bool TropicalWeightState::operator!= 
  (const TropicalWeightState &another) const
  { return not (*this == another); }

  TropicalWeightState::const_iterator 
  TropicalWeightState::begin(void) const
  { return TropicalWeightState::const_iterator(state,t); }

  TropicalWeightState::const_iterator 
  TropicalWeightState::end(void) const
  { return TropicalWeightState::const_iterator(); }

  void TropicalWeightState::print(KeyTable &key_table, ostream &out,
				  TropicalWeightStateIndexer &indexer) const
  {
    for (TropicalWeightState::const_iterator it = begin(); it != end(); ++it)
      { 
	TropicalWeightTransition tr = *it;
	tr.print(key_table,out,indexer);
      }
    if (get_final_weight() != TropicalWeight::Zero())
      { out << state << "\t" << get_final_weight() << std::endl; }
  }

  HfstState TropicalWeightState::get_state_number(void)
  {
    return this->state;
  }

  HfstWeight TropicalWeightState::get_state_weight(void)
  {
    return (t->Final(state)).Value();
  }

  TropicalWeightStateIndexer::TropicalWeightStateIndexer(StdVectorFst * t):
    t(t) {}

  unsigned int TropicalWeightStateIndexer::operator[]
  (const TropicalWeightState &state)
  { return state.state; }

  const TropicalWeightState TropicalWeightStateIndexer::operator[]
  (unsigned int number)
  { return TropicalWeightState(number,t); }

  */


  TropicalWeightStateIterator::TropicalWeightStateIterator(StdVectorFst * t):
    iterator(new StateIterator<StdVectorFst>(*t))
  {}

  TropicalWeightStateIterator::~TropicalWeightStateIterator(void)
  { delete iterator; }

  void TropicalWeightStateIterator::next(void)
  {
    iterator->Next();
  }

  bool TropicalWeightStateIterator::done(void)
  {
    return iterator->Done();
  }

  TropicalWeightState TropicalWeightStateIterator::value(void)
  {
    return iterator->Value();
  }



  TropicalWeightTransition::TropicalWeightTransition(const StdArc &arc, StdVectorFst *t):
    arc(arc), t(t)
  {}

  TropicalWeightTransition::~TropicalWeightTransition(void)
  {}

  std::string TropicalWeightTransition::get_input_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.ilabel);
  }

  std::string TropicalWeightTransition::get_output_symbol(void) const
  {
    return t->InputSymbols()->Find(arc.olabel);
  }

  TropicalWeightState TropicalWeightTransition::get_target_state(void) const
  {
    return arc.nextstate;
  }

  TropicalWeight TropicalWeightTransition::get_weight(void) const
  {
    return arc.weight;
  }



  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator(StdVectorFst *t, StateId state):
    arc_iterator(new ArcIterator<StdVectorFst>(*t, state)),
    t(t)
  {}

  TropicalWeightTransitionIterator::~TropicalWeightTransitionIterator(void)
  {}

  void TropicalWeightTransitionIterator::next()
  {
    arc_iterator->Next();
  }

  bool TropicalWeightTransitionIterator::done()
  {
    return arc_iterator->Done();
  }

  TropicalWeightTransition TropicalWeightTransitionIterator::value()
  {
    return TropicalWeightTransition(arc_iterator->Value(), this->t);
  }



  /*
  TropicalWeightTransition::TropicalWeightTransition
  (const StdArc &arc,StateId source_state,StdVectorFst * t):
    arc(arc), source_state(source_state), t(t) {}
  
  Key TropicalWeightTransition::get_input_key(void) const
  { return arc.ilabel; }

  Key TropicalWeightTransition::get_output_key(void) const
  { return arc.olabel; }

  TropicalWeightState TropicalWeightTransition::get_target_state(void) const
  { return TropicalWeightState(arc.nextstate,t); }

  TropicalWeightState TropicalWeightTransition::get_source_state(void) const
  { return TropicalWeightState(source_state,t); }

  TropicalWeight TropicalWeightTransition::get_weight(void) const
  { return arc.weight; }

  void TropicalWeightTransition::print
  (KeyTable &key_table, ostream &out, TropicalWeightStateIndexer &indexer) 
    const
  {
    (void)indexer;
    out << source_state << "\t"
	<< arc.nextstate << "\t"
	<< key_table[arc.ilabel] << "\t"
	<< key_table[arc.olabel] << "\t"
	<< arc.weight << std::endl;				       
  }

  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator
  (StateId state,StdVectorFst * t):
    arc_iterator(new ArcIterator<StdVectorFst>(*t,state)), state(state), t(t)
  { end_iterator = arc_iterator->Done(); }

  TropicalWeightTransitionIterator::TropicalWeightTransitionIterator(void):
    arc_iterator(NULL),state(0), t(NULL), end_iterator(true)
  {}
  
  TropicalWeightTransitionIterator::~TropicalWeightTransitionIterator(void)
  { delete arc_iterator; }

  void TropicalWeightTransitionIterator::operator=  
  (const TropicalWeightTransitionIterator &another)
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
    arc_iterator = new ArcIterator<StdVectorFst>(*(another.t),another.state);
    arc_iterator->Seek(another.arc_iterator->Position());
    t = another.t;
    end_iterator = false;
  }

  bool TropicalWeightTransitionIterator::operator== 
  (const TropicalWeightTransitionIterator &another)
  { if (end_iterator and another.end_iterator)
      { return true; }
    if (end_iterator or another.end_iterator)
      { return false; }
    return 
      (t == another.t) and
      (state == another.state) and
      (arc_iterator->Position() == another.arc_iterator->Position());
  }

  bool TropicalWeightTransitionIterator::operator!= 
  (const TropicalWeightTransitionIterator &another)
  { return not (*this == another); }

  const TropicalWeightTransition TropicalWeightTransitionIterator::operator* 
  (void)
  { return TropicalWeightTransition(arc_iterator->Value(),state,t); }

  void TropicalWeightTransitionIterator::operator++ (void)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }

  void TropicalWeightTransitionIterator::operator++ (int)
  {
    if (end_iterator)
      { return; }
    arc_iterator->Next();
    if (arc_iterator->Done())
      { end_iterator = true; }
  }
  */

  fst::SymbolTable * create_symbol_table(std::string name);

  fst::SymbolTable * create_symbol_table(std::string name) {
    fst::SymbolTable * st = new fst::SymbolTable(name);
    st->AddSymbol("@_EPSILON_SYMBOL_@", 0);
    st->AddSymbol("@_UNKNOWN_SYMBOL_@", 1);
    st->AddSymbol("@_IDENTITY_SYMBOL_@", 2);
    return st;
  }
  
  void initialize_symbol_tables(StdVectorFst *t) {
    SymbolTable *st = create_symbol_table("");
    t->SetInputSymbols(st);
    //t->SetOutputSymbols(st);
    return;
  }

  StdVectorFst * TropicalWeightTransducer::create_empty_transducer(void)
  { 
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s = t->AddState();
    t->SetStart(s);
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::create_epsilon_transducer(void)
  { 
    StdVectorFst * t = new StdVectorFst;
    StateId s = t->AddState();
    t->SetStart(s);
    t->SetFinal(s,0);
    return t;
  }

  // could these be removed?
  StdVectorFst * TropicalWeightTransducer::define_transducer(Key k)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(k,k,0,s2));
    return t;
  }
  StdVectorFst * TropicalWeightTransducer::define_transducer
  (const KeyPair &key_pair)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(key_pair.first,key_pair.second,0,s2));
    return t;
  }

  StdVectorFst * TropicalWeightTransducer::define_transducer(const std::string &symbol)
  {
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(t->InputSymbols()->AddSymbol(symbol),
			t->InputSymbols()->AddSymbol(symbol),0,s2));
    return t;
  }
  StdVectorFst * TropicalWeightTransducer::define_transducer
    (const std::string &isymbol, const std::string &osymbol)
  {
    StdVectorFst * t = new StdVectorFst;
    initialize_symbol_tables(t);
    StateId s1 = t->AddState();
    StateId s2 = t->AddState();
    t->SetStart(s1);
    t->SetFinal(s2,0);
    t->AddArc(s1,StdArc(t->InputSymbols()->AddSymbol(isymbol),
			t->InputSymbols()->AddSymbol(osymbol),0,s2));
    return t;
  }

  // HERE

  StdVectorFst * TropicalWeightTransducer::define_transducer
  (const KeyPairVector &kpv)
  {
    StdVectorFst * t = new StdVectorFst;
    StateId s1 = t->AddState();
    t->SetStart(s1);
    for (KeyPairVector::const_iterator it = kpv.begin();
	 it != kpv.end();
	 ++it)
      {
	StateId s2 = t->AddState();
	t->AddArc(s1,StdArc(it->first,it->second,0,s2));
	s1 = s2;
      }
    t->SetFinal(s1,0);
    return t;
  }

  StdVectorFst * 
  TropicalWeightTransducer::copy(StdVectorFst * t)
  { return new StdVectorFst(*t); }

  StdVectorFst * 
  TropicalWeightTransducer::determinize(StdVectorFst * t)
  {
    RmEpsilonFst<StdArc> rm(*t);
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(rm,
			  &encode_mapper);
    DeterminizeFst<StdArc> det(enc);
    DecodeFst<StdArc> dec(det,
			  encode_mapper);
    return new StdVectorFst(dec);
  }
  
  StdVectorFst * TropicalWeightTransducer::minimize
  (StdVectorFst * t)
  {
    StdVectorFst * determinized_t = determinize(t);
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(*determinized_t,
			  &encode_mapper);
    StdVectorFst fst_enc(enc);
    Minimize<StdArc>(&fst_enc);
    DecodeFst<StdArc> dec(enc,
			  encode_mapper);
    delete determinized_t;
    return new StdVectorFst(dec);
  }

  /* For HfstMutableTransducer */

  StateId 
  TropicalWeightTransducer::add_state(StdVectorFst *t)
  { 
    StateId s = t->AddState();
    if (s == 0)
      t->SetStart(s);
    return s;
  }

  void 
  TropicalWeightTransducer::set_final_weight(StdVectorFst *t, StateId s, float w)
  {
    t->SetFinal(s, w);
    return;
  }

  void 
  TropicalWeightTransducer::add_transition(StdVectorFst *t, StateId source, std::string &isymbol, std::string &osymbol, float w, StateId target)
  {
    /*if (t->InputSymbols() != t->OutputSymbols()) {
      fprintf(stderr, "ERROR:  TropicalWeightTransducer::add_transition:  input and output symbols are not the same\n"); 
      throw hfst::exceptions::ErrorException(); 
      }*/
    Key ilabel = t->InputSymbols()->AddSymbol(isymbol);
    Key olabel = t->InputSymbols()->AddSymbol(osymbol);
    t->AddArc(source, StdArc(ilabel, olabel, w, target));
    return;
  }

  float 
  TropicalWeightTransducer::get_final_weight(StdVectorFst *t, StateId s)
  {
    return t->Final(s).Value();
  }

  float 
  TropicalWeightTransducer::is_final(StdVectorFst *t, StateId s)
  {
    return ( t->Final(s) != TropicalWeight::Zero() );
  }

  StateId
  TropicalWeightTransducer::get_initial_state(StdVectorFst *t)
  {
    return t->Start();
  }

  StdVectorFst * 
  TropicalWeightTransducer::remove_epsilons(StdVectorFst * t)
  { return new StdVectorFst(RmEpsilonFst<StdArc>(*t)); }

  StdVectorFst * 
  TropicalWeightTransducer::n_best(StdVectorFst * t,int n)
  { 
    if (n < 0)
      { throw ImpossibleTransducerPowerExcpetion(); }
    StdVectorFst * n_best_fst = new StdVectorFst(); 
    fst::ShortestPath(*t,n_best_fst,(size_t)n);
    return n_best_fst;
  }

  StdVectorFst * 
  TropicalWeightTransducer::repeat_star(StdVectorFst * t)
  { return new StdVectorFst(ClosureFst<StdArc>(*t,CLOSURE_STAR)); }

  StdVectorFst * 
  TropicalWeightTransducer::repeat_plus(StdVectorFst * t)
  { return new StdVectorFst(ClosureFst<StdArc>(*t,CLOSURE_PLUS)); }

  StdVectorFst *
  TropicalWeightTransducer::repeat_n(StdVectorFst * t,int n)
  {
    if (n <= 0)
      { return create_empty_transducer(); }

    StdVectorFst * repetition = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { Concat(repetition,*t); }
    return repetition;
  }

  StdVectorFst *
  TropicalWeightTransducer::repeat_le_n(StdVectorFst * t,int n)
  {
    if (n <= 0)
      { return create_empty_transducer(); }

    StdVectorFst * repetition = create_epsilon_transducer();
    StdVectorFst * t_i_times = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      { 
	Union(repetition,*t_i_times); 
	Concat(t_i_times,*t);
      }
    delete t_i_times;
    return repetition;
  }

  StdVectorFst * 
  TropicalWeightTransducer::optionalize(StdVectorFst * t)
  {
    StdVectorFst * eps = create_epsilon_transducer();
    Union(eps,*t);
    return eps;
  }

  StdVectorFst * 
  TropicalWeightTransducer::invert(StdVectorFst * t)
  {
    StdVectorFst * inverse = copy(t);
    Invert(inverse);
    return inverse;
  }

  /* Makes valgrind angry... */
  StdVectorFst * 
  TropicalWeightTransducer::reverse(StdVectorFst * t)
  {
    StdVectorFst * reversed = new StdVectorFst;
    Reverse<StdArc,StdArc>(*t,reversed);
    return reversed;
  }

  StdVectorFst * TropicalWeightTransducer::extract_input_language
  (StdVectorFst * t)
  { return new StdVectorFst(ProjectFst<StdArc>(*t,PROJECT_INPUT)); }

  StdVectorFst * TropicalWeightTransducer::extract_output_language
  (StdVectorFst * t)
  { return new StdVectorFst(ProjectFst<StdArc>(*t,PROJECT_OUTPUT)); }
  
  typedef std::pair<int,int> LabelPair;
  typedef std::vector<LabelPair> LabelPairVector;
  StdVectorFst * TropicalWeightTransducer::substitute
  (StdVectorFst * t,Key old_key,Key new_key) 
  {
    LabelPairVector v;
    v.push_back(LabelPair(old_key,new_key));
    RelabelFst<StdArc> t_subst(*t,v,v);
    return new StdVectorFst(t_subst);
  }
  
  StdVectorFst * TropicalWeightTransducer::substitute(StdVectorFst * t,
			    KeyPair old_key_pair,
			    KeyPair new_key_pair)
  {
    EncodeMapper<StdArc> encode_mapper(0x0001,ENCODE);
    EncodeFst<StdArc> enc(*t,&encode_mapper);

    StdArc old_pair_code = 
      encode_mapper(StdArc(old_key_pair.first,old_key_pair.second,0,0));
    StdArc new_pair_code =
      encode_mapper(StdArc(new_key_pair.first,new_key_pair.second,0,0));

    // First cast up, then cast down... For some reason dynamic_cast<StdVectorFst*>
    // doesn't work although both EncodeFst<StdArc> and StdVectorFst extend Fst<StdArc>. 
    // reinterpret_cast worked, but that is apparently unsafe...
    StdVectorFst * subst = 
      substitute(static_cast<StdVectorFst*>(static_cast<Fst<StdArc>*>(&enc)),
		 static_cast<Key>(old_pair_code.ilabel),
		 static_cast<Key>(new_pair_code.ilabel));

    DecodeFst<StdArc> dec(*subst,encode_mapper);
    delete subst;
    return new StdVectorFst(dec);
  }

  StdVectorFst * TropicalWeightTransducer::compose(StdVectorFst * t1,
			 StdVectorFst * t2)
  {
    ComposeFst<StdArc> compose(*t1,*t2);
    return new StdVectorFst(compose);
  }

  StdVectorFst * TropicalWeightTransducer::concatenate(StdVectorFst * t1,
						       StdVectorFst * t2)
  {
    ConcatFst<StdArc> concatenate(*t1,*t2);
    return new StdVectorFst(concatenate);
  }

  StdVectorFst * TropicalWeightTransducer::disjunct(StdVectorFst * t1,
			  StdVectorFst * t2)
  {
    UnionFst<StdArc> disjunct(*t1,*t2);
    return new StdVectorFst(disjunct);
  }

  StdVectorFst * TropicalWeightTransducer::intersect(StdVectorFst * t1,
			   StdVectorFst * t2)
  {
    IntersectFst<StdArc> intersect(*t1,*t2);
    return new StdVectorFst(intersect);
  }

  StdVectorFst * TropicalWeightTransducer::subtract(StdVectorFst * t1,
			  StdVectorFst * t2)
  {
    DifferenceFst<StdArc> subtract(*t1,*t2);
    return new StdVectorFst(subtract);
  }

  StdVectorFst * TropicalWeightTransducer::set_weight(StdVectorFst * t,float f)
  {
    StdVectorFst * t_copy = new StdVectorFst(*t);
    for (fst::StateIterator<StdVectorFst> iter(*t); 
	 not iter.Done(); iter.Next())
      {
	if (t_copy->Final(iter.Value()) != fst::TropicalWeight::Zero())
	  { t_copy->SetFinal(iter.Value(),f); }
      }
    return t_copy;
  }
  /*
  TropicalWeightTransducer::const_iterator 
  TropicalWeightTransducer::begin(StdVectorFst * t)
  { return TropicalWeightStateIterator(t); }

  TropicalWeightTransducer::const_iterator 
  TropicalWeightTransducer::end(StdVectorFst * t)
  { (void)t;
    return TropicalWeightStateIterator(); }

  void TropicalWeightTransducer::print
  (StdVectorFst * t, KeyTable &key_table, ostream &out) 
  {
    TropicalWeightStateIndexer indexer(t);
    for (TropicalWeightTransducer::const_iterator it = begin(t);
	 it != end(t);
	 ++it)
      { 
	TropicalWeightState s = *it;
	s.print(key_table,out,indexer);
      }
  }
  */

  void TropicalWeightTransducer::print
  (StdVectorFst * t, KeyTable &key_table, ostream &out) 
  {
    (void)t;
    (void)key_table;
    (void)out;
    return; // dummy implementation
  }


#ifdef foo
  StdVectorFst * TropicalWeightTransducer::harmonize
  (StdVectorFst * t,KeyMap &key_map)
  {
    LabelPairVector v;
    for (KeyMap::iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      {
	v.push_back(LabelPair(it->first,it->second));
      }
    RelabelFst<StdArc> t_subst(*t,v,v);
    return new StdVectorFst(t_subst);
  }
#endif

  // TODO
  void expand_unknown(StdVectorFst *t, KeyTable key_table, SymbolSet &expand_unknown,
		      SymbolPairSet &expand_non_identity, Symbol unknown_symbol)
  {
    /*
    Key unknown_key = key_table.get_key(unknown_symbol);
      for (fst::StateIterator<StdVectorFst> iter(*t); 
	   not iter.Done(); iter.Next())
	{
	  StateId s = iter.Value();
	  for (fst::ArcIterator<StdVectorFst> it(*t,s); !it.Done(); it.Next())
	    {
	      const StdArc &arc = it.Value();
	      if (arc.ilabel == unknown_key && arc.olabel == unknown_key) {
		
	      }
		
	    }
	  
	    }*/
    (void)t;
    (void)key_table;
    (void)expand_unknown;
    (void)expand_non_identity;
    (void)unknown_symbol;
    return;
  }

  void extract_reversed_strings
  (StdVectorFst * t, StdArc::StateId s, KeyTable &kt,
   WeightedStrings<float>::Vector &reversed_results)
  {
    WeightedStrings<float>::Vector reversed_continuations;
    for (fst::ArcIterator<StdVectorFst> it(*t,s); !it.Done(); it.Next())
      {
	const StdArc &arc = it.Value();
	extract_reversed_strings(t,arc.nextstate,kt,reversed_continuations);
	std::string istring;
	std::string ostring;

	if (arc.ilabel != 0)
	  { istring = kt[arc.ilabel]; }
	if (arc.olabel != 0)
	  { ostring = kt[arc.olabel]; }
	WeightedString<float> 
	  arc_string(istring,ostring,arc.weight.Value());
	WeightedStrings<float>::add(arc_string,reversed_continuations);
	WeightedStrings<float>::cat(reversed_results,reversed_continuations);
	reversed_continuations.clear();
      }
    if (t->Final(s) != TropicalWeight::Zero()) 
      { reversed_results.push_back(WeightedString<float>
				   ("","",t->Final(s).Value())); }
  }

  void TropicalWeightTransducer::extract_strings
  (StdVectorFst * t, KeyTable &kt, WeightedStrings<float>::Set &results)
  {
    if (t->Start() == -1)
      { return; }
    WeightedStrings<float>::Vector reversed_results;
    extract_reversed_strings(t,t->Start(),kt,reversed_results);
    results.insert(reversed_results.begin(),reversed_results.end());
  }

  void TropicalWeightTransducer::represent_empty_transducer_as_having_one_state(StdVectorFst *t)
  {
    if (t->Start() == fst::kNoStateId || t->NumStates() == 0) {
      delete t;
      t = create_empty_transducer();
    }
    return;
  }

  }
}

#ifdef DEBUG_MAIN
using namespace hfst::implementations;
hfst::symbols::GlobalSymbolTable KeyTable::global_symbol_table;
int main(void) 
{
  TropicalWeightTransducer ofst;
  StdVectorFst * t = ofst.create_empty_transducer();
  for (TropicalWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      TropicalWeightState s = *it;
    }
  delete t;
  t = ofst.create_epsilon_transducer();
  for (TropicalWeightTransducer::const_iterator it = ofst.begin(t);
       it != ofst.end(t);
       ++it)
    {
      TropicalWeightState s = *it;
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
  StdVectorFst * tt;
  tt = ofst.copy(t);
  tt->AddArc(0,StdArc(0,0,0,1));
  ofst.print(tt,key_table,std::cerr);
  StdVectorFst * tt_det = ofst.minimize(tt);
  ofst.print(tt_det,key_table,std::cerr);
  delete t;
  t = ofst.invert(tt);
  delete t;
  delete tt;
  delete tt_det;
  std::cerr << "Test substitution" << std::endl;
  StdVectorFst * fst = ofst.define_transducer(KeyPair(key_table["a"],
						      key_table["b"]));
  std::cerr << "Print a:b" << std::endl;
  ofst.print(fst,key_table,std::cerr);
  key_table.add_symbol("c");
  StdVectorFst * fst1 = ofst.substitute(fst,key_table["a"],key_table["c"]);
  std::cerr << "Print c:b" << std::endl;
  ofst.print(fst1,key_table,std::cerr);
  StdVectorFst * fst2 = 
    ofst.substitute(fst,
		    KeyPair(key_table["a"],key_table["b"]),
		    KeyPair(key_table["c"],key_table["c"]));
  std::cerr << "Print c:c" << std::endl;
  ofst.print(fst2,key_table,std::cerr);
  delete fst;
  delete fst1;
  delete fst2;
  TropicalWeightInputStream input;
  StdVectorFst * input_fst = input.read_transducer(key_table);
  ofst.print(input_fst,key_table,std::cerr);
  delete input_fst;
}
#endif
