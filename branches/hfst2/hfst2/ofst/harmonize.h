#include "../sfst/alphabet.h"
#include "fst/lib/fstlib.h"


namespace HWFST {
  fst::StdVectorFst *harmonize_sfst( fst::StdVectorFst *t, Alphabet &tr_alpha,
				     Alphabet &alpha );
  fst::StdVectorFst *harmonize( fst::StdVectorFst *t, KeyTable *table_t,
				KeyTable *T, Alphabet& alpha );
}

