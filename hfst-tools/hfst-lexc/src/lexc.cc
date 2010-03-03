//! @file lexc.cc
//!
//! @brief Implementation of lexc compilation.
//! 
//! @author Tommi A. Pirinen
//!

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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>

using std::string;

#if NESTED_BUILD
#include <hfst2/regexp/regexp.h>
#endif
#if !NESTED_BUILD
#include <hfst2/regexp.h>
#endif

#include "lexcio.h"
#include "lexc.h"
#include "string-munging.h"
#include "xducer.h"
#include "xymbol.h"
#include "xymbol-bridges.h"

LexcCompiler::LexcCompiler() :
	lexical_(),
	stringTrie_(),
	regexpUnion_(),
	morphotax_(),
	pi_(),
	gamma_(),
	sigma_(),
	initialLexiconName_(),
	finalContinuation_(),
	currentLexiconName_(),
	closedSigma_(false),
	hasRegExps_(false),
	hasStrings_(false)
{
	lexical_.makeEmpty();
	stringTrie_.makeEmpty();
	regexpUnion_.makeEpsilon();
	morphotax_.makeEmpty();
}

void
LexcCompiler::addAlphabet(const string& alpha)
{
	Xymbol newX = Xymbol(alpha);
	sigma_.insert(newX);
	for (XymbolSet::const_iterator x = sigma_.begin();
			x != sigma_.end();
			++x)
	{
		pi_.insert(XymbolPair(*x, newX));
		pi_.insert(XymbolPair(newX, *x));
	}
}

// Construct vector nameJoiner upper:lower contJoiner and add to trie
void
LexcCompiler::addStringEntry(const string& data,
		const string& continuation, double weight)
{
	string encodedCont = string(continuation);
	encodedCont = joinerEncode(encodedCont);
	XymbolPair contJoiner = 
		XymbolPair(Xymbol(encodedCont), Xymbol(encodedCont));
	gamma_.insert(Xymbol(encodedCont));
	continuations_.insert(continuation);
	lexc_timer_start("string-compile");
	XymbolVector* dataVec = xymbolVectorFromUtf8String(data);
	lexc_timer_end("string-compile");
	lexc_timer_start("fill-sigma");
	unsigned int ats = 0;
	unsigned int lbr = 0;
	for (XymbolVector::iterator x = dataVec->begin();
			x != dataVec->end(); ++x)
	{
		if (sigma_.find(*x) == sigma_.end())
		{
			lexc_printf(PRINT_DEBUG, 0, "Found Xymbol %s from %s\n",
					x->getName().c_str(),
					data.c_str());
			addAlphabet(x->getName());
		}
		if (x->getName() == "@")
		{
			ats++;
			if (ats >= 2)
			{
				lexc_printf(PRINT_WARNING, -1, "Two @s in %s do not form "
						"an special character\n", data.c_str());
			}
		}
		else if (x->getName() == "[")
		{
			lbr++;
		}
		else if (x->getName() == "]")
		{
			if (lbr > 0)
			{
				lexc_printf(PRINT_WARNING, -1, "Square brackets in %s do not "
						"form a tag\n", data.c_str());
			}
		}
	}
	lexc_timer_end("fill-sigma");
	XymbolPairVector* morphemeVec = 
		xymbolVectorToIdentityXymbolPairVector(*dataVec);
	morphemeVec->push_back(contJoiner);
	morphemeVec->insert(morphemeVec->begin(), currentLexiconName_);
	lexc_timer_start("trie-union");
	if (weighted && (weight != 0))
	{
		stringTrie_.addToTrie(*morphemeVec, weight);
	}
	else
	{
		stringTrie_.addToTrie(*morphemeVec);
	}
	lexc_timer_end("trie-union");
	delete dataVec;
	delete morphemeVec;
	hasStrings_ = true;
}

