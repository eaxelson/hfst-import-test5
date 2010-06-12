#ifndef _FORMATTER_H_
#define _FORMATTER_H_

#include "lookup-path.h"
#include "tokenizer.h"

/**
 * Abstract base class for handling the outputting of lookup results. Subclasses
 * implement different formatting systems.
 */
class OutputFormatter
{
 protected:
  TokenIOStream& token_stream;
  
  /**
   * Return a sorted copy of the path vector that contains no more than
   * maxAnalyses entries
   */
  LookupPathVector preprocess_finals(const LookupPathVector& finals) const;
 public:
  OutputFormatter(TokenIOStream& s): token_stream(s) {}
  virtual ~OutputFormatter() {}
  
  /**
   * Take a list of lookup paths that end in final states, and produce a list of
   * string representations of the paths that can be written to the output
   * @param finals a list of lookup paths ending in final states
   * @param state the capitalization of the surface form
   */
  virtual std::vector<std::string> process_finals(const LookupPathVector& finals,
                                                  CapitalizationState state) const = 0;
  virtual void print_word(const TokenVector& surface_form, 
                          std::vector<std::string> const &analyzed_forms) const = 0;
  virtual void print_unknown_word(const TokenVector& surface_form) const = 0;
  
  /**
   * Whether non-alphabetic characters that aren't in the transducer should be
   * passed through to the output or not
   */
  virtual bool preserve_nonalphabetic() const = 0;
};

class ApertiumOutputFormatter: public OutputFormatter
{
 public:
  ApertiumOutputFormatter(TokenIOStream& s): OutputFormatter(s) {}
  
  std::vector<std::string> process_finals(const LookupPathVector& finals,
                                          CapitalizationState state) const;
  void print_word(const TokenVector& surface_form, 
                  std::vector<std::string> const &analyzed_forms) const;
  void print_unknown_word(const TokenVector& surface_form) const;
  
  bool preserve_nonalphabetic() const {return true;}
};

class XeroxOutputFormatter: public OutputFormatter
{
  /**
   * Return a copy of the given token vector which has all superblanks
   * converted to blanks
   */
  TokenVector clear_superblanks(const TokenVector& tokens) const;
 public:
  XeroxOutputFormatter(TokenIOStream& s): OutputFormatter(s) {}
  
  std::vector<std::string> process_finals(const LookupPathVector& finals,
                                          CapitalizationState state) const;
  void print_word(const TokenVector& surface_form, 
                  std::vector<std::string> const &analyzed_forms) const;
  void print_unknown_word(const TokenVector& surface_form) const;
  
  bool preserve_nonalphabetic() const {return false;}
};

#endif
