#include <string>

/** @file MyFst.h
    @brief Contains declarations for a very simple transducer library,
    My Finite-State Transducer Library - MFSTL, which is used 
    to exemplify how a new library is added under HFST.

    The classes and functions do not work in any way properly
    but serve as an example of a transducer library that is
    added under HFST.
*/

/** @brief Namespace for classes in the example transducer library MFSTL. */
namespace mfstl {

  /** @brief The transducer class of MFSTL. */
  class MyFst {
  private:
    std::string istring;
    std::string ostring;
  public:
    /** @brief Basic constructor. */
    MyFst();
    /** @brief Another constructor. */
    MyFst(std::string str);
    /** @brief Yet another constructor. */
    MyFst(std::string istr, std::string ostr);
    /** @brief Delete transducer. */
    ~MyFst();
    
    /** @brief Invert the transducer. */
    void invert();
    /** @brief Return a deep copy of the transducer. */
    MyFst * copy(); 
  };

}

