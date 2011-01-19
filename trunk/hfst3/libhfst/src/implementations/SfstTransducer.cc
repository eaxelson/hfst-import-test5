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
#include <time.h>

#ifndef DEBUG_MAIN
namespace hfst { namespace implementations {

    float sfst_seconds_in_harmonize=0;

    float SfstTransducer::get_profile_seconds() {
      return sfst_seconds_in_harmonize;
    }

  void sfst_set_hopcroft(bool value) {
    SFST::Transducer::hopcroft_minimisation=value;
  }

    /** Create an SfstInputStream that reads from stdin. */
  SfstInputStream::SfstInputStream(void):
      is_minimal(false)
  {
    this->input_file = stdin;
  }
    /** Create an SfstInputStream that reads from file \a filename. */
    SfstInputStream::SfstInputStream(const std::string &filename_):
      filename(std::string(filename_)), is_minimal(false)
  {
    if (filename == std::string())
      { input_file = stdin; }
    else {
      input_file = fopen(filename.c_str(),"r");
      if (input_file == NULL)
	{ throw FileNotReadableException(); }
    }
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

  char SfstInputStream::stream_get() {
    return (char) fgetc(input_file); }

  void SfstInputStream::stream_unget(char c) {
    ungetc ( (int)c, input_file ); }

    /*
  void SfstInputStream::stream_putback(char c) {
    if (EOF == ungetc(c, input_file))
      assert(false); }

  void SfstInputStream::stream_getline(char* s, streamsize n, char delim) {
    if ((int)n == 0)
      assert(false);
    unsigned int i=0;
    while(true)
      {
	if (i == ((unsigned int)n-1) ) {
	  s[i] = '\0';
	  break;
	}
	int c = getc(input_file);
	if (feof(input_file) || (c == delim)) {
	  s[i] = '\0';
	  break;
	}
	else
	  s[i] = delim;
	i++;
      }
      }*/

  
  bool SfstInputStream::is_eof(void)
  {
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

#ifdef FOO
  /* Skip the identifier string "MINIMAL" */
  bool SfstInputStream::skip_minimality_identifier(void)
  {
    char c = getc(input_file);
    ungetc(c,input_file);
    //fprintf(stderr, "skip_minimality_identifier: c == %c\n", c);
    if ( c != 'M') 
	return false;
    else 
      {
	char minimality_identifier[8];
	int count = fread(minimality_identifier,8,1,input_file);
	if (count != 1) {
	  throw NotTransducerStreamException();
	}
	if (0 != strcmp(minimality_identifier,"MINIMAL")) {
	  throw NotTransducerStreamException();
	}
	return true;
      }
  }

  /* Skip the identifier string "SFST_TYPE" */
  bool SfstInputStream::skip_identifier_version_3_0(void)
  { 
    char sfst_identifier[10];
    int sfst_id_count = fread(sfst_identifier,10,1,input_file);
    if (sfst_id_count != 1)
      { //fprintf(stderr, "#2\n");
	throw NotTransducerStreamException(); }
    if (0 != strcmp(sfst_identifier,"SFST_TYPE"))
      { //fprintf(stderr, "#3: %s\n", sfst_identifier);
	throw NotTransducerStreamException(); }
    return skip_minimality_identifier();
  }
  
  bool SfstInputStream::skip_hfst_header(void)
  {
    char hfst_header[6];
    int header_count = fread(hfst_header,6,1,input_file);
    if (header_count != 1)
      { //fprintf(stderr, "#1\n");
	throw NotTransducerStreamException(); }
    try { return skip_identifier_version_3_0(); }
    catch (NotTransducerStreamException e) { throw e; }
  }
#endif // FOO  


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

    //clock_t startclock = clock();

    //std::cerr << *t1 << "--\n" << *t2;

    // 1. Calculate the set of unknown symbols for transducers t1 and t2.

    StringSet unknown_t1;    // symbols known to another but not this
    StringSet unknown_t2;    // and vice versa

    if (unknown_symbols_in_use) {
      StringSet t1_symbols = get_alphabet(t1);
      StringSet t2_symbols = get_alphabet(t2);
      collect_unknown_sets(t1_symbols, unknown_t1,
			   t2_symbols, unknown_t2);
    }

    Transducer * new_t1 = &t1->copy(false, &t2->alphabet);
    t2->alphabet.insert_symbols(new_t1->alphabet);
    delete t1;
    t1 = new_t1;

    //std::cerr << *t1 << "--\n" << *t2;

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

    //clock_t endclock = clock();

    /* sfst_seconds_in_harmonize = sfst_seconds_in_harmonize + 
       ( (float)(endclock - startclock) / CLOCKS_PER_SEC); */

    return std::pair<Transducer*, Transducer*>(harmonized_t1, harmonized_t2);

  }

    void SfstInputStream::ignore(unsigned int n)
    { 
      for (unsigned int i=0; i<n; i++)
	fgetc(input_file);
    }

    bool SfstInputStream::set_implementation_specific_header_data(StringPairVector &header_data, unsigned int index)
    {
      if (index != (header_data.size()-1) )
	return false;

      if ( not ( strcmp("minimal", header_data[index].first.c_str()) == 0) )
	return false;

      if ( strcmp("true", header_data[index].second.c_str()) == 0 )
	is_minimal=true;  // SEGFAULT?
      else if ( strcmp("false", header_data[index].second.c_str()) == 0 )
	is_minimal=false;
      else
	return false;

      return true;
    }
    
    unsigned int SfstTransducer::number_of_states(Transducer* )
    {
      return -1;
    }

    Transducer * SfstInputStream::read_transducer()
  {
    if (is_eof())
      { throw FileIsClosedException(); }
    Transducer * t = NULL;
    try 
      {
	Transducer * t = new Transducer(input_file,true);
	//tt.alphabet.clear();
	//t = &tt.copy();
	if (not is_minimal) {
	  t->minimised = false;
	  t->deterministic = false;
	}
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
  { ofile = stdout; }

    SfstOutputStream::SfstOutputStream(const std::string &str):
    filename(std::string(str))
  {
    if (filename != std::string()) {
      ofile = fopen(filename.c_str(), "wb");
      if (ofile == NULL)
	throw FileNotReadableException();
    } 
    else
      ofile = stdout;
  }

  void SfstOutputStream::close(void) 
  {
    if (filename != std::string())
      { fclose(ofile); }
  }

    void SfstOutputStream::append_implementation_specific_header_data(std::vector<char> &header, Transducer *t)
    {
      std::string min("minimal");
      for (unsigned int i=0; i<min.length(); i++)
	header.push_back(min[i]);
      header.push_back('\0');

      std::string min_value;
      if (t->minimised && t->deterministic)
	min_value = std::string("true");
      else
	min_value = std::string("false");

      for (unsigned int i=0; i<min_value.length(); i++)
	header.push_back(min_value[i]);
      header.push_back('\0');
    }

    /*
    void SfstOutputStream::write_3_0_library_header(FILE *file, bool is_minimal)
  {
    fputs("HFST3",file);
    fputc(0, file);
    fputs("SFST_TYPE",file);
    fputc(0, file);
    if (is_minimal) {
      fputs("MINIMAL",file);
      fputc(0, file); 
    }
  }
    */

    void SfstOutputStream::write(const char &c)
    {
      fputc(c,ofile);
    }

    void SfstOutputStream::write_transducer(Transducer * transducer)
  { 
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


    Transducer * SfstTransducer::define_transducer(const std::string &symbol)
  { Transducer * t = new Transducer;
    initialize_alphabet(t); 
    Node * n = t->new_node();

    unsigned int number;
    if (strcmp(symbol.c_str(),"@_EPSILON_SYMBOL_@") == 0)
      number=0;
    else
      number=t->alphabet.add_symbol(symbol.c_str());

    t->root_node()->add_arc(Label(number),n,t);
    n->set_final(1);
    return t; }
  
    Transducer * SfstTransducer::define_transducer(const std::string &isymbol, const std::string &osymbol)
  { Transducer * t = new Transducer;
    initialize_alphabet(t);
    Node * n = t->new_node();

    unsigned int inumber,onumber;
    if (strcmp(isymbol.c_str(),"@_EPSILON_SYMBOL_@") == 0)
      inumber=0;
    else
      inumber=t->alphabet.add_symbol(isymbol.c_str());
    if (strcmp(osymbol.c_str(),"@_EPSILON_SYMBOL_@") == 0)
      onumber=0;
    else
      onumber=t->alphabet.add_symbol(osymbol.c_str());

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

  std::pair<Transducer*, Transducer*> SfstTransducer::harmonize 
  (Transducer *t1, Transducer *t2, bool unknown_symbols_in_use) ;

  std::vector<Transducer*> SfstTransducer::extract_paths(Transducer *t)
  { vector<Transducer*> paths;
    //fprintf(stderr, "enumerating paths from transducer:\n");
    //std::cerr << *t;
    t->enumerate_paths(paths);
    //fprintf(stderr, "paths enumerated\n");
    // paths contains vectors whose alphabet does not have special symbols
    Transducer *foo = define_transducer("@_EPSILON_SYMBOL_@");
    for (unsigned int i=0; i<(unsigned int)paths.size(); i++) {
      (paths[i])->alphabet.copy(t->alphabet);
      //fprintf(stderr, "harmonizing transducer\n");
      //std::cerr << *paths[i];  // prints symbol "\3"
      //fprintf(stderr, "according to transducer\n");
      //std::cerr << *foo;
      std::pair<Transducer*,Transducer*> harm = harmonize(paths[i],foo,false);
      //delete paths[i];
      paths[i] = harm.first;
      //delete harm.second;
    }
    delete foo;
    return paths;
  }

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
    for (SFST::Alphabet::CharMap::const_iterator it 
	   = cm.begin(); it != cm.end(); it++) {
      if(FdOperation::is_diacritic(it->second))
        table->define_diacritic(it->first, it->second);
    }
    return table;
  }

  StringSet SfstTransducer::get_alphabet(Transducer * t)
  {
    StringSet s;
    SFST::Alphabet::CharMap cm = t->alphabet.get_char_map();
    for ( SFST::Alphabet::CharMap::const_iterator it = cm.begin();
	  it != cm.end(); it++ ) {
      s.insert( std::string(it->second) );
    }
    return s;
  }

  StringPairSet SfstTransducer::get_symbol_pairs(Transducer *t)
  {
    StringPairSet s;
    t->alphabet.clear_char_pairs();
    t->complete_alphabet();
    for (SFST::Alphabet::const_iterator it = t->alphabet.begin();
	 it != t->alphabet.end(); it++)
      {
	const char * isymbol = t->alphabet.code2symbol(it->lower_char());
	const char * osymbol = t->alphabet.code2symbol(it->upper_char());

	if (isymbol == NULL) {
	  fprintf(stderr, "ERROR: input number %i not found\n", it->lower_char());
	  exit(1);
	}
	if (osymbol == NULL) {
	  fprintf(stderr, "ERROR: input number %i not found\n", it->upper_char());
	  exit(1);
	}

	s.insert(StringPair(std::string(isymbol),
			    std::string(osymbol)
			    ));
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

#else
using namespace hfst::implementations;
#include <iostream>

int main(int argc, char * argv[]) 
{
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
}
#endif


