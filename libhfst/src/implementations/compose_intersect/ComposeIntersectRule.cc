#include "ComposeIntersectRule.h"

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
