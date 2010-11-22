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
#ifndef _HFST_GRAMMAR_H_
#define _HFST_GRAMMAR_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "HfstTransducer.h"


namespace hfst
{
#if HAVE_OPENFST
  /** \brief A set of transducers used in function HfstTransducer::compose_inte
rsect. 

      @see HfstTransducer::compose_intersect */

  class HfstGrammar
  {
  protected:
    HfstTransducer &first_rule;
    hfst::implementations::Grammar * grammar;
    hfst::implementations::TransducerVector transducer_vector;
  public:
    /** \brief Convert \a rule_vector into an HfstGrammar. */
    HfstGrammar(HfstTransducerVector &rule_vector);
    /** \brief Convert \a rule into an HfstGrammar. */
    HfstGrammar(HfstTransducer &rule);
    /** \brief Get the first rule of the grammar. */
    HfstTransducer get_first_rule(void);
    /** \brief Delete the grammar. */
    ~HfstGrammar(void);
    friend class HfstTransducer;
  };

#endif

}


/* vim: set ft=cpp.doxygen: */
#endif
