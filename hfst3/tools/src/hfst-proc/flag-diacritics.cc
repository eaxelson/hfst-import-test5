#include "flag-diacritics.h"

bool
FdOperation::is_diacritic(const std::string& diacritic_string)
{
  // All diacritics have form @[A-Z][.][A-Z]+([.][A-Z]+)?@
  if (diacritic_string.size() < 5)
    { return false; }
  if (diacritic_string.at(2) != '.')
    { return false; }
  // These two checks probably always succeed...
  if (diacritic_string.at(0) != '@')
    { return false; }
  if (diacritic_string.at(diacritic_string.size()-1) != '@')
    { return false; }
  switch (diacritic_string.at(1))
    {
    case 'P':
      break;
    case 'N':
      break;
    case 'D':
      break;
    case 'R':
      break;
    case 'C':
      break;
    case 'U':
      break;
    default:
      return false;
    }
  if (diacritic_string.find_last_of('.') == 2)
    {
      if ((diacritic_string.at(1) != 'R') and
	  (diacritic_string.at(1) != 'D') and
	  (diacritic_string.at(1) != 'C'))
      { return false; }
    }
  return true;
}

bool
FdState::apply_operation(const FdOperation& op)
{
  switch(op.Operator()) {
  case Pop: // positive set
    values[op.Feature()] = op.Value();
    return true;
    
  case Nop: // negative set (literally, in this implementation)
    values[op.Feature()] = -1*op.Value();
    return true;
    
  case Rop: // require
    if (op.Value() == 0) // empty require
      return (values[op.Feature()] != 0);
    else // nonempty require
      return (values[op.Feature()] == op.Value());
      
  case Dop: // disallow
    if (op.Value() == 0) // empty disallow
       return (values[op.Feature()] == 0);
    else // nonempty disallow
      return (values[op.Feature()] != op.Value());
      
  case Cop: // clear
    values[op.Feature()] = 0;
    return true;
    
  case Uop: // unification
    if(values[op.Feature()] == 0 || // if the feature is unset or
       values[op.Feature()] == op.Value() || // the feature is at this value already or
       (values[op.Feature()] < 0 &&
       (values[op.Feature()]*-1 != op.Value())) // the feature is negatively set to something else
       )
    {
      values[op.Feature()] = op.Value();
      return true;
    }
    return false;
  }
  throw; // for the compiler's peace of mind
}

bool
FdState::apply_operation(SymbolNumber symbol)
{
  const FdOperation* op = table.get_operation(symbol);
  if(op)
    return apply_operation(*op);
  return true; // if the symbol isn't a diacritic
}

bool
FdState::fails()
{
  return error_flag;
}

void
FdState::reset()
{
  error_flag = false;
  values.clear();
  values.insert(values.begin(), table.num_features(), 0);
}


FdTable::FdTable():
  feature_map(), value_map()
{
  value_map[std::string()] = 0; // empty value = neutral
}

void
FdTable::define_diacritic(SymbolNumber symbol, const std::string& str)
{
  if(!FdOperation::is_diacritic(str))
    throw;
  
  FdOperator op = FdOperation::char_to_operator(str.at(1));
  std::string feat;
  std::string val;
  
  // Third character is always the first fullstop.
  size_t first_full_stop_pos = 2;
  // Find the second full stop, if there is one.
  size_t second_full_stop_pos = str.find('.',first_full_stop_pos+1);
  size_t last_char_pos = str.size() - 1;
  if(second_full_stop_pos == std::string::npos)
  {
    assert(op == Cop || op == Dop || op == Rop);
    feat = str.substr(first_full_stop_pos+1, last_char_pos-first_full_stop_pos-1);
  }
  else
  {
    feat = str.substr(first_full_stop_pos+1, second_full_stop_pos-first_full_stop_pos-1);
    val = str.substr(second_full_stop_pos+1, last_char_pos-second_full_stop_pos-1);
  }
  
  if(feature_map.count(feat) == 0)
  {
    FdFeature next = feature_map.size();
    feature_map[feat] = next;
  }
  if(value_map.count(val) == 0)
  {
    FdValue next = value_map.size()+1;
    value_map[val] = next;
  }
  
  operations.insert(std::pair<SymbolNumber,FdOperation>(symbol, FdOperation(op, feature_map[feat], value_map[val], str)));
}

bool
FdTable::is_diacritic(SymbolNumber symbol) const
{
  return operations.find(symbol) != operations.end();
}

const FdOperation*
FdTable::get_operation(SymbolNumber symbol) const
{
  std::map<SymbolNumber, FdOperation>::const_iterator i = operations.find(symbol);
  return (i==operations.end()) ? NULL : &(i->second);
}

bool
FdTable::is_valid_string(SymbolNumberVector symbols)
{
  FdState state(*this);
  
  for(SymbolNumberVector::const_iterator i=symbols.begin(); i!=symbols.end(); i++)
  {
    if(!state.apply_operation(*i))
      return false;
  }
  return !state.fails();
}

