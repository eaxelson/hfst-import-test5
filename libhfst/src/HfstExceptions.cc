#include "HfstExceptions.h"

namespace hfst {
  namespace exceptions {

    HfstInterfaceException::HfstInterfaceException():
      message(std::string())
    { }

    HfstInterfaceException::HfstInterfaceException(std::string msg):
      message(msg)
    { }

    std::string HfstInterfaceException::get_message() { 
      return message; 
    }

  }
}

