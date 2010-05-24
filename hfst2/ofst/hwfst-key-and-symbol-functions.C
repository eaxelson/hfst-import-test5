#include "alphabet.h"

extern Alphabet TheAlphabet;

namespace HWFST {


// **********  Defining and using Symbols  **********

Symbol define_symbol( const char *s ) { return TheAlphabet.add_symbol(s); }
bool is_symbol( const char *s ) { return TheAlphabet.is_symbol(s); }
Symbol get_symbol( const char *s ) { return TheAlphabet.get_symbol(s); }
const char * get_symbol_name( Symbol s ) { return TheAlphabet.get_symbol_name(s); }
bool is_equal( Symbol s1, Symbol s2 ) { return TheAlphabet.is_equal(s1, s2); }


// **********  Defining and using alphabets of symbols  **********

SymbolSet *create_empty_symbol_set() { return new SymbolSet(); }
SymbolSet *insert_symbol( Symbol s, SymbolSet *Si ) { Si->insert(s); return Si; }
bool has_symbol( Symbol s, SymbolSet *Si ) { return ( Si->find(s) != Si->end() ); }


// **********  Iterators over Symbols  **********

SymbolIterator begin_sigma_symbol( SymbolSet *Si ) { return Si->begin(); }
SymbolIterator end_sigma_symbol( SymbolSet *Si ) { return Si->end(); }
size_t size_sigma_symbol( SymbolSet *Si ) { return Si->size(); }
Symbol get_sigma_symbol( SymbolIterator Si ) { return (Symbol) *Si; }


// **********  Defining and using Symbol Pairs  **********

SymbolPair *define_symbolpair( Symbol s1, Symbol s2 ) { return new SymbolPair(s1, s2); }
Symbol get_input_symbol( SymbolPair *s ) { return s->lower_char(); }
Symbol get_output_symbol( SymbolPair *s ) { return s->upper_char(); }


// **********  Defining and using alphabets of symbol pairs  **********

SymbolPairSet *create_empty_symbolpair_set() { return new SymbolPairSet(); }
SymbolPairSet *insert_symbolpair( SymbolPair *p, SymbolPairSet *Pi ) {   Pi->insert(p); return Pi; }
bool has_symbolpair( SymbolPair *p, SymbolPairSet *Pi ) { return ( Pi->find(p) != Pi->end() ); }


// **********  Iterators over symbol pairs  **********

SymbolPairIterator begin_pi_symbol( SymbolPairSet *pi ) { return pi->begin(); }
SymbolPairIterator end_pi_symbol( SymbolPairSet *pi ) { return pi->end(); }
size_t size_pi_symbol( SymbolPairSet *pi ) { return pi->size(); }
SymbolPair *get_symbolpair( SymbolPairIterator pi ) { return *pi; }


// **********  Defining the connection between symbols and keys of transducers  **********

KeyTable *create_key_table() { return new KeyTable(); }
bool is_key( Key i, KeyTable *T ) { return T->is_key(i); }
bool is_symbol( Symbol s, KeyTable *T ) { return T->is_symbol(s); }
void associate_key( Key i, KeyTable *T, Symbol s ) { T->associate_key(i, s); }
Key get_key( Symbol s, KeyTable *T ) { return T->get_key(s); }
Key get_unused_key( KeyTable * T ) { return T->get_unused_key(); }
Symbol get_key_symbol( Key i, KeyTable *T ) { return T->get_key_symbol(i); }
  KeySet *get_key_set(KeyTable *T) { return T->get_key_set(); }
  SymbolSet *get_symbol_set(KeyTable *T) { return T->get_symbol_set(); }

void print_key_table(KeyTable *T) { T->print(TheAlphabet); }

KeyTable *read_symbol_table( istream& is, bool binary ) 
{ 
  KeyTable *kt;
  if (binary)
    kt = KeyTable::read(is, TheAlphabet);
  else
    kt = KeyTable::read_in_text_format(is, TheAlphabet);
  return kt; 
}

void write_symbol_table( KeyTable *T, ostream& os, bool binary ) 
{ 
  if (T) {
    if (binary)
      T->write(os, TheAlphabet);
    else
      T->write_in_text_format(os, TheAlphabet);
  }
  else {
    if (binary)
      TheAlphabet.store(os);
    else {
      TheAlphabet.write_in_text_format(os);
    }
  } 	 
}

KeyTable * gather_flag_diacritic_table( KeyTable * kt ) {

  KeyTable * flag_diacritic_table = create_key_table();

  for ( Key k = 1;
	k < kt->get_unused_key();
	++k ) {
    Symbol s = kt->get_key_symbol(k);
    const char * symbol_name = get_symbol_name(s);
    if ( parse_flag_diacritic(symbol_name) ) {
      flag_diacritic_table->associate_key(k,s);
    }
  }
  return flag_diacritic_table;
}


// **********  Defining and using alphabets of keys  **********

KeySet *create_empty_key_set() { return new KeySet(); }
KeySet *insert_key( Key s, KeySet *Si ) { Si->insert(s); return Si; }
bool has_key( Key s, KeySet *Si ) { return ( Si->find(s) != Si->end() ); }


// **********  Iterators over keys  **********

KeyIterator begin_sigma_key( KeySet *Si ) { return Si->begin(); }
KeyIterator end_sigma_key( KeySet *Si ) { return Si->end(); }
size_t size_sigma_key( KeySet *Si ) { return Si->size(); }
Key get_sigma_key( KeyIterator Si ) { return (Key) *Si; }


// **********  Defining and using key pairs  **********

KeyPair *define_keypair( Key s1, Key s2 ) { return new KeyPair(s1, s2); }
KeyPair *define_keypair( Key s ) { return new KeyPair(s,s); }
Key get_input_key( KeyPair *s ) { return s->lower_char(); }
Key get_output_key( KeyPair *s ) { return s->upper_char(); }


// **********  Defining and using alphabets of key pairs  **********

KeyPairSet *create_empty_keypair_set() { return new KeyPairSet(); }
KeyPairSet *insert_keypair( KeyPair *p, KeyPairSet *Pi ) { Pi->insert(p); return Pi; }
bool has_keypair( KeyPair *p, KeyPairSet *Pi ) { return ( Pi->find(p) != Pi->end() ); }


// **********  Iterators over key pairs  **********

KeyPairIterator begin_pi_key( KeyPairSet *pi ) { return pi->begin(); }
KeyPairIterator end_pi_key( KeyPairSet *pi ) { return pi->end(); }
size_t size_pi_key( KeyPairSet *pi ) { return pi->size(); }
KeyPair *get_pi_keypair( KeyPairIterator pi ) { return *pi; }

}
