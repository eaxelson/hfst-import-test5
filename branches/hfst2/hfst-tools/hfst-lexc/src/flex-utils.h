//!
//! @file flex-utils.h
//!
//! @brief Functions to aid making better error messages with flex and yacc.
//!
//! This file contains functions necessary to track line and column numberings,
//! symbol widths and so forth, while lexing and parsing through the file.
//! C style char arrays are used since flex plays nicer with chars than strings.

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

#ifndef GUARD_hlexc_input_h
#define GUARD_hlexc_input_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstring>

//! @brief Set filename used for position messages.
void set_infile_name(const char* s);

//! @brief Initialise memory of file positions to zeros.
//!
//! Sets all members of current yylloc structure to zeros.
void token_reset_positions();

//! @brief Keep memory of positions of last parsed tokens for error messages.
//!
//! Counts length, height and width of the given token. Update yylloc structure
//! provided by lex and yacc, for location data.
void token_update_positions(const char* token);

//! @brief writes token positions in standard format.
char* strdup_token_positions();

//! @brief create some sensible representation of current token.
char* strdup_token_part();
//! @brief Strips percent escaping and strdups
char* strip_percents(const char* s, bool do_zeros);

//! @brief Strips initial and final white space and strdups
char* strstrip(const char* s);

//! @brief extracts the variable substring part from token.
//! Omits constant string prefix, suffix and optionally strips spaces.
char* strdup_nonconst_part(const char* token, 
						const char* prefix,
						const char* suffix,
						bool strip);

// vim:set ft=cpp.doxygen:
#endif

