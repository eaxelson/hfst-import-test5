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

#include "SymbolDefs.h"
#include <stdbool.h>
#include "fomalib.h"

//#include "SFST/src/alphabet.h"

/*  const char * EPSILON_STRING = "@_EPSILON_SYMBOL_@";
    const char * UNKNOWN_STRING = "@_UNKNOWN_SYMBOL_@";
    const char * IDENTITY_STRING = "@_IDENTITY_SYMBOL_@";
    
    const unsigned int EPSILON_NUMBER = 0;
    const unsigned int UNKNOWN_NUMBER = 1;
    const unsigned int IDENTITY_NUMBER = 2; */

namespace hfst { 

namespace symbols
{

  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2)
  {
    for (StringSet::const_iterator it1 = s1.begin(); it1 != s1.end(); it1++) {
      String sym1 = *it1;
      if ( s2.find(sym1) == s2.end() )
	unknown2.insert(sym1);
    }
    for (StringSet::const_iterator it2 = s2.begin(); it2 != s2.end(); it2++) {
      String sym2 = *it2;
      if ( s1.find(sym2) == s1.end() )
	unknown1.insert(sym2);
    }
  }

} }
