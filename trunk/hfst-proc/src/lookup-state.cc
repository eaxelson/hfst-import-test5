#include "lookup-state.h"


inline bool indexes_transition_table(const TransitionTableIndex i)
{
  return i >= TRANSITION_TARGET_TABLE_START;
}
inline bool indexes_transition_index_table(const TransitionTableIndex i)
{
  return i < TRANSITION_TARGET_TABLE_START;
}


void
LookupState::init(LookupPath* initial)
{
  clear_paths();
  paths.push_back(initial);
  try_epsilons();
}

void
LookupState::lookup(const SymbolNumberVector& input)
{
  for(SymbolNumberVector::const_iterator it=input.begin(); it!=input.end(); it++)
    step(*it);
}

void
LookupState::step(const SymbolNumber input)
{
  if(input == NO_SYMBOL_NUMBER)
  {
    clear_paths();
    return;
  }
  
  apply_input(input);
  try_epsilons();
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



void
LookupState::try_epsilons()
{
  for(size_t i=0; i<paths.size(); i++)
  {
    const LookupPath& path = *paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_epsilon_index(path);
    else // indexes transition table
      try_epsilon_transitions(path);
  }
}

void
LookupState::try_epsilon_index(const LookupPath& path)
{
  // if this path points to an entry in the transition index table
  // which indexes one or more epsilon transtions
  const TransitionIndex& index = transducer.get_index(path.get_index()+1);
  
  if(index.matches(0))
  {
    // copy the current path, follow the index, add the new path to the list
    LookupPath& epsilon_path = *path.clone();
    epsilon_path.follow(index);
    add_path(epsilon_path);
  }
}

void
LookupState::try_epsilon_transitions(const LookupPath& path)
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
    
    if(transducer.is_epsilon(transition))
    {
      // copy the path, follow the transition, add the new path to the list
      LookupPath& epsilon_path = *path.clone();
      if(epsilon_path.follow(transition))
        add_path(epsilon_path);
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
LookupState::apply_input(const SymbolNumber input)
{
  LookupPathVector new_paths;
  if(input == 0)
  {
    replace_paths(new_paths);
    return;
  }
  
  for(size_t i=0; i<paths.size(); i++)
  {
    LookupPath& path = *paths[i];
    
    if(indexes_transition_index_table(path.get_index()))
      try_index(new_paths, path, input);
    else // indexes transition table
      try_transitions(new_paths, path, input);
  }

  replace_paths(new_paths);  
}

void
LookupState::try_index(LookupPathVector& new_paths, 
                         const LookupPath& path, 
                         const SymbolNumber input) const
{
  //??? is the +1 here correct?
  TransitionIndex index = transducer.get_index(path.get_index()+input+1);
  
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
LookupState::try_transitions(LookupPathVector& new_paths,
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

