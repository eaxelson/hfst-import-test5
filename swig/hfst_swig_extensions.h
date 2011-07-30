#include <string>
#include <vector>
#include <utility>
#include "HfstDataTypes.h"
#include "HfstTransducer.h"
#include "Hfst

typedef std::vector<std::pair <float, std::vector<std::string> > >
OneLevelPathVector;

typedef std::vector<std::pair <float, std::string> > FloatStringVector;

namespace hfst {

OneLevelPathVector vectorize(HfstOneLevelPaths * holps, bool remove_flags = true)
{
    std::vector<std::pair <float, std::vector<std::string> > > retval;
    for (HfstOneLevelPaths::const_iterator it = holps->begin();
	 it != holps->end(); ++it) {
	if (
	retval.push_back(*it);
    }
    return retval;
}


FloatStringVector detokenize_vector(OneLevelPathVector olpv)
{
    FloatStringVector retval;
    for (OneLevelPathVector::const_iterator it = olpv.begin(); it != olpv.end();
	 ++it) {
	std::string temp;
	for (std::vector<std::string>::const_iterator str_it =
		 it->second.begin(); str_it != it->second.end(); ++str_it) {
	    temp.append(*str_it);
	}
	retval.push_back(std::pair<float, std::string>(it->first, temp));
    }
    return retval;
}

FloatStringVector detokenize_paths(HfstOneLevelPaths * holps)
{
    return detokenize_vector(vectorize(holps));
}

}
