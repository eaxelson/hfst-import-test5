#pragma once

#include <cstdlib>
#include <vector>
#include <set>
#include "fst/lib/fstlib.h"
#include "../sfst/alphabet.h"
#include "../sfst/utf8.h"

namespace HWFST {

using std::vector;
using std::set;

typedef fst::StdArc Arc;
typedef fst::StdVectorFst Transducer;
typedef Arc::StateId StateId;
typedef fst::ArcIterator<Transducer> ArcIterator;
typedef fst::StateIterator<Transducer> StateIterator;
typedef fst::TropicalWeight Weight;

typedef vector<Key> KeyVector;
typedef vector<KeyVector*> KeyVectorVector;

struct label_cmp{
  bool operator() (const Label &l1, const Label &l2) const {
    if ( l1.lower_char() == l2.lower_char() )
      return l1.upper_char() < l2.upper_char();
    return l1.lower_char() < l2.lower_char();
  }
};

typedef set<Label,label_cmp> LabelSet;
 
};
