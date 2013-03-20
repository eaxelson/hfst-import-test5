//! @file LexcCompiler.cc
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
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using std::string;
using std::map;
using std::set;
using std::pair;
using std::vector;
using std::set_difference;

#include "LexcCompiler.h"
#include "HfstTransducer.h"
#include "XreCompiler.h"
#include "lexc-utils.h"
#include "lexc-parser.h"
#include "xre_utils.h"

using hfst::HfstTransducer;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransition;
using hfst::ImplementationType;
using hfst::xre::XreCompiler;

// stupid flex and yacc
extern FILE* hlexcin;
extern int hlexcparse();
extern int hlexcnerrs;

#ifndef DEBUG_MAIN

namespace hfst { namespace lexc {

LexcCompiler* lexc_ = 0;

LexcCompiler::LexcCompiler() :
    quiet_(false),
    verbose_(false),
    format_(TROPICAL_OPENFST_TYPE),
    xre_(TROPICAL_OPENFST_TYPE),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0),
    parseErrors_(false)
{}

LexcCompiler::LexcCompiler(ImplementationType impl) :
    quiet_(false),
    verbose_(false),
    format_(impl),
    xre_(impl),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0),
    parseErrors_(false)
{
    tokenizer_.add_multichar_symbol("@0@");
    tokenizer_.add_multichar_symbol("@ZERO@");
    string hash("#");
    lexiconNames_.insert(hash);
    tokenizer_.add_multichar_symbol(joinerEncode(hash));
}


LexcCompiler& LexcCompiler::parse(FILE* infile)
{
    lexc_ = this;
    if (infile == stdin)
      {
        hfst::lexc::set_infile_name("<stdin>");
      }
    else
      {
        hfst::lexc::set_infile_name("<unnamed>");
      }
    hlexcin = infile;
    hlexcparse();
    if (hlexcnerrs > 0)
      {
        parseErrors_ = true;
      }
    return *this;
}

LexcCompiler& LexcCompiler::parse(const char* filename)
{
    lexc_ = this;
    hfst::lexc::set_infile_name(filename);
    hlexcin = fopen(filename, "r");
    if (hlexcin == NULL)
      {
        fprintf(stderr, "could not open %s for reading\n", filename);
        parseErrors_ = true;
        return *this;
      }
    hlexcparse();
    if (hlexcnerrs > 0)
      {
        parseErrors_ = true;
      }
    return *this;
}

LexcCompiler&
LexcCompiler::setVerbosity(bool verbose)
{
    quiet_ = !verbose;
    verbose_ = verbose;
    return *this;
}

LexcCompiler&
LexcCompiler::addAlphabet(const string& alpha)
{
    tokenizer_.add_multichar_symbol(alpha);
    return *this;
}

// Construct vector nameJoiner upper:lower contJoiner and add to trie
LexcCompiler&
LexcCompiler::addStringEntry(const string& data,
        const string& continuation, double weight)
{
    currentEntries_++;
    totalEntries_++;
    continuations_.insert(continuation);
    string encodedCont = string(continuation);
    encodedCont = flagJoinerEncode(encodedCont, false);
    tokenizer_.add_multichar_symbol(encodedCont);
    HfstTransducer newPath(data + encodedCont, tokenizer_, format_);
    if (weight != 0)
      {
        newPath.set_final_weights(weight);
      }
    if (stringTries_.find(currentLexiconName_) == stringTries_.end())
      {
        stringTries_.insert(pair<string,HfstTransducer>(currentLexiconName_,
                                                        HfstTransducer(format_)));
      }
    stringTries_[currentLexiconName_].disjunct(newPath);
    if (!quiet_)
      {
        if ((currentEntries_ % 10000) == 0)
          {
            fprintf(stderr, SIZE_T_SPECIFIER "...", currentEntries_);
          }
      }
    return *this;
}

LexcCompiler&
LexcCompiler::addStringPairEntry(const string& upper, const string& lower,
        const string& continuation, double weight)
{
    currentEntries_++;
    totalEntries_++;
    continuations_.insert(continuation);
    string encodedCont = string(continuation);
    encodedCont = flagJoinerEncode(encodedCont, false);
    tokenizer_.add_multichar_symbol(encodedCont);
    HfstTransducer newPath(upper + encodedCont, lower + encodedCont,
                           tokenizer_, format_);
    if (weight != 0)
      {
        newPath.set_final_weights(weight);
      }
    if (stringTries_.find(currentLexiconName_) == stringTries_.end())
      {
        stringTries_.insert(pair<string,HfstTransducer>(currentLexiconName_,
                                                        HfstTransducer(format_)));
      }
    stringTries_[currentLexiconName_].disjunct(newPath);
    if (!quiet_)
      {
        if ((currentEntries_ % 10000) == 0)
          {
            fprintf(stderr, SIZE_T_SPECIFIER "...", currentEntries_);
          }
      }
    return *this;
}

// Construct transducer nameJoiner XRE contJoiner and add to trie
LexcCompiler&
LexcCompiler::addXreEntry(const string& regexp, const string& continuation,
        double weight)
{
    currentEntries_++;
    totalEntries_++;
    continuations_.insert(continuation);
    string encodedCont = string(continuation);
    encodedCont = flagJoinerEncode(encodedCont, false);
    tokenizer_.add_multichar_symbol(encodedCont);
    char* xre_encoded = hfst::xre::add_percents(encodedCont.c_str());
    HfstTransducer* newPaths = xre_.compile(regexp + " "  + string(xre_encoded));
    if (weight != 0)
      {
        newPaths->set_final_weights(weight);
      }
    newPaths->minimize();
    // FIXME: add all implicit chars to multichar symbols
    if (regexps_.find(currentLexiconName_) == regexps_.end())
      {
        regexps_.insert(pair<string,HfstTransducer>(currentLexiconName_,
                                                    HfstTransducer(format_)));
      }
    regexps_[currentLexiconName_].disjunct(*newPaths).minimize();
    if (!quiet_)
      {
        if ((currentEntries_ % 10000) == 0)
          {
            fprintf(stderr, SIZE_T_SPECIFIER "...", currentEntries_);
          }
      }
    return *this;
}

LexcCompiler&
LexcCompiler::addXreDefinition(const string& definition_name, const string& xre)
{
    // FIXME: collect implicit characters
    xre_.define(definition_name, xre);
    if (!quiet_)
      {
        fprintf(stderr,
            "Defined '%s': ? Kb., ? states, ? arcs, ? paths.\n",
            definition_name.c_str());
      }
    return *this;
}

LexcCompiler&
LexcCompiler::setCurrentLexiconName(const string& lexiconName)
{
    static bool firstLexicon = true;
    currentLexiconName_ = lexiconName;
    lexiconNames_.insert(lexiconName);
    string encodedName(lexiconName);
    flagJoinerEncode(encodedName, false);
    tokenizer_.add_multichar_symbol(encodedName);
    if (!quiet_)
      {
        if ((firstLexicon) && (lexiconName == "Root"))
          {
            setInitialLexiconName(lexiconName);
          }
        else if ((firstLexicon) && (lexiconName != "Root"))
          {
            fprintf(stderr, "first lexicon is not named Root\n");
            setInitialLexiconName(lexiconName);
          }
        else if ((!firstLexicon) && (lexiconName == "Root"))
          {
            fprintf(stderr, "Root is not first the first lexicon\n");
            setInitialLexiconName(lexiconName);
          }
        if (!firstLexicon)
          {
            fprintf(stderr, SIZE_T_SPECIFIER " ", currentEntries_);
          }
        fprintf(stderr, "%s...", lexiconName.c_str());
        firstLexicon = false;
      }
    currentEntries_ = 0;
    return *this;
}

LexcCompiler&
LexcCompiler::setInitialLexiconName(const string& lexiconName)
{
    initialLexiconName_ = lexiconName;
    lexiconNames_.insert(lexiconName);
    // for connectedness calculation:
    continuations_.insert(lexiconName);
    return *this;
}

HfstTransducer*
LexcCompiler::compileLexical()
  {
    if (parseErrors_)
      {
        return 0;
      }
    printConnectedness();
    HfstTransducer lexicons(format_);
    // combine tries with reg.exps and minimize
    if (verbose_)
      {
        fprintf(stderr, "Disjuncting lexicons... ");
      }
    for (set<string>::const_iterator s = lexiconNames_.begin();
         s != lexiconNames_.end();
         ++s)
      {
        if (*s == "#")
          {
            continue;
          }
        if (verbose_)
          {
            fprintf(stderr, "%s", s->c_str());
          }
        string joinerEnc = *s;
        flagJoinerEncode(joinerEnc, true);
        HfstTransducer leftJoiner(joinerEnc, joinerEnc, format_);
        HfstTransducer lexicon(format_);
        if (stringTries_.find(*s) != stringTries_.end())
          {
            lexicon.disjunct(stringTries_[*s]);
          }
        if (regexps_.find(*s) != regexps_.end())
          {
            lexicon.disjunct(regexps_[*s]);
          }
        lexicon = leftJoiner.concatenate(lexicon).minimize();
        if (verbose_)
          {
            fprintf(stderr, " minimising... ");
          }
        lexicons.disjunct(lexicon).minimize();
      }
    // repeat star to overgenerate
    lexicons.repeat_star().minimize();
    if (verbose_)
      {
        fprintf(stderr, "\n" "calculating correct lexicon combinations...");
      }
#if WANT_OLD_JOINERS
    for (set<string>::const_iterator s = lexiconNames_.begin();
         s != lexiconNames_.end();
         ++s)
      {
        if (*s == "#")
          {
            // # doesn't form pairs, only works at end
            continue;
          }
        if (*s == initialLexiconName_)
          {
            // we compose and remove root lexicon last to avoid difficulties
            // with its dual meaning
            continue;
          }
        if (verbose_)
          {
            fprintf(stderr, "%s -> %s -> #, ", initialLexiconName_.c_str(),
                    s->c_str());
          }
        string startEnc = initialLexiconName_;
        flagJoinerEncode(startEnc);
        HfstTransducer start(startEnc, startEnc, format_);
        string joinerEnc = *s;
        joinerEncode(joinerEnc);
        HfstTransducer joiner(joinerEnc, joinerEnc, format_);
        string endEnc = "#";
        joinerEncode(endEnc);
        HfstTransducer end(endEnc, endEnc, format_);
        HfstTransducer sigmaStar("@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@",
                                 format_);
        sigmaStar = sigmaStar.subtract(start).subtract(end).subtract(joiner);
        sigmaStar.repeat_star();
        HfstTransducer joinerPair = joiner.repeat_n(2);
        HfstTransducer morphotax = sigmaStar.disjunct(joinerPair);
        morphotax.repeat_star();
        morphotax = start.concatenate(morphotax).concatenate(end).minimize();
        lexicons = lexicons.compose(morphotax);
        lexicons.substitute(joinerEnc, "@_EPSILON_SYMBOL_@").minimize();
      }
      {
        // now same for initial lexicon
        string startEnc = initialLexiconName_;
        joinerEncode(startEnc);
        HfstTransducer start(startEnc, startEnc, format_);
        HfstTransducer joiner(startEnc, startEnc, format_);
        string endEnc = "#";
        joinerEncode(endEnc);
        HfstTransducer end(endEnc, endEnc, format_);
        HfstTransducer sigmaStar("@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@",
                                 format_);
        sigmaStar = sigmaStar.subtract(start).subtract(end).subtract(joiner);
        sigmaStar.repeat_star();
        HfstTransducer joinerPair = joiner.repeat_n(2);
        HfstTransducer morphotax = sigmaStar.disjunct(joinerPair);
        morphotax.repeat_star();
        morphotax = start.concatenate(morphotax).concatenate(end).minimize();
        lexicons = lexicons.compose(morphotax);
        lexicons.substitute(startEnc, "@_EPSILON_SYMBOL_@").minimize();
      }
    string endEnc = "#";
    joinerEncode(endEnc);
    lexicons.substitute(endEnc, "@_EPSILON_SYMBOL_@");
#endif
    lexicons.substitute("@ZERO@", "0");
    HfstTransducer* rv = new HfstTransducer(lexicons);
    rv->minimize();
    return rv;
}


