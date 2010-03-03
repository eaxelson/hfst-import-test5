
#ifndef _ALPHABET_H_
#define _ALPHABET_H_

#include <cstring>
#include <cstdio>

#include "basic.h"

#include <set>
#include <vector>
#include <map>

#include "parse_flag_diacritics.h"

using std::set;
using std::map;
using std::vector;

#include <iostream>
using std::ostream;
using std::istream;

// ADDED
#include <fstream>
using std::ifstream;
using std::ofstream;

#ifndef CODE_DATA_TYPE
typedef unsigned short Character;  // data type of the symbol codes
#else
typedef unsigned CODE_DATA_TYPE Character;
#endif


// data type used to indicate whether some action is to be performed
// on the analysis level (lower) or the surface level (upper)
typedef enum {upper, lower} Level;

#include <ext/hash_set>
#include <ext/hash_map>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
using __gnu_cxx::hash_set;

extern char EpsilonString[]; // holds the symbol representing the empty string
                             // which is usually "<>"



typedef unsigned short Key;
typedef set<Key> KeySet;

/*****************  class Label  **************************/
// (The implementation of KeyPair and SymbolPair)

class Label {

 private:
  // data structure where the two symbols are stored
  struct {
    Character lower;
    Character upper;
    float weight;
  } label;


 public:
  static const Character epsilon=0; // code of the empty symbol

  // Returns the constant symbol 'epsilon'
  Character get_epsilon() { return epsilon; };

  // new label with two identical symbols
  Label( Character c=epsilon ) { label.lower = label.upper = c; label.weight=0; };

  // new label with two different symbols
  Label( Character c1, Character c2 )
    { label.lower = c1; label.upper = c2; label.weight=0; };

  // new label with two different symbols
  Label( Character c1, Character c2, float weight )
    { label.lower = c1; label.upper = c2; label.weight = weight; };

  // returns the indicated symbol of the label
  Character get_char( Level l ) const
    { return ((l==upper)? label.upper: label.lower); };

  // returns the "upper" symbol of the label (i.e. the surface symbol)
  Character upper_char() const {  return label.upper; };

  // returns the "lower" symbol of the label (i.e. the analysis symbol)
  Character lower_char() const {  return label.lower; };

  // ADDED
  float get_weight() const {  return label.weight; };

  // replaces symbols in a label
  Label replace_char( Character c, Character nc, bool ignore_epsilons=false ) const {
    Label l = *this;
    if (ignore_epsilons)
      if (label.lower == Label::epsilon && label.upper == Label::epsilon)
	  return l;
    if (label.lower == c)
      l.label.lower = nc;
    if (label.upper == c)
      l.label.upper = nc;
    return l;
  };

  // replaces symbols in a label
  Label replace_char( KeySet *ks, Character nc ) const {
    Label l = *this;
    if (ks->find(label.lower) != ks->end())
      l.label.lower = nc;
    if (ks->find(label.upper) != ks->end())
      l.label.upper = nc;
    return l;
  };

  // operators checking the equality of labels
  int operator==( Label l ) const
    { return (label.lower==l.label.lower && label.upper==l.label.upper); };
  int operator!=( Label l ) const
    { return !(l == *this); };

  // comparison operator needed for sorting labels
  int operator<( Label l ) const { 
    return (upper_char() < l.upper_char()); };

  // check whether the label is epsilon (i.e. both symbols are epsilon)
  // transitions with epsilon labels are epsilon transitions
  int is_epsilon() const
    { return (label.upper == epsilon && label.lower == epsilon); };

  // check whether the "upper" symbol is epsilon
  int upper_is_epsilon() const
    { return (label.upper == epsilon); };

  // check whether the "lower" symbol is epsilon
  int lower_is_epsilon() const
    { return (label.lower == epsilon); };

  // hash function needed to store labels in a hash table
  struct label_hash {
    size_t operator() ( const Label l ) const {
      return (size_t)l.lower_char() ^ 
	((size_t)l.upper_char() << 16) ^
	((size_t)l.upper_char() >> 16);
    }
  };

