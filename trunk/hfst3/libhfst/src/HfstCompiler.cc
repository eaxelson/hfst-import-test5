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
  { return SFST::symbol_code(s); }
  
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

  HfstTransducer * HfstCompiler::make_mapping( Ranges *r1, Ranges *r2, ImplementationType type )
  {
    return new HfstTransducer(type);
  }
  
  HfstTransducer * HfstCompiler::result( HfstTransducer *t, bool)
  {
    t->minimize(); 
    return t;
  }
  
  void HfstCompiler::def_alphabet( HfstTransducer *t )
  {
    t->convert(SFST_TYPE);
    cerr << *t;
    SFST::def_alphabet(t->implementation.sfst);      // def_alphabet deletes its argument
    t->implementation.sfst = new SFST::Transducer(); // double delete is avoided here
    delete t;
    return;
  }
  
}
