#ifndef KEY_STROKES_PER_CHARACTER_H
#define KEY_STROKES_PER_CHARACTER_H

#include <hfst/HfstTransducer.h>
#include <map>
#include <string>

using hfst::HfstTokenizer;
using hfst::StringVector;

struct UnknownSymbol
{
  UnknownSymbol(std::string symbol);
  std::string symbol;
};

class KeyStrokesPerCharacter
{
 public:
  KeyStrokesPerCharacter(size_t fixed_penalty_for_unknown_words=0);
  void read_next_unknown_word(std::string word,bool compute_penalty=false);
  void read_next_word(std::string word,size_t position);
  double get_key_strokes_per_character(void);

 protected:
  typedef std::map<std::string,size_t> SymbolNumberMap;
  
  SymbolNumberMap symbol_key_stroke_map;
  SymbolNumberMap symbol_button_map;

  HfstTokenizer tokenizer;
  size_t fixed_penalty_for_unknown_words;
  size_t key_strokes;
  size_t total_chars;

};

#endif // KEY_STROKES_PER_CHARACTER_H