  // hash function needed to store labels in a hash table
  struct label_cmp {
    bool operator() ( const Label l1, const Label l2 ) const {
      return (l1.lower_char() < l2.lower_char() ||
	      (l1.lower_char() == l2.lower_char() && 
	       l1.upper_char() < l2.upper_char()));
    }
  };

  // comparison operator needed to store labels in a hash table
  struct label_eq {
    bool operator() ( const Label l1, const Label l2 ) const {
      return (l1.lower_char() == l2.lower_char() &&
	      l1.upper_char() == l2.upper_char());
    }
  };
};

// --- Datatypes for Symbols ---
typedef unsigned short Symbol;
typedef set<unsigned short> SymbolSet;
typedef set<unsigned short>::iterator SymbolIterator;


typedef vector<Label> Analysis;



// hash function needed to store label pointers in a hash table
struct pointer_label_cmp {
  bool operator() ( const Label *l1, const Label *l2 ) const {
    return (l1->lower_char() < l2->lower_char() ||
	    (l1->lower_char() == l2->lower_char() && 
	     l1->upper_char() < l2->upper_char()));
  }
};

// --- Datatypes for Symbol Pairs ---
typedef Label SymbolPair;
//typedef Alphabet::LabelSet SymbolPairSet;
//typedef Alphabet::LabelSet::iterator SymbolPairIterator;
typedef set<Label*> SymbolPairSet;
typedef set<Label*, pointer_label_cmp>::iterator SymbolPairIterator;

// --- Datatypes for Keys ---
//typedef unsigned short Key;
//typedef set<Key> KeySet;
typedef vector<Key> KeyVector;
typedef vector<KeyVector*> KeyVectorVector;
typedef set<Key>::iterator KeyIterator;


// --- Datatypes for Key Pairs ---
typedef Label KeyPair;
typedef vector<KeyPair*> KeyPairVector;
//typedef Alphabet::LabelSet KeyPairSet;
//typedef Alphabet::LabelSet::iterator KeyPairIterator;
typedef set<Label*, pointer_label_cmp> KeyPairSet;
typedef set<Label*, pointer_label_cmp>::iterator KeyPairIterator;



class KeyTable;

/*****************  class Alphabet  *******************************/
// (The implementation of Global symbol table and written KeyTables)

class Alphabet {

  // string comparison operators needed to stored strings in a hash table
  struct eqstr {
      bool operator()(const char* s1, const char* s2) const {
	  return strcmp(s1, s2) == 0;
      }
  };

 public: 
  // data structure storing labels without repetitions (i.e. as a set)
  // (The implementation of KeyPairSet and SymbolPairSet)
  typedef set<Label, Label::label_cmp> LabelSet;
  
 private:
  // data structure storing labels without repetitions (i.e. as a set)
  //typedef set<Label, Label::label_cmp> LabelSet;

  // hash table used to map the symbols to their codes
  typedef hash_map<const char*, Character, hash<const char*>,eqstr> SymbolMap;

  // hash table used to map the codes back to the symbols
  typedef hash_map<Character, char*> CharMap;


 private:
  SymbolMap sm; // maps symbols to codes

  CharMap  cm; // maps codes to symbols
  LabelSet ls; // set of labels known to the alphabet

  // add a new symbol with symbol code c
  void add( const char *symbol, Character c );

 public:
  bool utf8;


  // HFST version 2.0 functions (added by Erik Axelson)

  bool is_symbol( const char *s );
  bool is_symbol( unsigned short p );
  Symbol create_symbol();
  Symbol create_symbol( unsigned short int );
  Symbol set_symbol_name( char*, Symbol );
  Symbol add_symbol_name( char*, Symbol );
  const char * get_symbol_name( Symbol s );
  unsigned short get_symbol_number( Symbol s );
  Symbol get_symbol( const char *s );
  Symbol get_symbol( unsigned short p );
  bool is_equal( Symbol s1, Symbol s2 );

  void add_epsilon();

  //typedef CharMap::iterator SymbolIterator;
  //SymbolIterator begin_sigma() { return cm.begin(); }
  //SymbolIterator end_sigma() { return cm.end(); }
  //size_t size_sigma() { return cm.size(); }
  //Symbol get_symbol( SymbolIterator si ) { return si->first; }

  // ADDED
  void add_parallel_name( const char *s, unsigned short p );

