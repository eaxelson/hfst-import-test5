/** @file hfst.hh
 *  @brief Convenience header including full HFST3 public API. 
 *
 *  This file must be included to a program that uses the HFST API.
 */

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
#ifndef _HFST_H_
#define _HFST_H_

#if HAVE_CONFIG_H
#  include <config.h>
#  ifndef HAVE_MALLOC
#    error "your configure failed to find malloc, check README for further instructions"
#  endif
#endif

// These are all the public HFST3 APIs.
#include "HfstTransducer.h"
#include "HfstDataTypes.h"
#include "HfstSymbolDefs.h"
#include "HfstTokenizer.h"
#include "HfstExceptionDefs.h"
#include "HfstInputStream.h"
#include "HfstOutputStream.h"

#endif

// vim: set ft=cpp.doxygen:

