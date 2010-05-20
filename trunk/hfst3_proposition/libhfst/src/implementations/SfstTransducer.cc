#include "SfstTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {
    /** Create an SfstInputStream that reads from stdin. */
  SfstInputStream::SfstInputStream(void)
  {
    this->input_file = stdin;
  }
    /** Create an SfstInputStream that reads from file \a filename. */
  SfstInputStream::SfstInputStream(const char * filename):
  filename(filename)
  {
    input_file = NULL;
  }
    /** Open the stream. */
  void SfstInputStream::open(void)
  {
    if (filename == std::string())
      { return; }
    input_file = fopen(filename.c_str(),"r");
    if (input_file == NULL)
      { throw FileNotReadableException(); }
  }
    /** Close the stream. */
  void SfstInputStream::close(void)
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
  bool SfstInputStream::is_open(void)
  {
    return input_file != NULL;
  }
  
  bool SfstInputStream::is_eof(void)
  {
    if (not is_open())
      { return true; }
    int c = getc(input_file);
    bool retval = (feof(input_file) != 0);
    ungetc(c, input_file);
    return retval;
  }
  
  bool SfstInputStream::is_bad(void)
  {
    return is_eof();
  }
  
  bool SfstInputStream::is_good(void)
  {
    return not is_bad();
  };

  bool SfstInputStream::is_fst(void)
  {
    if (not is_good())
      { return false; }
    std::fpos_t position;
    std::fgetpos(input_file,&position);
    int sign = fgetc(input_file);
    std::fsetpos(input_file,&position);
    return sign == (int)'a';
  }

  void SfstInputStream::add_symbol(StringSymbolMap &string_number_map,
				   Character c,
				   Alphabet &alphabet)
  {
    const char * string_symbol = 
      alphabet.code2symbol(c);
    if (string_number_map.find(string_symbol) == string_number_map.end())
      { string_number_map[string_symbol] = c; }
    else if (string_number_map[string_symbol] != c)
      { throw SymbolRedefinedException(); }
  }

  void
  SfstInputStream::populate_key_table(KeyTable &key_table,
				      Alphabet &alphabet,
				      KeyMap &key_map)
  {
    Symbol max_symbol_number = 0;
    StringSymbolMap string_number_map;
    for (Alphabet::const_iterator it = alphabet.begin();
	 it != alphabet.end();
	 it++)
      {
	Label l = *it;

	if (max_symbol_number < l.lower_char())
	  { max_symbol_number = l.lower_char(); }

	if (max_symbol_number < l.upper_char())
	  { max_symbol_number = l.upper_char(); }

	try 
	  {
	    if (l.lower_char() != 0)
	      { add_symbol(string_number_map,l.lower_char(),alphabet); }
	    if (l.upper_char() != 0)
	      { add_symbol(string_number_map,l.upper_char(),alphabet); }
	  }
	catch (const SymbolRedefinedException e)
	  { throw e; }
      }


    StringSymbolVector string_symbols(max_symbol_number+1,"");
    for (StringSymbolMap::iterator it = string_number_map.begin();
	 it != string_number_map.end();
	 ++it)
      { string_symbols[it->second] = it->first; }

    StringSymbolVector dummy_symbols;
    KeyTable transducer_key_table;
    for (size_t i = 1; i < string_symbols.size(); ++i)
      {
	if (string_symbols[i] == "")
	  { 
	    ostringstream oss(ostringstream::out);
	    oss << "@EMPTY@" << i;
	    transducer_key_table.add_symbol(oss.str().c_str());
	  }
	else
	  { transducer_key_table.add_symbol(string_symbols[i].c_str()); }
      }
    try 
      { transducer_key_table.harmonize(key_map,key_table); }
    catch (const char * p)
      { throw p; }
  }

  /* Skip the identifier string "SFST_TYPE" */
  void SfstInputStream::skip_identifier_version_3_0(void)
  { 
    char sfst_identifier[10];
    int sfst_id_count = fread(sfst_identifier,10,1,input_file);
    if (sfst_id_count != 1)
      { throw NotTransducerStreamException(); }
    if (0 != strcmp(sfst_identifier,"SFST_TYPE"))
      { throw NotTransducerStreamException(); }
  }
  
  void SfstInputStream::skip_hfst_header(void)
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
  
