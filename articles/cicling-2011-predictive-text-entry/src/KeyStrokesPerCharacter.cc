#include "KeyStrokesPerCharacter.h"

#define MAXBUFFER 100000

UnknownSymbol::UnknownSymbol(std::string symbol):
  symbol(symbol)
{}

KeyStrokesPerCharacter::KeyStrokesPerCharacter
(size_t fixed_penalty_for_unknown_words):
  fixed_penalty_for_unknown_words(fixed_penalty_for_unknown_words),
  key_strokes(0),
  total_chars(0)
{
  symbol_key_stroke_map["a"] = 1;
  symbol_key_stroke_map["b"] = 2;
  symbol_key_stroke_map["c"] = 3;
  symbol_key_stroke_map["ä"] = 4;
  symbol_key_stroke_map["å"] = 5;

  symbol_key_stroke_map["d"] = 1;
  symbol_key_stroke_map["e"] = 2;
  symbol_key_stroke_map["f"] = 3;

  symbol_key_stroke_map["g"] = 1;
  symbol_key_stroke_map["h"] = 2;
  symbol_key_stroke_map["i"] = 3;

  symbol_key_stroke_map["j"] = 1;
  symbol_key_stroke_map["k"] = 2;
  symbol_key_stroke_map["l"] = 3;

  symbol_key_stroke_map["m"] = 1;
  symbol_key_stroke_map["n"] = 2;
  symbol_key_stroke_map["o"] = 3;
  symbol_key_stroke_map["ö"] = 4;

  symbol_key_stroke_map["p"] = 1;
  symbol_key_stroke_map["q"] = 2;
  symbol_key_stroke_map["r"] = 3;
  symbol_key_stroke_map["s"] = 4;

  symbol_key_stroke_map["t"] = 1;
  symbol_key_stroke_map["u"] = 2;
  symbol_key_stroke_map["v"] = 3;

  symbol_key_stroke_map["w"] = 1;
  symbol_key_stroke_map["x"] = 2;
  symbol_key_stroke_map["y"] = 3;
  symbol_key_stroke_map["z"] = 3;

  symbol_key_stroke_map[" "] = 1;

  symbol_button_map["a"] = 2;
  symbol_button_map["b"] = 2;
  symbol_button_map["c"] = 2;
  symbol_button_map["ä"] = 2;
  symbol_button_map["å"] = 2;

  symbol_button_map["d"] = 3;
  symbol_button_map["e"] = 3;
  symbol_button_map["f"] = 3;

  symbol_button_map["g"] = 4;
  symbol_button_map["h"] = 4;
  symbol_button_map["i"] = 4;

  symbol_button_map["j"] = 5;
  symbol_button_map["k"] = 5;
  symbol_button_map["l"] = 5;

  symbol_button_map["m"] = 6;
  symbol_button_map["n"] = 6;
  symbol_button_map["o"] = 6;
  symbol_button_map["ö"] = 6;

  symbol_button_map["p"] = 7;
  symbol_button_map["q"] = 7;
  symbol_button_map["r"] = 7;
  symbol_button_map["s"] = 7;

  symbol_button_map["t"] = 8;
  symbol_button_map["u"] = 8;
  symbol_button_map["v"] = 8;

  symbol_button_map["w"] = 9;
  symbol_button_map["x"] = 9;
  symbol_button_map["y"] = 9;
  symbol_button_map["z"] = 9;

  symbol_button_map[" "] = 10;

}

void KeyStrokesPerCharacter::read_next_unknown_word(std::string word,
						    bool compute_penalty)
{
  StringVector tokenized_line = tokenizer.tokenize_one_level(word);
  
  if (compute_penalty)
    {
      // Enter word characcters.
      key_strokes += tokenized_line.size();

      // Scroll through suggestions.
      key_strokes += fixed_penalty_for_unknown_words;

      // switch to multitap.
      key_strokes += 1;

      // Delete  incorrect suggestion.
      key_strokes += tokenized_line.size();
    }

  for (StringVector::const_iterator it = tokenized_line.begin();
       it != tokenized_line.end();
       ++it)
    {
      size_t key_strokes_for_this_char = symbol_key_stroke_map[*it];
      if (key_strokes_for_this_char == 0)
	{ throw UnknownSymbol(*it); }
      
      key_strokes += key_strokes_for_this_char;

      if (it != tokenized_line.begin())
	{
	  if (symbol_button_map[*it] == symbol_button_map[*(it - 1)])
	    { ++key_strokes; }
	}
    }
  
  if (compute_penalty)
    {
      // switch to predictive text entry.
      key_strokes += 1;
    }

  total_chars += tokenized_line.size();
}

void KeyStrokesPerCharacter::read_next_word(std::string word,
					    size_t position)
{
  StringVector tokenized_line = tokenizer.tokenize_one_level(word);

  key_strokes += position + tokenized_line.size();

  total_chars += tokenized_line.size();
}

double KeyStrokesPerCharacter::get_key_strokes_per_character(void)
{
  double key_strokes_per_character = 0.0;
  
  if (total_chars > 0)
    { key_strokes_per_character = key_strokes * 1.0 / total_chars; }

  key_strokes = 0;
  total_chars = 0;

  return key_strokes_per_character;
}

#include <sstream>

int get_pos(std::string line)
{
  if (line.find('\t') == std::string::npos)
    { return -2; }

  if (line.substr(line.find('\t')+1) == "infty")
    { return -1; }

  std::istringstream in(line.substr(line.find('\t')+1));
  int pos;
  in >> pos;

  if (in.fail())
    { return -2; }
  return pos;
}

std::string get_word(std::string line)
{
  if (line.find('\t') == std::string::npos)
    { return line; }
  return line.substr(0,line.find('\t')); 
}

int main(void)
{
  KeyStrokesPerCharacter multitap;
  KeyStrokesPerCharacter new_method(10);

  try
    {
      while (std::cin.peek() != -1)
	{
	  char line[100000];
	  std::cin.getline(line,100000);

	  int pos = get_pos(line);

	  if (pos == -2)
	    { 
	      std::cerr << "Line: \"" << line << "\" is invalid" << std::endl;
	      exit(1);
	    }

	  std::string word = get_word(line);

	  if (pos == -1)
	    { new_method.read_next_unknown_word(word,true); }
	  else
	    { new_method.read_next_word(word,static_cast<size_t>(pos)); }

	  multitap.read_next_unknown_word(word);
	}
    }
  catch (UnknownSymbol &e)
    {
      std::cerr << "Symbol \"" << e.symbol << "\" couldn't be encoded."
		<< std::endl;
      exit(1);
    }

  std::cout << "multitap: " << multitap.get_key_strokes_per_character()
	    << " kspc" << std::endl;

  std::cout << "new_method: " << new_method.get_key_strokes_per_character()
	    << " kspc" << std::endl;

}
