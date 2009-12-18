#ifndef _CONVERT_TRANSDUCER_H_
#define _CONVERT_TRANSDUCER_H_
#include <map>
#include <iostream>
#include <vector>
#include <map>
#include "HfstExceptions.h"
#include "SFST/src/fst.h"
#include "openfst-1.1/src/include/fst/fstlib.h"



namespace HFST_IMPLEMENTATIONS {

  using namespace HFST_EXCEPTIONS;

  typedef fst::StdArc::StateId StateId;
  typedef fst::ArcIterator<fst::StdVectorFst> StdArcIterator;
  typedef fst::StdVectorFst InternalTransducer;
  typedef std::vector<SFST::Node *> SfstStateVector;
  typedef std::vector<StateId> OfstStateVector;
  typedef std::map<SFST::Node *,StateId> SfstToOfstStateMap;
  typedef std::map<StateId,SFST::Node *> OfstToSfstStateMap;


    /* SFST::Transducer * is the sfst transducer format.
       fst::StdVectorFst * is the openfst transducer format. */
    
    /* Read an SFST::Transducer * and return the equivalent transducer in
       internal format. */
    InternalTransducer * sfst_to_internal_format(SFST::Transducer * t);
  
  /* Read an fst::StdVectorFst * and return the equivalent transducer in
     internal format. */
    InternalTransducer * tropical_ofst_to_internal_format
      (fst::StdVectorFst * t);
  
    /* Read a transducer in internal format and return the equivalent
       SFST::Transducer *. */
    SFST::Transducer * internal_format_to_sfst(InternalTransducer * t);
    
    /* Read a transducer in internal format and return the equivalent
       fst::StdVectorFst * */
    fst::StdVectorFst * internal_format_to_openfst(InternalTransducer * t);
}
#endif
