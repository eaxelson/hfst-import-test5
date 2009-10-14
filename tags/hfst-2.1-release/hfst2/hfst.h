#include "sfst/alphabet.h"
#include "sfst/fst.h"

#ifndef GUARD_HFST_h
#define GUARD_HFST_h

#include "hfst_defs_public.h"

DECLARE_HANDLE_TYPE(Transducer);

class Node;
union State {
  Node *node;   // SFST
  int state_id; // OpenFst
};


/** \mainpage Helsinki Finite-State Transducer Technology (HFST) interface v 2.0

HFST &mdash; The Helsinki Finite-State Transducer technology is intended for creating and manipulating weighted or unweighted synchronic transducers implementing regular relations.

Currently HFST has been implemented using the SFST and OpenFst software libraries. Other versions may be added in some future release.
The SFST implementation is unweighted and the OpenFst implementation weighted.
More info on SFST tools is in <A HREF="http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html">http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html</A> and on OpenFst in <A HREF="http://www.openfst.org">http://www.openfst.org</A>.

The SFST implementation is in namespace HFST and the OpenFst implementation in namespace HWFST. As they behave almost similarily, only one documentation is given. There are some differences (mainly concerning weight handling) which are mentioned in the documentation.

HFST and HWFST functions and datatypes are divided to five
<A HREF="modules.html">modules</A> or layers:
\link KeyLayer Key layer \endlink, 
\link TransducerLayer Transducer layer \endlink, 
\link SymbolLayer Symbol layer \endlink, 
\link RuleLayer Rule layer \endlink and
\link CoreExtensionLayer Core extension layer \endlink.

Unless otherwise specified:

-  the functions delete their transducer argument(s)
-  the functions do not change their other arguments
 
The examples use Xerox transducer notations ( http://www.xrce.xerox.com/Research-Development/Publications/1997-005/(language) )
with the following added notation:

- <tt>[.]</tt> &nbsp; The wildcard symbol. Allows any single key pair defined in the KeyPairSet argument of a function.


<!-- General information on functions and documentation notations is in
<A HREF="namespaceHFST.html#_details">HFST namespace documentation</A>. -->

*/

/** \brief Namespace for HFST functions. 

Functions in the HFST namespace are implemented with SFST. */
namespace HFST {}


/** \brief Namespace for HWFST functions. 

Functions in the HWFST namespace are implemented with OpenFst. */
namespace HWFST {}



#ifndef SKIP_HFST_DOCUMENTATION

class Arc;
class ArcsIter;

namespace HFST {
  struct range_t {
    Character character;
    struct range_t *next;
  };
  typedef range_t Range;
  typedef Range Range;

  void delete_range(Range *t);
  
  struct ranges_t {
    Range  *range;
    struct ranges_t *next;
  };   
  typedef ranges_t Ranges;
  typedef Ranges Ranges;

  void delete_ranges(Ranges *t);

  typedef State State;
  typedef Arc Transition;
  typedef ArcsIter TransitionIterator;
  
#include "key-layer.h"
#include "transducer-layer.h"
#include "rule-layer.h"
#include "symbol-layer.h"
#include "range-functions.h"
#include "core-extension-layer.h"

  void print_properties( TransducerHandle t, ostream &os );
  void print_key_table(KeyTable *T);
}

#endif


#ifndef SKIP_HWFST_DOCUMENTATION

#include "ofst/fst/lib/fst.h"

namespace fst { 
  //struct StdArc; // comment this when moving to OpenFst version 1.1
  template <class F> class ArcIterator;
  template <class A> class Fst;
  typedef Fst<StdArc> StdFst;
}

namespace HWFST {
  typedef HFST::Range Range;
  typedef HFST::Ranges Ranges;

  typedef State State;
  typedef fst::StdArc Transition;
  typedef fst::ArcIterator<fst::StdFst> TransitionIterator;

#include "key-layer.h"
#include "transducer-layer.h"
#include "rule-layer.h"
#include "symbol-layer.h"
#include "range-functions.h"  
#include "core-extension-layer.h"

  void print_properties( TransducerHandle t, ostream &os );
  void print_key_table(KeyTable *T);
  void set_ofst_symbol_table( TransducerHandle t, KeyTable *T, bool input=true );
  KeyTable *to_hwfst_compatible_format(TransducerHandle t);
  bool has_ofst_input_symbol_table(TransducerHandle t);
}

#endif


#endif /* GUARD_HFST_h */