  void add_primary_symbols( KeyTable *T, Alphabet& alpha );

  KeyTable *to_key_table( Alphabet& alpha );

  hash_map<Key, vector<char*> > to_symbol_table(); 

  void write_in_text_format(ostream& os); 

  vector< std::pair<char*,Key> > to_text_format();

  vector< vector<char*>* > * symbols_to_vector() const;

  void print();

  vector<KeyPair> string_to_keypair_vector(const char *str);

  // UPDATE CALLS TO OLD ITERATOR FUNCTIONS!
  // iterator over the set of known symbols
  /*typedef CharMap::iterator char_iterator;
  char_iterator ci_begin() {return cm.begin(); };
  char_iterator ci_end() {return cm.end(); };
  size_t ci_size() { return cm.size(); };*/


  // iterators over the set of known labels
  typedef LabelSet::iterator iterator;
  typedef LabelSet::const_iterator const_iterator;
  Alphabet();
  ~Alphabet() { clear(); };
  const_iterator begin() const { return ls.begin(); };
  const_iterator end() const { return ls.end(); };
  size_t size() const { return ls.size(); };

  void clear();
  void clear_char_pairs() { ls.clear(); };

  // lookup a label in the alphabet
  iterator find( Label l ) { return ls.find(l); };

  // insert a label in the alphabet
  void insert( Label l ) { if (!l.is_epsilon()) ls.insert(l); };

  // Insert p into the set of symbol pairs
  static bool insert_pair( Label p, LabelSet& Pi ) {
    if (!p.is_epsilon()) {
      Pi.insert(p);
      return true;
    }
    return false;
  }

  // Is p in Pi
  static bool has_pair( Label p, LabelSet& Pi ) {
    return (Pi.find(p) != Pi.end());
  }

  // insert the known symbols from another alphabet
  void insert_symbols( const Alphabet& );

  // Insert Symbol Pairs to the alphabet
  void insert_symbol_pairs( LabelSet& Pi );

  // insert the labels and known symbols from another alphabet
  void copy( const Alphabet& );

  // create the alphabet of a transducer obtained by a composition operation
  void compose( const Alphabet &la, const Alphabet &ua );

  // add a symbol to the alphabet and return its code
  Character add_symbol(const char *symbol);

  // add a symbol to the alphabet with a given code
  void add_symbol(const char *symbol, Character c );

  // added by Erik Axelson
  // for hfst-calculate
  void read_symbol_table_text( istream& is );

  // create a new marker symbol and return its code
  Character new_marker( void );
  void delete_markers();

  // compute the complement of a symbol set
  void complement( vector<Character> &sym );
  
  // return the code of the argument symbol
  int symbol2code( const char *s ) const { 
    SymbolMap::const_iterator p = sm.find(s);
    if (p != sm.end()) return p->second;
    return EOF;
  };

  // added by Erik Axelson
  bool name2number( const char *n, unsigned short& p ) {
    SymbolMap::const_iterator it = sm.find(n);
    if (it != sm.end()) {
      p = it->second;
      return true;
    }
    return false;
  }

  // added by Erik Axelson
  bool number2name( unsigned short p, char *n ) {
    CharMap::const_iterator it=cm.find(p);
    if (it == cm.end())
      return false;
    else {
      n = strdup(it->second);
      return true;
    }
  }


  // return the symbol for the given symbol code
  const char *code2symbol( Character c ) const {
    CharMap::const_iterator p=cm.find(c);
    if (p == cm.end())
      return NULL;
    else
      return p->second;
  };
  

  // write the symbol for the given symbol code into a string
  void write_char( Character c, char *buffer, int *pos,
		   bool with_brackets=true ) const;

  // write the symbol pair of a given label into a string
  void write_label( Label l, char *buffer, int *pos,
		    bool with_brackets=true ) const;

  // write the symbol for the given symbol code into a buffer and return
  // a pointer to it
  // the flag "with_brackets" indicates whether the angle brackets
  // surrounding multi-character symbols are to be printed or not
  const char *write_char( Character c, bool with_brackets=true ) const;

  // write the symbol pair of a given label into a string
  // and return a pointer to it
  const char *write_label( Label l, bool with_brackets=true ) const;

