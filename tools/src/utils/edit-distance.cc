//! @file edit-distance.cc
//!
//! @brief create edit distance stuff
//!
//! @author HFST Team


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <set>
#include <string>
#include <sstream>

using std::set;
using std::string;
using std::stringstream;

#include <hfst.hpp>

using hfst::HfstTransducer;
using hfst::ImplementationType;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransition;

#include "edit-distance.h"



HfstTransducer* 
create_edit_distance(set<string>& alphabet,  unsigned long length,
                     float weight, ImplementationType format)
{
    HfstBasicTransducer edit;
    HfstState edit0_state = 0;
    HfstState edit1_state = 1;
    edit.add_state(edit0_state);
    edit.set_final_weight(edit0_state, 0);
    edit.add_state(edit1_state);
    edit.set_final_weight(edit1_state, 0);
    // ?:? runs allowed at beginning
    HfstBasicTransition other(edit0_state, hfst::internal_identity,
                              hfst::internal_identity, 0);
    edit.add_transition(edit0_state, other);
    // ?:? runs allowed at end
    HfstBasicTransition other_end(edit1_state, hfst::internal_identity,
                              hfst::internal_identity, 0);
    edit.add_transition(edit1_state, other_end);
    for (set<string>::const_iterator s = alphabet.begin();
         s != alphabet.end();
         ++s)
      {
        // x:x at beginning
        HfstBasicTransition correct(edit0_state, *s, *s, 0);
        // x:x at end
        HfstBasicTransition correct_end(edit1_state, *s, *s, 0);
        // x:0 is one error from beginning to end
        HfstBasicTransition deletion(edit1_state, *s, hfst::internal_epsilon,
                                     weight);
        // 0:x is one error from beginning to end
        HfstBasicTransition addition(edit1_state, hfst::internal_epsilon, *s,
                                     weight);

        edit.add_transition(edit0_state, correct);
        edit.add_transition(edit0_state, deletion);
        edit.add_transition(edit1_state, correct_end);
        for (set<string>::const_iterator r = alphabet.begin();
             r != alphabet.end();
             ++r)
          {
            // for substitution we need auxiliary path for transposition
            HfstState rs = edit.add_state();
            // x:y is one error from beginning to auxiliary state
            HfstBasicTransition change(rs, *s, *r, weight);
            edit.add_transition(edit0_state, change);
            // y:x freely from auxiliary to end without extra weight
            HfstBasicTransition transpose(edit1_state, *r, *s, 0);
            edit.add_transition(rs, transpose);
            // epsilon from auxiliary to end
            HfstBasicTransition breakout(edit1_state, hfst::internal_epsilon,
                                         hfst::internal_epsilon, 0);
            edit.add_transition(rs, breakout);
          }
      }
    HfstTransducer* rv = new HfstTransducer(edit, format);
    if (length > 1)
      {
        rv->repeat_n(length);
        stringstream longname;
        longname << "repeat-n(edit(" << *(alphabet.begin()) << "...), " <<
            length << ")";
        rv->set_name(longname.str());
      }
    else
      {
        rv->set_name(string("edit(") + *(alphabet.begin()) + "...)");
      }
    return rv;
}

