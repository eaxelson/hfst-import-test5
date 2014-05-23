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

  bool get_encode_weights();

namespace implementations
{

  StdVectorFst * TropicalWeightTransducer::minimize(StdVectorFst * t)
  {
    RmEpsilon<StdArc>(t);

    EncodeMapper<StdArc> encode_mapper
      (hfst::get_encode_weights() ? (kEncodeLabels|kEncodeWeights) : (kEncodeLabels), ENCODE);
    Encode(t, &encode_mapper);
    StdVectorFst * det = new StdVectorFst();

    Determinize<StdArc>(*t, det);
    Minimize<StdArc>(det);
    Decode(det, encode_mapper);
    return det;
  }

}
}
