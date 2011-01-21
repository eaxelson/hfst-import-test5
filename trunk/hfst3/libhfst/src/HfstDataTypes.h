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
#ifndef _HFST_DATA_TYPES_H_
#define _HFST_DATA_TYPES_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "HfstTransducer.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>


/** @file HfstDataTypes.h  
    \brief Datatypes that are needed when using the HFST API. */

namespace hfst
{

  class HfstTransducer;
  class HfstGrammar;
  class HfstInputStream;
  class HfstOutputStream;

  /// @brief a vector of transducers for methods applying a cascade of automata
  typedef std::vector<HfstTransducer> HfstTransducerVector;

  // ENUMS AND TYPEDEFS...

  /** \brief The type of an HfstTransducer. */
  enum ImplementationType 
  {
    SFST_TYPE, /**< An SFST transducer, unweighted. */
    TROPICAL_OFST_TYPE, /**< An OpenFst transducer with tropical weights. */
    LOG_OFST_TYPE, /**< An OpenFst transducer with logarithmic weights. */
    FOMA_TYPE, /**< A foma transducer, unweighted. */
    /* Add an enumerator for your transducer type here. */
    //MY_TRANSDUCER_LIBRARY_TYPE, 
    HFST_OL_TYPE, /**< An HFST optimized lookup transducer, unweighted */
    HFST_OLW_TYPE, /**< An HFST optimized lookup transducer with weights */
    HFST2_TYPE, /**< HFST2 header present, conversion required */
    UNSPECIFIED_TYPE, /**< Format left open by e.g. default constructor */
    ERROR_TYPE /**< Type not recognised. 
		  This type might be returned by a function if an error occurs. */ 
  };

  /** \brief The type of a push operation.
      @see HfstTransducer::push_weights */
  enum PushType
  { 
    TO_INITIAL_STATE /**< Push weights towards initial state. */,
    TO_FINAL_STATE /**< Push weights towards final state(s). */ 
  };

  //! @brief A pair of transducers
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::pair <HfstTransducer,HfstTransducer> HfstTransducerPair;
  //! @brief A set of transducer pairs
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::set <HfstTransducerPair> HfstTransducerPairSet;
  //! @brief A vector of transducer pairs
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::vector <HfstTransducerPair> HfstTransducerPairVector;

  typedef std::vector<std::string> HfstArcPath;
  //! @brief A path of one level of arcs with collected weight,
  //!
  //! Used as the source and result data type for lookups and downs.
  typedef std::pair<HfstArcPath,float> HfstLookupPath;
  //! @brief A set of simple paths.
  //!
  //! Used as return type of lookup with multiple, unique results.
  typedef std::set<HfstLookupPath> HfstLookupPaths;

  // ...ENUMS AND TYPEDEFS


}
// vim: set ft=cpp.doxygen: 
#endif
