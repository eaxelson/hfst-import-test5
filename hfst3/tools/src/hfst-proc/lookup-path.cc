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

#include "lookup-path.h"

//////////Function definitions for class LookupPath

void
LookupPath::follow(const TransitionIndex& index)
{
  this->index = index.get_target();
  final = index.final();
}

bool
LookupPath::follow(const Transition& transition)
{
  index = transition.get_target();
  final = transition.final();
  if(transducer.get_alphabet().symbol_to_string(transition.get_output_symbol()) != "")
    output_symbols.push_back(transition.get_output_symbol());
  
  return true;
}

bool
LookupPath::operator<(const LookupPath& o) const
{
  return output_symbols < o.output_symbols;
}


//////////Function definitions for class PathFd

bool
PathFd::evaluate_flag_diacritic(SymbolNumber s)
{
  return fd_state.apply_operation(s);
}


//////////Function definitions for class LookupPathFd

bool
LookupPathFd::follow(const Transition& transition)
{
  if(evaluate_flag_diacritic(transition.get_input_symbol()))
    return LookupPath::follow(transition);
  return false;
}


//////////Function definitions for class LookupPathW

void
LookupPathW::follow(const TransitionIndex& index)
{
  final_weight = static_cast<const TransitionWIndex&>(index).final_weight();
  return LookupPath::follow(index);
}

bool
LookupPathW::follow(const Transition& transition)
{
  weight += static_cast<const TransitionW&>(transition).get_weight();
  //**is this right? I'm not so sure about the precise semantics of weights
  //  and finals in this system**
  final_weight = static_cast<const TransitionW&>(transition).get_weight();
  return LookupPath::follow(transition);
}

bool
LookupPathW::operator<(const LookupPathW& o) const
{
  return ((get_weight() < o.get_weight()) ||
          (get_weight() == o.get_weight() && this->LookupPath::operator<(o)));
}


//////////Function definitions for class LookupPathWFd

bool
LookupPathWFd::follow(const Transition& transition)
{
  if(evaluate_flag_diacritic(transition.get_input_symbol()))
    return LookupPathW::follow(transition);
  return false;
}

