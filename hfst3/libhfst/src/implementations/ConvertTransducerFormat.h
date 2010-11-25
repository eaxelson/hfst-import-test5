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

#ifndef _CONVERT_TRANSDUCER_H_
#define _CONVERT_TRANSDUCER_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <map>
#include <iostream>
#include <vector>
#include <map>

#if HAVE_OPENFST
#include <fst/fstlib.h>
#endif

#if HAVE_SFST
#include "sfst/fst.h"
#endif

#if HAVE_FOMA
#ifndef _FOMALIB_H_
#define _FOMALIB_H_
#include <stdbool.h>
#include "fomalib.h"
#endif
#endif

#include "HfstExceptions.h"

#include "optimized-lookup/transducer.h"

#include "HfstInternalTransducer.h"

struct fsm;

namespace hfst { namespace implementations {

  using namespace hfst::exceptions;

  // for testing
  //typedef fst::StdVectorFst InternalTransducer;

#if HAVE_OPENFST
  typedef fst::StdArc::StateId StateId;
  typedef fst::ArcIterator<fst::StdVectorFst> StdArcIterator;
  //typedef fst::StdVectorFst InternalTransducer;
#endif
#if HAVE_SFST
  typedef std::vector<SFST::Node *> SfstStateVector;
  typedef std::map<SFST::Node *,unsigned int> SfstToInternalStateMap;
#endif
#if HAVE_OPENFST
  typedef std::vector<StateId> OfstStateVector;
#endif

  /* Not needed.. */
  //typedef std::map<SFST::Node *,StateId> SfstToOfstStateMap;
  //typedef std::map<StateId,SFST::Node *> OfstToSfstStateMap;
  //typedef std::map<int,StateId> FomaToOfstStateMap;
  //typedef std::map<StateId,int> OfstToFomaStateMap;
  /* .. not needed ends. */

#if HAVE_OPENFST
  // added
  typedef std::map<hfst_ol::TransitionTableIndex,unsigned int> HfstOlToInternalStateMap;

  typedef std::map<hfst_ol::TransitionTableIndex,StateId> HfstOlToOfstStateMap;
  typedef fst::ArcTpl<fst::LogWeight> LogArc;
  typedef fst::VectorFst<LogArc> LogFst;
#endif

    /* -------------------------------------------------
       Conversion through an internal transducer format.
       ------------------------------------------------- */
    
    /* Read an SFST::Transducer * and return the equivalent transducer in
       internal format. */
#if HAVE_SFST
    HfstInternalTransducer * sfst_to_internal_hfst_format(SFST::Transducer * t);
#endif

#if HAVE_FOMA
    HfstInternalTransducer * foma_to_internal_hfst_format(struct fsm * t);
#endif  

  /* Read an fst::StdVectorFst * and return the equivalent transducer in
     internal format. */
#if HAVE_OPENFST
    HfstInternalTransducer * tropical_ofst_to_internal_hfst_format
      (fst::StdVectorFst * t);

  /* Read a LogFst * and return the equivalent transducer in
     internal format. */
    HfstInternalTransducer * log_ofst_to_internal_hfst_format
      (LogFst * t);
    
    /* Read an hfst_ol::Transducer * and return the equivalent transducer in
       internal format. */
    HfstInternalTransducer * hfst_ol_to_internal_hfst_format(hfst_ol::Transducer * t);
#endif  

    /* Read a transducer in internal format and return the equivalent
       SFST::Transducer *. */
#if HAVE_SFST
    SFST::Transducer * hfst_internal_format_to_sfst(const HfstInternalTransducer * t);
#endif

#if HAVE_FOMA
    struct fsm * hfst_internal_format_to_foma(const HfstInternalTransducer * t);
#endif    

    /* Read a transducer in internal format and return the equivalent
       fst::StdVectorFst * */
#if HAVE_OPENFST
    fst::StdVectorFst * hfst_internal_format_to_tropical_ofst(const HfstInternalTransducer * t);

    /* Read a transducer in internal format and return the equivalent
       LogFst * */
    LogFst * hfst_internal_format_to_log_ofst(const HfstInternalTransducer * t);
    
    /* Read a transducer in internal format and return the equivalent
       hfst_ol::Transducer * */
    hfst_ol::Transducer * hfst_internal_format_to_hfst_ol(HfstInternalTransducer * t, bool weighted);
#endif

} }
#endif
