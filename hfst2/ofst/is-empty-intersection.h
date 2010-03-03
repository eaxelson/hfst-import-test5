#include "fst/lib/fstlib.h"
#include "../sfst/alphabet.h"

namespace HWFST {
  typedef fst::StdArc::StateId StateId;
  bool is_empty_intersection( fst::StdVectorFst *t1, fst::StdVectorFst *t2 );
}
