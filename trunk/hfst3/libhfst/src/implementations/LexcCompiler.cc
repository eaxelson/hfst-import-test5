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

using std::string;
using std::map;

#include "LexcCompiler.h"
#include "../HfstTransducer.h"
#include "XreCompiler.h"
#include "lexc-utils.h"
#include "lexc-parser.h"

using hfst::HfstTransducer;
using hfst::HfstMutableTransducer;
using hfst::HfstState;
using hfst::HfstStateIterator;
using hfst::HfstTransitionIterator;
using hfst::ImplementationType;
using hfst::xre::XreCompiler;

// stupid flex§
extern FILE* hlexcin;
extern int hlexcparse();

namespace hfst { namespace lexc {

LexcCompiler* lexc_ = 0;

LexcCompiler::LexcCompiler() :
    quiet_(false),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0)
{}

LexcCompiler::LexcCompiler(ImplementationType impl) :
    quiet_(false),
    format_(impl),
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
            fprintf(stderr, "%d...", currentEntries_);
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
            fprintf(stderr, "%d...", currentEntries_);
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
    HfstTransducer* newPaths = xre_.compile(regexp + " "  + encodedCont);
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
            fprintf(stderr, "%d...", currentEntries_);
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
            fprintf(stderr, "%d ", currentEntries_);
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
    HfstMutableTransducer rebuilt;
    map<string,HfstTransducer> lexicons;
    // combine tries with reg.exps and minimize
    for (set<string>::const_iterator s = lexiconNames_.begin();
         s != lexiconNames_.end();
         ++s)
      {
        HfstTransducer lexicon(format_);
        if (stringTries_.find(*s) != stringTries_.end())
          {
            lexicon.disjunct(stringTries_[*s]);
          }
        if (regexps_.find(*s) != regexps_.end())
          {
            lexicon.disjunct(regexps_[*s]);
          }
        lexicon.minimize();
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
    rebuilt.add_line(new_end, 0);
    starts[startEnc] = new_end;
    HfstState sink = first_free;
    first_free++;
    // connect lexicons
    for (map<string,HfstTransducer>::const_iterator lex = lexicons.begin();
         lex != lexicons.end();
         ++lex)
      {
        // help structures
        HfstMutableTransducer mut(lex->second);
        map<HfstState,HfstState> rebuildMap;
        // connect start states
        string nameEnc(lex->first);
        joinerEncode(nameEnc);
        HfstState start_nu = starts[nameEnc];
        HfstState start_old = 0;
        rebuildMap[start_old] = start_nu;
        // clone all states
        for (HfstStateIterator state(mut);
             !state.done();
             state.next())
          {
            HfstState old_state = state.value();
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
            for (HfstTransitionIterator transition(mut, old_state);
                 !transition.done();
                 transition.next())
              {
                HfstTransition old_transition = transition.value();
                string old_osymbol = old_transition.osymbol;
                string old_isymbol = old_transition.isymbol;
                string nu_osymbol;
                string nu_isymbol;
                float nu_weight = old_transition.weight;
                HfstState old_target = old_transition.target;
                HfstState nu_target;
                if (old_osymbol.substr(0, 5) == "@LEXC")
                  {
                    // quasitransition to new lexicon 
                    nu_weight += mut.get_final_weight(old_target);
                    nu_osymbol = "@_EPSILON_SYMBOL_@";
                    nu_isymbol = "@_EPSILON_SYMBOL_@";
                    if (starts.find(nu_osymbol) != starts.end())
                      {
                        nu_target = starts[nu_osymbol];
                      }
                    else
                      {
                        // non-existent continuation, kill path
                        nu_target = sink;
                      }
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
                HfstState nu_icode = rebuilt.alphabet->add_symbol(nu_isymbol.c_str());
                HfstState nu_ocode = rebuilt.alphabet->add_symbol(nu_osymbol.c_str());

                rebuilt.add_line(nu_state, nu_target,
                                 nu_icode, nu_ocode,
                                 nu_weight);
              }
          }
      }
    // FIXME: howto create HfstTransducer from internal?
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

// vim: set ft=cpp.doxygen:

