#ifndef _CONVERT_TRANSDUCER_H_
#define _CONVERT_TRANSDUCER_H_
#include <map>
#include <iostream>
#include <vector>
#include <map>
#include <fst/fstlib.h>
#include "HfstExceptions.h"
#include "SFST/src/fst.h"

struct fsm;

namespace hfst { namespace implementations {

  using namespace hfst::exceptions;

  typedef fst::StdArc::StateId StateId;
  typedef fst::ArcIterator<fst::StdVectorFst> StdArcIterator;
  typedef fst::StdVectorFst InternalTransducer;
  typedef std::vector<SFST::Node *> SfstStateVector;
  typedef std::vector<StateId> OfstStateVector;
  typedef std::map<SFST::Node *,StateId> SfstToOfstStateMap;
  typedef std::map<StateId,SFST::Node *> OfstToSfstStateMap;
  typedef std::map<int,StateId> FomaToOfstStateMap;
  typedef std::map<StateId,int> OfstToFomaStateMap;
  typedef fst::ArcTpl<fst::LogWeight> LogArc;
  typedef fst::VectorFst<LogArc> LogFst;

    /* SFST::Transducer * is the sfst transducer format.
       fst::StdVectorFst * is the openfst transducer format. */
    
    /* Read an SFST::Transducer * and return the equivalent transducer in
       internal format. */
    InternalTransducer * sfst_to_internal_format(SFST::Transducer * t);

    InternalTransducer * foma_to_internal_format(struct fsm * t);
  
  /* Read an fst::StdVectorFst * and return the equivalent transducer in
     internal format. */
    InternalTransducer * tropical_ofst_to_internal_format
      (fst::StdVectorFst * t);

  /* Read a LogFst * and return the equivalent transducer in
     internal format. */
    InternalTransducer * log_ofst_to_internal_format
      (LogFst * t);
  
    /* Read a transducer in internal format and return the equivalent
       SFST::Transducer *. */
    SFST::Transducer * internal_format_to_sfst(InternalTransducer * t);

    struct fsm * internal_format_to_foma(InternalTransducer * t);
    
    /* Read a transducer in internal format and return the equivalent
       fst::StdVectorFst * */
    fst::StdVectorFst * internal_format_to_openfst(InternalTransducer * t);

    /* Read a transducer in internal format and return the equivalent
       LogFst * */
    LogFst * internal_format_to_log_ofst(InternalTransducer * t);
} }
#endif
