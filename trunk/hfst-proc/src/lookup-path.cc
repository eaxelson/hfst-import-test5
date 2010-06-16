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
PathFd::evaluate_flag_diacritic(const FlagDiacriticOperation& op)
{
  switch (op.Operation()) {
  case P: // positive set
    fd_state[op.Feature()] = op.Value();
    return true;
    
  case N: // negative set (literally, in this implementation)
    fd_state[op.Feature()] = -1*op.Value();
    return true;
    
  case R: // require
    if (op.Value() == 0) // empty require
      return (fd_state[op.Feature()] != 0);
    else // nonempty require
      return (fd_state[op.Feature()] == op.Value());
      
  case D: // disallow
    if (op.Value() == 0) // empty disallow
       return (fd_state[op.Feature()] == 0);
    else // nonempty disallow
      return (fd_state[op.Feature()] != op.Value());
      
  case C: // clear
    fd_state[op.Feature()] = 0;
    return true;
    
  case U: // unification
    if(fd_state[op.Feature()] == 0 || // if the feature is unset or
       fd_state[op.Feature()] == op.Value() || // the feature is at this value already or
       (fd_state[op.Feature()] < 0 &&
       (fd_state[op.Feature()]*-1 != op.Value())) // the feature is negatively set to something else
       )
    {
      fd_state[op.Feature()] = op.Value();
      return true;
    }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool
PathFd::evaluate_flag_diacritic(SymbolNumber s)
{
  if(symbol_table[s].fd_op.isFlag())
  {
    if(evaluate_flag_diacritic(symbol_table[s].fd_op))
    {
      if(printDebuggingInformationFlag)
        std::cout << "flag diacritic [" << s << "] allowed" << std::endl;
      return true;
    }
    else
    {
      if(printDebuggingInformationFlag)
        std::cout << "flag diacritic [" << s << "] disallowed" << std::endl;
      return false;
    }
  }
  else
    return true;
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

