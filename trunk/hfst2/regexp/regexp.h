//!
//! @file regexp.h
//!
//! @brief Parsers for turning strings to HFST transducers. All of these are
//!		built on HFST API and may be plugged to external applications or built
//!		as part of API libraries or whatever. 
//!
//! @author Tommi A. Pirinen

//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the Licence.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GUARD_regexp_h
#define GUARD_regexp_h

#include "hfst.h"

namespace HFST
{

// Regular expression compiling
//! @brief compile Xerox style regular expression to transducer.
//! This version assumes that xre compiler has been set up already, and may be
//! used for subsequent calls.
TransducerHandle compile_xre(const char *xre);

//! @brief Setup variables for xre compilation and compile expression @a xre.
//! @param negation_pi defines set of key pairs against which negation in
//!		expression ! or ~ is interpreted.
//! @param creation_pi defines set of key pairs which is used for creating
//!		expressions using ?
//! @param kt defines KeyTable which provides existing keys for symbols in
//!		expression @a xre. Unrecognised symbols will be silently added at
//!		arbitrary keys.
TransducerHandle compile_xre(const char *xre,
	KeyPairSet* negation_pi, KeyPairSet* creation_pi,
	KeyTable* kt);
//! @brief define reg.exp macro name.
//! For each defined macro, any further compiled XRE will substitute all
//! pairs named @a label with transducer compiled from @a xre.
bool xre_add_definition(const char* label, const char* xre);
//! @brief recall results of last @a compile_xre(const char).
TransducerHandle xre_get_last_transducer();
//! @brief return current key table
KeyTable* xre_get_last_key_table();
//! @brief return set of keys found in last expressions
KeySet* xre_get_last_key_set();
//! @brief reset key table used
void xre_set_key_table(KeyTable* kt);
//! @brief reset key pair set used for negations
void xre_set_negation_key_pair_set(KeyPairSet* kps);
//! @brief reset key pair set used for creations
void xre_set_creation_key_pair_set(KeyPairSet* kps);
}

// duplicated name space stuff
namespace HWFST
{

TransducerHandle compile_xre(const char *xre);
TransducerHandle compile_xre(const char *xre,
	KeyPairSet* negation_pi, KeyPairSet* creation_pi,
	KeyTable* kt);
bool xre_add_definition(const char* label, const char* xre);
TransducerHandle xre_get_last_transducer();
HFST::KeyTable* xre_get_last_key_table();
HFST::KeySet* xre_get_last_key_set();
void xre_set_key_table(KeyTable* kt);
void xre_set_negation_key_pair_set(KeyPairSet* kps);
void xre_set_creation_key_pair_set(KeyPairSet* kps);
}
#endif

// vim: set ft=cpp.doxygen:
