#ifndef __HFST_PROC_H__
#define __HFST_PROC_H__

#include <cstdlib>
#include <climits>
#include <getopt.h>
#include <iostream>
#include <libgen.h>

#include <map>
#include <vector>
#include <set>

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif

#define PACKAGE_VERSION "0.0.1"

/** 
 * The flag diacritic operators as given in:
 *   Beesley & Karttunen (2003) Finite State Morphology 
 */

enum FlagDiacriticOperator 
{
  // (P) Positive:    When a @P.feature.value@ flag diacritic is encountered, the 
  //       value of the indicated feature is simply set or reset to the indicated value. 
  // (N) Negative:    When an @N.feature.value@ flag diacritic is encountered, the 
  //       value of feature is set or reset to the negation or complement of value.
  // (R) Require:     When an @R.feature.value@ flag diacritic is encountered, a test 
  //       is performed; this test succeeds if and only if feature is currently set 
  //       to value.
  // (D) Disallow:    When a @D.feature.value@ flag diacritic is encountered, the test 
  //       succeeds if and only if feature is currently neutral or is set to a value 
  //       that is incompatible with value.
  // (C) Clear:       When a @C.feature@ Flag Diacritic is encountered, the value of 
  //       feature is reset to neutral.
  // (U) Unification: If feature is currently neutral, then encountering @U.feature.value@ 
  //       simply causes feature to be set to value. Else if feature is currently 
  //       set (non-neutral), then the test will succeed if and only if value is 
  //       compatible with the current value of feature.

  P,  
  N, 
  R, 
  D, 
  C, 
  U 
};


/**
 * Kind of output of the generator module (taken from lttoolbox/fst_processor.h)
 */
enum GenerationMode
{
  // The marks are as follows: '*' is a source language unknown word, '@' is a
  // lexical transfer error and '#' is a generation error
  //
  //       *El @viejo\<adj\> #white\<adj\> dog
  //
  //   gm_all = All word marks are output along with tags for erroneous words    
  //          --> *El @viejo\<adj\> #white\<adj\> dog
  // 
  //   gm_unknown = Only source unknown words are marked
  //          --> *El viejo white dog
  // 
  //   gm_clean = No words are marked
  //          --> El viejo white dog
  // 
  //   gm_marked = All words are marked, but tags are stripped
  //          --> *El @viejo #white dog
  // 
  //   Further details: http://wiki.apertium.org/wiki/Apertium_stream_format
  //

  gm_clean,
  gm_unknown,
  gm_all,
  gm_marked
};

enum HeaderFlag
{
  hf_uw_input_epsilon_cycles,
  hf_input_epsilon_cycles,
  hf_epsilon_epsilon_transitions,
  hf_input_epsilon_transitions,
  hf_minimised,
  hf_input_deterministic,
  hf_deterministic,
  hf_weighted,
  hf_cyclic
};

class HFSTTransducer;
class HFSTTransducerHeader;
class HFSTTransducerAlphabet;
class FlagDiacriticOperation;

typedef unsigned short SymbolNumber;                 // TODO: Describe these
typedef unsigned int TransitionTableIndex;           
typedef unsigned int TransitionNumber;               
typedef unsigned int StateIdNumber;                  
typedef unsigned int ArcNumber;
typedef short ValueNumber;
typedef std::map<SymbolNumber,const char*> KeyTable;
typedef std::vector<FlagDiacriticOperation> OperationVector;

const StateIdNumber NO_ID_NUMBER = UINT_MAX;
const SymbolNumber NO_SYMBOL_NUMBER = USHRT_MAX;
const TransitionTableIndex NO_TABLE_INDEX = UINT_MAX;


/******************************************************************************
 * This class implements a flag diacritic operation
 *****************************************************************************/

class FlagDiacriticOperation
{
private:
  FlagDiacriticOperator operation;
  SymbolNumber feature;
  ValueNumber value;

public:
  FlagDiacriticOperation(FlagDiacriticOperator operation, SymbolNumber feat, ValueNumber value);

};



/******************************************************************************
 * This class implements the header of an HFST transducer.
 *****************************************************************************/

class HFSTTransducerHeader 
{
private:
  SymbolNumber number_of_symbols;
  SymbolNumber number_of_input_symbols;
  TransitionTableIndex transition_index_table_size;
  TransitionTableIndex transition_target_table_size;

  StateIdNumber number_of_states;
  TransitionNumber number_of_transitions;

  bool weighted;
  bool input_deterministic;
  bool deterministic;
  bool minimised;
  bool cyclic;
  bool has_epsilon_epsilon_transitions;
  bool has_input_epsilon_transitions;
  bool has_input_epsilon_cycles;
  bool has_unweighted_input_epsilon_cycles;
  
  void readProperty(bool &property, FILE *transducer);

public:
  HFSTTransducerHeader();
  ~HFSTTransducerHeader();

  void readHeader(FILE *transducer);
  bool probeFlag(HeaderFlag flag);
  SymbolNumber symbolCount();

};

/******************************************************************************
 * This class implements the alphabet of an HFST transducer.
 *****************************************************************************/

class HFSTTransducerAlphabet
{
private:
  SymbolNumber number_of_symbols;
  KeyTable *key_table;
  char *line;

  std::map<std::string, SymbolNumber> feature_bucket;
  std::map<std::string, ValueNumber> value_bucket;

  ValueNumber value_number;
  SymbolNumber feature_number;

public:
  HFSTTransducerAlphabet();
  ~HFSTTransducerAlphabet();

  void readAlphabet(FILE *transducer, SymbolNumber symbol_number);
  void getNextSymbol(FILE *transducer, SymbolNumber key);
  SymbolNumber getStateSize();
};

/******************************************************************************
 * This class implements an HFST transducer.
 *****************************************************************************/

class HFSTTransducer
{
protected:
  HFSTTransducerHeader header;
  HFSTTransducerAlphabet alphabet;

public:
  HFSTTransducer();
  ~HFSTTransducer();

  void loadTransducer(FILE *input);
};

/******************************************************************************
 * This is the wrapper class for Apertium stream format. It contains an HFST
 * transducer object, and methods to read a stream and tokenise and analyse, 
 * and to read a stream and generate. The general layout is modelled after the
 * FSTProcessor class in lttoolbox/fst_processor.h 
 *****************************************************************************/

class HFSTApertiumApplicator 
{

private:
  HFSTTransducer transducer;
  bool dictionaryCase;
  bool nullFlush;

  void streamError();

public:
  HFSTApertiumApplicator();
  ~HFSTApertiumApplicator();

  void loadTransducer(FILE *input);

  void setDictionaryCaseMode(bool const value);
  void setNullFlush(bool const value);

  void initAnalysis();  
  void initGeneration();  

  void analysis(FILE *input = stdin, FILE *output = stdout);
  void generation(FILE *input = stdin, FILE *output = stdout, GenerationMode mode = gm_unknown);

};

#endif /* __HFST_PROC_H__ */
