//#include "fstlib.h"
#include "harmonize.h"

namespace HWFST {
  
  fst::StdVectorFst *harmonize_sfst( fst::StdVectorFst *t, Alphabet &tr_alpha,
				     Alphabet &alpha ) 
  {    
    for( int i = 0; i < t->NumStates(); ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();
	if (arc.ilabel != 0) {
	  // fprintf(stderr, "harmonizing: ");
	  const char *name = tr_alpha.code2symbol(arc.ilabel);
	  //if (name == NULL)
	  (void) alpha.add_symbol( name );
	  //fprintf(stderr, "replacing %hu with %hu\n", arc.ilabel, isymbol);
	  arc.ilabel = KeyTable::harmonize_key_sfst(arc.ilabel, tr_alpha, alpha);
	}
	if (arc.olabel != 0) {
	  //fprintf(stderr, "harmonizing: ");
	  const char *name = tr_alpha.code2symbol(arc.olabel);
	  //if (name == NULL)
	  (void) alpha.add_symbol( name );
	  //fprintf(stderr, "replacing %hu with %hu\n", arc.olabel, osymbol);
	  arc.olabel = KeyTable::harmonize_key_sfst(arc.olabel, tr_alpha, alpha);
	}
	it.SetValue(arc);
	//Label l = Label(isymbol, osymbol);
	//new_alpha.insert(l);
      }	
    } 
    return t;
  }

  
  fst::StdVectorFst *harmonize( fst::StdVectorFst *t, KeyTable *old_table,
				KeyTable *new_table, Alphabet& alpha ) 
  {    
    for( int i = 0; i < t->NumStates(); ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();
	arc.ilabel = KeyTable::harmonize_key(arc.ilabel, old_table, new_table, alpha);
	arc.olabel = KeyTable::harmonize_key(arc.olabel, old_table, new_table, alpha);
	it.SetValue(arc);
      }	
    } 
    return t;
  }    
  
}


