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
#include <ctime>

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
#include "lexc-parser.hh"
#include "xre_utils.h"
#include "../../../tools/src/HfstStrings2FstTokenizer.h"

using hfst::HfstTransducer;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstState;
using hfst::implementations::HfstBasicTransition;
using hfst::ImplementationType;
using hfst::xre::XreCompiler;


//using same alg as strings to fst
static char *epsilonname=NULL; // FIX: use this
static bool has_spaces=false;
static bool disjunct_strings=false;
static bool pairstrings=false;
static char *multichar_symbol_filename=NULL;
static StringVector multichar_symbols;


// stupid flex and yacc
extern FILE* hlexcin;
extern int hlexcparse();
extern int hlexcnerrs;
extern void hlexclex_destroy();

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
    parseErrors_(false),
    with_flags_(false)
{}

LexcCompiler::LexcCompiler(ImplementationType impl) :
    quiet_(false),
    verbose_(false),
    format_(impl),
    xre_(impl),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0),
    parseErrors_(false),
    with_flags_(false)
{
    tokenizer_.add_multichar_symbol("@_EPSILON_SYMBOL_@");
    tokenizer_.add_multichar_symbol("@0@");
    tokenizer_.add_multichar_symbol("@ZERO@");
    tokenizer_.add_multichar_symbol("@@ANOTHER_EPSILON@@");
    string hash("#");
    lexiconNames_.insert(hash);
    tokenizer_.add_multichar_symbol(joinerEncode(hash));
}

LexcCompiler::LexcCompiler(ImplementationType impl, bool withFlags) :
    quiet_(false),
    verbose_(false),
    format_(impl),
    xre_(impl),
    initialLexiconName_("Root"),
    totalEntries_(0),
    currentEntries_(0),
    parseErrors_(false),
    with_flags_(withFlags)
{
    tokenizer_.add_multichar_symbol("@_EPSILON_SYMBOL_@");
    tokenizer_.add_multichar_symbol("@0@");
    tokenizer_.add_multichar_symbol("@ZERO@");
    tokenizer_.add_multichar_symbol("@@ANOTHER_EPSILON@@");
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
    hlexclex_destroy();
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
    hlexclex_destroy();
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
LexcCompiler::addNoFlag(const string& lexname)
{
    noFlags_.insert(lexname);
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
    if (with_flags_)
    {
        if (noFlags_.find(continuation) == noFlags_.end())
          {
            encodedCont = flagJoinerEncode(encodedCont, false);
          }
        else
          {
            encodedCont = joinerEncode(encodedCont);
          }
    }
    else
    {
        encodedCont = joinerEncode(encodedCont);
    }
    tokenizer_.add_multichar_symbol(encodedCont);

    /*
    HfstTransducer newPath(data + string("@@ANOTHER_EPSILON@@"),
                           tokenizer_, format_);
    if (weight != 0)
      {
        newPath.set_final_weights(weight);
      }
    if (stringTries_.find(currentLexiconName_) == stringTries_.end())
      {
        stringTries_.insert(pair<string,HfstTransducer*>(currentLexiconName_,
                                                        new HfstTransducer(format_)));
      }
    HfstTransducer joiner(encodedCont, tokenizer_, format_);
    newPath.concatenate(joiner).minimize();
    stringTries_[currentLexiconName_]->disjunct(newPath);
    if ((currentEntries_ % 50) == 0)
      {
        stringTries_[currentLexiconName_]->minimize();
      }
    if (!quiet_)
      {
        if ((currentEntries_ % 10000) == 0)
          {
            fprintf(stderr, SIZE_T_SPECIFIER "...", currentEntries_);
          }
      }

    */

    // build string pair vector map
    //TODO: manage weights

    string joinerEnc = currentLexiconName_;
    if (with_flags_)
    {
        if (noFlags_.find(currentLexiconName_) == noFlags_.end())
          {
            flagJoinerEncode(joinerEnc, true);
          }
        else
          {
            joinerEncode(joinerEnc);
          }
    }
    else
    {
        joinerEncode(joinerEnc);
    }

    tokenizer_.add_multichar_symbol(joinerEnc);
 //   StringPair newStringPair(data + string("@@ANOTHER_EPSILON@@") + encodedCont,
 //                                             data + string("@@ANOTHER_EPSILON@@") + encodedCont);

    StringPairVector newVector(tokenizer_.tokenize(joinerEnc + data + encodedCont));

  //  HfstBasicTransducer oneLexicon;
  //  oneLexicon.disjunct(newVector, 0);

    stringsTrie_.disjunct(newVector, 0);

    /*
    if (stringVectors_.find(currentLexiconName_) == stringVectors_.end())
    {
        stringVectors_.insert(pair<string,HfstBasicTransducer*>(currentLexiconName_,
                                                      new HfstBasicTransducer()));
    }
    stringVectors_[currentLexiconName_]->disjunct(newVector, 0);

*/


    return *this;
}

LexcCompiler&
LexcCompiler::addStringPairEntry(const string& upper, const string& lower,
        const string& continuation, double weight)
{
    //printf("upper: %s lower: %s: continuation: %s \n", upper.c_str(), lower.c_str(), continuation.c_str());
           //identityWoJoin.write_in_att_format(stdout, 1);
    currentEntries_++;
    totalEntries_++;
    continuations_.insert(continuation);
    string encodedCont = string(continuation);
    if (with_flags_) {
        if (noFlags_.find(continuation) == noFlags_.end())
          {
            encodedCont = flagJoinerEncode(encodedCont, false);
          }
        else
          {
            encodedCont = joinerEncode(encodedCont);
          }
    }
    else
    {
        encodedCont = joinerEncode(encodedCont);
    }
    tokenizer_.add_multichar_symbol(encodedCont);

    /*
    HfstTransducer newPath(upper + "@@ANOTHER_EPSILON@@",
                           lower + "@@ANOTHER_EPSILON@@",
                           tokenizer_, format_);
    if (weight != 0)
      {
        newPath.set_final_weights(weight);
      }
    if (stringTries_.find(currentLexiconName_) == stringTries_.end())
      {
        stringTries_.insert(pair<string,HfstTransducer*>(currentLexiconName_,
                                                       new HfstTransducer(format_)));
      }
    HfstTransducer joiner(encodedCont, tokenizer_, format_);
    newPath.concatenate(joiner).minimize();
    stringTries_[currentLexiconName_]->disjunct(newPath);
    if ((currentEntries_ % 50) == 0)
      {
        stringTries_[currentLexiconName_]->minimize();
      }
    if (!quiet_)
      {
        if ((currentEntries_ % 10000) == 0)
          {
            fprintf(stderr, SIZE_T_SPECIFIER "...", currentEntries_);
          }
      }
     */
    // build string pair vector map
    //TODO: manage weights

    string joinerEnc = currentLexiconName_;
    if (with_flags_)
    {
        if (noFlags_.find(currentLexiconName_) == noFlags_.end())
          {
            flagJoinerEncode(joinerEnc, true);
          }
        else
          {
            joinerEncode(joinerEnc);
          }
    }
    else
    {
        joinerEncode(joinerEnc);
    }
    tokenizer_.add_multichar_symbol(joinerEnc);
   // StringPair newStringPair(upper + string("@@ANOTHER_EPSILON@@") + encodedCont,
   //                                           lower + string("@@ANOTHER_EPSILON@@") + encodedCont);


    int upperSize = upper.size();
    int lowerSize = lower.size();

    StringVector upV;
    StringVector loV;

    StringPairVector newVector;

    if ( upperSize > lowerSize)
    {
        std::string epsilons = "";
        for(int i=1; i <= upperSize-lowerSize ; i++)
        {
            //cout << "another epsilon \n";
            epsilons = epsilons + string("@@ANOTHER_EPSILON@@");

        }
        newVector = tokenizer_.tokenize(joinerEnc + upper + encodedCont,
                            joinerEnc + lower + epsilons + encodedCont);
    }
    else if (upperSize < lowerSize)
    {
        std::string epsilons = "";
        for(int i=1; i <= lowerSize-upperSize ; i++)
        {
            //cout << "another epsilon \n";
            epsilons = epsilons + string("@@ANOTHER_EPSILON@@");

        }
        newVector = tokenizer_.tokenize(joinerEnc + upper + epsilons + encodedCont,
                            joinerEnc + lower + encodedCont);
    }
    else
    {
        newVector = tokenizer_.tokenize(joinerEnc + upper + encodedCont,
                    joinerEnc + lower + encodedCont);
    }


  //  StringPairVector newVector(tokenizer_.tokenize(joinerEnc + upper + string("@@ANOTHER_EPSILON@@") + encodedCont,
   //         joinerEnc + lower + string("@@ANOTHER_EPSILON@@") + encodedCont));

    /*
    HfstBasicTransducer oneLexicon;
    oneLexicon.disjunct(newVector, 0);

    if (stringVectors_.find(currentLexiconName_) == stringVectors_.end())
    {
        stringVectors_.insert(pair<string,HfstBasicTransducer*>(currentLexiconName_,
                                                      new HfstBasicTransducer()));
    }
    stringVectors_[currentLexiconName_]->disjunct(newVector, 0);
     */
    stringsTrie_.disjunct(newVector, 0);




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
    if (with_flags_)
    {
        if (noFlags_.find(continuation) == noFlags_.end())
          {
            encodedCont = flagJoinerEncode(encodedCont, false);
          }
        else
          {
            encodedCont = joinerEncode(encodedCont);
          }
    }
    else
    {
        encodedCont = joinerEncode(encodedCont);
    }
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
        regexps_.insert(pair<string,HfstTransducer*>(currentLexiconName_,
                                                   new HfstTransducer(format_)));
      }
    regexps_[currentLexiconName_]->disjunct(*newPaths).minimize();
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
    if (noFlags_.find(lexiconName) == noFlags_.end())
      {
        string encodedName(lexiconName);
        flagJoinerEncode(encodedName, false);
        tokenizer_.add_multichar_symbol(encodedName);
        flagJoinerEncode(encodedName, true);
        tokenizer_.add_multichar_symbol(encodedName);
      }
    else
      {
        string encodedName(lexiconName);
        joinerEncode(encodedName);
        tokenizer_.add_multichar_symbol(encodedName);
      }
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
/*
    if( with_flags_)
        fprintf(stderr, "With Flags \n \n");
    else
        fprintf(stderr, "no Flags \n \n");
*/

    HfstTransducer lexicons(stringsTrie_, format_);
    lexicons.minimize();
    // repeat star to overgenerate
    lexicons.repeat_star().minimize();

    lexicons.substitute("@@ANOTHER_EPSILON@@", "@_EPSILON_SYMBOL_@");
    lexicons.substitute("@ZERO@", "0");
    lexicons.prune_alphabet();


    HfstTransducer start(joinerEncode(initialLexiconName_), tokenizer_, format_);
    string endString = "#";
    joinerEncode(endString);
    HfstTransducer end(endString, tokenizer_, format_);
    lexicons = start.concatenate(lexicons).concatenate(end).minimize();

    //printf("lexicons: \n");
    //lexicons.write_in_att_format(stdout, 1);

    /*
    // if flags

    // for every lex_joiner in noFlags, find only strings where it occurs twice in a row
    // and then replace those joiners with epsilons
    for (set<string>::const_iterator s = noFlags_.begin();
         s != noFlags_.end();
         ++s)
    {
        ...
        string joinerEnc = *s;
        joinerEncode(joinerEnc);
        HfstTransducer joiner(joinerEnc, joinerEnc, format_);
     }
    */
    HfstBasicTransducer joinersTrie_;
    HfstSymbolSubstitutions allJoinersToEpsilon;
    // for every lex_joiner in noFlags, find only strings where it occurs twice in a row
    // and then replace those joiners with epsilons
    for (set<string>::const_iterator s = lexiconNames_.begin();
         s != lexiconNames_.end();
         ++s)
    {
        if (verbose_)
        {
        fprintf(stderr, "Morphotaxing... %s ", s->c_str());
        }
        string joinerEnc = *s;
        joinerEncode(joinerEnc);
        HfstTransducer joiner(joinerEnc, joinerEnc, format_);


        // joiners trie version (later compose)
       // tokenizer_.add_multichar_symbol(joinerEnc);
        StringPairVector newVector(tokenizer_.tokenize(joinerEnc + joinerEnc));
        joinersTrie_.disjunct(newVector, 0);

        //printf ("\ninsert...\n");
        allJoinersToEpsilon.insert(StringPair(joinerEnc, "@_EPSILON_SYMBOL_@"));
        //printf ("\ndone.\n");
     }

    string rootJoiner = initialLexiconName_;
    string hashJoiner = "#";
    joinerEncode(rootJoiner);
    joinerEncode(hashJoiner);

    allJoinersToEpsilon.insert(StringPair(rootJoiner, "@_EPSILON_SYMBOL_@"));
    allJoinersToEpsilon.insert(StringPair(hashJoiner, "@_EPSILON_SYMBOL_@"));


    /// get right side of every pair
    HfstBasicTransducer fsm(lexicons);
    StringSet rightSymbols;
    // Go through all states
    for (HfstBasicTransducer::const_iterator it = fsm.begin();
    it != fsm.end(); it++ )
    {
        // Go through all transitions
        for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it
         = it->begin(); tr_it != it->end(); tr_it++)
        {
            String alph2 = tr_it->get_output_symbol();

            String prefix1("@@ANOTHER_EPSILON@@");
            String prefix2("$_LEXC_JOINER.");
            String prefix3("@_");
            if( alph2.substr(0, prefix1.size()) != prefix1 &&
                    alph2.substr(0, prefix2.size()) != prefix2 &&
                    alph2.substr(0, prefix3.size()) != prefix3 )
            {
                rightSymbols.insert(alph2);

            }
        }
    }

    for ( StringSet::const_iterator it = rightSymbols.begin(); it != rightSymbols.end(); ++it)
    {
        String alph = *it;
        tokenizer_.add_multichar_symbol(alph);
        StringPairVector newVector(tokenizer_.tokenize(alph));
        joinersTrie_.disjunct(newVector, 0);
    }

    HfstTransducer joinersAll(joinersTrie_, format_);
    joinersAll.repeat_star();
    joinersAll.minimize();

     //printf("lexicons before compose: \n");
     //lexicons.write_in_att_format(stdout, 1);

    // printf("joinersAll: \n");
    // joinersAll.write_in_att_format(stdout, 1);
    // printf("\n");

    lexicons.compose(joinersAll).minimize();

    //printf("lexicons --filtered: \n");
    //lexicons.write_in_att_format(stdout, 1);

    lexicons.substitute(allJoinersToEpsilon).minimize();
    lexicons.prune_alphabet();



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
        fprintf(existence_check, "Definitions\n"        \
                "\tdef1 = a\tb c;\n" \
                "\tdef2 = \"asdf\";\n" \
                "\tdef3 = \"sdfg\";\n" \
                "\tdef4 = \"xxx\"; def5 = \"zzz\";\n" \
                "\tdef6 = \"%%;\";\n" \
                "LEXICON Root\ncat # ;\ndog Plural ;\n");
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