const LexcCompiler&
LexcCompiler::printConnectedness() const
{
    if (!quiet_ && (lexiconNames_ != continuations_))
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
            for (vector<string>::iterator s = contMinusLex.begin();
                    s != contMinusLexEnd; ++s)
            {
                fprintf(stderr,
                    "*** ERROR: Sublexicon is mentioned but not defined."
                    " (%s) ***\n\n", s->c_str());
            }
        }
        if (lexMinusContEnd - lexMinusCont.begin() > 0)
        {
            fprintf(stderr, "Sublexicons defined but not used\n");
            for (vector<string>::iterator s = lexMinusCont.begin();
                    s != lexMinusContEnd; ++s)
            {
                fprintf(stderr, "%s ", s->c_str());
            }
            fprintf(stderr, "\n");
        }
    }
    return *this;
}

} }

#else
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace hfst;
using hfst::lexc::LexcCompiler;

int
main(int argc, char** argv)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "constructors: ";
    std::cout << " (default)...";
    LexcCompiler lexcDefault();
#if HAVE_SFST
    std::cout << " (SFST)...";
    LexcCompiler lexcSfst(SFST_TYPE);
#endif
#if HAVE_OPENFST
    std::cout << " (OpenFST)...";
    LexcCompiler lexcOfst(TROPICAL_OPENFST_TYPE);
#endif
#if HAVE_FOMA
    std::cout << " (foma)...";
    LexcCompiler lexcFoma(FOMA_TYPE);
#endif
    std::cout << std::endl << "set verbose:";
#if HAVE_SFST
    lexcSfst.setVerbosity(true);
    lexcSfst.setVerbosity(false);
#endif
#if HAVE_OFST
    lexcOfst.setVerbosity(true);
    lexcOfst.setVerbosity(false);
#endif
#if HAVE_FOMA
    lexcFoma.setVerbosity(true);
    lexcFoma.setVerbosity(false);
#endif
    FILE* existence_check = fopen("LexcCompiler_test.lexc", "r");
    if (existence_check == NULL)
      {
        existence_check = fopen("LexcCompiler_test.lexc", "w");
        assert(existence_check != NULL);
        fprintf(existence_check, "LEXICON Root\ncat # ;\ndog Plural ;\n");
        fclose(existence_check);
      }
    existence_check = fopen("LexcCompiler_test2.lexc", "r");
    if (existence_check == NULL)
      {
        existence_check = fopen("LexcCompiler_test2.lexc", "w");
        assert(existence_check != NULL);
        fprintf(existence_check, "LEXICON Plural\ns # ;\n");
        fclose(existence_check);
      }
    std::cout << std::endl << "parsing: ";
#if HAVE_SFST
    std::cout << "sfst parse(FILE)...";
    FILE* sfstFile = fopen("LexcCompiler_test.lexc", "r");
    lexcSfst.parse(sfstFile);
    fclose(sfstFile);
    std::cout << "parse(filename)...";
    lexcSfst.parse("LexcCompiler_test2.lexc");
#endif
#if HAVE_OPENFST
    std::cout << "ofst parse(FILE)...";
    FILE* ofstFile = fopen("LexcCompiler_test.lexc", "r");
    lexcOfst.parse(ofstFile);
    fclose(ofstFile);
    std::cout << "parse(filename)...";
    lexcOfst.parse("LexcCompiler_test2.lexc");
#endif
#if HAVE_FOMA
    std::cout << "foma parse(FILE)...";
    FILE* fomaFile = fopen("LexcCompiler_test.lexc", "r");
    lexcFoma.parse(fomaFile);
    fclose(fomaFile);
    std::cout << "parse(filename)...";
    lexcFoma.parse("LexcCompiler_test2.lexc");
#endif
      
    std::cout << std::endl << "add multichars:";
#if HAVE_SFST
    lexcSfst.addAlphabet("foo");
    lexcSfst.addAlphabet("bar");
#endif
#if HAVE_OFST
    lexcOfst.addAlphabet("foo");
    lexcOfst.addAlphabet("bar");
#endif
#if HAVE_FOMA
    lexcFoma.addAlphabet("foo");
    lexcFoma.addAlphabet("bar");
#endif
    std::cout << std::endl << "set lexicon name:";
#if HAVE_SFST
    lexcSfst.setCurrentLexiconName("Root");
#endif
#if HAVE_OFST
    lexcOfst.setCurrentLexiconName("Root");
#endif
#if HAVE_FOMA
    lexcFoma.setCurrentLexiconName("Root");
#endif
    std::cout << std::endl << "add entries:";
#if HAVE_SFST
    std::cout << " sfst string(dog, #)...";
    lexcSfst.addStringEntry("dog", "#", 0);
    std::cout << " sfst string(banana, apple, #)...";
    lexcSfst.addStringPairEntry("banana", "apple", "#", 0);
    std::cout << " sfst xre(f i:o 0:u g h t, #)...";
    lexcSfst.addXreEntry("f i:o 0:u g h t", "#", 0);
#endif
#if HAVE_OFST
    std::cout << " ofst string(dog, #)...";
    lexcOfst.addStringEntry("dog", "#", 0);
    std::cout << " ofst string(banana, apple, #)...";
    lexcOfst.addStringPairEntry("banana", "apple", "#", 0);
    std::cout << " ofst xre(f i:o 0:u g h t, #)...";
    lexcOfst.addXreEntry("f i:o 0:u g h t", "#", 0);
#endif
#if HAVE_FOMA
    std::cout << " foma string(dog, #)...";
    lexcFoma.addStringEntry("dog", "#", 0);
    std::cout << " foma string(banana, apple, #)...";
    lexcFoma.addStringPairEntry("banana", "apple", "#", 0);
    std::cout << " foma xre(f i:o 0:u g h t, #)...";
    lexcFoma.addXreEntry("f i:o 0:u g h t", "#", 0);
#endif
    std::cout << std::endl << "add definitions:";
#if HAVE_SFST
    std::cout << " sfst define(vowels, a | e | i | o | u | y)...";
    lexcSfst.addXreDefinition("Vowels", "a | e | i | o | u | y");
#endif
#if HAVE_OFST
    std::cout << " ofst define(vowels, a | e | i | o | u | y)...";
    lexcOfst.addXreDefinition("Vowels", "a | e | i | o | u | y");
#endif
#if HAVE_FOMA
    std::cout << " foma define(vowels, a | e | i | o | u | y)...";
    lexcFoma.addXreDefinition("Vowels", "a | e | i | o | u | y");
#endif
    std::cout << std::endl << "set initial lexicon name:";
#if HAVE_SFST
    std::cout << " sfst initial(Root)...";
    lexcSfst.setInitialLexiconName("Root");
#endif
#if HAVE_OFST
    std::cout << " ofst initial(Root)...";
    lexcOfst.setInitialLexiconName("Root");
#endif
#if HAVE_FOMA
    std::cout << " foma initial(Root)...";
    lexcFoma.setInitialLexiconName("Root");
#endif
    std::cout << std::endl << "compile:";
#if HAVE_SFST
    std::cout << " sfst compile...";
    assert(lexcSfst.compileLexical() != NULL);
#endif
#if HAVE_OFST
    std::cout << " ofst compile...";
    assert(lexcOfst.compileLexical() != NULL);
#endif
#if HAVE_FOMA
    std::cout << " foma compile...";
    assert(lexcFoma.compileLexical() != NULL);
#endif
    return EXIT_SUCCESS;
  }
#endif
// vim: set ft=cpp.doxygen:

