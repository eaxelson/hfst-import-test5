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

using std::string;
using std::map;
using std::set;

#include "LexcCompiler.h"
#include "../../../libhfst/src/HfstTransducer.h"
#include "../../../libhfst/src/implementations/HfstTransitionGraph.h"
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

// stupid flex§
extern FILE* hlexcin;
extern int hlexcparse();

#ifndef DEBUG_MAIN

namespace hfst { namespace lexc {

LexcCompiler* lexc_ = 0;

LexcCompiler::LexcCompiler() :
    quiet_(false),
    format_(TROPICAL_OPENFST_TYPE),
    xre_(TROPICAL_OPENFST_TYPE),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0)
{}

LexcCompiler::LexcCompiler(ImplementationType impl) :
    quiet_(false),
    format_(impl),
    xre_(impl),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0)
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
    hlexcin = infile;
    hlexcparse();
    return *this;
}

LexcCompiler& LexcCompiler::parse(const char* filename)
{
    lexc_ = this;
    hlexcin = fopen(filename, "r");
    if (hlexcin == NULL)
      {
        fprintf(stderr, "could not open %s for reading\n", filename);
        return *this;
      }
    hlexcparse();
    return *this;
}

LexcCompiler&
LexcCompiler::setVerbosity(bool verbose)
{
    quiet_ = !verbose;
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
    encodedCont = joinerEncode(encodedCont);
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
            fprintf(stderr, "%zu...", currentEntries_);
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
    encodedCont = joinerEncode(encodedCont);
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
            fprintf(stderr, "%zu...", currentEntries_);
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
    encodedCont = joinerEncode(encodedCont);
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
            fprintf(stderr, "%zu...", currentEntries_);
          }
      }
    return *this;
}

LexcCompiler&
LexcCompiler::addXreDefinition(const string& definition_name, const string& xre)
{
    // collect implicit characters
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
    currentEntries_ = 0;
    static bool firstLexicon = true;
    currentLexiconName_ = lexiconName;
    lexiconNames_.insert(lexiconName);
    string encodedName(lexiconName);
    joinerEncode(encodedName);
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
            fprintf(stderr, "%zu ", currentEntries_);
          }
        fprintf(stderr, "%s...", lexiconName.c_str());
        firstLexicon = false;
      }
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
    printConnectedness();
    HfstBasicTransducer rebuilt;
    map<string,HfstTransducer> lexicons;
    // combine tries with reg.exps and minimize
    for (set<string>::const_iterator s = lexiconNames_.begin();
         s != lexiconNames_.end();
         ++s)
      {
        if (*s == "#")
          {
            continue;
          }
        HfstTransducer lexicon(format_);
        if (stringTries_.find(*s) != stringTries_.end())
          {
            lexicon.disjunct(stringTries_[*s]);
          }
        if (regexps_.find(*s) != regexps_.end())
          {
            lexicon.disjunct(regexps_[*s]);
          }
        lexicon.determinize();
        lexicons.insert(pair<string,HfstTransducer>(*s, lexicon));
      }
    // build initial states
    map<string,HfstState> starts;
    HfstState first_free = 0;
    string startEnc(initialLexiconName_);
    joinerEncode(startEnc);
    starts[startEnc] = first_free;
    first_free++;
    for (map<string,HfstTransducer>::const_iterator lex = lexicons.begin();
         lex != lexicons.end();
         ++lex)
      {
        string nameEnc(lex->first);
        joinerEncode(nameEnc);
        if (nameEnc != startEnc)
          {
            starts[nameEnc] = first_free;
            first_free++;
          }
      }
    string ender("#");
    joinerEncode(ender);
    HfstState new_end = first_free;
    first_free++;
    rebuilt.set_final_weight(new_end, 0);
    starts[ender] = new_end;
    HfstState sink = first_free;
    first_free++;
    // connect lexicons
    for (map<string,HfstTransducer>::const_iterator lex = lexicons.begin();
         lex != lexicons.end();
         ++lex)
      {
        // help structures
        HfstBasicTransducer mut(lex->second);
        map<HfstState,HfstState> rebuildMap;
        // connect start states
        string nameEnc(lex->first);
        joinerEncode(nameEnc);
        HfstState start_nu = starts[nameEnc];
        HfstState start_old = 0;
        rebuildMap[start_old] = start_nu;
        // clone all states
        for (HfstBasicTransducer::iterator state = mut.begin();
             state != mut.end();
             state++)
          {
            HfstState old_state = state->first;
            HfstState nu_state;
            if (rebuildMap.find(old_state) != rebuildMap.end())
              {
                // already built
                nu_state = rebuildMap[old_state];
              }
            else
              {
                // create new
                nu_state = first_free;
                first_free++;
                rebuildMap[old_state] = nu_state;
              }
            // clone all transitions
	    HfstBasicTransducer::HfstTransitionSet transitions = mut[old_state];
	    for ( HfstBasicTransducer::HfstTransitionSet::iterator transition 
		    = transitions.begin(); 
		  transition != transitions.end();
		  transition++ )
              {
                //HfstTransitionData old_transition = transition->get_transition_data();
                string old_osymbol = transition->get_input_symbol();
                string old_isymbol = transition->get_output_symbol();
                string nu_osymbol;
                string nu_isymbol;
                float nu_weight = transition->get_weight();
                HfstState old_target = transition->get_target_state();
                HfstState nu_target;
                if (old_osymbol.substr(0, 5) == "@LEXC")
                  {
                    // quasitransition to new lexicon 
                    if (mut.is_final_state(old_target))
                      {
                        nu_weight += mut.get_final_weight(old_target);
                      }
                    if (starts.find(old_osymbol) != starts.end())
                      {
                        nu_target = starts[old_osymbol];
                      }
                    else
                      {
                        // non-existent continuation, kill path
                        nu_target = sink;
                      }
                    nu_osymbol = "@_EPSILON_SYMBOL_@";
                    nu_isymbol = "@_EPSILON_SYMBOL_@";
                  }
                else
                  {
                    // regular transition
                    nu_isymbol = old_isymbol;
                    nu_osymbol = old_osymbol;
                    if (rebuildMap.find(old_target) != rebuildMap.end())
                      {
                        // target already built
                        nu_target = rebuildMap[old_target];
                      }
                    else
                      {
                        // new target, rebuild asap
                        nu_target = first_free;
                        first_free++;
                        rebuildMap[old_target] = nu_target;
                      }
                  }
                //HfstState nu_icode = rebuilt.alphabet->add_symbol(nu_isymbol.c_str());
                //HfstState nu_ocode = rebuilt.alphabet->add_symbol(nu_osymbol.c_str());

		/*                rebuilt.add_line(nu_state, nu_target,
                                 nu_icode, nu_ocode,
                                 nu_weight); */
		rebuilt.add_transition(nu_state, HfstBasicTransition
				       (nu_target,
					nu_isymbol,
					nu_osymbol,
					nu_weight));
              }
          }
      }
    HfstTransducer* rv = new HfstTransducer(rebuilt, format_);

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

