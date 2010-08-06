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
 
  HfstCompiler::HfstCompiler() {}
  
  HfstTransducer * HfstCompiler::make_transducer(Range *r1, Range *r2, ImplementationType type)
  {
    HfstTransducer * t = new HfstTransducer(type);

    /*if (type == SFST_TYPE) {
      t->implementation.sfst = SFST::make_transducer(r1,r2);
      cerr << *(t->implementation.sfst);
      return t;
      }*/

    if (r1 == NULL || r2 == NULL) {
      if (!SFST::Alphabet_Defined)
	printf("ERROR: The wildcard symbol '.' requires the definition of an alphabet");
      
      // one of the ranges was '.'
      for(SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); 
	  it!=SFST::TheAlphabet.end(); it++) {
	if ((r1 == NULL || SFST::in_range(it->lower_char(), r1)) &&
	    (r2 == NULL || SFST::in_range(it->upper_char(), r2))) {
	  HfstTransducer tr(
			    SFST::TheAlphabet.code2symbol(it->lower_char()),
			    SFST::TheAlphabet.code2symbol(it->upper_char()),
			    type);
	  t->disjunct(tr);
	}
      }      
    }
    else {
      for (;;) {
	HfstTransducer tr(
			  SFST::TheAlphabet.code2symbol(r1->character),
			  SFST::TheAlphabet.code2symbol(r2->character),
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
      SFST::free_values(r1);
    SFST::free_values(r2);
    return t;
  }
  
  HfstCompiler::Character HfstCompiler::character_code( unsigned int uc )
  { return SFST::character_code(uc); }
  
  HfstCompiler::Character HfstCompiler::symbol_code( char *s )
  { // In SFST programming language epsilon is denoted as "<>"
    // but in HFST as "@_EPSILON_SYMBOL_@". That is why it must be
    // treated separately here.
    if (strcmp(s,"<>") == 0)
      return 0;
    return SFST::symbol_code(s); }
  
  unsigned int HfstCompiler::utf8toint( char *s )
  { return SFST::utf8toint(s); }
    
  HfstCompiler::Range *HfstCompiler::add_value( Character c, Range *r) {
    return SFST::add_value(c,r); }

  HfstCompiler::Range *HfstCompiler::add_values( unsigned int i, unsigned int j, Range *r) {
    return SFST::add_values(i,j,r); }

  HfstCompiler::Range *HfstCompiler::append_values( Range *r1, Range *r2 ) {
    return SFST::append_values(r1,r2); }

  HfstCompiler::Ranges *HfstCompiler::add_range( Range *r, Ranges *l ) {
    return SFST::add_range(r,l); }

  HfstCompiler::Range *HfstCompiler::complement_range( Range *r ) {
    return SFST::complement_range(r);
  }
  
  bool HfstCompiler::def_svar( char *name, Range *r ) {
    return SFST::def_svar(name,r);
  }

  HfstCompiler::Range *HfstCompiler::svar_value( char *name ) {
    return svar_value(name);
  }

  HfstCompiler::Range *HfstCompiler::rsvar_value( char *name ) {
    return rsvar_value(name);
  }

  HfstTransducer * HfstCompiler::insert_freely(HfstTransducer *t, Character input, Character output) {
    t->insert_freely(hfst::StringPair(SFST::TheAlphabet.code2symbol(input), SFST::TheAlphabet.code2symbol(output)));
    return t;
  }

  HfstCompiler::Contexts *HfstCompiler::make_context( HfstTransducer *l, HfstTransducer *r )
  {
    if (l->get_type() != r->get_type()) {
      printf("ERROR: in hfst-compiler.yy: context transducers do not have the same type.\n");
      exit(1);
    }
    ImplementationType type = l->get_type();

    if (l == NULL)
      l = new HfstTransducer(type);
    if (r == NULL)
      r = new HfstTransducer(type);
    
    HfstCompiler::Contexts *c=new HfstCompiler::Contexts();
    c->left = l;
    c->right = r;
    c->next = NULL;
    
    return c;
  }

  HfstCompiler::Contexts *HfstCompiler::add_context( HfstCompiler::Contexts *nc, HfstCompiler::Contexts *c )    
  {
    if (nc->left->get_type() != c->left->get_type() || 
	nc->right->get_type() != c->right->get_type() ) {
      printf("ERROR: in hfst-compiler.yy: context transducers do not have the same type.\n");
      exit(1);
    }
    nc->next = c;
    return nc;
  }

  HfstTransducer * HfstCompiler::negation( HfstTransducer *t )    
  {
    //if (RS.size() > 0 || RSS.size() > 0)
    //  cerr << "\nWarning: agreement operation inside of negation!\n";
    //if (!SFST::Alphabet_Defined)
    //  error("Negation requires the definition of an alphabet");

    // go through all symbol pairs in TheAlphabet and copy them to sps
    StringPairSet sps;
    for( SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); it!=SFST::TheAlphabet.end(); it++ ) {
      SFST::Label l=*it;
      sps.insert(StringPair( SFST::TheAlphabet.code2symbol(l.lower_char()),
			     SFST::TheAlphabet.code2symbol(l.upper_char())) );
    }
    // construct a universal language transducer
    HfstTransducer * pi_star = new HfstTransducer(sps, t->get_type());
    pi_star->repeat_star();
    pi_star->subtract(*t);
    delete t;
    return pi_star;
  }

  // TODO
  HfstTransducer * HfstCompiler::explode_and_minimize( HfstTransducer *t ) {
    return t;
  }

  // TODO
  HfstTransducer * HfstCompiler::restriction( HfstTransducer * t, Twol_Type type, Contexts *c, int direction ) {
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer * HfstCompiler::make_rule( HfstTransducer * lc, Range * lower_range, Twol_Type type, 
					    Range * upper_range, HfstTransducer * rc ) {
    /*if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of replacement rule!\n";
    
    if (!Alphabet_Defined)
    error("Two level rules require the definition of an alphabet");  */

    HfstTransducerPair tr_pair(*(lc), *(rc));
    StringPairSet sps;
    for( SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); it!=SFST::TheAlphabet.end(); it++ ) {
      SFST::Label l=*it;
      sps.insert(StringPair( SFST::TheAlphabet.code2symbol(l.lower_char()),
			     SFST::TheAlphabet.code2symbol(l.upper_char())) );
    } 

    StringPairSet mappings;
    Range * r1 = lower_range;
    Range * r2 = upper_range;

    if (r1 == NULL || r2 == NULL) {
      if (!SFST::Alphabet_Defined)
	printf("ERROR: The wildcard symbol '.' requires the definition of an alphabet");
      
      // one of the ranges was '.'
      for(SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); 
	  it!=SFST::TheAlphabet.end(); it++) {
	if ((r1 == NULL || SFST::in_range(it->lower_char(), r1)) &&
	    (r2 == NULL || SFST::in_range(it->upper_char(), r2))) {
	  mappings.insert( StringPair(
				      SFST::TheAlphabet.code2symbol(it->lower_char()),
				      SFST::TheAlphabet.code2symbol(it->upper_char()) ) );
	}
      }      
    }
    else {
      for (;;) {
	mappings.insert( StringPair(
				    SFST::TheAlphabet.code2symbol(r1->character),
				    SFST::TheAlphabet.code2symbol(r2->character) ) );
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
      case SFST::twol_left:
	return new HfstTransducer(rules::two_level_if(tr_pair, mappings, sps));
	break;
      case SFST::twol_right:
	return new HfstTransducer(rules::two_level_only_if(tr_pair, mappings, sps));
	break;
      case SFST::twol_both:
	return new HfstTransducer(rules::two_level_if_and_only_if(tr_pair, mappings, sps));
	break;
      }
    return NULL;

  }


  HfstTransducer * HfstCompiler::replace_in_context(HfstTransducer * mapping, Repl_Type repl_type, Contexts *contexts, bool optional) {
    
    HfstTransducerPair tr_pair(*(contexts->left), *(contexts->right));
    StringPairSet sps;
    for( SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); it!=SFST::TheAlphabet.end(); it++ ) {
      SFST::Label l=*it;
      sps.insert(StringPair( SFST::TheAlphabet.code2symbol(l.lower_char()),
			     SFST::TheAlphabet.code2symbol(l.upper_char())) );
    } 
    switch (repl_type) 
      {
      case SFST::repl_up:
	return new HfstTransducer(rules::replace_up(tr_pair, *mapping, optional, sps));
	break;
      case SFST::repl_down:
	return new HfstTransducer(rules::replace_down(tr_pair, *mapping, optional, sps));
	break;
      case SFST::repl_left:
	return new HfstTransducer(rules::replace_left(tr_pair, *mapping, optional, sps));
	break;
      case SFST::repl_right:
	return new HfstTransducer(rules::replace_right(tr_pair, *mapping, optional, sps));
	break;
      }
    return NULL;
  }

  HfstTransducer * HfstCompiler::replace(HfstTransducer * mapping, Repl_Type repl_type, bool optional) {
    
    StringPairSet sps;
    for( SFST::Alphabet::const_iterator it=SFST::TheAlphabet.begin(); it!=SFST::TheAlphabet.end(); it++ ) {
      SFST::Label l=*it;
      sps.insert(StringPair( SFST::TheAlphabet.code2symbol(l.lower_char()),
			     SFST::TheAlphabet.code2symbol(l.upper_char())) );
    } 
    switch (repl_type) 
      {
      case SFST::repl_up:
	return new HfstTransducer(rules::replace_up(*mapping, optional, sps));
	break;
      case SFST::repl_down:
	return new HfstTransducer(rules::replace_down(*mapping, optional, sps));
	break;
      default:
	return NULL;
      }
  }

  HfstTransducer * HfstCompiler::make_mapping( Ranges *r1, Ranges *r2, ImplementationType type ) {
    return new HfstTransducer(type);
  }
  
  HfstTransducer * HfstCompiler::result( HfstTransducer *t, bool) {
    t->minimize(); 
    return t;
  }
  
  void HfstCompiler::def_alphabet( HfstTransducer *tr )
  {
    // explode, minimize

    SFST::TheAlphabet.clear_char_pairs();
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
	    SFST::TheAlphabet.insert(SFST::Label(SFST::TheAlphabet.symbol2code(tr.isymbol.c_str()),
						 SFST::TheAlphabet.symbol2code(tr.osymbol.c_str())));
	    transition_it.next();
	  }
	state_it.next();
      }
    SFST::Alphabet_Defined = 1;
  }

}
