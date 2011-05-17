%module libhfst

%{
#define SWIG_FILE_WITH_INIT
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
%}

namespace hfst
{
class HfstTransducer;
class HfstInputStream;
class HfstOutputStream;
}