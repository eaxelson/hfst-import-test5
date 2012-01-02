//! @unicode_data.h
//! @brief C representation of part of UnicodeData.txt important for casing
//!     and stuff.


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.



#ifndef GUARD_hfst_unicode_data_h
#define GUARD_hfst_unicode_data_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <stdint.h>

//! @brief data structure holding fields 2, 12, 13, and 14 of UnicodeData.txt.
typedef struct {
    uint32_t upper;
    uint32_t lower;
    uint32_t title;
    const char* category;
  } unicode_data;

//! @brief an array holding Unicode database up to and excluding plane 16.
extern unicode_data* unicode;

void init_unicode_data();
#endif
// vim: set ft=cpp.doxygen:
