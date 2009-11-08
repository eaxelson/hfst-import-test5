//!
//! @file xymbol.h
//!
//! @brief My re-implementation of deformed bridge pattern over
//! symbols/keys/Characters/arcs/things/stuff or whatever concept du jour
//! for them is in HFST.
//! This is only done to remove my confusion over HFST concepts in single
//! file rather than spread it around all source code.
//!
//! @author Tommi A. Pirinen
//! 
//! @todo this should not exist at all, or exist implemented on some level in
//! the API.

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

#ifndef GUARD_xymbol_h
#define GUARD_xymbol_h
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <set>
#include <string>
#include <vector>

#include <hfst2/hfst.h>

//! @brief Xymbol is OOP representation of HFST concepts of symbol, key and
//! string for hfst-lexc purposes.
//! 
class Xymbol
{
	public:
	//! @brief Create a undefined symbol of default weightedness. This default
	//! constructor should not be used unless required by e.g. STL containers.
	Xymbol();
	//! @brief Create symbol whose string form is @a name. If @a name is not
	//! found from default key table it is created in current weightedness
	//! mode. 
	//! @note It is not well defined whether default conversion of string
	//! should be a symbol or symbol vector?
	explicit Xymbol(const std::string& name);
	//! @brief Create symbol with given name @e and key. Use only if you
	//! know that given name is assigned given key in current weightedness
	//! mode
	Xymbol(const std::string& name, long key);
	//! @brief Create symbol with given key that must be in use in current
	//! weightedness mode, or 0 for epsilon.
	explicit Xymbol(unsigned long key);
	//! @brief Make a deep copy of symbol in current weightedness mode. May
	//! transform weighted symbols to unweighted namespace and vice versa.
	Xymbol(const Xymbol& ref);

	//! @brief Make symbols sort against their almost unique numeric values
	//! only to improve performance.
	bool operator<(const Xymbol& ref) const;

	//! @brief Make symbols compare against their almost unique numeric values
	//! only.
	bool operator==(const Xymbol& rhs) const;
	//! @brief Make symbols compare against their almost unique numeric values
	//! only
	bool operator!=(const Xymbol& rhs) const;
	//! @brief Make assignment lookup uncached values
	Xymbol& operator=(const Xymbol& rhs);
	//! @brief Change symbol by numeric value. 
	//! Looks up new name as well.
	Xymbol& setKey(long key);
	//! @brief Change symbol by string representation. Looks up numeric value
	//! and creates new if non-existent.
	Xymbol& setName(const std::string& name);

	//! @brief Lookup numeric value in unweighted namespace
	HFST::Key getKey() const;
	//! @brief Lookup numeric value in weighted namespace.
	HWFST::Key getWey() const;
	//! @brief Lookup name from current namespace.
	const std::string& getName() const;
	//! @brief Lookup symbol mapping value in unweighted namespace.
	//! Should not be needed for normal operation.
	HFST::Symbol getSymbol() const;
	//! @brief Lookup symbol mapping in weighted namespace.
	//! Should not be needed for normal operation.
	HWFST::Symbol getWymbol() const;

	//! @brief Create a string representing all of symbols parameters,
	//! suitable for debugging. The format is arbitrary and may change
	//! at whim of mind.
	void dumpDebugString() const;

	private:
	bool w_;
	std::string name_;
	HFST::Key key_;
	HWFST::Key wey_;
	HFST::Symbol symbol_;
	HWFST::Symbol wymbol_;
	bool valid_;
	bool walid_;
};

//! @brief macro for epsilon constructor rhs.
#define EPSILON_XYMBOL (Xymbol(static_cast<long unsigned int>(0)))

//! @brief STL set for symbols
typedef std::set<Xymbol> XymbolSet;
//! @brief STL vector for symbols
typedef std::vector<Xymbol> XymbolVector;
//! @brief STL pair for symbols
typedef std::pair<Xymbol, Xymbol> XymbolPair;
//! @brief STL set for STL pairs of symbols
typedef std::set<XymbolPair> XymbolPairSet;
//! @brief STL vector of STL pairs of symbols
typedef std::vector<XymbolPair> XymbolPairVector;

#endif
// vim: set ft=cpp.doxygen:

