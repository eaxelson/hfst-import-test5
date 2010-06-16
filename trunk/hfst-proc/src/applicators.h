#ifndef _APPLICATORS_H_
#define _APPLICATORS_H_

#include "tokenizer.h"

/**
 * Abstract base class for actions that can be done using a transducer
 */
class Applicator
{
 protected:
  const AbstractTransducer& transducer;
  TokenIOStream& token_stream;
 public:
  Applicator(const AbstractTransducer& t, TokenIOStream& ts): transducer(t), token_stream(ts) {}
  virtual ~Applicator() {}
  
  virtual void apply() = 0;
};

/**
 * Splits the input stream into tokens and outputs the results
 */
class TokenizationApplicator: public Applicator
{
 private:
 public:
  TokenizationApplicator(const AbstractTransducer& t, TokenIOStream& ts): Applicator(t,ts) {}
  void apply();
};

class AnalysisApplicator: public Applicator
{
 private:
  OutputFormatter& formatter;
  CapitalizationMode caps_mode;
 public:
  AnalysisApplicator(const AbstractTransducer& t, TokenIOStream& ts,
                     OutputFormatter& o, CapitalizationMode c):
    Applicator(t,ts), formatter(o), caps_mode(c) {}
  void apply();
};

class GenerationApplicator: public Applicator
{
 private:
  GenerationMode mode;
  CapitalizationMode caps_mode;
  
  /**
   * Split the given token string into a set of token strings to generate with,
   * breaking on + after apertium-style tags.
   */
  std::vector<TokenVector> split(const TokenVector& tokens) const;
  
  void lookup(const TokenVector& tokens);

 public:
  GenerationApplicator(const AbstractTransducer& t, TokenIOStream& ts,
                       GenerationMode m, CapitalizationMode c):
    Applicator(t,ts), mode(m), caps_mode(c) {}
  void apply();
};

#endif
