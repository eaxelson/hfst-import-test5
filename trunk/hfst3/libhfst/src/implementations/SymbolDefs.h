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

#ifndef _SYMBOL_DEFS_H_
#define _SYMBOL_DEFS_H_
#include <vector>
#include <string>
#include <map>
#include <set>

/* extern const char * EPSILON_STRING;
   extern const char * UNKNOWN_STRING;
   extern const char * IDENTITY_STRING;
   
   extern const unsigned int EPSILON_NUMBER;
   extern const unsigned int UNKNOWN_NUMBER;
   extern const unsigned int IDENTITY_NUMBER; */

namespace hfst
{

  /* A symbol in a transition. */
  typedef std::string String;
  /* A set of strings. */
  typedef std::set<String> StringSet;
  /** \brief A symbol pair in a transition. 

      @see HfstTransducer::substitute(const StringPair&, const StringPair&) and other substitute functions */
  typedef std::pair<std::string, std::string> StringPair;
  /** \brief A vector of transitions that represents a path in a transducer. 

      @see HfstTokenizer */
  typedef std::vector<StringPair> StringPairVector;
  /** \brief A set of symbol pairs used in substituting symbol pairs. 

      @see HfstTransducer::substitute(const StringPair&, const StringPairSet &) */
  typedef std::set<StringPair> StringPairSet;
  /* Maps strings to numbers. */
  typedef std::map<String,unsigned int> StringNumberMap;
  /* Maps numbers to numbers. */
  typedef std::map<unsigned int,unsigned int> NumberNumberMap;

  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2);
}
#endif
