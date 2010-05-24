#include "InputReader.h"

void InputReader::set_input(std::istream * file)
{
  buffer_size = 500000;
  buffer_index = 0;
  input_stream = file;
  buffer = new char[buffer_size];
  input_stream->getline(buffer,buffer_size);
}

void InputReader::warn(const char * warning)
{
  std::cerr << std::endl;
  std::cerr << warning << std::endl;
  std::cerr << "on line " << line_number << ":" << std::endl;
  std::cerr << buffer << std::endl;
}

void InputReader::error(const char * err)
{
  std::cerr << std::endl;
  std::cerr << err << std::endl;
  std::cerr << "on line " << line_number << ":" << std::endl;
  std::cerr << buffer << std::endl;
  std::cerr << "Aborted." << std::endl << std::endl;
}

char InputReader::input(void)
{
  if ((buffer[buffer_index] == 0) or (buffer_index > buffer_size - 1))
    {
      input_stream->getline(buffer,buffer_size);
      buffer_index = 0;
      if (input_stream->eof())
	{
	  return 0;
	}
      else
	{
	  return '\n';
	}
    }
  return buffer[buffer_index++];
}
