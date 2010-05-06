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
  hf_minimised,
  hf_deterministic,
  hf_weighted,
  hf_cyclic
};

class HFSTTransducer
{
protected:
  HFSTTransducerHeader header;

public:
  HFSTTransducer();
  ~HFSTTransducer();
}

class HFSTTransducerHeader 
{
private:
  bool weighted;
  bool deterministic;
  bool minimised;
  bool cyclic;
  bool has_input_epsilon_cycles;
  bool has_unweighted_input_epsilon_cycles;
  
  void readProperty(bool &property, FILE *transducer);

public:
  HFSTTransducerHeader(FILE *transducer);
  ~HFSTTransducerHeader();

  bool probeFlag(HeaderFlag flag);

};

class HFSTTransducerAlphabet
{

};

class HFSTApertiumApplicator 
{

private:
  HFSTTransducer
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
