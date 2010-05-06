#ifndef __HFST_PROC_H__
#define __HFST_PROC_H__

#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <libgen.h>

#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif

#define PACKAGE_VERSION "0.0.1"

typedef unsigned short SymbolNumber;       // TODO: What is this ?
typedef unsigned int TransitionTableIndex; // TODO: What is this ? 
typedef unsigned int TransitionNumber;     // TODO: What is this ? 
typedef unsigned int StateIdNumber;        // TODO: What is this ? 

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

};

/******************************************************************************
 * This class implements the alphabet of an HFST transducer.
 *****************************************************************************/

class HFSTTransducerAlphabet
{

};

/******************************************************************************
 * This class implements an HFST transducer.
 *****************************************************************************/

class HFSTTransducer
{
protected:
  HFSTTransducerHeader header;

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
