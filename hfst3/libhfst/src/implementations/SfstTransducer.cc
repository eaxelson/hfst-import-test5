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

#include "SfstTransducer.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace hfst { namespace implementations {

  void sfst_set_hopcroft(bool value) {
    SFST::Transducer::hopcroft_minimisation=value;
  }

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
    return is_fst(input_file);
  }
  
  bool SfstInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == (int)'a';
  }
  
  bool SfstInputStream::is_fst(std::istream &s)
  {
    return s.good() && (s.peek() == (int)'a');
  }

  void SfstInputStream::add_symbol(StringNumberMap &string_number_map,
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

  Transducer * SfstTransducer::expand_arcs(Transducer * t, StringSet &unknown)
  {
    //std::set<char*> unknown_c_str;
    //for (StringSet::iterator it = unknown.begin(); it != unknown.end(); it++) {
    //  unknown_c_str.insert(strdup(it->c_str()));
    //}
    Transducer &tc = t->copy();
    SfstTransducer::expand(&tc, unknown);
    //for (std::set<char*>::iterator it = unknown_c_str.begin(); it != unknown_c_str.end(); it++) {
    //7  free(*it);
    //}
    return &tc;
  }


  std::pair<Transducer*, Transducer*> SfstTransducer::harmonize 
  (Transducer *t1, Transducer *t2, bool unknown_symbols_in_use) 
  {

    // 1. Calculate the set of unknown symbols for transducers t1 and t2.

    StringSet unknown_t1;    // symbols known to another but not this
    StringSet unknown_t2;    // and vice versa

    if (unknown_symbols_in_use) {
      StringSet t1_symbols = get_string_set(t1);
      StringSet t2_symbols = get_string_set(t2);
      collect_unknown_sets(t1_symbols, unknown_t1,
			   t2_symbols, unknown_t2);
    }

    Transducer * new_t1 = &t1->copy(false, &t2->alphabet);
    t2->alphabet.insert_symbols(new_t1->alphabet);
    delete t1;
    t1 = new_t1;

#ifdef foo
    // 2. Add new symbols from transducer t1 to the symbol table of transducer t2...

    for ( StringSet::const_iterator it = unknown_t2.begin();
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

    if (unknown_symbols_in_use) {
      harmonized_t1 = expand_arcs(t1, unknown_t1);
      delete t1;
      
      harmonized_t2 = expand_arcs(t2, unknown_t2);
      delete t2;
    }
    else {
      harmonized_t1 = &t1->copy();
      harmonized_t2 = &t2->copy();
    }

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

  void SfstTransducer::print_test(Transducer *t)
  {
    std::cerr << *t;
  }

  void SfstTransducer::print_alphabet(Transducer *t) {
    printf("alphabet..\n");
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it = cm.begin(); it != cm.end(); it++)
      printf("%i\t%s\n",it->first,it->second);
    printf("..alphabet\n");
  }

  void SfstTransducer::initialize_alphabet(Transducer *t) {

    const char * EPSILON_STRING = "<>";
    const char * UNKNOWN_STRING = "@_UNKNOWN_SYMBOL_@";
    const char * IDENTITY_STRING = "@_IDENTITY_SYMBOL_@";
    const unsigned int EPSILON_NUMBER = 0;
    const unsigned int UNKNOWN_NUMBER = 1;
    const unsigned int IDENTITY_NUMBER = 2;

    t->alphabet.clear();
    t->alphabet.utf8 = true;
    t->alphabet.add_symbol(EPSILON_STRING, EPSILON_NUMBER);
    t->alphabet.add_symbol(UNKNOWN_STRING, UNKNOWN_NUMBER);
    t->alphabet.add_symbol(IDENTITY_STRING, IDENTITY_NUMBER);
  }

  Transducer * SfstTransducer::create_empty_transducer(void)
  { Transducer * retval = new Transducer(); 
    initialize_alphabet(retval);
    return retval;
  }
  
  Transducer * SfstTransducer::create_epsilon_transducer(void)
  { Transducer * t = new Transducer; 
    initialize_alphabet(t);
    t->root_node()->set_final(1);
    return t; }
  
  Transducer * SfstTransducer::define_transducer(unsigned int number)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(number),n,t);
    n->set_final(1);
    return t; }
  
    Transducer * SfstTransducer::define_transducer(unsigned int inumber, unsigned int onumber)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->new_node();
    t->root_node()->add_arc(Label(inumber, 
				  onumber),n,t);
    n->set_final(1);
    return t; }


  Transducer * SfstTransducer::define_transducer(const char *symbol)
  { Transducer * t = new Transducer;
    initialize_alphabet(t); 
    Node * n = t->new_node();

    unsigned int number;
    if (strcmp(symbol,"@_EPSILON_SYMBOL_@") == 0)
      number=0;
    else
      number=t->alphabet.add_symbol(symbol);

    t->root_node()->add_arc(Label(number),n,t);
    n->set_final(1);
    return t; }
  
  Transducer * SfstTransducer::define_transducer(const char *isymbol, const char *osymbol)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->new_node();

    unsigned int inumber,onumber;
    if (strcmp(isymbol,"@_EPSILON_SYMBOL_@") == 0)
      inumber=0;
    else
      inumber=t->alphabet.add_symbol(isymbol);
    if (strcmp(osymbol,"@_EPSILON_SYMBOL_@") == 0)
      onumber=0;
    else
      onumber=t->alphabet.add_symbol(osymbol);

    t->root_node()->add_arc(Label(inumber,onumber),n,t);
    n->set_final(1);
    return t; }

  Transducer * SfstTransducer::define_transducer(const StringPairVector &spv)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->root_node();
    for (StringPairVector::const_iterator it = spv.begin();
	 it != spv.end();
	 ++it)
      {
	Node * temp = t->new_node();

	unsigned int inumber,onumber;
	if (strcmp(it->first.c_str(),"@_EPSILON_SYMBOL_@") == 0 || 
	    strcmp(it->first.c_str(),"<>") == 0 )
	  inumber=0;
	else
	  inumber=t->alphabet.add_symbol(it->first.c_str());
	if (strcmp(it->second.c_str(),"@_EPSILON_SYMBOL_@") == 0 ||
	    strcmp(it->second.c_str(),"<>") == 0 )
	  onumber=0;
	else
	  onumber=t->alphabet.add_symbol(it->second.c_str());

	n->add_arc(Label(inumber,onumber),temp,t);
	n = temp;
      }
    n->set_final(1);
    return t; }

    Transducer * SfstTransducer::define_transducer(const StringPairSet &sps, bool cyclic)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->root_node();
    Node * new_node = n;
    if (not sps.empty()) {
      if (not cyclic)
	new_node = t->new_node();
      for (StringPairSet::const_iterator it = sps.begin();
	   it != sps.end();
	   ++it)
	{
	  unsigned int inumber,onumber;
	  if (strcmp(it->first.c_str(),"@_EPSILON_SYMBOL_@") == 0)
	    inumber=0;
	  else
	    inumber=t->alphabet.add_symbol(it->first.c_str());
	  if (strcmp(it->second.c_str(),"@_EPSILON_SYMBOL_@") == 0)
	    onumber=0;
	  else
	    onumber=t->alphabet.add_symbol(it->second.c_str());

	  n->add_arc(Label(inumber,onumber),new_node,t);
	}
    }
    new_node->set_final(1);
    return t; }

  Transducer * SfstTransducer::define_transducer(const std::vector<StringPairSet> &spsv)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->root_node();
    for (std::vector<StringPairSet>::const_iterator it = spsv.begin();
	 it != spsv.end();
	 ++it)
      {
	Node * temp = t->new_node();

	for (StringPairSet::const_iterator it2 = (*it).begin(); it2 != (*it).end(); it2++ ) 
	  {
	    unsigned int inumber,onumber;
	    if (strcmp(it2->first.c_str(),"@_EPSILON_SYMBOL_@") == 0 || 
		strcmp(it2->first.c_str(),"<>") == 0 )
	      inumber=0;
	    else
	      inumber=t->alphabet.add_symbol(it2->first.c_str());
	    if (strcmp(it2->second.c_str(),"@_EPSILON_SYMBOL_@") == 0 ||
		strcmp(it2->second.c_str(),"<>") == 0 )
	      onumber=0;
	    else
	      onumber=t->alphabet.add_symbol(it2->second.c_str());
	    
	    n->add_arc(Label(inumber,onumber),temp,t);
	  }

	n = temp;
      }
    n->set_final(1);
    return t; }

  Transducer * SfstTransducer::copy(Transducer * t)
  { return &t->copy(); }
  
  Transducer * SfstTransducer::determinize(Transducer * t)
  { return &t->determinise(); }
  
  Transducer * SfstTransducer::minimize(Transducer * t)
  { Transducer * retval = &t->minimise(false); 
    retval->alphabet.copy(t->alphabet); 
    return retval; }
  
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
  { if (n < 0) { throw ImpossibleTransducerPowerException(); }
    Transducer * power = create_epsilon_transducer();
    for (int i = 0; i < n; ++i)
      {
	Transducer * temp = &(*power + *t);
	delete power;
	power = temp;
      }
    return power; }
  
  Transducer * SfstTransducer::repeat_le_n(Transducer * t,int n)
  { if (n < 0) { throw ImpossibleTransducerPowerException(); }
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


  static bool extract_strings(Transducer * t, Node *node,
           Node2Int &all_visitations, Node2Int &path_visitations,
           vector<char>& lbuffer, int lpos, std::vector<char>& ubuffer, int upos,
           hfst::ExtractStringsCb& callback, int cycles,
           std::vector<hfst::FdState<Character> >* fd_state_stack, bool filter_fd)
  {
    if(cycles >= 0 && path_visitations[node] > cycles)
      return true;
    all_visitations[node]++;
    path_visitations[node]++;
    
    if(lpos > 0 && upos > 0) {
      lbuffer[lpos] = 0;
      ubuffer[upos] = 0;
      bool final = node->is_final();
      hfst::WeightedPath<float> path(&lbuffer[0],&ubuffer[0],0);
      hfst::ExtractStringsCb::RetVal ret = callback(path, final);
      if(!ret.continueSearch || !ret.continuePath)
      {
        path_visitations[node]--;
        return ret.continueSearch;
      }
    }
    
    // sort arcs by number of visitations
    vector<Arc*> arc;
    for( ArcsIter p(node->arcs()); p; p++ ) {
      Arc *a=p;
      Node *n=a->target_node();
      size_t i;
      for( i=0; i<arc.size(); i++ )
        if (all_visitations[n] < all_visitations[arc[i]->target_node()])
          break;
      arc.push_back(NULL);
      for( size_t k=arc.size()-1; k>i; k-- )
        arc[k] = arc[k-1];
      arc[i] = a;
    }
    
    bool res = true;
    for( size_t i=0; i<arc.size() && res == true; i++ ) {
      Label l = arc[i]->label();
      bool added_fd_state = false;
      
      if (fd_state_stack) {
        if(fd_state_stack->back().get_table().get_operation(l.lower_char()) != NULL) {
          fd_state_stack->push_back(fd_state_stack->back());
          if(fd_state_stack->back().apply_operation(l.lower_char()))
            added_fd_state = true;
          else {
            fd_state_stack->pop_back();
            continue; // don't follow the transition
          }
        }
      }
      
      int lp=lpos;
      int up=upos;
      
      Character lc=l.lower_char();
      Character uc=l.upper_char();
      if (lc != Label::epsilon && (!filter_fd || fd_state_stack->back().get_table().get_operation(lc)==NULL))
      {
        const char* c = t->alphabet.write_char(lc);
        size_t clen = strlen(c);
        if(lpos+clen >= lbuffer.size())
          lbuffer.resize(lbuffer.size()*2, 0);
        strcpy(&lbuffer[lpos], c);
        lp += clen;
      }
      if (uc != Label::epsilon && (!filter_fd || fd_state_stack->back().get_table().get_operation(uc)==NULL))
      {
        const char* c = t->alphabet.write_char(uc);
        size_t clen = strlen(c);
        if(upos+clen > ubuffer.size())
          ubuffer.resize(ubuffer.size()*2, 0);
        strcpy(&ubuffer[upos], c);
        up += clen;
      }
      
      res = extract_strings(t, arc[i]->target_node(), all_visitations, path_visitations,
			   lbuffer, lp, ubuffer, up, callback, cycles, fd_state_stack, filter_fd);
      
      if(added_fd_state)
        fd_state_stack->pop_back();
    }
    
    path_visitations[node]--;
    return res;
  }
  
  static const int BUFFER_START_SIZE = 64;
  
  void SfstTransducer::extract_strings(Transducer * t, hfst::ExtractStringsCb& callback, int cycles, FdTable<SFST::Character>* fd, bool filter_fd)
  {
    if(!t->root_node())
      return;
    
    vector<char> lbuffer(BUFFER_START_SIZE, 0);
    vector<char> ubuffer(BUFFER_START_SIZE, 0);
    Node2Int all_visitations;
    Node2Int path_visitations;
    vector<hfst::FdState<Character> >* fd_state_stack = (fd==NULL) ? NULL : new std::vector<hfst::FdState<Character> >(1, hfst::FdState<Character>(*fd));
    
    hfst::implementations::extract_strings(t, t->root_node(), all_visitations, path_visitations, lbuffer, 0, ubuffer, 0, callback, cycles, fd_state_stack, filter_fd);
  }


  Transducer * SfstTransducer::insert_freely(Transducer * t, const StringPair &symbol_pair)
  {
    return &t->freely_insert( Label( t->alphabet.add_symbol(symbol_pair.first.c_str()),
				     t->alphabet.add_symbol(symbol_pair.second.c_str()) ));
  }

  
  Transducer * SfstTransducer::substitute(Transducer * t,String old_symbol,String new_symbol)
  { Transducer * retval = &t->replace_char(t->alphabet.add_symbol(old_symbol.c_str()),t->alphabet.add_symbol(new_symbol.c_str()));
    retval->alphabet.copy(t->alphabet);
    return retval; }

  Transducer * SfstTransducer::substitute(Transducer *t, const StringPair &symbol_pair, Transducer *tr)
  { Transducer * retval = &t->splice( Label(
					t->alphabet.add_symbol(symbol_pair.first.c_str()),
					t->alphabet.add_symbol(symbol_pair.second.c_str()) ), tr );
    retval->alphabet.copy(t->alphabet);
    return retval;
  }

  
  Transducer * SfstTransducer::compose
  (Transducer * t1, Transducer * t2)
  { 
    return &t1->operator||(*t2); }

  Transducer * SfstTransducer::concatenate
  (Transducer * t1, Transducer * t2)
  { return &t1->operator+(*t2); }

  Transducer * SfstTransducer::disjunct
  (Transducer * t1, Transducer * t2)
  { return &t1->operator|(*t2); }

  Transducer * SfstTransducer::disjunct
  (Transducer * t, const StringPairVector &spv)
  {
    Node *node= t->root_node();
    for (StringPairVector::const_iterator it = spv.begin(); it != spv.end(); it++) 
      {
	unsigned int inumber,onumber;
	if (strcmp(it->first.c_str(),"@_EPSILON_SYMBOL_@") == 0 || 
	    strcmp(it->first.c_str(),"<>") == 0 )
	  inumber=0;
	else
	  inumber=t->alphabet.add_symbol(it->first.c_str());
	if (strcmp(it->second.c_str(),"@_EPSILON_SYMBOL_@") == 0 ||
	    strcmp(it->second.c_str(),"<>") == 0 )
	  onumber=0;
	else
	  onumber=t->alphabet.add_symbol(it->second.c_str());

	Label l(inumber, onumber);
	t->alphabet.insert(l);
	Arcs *arcs=node->arcs();
	node = arcs->target_node( l );
	if (node == NULL) {
	  node = t->new_node();
	  arcs->add_arc( l, node, t );
	}
    }
    node->set_final(1);
    return t;
  }

  Transducer * SfstTransducer::intersect
  (Transducer * t1, Transducer * t2)
  { return &t1->operator&(*t2); }

  Transducer * SfstTransducer::subtract
  (Transducer * t1, Transducer * t2)
  { return &t1->operator/(*t2); }

  bool SfstTransducer::are_equivalent(Transducer * t1, Transducer * t2)
  {
    return (*t1 == *t2);
  }
  
  bool SfstTransducer::is_cyclic(Transducer * t)
  {
    return t->is_cyclic();
  }


  FdTable<SFST::Character>* SfstTransducer::get_flag_diacritics(Transducer * t)
  {
    FdTable<SFST::Character>* table = new FdTable<SFST::Character>();
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for (SFST::Alphabet::CharMap::const_iterator it = cm.begin(); it != cm.end(); it++) {
      if(FdOperation::is_diacritic(it->second))
        table->define_diacritic(it->first, it->second);
    }
    return table;
  }

  StringSet SfstTransducer::get_string_set(Transducer * t)
  {
    StringSet s;
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for ( SFST::Alphabet::CharMap::const_iterator it = cm.begin();
	  it != cm.end(); it++ ) {
      s.insert( std::string(it->second) );
    }
    return s;
  }

  /* Expand a transition according to the previously unknown symbols in s. */
  void SfstTransducer::expand_node( Transducer *t, Node *origin, Label &l, Node *target, hfst::StringSet &s )
  {
    if ( l.lower_char() == 1 && l.upper_char() == 1 )     // cross product "?:?"
      {
	for (hfst::StringSet::iterator it1 = s.begin(); it1 != s.end(); it1++) 
	  {
	    int inumber = t->alphabet.symbol2code(it1->c_str());
	    for (hfst::StringSet::iterator it2 = s.begin(); it2 != s.end(); it2++) 
	      {
		int onumber = t->alphabet.symbol2code(it2->c_str());
		if (inumber != onumber) {  
		  // add transitions of type x:y (non-identity cross-product of symbols in s)
		  origin->add_arc( Label(inumber, onumber), target, t );
		}
	      }
	    // add transitions of type x:? and ?:x here
	    origin->add_arc( Label(inumber, 1), target, t );
	    origin->add_arc( Label(1, inumber), target, t );
	  }
      }
    else if (l.lower_char() == 2 && l.upper_char() == 2 )  // identity "?:?"	     
      {
	for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	  {
	    int number = t->alphabet.symbol2code(it->c_str());
	    // add transitions of type x:x
	    origin->add_arc( Label(number, number), target, t );
	  }
      }
    else if (l.lower_char() == 1)  // "?:x"
      {
	for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	  {
	    int number = t->alphabet.symbol2code(it->c_str());
	    origin->add_arc( Label(number, l.upper_char()), target, t );
	  }
      }
    else if (l.upper_char() == 1)  // "x:?"
      {
	for (hfst::StringSet::iterator it = s.begin(); it != s.end(); it++) 
	  {
	    int number = t->alphabet.symbol2code(it->c_str());
	    origin->add_arc( Label(l.lower_char(), number), target, t );
	  }
      }  
    // keep the original transition in all cases
    return;
  }

  /*******************************************************************/
  /*                                                                 */
  /*  HFST addition                                                  */
  /*  Transducer::expand_nodes                                       */
  /*                                                                 */
  /*******************************************************************/

  void SfstTransducer::expand2( 
			       Transducer *t, Node *node,
			       hfst::StringSet &new_symbols, std::set<Node*> &visited_nodes )
  {
    if (visited_nodes.find(node) == visited_nodes.end()) {
      visited_nodes.insert(node);
      // iterate over all outgoing arcs of node
      for( ArcsIter p(node->arcs()); p; p++ ) {
	Arc *arc=p;
	expand2(t, arc->target_node(), new_symbols, visited_nodes);
	Label l = arc->label();
	expand_node( t, node, l, arc->target_node(), new_symbols);
      }
    }
    return;
  }
    
  /* Expand all transitions according to the previously unknown symbols
     listed in new_symbols. */
  void SfstTransducer::expand(Transducer *t, hfst::StringSet &new_symbols)
  {
    std::set<Node*> visited_nodes;
    expand2(t, t->root_node(), new_symbols, visited_nodes);
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


