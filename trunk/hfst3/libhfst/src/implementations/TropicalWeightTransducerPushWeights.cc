//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "TropicalWeightTransducer.h"
#include "HfstSymbolDefs.h"
#include "HfstLookupFlagDiacritics.h"

namespace hfst { 

namespace implementations
{

  StdVectorFst * TropicalWeightTransducer::push_weights
    (StdVectorFst * t, bool to_initial_state)
  {
    assert (t->InputSymbols() != NULL);
    fst::StdVectorFst * retval = new fst::StdVectorFst();
    if (to_initial_state)
      fst::Push<StdArc, REWEIGHT_TO_INITIAL>(*t, retval, fst::kPushWeights);
    else
      fst::Push<StdArc, REWEIGHT_TO_FINAL>(*t, retval, fst::kPushWeights);
    retval->SetInputSymbols(t->InputSymbols());
    return retval;
  }

  }
}

