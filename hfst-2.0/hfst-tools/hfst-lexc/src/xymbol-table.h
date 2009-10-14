//! @file xymbol-table.h
//! @brief Hack around the symbol tables.
//! @todo should be replaced with proper oop implementation I suppose.
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

#ifndef GUARD_xymbol_table_h
#define GUARD_xymbol_table_h
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <hfst2/hfst.h>

// Icky global stuff
//! @brief All keys and symbols incl. tokeniser input and joiners.
extern HFST::KeyTable* magicKeyTableWithEverything;
extern HWFST::KeyTable* wagicKeyTableWithEverything;

//! @brief Initialise all globals, special keys and stuff.
void initialiseLexcSymbolTablesAndSets();
//! @brief Clean up all globals, special keys and stuff.
void destructLexcSymbolTablesAndSets();


// vim: set ft=cpp.doxygen:
#endif // GUARD_key_utils_h

//
