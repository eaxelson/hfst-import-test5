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

#include "XfsmTransducer.h"

#ifndef MAIN_TEST
namespace hfst { namespace implementations {

    void XfsmTransducer::initialize_xfsm()
    {
      FST_CNTXTptr cntxt = initialize_cfsm();
      (void) set_char_encoding(cntxt, CHAR_ENC_UTF_8);
    }

    NETptr XfsmTransducer::create_xfsm_unknown_to_unknown_transducer()
    {
      NETptr result = null_net();
      STATEptr final = add_state_to_net(result, 1);
      id_type ti = id_pair_to_id(OTHER, OTHER);
      if( add_arc_to_state(result, result->start.state, ti, final, NULL, 0) == NULL )
        throw "add_arc_to_state failed";
      //std::cerr << "create_xfsm_unknown_to_unknown_transducer() result:" << std::endl;
      //PAGEptr page = network_to_page(result, NULL);
      //print_page(page, stderr);
      return result;
    }

    NETptr XfsmTransducer::create_xfsm_identity_to_identity_transducer()
    {
      NETptr result = null_net();
      STATEptr final = add_state_to_net(result, 1);
      id_type ti = OTHER;
      if( add_arc_to_state(result, result->start.state, ti, final, NULL, 0) == NULL )
        throw "add_arc_to_state failed";
      //std::cerr << "create_xfsm_identity_to_identity_transducer() result:" << std::endl;
      //PAGEptr page = network_to_page(result, NULL);
      //print_page(page, stderr);
      return result;
    }


  } }

#else // MAIN_TEST was defined
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace hfst::implementations;

int main(int argc, char * argv[]) 
{
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
}
#endif // MAIN_TEST
