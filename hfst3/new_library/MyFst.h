#include <string>

/* Test for adding a new library to HFST. */

namespace mfstl {

  class MyFst {
  private:
    std::string istring;
    std::string ostring;
  public:
    MyFst();
    MyFst(std::string str);
    MyFst(std::string istr, std::string ostr);
    ~MyFst();
    
    void invert();
    MyFst * copy(); 
  };

}

