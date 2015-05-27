#ifdef WINDOWS
#  include <windows.h>
#  include <string>
#endif // WINDOWS

#include <cstdarg>
#include <cstdio>

namespace hfst 
{
#ifdef WINDOWS
  std::string wide_string_to_string(const std::wstring & wstr)
    {
      int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
      std::string str( size_needed, 0 );
      WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
      return str;
    }
#endif // WINDOWS

#ifdef WINDOWS
  bool output_to_console = false;
  void print_output_to_console(bool val) { output_to_console = val; }
  bool is_output_printed_to_console() { return output_to_console; }
#else
  void print_output_to_console(bool val) { (void)val; }
  bool is_output_printed_to_console() { return false; }
#endif // WINDOWS

  int hfst_fprintf(FILE * stream, const char * format, ...)
  {
    va_list args;
    va_start(args, format);
#ifdef WINDOWS
    if (output_to_console && (stream == stdout || stream == stderr))
      {
        char buffer [1024];
        int r = vsprintf(buffer, format, args);
        va_end(args);
        if (r < 0)
          return r;
        HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (stream == stderr)
          stdHandle = GetStdHandle(STD_ERROR_HANDLE);

        std::string pstr(buffer);
        DWORD numWritten = 0;
        int wchars_num =
          MultiByteToWideChar(CP_UTF8 , 0 , pstr.c_str() , -1, NULL , 0 );
        wchar_t* wstr = new wchar_t[wchars_num];
        MultiByteToWideChar(CP_UTF8 , 0 ,
                            pstr.c_str() , -1, wstr , wchars_num );
        int retval = WriteConsoleW(stdHandle, wstr, wchars_num-1, &numWritten, NULL);
        delete[] wstr;

        return retval;
      }
    else
      {
        int retval = vfprintf(stream, format, args);
        va_end(args);
        return retval;
      }
#else
    int retval = vfprintf(stream, format, args);
    va_end(args);
    return retval;
#endif // WINDOWS
  }

} // namespace hfst
