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

/**

   This file contains functions that are needed by the SFST programming
   language parser defined in the file 'hfst-compiler.yy'. The parser is
   used by the command line program 'hfst-calculate'. 

   This file is based on SFST's file 'interface.C'. Some functions are
   copied as such and some are less or more modified so that they work
   properly with the HFST interface.

 **/

#include "HfstCompiler.h"

namespace hfst
{
 
  //using namespace implementations;

  HfstTransducer * HfstCompiler::make_transducer(Range *r1, Range *r2, ImplementationType type)
  {

    HfstTransducer * t = new HfstTransducer(type);

    /*
    if (type == SFST_TYPE && SFST_IN_USE) {
       t->implementation.sfst = sfst_interface->make_transducer(r1,r2);
       return t;
    }
    */

    if (r1 == NULL || r2 == NULL) {
      if (!Alphabet_Defined)
	printf("ERROR: The wildcard symbol '.' requires the definition of an alphabet");
      
      // one of the ranges was '.'
      for(HfstAlphabet::const_iterator it=TheAlphabet.begin(); 
	  it!=TheAlphabet.end(); it++) {
	if ((r1 == NULL || in_range(it->first, r1)) &&
	    (r2 == NULL || in_range(it->second, r2))) {
	  HfstTransducer tr(
			    TheAlphabet.code2symbol(it->first),
			    TheAlphabet.code2symbol(it->second),
			    type);
	  t->disjunct(tr);
	}
      }      
    }
    else {
      for (;;) {
	HfstTransducer tr(
			  TheAlphabet.code2symbol(r1->character),
			  TheAlphabet.code2symbol(r2->character),
			  type);
	t->disjunct(tr);
	if (!r1->next && !r2->next)
	  break;
	if (r1->next)
	  r1 = r1->next;
	if (r2->next)
	  r2 = r2->next;
      }
    }

    return t;    
  }
  
  HfstTransducer * HfstCompiler::new_transducer( Range *r1, Range *r2, ImplementationType type )
  {
    HfstTransducer * t = make_transducer(r1, r2, type);
    if (r1 != r2)
      free_values(r1);
    free_values(r2);
    return t;
  }
  
  Character HfstCompiler::character_code( unsigned int uc ) {
    //if (TheAlphabet.utf8)
    return symbol_code(HfstBasic::fst_strdup(HfstUtf8::int2utf8(uc)));

    /* unsigned char *buffer=(unsigned char*)malloc(2);
       buffer[0] = (unsigned char)uc;
       buffer[1] = 0;      
       return symbol_code((char*)buffer); */
  }

  void HfstCompiler::free_values( Range *r ) {
    if (r) {
      free_values(r->next);
      delete r;
    }
  }

  void HfstCompiler::free_values( Ranges *r ) {
    if (r) {
      free_values(r->next);
      delete r;
    }
  }

  void HfstCompiler::error( const char *message ) {
    cerr << "\nError: " << message << "\naborted.\n";
    exit(1);
  }

  void HfstCompiler::error2( const char *message, char *input ) {
    cerr << "\nError: " << message << ": " << input << "\naborted.\n";
    exit(1);
  }
  
  Character HfstCompiler::symbol_code( char *symbol )
  { // In SFST programming language epsilon is denoted as "<>"
    // but in HFST as "@_EPSILON_SYMBOL_@". That is why it must be
    // treated separately here.
    if (strcmp(symbol,"<>") == 0)
      return 0;
    int c=TheAlphabet.symbol2code(symbol);
    if (c == EOF)
      c = TheAlphabet.add_symbol( symbol );
    free(symbol);
    return (Character)c;
  }
  
  unsigned int HfstCompiler::utf8toint( char *s ) { 
    return HfstUtf8::utf8toint(s);
  }

  bool HfstCompiler::in_range( unsigned int c, Range *r ) {
    while (r) {
      if (r->character == c)
	return true;
      r = r->next;
    }
    return false;
  }

  Range * HfstCompiler::add_value( Character c, Range *r) {
    Range *result=new Range;
    result->character = c;
    result->next = r;
    return result;
  }

  Range * HfstCompiler::add_values( unsigned int c1, unsigned int c2, Range *r) {
    for( unsigned int c=c2; c>=c1; c-- )
      r = add_value(character_code(c), r);
    return r;
  }

  Range * HfstCompiler::append_values( Range *r2, Range *r ) {
    if (r2 == NULL)
      return r;
    return add_value(r2->character, append_values(r2->next, r));
  }

  Ranges * HfstCompiler::add_range( Range *r, Ranges *l ) {
    Ranges *result = new Ranges;
    result->range = r;
    result->next = l;
    return result;
  }

  Range * HfstCompiler::complement_range( Range *r ) {
    vector<Character> sym;
    for( Range *p=r; p; p=p->next)
      sym.push_back( p->character );
    free_values( r );

    TheAlphabet.complement(sym);
    if (sym.size() == 0)
      error("Empty character range!");
    

    Range *result=NULL;
    for( size_t i=0; i<sym.size(); i++ ) {
      Range *tmp = new Range;
      tmp->character = sym[i];
      tmp->next = result;
      result = tmp;
    }

    return result;
  }
  
  bool HfstCompiler::def_var( char *name, HfstTransducer *t ) {
    // delete the old value of the variable
    VarMap::iterator it=VM.find(name);
    if (it != VM.end()) {
      char *n=it->first;
      HfstTransducer *v=it->second;
      VM.erase(it);
      delete v;
      free(n);
    }
    
    t = explode(t);
    t->minimize();
    
    VM[name] = t;
    //printf("def_var: defined variable \"%s\"\n", name);
    // TODO
    //return t->is_empty();
    return false;
  }

  bool HfstCompiler::def_rvar( char *name, HfstTransducer *t ) {
    if (t->is_cyclic())
      error2("cyclic transducer assigned to", name);
    return def_var( name, t );
  }
  
  HfstTransducer * HfstCompiler::var_value( char *name ) {
    VarMap::iterator it=VM.find(name);
    if (it == VM.end()) {
      printf("undefined variable %s\n", name);
      exit(1);
    }
    free(name);
    return new HfstTransducer(*(it->second));
  }

  HfstTransducer * HfstCompiler::rvar_value( char *name, ImplementationType type ) {
    if (RS.find(name) == RS.end())
      RS.insert(HfstBasic::fst_strdup(name));
    Range *r=add_value(symbol_code(name), NULL);
    return new_transducer(r,r,type); 
  }

  bool HfstCompiler::def_svar( char *name, Range *r ) {
    // delete the old value of the variable
    SVarMap::iterator it=SVM.find(name);
    if (it != SVM.end()) {
      char *n=it->first;
      Range *v=it->second;
      SVM.erase(it);
      delete v;
      free(n);
    }
    SVM[name] = r;
    return r == NULL;
  }

  Range *HfstCompiler::copy_values( const Range *r ) {
    if (r == NULL)
      return NULL;
    return add_value( r->character, copy_values(r->next));
  }

  Range *HfstCompiler::svar_value( char *name ) {
    SVarMap::iterator it=SVM.find(name);
    if (it == SVM.end())
      error2("undefined variable", name);
    free(name);
    return copy_values(it->second);
  }

  Range *HfstCompiler::rsvar_value( char *name ) {
    if (RSS.find(name) == RSS.end())
      RSS.insert(HfstBasic::fst_strdup(name));
    return add_value(symbol_code(name), NULL);
  }

  HfstTransducer * HfstCompiler::insert_freely(HfstTransducer *t, Character input, Character output) {
    t->insert_freely(hfst::StringPair(TheAlphabet.code2symbol(input), TheAlphabet.code2symbol(output)));
    return t;
  }

  HfstTransducer * HfstCompiler::substitute(HfstTransducer *t, Character old_char, Character new_char) {
    t->substitute(std::string(TheAlphabet.code2symbol(old_char)), std::string(TheAlphabet.code2symbol(new_char)));
    return t;
  }

  HfstTransducer * HfstCompiler::substitute(HfstTransducer *t, Character old_char_in, Character old_char_out,
					    Character new_char_in, Character new_char_out) {
    t->substitute( hfst::StringPair(TheAlphabet.code2symbol(old_char_in), 
				    TheAlphabet.code2symbol(old_char_out)),
		   hfst::StringPair(TheAlphabet.code2symbol(new_char_in), 
				    TheAlphabet.code2symbol(new_char_out)) );
    return t;
  }

