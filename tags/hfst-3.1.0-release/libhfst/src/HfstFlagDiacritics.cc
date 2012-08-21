#include "HfstFlagDiacritics.h"

#ifndef MAIN_TEST

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

std::string::size_type FdOperation::find_diacritic
(const std::string& diacritic_str, 
 std::string::size_type& length)
{
  std::string::size_type start = diacritic_str.find('@');
  if(start != std::string::npos)
  {
    std::string::size_type end = diacritic_str.find('@', start+1);
    if(end != std::string::npos)
    {
      if(is_diacritic(diacritic_str.substr(start, end-start)))
      {
        length = end-start;
        return start;
      }
    }
  }
  return std::string::npos;

}

}

#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST