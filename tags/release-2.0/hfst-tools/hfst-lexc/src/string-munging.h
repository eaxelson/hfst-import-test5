//! @file string-munging.h
//!
//! @brief Various string handling methods for HFST lexc.
//! 
//! @author Tommi A. Pirinen


//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GUARD_lexc_utils_h
#define GUARD_lexc_utils_h
#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include <string>

const char LEXC_JOINER_START[] = "@LEXC_JOINER.";
const char LEXC_JOINER_END[] = "@";
const char LEXC_DFN_START[] = "@LEXC_DEFINITION.";
const char LEXC_DFN_END[] = "@";

// RECODE LEXC STYLE

//! @brief Strips lexc style percent escaping from a string.
//!
//! E.g. like stripslashes() in PHP. 
std::string& stripPercents(std::string& s);

//! @brief Adds percents to SFST string to make it look like lexc style string.
//!
//! Only affects SFST symbols that have different interpretation under lexc
//! format.
std::string& addPercents(std::string& s);

//! @brief Find inner representation of given joiner name string.
std::string& joinerEncode(std::string& s);

//! @brief Format inner representation of joiner string in readable format as
//! it was in lexc source.
std::string& joinerDecode(std::string& s);

//! @brief Find inner representation of given XRE macro name.
std::string& xreDefinitionEncode(std::string& s);

//! @brief Format inner representation of macro definition string as it was in
//! lexc source.
std::string& xreDefinitionDecode(std::string& s);


// vim: set ft=cpp.doxygen:
#endif // GUARD_lexc_utils_h
