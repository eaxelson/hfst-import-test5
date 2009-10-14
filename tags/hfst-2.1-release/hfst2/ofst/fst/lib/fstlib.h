// fstlib.h

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: riley@google.com (Michael Riley)
//
// \page FstLib FST - Weighted Finite State Transducers
// This is a library for constructing, combining, optimizing, and
// searching "weighted finite-state transducers" (FSTs). Weighted
// finite-state transducers are automata where each transition has an
// input label, an output label, and a weight. The more familiar
// finite-state acceptor is represented as a transducer with each
// transition's input and output the same.  Finite-state acceptors
// are used to represent sets of strings (specifically, "regular" or
// "rational sets"); finite-state transducers are used to represent
// binary relations between pairs of strings (specifically, "rational
// transductions"). The weights can be used to represent the cost of
// taking a particular transition.
//
// In this library, the transducers are templated on the Arc
// (transition) definition, which allows changing the label, weight,
// and state ID sets. Labels and state IDs are restricted to signed
// integral types but the weight can be an arbitrary type whose
// members satisfy certain algebraic ("semiring") properties.
//
// For more information, see the FST Library Wiki page:
// http://wiki.corp.google.com/twiki/bin/view/Main/FstLibrary

// \file
// This convenience file includes all other FST inl.h files.
//

#ifndef FST_LIB_FSTLIB_H__
#define FST_LIB_FSTLIB_H__

// Abstract FST classes
#include "fst.h"
#include "expanded-fst.h"
#include "mutable-fst.h"

// Concrete FST classes
#include "vector-fst.h"
#include "const-fst.h"

// FST algorithms and delayed FST classes
#include "arcsort.h"
#include "closure.h"
#include "compose.h"
#include "concat.h"
#include "connect.h"
#include "determinize.h"
#include "difference.h"
#include "encode.h"
#include "epsnormalize.h"
#include "equal.h"
#include "equivalent.h"
#include "factor-weight.h"
#include "intersect.h"
#include "invert.h"
#include "map.h"
#include "minimize.h"
#include "project.h"
#include "prune.h"
#include "push.h"
#include "randequivalent.h"
#include "randgen.h"
#include "relabel.h"
#include "replace.h"
#include "reverse.h"
#include "reweight.h"
#include "rmepsilon.h"
#include "rmfinalepsilon.h"
#include "shortest-distance.h"
#include "shortest-path.h"
#include "synchronize.h"
#include "topsort.h"
#include "union.h"
#include "verify.h"
#include "visit.h"
// -- by Erik Axelson
#include "info-main.h"

#endif  // FST_LIB_FSTLIB_H__
