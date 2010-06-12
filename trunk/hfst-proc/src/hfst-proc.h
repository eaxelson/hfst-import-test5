/*
  
  Copyright 2009 University of Helsinki
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  
*/
 
#ifndef _HFST_OPTIMIZED_LOOKUP_H_
#define _HFST_OPTIMIZED_LOOKUP_H_

#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <limits>
#include <string>
#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define OL_FULL_DEBUG 0

class LookupPath;

/**
 * A list of pointers to lookup paths. They are pointers for the sake of
 * polymorphism
 */
typedef std::vector<LookupPath*> LookupPathVector;

class TokenIOStream;
class Token;
typedef std::vector<Token> TokenVector;
class OutputFormatter;
class SymbolProperties;

enum OutputType {Apertium, xerox};
extern OutputType outputType;

extern bool verboseFlag;

extern bool displayWeightsFlag;
extern int maxAnalyses;
extern bool preserveDiacriticRepresentationsFlag;

// the following flags are only meaningful with certain debugging #defines
extern bool printDebuggingInformationFlag;

typedef unsigned short SymbolNumber;
typedef unsigned int TransitionTableIndex;
typedef unsigned int TransitionNumber;
typedef unsigned int StateIdNumber;
typedef short ValueNumber;
typedef float Weight;
typedef std::vector<SymbolNumber> SymbolNumberVector;
typedef std::vector<SymbolProperties> SymbolTable;


const SymbolNumber NO_SYMBOL_NUMBER = std::numeric_limits<SymbolNumber>::max();
const TransitionTableIndex NO_TABLE_INDEX = std::numeric_limits<TransitionTableIndex>::max();
const Weight INFINITE_WEIGHT = static_cast<float>(NO_TABLE_INDEX);

// the flag diacritic operators as given in
// Beesley & Karttunen, Finite State Morphology (U of C Press 2003)
enum FlagDiacriticOperator {P, N, R, D, C, U};

enum HeaderFlag {Weighted, Deterministic, Input_deterministic, Minimized,
  	 Cyclic, Has_epsilon_epsilon_transitions,
  	 Has_input_epsilon_transitions, Has_input_epsilon_cycles,
  	 Has_unweighted_input_epsilon_cycles};

enum GenerationMode
{
  gm_clean,      // clear all
  gm_unknown,    // display unknown words, clear transfer and generation tags
  gm_all,        // display all
  gm_marked      // marked generation
};

/**
 * Different methods of dealing with capitalization during generation
 * IgnoreCase     - allow uppercase symbols to be treated as lowercase during
 *                  lookup. Retain the surface form's case for the output
 * CaseSensitive  - only allow the given case of the surface form symbol to be
 *                  fed into the transducer for lookup
 * DictionaryCase - allow uppercase symbols to be treated as lowecase during
 *                  lookup. Output the result with the capitalization found in
 *                  the transducer
 */
enum CapitalizationMode {IgnoreCase, CaseSensitive, DictionaryCase};

/**
 * Describes the case properties of a surface form (detection examines only
 * the first and last symbols)
 * LowerCase      - The first and last symbols are lowercase
 * FirstUpperCase - The first symbol is uppercase and the last is lowercase
 * UpperCase      - The first and last symbols are uppercase
 */
enum CapitalizationState {Unknown, LowerCase, FirstUpperCase, UpperCase};

class TransitionIndex;
class Transition;

typedef std::vector<TransitionIndex*> TransitionIndexVector;
typedef std::vector<Transition*> TransitionVector;

// This is 2^31, hopefully equal to UINT_MAX/2 rounded up.
// For some profound reason it can't be replaced with (UINT_MAX+1)/2.
const TransitionTableIndex TRANSITION_TARGET_TABLE_START = 2147483648u;

void stream_error(const char* e);
void stream_error(std::string e);


#endif
