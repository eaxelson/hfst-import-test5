#include "FlagDiacritics.h"

namespace hfst {

bool FdOperation::is_diacritic(const std::string& diacritic_string)
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

}
