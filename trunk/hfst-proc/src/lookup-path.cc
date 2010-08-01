#include "lookup-path.h"

//////////Function definitions for class LookupPath

void
LookupPath::follow(const TransitionIndex& index)
{
  this->index = index.target();
  final = index.final();
}

bool
LookupPath::follow(const Transition& transition)
{
  index = transition.target();
  final = transition.final();
  if(transducer.get_alphabet().symbol_to_string(transition.get_output()) != "")
    output_symbols.push_back(transition.get_output());
  
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
  if(evaluate_flag_diacritic(transition.get_input()))
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
  if(evaluate_flag_diacritic(transition.get_input()))
    return LookupPathW::follow(transition);
  return false;
}