void
LexcCompiler::addStringPairEntry(const string& upper, const string& lower,
		const string& continuation, double weight)
{
	string encodedCont = string(continuation);
	encodedCont = joinerEncode(encodedCont);
	XymbolPair contJoiner = 
		XymbolPair(Xymbol(encodedCont), Xymbol(encodedCont));
	gamma_.insert(Xymbol(encodedCont));
	continuations_.insert(continuation);
	lexc_timer_start("string-compile");
	XymbolVector* upperVec = xymbolVectorFromUtf8String(upper);
	lexc_timer_end("string-compile");
	lexc_timer_start("fill-sigma");
	unsigned int ats = 0;
	unsigned int lbr = 0;
	for (XymbolVector::iterator x = upperVec->begin();
			x != upperVec->end(); ++x)
	{
		if (sigma_.find(*x) == sigma_.end())
		{
			lexc_printf(PRINT_DEBUG, 0, "Found Xymbol %s from %s\n",
					x->getName().c_str(),
					upper.c_str());
			addAlphabet(x->getName());
		}
		if (x->getName() == "@")
		{
			ats++;
			if (ats >= 2)
			{
				lexc_printf(PRINT_WARNING, -1, "Two @s in %s do not form "
						"an special character\n", upper.c_str());
			}
		}
		else if (x->getName() == "[")
		{
			lbr++;
		}
		else if (x->getName() == "]")
		{
			if (lbr > 0)
			{
				lexc_printf(PRINT_WARNING, -1, "Square brackets in %s do not "
						"form a tag\n", upper.c_str());
			}
		}
	}
	lexc_timer_end("fill-sigma");
	// same for lower
	lexc_timer_start("string-compile");
	XymbolVector* lowerVec = xymbolVectorFromUtf8String(lower);
	lexc_timer_end("string-compile");
	lexc_timer_start("fill-sigma");
	ats = 0;
	lbr = 0;
	for (XymbolVector::iterator x = lowerVec->begin();
			x != lowerVec->end(); ++x)
	{
		if (sigma_.find(*x) == sigma_.end())
		{
			lexc_printf(PRINT_DEBUG, 0,  "Found Xymbol %s from %s\n",
					x->getName().c_str(),
					lower.c_str());
			addAlphabet(x->getName());
		}
		if (x->getName() == "@")
		{
			ats++;
			if (ats >= 2)
			{
				lexc_printf(PRINT_WARNING, -1, "Two @s in %s do not form "
						"an special character\n", lower.c_str());
			}
		}
		else if (x->getName() == "[")
		{
			lbr++;
		}
		else if (x->getName() == "]")
		{
			if (lbr > 0)
			{
				lexc_printf(PRINT_WARNING, -1, "Square brackets in %s do not "
						"form a tag\n", lower.c_str());
			}
		}
	}
	lexc_timer_end("fill-sigma");
	XymbolPairVector* morphemeVec =
		xymbolVectorsAlignLeft(*upperVec, *lowerVec);
	morphemeVec->push_back(contJoiner);
	morphemeVec->insert(morphemeVec->begin(), currentLexiconName_);
	lexc_timer_start("trie-union");
	if (weighted && (weight != 0))
	{
		stringTrie_.addToTrie(*morphemeVec, weight);
	}
	else
	{
		stringTrie_.addToTrie(*morphemeVec);
	}
	lexc_timer_end("trie-union");
	delete upperVec;
	delete lowerVec;
	delete morphemeVec;
	hasStrings_ = true;
}

// Construct transducer nameJoiner XRE contJoiner and add to trie
void
LexcCompiler::addXreEntry(const string& regexp, const string& continuation,
		double weight)
{
	string encodedCont = string(continuation);
	encodedCont = joinerEncode(encodedCont);
	XymbolPair contJoiner = XymbolPair(Xymbol(encodedCont), Xymbol(encodedCont));
	gamma_.insert(Xymbol(encodedCont));
	continuations_.insert(continuation);
	Xducer xrecomp = Xducer();
	lexc_timer_start("xre-compile");
	xrecomp.compileFromXre(regexp, pi_);
	lexc_timer_end("xre-compile");
	if (weighted && (weight != 0))
	{
		xrecomp.setFinalWeights(weight);
	}
	lexc_timer_start("determinise");
	xrecomp.determinise();
	lexc_timer_end("determinise");
	lexc_timer_start("minimise");
	xrecomp.minimise();
	lexc_timer_end("minimise");
	XymbolSet* compsyms = xymbolSetFromXducer(xrecomp);
	lexc_timer_start("fill-sigma");
	for (XymbolSet::iterator x = compsyms->begin();
			x != compsyms->end(); ++x)
	{
		if (sigma_.find(*x) == sigma_.end())
		{
			lexc_printf(PRINT_DEBUG, 0, "Found Xymbol %s from %s\n",
					x->getName().c_str(),
					regexp.c_str());
			addAlphabet(x->getName());
		}
	}
	lexc_timer_end("fill-sigma");
	Xducer entry = Xducer(currentLexiconName_);
	Xducer endJoiner = Xducer(contJoiner);
	entry.concatenate(xrecomp).concatenate(endJoiner);
	lexc_timer_start("determinise");
	entry.determinise();
	lexc_timer_end("determinise");
	lexc_timer_start("minimise");
	entry.minimise();
	lexc_timer_end("minimise");
	lexc_timer_start("xre-union");
	regexpUnion_.disjunct(entry);
	lexc_timer_end("xre-union");
	lexc_timer_start("determinise");
	regexpUnion_.determinise();
	lexc_timer_end("determinise");
	lexc_timer_start("minimise");
	regexpUnion_.minimise();
	lexc_timer_end("minimise");
	hasRegExps_ = true;
}

void
LexcCompiler::addXreDefinition(const string& definition_name, const string& xre)
{
	// collect implicit characters
	Xducer regcomp = Xducer();
	lexc_timer_start("xre-compile");
	regcomp.compileFromXre(xre, pi_);
	lexc_timer_end("xre-compile");
	XymbolSet* compSyms = xymbolSetFromXducer(regcomp);
	lexc_timer_start("fill-sigma");
	for (XymbolSet::iterator x = compSyms->begin();
		 x != compSyms->end(); ++x)
	{
		addAlphabet(x->getName());
	}
	lexc_timer_end("fill-sigma");
	if (weighted)
	{
		HWFST::xre_add_definition(definition_name.c_str(), xre.c_str());
	}
	else
	{
		HFST::xre_add_definition(definition_name.c_str(), xre.c_str());
	}
}

void
LexcCompiler::setCurrentLexiconName(const string& lexiconName)
{
	string encodedName = string(lexiconName);
	encodedName = joinerEncode(encodedName);
	currentLexiconName_ = XymbolPair(Xymbol(encodedName),
		   	Xymbol(encodedName));
	gamma_.insert(Xymbol(encodedName));
	lexiconNames_.insert(lexiconName);
}

void
LexcCompiler::setInitialLexiconName(const string& lexiconName)
{
	string encodedName = string(lexiconName);
	encodedName = joinerEncode(encodedName);
	initialLexiconName_ =
		XymbolPair(Xymbol(encodedName), Xymbol(encodedName));
	gamma_.insert(Xymbol(encodedName));
	lexiconNames_.insert(lexiconName);
	continuations_.insert(lexiconName);
}

void 
LexcCompiler::setFinalLexiconName(const string& lexiconName)
{
	string encodedName = string(lexiconName);
	encodedName = joinerEncode(encodedName);
	finalContinuation_ = XymbolPair(Xymbol(encodedName), Xymbol(encodedName));
	gamma_.insert(Xymbol(encodedName));
	lexiconNames_.insert(lexiconName);
	continuations_.insert(lexiconName);
}

const Xducer&
LexcCompiler::getStringTrie() const
{
	return stringTrie_;
}

const Xducer&
LexcCompiler::getRegexpUnion() const
{
	return regexpUnion_;
}

const Xducer&
LexcCompiler::compileMorphotax()
{
	Xducer initial = Xducer(initialLexiconName_);
	Xducer sigmastar = Xducer(sigma_).repeatStar();
	Xducer contcont;
	contcont.makeEmpty();
	for (XymbolSet::const_iterator x = gamma_.begin();
			x != gamma_.end();
			++x)
	{
		Xducer contpair = Xducer(XymbolPair(*x, *x)).repeat(2);
		lexc_timer_start("minimise");
		contcont.disjunct(contpair).minimise();
		lexc_timer_end("minimise");
	}
	Xducer contOrLetters = contcont.disjunct(sigmastar).repeatStar();
	lexc_timer_start("minimise");
	contOrLetters.minimise();
	lexc_timer_end("minimise");
	Xducer final = Xducer(finalContinuation_);
	morphotax_.makeEpsilon().concatenate(initial);
	morphotax_.concatenate(contOrLetters).concatenate(final);
	lexc_timer_start("minimise");
	morphotax_.minimise();
	lexc_timer_end("minimise");
	lexc_xducer_printf(morphotax_, "Morphotax\n");
	return morphotax_;
}

