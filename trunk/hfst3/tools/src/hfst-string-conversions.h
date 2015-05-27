#ifdef WINDOWS
#  include <string>
#endif // WINDOWS

#include <cstdio>

namespace hfst 
{

#ifdef WINDOWS
  std::string wide_string_to_string(const std::wstring & wstr);
#endif // WINDOWS

  // no effect on linux or mac
  void print_output_to_console(bool val);
  // always false on linux or mac
  bool is_output_printed_to_console();

  int hfst_fprintf(FILE * stream, const char * format, ...);

}
