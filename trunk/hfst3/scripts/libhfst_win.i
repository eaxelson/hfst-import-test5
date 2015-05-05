%module libhfst
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_set.i"

%{
#define HFSTIMPORT
#include "HfstDataTypes.h"
#include "HfstTransducer.h"
#include "parsers/XreCompiler.h"
#include "implementations/HfstTransitionGraph.h"

// todo instead: #include "hfst_extensions.h"

namespace hfst {

typedef std::vector<float> FloatVector;

hfst::ImplementationType type = hfst::TROPICAL_OPENFST_TYPE;

hfst::HfstTokenizer deftok; // default tokenizer

void set_default_fst_type(hfst::ImplementationType t)
{
	type = t;
}

hfst::ImplementationType get_default_fst_type()
{
	return type;
}

std::string fst_type_to_string(hfst::ImplementationType t)
{
	std::string retval = hfst::implementation_type_to_string(t);
	return retval;
}
	
hfst::HfstTransducer fst(const std::string & symbol)
{
	return hfst::HfstTransducer(symbol, type);
}

hfst::HfstTransducer fst(const std::string & isymbol, const std::string & osymbol)
{
	return hfst::HfstTransducer(isymbol, osymbol, type);
}

hfst::HfstTransducer * regex(const std::string & regex_string)
{
	hfst::xre::XreCompiler comp(type);
	return comp.compile(regex_string);
}

hfst::HfstTransducer word(const std::string & w, float weight=0)
{
	return hfst::HfstTransducer(w, deftok, type);
}

hfst::HfstTransducer word_pair(const std::string & wi, const std::string & wo, float weight=0)
{
	return hfst::HfstTransducer(wi, wo, deftok, type);
}

hfst::HfstTransducer word_list_hfst(const StringVector & wl, const FloatVector & weights)
{
	hfst::implementations::HfstBasicTransducer retval;
	unsigned int i=0;
	for (std::vector<std::string>::const_iterator it = wl.begin();
		it != wl.end(); it++)
		{
			retval.disjunct(deftok.tokenize(*it), weights[i]);
			i++;
		}
	return hfst::HfstTransducer(retval, type);
}

hfst::HfstTransducer word_pair_list_hfst(const StringPairVector & wpl, const FloatVector & weights)
{
	hfst::implementations::HfstBasicTransducer retval;
	unsigned int i=0;
	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = wpl.begin();
		it != wpl.end(); it++)
		{
			retval.disjunct(deftok.tokenize(it->first, it->second), weights[i]);
			i++;
		}
	return hfst::HfstTransducer(retval, type);
}

}

%}

%include <windows.h>

%include "typemaps.i"

namespace std {
%template(StringVector) vector<string>;
%template(StringPair) pair<string, string>;
%template(StringPairVector) vector<pair<string, string > >;
%template(FloatVector) vector<float>;
%template(StringSet) set<string>;
%template(HfstOneLevelPath) pair<float, vector<string> >;
%template(HfstOneLevelPaths) set<pair<float, vector<string> > >;
}

%ignore hfst::HfstTransducer::lookup_fd(const std::string & s) const;

typedef __int64 ssize_t;

namespace hfst
{

typedef std::vector<std::string> StringVector;
typedef std::pair<std::string, std::string> StringPair;
typedef std::vector<std::pair<std::string, std::string> > StringPairVector;
typedef std::vector<float> FloatVector;
typedef std::set<std::string> StringSet;
typedef std::pair<float, std::vector<std::string> > HfstOneLevelPath;
typedef std::set<std::pair<float, std::vector<std::string> > > HfstOneLevelPaths;

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

HfstOneLevelPaths * lookup_fd(const std::string & s, int limit=-1) const;

    %extend {
    char *__str__() {
         static char tmp[1024];
         $self->write_in_att_format(tmp);
         return tmp;
    }
    };


};

hfst::HfstTransducer fst(const std::string & symbol);
hfst::HfstTransducer fst(const std::string & isymbol, const std::string & osymbol);
hfst::HfstTransducer * regex(const std::string & regex_string);
hfst::HfstTransducer word(const std::string & w, float weight=0);
//hfst::HfstTransducer word(const std::string & w, const hfst::HfstTokenizer & tok);
hfst::HfstTransducer word_pair(const std::string & wi, const std::string & wo, float weight=0);
//hfst::HfstTransducer word_pair(const std::string & wi, const std::string & wo, const hfst::HfstTokenizer & tok);
hfst::HfstTransducer word_list_hfst(const StringVector & wl, const FloatVector & weights);
hfst::HfstTransducer word_pair_list_hfst(const StringPairVector & wpl, const FloatVector & weights);


%pythoncode %{

EPSILON='@_EPSILON_SYMBOL_@'
UNKNOWN='@_UNKNOWN_SYMBOL_@'
IDENTITY='@_IDENTITY_SYMBOL_@'

def word_list(l):
	v = StringVector()
	f = FloatVector()
	for word in l:
		v.push_back(word[0])
		if (len(word) > 1):
			f.push_back(word[1])
		else:
			f.push_back(0)
	return _libhfst.word_list_hfst(v,f)
		
def word_pair_list(l):
	v = StringPairVector()
	f = FloatVector()
	for word in l:
		v.push_back(StringPair(word[0],word[1]))
		if (len(word) > 2):
			f.push_back(word[2])
		else:
			f.push_back(0)	
	return _libhfst.word_pair_list_hfst(v,f)

%}

void set_default_fst_type(hfst::ImplementationType t);
hfst::ImplementationType get_default_fst_type();
std::string fst_type_to_string(hfst::ImplementationType t);

}
