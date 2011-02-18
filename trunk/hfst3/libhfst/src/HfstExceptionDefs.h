#ifndef _HFST_EXCEPTION_DEFS_H_
#define _HFST_EXCEPTION_DEFS_H_

#include <string>
#include <sstream>

//! @brief Base class for HfstExceptions. Holds its own name and the file 
//! and line number where it was thrown.
struct HfstException
{
  std::string name;
  std::string file;
  size_t line;
  HfstException(void);
  HfstException(const std::string &name,const std::string &file,size_t line);
  std::string operator() (void) const;
};

//! @brief Macro to throw an exception of type @a E.
//! Use @a THROW instead of regular @a throw with subclasses of HfstException.
#define HFST_THROW(E) throw E(#E,__FILE__,__LINE__)

//! @brief Declare a subclass of @a HfstException of type @a CHILD.
#define HFST_EXCEPTION_CHILD_DECLARATION(CHILD) \
    struct CHILD : public HfstException \
    { CHILD(const std::string &name,const std::string &file,size_t line); } 

//! Define a subclass of @a HfstException of type @a CHILD.
#define HFST_EXCEPTION_CHILD_DEFINITION(CHILD) \
    CHILD::CHILD \
      (const std::string &name,const std::string &file,size_t line):	\
      HfstException(name,file,line)					\
      {}

// Example declaring an exception class SomeHfstException:
//HFST_EXCEPTION_CHILD_DECLARATION(SomeHfstException);

#endif // #ifndef _HFST_EXCEPTION_DEFS_H_
