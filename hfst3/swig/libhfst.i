%module libhfst
%include "std_string.i"
%include "std_set.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%{
#define SWIG_FILE_WITH_INIT
#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"
#include "HfstDataTypes.h"
#include "hfst_swig_extensions.h"
%}

namespace std {
%template(StringVector) vector<string>;
%template(OneLevelPath) pair<float, vector<string> >;
%template(OneLevelPathVector) vector<pair<float, vector<string> > >;
%template(OneLevelPaths) set<pair<float, vector<string> > >;
%template(FloatStringVector) vector<pair<float, string> >;
}

namespace hfst
{
class HfstInputStream;
class HfstOneLevelPaths;

std::vector<std::pair <float, std::vector<std::string> > > vectorize(HfstOneLevelPaths * olps);
std::vector<std::pair <float, std::string> > detokenize_vector(OneLevelPathVector olpv);
std::vector<std::pair <float, std::string> > detokenize_paths(HfstOneLevelPaths * olps);

class HfstTransducer {
public:
    HfstTransducer();
    HfstTransducer(HfstInputStream & in);
    HfstOneLevelPaths * lookup_fd(const std::string & s, ssize_t limit = -1) const;
};

class HfstInputStream{
public:
HfstInputStream(void);
HfstInputStream(const std::string & filename);
void close(void);
};

class HfstOutputStream;

class FdOperation{
public:
FdOperation(const hfst::FdOperation&);
static bool is_diacritic(const std::string& diacritic_str);
};

}

//%pythoncode %{
//%}