  // scan the next multi-character symbol in the argument string
  int next_mcsym( char*&, int extended=1 );

  // scan the next symbol in the argument string
  int next_code( char*&, int extended=1 );

  // convert a character string into a symbol or label sequence
  void string2symseq( char*, vector<Character>& );
  void string2labelseq( char*, vector<Label>& );

  // scan the next label in the argument string
  Label next_label( char*&, int extended=1 );

  // store the alphabet in the argument file (in binary form)
  void store( FILE* ) const;
  
  // Added by Miikka Silfverberg
  void store( ostream &out ) const;

  // read the alphabet from the argument file
  void read( FILE* );
  void read( BinaryReader &reader ); // Added by Miikka Silfverberg

  // disambiguation and printing of analyses
  int compute_score( Analysis &ana );
  void disambiguate( vector<Analysis> &analyses );
  char *print_analysis( Analysis &ana, bool both_layers );

  friend ostream &operator<<(ostream&, const Alphabet&);
  friend class KeyTable;
};



//typedef Alphabet::SymbolIterator SymbolIterator;

// HFST version 2.0 (added by Erik Axelson)

// Defining and using alphabets of symbol pairs

//typedef Alphabet::PublicLabelSet SymbolPairSet;

// Iterators over symbol pairs
//typedef Alphabet::LabelSet::iterator SymbolPairIterator;




/*****************  class KeyTable  *******************************/

class KeyTable {

 private:
  // hash table used to map transducer numbers to their indices
  //  typedef hash_map<Key, Symbol> KeyMap;
  typedef map<Key, Symbol> KeyMap;
  // has table used to map indices back to their numbers
  //typedef hash_map<Symbol, Key> SymbolMap;
  typedef map<Symbol, Key> SymbolMap;
 private:
  KeyMap km;
  SymbolMap sm;

 public:
  bool is_key( Key i );
  bool is_symbol( Symbol s );
  void associate_key( Key i, Symbol s );
  Key add_symbol(Symbol s);
  Key get_key( Symbol s );
  Symbol get_key_symbol( Key i );
  void print(Alphabet& alpha);
  Key get_unused_key( void );
  bool is_empty();
  // copy contents of T to keytable Works (only?) if keytable is empty
  void merge( KeyTable *T );
  KeySet *get_key_set();
  SymbolSet *get_symbol_set();

  static KeyTable *read( istream& is, Alphabet& alpha ); 	 
  static KeyTable *read_in_text_format( istream& is, Alphabet& alpha );
  void write( ostream& os, Alphabet& alpha ); 	 
  void write_in_text_format(ostream& os, Alphabet &alpha); 
  vector< std::pair<char*,Key> > to_text_format( Alphabet& alpha );

  Alphabet &to_alphabet( Alphabet& alpha );

  static Key harmonize_key( Key old_key, KeyTable *old_table, KeyTable *new_table, Alphabet& alpha );
  static Key harmonize_key_sfst( Key old_key, Alphabet& old_alpha, Alphabet& alpha );

  KeyTable() {}
  KeyTable( vector< std::pair<char*,Key> > mapping, Alphabet& alpha );
  ~KeyTable() { km.clear(); sm.clear(); }

};


/* COMMON ALPHABET DECLARATIONS */

/*namespace COMMON_ALPHABET {

  SymbolPair *define_symbolpair( Symbol s1, Symbol s2 );
  Symbol get_input_symbol( SymbolPair *s );
  Symbol get_output_symbol( SymbolPair *s );

  SymbolPairSet *create_empty_symbolpair_set();
  SymbolPairSet *insert_symbolpair( SymbolPair *p, SymbolPairSet *Pi );
  bool has_symbolpair( SymbolPair *p, SymbolPairSet *Pi );

  SymbolPairIterator begin_pi( SymbolPairSet *Pi );
  SymbolPairIterator end_pi( SymbolPairSet *Pi );
  size_t size_pi( SymbolPairSet *Pi );
  SymbolPair *get_symbolpair( SymbolPairIterator pi );

  }*/

// write the alphabet to the output stream (in readable form)
ostream &operator<<(ostream&, Alphabet&);


#endif

