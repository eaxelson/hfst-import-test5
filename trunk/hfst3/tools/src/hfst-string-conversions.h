#ifdef WINDOWS
#  include <string>
#endif // WINDOWS

#include <cstdio>

namespace hfst 
{

#ifdef WINDOWS
  /* Convert utf-8 string \a wstr into an ordinary string. */
  //std::string wide_string_to_string(const std::wstring & wstr);
  /* Get a line from console input and store it into \a str.
         \a buffer_size defines maximum of input length. 
         Return whether the read operation was succesful. */
  bool get_line_from_console(std::string & str, size_t buffer_size, bool keep_newline = false);

  void set_console_cp_to_utf8();
#endif // WINDOWS

  /* Whether hfst_fprintf prints directly to console output.
         Has no effect on linux or mac. */
  void print_output_to_console(bool val);
  /* Return whether hfst_fprintf prints directly to console output.
         The default is false. */
  bool is_output_printed_to_console();

  /* Wrapper around fprintf that prints to console output,
         is_output_printed_to_console is true. 
         On linux and mac, calls always fprintf directly. */
  int hfst_fprintf(FILE * stream, const char * format, ...);

}
