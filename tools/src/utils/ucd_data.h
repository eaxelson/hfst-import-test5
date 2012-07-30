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



#ifndef GUARD_hfst_ucd_data_h
#define GUARD_hfst_ucd_data_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <stdint.h>

//! @brief data structure holding fields of the UnicodeData.txt.
//! @note  all fields are loaded iff configured with WANT_FULL_UCD, otherwise
//!        only simple capitalisation and category is used. general category
//!        uses 4+3 bytes per code point and case mapping 3*4 bytes per 
//!        code point. Other auxiliary data may use up to few 100s of bytes
//!        per code point (names, comments and all that).
typedef struct {
#   if WANT_FULL_UCD
    const char* name;
#   endif
    char* general_category;
#   if WANT_FULL_UCD
    const char* canonical_combining_class;
    const char* bidi_class;
    const char* decomposition_type;
    const char* decomposition_mapping;
    const char* numeric_type;
    double numeric_value;
    bool bidi_mirrored;
    const char* unicode_1_name;
    const char* iso_comment;
#   endif
    uint32_t simple_uppercase_mapping;
    uint32_t simple_lowercase_mapping;
    uint32_t simple_titlecase_mapping;
  } ucd_data;

//! @brief read unicode character database from well-known location.
//! @todo  this reads whole database into memory without any compression, i.e.
//!        wasting some 10 mebi times the ucd data size. End users wishing to
//!        have efficient version of this who cannot use ICU or glib are
//!        encouraged to offer better versions.
//! @return pointer to data on success, NULL if no data found or unreadable.
ucd_data* init_ucd_data();

//! @brief terminate ucd database @a ucd
void terminate_ucd_data(ucd_data* ucd);

//! @brief determine whether codepoint @a cp is a letter. That is, if it's
//!        general category is @e L.
bool ucd_is_letter(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is a number. That is, if it's
//!        general category is @e N.
bool ucd_is_number(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is a mark. That is, if it's
//!        general category is @e M.
bool ucd_is_mark(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is a symbol. That is, if it's
//!        general category is @e S.
bool ucd_is_symbol(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is a separator. That is, if it's
//!        general category is @e Z.
bool ucd_is_separator(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is special control. That is, if 
//!        it's general category is @e C.
bool ucd_is_control(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is punctuation. That is, if 
//!        it's general category is @e P.
bool ucd_is_punctuation(const ucd_data* ucd, uint32_t cp);
//! @brief determine whether codepoint @a cp is of given general category.
bool ucd_is_general_category(const ucd_data* ucd, uint32_t cp, 
                             const char* general_category);
//! @brief uppercase @a cp if it has simple uppercasing data.
uint32_t ucd_simple_uppercase_mapping(const ucd_data* ucd, uint32_t cp);
//! @brief lowercase @a cp if it has simple lowercasing data.
uint32_t ucd_simple_lowercase_mapping(const ucd_data* ucd, uint32_t cp);
//! @brief titlecase @a cp if it has simple titlecasing data.
uint32_t ucd_simple_titlecase_mapping(const ucd_data* ucd, uint32_t cp);
#endif
// vim: set ft=cpp.doxygen:
