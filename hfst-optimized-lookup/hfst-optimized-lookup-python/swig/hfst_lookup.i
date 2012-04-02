%module hfst_lookup
%include "std_string.i"
%include "std_set.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%{
#define SWIG_FILE_WITH_INIT
#include "transducer.h"
#include "HfstFlagDiacritics.h"
#include "HfstExceptionDefs.h"
#include "HfstSymbolDefs.h"
%}

namespace std {
%template(StringFloatPair) pair<string, float>;
%template(StringFloatVector) vector<pair<string, float> >;
}

namespace hfst_ol {
class Transducer;
//class Speller;

class Transducer{
public:
    Transducer(const std::string & filename);
    std::vector<std::pair<std::string, float> > lookup(const std::string & input);
};
}


//class Speller{
//public:
//};
