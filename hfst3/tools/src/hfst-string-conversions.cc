#ifdef WINDOWS
#  include <windows.h>
#  include <string>
#endif // WINDOWS

#include <cstdarg>
#include <cstdio>

#include <iostream> // for debugging

namespace hfst 
{
#ifdef WINDOWS
  /*  std::string wide_string_to_string(const std::wstring & wstr)
    {
      int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
      std::string str( size_needed, 0 );
      WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
      return str;
      }*/
#endif // WINDOWS*/

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

#ifdef WINDOWS
  bool get_line_from_console(std::string & str, size_t buffer_size, bool keep_newline /* = false*/)
  {
    bool DEBUG = false;  
    SetConsoleCP(65001);
    const HANDLE stdIn = GetStdHandle(STD_INPUT_HANDLE);
    WCHAR * buffer = new WCHAR [buffer_size];
    DWORD numRead = 0;
    if (ReadConsoleW(stdIn, buffer, size_t(buffer_size/4), &numRead, NULL))
      {
        if (DEBUG) { std::cerr << "get_line_from_console: numRead is " << numRead << std::endl; }
        
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, (int)numRead, NULL, 0, NULL, NULL);
        if (DEBUG) { std::cerr << "conversions: size_needed is " << size_needed << std::endl; }
        CHAR * strbuf = new CHAR [size_needed];
        WideCharToMultiByte(CP_UTF8, 0, buffer, (int)numRead, strbuf, size_needed, NULL, NULL);
        delete buffer;
        strbuf[size_needed] = 0; // NULL-terminate the buffer
        str = std::string(strbuf);
        
        if (DEBUG) { std::cerr << "get_line_from_console: size of str is now (1) " << str.size() << std::endl; }
        
        if (str[0] == (char)26 || str[0] == (char)4) // control+Z, control+D
          return false;

        // Get rid of carriage returns and newlines.

        if (str.size() == 0)
          return true;

        // Get rid of carriage return
        if (str.size() > 1)
          {
            if (str[str.size()-2] == '\r')
              str.erase(str.size()-2, 1);
          }

        if (str[str.size()-1] != '\n')
          return true;

        if (keep_newline)
          return true;

        str.erase(str.size()-1);
        return true;
        
        if (DEBUG) { std::cerr << "get_line_from_console: size of str is now (2) " << str.size() << std::endl; }
        return true;
      }
    return false;
  }

  void set_console_cp_to_utf8()
  {
    SetConsoleCP(65001);
  }
#endif // WINDOWS

} // namespace hfst
