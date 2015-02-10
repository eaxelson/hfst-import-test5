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

#ifndef _XFSM_TRANSDUCER_H_
#define _XFSM_TRANSDUCER_H_

#include "HfstSymbolDefs.h"
#include "HfstExceptionDefs.h"
#include "HfstExtractStrings.h"
#include "HfstFlagDiacritics.h"
#include <stdlib.h>

#ifndef _XFSMLIB_H_
#define _XFSMLIB_H_
#include "xfsm/xfsm_api.h"
#endif
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>

#include "../FormatSpecifiers.h"

/** @file XfsmTransducer.h
    \brief Declarations of functions and datatypes that form a bridge between
    HFST API and xfsm. */

namespace hfst {
  namespace implementations
{
  class XfsmTransducer {
  public:
    static void initialize_xfsm();
  } ;
} }
#endif
