#include "FlagDiacritics.h"

#ifndef DEBUG_MAIN
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

std::string::size_type FdOperation::find_diacritic(const std::string& diacritic_str, 
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
#else

#include "FlagDiacritics.h"
#include <iostream>
#include <cassert>
#include <cstdlib>

int
main(int argc, char** argv)
{
  std::string::size_type len = 0;
  std::cout << "Unit Tests for " __FILE__ ":";
  std::cout << std::endl << "find diacritic... ";

    assert(hfst::FdOperation::find_diacritic("@P.A.X@", len) != std::string::npos);
    assert(hfst::FdOperation::find_diacritic("FOO", len) == std::string::npos);
  std::cout << std::endl << "is diacritic... ";
    assert(hfst::FdOperation::is_diacritic("@P.A.X@"));
    assert(!hfst::FdOperation::is_diacritic("FOO"));
    std::cout << "ok" << std::endl;
    return EXIT_SUCCESS;
}
#endif