  HfstTransducer * HfstCompiler::substitute(HfstTransducer *t, Character old_char_in, Character old_char_out,
					    HfstTransducer *tr) {
    t->substitute( hfst::StringPair(TheAlphabet.code2symbol(old_char_in),
				    TheAlphabet.code2symbol(old_char_out)), *tr );
    return t;
  }

  Contexts *HfstCompiler::make_context( HfstTransducer *l, HfstTransducer *r )
  {
    if (l != NULL && r != NULL) {
      if (l->get_type() != r->get_type()) {
	printf("ERROR: in hfst-compiler.yy: context transducers do not have the same type.\n");
	exit(1);
      }
    }

    ImplementationType type;
    if (l != NULL)
      type = l->get_type();
    else
      type = r->get_type();

    if (l == NULL)
      l = new HfstTransducer("@_EPSILON_SYMBOL_@",type);
    if (r == NULL)
      r = new HfstTransducer("@_EPSILON_SYMBOL_@",type);
    
    Contexts *c=new Contexts();
    c->left = l;
    c->right = r;
    c->next = NULL;
    
    return c;
  }

  Contexts *HfstCompiler::add_context( Contexts *nc, Contexts *c )    
  {
    if (nc->left->get_type() != c->left->get_type() || 
	nc->right->get_type() != c->right->get_type() ) {
      printf("ERROR: in hfst-compiler.yy: context transducers do not have the same type.\n");
      exit(1);
    }
    nc->next = c;
    return nc;
  }

  void HfstCompiler::warn(const char *msg) {
    cerr << "\nWarning: " << msg << "!\n";
  }

  HfstTransducer * HfstCompiler::negation( HfstTransducer *t )    
  {
    if (RS.size() > 0 || RSS.size() > 0)
      warn("agreement operation inside of negation");
    if (!Alphabet_Defined)
      error("Negation requires the definition of an alphabet");

    // go through all symbol pairs in TheAlphabet and copy them to sps
    StringPairSet sps;
    for( HfstAlphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++ ) {
      HfstAlphabet::NumberPair l=*it;
      sps.insert(StringPair( TheAlphabet.code2symbol(l.first),
			     TheAlphabet.code2symbol(l.second)) );
    }
    // construct a universal language transducer
    HfstTransducer * pi_star = new HfstTransducer(sps, t->get_type());
    pi_star->repeat_star();
    pi_star->subtract(*t);
    delete t;
    return pi_star;
  }


  HfstTransducer * HfstCompiler::explode( HfstTransducer *t ) {

    //printf("explode...\n");
    if (RS.size() == 0 && RSS.size() == 0) {
      //printf("... no need to explode\n");
      return t;
    }
    
    t->minimize();

    // Make a tokenizer that recognizes all multicharacter symbols in t.
    // It is needed when weighted paths are transformed into transducers.
    HfstTokenizer TOK = t->create_tokenizer();

    // transducer agreement variable names
    vector<char*> name;
    for( RVarSet::iterator it=RS.begin(); it!=RS.end(); it++) {
      name.push_back(*it);
    }
    RS.clear();
    
    // replace all agreement variables
    for( size_t i=0; i<name.size(); i++ ) {
      //printf("substituting transducer agreement variable \"%s\"\n", name[i]);
      HfstTransducer *nt = new HfstTransducer(t->get_type()); // an initially empty transducer
      
      // enumerate all paths of the transducer
      HfstTransducer *vt=var_value(strdup(name[i])); // var_value frees its argument
      WeightedPaths<float>::Set paths;
      vt->extract_strings(paths, -1, -1);
      delete vt;

      // transform weighted paths to a vector of transducers
      vector<HfstTransducer*> transducer_paths;
      for (WeightedPaths<float>::Set::iterator it = paths.begin(); it != paths.end(); it++) {
	WeightedPath<float> wp = *it;
	HfstTransducer * path = new HfstTransducer(wp.istring, wp.ostring, TOK, t->get_type());
	path->set_final_weights(wp.weight);
	transducer_paths.push_back(path);
      }
      
      // insert each path
      for( size_t j=0; j<transducer_paths.size(); j++ ) {
	//printf("  substituting transducer agreement variable \"%s\" with transducer:\n", name[i]);
	//cerr << *(transducer_paths[j]);
	HfstTransducer ti(*t);
	//printf("in transducer:\n");
	//cerr << ti;
	ti.substitute(StringPair(std::string(name[i]), std::string(name[i])), *(transducer_paths[j]));
	//printf("  ...substituted\n");
	delete transducer_paths[j];	
	nt->disjunct(ti);
      }
      free(name[i]); // svar_value was given a copy of name[i], so the value is freed here

      delete t;
      t = nt;
    }
    
    name.clear();
    for( RVarSet::iterator it=RSS.begin(); it!=RSS.end(); it++)
      name.push_back(*it);
    RSS.clear();
    
    // replace all agreement variables
    for( size_t i=0; i<name.size(); i++ ) {
      //printf("substituting range agreement variable \"%s\"\n", name[i]);
      HfstTransducer *nt = new HfstTransducer(t->get_type()); 
      Range *r=svar_value(strdup(name[i]));  // svar_value frees its argument
      
      // insert each character
      while (r != NULL) {
	
	// insertion
	HfstTransducer ti(*t);
	// agreement variable marker should always appear on both sides of the tape..
	//printf("substituting agreement range variable %s with %s\n", name[i], TheAlphabet.code2symbol(r->character));
	ti.substitute(std::string(name[i]), TheAlphabet.code2symbol(r->character));	
	nt->disjunct(ti);
	
	Range *next = r->next;
	delete r;
	r = next;
      }
      free(name[i]);  // svar_value was given a copy of name[i], so the value is freed here
      delete t;
      t = nt;
    }    
    return t;
  }

  // TODO
  HfstTransducer * HfstCompiler::restriction( HfstTransducer * t, Twol_Type type, Contexts *c, int direction ) {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer * HfstCompiler::make_rule( HfstTransducer * lc, Range * lower_range, Twol_Type type, 
					    Range * upper_range, HfstTransducer * rc ) {
    if (RS.size() > 0 || RSS.size() > 0)
      cerr << "\nWarning: agreement operation inside of replacement rule!\n";
    
    if (!Alphabet_Defined)
      cerr << "\nERROR: Two level rules require the definition of an alphabet!\n";

    HfstTransducerPair tr_pair(*(lc), *(rc));
    StringPairSet sps;
    for( HfstAlphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++ ) {
      HfstAlphabet::NumberPair l=*it;
      sps.insert(StringPair( TheAlphabet.code2symbol(l.first),
			     TheAlphabet.code2symbol(l.second)) );
    } 

    StringPairSet mappings;
    Range * r1 = lower_range;
    Range * r2 = upper_range;

    if (r1 == NULL || r2 == NULL) {
      if (!Alphabet_Defined)
	printf("ERROR: The wildcard symbol '.' requires the definition of an alphabet");
      
      // one of the ranges was '.'
      for(HfstAlphabet::const_iterator it=TheAlphabet.begin(); 
	  it!=TheAlphabet.end(); it++) {
	if ((r1 == NULL || in_range(it->first, r1)) &&
	    (r2 == NULL || in_range(it->second, r2))) {
	  mappings.insert( StringPair(
				      TheAlphabet.code2symbol(it->first),
				      TheAlphabet.code2symbol(it->second) ) );
	}
      }      
    }
    else {
      for (;;) {
	mappings.insert( StringPair(
				    TheAlphabet.code2symbol(r1->character),
				    TheAlphabet.code2symbol(r2->character) ) );
	if (!r1->next && !r2->next)
	  break;
	if (r1->next)
	  r1 = r1->next;
	if (r2->next)
	  r2 = r2->next;
      }
    }
    
    switch(type)
      {
      case twol_left:
	return new HfstTransducer(rules::two_level_if(tr_pair, mappings, sps));
	break;
      case twol_right:
	return new HfstTransducer(rules::two_level_only_if(tr_pair, mappings, sps));
	break;
      case twol_both:
	return new HfstTransducer(rules::two_level_if_and_only_if(tr_pair, mappings, sps));
	break;
      }
    return NULL;

  }

  HfstTransducer * HfstCompiler::read_words(char *filename) {
    return NULL;  // TODO!
  }

  HfstTransducer * HfstCompiler::read_transducer(char *filename) {
    if (Verbose)
      fprintf(stderr,"\nreading transducer from %s...", filename);
    HfstInputStream is(filename);
    is.open();
    HfstTransducer *t = new HfstTransducer(is);
    is.close();
    free(filename);
    if (Verbose)
      fprintf(stderr,"finished\n");
    return t;
  }

  void HfstCompiler::write_to_file(HfstTransducer *t, char* filename) {
    HfstOutputStream os(std::string(filename), t->get_type());
    os.open();
    os << *t;
    os.close();
    return;
  }

  HfstTransducer * HfstCompiler::replace_in_context(HfstTransducer * mapping, Repl_Type repl_type, Contexts *contexts, bool optional) {
    
    HfstTransducerPair tr_pair(*(contexts->left), *(contexts->right));
    StringPairSet sps;
    //printf("inserting pairs:\n");
    for( HfstAlphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++ ) {
      HfstAlphabet::NumberPair l=*it;
      //printf("inserting pair %i:%i... ", l.lower_char(), l.upper_char());
      sps.insert(StringPair( TheAlphabet.code2symbol(l.first),
			     TheAlphabet.code2symbol(l.second)) );
      //printf("ok\n");
    } 
    switch (repl_type) 
      {
      case repl_up:
	return new HfstTransducer(rules::replace_up(tr_pair, *mapping, optional, sps));
	break;
      case repl_down:
	return new HfstTransducer(rules::replace_down(tr_pair, *mapping, optional, sps));
	break;
      case repl_left:
	return new HfstTransducer(rules::replace_left(tr_pair, *mapping, optional, sps));
	break;
      case repl_right:
	return new HfstTransducer(rules::replace_right(tr_pair, *mapping, optional, sps));
	break;
      }
    return NULL;
  }

  HfstTransducer * HfstCompiler::replace(HfstTransducer * mapping, Repl_Type repl_type, bool optional) {
    
    StringPairSet sps;
    for( HfstAlphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++ ) {
      HfstAlphabet::NumberPair l=*it;
      sps.insert(StringPair( TheAlphabet.code2symbol(l.first),
			     TheAlphabet.code2symbol(l.second)) );
    } 
    switch (repl_type) 
      {
      case repl_up:
	return new HfstTransducer(rules::replace_up(*mapping, optional, sps));
	break;
      case repl_down:
	return new HfstTransducer(rules::replace_down(*mapping, optional, sps));
	break;
      default:
	return NULL;
      }
  }

  HfstTransducer * HfstCompiler::make_mapping( Ranges *list1, Ranges *list2, ImplementationType type ) {

    //Transducer *Interface::make_mapping( Ranges *list1, Ranges *list2 )

    Ranges *l1=list1;
    Ranges *l2=list2;
    HfstTransducer *t=new HfstTransducer(0, type); // an epsilon transducer

    //Node *node=t->root_node();
    while (l1 && l2) {
      //Node *nn=t->new_node();
      HfstTransducer disj(type); // an empty transducer
      for( Range *r1=l1->range; r1; r1=r1->next )
	for( Range *r2=l2->range; r2; r2=r2->next ) {
	  //node->add_arc( Label(r1->character, r2->character), nn, t );
	  HfstTransducer tr(TheAlphabet.code2symbol(r1->character), 
			    TheAlphabet.code2symbol(r2->character),
			    type);
	  disj.disjunct(tr);
	}
      //node = nn;
      t->concatenate(disj);
      l1 = l1->next;
      l2 = l2->next;
    }
    while (l1) {
      //Node *nn=t->new_node();
      HfstTransducer disj(type); // an empty transducer
      for( Range *r1=l1->range; r1; r1=r1->next ) {
	//node->add_arc( Label(r1->character, Label::epsilon), nn, t );
	  HfstTransducer tr(TheAlphabet.code2symbol(r1->character), 
			    TheAlphabet.code2symbol(0),
			    type);
	  disj.disjunct(tr);
      }
      //node = nn;
      t->concatenate(disj);
      l1 = l1->next;
    }
    while (l2) {
      //Node *nn=t->new_node();
      HfstTransducer disj(type); // an empty transducer
      for( Range *r2=l2->range; r2; r2=r2->next ) {
	//node->add_arc( Label(Label::epsilon, r2->character), nn, t );
	  HfstTransducer tr(TheAlphabet.code2symbol(0), 
			    TheAlphabet.code2symbol(r2->character),
			    type);
	  disj.disjunct(tr);
      }
      //node = nn;
      t->concatenate(disj);
      l2 = l2->next;
    }
    //node->set_final(1);

    free_values(list1);
    free_values(list2);
    
    t->minimize();

    return t;
  }

  
  HfstTransducer * HfstCompiler::result( HfstTransducer *t, bool switch_flag) {

    t = explode(t);

    // delete the variable values
    vector<char*> s;
    for( VarMap::iterator it=VM.begin(); it != VM.end(); it++ ) {
      s.push_back(it->first);
      delete it->second;
      it->second = NULL;
    }
    VM.clear();
    for( size_t i=0; i<s.size(); i++ )
      free(s[i]);
    s.clear();
    
    if (switch_flag)
      t->invert();
    //add_alphabet(t);
    t->minimize();

    // testing conversion
    /*printf("testing_conversion:\n");
    HfstInternalTransducer *internal = tropical_ofst_to_internal_hfst_format(t->implementation.tropical_ofst);
    printf("  ..TROPICAL -> INTERNAL\n");
    t->implementation.sfst = hfst_internal_format_to_sfst(internal);
    printf("  ..INTERNAL -> SFST\n");
    delete internal;
    internal = sfst_to_internal_hfst_format(t->implementation.sfst);
    printf("  ..SFST -> INTERNAL\n");
    t->implementation.foma = hfst_internal_format_to_foma(internal);
    printf("  ..INTERNAL -> FOMA\n");
    delete internal;
    internal = foma_to_internal_hfst_format(t->implementation.foma);
    printf("  ..FOMA -> INTERNAL\n");
    t->implementation.tropical_ofst = hfst_internal_format_to_tropical_ofst(internal);
    printf("  ..INTERNAL -> TROPICAL\n");
    delete internal;*/

    /*printf("testing write_att and read_att:\n");
    std::cerr << *t;
    printf("--\n");
    t->write_in_att_format("FOOBAR");
    HfstTransducer tr = HfstTransducer::read_in_att_format("FOOBAR", t->type);
    tr.write_in_att_format(stderr);*/

    return t;
  }
  
  void HfstCompiler::def_alphabet( HfstTransducer *tr )
  {
    tr = explode(tr);
    tr->minimize();

    TheAlphabet.clear_pairs();
    //TheAlphabet.copy(t->alphabet);

    HfstMutableTransducer * t  = HfstTransducer::hfst_transducer_to_internal(tr);
    
    HfstStateIterator state_it(*t);
    while (not state_it.done()) 
      {
	unsigned int s = state_it.value();
	HfstTransitionIterator transition_it(*t,s);
	while (not transition_it.done()) 
	  {
	    HfstTransition tr = transition_it.value();
	    TheAlphabet.insert(HfstAlphabet::NumberPair(TheAlphabet.symbol2code(tr.isymbol.c_str()),
							TheAlphabet.symbol2code(tr.osymbol.c_str())));
	    transition_it.next();
	  }
	state_it.next();
      }
      Alphabet_Defined = 1;
  }

  namespace HfstUtf8 {

  /*******************************************************************/
  /*                                                                 */
  /*  int2utf8                                                       */
  /*                                                                 */
  /*******************************************************************/

  char *int2utf8( unsigned int sym )

  {
    static unsigned char ch[5];

    if (sym < 128) {
      // 1-byte UTF8 symbol, 7 bits
      ch[0] = (unsigned char)sym;
      ch[1] = 0;
    }
  
    else if (sym < 2048) {
      // 2-byte UTF8 symbol, 5+6 bits
      ch[0] = (unsigned char)((sym >> 6) | set2MSbits);
      ch[1] = (unsigned char)((sym & get6LSbits) | set1MSbits);
      ch[2] = 0;
    }
  
    else if (sym < 65536) {
      // 3-byte UTF8 symbol, 4+6+6 bits
      ch[0] = (unsigned char)((sym >> 12) | set3MSbits);
      ch[1] = (unsigned char)(((sym >> 6) & get6LSbits) | set1MSbits);
      ch[2] = (unsigned char)((sym & get6LSbits) | set1MSbits);
      ch[3] = 0;
    }
  
    else if (sym < 2097152) {
      // 4-byte UTF8 symbol, 3+6+6+6 bits
      ch[0] = (unsigned char)((sym >> 18) | set4MSbits);
      ch[1] = (unsigned char)(((sym >> 12) & get6LSbits) | set1MSbits);
      ch[2] = (unsigned char)(((sym >> 6) & get6LSbits) | set1MSbits);
      ch[3] = (unsigned char)((sym & get6LSbits) | set1MSbits);
      ch[4] = 0;
    }
  
    else
      return NULL;

    return (char*)ch;
  }


  /*******************************************************************/
  /*                                                                 */
  /*  utf8toint                                                      */
  /*                                                                 */
  /*******************************************************************/

  unsigned int utf8toint( char **s )

  {
    int bytes_to_come;
    unsigned int result=0;
    unsigned char c=(unsigned char)**s;

    if (c >= (unsigned char)set4MSbits) { // 1111xxxx
      bytes_to_come = 3;
      result = (result << 3) | (c & get3LSbits);
    }
      
    else if (c >= (unsigned char) set3MSbits) { // 1110xxxx
      // start of a three-byte symbol
      bytes_to_come = 2;
      result = (result << 4) | (c & get4LSbits);
    }
      
    else if (c >= (unsigned char) set2MSbits) { // 1100xxxx
      // start of a two-byte symbol
      bytes_to_come = 1;
      result = (result << 5) | (c & get5LSbits);
    }
      
    else if (c < (unsigned char) set1MSbits) { // 0100xxxx
      // one-byte symbol
      bytes_to_come = 0;
      result = c;
    }

    else
      return 0; // error

    while (bytes_to_come > 0) {
      bytes_to_come--;
      (*s)++;
      c = (unsigned char)**s;
      if (c < (unsigned char) set2MSbits &&
	  c >= (unsigned char) set1MSbits)    // 1000xxxx
	{
	  result = (result << 6) | (c & get6LSbits);
	}
      else
	return 0;
    }

    (*s)++;
    return result;
  }

  /*******************************************************************/
  /*                                                                 */
  /*  utf8toint                                                      */
  /*                                                                 */
  /*******************************************************************/

  unsigned int utf8toint( char *s )

  {
    unsigned int result = utf8toint( &s );
    if (*s == 0) // all bytes converted?
      return result;
    return 0;
  }

  }

    namespace HfstBasic {

  /*******************************************************************/
  /*                                                                 */
  /*  fst_strdup                                                     */
  /*                                                                 */
  /*******************************************************************/

  char* fst_strdup(const char* pString)

  {
    char* pStringCopy = (char*)malloc(strlen(pString) + 1);
    if (pStringCopy == NULL) {
      fprintf(stderr, "\nError: out of memory (malloc failed)\naborted.\n");
      exit(1);
    }
    strcpy(pStringCopy, pString);
    return pStringCopy;
  }


  /*******************************************************************/
  /*                                                                 */
  /*  read_string                                                    */
  /*                                                                 */
  /*******************************************************************/

  int read_string( char *buffer, int size, FILE *file )

  {
    for( int i=0; i<size; i++ ) {
      int c=fgetc(file);
      if (c == EOF || c == 0) {
	buffer[i] = 0;
	return (c==0);
      }
      buffer[i] = (char)c;
    }
    buffer[size-1] = 0;
    return 0;
  }


  /*******************************************************************/
  /*                                                                 */
  /*  read_num                                                       */
  /*                                                                 */
  /*******************************************************************/

  size_t read_num( void *p, size_t n, FILE *file )

  {
    char *pp=(char*)p;
    size_t result=fread( pp, 1, n, file );
    //if (Switch_Bytes) {
    if (false) {
      size_t e=n/2;
      for( size_t i=0; i<e; i++ ) {
	char tmp=pp[i];
	pp[i] = pp[--n];
	pp[n] = tmp;
      }
    }
    return result;
  }  
    }

}
