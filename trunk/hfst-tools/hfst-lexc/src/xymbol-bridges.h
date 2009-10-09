//! @file xymbol-bridges.h
//!
//! @brief Temporary stuff converting stuff
//!
//! @author Tommi A. Pirinen
//! 
//! @todo this should not exist at all, or exist implemented on some level in
//! the API. I do not yet understand how to extend STL containers with the
//! constructors used in here. Also, if we define HFST::Key* == HWFST::Key*
//! the need of duplicating Key handling functions disappears.

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

#ifndef GUARD_xymbol_bridges_h
#define GUARD_xymbol_bridges_h
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <set>
#include <string>
#include <vector>

#include <hfst2/hfst.h>

#if NESTED_BUILD
#include <hfst2/string/string.h>
#endif
#if !NESTED_BUILD
#include <hfst2/string.h>
#endif

#include "xymbol.h"
#include "xducer.h"

extern bool weighted;

// This legacy stuff needs converting to OOP stuff
XymbolVector* xymbolVectorFromUtf8String(const std::string& s);
XymbolSet* xymbolSetFromUtf8String(const std::string& s);
XymbolSet* xymbolSetFromXducer(Xducer& x);
// Aligning 2 vectors
XymbolPairVector* xymbolVectorsAlignLeft(const XymbolVector& upper, const XymbolVector& lower);

// Trivial conversions
XymbolPair* xymbolToIdentityXymbolPair(const Xymbol& k);
XymbolPairSet* xymbolSetToIdentityXymbolPairSet(const XymbolSet& ks);
XymbolSet* xymbolPairSetToXymbolSet(const XymbolPairSet& kps);
XymbolPairVector* xymbolVectorToIdentityXymbolPairVector(const XymbolVector& ks);

HFST::KeySet* xymbolSetToKeySet(const XymbolSet& xs);
HWFST::KeySet* xymbolSetToWeySet(const XymbolSet& xs);
HFST::KeyVector* xymbolVectorToKeyVector(const XymbolVector& xv);
HWFST::KeyVector* xymbolVectorToWeyVector(const XymbolVector& xv);
HFST::KeyPair* xymbolPairToKeyPair(const XymbolPair& xp);
HWFST::KeyPair* xymbolPairToWeyPair(const XymbolPair& xp);
HFST::KeyPairSet* xymbolPairSetToKeyPairSet(const XymbolPairSet& xps);
HWFST::KeyPairSet* xymbolPairSetToWeyPairSet(const XymbolPairSet& xps);
HFST::KeyPairVector* xymbolPairVectorToKeyPairVector(const XymbolPairVector& xpv);
HWFST::KeyPairVector* xymbolPairVectorToWeyPairVector(const XymbolPairVector& xpv);

Xymbol* keyToXymbol(HFST::Key k);
Xymbol* weyToXymbol(HWFST::Key k);
XymbolPair* keyPairToXymbolPair(HFST::KeyPair* k);
XymbolPair* weyPairToXymbolPair(HWFST::KeyPair* k);
XymbolSet* keySetToXymbolSet(HFST::KeySet* ks);
XymbolSet* weySetToXymbolSet(HWFST::KeySet* ks);
XymbolVector* keyVectorToXymbolVector(HFST::KeyVector* kv);
XymbolVector* weyVectorToXymbolVector(HWFST::KeyVector* kv);
XymbolPairSet* keyPairSetToXymbolPairSet(HFST::KeyPairSet* kps);
XymbolPairSet* weyPairSetToXymbolPairSet(HWFST::KeyPairSet* kps);
XymbolPairVector* keyPairVectorToXymbolPairVector(HFST::KeyPairVector* kpv);
XymbolPairVector* weyPairVectorToXymbolPairVector(HWFST::KeyPairVector* kpv);



#endif
// vim: set ft=cpp.doxygen:

