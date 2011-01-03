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
#endif // HAVE_CONFIG_H

#include <map>
#include <iostream>
#include <vector>
#include <map>

#if HAVE_OPENFST
#include <fst/fstlib.h>
#endif // HAVE_OPENFST

#if HAVE_SFST
#include "sfst/fst.h"
#endif // HAVE_SFST

#if HAVE_FOMA
#ifndef _FOMALIB_H_
#define _FOMALIB_H_
#include <stdbool.h>
#include "fomalib.h"
#endif // _FOMALIB_H_
#endif // HAVE_FOMA

#if HAVE_MFSTL
#ifndef _MFSTLLIB_H_
#define _MFSTLLIB_H_
#include "mfstl/MyFst.h"
#endif // _MFSTLLIB_H_
#endif // HAVE_MFSTL

#include "HfstExceptions.h"
#include "optimized-lookup/transducer.h"
//#include "HfstNet.h"

struct fsm;

/** @file ConvertTransducerFormat.h
    \brief Declarations of functions for converting between transducer backend formats. */

namespace hfst { 

  class HfstTransducer;

namespace implementations {

    template <class T, class W> class HfstNet;
    class TransitionData;
    typedef HfstNet<TransitionData, float> HfstFsm; 
    typedef unsigned int HfstState;

  using namespace hfst::exceptions;

#if HAVE_OPENFST
  typedef fst::StdArc::StateId StateId;
  typedef fst::ArcIterator<fst::StdVectorFst> StdArcIterator;
#endif
#if HAVE_SFST
  //typedef std::vector<SFST::Node *> SfstStateVector;
  //typedef std::map<SFST::Node *,unsigned int> SfstToInternalStateMap;
#endif
#if HAVE_OPENFST
  //typedef std::vector<StateId> OfstStateVector;
#endif


#if HAVE_OPENFST
  //typedef std::map<hfst_ol::TransitionTableIndex,unsigned int> HfstOlToInternalStateMap;

  //typedef std::map<hfst_ol::TransitionTableIndex,StateId> HfstOlToOfstStateMap;
  typedef fst::ArcTpl<fst::LogWeight> LogArc;
  typedef fst::VectorFst<LogArc> LogFst;
#endif


  class ConversionFunctions {

  public:

    static HfstFsm * hfst_transducer_to_hfst_net(const hfst::HfstTransducer &t);

#if HAVE_SFST
  static void sfst_to_hfst_net( SFST::Node *node, SFST::NodeNumbering &index, 
				std::set<SFST::Node*> &visited_nodes, 
				HfstFsm *net, SFST::Alphabet &alphabet );

  static HfstFsm * sfst_to_hfst_net(SFST::Transducer * t);

  static SFST::Transducer * hfst_net_to_sfst(const HfstFsm * t);
#endif // HAVE_SFST
  
#if HAVE_FOMA
  static HfstFsm * foma_to_hfst_net(struct fsm * t);

  static struct fsm * hfst_net_to_foma(const HfstFsm * t);
#endif // HAVE_FOMA

#if HAVE_OPENFST
  static HfstFsm * tropical_ofst_to_hfst_net
    (fst::StdVectorFst * t);
  
  static StateId hfst_state_to_state_id
    (HfstState s, std::map<HfstState, StateId> &state_map, 
     fst::StdVectorFst * t);

  static fst::StdVectorFst * hfst_net_to_tropical_ofst
    (const HfstFsm * t);

  static HfstFsm * log_ofst_to_hfst_net
    (LogFst * t);
  
  static LogFst * hfst_net_to_log_ofst
    (const HfstFsm * t);
#endif // HAVE_OPENFST 
  

  static HfstFsm * hfst_ol_to_hfst_net(hfst_ol::Transducer * t);

  static hfst_ol::Transducer * hfst_net_to_hfst_ol
    (HfstFsm * t, bool weighted);


#if HAVE_MFSTL
  static HfstFsm * mfstl_to_hfst_net(mfstl::MyFst * t);

  static mfstl::MyFst * hfst_net_to_mfstl(const HfstFsm * t);
#endif // HAVE_MFSTL

  };


  } }
#endif // _CONVERT_TRANSDUCER_H_