#ifdef foo
  void SfstTransducer::harmonize(Transducer * t1, Transducer * t2)
  {
    Transducer * new_t1 = &t1->copy(false, &t2->alphabet);
    t2->alphabet.insert_symbols(new_t1->alphabet);
    delete t1;
    t1 = new_t1;
    return;
  }
#endif

  Transducer * SfstTransducer::expand_arcs(Transducer * t, StringSymbolSet &unknown)
  {
    return &t->expand(unknown);
  }


  std::pair<Transducer*, Transducer*> SfstTransducer::harmonize 
  (Transducer *t1, Transducer *t2) 
  {

    // 1. Calculate the set of unknown symbols for transducers t1 and t2.

    StringSymbolSet unknown_t1;    // symbols known to another but not this
    StringSymbolSet unknown_t2;    // and vice versa
    StringSymbolSet t1_symbols = get_string_symbol_set(t1);
    StringSymbolSet t2_symbols = get_string_symbol_set(t2);
    KeyTable::collect_unknown_sets(t1_symbols, unknown_t1,
				   t2_symbols, unknown_t2);

    Transducer * new_t1 = &t1->copy(false, &t2->alphabet);
    t2->alphabet.insert_symbols(new_t1->alphabet);
    delete t1;
    t1 = new_t1;

#ifdef foo
    // 2. Add new symbols from transducer t1 to the symbol table of transducer t2...

    for ( StringSymbolSet::const_iterator it = unknown_t2.begin();
	  it != unknown_t2.end(); it++ ) {
	t2->InputSymbols()->AddSymbol(*it);
	//fprintf(stderr, "added %s to the set of symbols unknown to t2\n", (*it).c_str());
    }
    // ...calculate the number mappings needed in harmonization...
    KeyMap km = create_mapping(t1, t2);

    // ... replace the symbol table of t1 with a copy of t2's symbol table
    delete t1->InputSymbols();
    t1->SetInputSymbols( new fst::SymbolTable(*(t2->InputSymbols())) );

    // ...and recode the symbol numbers of transducer t1 so that
    //    it follows the new symbol table.
    recode_symbol_numbers(t1, km);
#endif


    // 3. Calculate the set of symbol pairs to which a non-identity "?:?"
    //    transition is expanded for both transducers.
    
    Transducer *harmonized_t1;
    Transducer *harmonized_t2;

    harmonized_t1 = expand_arcs(t1, unknown_t1);
    delete t1;

    harmonized_t2 = expand_arcs(t2, unknown_t2);
    delete t2;

    // fprintf(stderr, "...TWT::harmonize\n");

    return std::pair<Transducer*, Transducer*>(harmonized_t1, harmonized_t2);

  }


