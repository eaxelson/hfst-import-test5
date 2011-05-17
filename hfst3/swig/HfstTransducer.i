%module libhfst

%{
#define SWIG_FILE_WITH_INIT
#include "HfstOutputStream.h"
%}

namespace hfst
{
HfstTransducer();
}