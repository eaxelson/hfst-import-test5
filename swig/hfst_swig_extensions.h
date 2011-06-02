#include <string>
#include <vector>
#include <utility>
#include "HfstDataTypes.h"
#include "HfstTransducer.h"

namespace hfst {
std::vector<std::pair <float, std::vector<std::string> > > vectorize(HfstOneLevelPaths * holps)
{
    std::vector<std::pair <float, std::vector<std::string> > > retval;
    for (HfstOneLevelPaths::const_iterator it = holps->begin();
	 it != holps->end(); ++it) {
	retval.push_back(*it);
    }
    return retval;
}

}
