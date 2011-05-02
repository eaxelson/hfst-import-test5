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

#include "HfstSymbolDefs.h"

#ifndef MAIN_TEST

namespace hfst {

bool is_epsilon(std::string const & str)
{
    return str == internal_epsilon;
}

bool is_unknown(std::string const & str)
{
    return str == internal_unknown;
}

bool is_identity(std::string const & str)
{
    return str == internal_identity;
}

  namespace symbols {
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
  }


}

#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST
