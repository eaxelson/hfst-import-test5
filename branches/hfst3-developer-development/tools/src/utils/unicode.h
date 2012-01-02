//! @file unicode.h
//! @brief HFST implementations for string handling and ctype functions


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



#ifndef GUARD_hfst_unicode_h
#define GUARD_hfst_unicode_h 1

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <stdlib.h>

//! @brief convert utf8 character to unicode codepoint
uint32_t utf8tocp(const char* c);

//! @brief convert codepoint to utf8 string. The resulting string has been
//!     allocated and needs to be freed.
char* cptoutf8(uint32_t cp);

//! @brief tokenise whole utf8 string to utf8 characters. Returns number
//!     of strings allocated.
size_t strtoutf8(const char* s, char*** rv);

//! @brief tokenise string to utf8 characters one at a time. Resulting
//!     string is allocated and needs to be freed. The saveptr will point
//!     to @c 0 at the end of string when finished, or to NULL on error.
char* utf8tok_r(const char* str, char **saveptr);

//! @brief convert utf8 character to its uppercase version
char* utf8toupper(const char* c);

//! @brief convert utf8 character to its lowercase version
char* utf8tolower(const char* c);

//! @brief convert utf8 character to its titlecase version
char* utf8totitle(const char* c);

//! @brief test whether UTF-8 character at @a c is uppercase unicode character.
bool utf8isupper(const char* c);
//! @brief test whether UTF-8 character at @a c is lowercase unicode character.
bool utf8islower(const char* c);
//! @brief test whether UTF-8 character at @a c is titlecase unicode character.
bool utf8istitle(const char* c);
//! @brief test whether UTF-8 character at @a c is unicode space character.
bool utf8isspace(const char* c);
//! @brief test whether UTF-8 character at @a c is alphabetic unicode character.
bool utf8isalpha(const char* c);
//! @brief test whether UTF-8 character at @a c is unicode digit character.
bool utf8isdigit(const char* c);
//! @brief test whether UTF-8 character at @a c is alphanumeric unicode
//!     character.
bool utf8isalnum(const char* c);
#endif
// vim: set ft=cpp.doxygen:
