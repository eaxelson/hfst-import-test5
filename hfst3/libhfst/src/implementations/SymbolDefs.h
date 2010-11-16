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

  /** \brief A symbol in a transition. 

    Strings <i>"@_EPSILON_SYMBOL_@"</i>, <i>"@_UNKNOWN_SYMBOL_@"</i> and <i>"@_IDENTITY_SYMBOL_@"</i> are reserved.

    - <i>"@_EPSILON_SYMBOL_@"</i> denotes the epsilon. 
    - <i>"@_UNKNOWN_SYMBOL_@"</i> denotes an unknown symbol, i.e. any symbol that 
      does not occur or has not occurred in the transducer. 
    - <i>"@_IDENTITY_SYMBOL_@"</i> denotes any unknown symbol that is the same
      on the input and output side of a transition.
   */
  typedef std::string String;
  /* A set of strings. */
  typedef std::set<String> StringSet;
  /** \brief A symbol pair in a transition. 

      @see HfstTransducer::substitute(const StringPair&, const StringPair&) and other substitute functions */
  typedef std::pair<std::string, std::string> StringPair;

  typedef std::pair<unsigned int, unsigned int> NumberPair;

  /** \brief A vector of transitions that represents a path in a transducer. 

      @see HfstTokenizer */
  typedef std::vector<StringPair> StringPairVector;

  typedef std::vector<NumberPair> NumberPairVector;

  /** \brief A set of symbol pairs used in substituting symbol pairs. 

     @see HfstTransducer::substitute(const StringPair&, const StringPairSet &) */
  typedef std::set<StringPair> StringPairSet;

  typedef std::set<NumberPair> NumberPairSet;

  /* Maps strings to numbers. */
  typedef std::map<String,unsigned int> StringNumberMap;
  /* Maps numbers to numbers. */
  typedef std::map<unsigned int,unsigned int> NumberNumberMap;

  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2);
}
#endif
