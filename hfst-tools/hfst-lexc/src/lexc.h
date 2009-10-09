//! @file lexc.h
//!
//! @brief Functions for building trie representation of lexc data
//!
//! @mainpage
//! HFST-lexc contains a finite state compilater for objects that resemble
//! Xerox lexc style lexicons. This compiler is modeled in LexcCompiler class.
//! LexcCompiler tries to be a minimal implementation needed to clone original
//! lexc functionality, it supports only the methods necessary to implement
//! original lexc, e.g. deleting entries from a lexicon during compilation is
//! not implemented.
//!
//! The transducer subsystem is largely irrelevant hack I use to bridge
//! some transducer implementations to my likings. The classes Xducer and
//! Xymbol should contain familiar transducer algebra. 
//! 
//! Finally the actual lexc handling is contained within yacc and flex parsers,
//! and simple command line and readline user interfaces are provided.

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

#ifndef GUARD_lexc_h
#define GUARD_lexc_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>

#include "xducer.h"

//! @brief A compiler holding information contained in lexc style lexicons.
//! A single LexcCompiler can be extended by adding entries to it, but little
//! else can be done with it. It is sufficient to implement clone of lexc.
class LexcCompiler
{
	public:
	//! @brief create compiler for initial lexicon @c Root and
	//! final lexicon @c #.
	LexcCompiler();

	//! @brief add @a alphabet to sigma set.
	void addAlphabet(const std::string& alphabet);

	//! @brief make sigma set closed and entries extending alphabet fatal errors.
	void setClosedSigma(bool closed);

	//! @brief set current processing lexicon name to @a lexicon_name.
	void setCurrentLexiconName(const std::string& lexicon_name);

	//! @brief add entry defined by a @a entry to current lexicon, pointing to
	//! @a continuation weighing @a weight to current lexicon.
	void addStringEntry(const std::string& entry, const std::string& continuation,
			const double weight);

	//! @brief add entry defined by @a upper:@a lower, pointing to
	//! @a continuation weighing @a weight to current lexicon.
	void addStringPairEntry(const std::string& upper, const std::string& lower,
			const std::string& continuation, const double weight);

	//! @brief add entry defined by regular expression @a xre, pointing to
	//! @a continuation weighing @a weight to current lexicon.
	void addXreEntry(const std::string& xre, const std::string& continuation, 
			const double weight);

	//! @brief add macro definition named @a name matching regular expression
	//! @a xre to known xerox regular expressions.
	void addXreDefinition(const std::string& name, const std::string& xre);

	//! @brief set start lexicon's name to @a lexicon_name.
	void setInitialLexiconName(const std::string& lexicon_name);

	//! @brief set end lexicon's name to @a lexicon_name.
	void setFinalLexiconName(const std::string& lexicon_name);

	//! @brief create final usable version of current lexicons and entries.
	const Xducer& compileLexical();

	//! @brief get trie formed by current string entries
	const Xducer& getStringTrie() const;

	//! @brief get union formed by current regular expression entries
	const Xducer& getRegexpUnion() const;

	//! @brief compile morphotax to limit star union of compiled entries.
	const Xducer& compileMorphotax();

	//! @brief check that current morphotax is connected and print anomalies.
	//! Works like xerox lexc, for compatibility.
	void printConnectedness() const;

	//! @brief dumps debugging data
	void dumpDebugString() const;

	private:
	Xducer lexical_;
	Xducer stringTrie_;
	Xducer regexpUnion_;
	Xducer morphotax_;
	XymbolPairSet pi_;
	XymbolSet gamma_;
	XymbolSet sigma_;
	XymbolPair initialLexiconName_;
	XymbolPair finalContinuation_;
	XymbolPair currentLexiconName_;
	std::set<std::string> lexiconNames_;
	std::set<std::string> continuations_;
	bool closedSigma_;
	bool hasRegExps_;
	bool hasStrings_;
}
;
// vim:set ft=cpp.doxygen:
#endif

