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
   used by the command line program hfst-calculate. 

 **/

#include "HfstCompiler.h"

namespace hfst
{
 
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
  
  Character HfstCompiler::character_code( unsigned int uc )
  { return EOF; } // COPY
  
  Character HfstCompiler::symbol_code( char *s )
  { // In SFST programming language epsilon is denoted as "<>"
    // but in HFST as "@_EPSILON_SYMBOL_@". That is why it must be
    // treated separately here.
    if (strcmp(s,"<>") == 0)
      return 0;
    return EOF; } // COPY
  
  unsigned int HfstCompiler::utf8toint( char *s )
  { return EOF; } // COPY
    
  Range * HfstCompiler::add_value( Character c, Range *r) {
    return NULL; } // COPY

  Range * HfstCompiler::add_values( unsigned int i, unsigned int j, Range *r) {
    return NULL; } // COPY

  Range * HfstCompiler::append_values( Range *r1, Range *r2 ) {
    return NULL; } // COPY

  Ranges * HfstCompiler::add_range( Range *r, Ranges *l ) {
    return NULL; } // COPY

  Range * HfstCompiler::complement_range( Range *r ) {
    return NULL;
  } // COPY
  
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
    // TODO
    //if (t->is_cyclic())
    //error2("cyclic transducer assigned to", name);
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
      RS.insert(fst_strdup(name));
    Range *r=add_value(symbol_code(name), NULL);
    return new_transducer(r,r,type); 
  }

  bool HfstCompiler::def_svar( char *name, Range *r ) {
    return false; // COPY
  }

  Range *HfstCompiler::svar_value( char *name ) {
    return NULL; // COPY
  }

  Range *HfstCompiler::rsvar_value( char *name ) {
    return NULL; // COPY
  }

  HfstTransducer * HfstCompiler::insert_freely(HfstTransducer *t, Character input, Character output) {
    t->insert_freely(hfst::StringPair(TheAlphabet.code2symbol(input), TheAlphabet.code2symbol(output)));
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
      l = new HfstTransducer(type);
    if (r == NULL)
      r = new HfstTransducer(type);
    
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
      NumberPair l=*it;
      sps.insert(StringPair( TheAlphabet.code2symbol(l->first),
			     TheAlphabet.code2symbol(l->second)) );
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
      NumberPair l=*it;
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
	if ((r1 == NULL || in_range(it->lower_char(), r1)) &&
	    (r2 == NULL || in_range(it->upper_char(), r2))) {
	  mappings.insert( StringPair(
				      TheAlphabet.code2symbol(it->lower_char()),
				      TheAlphabet.code2symbol(it->upper_char()) ) );
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

  //HfstTransducer * HfstCompiler::read_words(char *filename) {
  //}

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
      NumberPair l=*it;
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
      NumberPair l=*it;
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

  HfstTransducer * HfstCompiler::make_mapping( Ranges *r1, Ranges *r2, ImplementationType type ) {
    return new HfstTransducer(type);
  }
  
  HfstTransducer * HfstCompiler::result( HfstTransducer *t, bool switch_flag) {

    //printf("result...\n");
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
    return t;
  }
  
  void HfstCompiler::def_alphabet( HfstTransducer *tr )
  {
    // explode, minimize

    TheAlphabet.clear_char_pairs();
    //TheAlphabet.copy(t->alphabet);

    HfstMutableTransducer t(*tr);
    HfstStateIterator state_it(t);
    while (not state_it.done()) 
      {
	HfstState s = state_it.value();
	HfstTransitionIterator transition_it(t,s);
	while (not transition_it.done()) 
	  {
	    HfstTransition tr = transition_it.value();
	    TheAlphabet.insert(NumberPair(TheAlphabet.symbol2code(tr.isymbol.c_str()),
					  TheAlphabet.symbol2code(tr.osymbol.c_str())));
	    transition_it.next();
	  }
	state_it.next();
      }
    Alphabet_Defined = 1;

    //printf("TheAlphabet is now defined as:\n");
    //for( SFST::Alphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++ ) {
    //  SFST::Label l=*it;
    //  printf("  %i:%i\n", l.lower_char(), l.upper_char());
    //}
  }

}
