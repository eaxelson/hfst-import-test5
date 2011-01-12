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

/** @file HfstSymbolDefs.h
    \brief Typedefs and functions for symbols, symbol pairs and sets of symbols. */

namespace hfst
{

  /** \brief A UTF-8 symbol in a transition. 

    Strings <i>"@_EPSILON_SYMBOL_@"</i>, <i>"@_UNKNOWN_SYMBOL_@"</i> and <i>"@_IDENTITY_SYMBOL_@"</i> are reserved.

    - <i>"@_EPSILON_SYMBOL_@"</i> denotes the epsilon. 
    - <i>"@_UNKNOWN_SYMBOL_@"</i> denotes an unknown symbol, i.e. any symbol that 
      does not occur or has not occurred in the transducer. 
    - <i>"@_IDENTITY_SYMBOL_@"</i> denotes any unknown symbol that is the same
      on the input and output side of a transition.

    Strings of form <CODE> @[A-Z][.][A-Z]+([.][A-Z]+)?@ </CODE> are reserved for flag diacritics.
    For an example of flag diacritics, see #hfst::HfstTransducer::extract_strings_fd(WeightedPaths<float>::Set&, int, int, bool)

    An example of unknown and identity symbols:

\verbatim

\endverbatim
   */
  typedef std::string String;

  /* A set of strings. */
  typedef std::set<String> StringSet;

  /** \brief A symbol pair in a transition. 

      @see HfstTransducer::substitute(const StringPair&, const StringPair&) and other substitute functions */
  typedef std::pair<String, String> StringPair;

  /** \brief A vector of transitions that represents a path in a transducer. 

      @see HfstTokenizer */
  typedef std::vector<StringPair> StringPairVector;

  /** \brief A set of symbol pairs used in substituting symbol pairs and in rule functions. 

     @see HfstTransducer::substitute(const StringPair&, const StringPairSet &) #hfst::rules */
  typedef std::set<StringPair> StringPairSet;


  /* For internal use */
  typedef std::pair<unsigned int, unsigned int> NumberPair;
  typedef std::vector<NumberPair> NumberPairVector;
  typedef std::set<NumberPair> NumberPairSet;
  typedef std::map<String,unsigned int> StringNumberMap;
  typedef std::map<unsigned int,unsigned int> NumberNumberMap;
  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2);
}
#endif
