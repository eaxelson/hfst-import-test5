#include "../config.h"
#include <iostream>

class InputReader
{
private:
  std::istream * input_stream;
  int &line_number;

  char * buffer;

  size_t buffer_size;
  size_t buffer_index;

public:
  InputReader(int &counter):
    line_number(counter),
    buffer(NULL)
  { };

  ~InputReader(void)
  {
    delete[] buffer;
    buffer = NULL;
  }
  
  void set_input(std::istream * file);
  void warn(const char * warning);
  void error(const char * err);
  char input(void);
};
