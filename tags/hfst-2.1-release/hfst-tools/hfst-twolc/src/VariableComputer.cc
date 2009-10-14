#include "VariableComputer.h"

void VariableComputer::set_matcher(Matcher mm)
{
  matcher = mm;
}

void VariableComputer::reset(void)
{
  matcher = FREE;
  for (VariableContainerVector::iterator it = variables.begin();
       it != variables.end();
       ++it)
    {
      delete *it;
    }
  variables.clear();
}
#include <iostream>
void VariableComputer::define_variable(SymbolNumber variable,
				       VariableValueVector &values)
{
  VariableContainer * container =
    new VariableContainer(variable,
			  values,
			  variable_values);
  variables.push_back(container);

  if (not values.empty())
    {
      container->set_value();
    }
}

bool VariableComputer::check_variables(void)
{
  assert(not variables.empty());
  if ((matcher == MATCH) or (matcher == MIX))
    {
      size_t value_list_length = variables.at(0)->size();
      for (VariableContainerVector::iterator it = variables.begin();
	   it != variables.end();
	   ++it)
	{
	  if ((*it)->size() != value_list_length)
	    {
	      return false;
	    }
	}
    }
  return true;
}

void VariableComputer::begin(void)
{
  for (VariableContainerVector::iterator it = variables.begin();
       it != variables.end();
       ++it)
    {
      (*it)->reset();
    }
  if (matcher == MIX)
    {
      if (variables.size() > 1)
	{
	  next();
	}
    }
}

bool VariableComputer::end(void)
{
  if (variables.empty())
    {
      return true;
    }
  if ((matcher == FREE) or (matcher == MIX))
    {
      for (VariableContainerVector::iterator it = variables.begin();
	   it != variables.end();
	   ++it)
	{
	  if (not (*it)->end())
	    {
	      return false;
	    }
	}
    }
  else if (matcher == MATCH)
    {
      VariableContainer * first = *(variables.begin());
      if (not first->end())
	{
	  return false;	  
	}
    }
  else 
    {
      size_t offset = 0;
      
    }
  return true;
}

bool VariableComputer::duplicate_indices(void)
{
  IndexSet s;
  for (VariableContainerVector::iterator it = variables.begin();
       it != variables.end();
       ++it)
    {
      size_t i = (*it)->index();
      if (s.find(i) != s.end())
	{
	  return true;
	}
      s.insert(i);
    }
  return false;
}

bool VariableComputer::last_combination(void)
{
  if ((matcher == FREE) or (matcher == MATCH))
    {
      for (VariableContainerVector::iterator it = variables.begin();
	   it != variables.end();
	   ++it)
	{
	  if (not (*it)->last())
	    {
	      return false;
	    }
	}
      return true;
    }
  else
    {
      size_t offset = 0;
      for (VariableContainerVector::reverse_iterator it = variables.rbegin();
	   it != variables.rend();
	   ++it)
	{
	  if (not (*it)->nth_last(offset))
	    {
	      return false;
	    }
	  ++offset;
	}
      return true;
    }
}

bool VariableComputer::last_free_combination(void)
{
  for (VariableContainerVector::iterator it = variables.begin();
       it != variables.end();
       ++it)
    {
      if (not (*it)->last())
	{
	  return false;
	}
    }
  return true;
}

void VariableComputer::next(void)
{
  if ((matcher == FREE) or (matcher == MIX))
    {
      bool last = last_free_combination();
      for (VariableContainerVector::iterator it = variables.begin();
	   it != variables.end();
	   ++it)
	{
	  VariableContainer * v = *it;
	  v->next();
	  if (v->end())
	    {
	      if (not last)
		{
		  v->reset();	       
		  if (not v->end())
		    {
		      v->set_value();
		    }
		}
	    }
	  else
	    {	      
	      v->set_value();
	      break;
	    }
	}
      if (matcher == MIX)
	{
	  if (duplicate_indices())
	    {
	      if (not end())
		{
		  next();
		}
	    }
	}
    }
  else if (matcher == MATCH)
    {
      for (VariableContainerVector::iterator it = variables.begin();
	   it != variables.end();
	   ++it)
	{
	  (*it)->next();
	  if (not (*it)->end())
	    {
	      (*it)->set_value();
	    }
	}
    }
}
