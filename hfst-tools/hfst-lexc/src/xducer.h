//!
//! @file xducer.h
//!
//! @brief My re-implementation of deformed bridge pattern over transducer
//! implementation, because I do not have a clue of other way to implement
//! runtime selection between namespace scoped transducer thingies.
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

#ifndef GUARD_xducer_h
#define GUARD_xducer_h
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
#include <hfst2/hfst.h>

#include "xymbol.h"

//! @brief Xducer is an OOP formulation of HFST style transducers for hfst-lexc
//! purposes.
class Xducer
{
	public:

	//! @brief Create transducer with weightedness from global defaults.
	//! This default constructor should generally be avoided outside implicit
	//! usage by STL or somesuch. A transducer created with default constructor
	//! is not usable before assigning some values with e.g. make().
	Xducer();
	//! @brief Copies the actual transducer using specific libhfst copy command.
	//! This shall make a seep copy of transducer using underlying library,
	//! unlike default copy constructor where we get shallow copy prone to 
	//! breakeage.
	//!
	//! @note the weightedness may not vary.
	Xducer& operator=(const Xducer& ref);
	//! @brief Copies the actual transducer using specific libhfst copy command.
	//! This shall make a seep copy of transducer using underlying library,
	//! unlike default copy constructor where we get shallow copy prone to 
	//! breakeage.
	//!
	//! @note the weightedness may not vary.
	Xducer(const Xducer& ref);
	//! @brief Create a transducer accepting single identity pair @a x.
	Xducer(const Xymbol& x);
	//! @brief Create a transducer accepting single pair @a xp.
	Xducer(const XymbolPair& xp);
	//! @brief Create a transducer accepting any identity pair of symbols
	//! in the set @a xs.
	Xducer(const XymbolSet& xs);
	//! @brief Create a transducer accepting any pair in the set @a xps.
	Xducer(const XymbolPairSet& xps);
	//! @brief Create a transducer accepting ordered concatenation of
	//! identity pairs of symbols in @a xv from @c xv.begin() to
	//! @c xv.end()-1.
	Xducer(const XymbolVector& xv);
	//! @brief Create a transducer accepting ordered concatenation of
	//! pairs in @a xpv from @c xpv.begin() to @c xpv.end().
	Xducer(const XymbolPairVector& xpv);
	//! @brief Calls destructing function of hfstlib to underlying
	//! transducers if it is existing and clean 
	~Xducer();
	//! @brief Whether Xducer is in weighted format.
	bool isWeighted() const;
	//! @brief Whether other Xducer is in format that may be used with this.
	bool isOperableWith(const Xducer& other) const;
	//! @brief mark Xducer unusable.
	//! This should be done invariably after destructive operation of underlying
	//! transducer and before destructor kicks in. Use only after
	//! modifying underlying HFST lib transducer in such a way that it is no
	//! longer guaranteed to be usable.
	void invalidate();
	//! @brief whether Xducer is in usable state.
	bool isValid() const;
	//! @brief Whether transducer is empty.
	bool isEmpty() const;
	//! @brief Whether transducer is a trie.
	bool isTrie() const;
	//! @brief Changes format used for xducer.
	//!
	//! Changing format invalidates all potential contents of an
	//! @a Xducer. @a setFormat does NOT convert underlying Xducer but
	//! destroys it.
	void setWeighted(bool weightedness);
	//! @brief Returns a handle to SFST transducer.
	//! When modifying returned handle, care must be taken to
	//! @a invalidate() the Xducer holding the handle.
	HFST::TransducerHandle getSfstDucer();
	//! @brief Returns a handle to openfst transducer.
	//! When modifying returned handle, care must be taken to
	//! @a invalidate() the Xducer holding the handle.
	HWFST::TransducerHandle getOpenfstDucer();
	//! @brief Copies and returns a handle to SFST transducer.
	HFST::TransducerHandle copySfstDucer() const;
	//! @brief Copies and returns a handle to openfst transducer
	HWFST::TransducerHandle copyOpenfstDucer() const;
	//! @brief Make a deep copy of an @a Xducer with attributes.
	Xducer* clone() const;
	//! @brief makes a disjunction without destroying @a other.
	Xducer& safelyDisjunct(const Xducer &other);
	//! @brief makes a conjunction without destroying @a other.
	Xducer& safelyConjunct(const Xducer &other);
	//! @brief makes a composition without destroying @a other.
	Xducer& safelyCompose(const Xducer &other);
	//! @brief makes a concatenation without destroying @a other.
	Xducer& safelyConcatenate(const Xducer &other);
	//! @brief makes a disjunction, @a other will become unusable.
	Xducer& disjunct(Xducer &other);
	//! @brief makes a fast minimal disjunction. @a other must be in trie
	//! format and will become unusable.
	Xducer& disjunctTrie(Xducer &other);
	//! @brief makes a conjunction @a other will become unusable.
	Xducer& conjunct(Xducer &other);
	//! @brief makes a composition @a other will become unusbale.
	Xducer& compose(Xducer &other);
	//! @brief makes a concatenation @a other will become unusable.
	Xducer& concatenate(Xducer &other);
	//! @brief makes a complement.
	Xducer& complement(const XymbolPairSet& ref);
	//! @brief repeats n times.
	//! Result is equal to repeat(n, n).
	Xducer& repeat(size_t n);
	//! @brief repeats at least n at most k times.
	//! For n minus, use n = 0, k = n. For n plus, use repeatAtLeast.
	Xducer& repeat(size_t n, size_t k);
	//! @brief repeats at least n, at most infinite times.
	Xducer& repeatAtLeast(size_t n);
	//! @brief repeat zero to infinite times.
	Xducer& repeatStar();
	//! @brief repeat one to infinite times.
	Xducer& repeatPlus();
	//! @brief reverses.
	Xducer& revert();
	//! @brief inverts.
	Xducer& invert();
	//! @brief discards lower language.
	Xducer& projectUpper();
	//! @brief discards upper language.
	Xducer& projectLower();
	//! @brief (re-)initialise as epsilon.
	Xducer& makeEpsilon();
	//! @brief (re-)initialise as empty transducer.
	Xducer& makeEmpty();
	//! @brief (re-)initialise as empty transducer.
	Xducer& make();
	//! @brief (re-)initialise a transducer accepting single identity pair @a x.
	Xducer& make(const Xymbol& x);
	//! @brief (re-)initialise a transducer accepting single pair @a xp.
	Xducer& make(const XymbolPair& xp);
	//! @brief (re-)initialise a transducer accepting any identity pair of
	//! symbols in the set @a xs.
	Xducer& make(const XymbolSet& xs);
	//! @brief (re-)initialise a transducer accepting any pair in the set
	//! @a xps.
	Xducer& make(const XymbolPairSet& xps);
	//! @brief (re-)initialise a transducer accepting ordered concatenation of
	//! identity pairs of symbols in @a xv from @c xv.begin() to
	//! @c xv.end()-1.
	Xducer& make(const XymbolVector& xv);
	//! @brief (re-)initialise a transducer accepting ordered concatenation of
	//! pairs in @a xpv from @c xpv.begin() to @c xpv.end().
	Xducer& make(const XymbolPairVector& xpv);
	//! @brief (re-)initialise by computing a crossproduct from set of symbols
	Xducer& crossproduct(const XymbolSet& xd);
	//! @brief replace all symbols @a x1 in transducer by @a x2.
	Xducer& substitute(const Xymbol& x1, const Xymbol& x2);
	//! @brief replace all arcs @a xp1 with @a xp2
	Xducer& substitute(const XymbolPair& xp1, const XymbolPair& xp2);
	//! @brief replace all symbols in set @a xs with @a x.
	Xducer& substitute(const XymbolSet& xs, const Xymbol& x);
	//! @brief replace all arcs in set @a xps with @a xp.
	Xducer& substitute(const XymbolPairSet& xps, const XymbolPair& xp);
	//! @brief replace an arc with a transducer
	Xducer& substitute(const XymbolPair& xp, const Xducer& x);
	//! @brief increment trie transducer with a vector.
	Xducer& addToTrie(const XymbolVector& xv);
	//! @brief increment trie transducer with a vector.
	Xducer& addToTrie(const XymbolPairVector& xpv);
	//! @brief increment trie transducer with a weighted vector.
	Xducer& addToTrie(const XymbolVector& xv, double w);
	//! @brief increment trie transducer with a weighted vector.
	Xducer& addToTrie(const XymbolPairVector& xpv, double w);
	//! @brief initialise transducer from file.
	//!
	//! Tries to handle somehow cases where file is in other format than self.
	Xducer& read(std::istream& in);
	//! @brief saves transducer to given stream
	void write(std::ostream& out = std::cout) const;
	//! @brief initialise transducer from xerox style regular expression.
	//! The complement and question mark characters are parsed against 
	//! @a kps.
	Xducer& compileFromXre(const std::string& xre, const XymbolPairSet& kps);
	//! @brief minimise transducer.
	Xducer& minimise();
	//! @brief determinise transducer.
	Xducer& determinise();
	//! @brief remove epsilons.
	Xducer& removeEpsilons();
	//! @brief set all final states to have weight @a weight.
	Xducer& setFinalWeights(const double weight);
	//! @brief lock all weights to current positions.
	//! This effectively makes weight part of the label of an arc, therefore
	//! after encoding a a:b with weight of 1 will be a1:b1 and is not same
	//! transition on either side as a:b with weight of 2.
	Xducer& encodeWeights();
	//! @brief pushes weights towards end states of transducer.
	//! Push is stable and equivalent transformation I suppose.
	Xducer& pushWeights();
	//! @brief pushes weights towards the start state of transducer.
	Xducer& pullWeights();
	//! @brief print textual format of transducer to @a out.
	const Xducer& print(std::ostream& out) const;
	//! @brief count states.
	size_t computeStateCount() const;
	//! @brief count arcs.
	size_t computeArcCount() const;
	//! @brief count paths.
	size_t computePathCount() const;
	//! @brief Create expression for debugging purposes.
	void dumpDebugString() const;
	//! @brief Compute equivalent transducer with LEXC joiners removed.
	//! The resulting transducer will have proper morphotax calculated.
	//! LEXC joiners are assumed to have form
	//! @verbatim @LEXC_JOINER.name@@endverbatim 
	//! @b and populate
	//! the initial arcs.
	//! @pre Lexc joiner removal does anything reasonable iff the transducer
	//! is formed by LexcCompiler or happens to be a minimal sub-language of
	//! @f$\bigcup_{J \in \Gamma}(J) \Sigma^{\star} \bigcup_{J \in \Gamma}(J)@f$,
	//! where @f$\Gamma \cap \Sigma = \emptyset@f$, and @f$\Gamma@f$ set of
	//! joiners and @f$\Sigma@f$ set of actual alphabets.
	Xducer& removeLexcJoiners(const Xymbol& initial, const Xymbol& final);
	//! @brief Compute equivalent transducer with flag diacritics removed.
	//! The resulting transducer will have flag diacritic combinations calculated.
	//! Flag diacritics are assumed to have form
	//! @verbatim @op.name(.value)?@ @endverbatim.
	Xducer& removeFlagDiacritics();
	private:

	HFST::TransducerHandle sfstDucer_;
	HWFST::TransducerHandle openfstDucer_;
	HFST::KeyPair* keyPair_;
	HWFST::KeyPair* weyPair_;

	bool w_;
	bool sfstValid_;
	bool openfstValid_;
	bool isTrie_;
	bool isKeyPair_;

};

#endif
// vim: set ft=cpp.doxygen:
