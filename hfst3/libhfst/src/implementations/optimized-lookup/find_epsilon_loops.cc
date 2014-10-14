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

#include "./transducer.h"

namespace hfst_ol {

// Define an operation for checking state equivalence for the
// purpose of detecting the same situation happening twice
bool TraversalState::operator==(const TraversalState & rhs) const
{
    if (this->from_index != rhs.from_index) {
        return false;
    }
    if (this->to_index != rhs.to_index) {
        return false;
    }
    for (size_t i = 0; i < this->flags.size(); ++i) {
        if (this->flags[i] != rhs.flags[i]) {
            return false;
        }
    }
    return true;
}

bool TraversalState::operator<(const TraversalState & rhs) const
{
    if (this->from_index < rhs.from_index) {
        return true;
    }
    if (this->from_index > rhs.from_index) {
        return false;
    }
    if (this->to_index < rhs.to_index) {
        return true;
    }
    if (this->to_index > rhs.to_index) {
        return false;
    }
    for (size_t i = 0; i < this->flags.size(); ++i) {
        if (this->flags[i] < rhs.flags[i]) {
            return true;
        }
        if (this->flags[i] > rhs.flags[i]) {
            return false;
        }
    }
    return false;    
}

void Transducer::find_loop_epsilon_transitions(
    unsigned int input_pos,
    TransitionTableIndex i,
    PositionStates & position_states)
{
    FlagDiacriticState flags = flag_state.get_values();
    while (input_pos >= position_states.size()) {
        // first time at this input
        std::set<TraversalState> v;
        position_states.push_back(v);
    }
    TraversalState epsilon_reachable(i, tables->get_transition_target(i), flags);
    if (position_states[input_pos].count(epsilon_reachable) == 1) {
        // We've been here before
        throw true;
    }
    bool added_this_state = false;
    while (true)
    {
        TransitionTableIndex target = tables->get_transition_target(i);
        if (tables->get_transition_input(i) == 0) // epsilon
        {
            if (!added_this_state) {
                // We try to trap non-progressing loops
                position_states[input_pos].insert(epsilon_reachable);
                added_this_state = true;
            }
            find_loop(input_pos,
                      target,
                      position_states);
            found_transition = true;
            ++i;
        } else if (alphabet->is_flag_diacritic(
                       tables->get_transition_input(i))) {
            
            if (flag_state.apply_operation(
                    *(alphabet->get_operation(
                          tables->get_transition_input(i))))) {
                // flag diacritic allowed
                if (!added_this_state) {
                    position_states[input_pos].insert(epsilon_reachable);
                    added_this_state = true;
                }
                find_loop(input_pos,
                          target,
                          position_states);
                found_transition = true;
            }
            flag_state.assign_values(flags);
            ++i;
        } else { // it's not epsilon and it's not a flag, so nothing to do
            return;
        }
    }
}

void Transducer::find_loop_epsilon_indices(unsigned int input_pos,
                                                TransitionTableIndex i,
                                                PositionStates & position_states)
{
    if (tables->get_index_input(i) == 0)
    {
        find_loop_epsilon_transitions(input_pos,
                                      tables->get_index_target(i) - 
                                      TRANSITION_TARGET_TABLE_START,
                                      position_states);
        found_transition = true;
    }
}

void Transducer::find_loop_transitions(SymbolNumber input,
                                            unsigned int input_pos,
                                            TransitionTableIndex i,
                                            PositionStates & position_states)
{

    while (tables->get_transition_input(i) != NO_SYMBOL_NUMBER) {
        if (tables->get_transition_input(i) == input) {
            find_loop(input_pos,
                      tables->get_transition_target(i),
                      position_states);
            found_transition = true;
        } else {
            return;
        }
        ++i;
    }
}

void Transducer::find_loop_index(SymbolNumber input,
                                      unsigned int input_pos,
                                      TransitionTableIndex i,
                                      PositionStates & position_states)
{
    if (tables->get_index_input(i+input) == input)
    {
        find_loop_transitions(input,
                              input_pos,
                              tables->get_index_target(i+input) - 
                              TRANSITION_TARGET_TABLE_START,
                              position_states);
        found_transition = true;
    }
}




void Transducer::find_loop(unsigned int input_pos,
                           TransitionTableIndex i,
                           PositionStates & position_states)
{
    found_transition = false;
    
    if (indexes_transition_table(i))
    {
        i -= TRANSITION_TARGET_TABLE_START;
        find_loop_epsilon_transitions(input_pos,
                                      i+1,
                                      position_states);
        
        // input-string ended.
        if (input_tape[input_pos] == NO_SYMBOL_NUMBER)
        {
            return;
        }
      
        SymbolNumber input = input_tape[input_pos];
        ++input_pos;

        find_loop_transitions(input,
                              input_pos,
                              i+1,
                              position_states);
        if (alphabet->get_default_symbol() != NO_SYMBOL_NUMBER &&
            !found_transition) {
            find_loop_transitions(alphabet->get_default_symbol(),
                                  input_pos, i+1,
                                  position_states);
        }
    }
    else
    {
        find_loop_epsilon_indices(input_pos,
                                  i+1,
                                  position_states);
        
        if (input_tape[input_pos] == NO_SYMBOL_NUMBER)
        { // input-string ended.
            return;
        }
      
        SymbolNumber input = input_tape[input_pos];
        ++input_pos;

        find_loop_index(input,
                        input_pos,
                        i+1,
                        position_states);
        // If we have a default symbol defined and we didn't find an index,
        // check for that
        if (alphabet->get_default_symbol() != NO_SYMBOL_NUMBER && !found_transition) {
            find_loop_index(alphabet->get_default_symbol(),
                            input_pos, i+1,
                            position_states);
        }
    }
}


}
