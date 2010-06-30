#include "HfstCompiler.h"

namespace hfst
{
 
  HfstCompiler::HfstCompiler() {}
  
  HfstTransducer * HfstCompiler::make_transducer(Range *r1, Range *r2, ImplementationType type)
  {
    return new HfstTransducer(type);
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

  HfstTransducer * HfstCompiler::make_mapping( Ranges *r1, Ranges *r2, ImplementationType type )
  {
    return new HfstTransducer(type);
  }
  
  HfstTransducer * HfstCompiler::result( HfstTransducer *t, bool)
  {
    return t;
  }
  
  void HfstCompiler::def_alphabet( HfstTransducer *t )
  {
    t->convert(SFST_TYPE);
    SFST::def_alphabet(t->implementation.sfst);      // def_alphabet deletes its argument
    t->implementation.sfst = new SFST::Transducer(); // double delete is avoided here
    delete t;
    return;
  }
  
}