const Xducer&
LexcCompiler::compileLexical()
{
	lexc_printf(PRINT_XEROXLIKE, 0, "Building lexicon...");
	
	if (verbosity & PRINT_DEBUG)
	{
		lexc_xymbol_set_printf(sigma_, "sigma\n");
		lexc_xymbol_set_printf(gamma_, "gamma\n");
	}
	printConnectedness();
	lexical_.makeEmpty();
	if (hasStrings_)
	{
		stringTrie_.substitute(Xymbol("0"), Xymbol("@0@"));
		stringTrie_.substitute(Xymbol("@ZERO@"), Xymbol("0"));
		for (XymbolSet::const_iterator x = sigma_.begin();
				x != sigma_.end();
				++x)
		{
			stringTrie_.substitute(XymbolPair(*x, Xymbol("#")),
				XymbolPair(*x, Xymbol("@#@")));
		}
		lexc_xducer_printf(stringTrie_, "Strings trie\n");
		lexical_.disjunct(stringTrie_);
	}
	if (hasRegExps_)
	{
		lexc_xducer_printf(regexpUnion_, "RegExps\n");
		lexical_.disjunct(regexpUnion_);
	}
	lexc_printf(PRINT_VERBOSE, 0, "Determinizing... ");
	lexc_timer_start("determinise");
	lexical_.determinise();
	lexc_timer_end("determinise");
	lexc_xducer_printf(lexical_, "Strings or RegExps\n");
	// for each initial joiner target find final joiner and attach
	lexc_printf(PRINT_VERBOSE, 0, "Rebuilding... ");
	lexc_timer_start("morphotaxing");
	lexical_.removeLexcJoiners(initialLexiconName_.first,
			finalContinuation_.first);
	lexc_timer_end("morphotaxing");
	lexc_printf(PRINT_XEROXLIKE | PRINT_VERBOSE, 0, "Minimizing...");
	lexc_timer_start("determinise");
	lexical_.determinise();
	lexc_timer_end("determinise");
	lexc_timer_start("minimise");
	lexical_.minimise();
	lexc_timer_end("minimise");
	lexc_xducer_printf(lexical_, "Morphotaxed\n");
	lexc_printf(PRINT_XEROXLIKE, 0, "Done!\n");
	return lexical_;
}

void
LexcCompiler::dumpDebugString() const
{
	string s = "LexcCompiler: {";
	s += "sigma: ";
	for (XymbolSet::const_iterator x = sigma_.begin();
			x != sigma_.end(); ++x)
	{
		s += x->getName();
		s += " ";
	}
	s += "gamma: ";
	for (XymbolSet::const_iterator x = gamma_.begin();
			x != gamma_.end(); ++x)
	{
		s += x->getName();
		s += " ";
	}
	fprintf(stderr, "%s", s.c_str());
}

void
LexcCompiler::printConnectedness() const
{
	if ((verbosity & (PRINT_WARNING | PRINT_XEROXLIKE)) && 
			(lexiconNames_ != continuations_))
	{
		vector<string> lexMinusCont = vector<string>(lexiconNames_.size());
		vector<string> contMinusLex = vector<string>(continuations_.size());
		vector<string>::iterator lexMinusContEnd = set_difference(
				lexiconNames_.begin(), lexiconNames_.end(),
				continuations_.begin(), continuations_.end(),
				lexMinusCont.begin());
		vector<string>::iterator contMinusLexEnd = set_difference(
				continuations_.begin(), continuations_.end(),
				lexiconNames_.begin(), lexiconNames_.end(),
				contMinusLex.begin());
		if (contMinusLexEnd - contMinusLex.begin() > 0)
		{
			lexc_printf(PRINT_WARNING, 8, "Continuation classes were "
					"used without corresponding lexicon defined\n");
			if (verbosity & PRINT_VERBOSE)
			{
				lexc_print_list_start("Entries with these continuations "
						"have been DISCARDED");
			}
			for (vector<string>::iterator s = contMinusLex.begin();
					s != contMinusLexEnd; ++s)
			{
				if (verbosity & PRINT_VERBOSE)
				{
					lexc_list_printf("%s", s->c_str());
				}
				lexc_printf(PRINT_XEROXLIKE, 0, 
						"*** ERROR: Sublexicon is mentioned but not defined."
						" (%s) ***\n\n", s->c_str());
			}
			if (verbosity & PRINT_VERBOSE)
			{
				lexc_print_list_end("");
			}
		}
		if (lexMinusContEnd - lexMinusCont.begin() > 0)
		{
			lexc_printf(PRINT_WARNING, 7, "Lexicon names defined but not "
			   "referenced\n");
			if (verbosity & PRINT_VERBOSE)
			{
				lexc_print_list_start("These lexicons have been discarded");
			}
			else if (verbosity & PRINT_XEROXLIKE)
			{
				lexc_print_list_start("Sublexicons defined but not used");
			}
			for (vector<string>::iterator s = lexMinusCont.begin();
					s != lexMinusContEnd; ++s)
			{
				if (verbosity & (PRINT_VERBOSE | PRINT_XEROXLIKE))
				{
					lexc_list_printf("%s", s->c_str());
				}
			}
			if (verbosity & (PRINT_VERBOSE | PRINT_XEROXLIKE))
			{
				lexc_print_list_end("");
			}
		}
	}
}

// vim: set ft=cpp.doxygen:

