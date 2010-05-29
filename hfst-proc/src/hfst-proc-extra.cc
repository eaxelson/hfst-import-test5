#include "hfst-optimized-lookup.h"
#include "hfst-proc-extra.h"


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
  if(transition.get_output() != 0)
    output_symbols.push_back(transition.get_output());
  
  return true;
}


//////////Function definitions for class LookupPathFd

bool
LookupPathFd::evaluate_flag_diacritic(FlagDiacriticOperation op)
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

bool LookupPathFd::follow(const Transition& transition)
{
  if((*fd_operations)[transition.get_input()].isFlag())
  {
    if(evaluate_flag_diacritic((*fd_operations)[transition.get_input()]))
      return LookupPath::follow(transition);
    
    return false;
  }
  
  return LookupPath::follow(transition);
}

//////////Function definitions for class LookupPathW

void
LookupPathWFd::follow_weight(const TransitionIndex& index)
{
  final_weight = static_cast<const TransitionWIndex&>(index).final_weight();
}
void
LookupPathWFd::follow_weight(const Transition& transition)
{
  weight += static_cast<const TransitionW&>(transition).get_weight();
  //**is this right? I'm not so sure about the precise semantics of weights
  //  and finals in this system**
  final_weight = static_cast<const TransitionW&>(transition).get_weight();
}

void
LookupPathWFd::follow(const TransitionIndex& index)
{
  LookupPath::follow(index);
  follow_weight(index);
}

bool
LookupPathWFd::follow(const Transition& transition)
{
  if(LookupPathFd::follow(transition))
  {
    follow_weight(transition);
    return true;
  }
  
  return false;
}


//////////Function definitions for class LookupPathWFd

void
LookupPathW::follow_weight(const TransitionIndex& index)
{
  final_weight = static_cast<const TransitionWIndex&>(index).final_weight();
}
void
LookupPathW::follow_weight(const Transition& transition)
{
  weight += static_cast<const TransitionW&>(transition).get_weight();
  //**is this right? I'm not so sure about the precise semantics of weights
  //  and finals in this system**
  final_weight = static_cast<const TransitionW&>(transition).get_weight();
}

void
LookupPathW::follow(const TransitionIndex& index)
{
  LookupPath::follow(index);
  follow_weight(index);
}

bool
LookupPathW::follow(const Transition& transition)
{
  LookupPath::follow(transition);
  follow_weight(transition);
  
  return true;
}


//////////Function definitions for class LookupState

void
LookupState::init(LookupPath& initial)
{
  clear_paths();
  paths.push_back(&initial);
}
  
void
LookupState::clear_paths()
{
  for(LookupPathVector::const_iterator it = paths.begin(); it!=paths.end(); it++)
    delete *it;
  paths.clear();
}

bool
LookupState::is_final() const
{
  for(LookupPathVector::const_iterator i=paths.begin(); 
      i!=paths.end(); ++i)
  {
    TransitionTableIndex index = (*i)->get_index();
    if(indexes_transition_index_table(index))
    {
      if(transducer.get_index(index).final())
        return true;
    }
    else
    {
      if(transducer.get_transition(index).final())
        return true;
    }
  }
  return false;
}

const LookupPathVector
LookupState::get_finals() const
{
  LookupPathVector finals;
  for(LookupPathVector::const_iterator i=paths.begin(); i!=paths.end(); ++i)
  {
    TransitionTableIndex index = (*i)->get_index();
    if(indexes_transition_index_table(index))
    {
      if(transducer.get_index(index).final())
        finals.push_back(*i);
    }
    else
    {
      if(transducer.get_transition(index).final())
        finals.push_back(*i);
    }
  }
  return finals;
}

void
LookupState::add_path(LookupPath& path)
{
  paths.push_back(&path);
}

void
LookupState::replace_paths(LookupPathVector new_paths)
{
  clear_paths();
  paths = new_paths;
}



//////////Function definitions for class LookupStepper

void
LookupStepper::try_epsilons(LookupState& s) const
{
  for(size_t i=0; i<s.paths.size(); i++)
  {
    const LookupPath& path = *s.paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_epsilon_index(s, path);
    else // indexes transition table
      try_epsilon_transitions(s, path);
  }
}

void LookupStepper::try_epsilon_index(LookupState& s, const LookupPath& path) const
{
  // if this path points to an entry in the transition index table
  // which indexes one or more epsilon transtions
  const TransitionIndex& index = transducer.get_index(path.get_index()+1);
  
  if(index.matches(0))
  {
    // copy the current path, follow the index, add the new path to the list
    LookupPath& epsilon_path = *path.clone();
    epsilon_path.follow(index);
    s.add_path(epsilon_path);
  }
}

void
LookupStepper::try_epsilon_transitions(LookupState& s, const LookupPath& path) const
{
  TransitionTableIndex transition_index;
  
  // if the path is pointing to the "state" entry before the transitions
  if(transducer.get_transition(path.get_index()).get_input() == NO_SYMBOL_NUMBER)
    transition_index = path.get_index()+1;
  else // the path is pointing directly to a transition
    transition_index = path.get_index();
  
  while(true)
  {
    const Transition& transition = transducer.get_transition(transition_index);
    
    if(s.transducer.is_epsilon(transition))
    {
      // copy the path, follow the transition, add the new path to the list
      LookupPath& epsilon_path = *path.clone();
      if(epsilon_path.follow(transition))
        s.add_path(epsilon_path);
      else
      {
        // destroy the new path instead of pushing it
        delete &epsilon_path;
      }
    }
    else
      return;
    
    transition_index++;
  }
}


void
LookupStepper::apply_input(LookupState& s, const SymbolNumber input) const
{
  LookupPathVector new_paths;
  if(input == 0)
  {
    s.replace_paths(new_paths);
    return;
  }
  
  for(size_t i=0; i<s.paths.size(); i++)
  {
    LookupPath& path = *s.paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_index(new_paths, path, input);
    else // indexes transition table
      try_transitions(new_paths, path, input);
  }

  s.replace_paths(new_paths);  
}

void
LookupStepper::try_index(LookupPathVector& new_paths, 
                         const LookupPath& path, 
                         const SymbolNumber input) const
{
  TransitionIndex index = transducer.get_index(path.get_index()+input);
  
  if(index.matches(input))
  {
    // copy the path, follow the index, and handle the new transitions
    LookupPath& extended_path = *path.clone();
    extended_path.follow(index);
    try_transitions(new_paths, extended_path, input);
    delete &extended_path;
  }
}

void
LookupStepper::try_transitions(LookupPathVector& new_paths,
                               const LookupPath& path, 
                               const SymbolNumber input) const
{
  TransitionTableIndex transition_index;
  
  // if the path is pointing to the "state" entry before the transitions
  if(transducer.get_transition(path.get_index()).get_input() == NO_SYMBOL_NUMBER)
    transition_index = path.get_index()+1;
  else // the path is pointing directly to a transition
    transition_index = path.get_index();
  
  while(true)
  {
    const Transition& transition = transducer.get_transition(transition_index);
    
    if(transition.matches(input))
    {
      // copy the path, follow the transition, add the new path to the list
      LookupPath& extended_path = *path.clone();
      extended_path.follow(transition);
      new_paths.push_back(&extended_path);
    }
    else
      return;
    
    transition_index++;
  }
}



void
LookupStepper::init(LookupState& s, LookupPath& initial) const
{
  s.init(initial);
  try_epsilons(s);
}

void
LookupStepper::step(LookupState& s, const SymbolNumber input) const
{
  apply_input(s, input);
  try_epsilons(s);
}


