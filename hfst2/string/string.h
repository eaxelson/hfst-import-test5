//!
//! @file string.h
//!
//! @brief String to HFST conversions built on top of HFST API.
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

#ifndef GUARD_string_h
#define GUARD_string_h



#include "hfst.h"

namespace HFST
{
// basic atomic operations for UTF-8 and ASCII strings
//! @brief turns string to a key in keytable
Key stringToKey(const std::string& s, KeyTable* kt, bool addUnknown = true);

//! @brief turns string to vector encoding octets as keys.
KeyVector* stringAsciiToKeyVector(const std::string& s, KeyTable* kt, bool addUnkown = true);

//! @brief turns string to set of octets as keys.
KeySet* stringAsciiToKeySet(const std::string& s, KeyTable* kt, bool addUnknown = true);

//! @brief turns UTF-8 encoded string into vector of UTF-8 characters encoded
//! as Keys with @c stringToKey(s, kt).
KeyVector* stringUtf8ToKeyVector(const std::string& s, KeyTable* kt, bool addUnknown = true);

//! @brief finds UTF-8 characters from string and makes @a KeySet of them using
//! @c stringToKey(s, kt).
KeySet* stringUtf8ToKeySet(const std::string& s, KeyTable* kt, bool addUnknown = true);

//! @brief turn space separated string to a key vector
KeyVector* stringSeparatedToKeyVector(const std::string& s, KeyTable* kt,
		const std::string& sep, bool addUnknown = true);

//! @brief looks up string representation of key @a k.
std::string* keyToString(Key k, KeyTable* kt);

//! @brief turns key vector @a kv to string.
std::string* keyVectorToString(KeyVector* kv, KeyTable* kt);

}

namespace HWFST
{
Key stringToKey(const std::string& s, KeyTable* kt, bool addUnknown = true);
KeyVector* stringAsciiToKeyVector(const std::string& s, KeyTable* kt, bool addUnkown = true);
KeySet* stringAsciiToKeySet(const std::string& s, KeyTable* kt, bool addUnknown = true);
KeyVector* stringUtf8ToKeyVector(const std::string& s, KeyTable* kt, bool addUnknown = true);
KeySet* stringUtf8ToKeySet(const std::string& s, KeyTable* kt, bool addUnknown = true);
std::string* keyToString(Key k, KeyTable* kt);
std::string* keyVectorToString(KeyVector* kv, KeyTable* kt);
KeyVector* stringSeparatedToKeyVector(const std::string& s, KeyTable* kt,
		const std::string& sep, bool addUnknown = true);
}

// vim: set ft=cpp.doxygen:
#endif
