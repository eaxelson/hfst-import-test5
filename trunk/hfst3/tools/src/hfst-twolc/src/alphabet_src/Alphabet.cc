//! @file Alphabet.cc
//!
//! @author Miikka Silfverberg
//!
//! @brief The alphabet of a twolc-grammar.

//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 3 of the Licence.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Alphabet.h"

void Alphabet::define_set(const std::string &name,const SymbolRange &elements)
{ sets[name] = elements; }

const OtherSymbolTransducer &Alphabet::get_transducer(const SymbolPair &pair)
{ return alphabet.has_key(pair) ? alphabet[pair] : compute(pair); } 

bool Alphabet::is_pair(const std::string &input,const std::string &output)
{
  if (input == "__HFST_TWOLC_?" and output == "__HFST_TWOLC_?")
    { return true; }
  if (input == "__HFST_TWOLC_?")
    { return output_symbols.has_element(output); }
  if (output == "__HFST_TWOLC_?")
    { return input_symbols.has_element(input); }
  if (diacritics.has_element(input) and input == output)
    { return true; }
  return alphabet_set.has_element(SymbolPair(input,output));
}

void Alphabet::define_singleton_set(const std::string &name)
{ sets[name] = SymbolRange(1,name); }

const OtherSymbolTransducer &Alphabet::compute(const SymbolPair &pair)
{
  if (not sets.has_key(pair.first))
    { define_singleton_set(pair.first); }
  if (not sets.has_key(pair.second))
    { define_singleton_set(pair.second); }

  const std::string &input = pair.first;
  const std::string &output = pair.second;

  OtherSymbolTransducer pair_transducer;

  if (diacritics.has_element(input))
    { 
      pair_transducer.apply(&HfstTransducer::disjunct,
			    OtherSymbolTransducer(input,input)); 
      if (input != output and output != TWOLC_EPSILON and 
	  output != TWOLC_UNKNOWN)
	{ std::cerr << "Warning: Diacritic " << input << " in pair "
		    << input << ":" << output << " will correspond 0."
		    << std::endl; }
    }
  else if (input == TWOLC_UNKNOWN and output == TWOLC_UNKNOWN)
    { 
      for (HandySet<SymbolPair>::const_iterator it = alphabet_set.begin();
	   it != alphabet_set.end();
	   ++it)
	{
	  pair_transducer.apply(&HfstTransducer::disjunct,
				OtherSymbolTransducer(it->first,it->second));
	}
      pair_transducer.apply(&HfstTransducer::disjunct,
      			    OtherSymbolTransducer(TWOLC_UNKNOWN));
    }
  else if (input == TWOLC_UNKNOWN)
    {
      const SymbolRange &output_set = sets[output];
      for (SymbolRange::const_iterator it = output_set.begin();
	   it != output_set.end();
	   ++it)
	{
	  for (HandySet<SymbolPair>::const_iterator jt = alphabet_set.begin();
	       jt != alphabet_set.end();
	       ++jt)
	    {
	      if (*it == jt->second)
		{ 
		  pair_transducer.apply(&HfstTransducer::disjunct,
					OtherSymbolTransducer
					(jt->first,jt->second)); }
	    }
	}
    }
  else if (output == TWOLC_UNKNOWN)
    {
      const SymbolRange &input_set = sets[input];
      for (SymbolRange::const_iterator it = input_set.begin();
	   it != input_set.end();
	   ++it)
	{
	  for (HandySet<SymbolPair>::const_iterator jt = alphabet_set.begin();
	       jt != alphabet_set.end();
	       ++jt)
	    {
	      if (*it == jt->first)
		{ 
		  pair_transducer.apply(&HfstTransducer::disjunct,
					OtherSymbolTransducer
					(jt->first,jt->second)); }
	    }
	}
    }
  else
    {
      const SymbolRange &input_set = sets[input];
      const SymbolRange &output_set = sets[output];
      for (SymbolRange::const_iterator it = input_set.begin(); 
	   it != input_set.end(); 
	   ++it)
	{
	  for (SymbolRange::const_iterator jt = output_set.begin(); 
	       jt != output_set.end();
	       ++jt)
	    {
	      if (is_pair(*it,*jt))
		{ 
		  pair_transducer.apply(&HfstTransducer::disjunct,
					OtherSymbolTransducer(*it,*jt));
		}
	    }
	}
    }
  alphabet[pair] = pair_transducer;
  return alphabet[pair];
}

void Alphabet::define_alphabet_pair(const SymbolPair &pair)
{ 
  alphabet_set.insert(pair); 
  input_symbols.insert(pair.first);
  output_symbols.insert(pair.second);
}

void Alphabet::define_diacritics(const SymbolRange &diacs)
{ 
  diacritics.insert(diacs.begin(),diacs.end()); 
  for (HandySet<std::string>::iterator it = diacritics.begin();
       it != diacritics.end();
       ++it)
    { 
      alphabet_set.erase(SymbolPair(*it,*it)); 
      input_symbols.erase(*it);
      output_symbols.erase(*it);
    }
}

void Alphabet::alphabet_done(void)
{ OtherSymbolTransducer::set_symbol_pairs(alphabet_set); }

#ifdef TEST_ALPHABET
const char * arr1[2] = { "a","b" };
const char * arr2[3] = { "a","b","c" };
int main(void)
{
  OtherSymbolTransducer::set_transducer_type(hfst::TROPICAL_OPENFST_TYPE);
  Alphabet alphabet;
  std::string name1 = "X";
  SymbolRange sr1(arr1,arr1+2);
  std::string name2 = "Y";
  SymbolRange sr2(arr2,arr2+3);
  std::string name3 = "Z";
  SymbolRange sr3(arr1,arr1+1);

  alphabet.define_alphabet_pair(SymbolPair("a","b"));
  alphabet.define_alphabet_pair(SymbolPair("b","c"));
  alphabet.define_alphabet_pair(SymbolPair("b","b"));
  alphabet.alphabet_done();

  alphabet.define_set(name1,sr1);
  alphabet.define_set(name2,sr2);
  alphabet.define_set(name3,sr3);

  std::cout << 
    alphabet.get_transducer(SymbolPair(name1,name2)).get_transducer() 
	    << std::endl;

  std::cout << 
    alphabet.get_transducer(SymbolPair(name3,TWOLC_UNKNOWN)).get_transducer() 
	    << std::endl;

}
#endif
