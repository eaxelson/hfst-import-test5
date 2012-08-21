/* src/hfst.hpp.  Generated from hfst.hpp.in by configure.  */
/** @file hfst.hpp
 *  @brief Convenience header including full HFST3 public API.
 *
 *  This file @e must be included to a program that uses the HFST API. Only use
 *  other headers if you @e really know what you are doing.
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
#ifndef GUARD_HFST_HPP
#define GUARD_HFST_HPP

#if HAVE_CONFIG_H
#  include <config.h>
#  ifndef HAVE_MALLOC
#    error "your configure failed to find malloc, check README for further instructions"
#  endif
#endif

/* This below is autoheadered, Try not to touch! */

/* Define to 1 if you have the <backward/hash_map> header file. */
#define HAVE_BACKWARD_HASH_MAP 1

/* Define to 1 if you have the <backward/hash_set> header file. */
#define HAVE_BACKWARD_HASH_SET 1

/* Define to 1 if you have the <ext/hash_map> header file. */
#define HAVE_EXT_HASH_MAP 1

/* Define to 1 if you have the <ext/hash_set> header file. */
#define HAVE_EXT_HASH_SET 1

/* Define to compile foma support in HFST */
#define HAVE_FOMA 1

/* Define to 1 if you have the <hash_map> header file. */
#define HAVE_HASH_MAP 1

/* Define to 1 if you have the <hash_set> header file. */
#define HAVE_HASH_SET 1

/* Define to compile optimized lookup support in HFST */
#define HAVE_HFSTOL 1

/* Define to compile OpenFst support in HFST */
#define HAVE_OPENFST 1

/* Define to compile SFST support in HFST */
#define HAVE_SFST 1

/* Define to hfst version vector as long in base 10000 */
#define HFST_LONGVERSION 3000300012L

/* Automatically substitute to configure.ac revision */
#define HFST_REVISION "$Revision: 1950 $"

/* Define to libhfst pretty name for linked programs to use */
#define HFST_STRING $PACKAGE_STRING

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

