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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "ConvertTransducerFormat.h"
#include "optimized-lookup/convert.h"
#include "HfstTransitionGraph.h"
#include "HfstTransducer.h"

#ifndef MAIN_TEST
namespace hfst { namespace implementations
{


  HfstBasicTransducer * ConversionFunctions::
  hfst_transducer_to_hfst_basic_transducer
  (const hfst::HfstTransducer &t) {

#if HAVE_SFST
    if (t.type == SFST_TYPE)
      return sfst_to_hfst_basic_transducer(t.implementation.sfst); 
#endif // HAVE_SFST

#if HAVE_OPENFST
    if (t.type == TROPICAL_OPENFST_TYPE)
      return tropical_ofst_to_hfst_basic_transducer
        (t.implementation.tropical_ofst); 
    if (t.type == LOG_OPENFST_TYPE)
      return log_ofst_to_hfst_basic_transducer(t.implementation.log_ofst); 
#endif // HAVE_OPENFST
    
#if HAVE_FOMA
    if (t.type == FOMA_TYPE)
      return foma_to_hfst_basic_transducer(t.implementation.foma); 
#endif // HAVE_FOMA
    
    /* Add here your implementation. */
    //#if HAVE_MY_TRANSDUCER_LIBRARY
    //    if (t.type == MY_TRANSDUCER_LIBRARY_TYPE)
    //      return my_transducer_library_transducer_to_hfst_basic_transducer
    //        (t.implementation.my_transducer_library); 
    //#endif // HAVE_MY_TRANSDUCER_LIBRARY

    if (t.type == HFST_OL_TYPE || t.type == HFST_OLW_TYPE)
      return hfst_ol_to_hfst_basic_transducer(t.implementation.hfst_ol);

    HFST_THROW(FunctionNotImplementedException);
  }


  /* Add here your conversion functions or write them to a separate file. */
  //#if HAVE_MY_TRANSDUCER_LIBRARY
  //
  //HfstBasicTransducer * ConversionFunctions::
  //my_transducer_library_transducer_to_hfst_basic_transducer
  //  (my_namespace::MyFst * t) {
  //(void)t;
  //HFST_THROW(FunctionNotImplementedException);
  //}

  //my_namespace::MyFst * ConversionFunctions::
  //hfst_basic_transducer_to_my_transducer_library_transducer
  //  (const HfstBasicTransducer * t) {
  //(void)t;
  //HFST_THROW(FunctionNotImplementedException);
  //}
  //#endif // HAVE_MY_TRANSDUCER_LIBRARY


} }

#else // MAIN_TEST was defined
#include <cstdlib>
#include <cassert>

using namespace hfst;
using namespace hfst::implementations;

int main(void)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    HfstBasicTransducer net;
    net.add_transition(0, HfstBasicTransition(1, "c", "d", 1));
    net.add_transition(1, HfstBasicTransition(2, "a", "o", 2));
    net.add_transition(2, HfstBasicTransition(3, "t", "g", 3));
    net.set_final_weight(3, 4);
    std::cout << std::endl << "Conversions: ";
    std::cout << "skipped everything " <<
      "since they've disappeared into thin air" << std::endl;
    return 77;
  }
#endif // MAIN_TEST
