#include "ComposeIntersectRule.h"

#ifndef MAIN_TEST

namespace hfst
{
  namespace implementations
  { 
   ComposeIntersectRule::ComposeIntersectRule(const HfstBasicTransducer &t):
      ComposeIntersectFst(t,true)
    {}   
    ComposeIntersectRule::ComposeIntersectRule(void):
      ComposeIntersectFst()
    {}   
  }
}

#else // MAIN_TEST was defined

#include <iostream>

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST
