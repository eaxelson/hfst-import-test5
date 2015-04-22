%module libhfst
%include "std_string.i"

%{
#define HFSTIMPORT
#include "HfstDataTypes.h"
#include "HfstTransducer.h"
%}

%include <windows.h>
namespace hfst
{

enum ImplementationType
{
    SFST_TYPE,
    TROPICAL_OPENFST_TYPE,
    LOG_OPENFST_TYPE,
    FOMA_TYPE,
    XFSM_TYPE,
    HFST_OL_TYPE,
    HFST_OLW_TYPE,
    HFST2_TYPE,
    UNSPECIFIED_TYPE,
    ERROR_TYPE
};

class HfstTransducer 
{
public:	
HfstTransducer(ImplementationType);
HfstTransducer(const std::string &, const std::string &, ImplementationType);
~HfstTransducer();
HfstTransducer & concatenate(const HfstTransducer&, bool harmonize=true);
void write_in_att_format(const std::string &, bool write_weights=true) const;
};

}
