#include "ExceptionTest.h"

HfstException::HfstException(void) {}

HfstException::HfstException
(const std::string &name,const std::string &file,size_t line):
  name(name),
  file(file),
  line(line)
{}

std::string HfstException::operator() (void) const
{ 
  std::ostringstream o;
  o << "Exception: "<< name << " in file: " << file << " on line: " << line;
  return o.str();
}

//! Define a subclass of @a HfstException of type @a CHILD.
#define HFST_EXCEPTION_CHILD_DEFINITION(CHILD) \
    CHILD::CHILD \
      (const std::string &name,const std::string &file,size_t line):	\
      HfstException(name,file,line)					\
      {}

// Example defining an exception class SomeHfstException:
//HFST_EXCEPTION_CHILD_DEFINITION(SomeHfstException);