#ifdef foo
  Transducer * SfstTransducer::harmonize(Transducer * t, KeyMap &key_map)
  {
    Key no_key_number = 0;
    for (KeyMap::const_iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      { if (no_key_number < it->first) { no_key_number = it->first; }
	if (no_key_number < it->second) { no_key_number = it->second; } }
    ++no_key_number;

    KeyMap temp_keys;

    /* Every permutation is a product of transpositions... 
       Only three times as slow as converting by an optimal
       method and way more fun. */
    for (KeyMap::const_iterator it = key_map.begin();
	 it != key_map.end();
	 ++it)
      {
	if (it->first != it->second)
	  { 
	    Key first = it->first;
	    Key second = it->second;
	    if (temp_keys.find(first) != temp_keys.end())
	      { first = temp_keys[first]; }
	    temp_keys[second] = first;
	    Transducer * temp = substitute(t,second,no_key_number);
	    delete t;
	    t = temp;
	    temp = substitute(t,first,second);
	    delete t;
	    t = temp;
	    temp = substitute(t,no_key_number,first);
	    delete t;
	    t = temp;
	  }
      }
    return t;
  }
#endif

  Transducer * SfstInputStream::read_transducer(KeyTable &key_table)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    Transducer * t = NULL;
    try 
      {
	skip_hfst_header();
	Transducer tt = Transducer(input_file,true);
	t = &tt.copy();
      }
    catch (const char * p)
      {
	delete t;
#ifdef DEBUG
	std::cerr << p << std::endl;
#endif
	throw TransducerHasWrongTypeException();
      }
    try
      {
	KeyMap key_map;
	populate_key_table(key_table,
			   t->alphabet,
			   key_map);
	//t = SfstTransducer::harmonize(t,key_map);  // FIX THIS
	t->alphabet.clear();	
	return t;
      }
    catch (const HfstSymbolsException e)
      { 
	delete t;
	throw e; 
      }
    return NULL;
  }

  Transducer * SfstInputStream::read_transducer(bool has_header)
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    Transducer * t = NULL;
    try 
      {
	if (has_header)
	  skip_hfst_header();
	Transducer tt = Transducer(input_file,true);
	//tt.alphabet.clear();
	t = &tt.copy();
	return t;
      }
    catch (const char * p)
      {
	delete t;
	throw TransducerHasWrongTypeException();
      }
    return NULL;
  };


  // ---------- SfstOutputStream functions ----------

  SfstOutputStream::SfstOutputStream(void)
  {}
  SfstOutputStream::SfstOutputStream(const char * str):
    filename(str)
  {}
  void SfstOutputStream::open(void) {
    if (filename != std::string()) {
      ofile = fopen(filename.c_str(), "wb");
      if (ofile == NULL)
	throw FileNotReadableException();
    } 
    else {
      ofile = stdout;
    }
  }
  void SfstOutputStream::close(void) 
  {
    if (filename != std::string())
      { fclose(ofile); }
  }
  void SfstOutputStream::write_3_0_library_header(FILE *file)
  {
    fputs("HFST3",file);
    fputc(0, file);
    fputs("SFST_TYPE",file);
    fputc(0, file);
  }
  void SfstOutputStream::write_transducer(Transducer * transducer) 
  { 
    write_3_0_library_header(ofile);
    transducer->store(ofile); 
  }

  // ------------------------------------------------

  
  SfstState::SfstState(Node * state, Transducer * t) 
  { 
    this->state = state; 
    this->t = t;
  }
  
  SfstState::SfstState(const SfstState &s)
  { 
    this->state = s.state; 
    this->t = s.t;
  }
  
  bool SfstState::get_final_weight(void) const
  { return state->is_final(); }
  
  bool SfstState::operator< (const SfstState &another) const
  { return state < another.state; };
  
  bool SfstState::operator== (const SfstState &another) const 
  { return state == another.state; };
  
  bool SfstState::operator!= (const SfstState &another) const 
  { return not (*this == another); };

  //TransitionIterator<Transducer,bool> SfstState::begin(void) const 
  SfstTransitionIterator SfstState::begin(void) const 
  { return   SfstTransitionIterator(state,t); } 
  
  //TransitionIterator<Transducer*,bool> SfstState::end(void) const 
  SfstTransitionIterator SfstState::end(void) const 
  { return   SfstTransitionIterator(); } 

  void SfstTransducer::print_test(Transducer *t)
  {
    std::cerr << *t;
  }
    
  void SfstState::print(KeyTable &key_table, ostream &out,
			SfstStateIndexer &indexer) const
  {
    for (SfstState::const_iterator it = begin();
	 it != end();
	 ++it)
      { 
	SfstTransition t = *it;
	t.print(key_table,out,indexer);
      }
    if (get_final_weight())
      { out << indexer[*this] << "\t" << 0 << std::endl; }
  }

  SfstTransition::SfstTransition(Arc * arc, Node * n, Transducer * t) 
  { this->arc = arc; this->source_state = n; this->t = t; }
  
  SfstTransition::SfstTransition(const SfstTransition &t)
  { this->arc = t.arc; this->source_state = t.source_state; this->t = t.t; }
  
  Key SfstTransition::get_input_key(void) const
  { return arc->label().lower_char(); }
  
  Key SfstTransition::get_output_key(void) const
  { return arc->label().upper_char(); }
  
  SfstState SfstTransition::get_target_state(void) const
  { return SfstState(arc->target_node(),t); }
  
  SfstState SfstTransition::get_source_state(void) const
  { return SfstState(source_state,t); }

  bool SfstTransition::get_weight(void) const
  { return true; }

  void SfstTransition::print(KeyTable &key_table, ostream &out,
			     SfstStateIndexer &indexer) const
  {
    out << indexer[get_source_state()] << "\t"
    	<< indexer[get_target_state()] << "\t"
	<< key_table.get_print_name(get_input_key()) << "\t"
	<< key_table.get_print_name(get_output_key()) << "\t"
	<< 0 << std::endl;
  }
    
  SfstTransitionIterator::SfstTransitionIterator
  (Node * state, Transducer * t) : 
    state(state), arc_iterator(state->arcs()), t(t), end_iterator(false) 
  { if (not arc_iterator) { end_iterator = true; } }
  
  SfstTransitionIterator::SfstTransitionIterator (void) :
    state(NULL), arc_iterator(), t(NULL), end_iterator(true)
  {}
  
  void SfstTransitionIterator::operator= 
  (const SfstTransitionIterator &another)
  {
    if (&another == this)
      { return; }
    state = another.state;
    arc_iterator = ArcsIter(state->arcs());
    t = another.t;
    end_iterator = another.end_iterator;
  }
  
  /* This requires the SFST ArcsIter operator* to be
     const qualified. */
  bool SfstTransitionIterator::operator== 
  (const SfstTransitionIterator &another)
  {
    if (end_iterator and another.end_iterator)
      { return true; }
    if (state != another.state)
      { return false; }
    if (end_iterator or another.end_iterator)
      { return false; }
#ifdef DEBUG
    assert(arc_iterator);
    assert(another.arc_iterator);
#endif
    Arc * a_this = arc_iterator;
    const Arc * a_another = another.arc_iterator;
    return a_this == a_another;
  }
  
  bool SfstTransitionIterator::operator!= 
  (const SfstTransitionIterator &another)
  { return not (*this == another); }
  
  
  const SfstTransition 
  SfstTransitionIterator::operator* (void)
  { 
    if (end_iterator)
      { throw TransitionIteratorOutOfRangeExeption(); }
#ifdef DEBUG
    assert(arc_iterator);
#endif
    return SfstTransition(arc_iterator,state,t); 
  }
  
  void SfstTransitionIterator::operator++ (void)
  {
    if (end_iterator) { return; }
#ifdef DEBUG
    assert(arc_iterator);
#endif
    arc_iterator++;
    if (not arc_iterator) { end_iterator = true; }
  }
  
  void SfstTransitionIterator::operator++ (int)
  {
    if (end_iterator) { return; }
#ifdef DEBUG
    assert(arc_iterator);
#endif
    arc_iterator++;
    if (not arc_iterator) { end_iterator = true; }
  }
    
  Transducer * SfstTransducer::create_empty_transducer(void)
  { return new Transducer; }
  
  Transducer * SfstTransducer::create_epsilon_transducer(void)
  { Transducer * t = new Transducer; 
    t->root_node()->set_final(1);
    return t; }
  
    // could these be removed?
  Transducer * SfstTransducer::define_transducer(Key k)
  { Transducer * t = new Transducer; 
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(k),n,t);
    n->set_final(1);
    return t; }
  Transducer * SfstTransducer::define_transducer(const KeyPair &kp)
  { Transducer * t = new Transducer;
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(kp.first,kp.second),n,t);
    n->set_final(1);
    return t; }
  
  Transducer * SfstTransducer::define_transducer(const char *symbol)
  { Transducer * t = new Transducer; 
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(t->alphabet.add_symbol(symbol)),n,t);
    n->set_final(1);
    return t; }
  
  Transducer * SfstTransducer::define_transducer(const char *isymbol, const char *osymbol)
  { Transducer * t = new Transducer;
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(t->alphabet.add_symbol(isymbol), 
				  t->alphabet.add_symbol(osymbol)),n,t);
    n->set_final(1);
    return t; }

  Transducer * SfstTransducer::define_transducer(const KeyPairVector &kpv)
  { Transducer * t = new Transducer;
    Node * n = t->root_node();
    for (KeyPairVector::const_iterator it = kpv.begin();
	 it != kpv.end();
	 ++it)
      {
	Node * temp = t->new_node();
	n->add_arc(Label(it->first,it->second),temp,t);
	n = temp;
      }
    n->set_final(1);
    return t; }

  Transducer * SfstTransducer::define_transducer(const StringPairVector &spv)
  { Transducer * t = new Transducer;
    Node * n = t->root_node();
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	Node * temp = t->new_node();
	n->add_arc(Label(t->alphabet.add_symbol(it->first.c_str()),t->alphabet.add_symbol(it->second.c_str())),temp,t);
	n = temp;
      }
    n->set_final(1);
    return t; }

  Transducer * SfstTransducer::copy(Transducer * t)
  { return &t->copy(); }
  
  Transducer * SfstTransducer::determinize(Transducer * t)
  { return &t->determinise(); }
  
  Transducer * SfstTransducer::minimize(Transducer * t)
  { return &t->minimise(false); }
  
  Transducer * SfstTransducer::remove_epsilons(Transducer * t)
  { return &t->remove_epsilons(); }
  
  Transducer * SfstTransducer::repeat_star(Transducer * t)
  { return &t->kleene_star(); }
  
  Transducer * SfstTransducer::repeat_plus(Transducer * t)
  { Transducer * star = repeat_star(t); 
    t = &(*t + *star);
    delete star;
    return t; }
  
  Transducer * SfstTransducer::repeat_n(Transducer * t,int n)
  { if (n < 0) { throw ImpossibleTransducerPowerExcpetion(); }
    Transducer * power = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      {
	Transducer * temp = &(*power + *t);
	delete power;
	power = temp;
      }
    return power; }
  
  Transducer * SfstTransducer::repeat_le_n(Transducer * t,int n)
  { if (n < 0) { throw ImpossibleTransducerPowerExcpetion(); }
    Transducer * result = create_empty_transducer();
    for (int i = 0; i < n+1; ++i)
      {
	Transducer * power = repeat_n(t,i);
	Transducer * temp = &(*power | *result);
	delete power;
	delete result;
	result = temp;
      }
    return result; }
  
  Transducer * SfstTransducer::optionalize(Transducer * t)
  { Transducer *eps = create_epsilon_transducer();
    Transducer *opt = &(*t | *eps);
    delete eps;
    return opt; }
  
  Transducer * SfstTransducer::invert(Transducer * t)
  { return &t->switch_levels(); }
  
  Transducer * SfstTransducer::reverse(Transducer * t)
  { return &t->reverse(); }
  
  Transducer * SfstTransducer::extract_input_language(Transducer * t)
  { t->complete_alphabet();
    return &t->lower_level(); }
  
  Transducer * SfstTransducer::extract_output_language(Transducer * t)
  { t->complete_alphabet();
    return &t->upper_level(); }
  
  void SfstTransducer::extract_strings(Transducer * t, hfst::implementations::WeightedStrings<float>::Set &results)
  {
    t->generate_hfst(results, true);
  }

  Transducer * SfstTransducer::substitute(Transducer * t,Key old_key,Key new_key)
  { return &t->replace_char(old_key,new_key); }
  
  Transducer * SfstTransducer::substitute(Transducer * t,StringSymbol old_symbol,StringSymbol new_symbol)
  { Transducer * retval = &t->replace_char(t->alphabet.add_symbol(old_symbol.c_str()),t->alphabet.add_symbol(new_symbol.c_str())); 
    return retval; }

  Transducer * SfstTransducer::substitute
  (Transducer * t,KeyPair old_key_pair,KeyPair new_key_pair)
  { 
    (void)t;
    (void)old_key_pair;
    (void)new_key_pair;
    throw FunctionNotImplementedException(); }
  
  Transducer * SfstTransducer::compose
  (Transducer * t1, Transducer * t2)
  { return &t1->operator||(*t2); }

  Transducer * SfstTransducer::concatenate
  (Transducer * t1, Transducer * t2)
  { return &t1->operator+(*t2); }

  Transducer * SfstTransducer::disjunct
  (Transducer * t1, Transducer * t2)
  { return &t1->operator|(*t2); }

  Transducer * SfstTransducer::intersect
  (Transducer * t1, Transducer * t2)
  { return &t1->operator&(*t2); }

  Transducer * SfstTransducer::subtract
  (Transducer * t1, Transducer * t2)
  { return &t1->operator/(*t2); }

  SfstStateIterator::SfstStateIterator(Transducer * t):
  node_numbering(*t),t(t),current_state(0),ended(false)
  {
    if (node_numbering.number_of_nodes() == 0)
      { ended = true; }
  }

  SfstStateIterator::SfstStateIterator(void):
  node_numbering(),t(NULL),current_state(0),ended(true)
  {}

  void SfstStateIterator::operator= (const SfstStateIterator &another)
  {
    if (&another == this)
      { return; }
    this->t = another.t;
    node_numbering = NodeNumbering(*t);
    current_state = another.current_state;
  }

  bool SfstStateIterator::operator== (const SfstStateIterator &another) const
  {
    if (ended and another.ended) 
      { return true; } 
    if (this->t == another.t)
      {
	if (this->current_state == another.current_state)
	  { return true; }       
      }
    return false;
  }

  bool SfstStateIterator::operator!= (const SfstStateIterator &another) const
  { return not (*this == another); }

  const SfstState SfstStateIterator::operator* (void)
  { return SfstState(node_numbering.get_node(current_state),t); }

  void SfstStateIterator::operator++ (void)
  { 
    ++current_state;
    if ((int)node_numbering.number_of_nodes() <= current_state)
      { ended = true; }
  }

  void SfstStateIterator::operator++ (int)
  { 
    ++current_state; 
    if ((int)node_numbering.number_of_nodes() <= current_state)
      { ended = true; }
  }

  SfstStateIterator SfstTransducer::begin(Transducer * t)
  { return SfstStateIterator(t); }

  SfstStateIterator SfstTransducer::end(Transducer * t)
  { (void)t;
    return SfstStateIterator(); }

  SfstStateIndexer::SfstStateIndexer(Transducer * t):
    t(t),numbering(*t)
  {}

  unsigned int SfstStateIndexer::operator[] (const SfstState &state)
  {
    if (state.t != t)
      { throw StateBelongsToAnotherTransducerException(); }
    return numbering[state.state];
  }

  const SfstState SfstStateIndexer::operator[] (unsigned int number)
  {
    if (numbering.number_of_nodes() <= number)
      { throw StateIndexOutOfBoundsException(); }
    return SfstState(numbering.get_node(number),t);
  }

  void SfstTransducer::print(Transducer * t, 
			     KeyTable &key_table, ostream &out)
  {
    SfstStateIndexer indexer(t);
    for (SfstStateIterator it = begin(t);
	 it != end(t);
	 ++it)
      {
	SfstState s = *it;
	s.print(key_table,out,indexer);
      }
  }


  StringSymbolSet SfstTransducer::get_string_symbol_set(Transducer * t)
  {
    StringSymbolSet s;
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for ( SFST::Alphabet::CharMap::const_iterator it = cm.begin();
	  it != cm.end(); it++ ) {
      s.insert( std::string(it->second) );
    }
    return s;
  }
  
  KeyMap create_mapping(Transducer * t1, Transducer * t2, StringSymbolSet &unknown2)
  {
    (void)t1;
    (void)t2;
    (void)unknown2;
    KeyMap km;
    return km;
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
