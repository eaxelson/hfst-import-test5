#include "VariableContainer.h"

void VariableContainer::set_value(void)
{
  assert(value_it != values.end());
  variable_values[variable] = *value_it;
}

void VariableContainer::reset(void)
{
  value_it = values.begin();
  set_value();
}

size_t VariableContainer::size(void)
{
  return values.size();
}

size_t VariableContainer::index(void)
{
  return value_it - values.begin();
}

void VariableContainer::next(void)
{
  ++value_it;
}

bool VariableContainer::last(void)
{
  return value_it + 1 == values.end();
}

bool VariableContainer::nth_last(size_t offset)
{
  return value_it + offset + 1 == values.end();
}

bool VariableContainer::end(void)
{
  return value_it == values.end();
}
