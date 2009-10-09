
#include "utf8.h"
#include "alphabet.h"

const int BUFFER_SIZE=100000;

// legacy epsilon print name for hfst-calculate
char EpsilonString[]="<>";

#include <climits>

/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::add                                                  */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::add( const char *symbol, Character c )

{
  char *s = strdup(symbol);
  cm[c] = s;
  sm[s] = c;
}


// ADDED
void Alphabet::add_parallel_name( const char *s, unsigned short p ) {
  sm[s] = p;
}

// ADDED
void Alphabet::add_primary_symbols( KeyTable *T, Alphabet& alpha ) {
  for( CharMap::iterator it=cm.begin(); it!=cm.end(); it++ ) {
    //fprintf(stderr, "add_primary_symbols: adding key-symbol pair %hu '%s'?:", it->first, it->second);
    if (T) {
      if (! T->is_key(it->first) ) {
	T->associate_key(it->first, alpha.add_symbol(it->second) );
	//fprintf(stderr, "DONE\n");
      }
      //else
      //fprintf(stderr, "NOT DONE\n");
    }
    else
      alpha.add_symbol(it->second);
  }
  //fprintf(stderr, " add_primary_symbols ending\n");
}

// ADDED 
KeyTable *Alphabet::to_key_table( Alphabet& alpha ) {
  //fprintf(stderr, "########## Alphabet::to_key_table: BEGINS ##########\n");
  KeyTable *T = new KeyTable();
  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c = alpha.add_symbol(it->second);
    T->associate_key(it->first, c);
    //fprintf(stderr, "to_key_table: associating key %hu with symbol number %hu (name %s)\n", it->first, c, it->second);
  }
  for ( SymbolMap::const_iterator it2=sm.begin(); it2!=sm.end(); it2++ ) {
    Character c2 = alpha.add_symbol(it2->first);
    T->associate_key(it2->second, c2);
    //fprintf(stderr, "to_key_table: associating symbol number %hu (name %s) with key %hu\n", c2, it2->first, it2->second);
  }
  //fprintf(stderr, "Alphabet::to_key_table: TheAlphabet is now:\n");
  //alpha.print();
  //fprintf(stderr, "############ Alphabet::to_key_table: ENDS ############\n");
  return T;
}

// Returns a hash_map where at each key the primary name and the secondary
// names (in that order) of that key are listed in a vector.  
hash_map<Key, vector<char*> > Alphabet::to_symbol_table() 
{  
  hash_map<Key, vector<char*> > result;
  // add primary names
  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    vector<char*> names;
    names.push_back(strdup(it->second));
    result[it->first] = names;
  }
  // add secondary names
  for( SymbolMap::const_iterator it2=sm.begin(); it2!=sm.end(); it2++ ) {
    // test that the name is a secondary name
    if ( strcmp(result[it2->second][0], it2->first) != 0 )
      result[it2->second].push_back(strdup(it2->first));
  }
  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::Alphabet                                             */
/*                                                                 */
/*FE****************************************************************/

Alphabet::Alphabet()

{ 
  // utf8 = false;
  utf8 = true;   // CHANGED
  //add(EpsilonString, Label::epsilon); CHANGED
}


// ADDED
void Alphabet::add_epsilon() {
  add(EpsilonString, Label::epsilon);
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::clear                                                */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::clear()

{
  char **s=new char*[cm.size()];
  ls.clear();
  sm.clear();

  size_t i, n=0;
  for( CharMap::iterator it=cm.begin(); it!=cm.end(); it++ )
    s[n++] = it->second;
  cm.clear();

  for( i=0; i<n; i++ )
    free(s[i]);
  delete[] s; // Miikka Silfverberg changed delete to delete[]
}

// ADDED
void Alphabet::print() {

  fprintf(stderr, "\nThe alphabet contains:\n");

  //fprintf(stderr, "number\tname\n");
  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    fprintf(stderr, " %hu\t%s\n", c, s);
  }
  //fprintf(stderr, "name\tnumber\n");
  for ( SymbolMap::const_iterator it2=sm.begin(); it2!=sm.end(); it2++ ) {
    Character c2=it2->second;
    const char *s2=it2->first;
    fprintf(stderr, " %s\t%hu\n", s2, c2);
  }
  //fprintf(stderr, "number:number\n");
  for( LabelSet::const_iterator p=ls.begin(); p!=ls.end(); p++ ) {
    Character cl= p->lower_char();
    Character cu= p->upper_char();
    fprintf(stderr, " %hu:%hu\n", cl, cu);
  }
  fprintf(stderr, "\n");
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::new_marker                                           */
/*                                                                 */
/*FE****************************************************************/

Character Alphabet::new_marker()

{
  // find some unused character code
  for(Character i=1; i!=0; i++)
    if (cm.find(i) == cm.end()) {
      // create a unique identifier string
      char symbol[100];
      sprintf(symbol,">%ld<",(long)i);
      add(symbol, i);
      return i;
    }
  
  throw "Error: too many symbols in transducer definition";
}


/*FA****************************************************************/
/*                                                                 */
/*  is_marker_symbol                                               */
/*                                                                 */
/*FE****************************************************************/

static bool is_marker_symbol( const char *s )

{
  // recogize strings matching the expression ">[0-9]+<"
  if (s != NULL && *s == '>') {
    do { s++; } while (*s >= '0' && *s <= '9');
    if (*s=='<' && *(s+1) == 0 && *(s-1) != '>')
      return true;
  }
  return false;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::delete_markers                                       */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::delete_markers()

{
  vector<char*> sym;
  vector<Character> code;
  vector<Label> label;

  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    if (!is_marker_symbol(s)) {
      sym.push_back(strdup(s));
      code.push_back(c);
    }
  }
    
  for( LabelSet::const_iterator it=begin(); it!=end(); it++ ) {
    Label l=*it;
    if (!is_marker_symbol(code2symbol(l.upper_char())) &&
	!is_marker_symbol(code2symbol(l.lower_char())))
      label.push_back(l);
  }

  clear();

  for( size_t i=0; i<sym.size(); i++ ) {
    add_symbol(sym[i], code[i]);
    free(sym[i]);
  }
  for( size_t i=0; i<label.size(); i++ )
    insert( label[i] );
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::add_symbol                                           */
/*                                                                 */
/*FE****************************************************************/

Character Alphabet::add_symbol(const char *symbol)

{
  if (symbol==NULL) {
    fprintf(stderr, "ERROR: Alphabet::add_symbol: symbol is NULL\n");
    exit(1);
  }
  if (sm.find(symbol) != sm.end())
    return sm[symbol];

  // assign the symbol to some unused character
  //for(Character i=1; i!=0; i++)
  for(Character i=0; i!=65535; i++)  // define as const in a header !!!!!
    if (cm.find(i) == cm.end()) {
      add(symbol, i);
      return i;
    }
  
  throw "Error: too many symbols in transducer definition";
}



/*******************************************************************/
/*                                                                 */ 
/*           HFST version 2.0 alphabet functions                   */
/*           (added by Erik Axelson)                               */
/*                                                                 */
/*******************************************************************/


void Alphabet::read_symbol_table_text( istream& is ) {

  char buf [256];
  char *buffer = buf;
  while (true) {
    is.getline(buffer, 256);
    if ( is.eof() || strcmp(buffer,"") == 0)
      break;
    char *symbol_name = strtok(buffer,"\t");    
    unsigned short symbol_number = (unsigned short) atoi(strtok(NULL, "\0"));
    //fprintf(stderr, "Read symbol '%s' and number '%hu'.\n", symbol_name, symbol_number);
    if (code2symbol(symbol_number) == NULL)
      add_symbol(symbol_name, symbol_number);
    else
      add_parallel_name(symbol_name, symbol_number);
  }
};


Symbol Alphabet::create_symbol() {
  // find an unused symbol position
  for(Symbol i=1; i!=0; i++)
    if (cm.find(i) == cm.end()) {
      cm[i] = NULL;
      return i;
    }  
  throw "Error: too many symbols in transducer definition";
}

Symbol Alphabet::create_symbol( unsigned short p ) {
  // see if 
  if (cm.find(p) == cm.end()) {
    cm[p] = NULL;
    return p;
  }  
  throw "Error: Cannot create a new symbol at a position that is already reserved";
}


Symbol Alphabet::set_symbol_name( char *n, Symbol s ) {

  // check whether the symbol name was previously defined
  unsigned short previous_position;
  bool found = name2number(n, previous_position);
  if (found) {
    if (previous_position == s) {  // A: symbol name has already been inserted at the same position
      if ( strcmp(cm[s], n) == 0 ) // A1: re-inserting the same primary name 
	return s;
      else {                       // A2: making a parallel name the primary name
	cm[s] = strdup(n);
	return s;
      }   
    }

    // B: symbol name has already been inserted at another position -> error
    if (strlen(n) < 60) {
      static char message[100];
      sprintf(message, "ERROR: cannot reinsert symbol '%s' in alphabet at position number %hu\nThe symbol has already been inserted at position number %hu", n, s, previous_position);
      throw message;
    }
    else
      throw "ERROR: symbol has already been inserted in alphabet in another position";
  }
  
  // check whether the symbol number is reserved
  char *previous_name=NULL;
  found = number2name(s, previous_name);
  if (found) {
    if ( previous_name != NULL ) // if a name was defined for the symbol
      sm.erase(previous_name);   // remove the entry 'previous_name -> s'
    sm[strdup(n)] = s;               // insert the entry 'n -> s'
  }
  delete previous_name;
  cm[s] = strdup(n);
  return s;
}


Symbol Alphabet::add_symbol_name( char *n, Symbol s ) {

  // check if symbol s has a primary name
  char *primary_name=NULL;
  bool found = number2name(s, primary_name);
  if( !found || primary_name==NULL ) {
    fprintf(stderr, "\nERROR: Cannot add a parallel name to symbol number %hu. The symbol does not have a primary name.\n", s);
    exit(1);
  }
  delete primary_name;
  
  // check if the name is already in use
  unsigned short previous_number;
  found = name2number(n, previous_number);
  if (found) {
    if (previous_number == s) // re-inserting the same parallel name or making the primary name a parallel name -> no effect
      return s;
    fprintf(stderr, "\nERROR: Cannot add '%s' as a parallel name for symbol %hu. The name is already reserved for symbol %hu.\n", n, s, previous_number);
    exit(1);
  }
  
  sm[strdup(n)] = s;
  return s;
}


bool Alphabet::is_symbol( const char *s ) {
  SymbolMap::const_iterator it = sm.find(s);
  if (it != sm.end())
    return true;
  return false;
}

bool Alphabet::is_symbol( unsigned short p ) {
  CharMap::const_iterator it=cm.find(p);
  if (it == cm.end())
    return false;
  return true;
}

const char * Alphabet::get_symbol_name( Symbol s ) {
  return code2symbol(s);
  /*char *name=NULL;
  bool found = number2name(s, name);
  if (found)
    return name;
  fprintf(stderr, "\nFATAL: 'get_symbol_name( Symbol s )': Symbol number %hu not defined.\n", s);
  exit(1);*/
}

unsigned short Alphabet::get_symbol_number( Symbol s ) {
  return s;
}

Symbol Alphabet::get_symbol( const char *s ) {
  unsigned short number;
  bool found = name2number(s, number);
  if (found)
    return number;
  fprintf(stderr, "\nERROR: In function 'get_symbol( char *s )': Print name '%s' does not indicate a symbol.\n", s);
  exit(1);
}

Symbol Alphabet::get_symbol( unsigned short p ) {
  char *name=NULL;
  bool found = number2name(p, name);
  if (found) {
    delete name;
    return p;
  }
  fprintf(stderr, "\nERROR: In function 'get_symbol( unsigned short p )': Position %hu does not refer to a symbol.\n", p);
  exit(1);
}

bool Alphabet::is_equal( Symbol s1, Symbol s2 ) {
  return ( s1 == s2 );
}



/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::add_symbol                                           */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::add_symbol( const char *symbol, Character c )

{
  // check whether the symbol was previously defined
  int sc=symbol2code(symbol);
  if (sc != EOF) {
    if ((Character)sc == c)
      return;

    if (strlen(symbol) < 60) {
      static char message[100];
      sprintf(message, "Error: reinserting symbol '%s' in alphabet with incompatible character value %u %u", symbol, (unsigned)sc, (unsigned)c);
      fprintf(stderr, "Error: reinserting symbol '%s' in alphabet with incompatible character value %u %u", symbol, (unsigned)sc, (unsigned)c);
      throw message;
    }
    else
      throw "reinserting symbol in alphabet with incompatible character value";
  }

  // check whether the character is already in use
  const char *s=code2symbol(c);
  if (s == NULL)
    add(symbol, c);
  else {
    if (strcmp(s, symbol) != 0) {
      static char message[100];
      if (strlen(symbol) < 70) {
	sprintf(message,"Error: defining symbol %s as character %d (previously defined as %s)", symbol, (unsigned)c, s);
	fprintf(stderr,"Error: defining symbol %s as character %d (previously defined as %s)", symbol, (unsigned)c, s);
      }
      else
	sprintf(message,"Error: defining a (very long) symbol with previously used character");
      throw message;
    }
  }
}



/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::write_char                                           */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::write_char( Character c, char *buffer, int *pos, 
			   bool with_brackets) const
{
  const char *s = code2symbol(c);

  if (s) {
    int i = 0;
    int l=strlen(s)-1;
    if (!with_brackets && s[i] == '<' && s[l] == '>') { i++; l--; }
    while (i <= l)
      buffer[(*pos)++] = s[i++];
  }
  else {
    unsigned int uc = c;
    if (uc>=32 && uc<256)
      buffer[(*pos)++] = (char)c;
    else {
      sprintf(buffer+(*pos),"\\%u", uc);
      *pos += strlen(buffer+(*pos));
    }
  }
  buffer[*pos] = '\0';
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::write_char                                           */
/*                                                                 */
/*FE****************************************************************/

const char *Alphabet::write_char( Character c, bool with_brackets ) const

{
  static char buffer[1000];
  int n=0;

  write_char( c, buffer, &n, with_brackets );
  return buffer;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::write_label                                          */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::write_label( Label l, char *buffer, int *pos, 
			    bool with_brackets ) const
{
  Character lc=l.lower_char();
  Character uc=l.upper_char();
  write_char( lc, buffer, pos, with_brackets );
  if (lc != uc) {
    buffer[(*pos)++] = ':';
    write_char( uc, buffer, pos, with_brackets );
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::write_label                                          */
/*                                                                 */
/*FE****************************************************************/

const char *Alphabet::write_label( Label l, bool with_brackets  ) const

{
  static char buffer[1000];
  int n=0;
  write_label( l, buffer, &n, with_brackets );
  return buffer;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::insert_symbols                                       */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::insert_symbols( const Alphabet &a )

{
  //fprintf(stderr, "Alphabet::insert_symbols: \n");
  for( CharMap::const_iterator it=a.cm.begin(); it!=a.cm.end(); it++ ) {
    add_symbol(it->second, it->first);
    //fprintf(stderr, " added symbol %s at index %hu\n", it->second, it->first);
  }
  // ADDED
  for( SymbolMap::const_iterator it=a.sm.begin(); it!=a.sm.end(); it++ )
    sm[it->first] = it->second;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::complement                                           */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::complement( vector<Character> &sym )

{
  vector<Character> result;
  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c = it->first;
    if (c != Label::epsilon) {
      size_t i;
      for( i=0; i<sym.size(); i++ )
	if (sym[i] == c)
	  break;
      if (i == sym.size())
	result.push_back(c);
    }
  }
  sym.swap(result);
}

// Insert Symbol Pairs to the alphabet
void Alphabet::insert_symbol_pairs( LabelSet& Pi ) {
  for( LabelSet::const_iterator it=Pi.begin(); it!=Pi.end(); it++ )
    ls.insert( *it );
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::copy                                                 */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::copy( const Alphabet &a )

{
  insert_symbols( a );
  utf8 = a.utf8;
  for( LabelSet::const_iterator it=a.begin(); it!=a.end(); it++ )
    ls.insert( *it );
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::compose                                              */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::compose( const Alphabet &la, const Alphabet &ua )

{
  // insert the symbols
  insert_symbols(la);
  insert_symbols(ua);
  utf8 = la.utf8;

  hash_map<Character, hash_set<Character> > cs;

  // create a hash table for a quick lookup of the target characters
  for( iterator it=ua.begin(); it!=ua.end(); it++ ) {
    Character lc=it->lower_char();
    if (lc == Label::epsilon)
      insert(*it);
    else
      cs[lc].insert(it->upper_char());
  }

  for( iterator it=la.begin(); it!=la.end(); it++ ) {
    Character uc=it->upper_char();
    if (uc == Label::epsilon)
      insert(*it);
    else {
      if (cs.find(uc) != cs.end()) {
	hash_set<Character> s=cs[uc];
	Character lc=it->lower_char();
	for( hash_set<Character>::iterator it=s.begin(); it!=s.end(); it++)
	  insert(Label(lc, *it));
      }
    }
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  operator<<(Alphabet)                                           */
/*                                                                 */
/*FE****************************************************************/

ostream &operator<<( ostream &s, const Alphabet &a )

{
  for( Alphabet::CharMap::const_iterator it=a.cm.begin(); it!=a.cm.end(); it++ )
    s << it->first << " -> " << it->second << "\n";
  for( Alphabet::iterator it=a.begin(); it!=a.end(); it++ )
    s << a.write_label(*it) << " ";
  s << "\n";
  return s;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::next_mcsym                                           */
/*                                                                 */
/*  recognizes multi-character symbols which are enclosed with     */
/*  angle brackets <...>. If the value of the argument flag is     */
/*  smaller than 2, the multi-character symbol must be already in  */
/*  the lexicon in order to be recognized.                         */
/*                                                                 */
/*FE****************************************************************/

int Alphabet::next_mcsym( char* &string, int extended )

{

  char *start=string;

  if (*start == '<')
    // symbol might start here
    for( char *end=start+1; *end; end++ )
      if (*end == '>') {
	// matching pair of angle brackets found
	// mark the end of the substring with \0
	char lastc = *(++end);
	*end = 0;

	int c;
	if (extended <= 2) {
	  c = add_symbol( start );
	}
	else {
	  // fprintf(stderr, "next_mcsym: calling 'symbol2code' with argument: '%s'\n", start);
	  c = symbol2code(start);
	}
	// restore the original string
	*end = lastc;

	if (c != EOF) {
	  // symbol found
	  // return its code
	    //fprintf(stderr, "next_mcsym: symbol code found: %i\n", c);
	  string = end;
	  return (Character)c;
	}
	else {
	  // not a complex character
	    //fprintf(stderr, "next_mcsym: symbol not found: '%s'\n", start);
	  break;
	}
      }
  return EOF;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::next_code                                            */
/*                                                                 */
/*FE****************************************************************/

int Alphabet::next_code( char* &string, int extended )

{
  if (*string == 0)
    return EOF; // finished

  int c = next_mcsym(string, extended);
  if (c != EOF)
    return c;

  if (extended && *string == '\\') {
    string++; // remove quotation

    // ADDED: '\n' and '\t' handled here
    if ( *string == 'n' || *string == 't' ) {
      char buffer[2];
      if ( *string == 'n' )
	buffer[0] = '\n';
      else
	buffer[0] = '\t';
      buffer[1] = 0;
      string++;
      return (int)add_symbol(buffer);
    }
  }

  if (utf8) {
    unsigned int c = utf8_to_int( &string );
    return (int)add_symbol(int2utf8(c));
  }
  else {
    char buffer[2];
    buffer[0] = *string;
    buffer[1] = 0;
    string++;
    return (int)add_symbol(buffer);
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::next_label                                           */
/*                                                                 */
/*FE****************************************************************/

Label Alphabet::next_label( char* &string, int extended )

{

  //fprintf(stderr, "next_label starts: input string is '%s'\n", string);
  // read first character
  int c = next_code( string, extended );
  if (c == EOF)
    return Label(); // end of string reached

  Character lc=(Character)c;
  if (!extended || *string != ':') { // single character?
    if (lc == Label::epsilon)
      return next_label(string); // ignore epsilon
    return Label(lc);
  }

  // read second character
  string++; // jump over ':'
  c = next_code( string );
  if (c == EOF) {
    static char buffer[1000];
    sprintf(buffer,"Error: incomplete symbol in input file: %s", string);
    throw buffer;
  }

  Label l(lc, (Character)c);
  if (l.is_epsilon())
    return next_label(string); // ignore epsilon transitions
  return l;
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::string2symseq                                        */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::string2symseq( char *s, vector<Character> &ch )

{
  int c;
  while ((c = next_code(s, false)) != EOF)
    ch.push_back((Character)c);
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::string2labelseq                                      */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::string2labelseq( char *s, vector<Label> &labels )

{
  Label l;
  while ((l = next_label(s)) != Label::epsilon)
    labels.push_back(l);
}


/* Added by Erik Axelson. */

vector< vector<char*>* > * Alphabet::symbols_to_vector() const {

  vector< vector<char*>* > * symbol_vector = new vector< vector<char*>* >();

  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    vector<char*> * new_vector = new vector<char*>();
    symbol_vector->push_back( new_vector );
    //fprintf(stderr, "  inserting '%s' at position %hu..\n", s, c);
    symbol_vector->at(c)->push_back(strdup(s));
  }

  for ( SymbolMap::const_iterator it=sm.begin(); it!=sm.end(); it++ ) {
    Character c2=it->second;
    char *s2=strdup(it->first);
    if ( strcmp(s2, symbol_vector->at(c2)->at(0)) != 0 ) // a secondary name
      symbol_vector->at(c2)->push_back(s2);
  }
  
  return symbol_vector;
} 


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::store                                                */
/*                                                                 */
/*FE****************************************************************/


void Alphabet::store( FILE *file ) const

{
  char c=(utf8)? 1: 0;
  fputc(c, file);

  // write the symbol mapping
  //Character n=cm.size();
  Character n=sm.size(); // secondary names are stored, too
  fwrite(&n, sizeof(n), 1, file);

  /*vector<vector<char*>* > * symbol_vector = symbols_to_vector();
  
  for (unsigned int i=0; i < symbol_vector->size(); i++) {
    Character c = i;
    for (unsigned int j=0; j < symbol_vector->at(i)->size(); j++) {
      char *s = symbol_vector->at(i)->at(j);
      fwrite(&c, sizeof(c), 1, file);
      fwrite(s, sizeof(char), strlen(s)+1, file);
      //delete s; ?
    }
    // delete symbol_vector->at(i); ?
    }*/
  // delete symbol_vector; ?

  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    fwrite(&c, sizeof(c), 1, file);
    fwrite(s, sizeof(char), strlen(s)+1, file);
    // ADDED
    for ( SymbolMap::const_iterator it2=sm.begin(); it2!=sm.end(); it2++ ) {
      Character c2=it2->second;
      char *s2=strdup(it2->first);
      if ( strcmp(s2, s) != 0  &&  c2 == c ) { // a secondary name
	fwrite(&c2, sizeof(c2), 1, file);
	fwrite(s2, sizeof(char), strlen(s2)+1, file);
      }
      delete s2;
    }
  }

  // write the character pairs
  n = size();
  fwrite(&n, sizeof(n), 1, file);
  for( LabelSet::const_iterator p=ls.begin(); p!=ls.end(); p++ ) {
    Character c=p->lower_char();
    fwrite(&c, sizeof(c), 1, file);
    c = p->upper_char();
    fwrite(&c, sizeof(c), 1, file);
  }

  if (ferror(file))
    throw "Error encountered while writing alphabet to file\n";
}

/*void Alphabet::store( ostream &out ) const

{
  char c=(utf8)? 1: 0;
  out.put(c);

  // write the symbol mapping
  Character n=cm.size();
  out.write((char*)(&n), sizeof(n));
  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    out.write((char*)(&c), sizeof(c));
    out.write(s, sizeof(char)*(strlen(s)+1));
  }

  // write the character pairs
  n = size();
  out.write((char*)(&n), sizeof(n));
  for( LabelSet::const_iterator p=ls.begin(); p!=ls.end(); p++ ) {
    Character c=p->lower_char();
    out.write((char*)(&c), sizeof(c));
    c = p->upper_char();
    out.write((char*)(&c), sizeof(c));
  }

  if (out.bad())
    throw "Error encountered while writing alphabet to file\n";
}*/


/*#################################################################*/
/*# TALLENNETAAN AAKKOSTO*/
/*# alphabet.h ja alphabet.C*/
/*#################################################################*/
//Added by Miikka Silfverberg
void Alphabet::store( ostream &out ) const

{
  char c=(utf8)? 1: 0;
  out.put(c);

  // write the symbol mapping
  //Character n=cm.size();
  Character n=sm.size(); // secondary names are stored, too
  out.write((char*)(&n), sizeof(n));

  /*vector<vector<char*>* > * symbol_vector = symbols_to_vector();

  for (unsigned int i=0; i < symbol_vector->size(); i++) {
    Character c = i;
    for (unsigned int j=0; j < symbol_vector->at(i)->size(); j++) {
      char *s = symbol_vector->at(i)->at(j);
      out.write((char*)(&c), sizeof(c));
      out.write(s, sizeof(char)*(strlen(s)+1));
    }
    }*/

  for( CharMap::const_iterator it=cm.begin(); it!=cm.end(); it++ ) {
    Character c=it->first;
    char *s=it->second;
    out.write((char*)(&c), sizeof(c));
    out.write(s, sizeof(char)*(strlen(s)+1));
    // ADDED
    for ( SymbolMap::const_iterator it2=sm.begin(); it2!=sm.end(); it2++ ) {
      Character c2=it2->second;
      char *s2=strdup(it2->first);
      if ( strcmp(s2, s) != 0  &&  c2 == c ) { // a secondary name
	out.write((char*)(&c2), sizeof(c2));
	out.write(s2, sizeof(char)*(strlen(s2)+1));
      }
      free(s2);
    }
  }

  // write the character pairs
  n = size();
  out.write((char*)(&n), sizeof(n));
  for( LabelSet::const_iterator p=ls.begin(); p!=ls.end(); p++ ) {
    Character c=p->lower_char();
    out.write((char*)(&c), sizeof(c));
    c = p->upper_char();
    out.write((char*)(&c), sizeof(c));
  }

  if (out.bad())
    throw "Error encountered while writing alphabet to file\n";
}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::read                                                 */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::read( FILE *file )

{

  utf8 = (fgetc(file) != 0);
  // read the symbol mapping
  Character n=0;
  read_num(&n, sizeof(n), file);

  Character previous_character=0;

  for( int i=0; i<n; i++) {
    char buffer[BUFFER_SIZE];
    Character c;
    read_num(&c, sizeof(c), file);
    if (!read_string(buffer, BUFFER_SIZE, file) || 
	feof(file) || ferror(file))
      throw "Error1 occurred while reading alphabet!\n";
    // ADDED
    if ( c == previous_character && i>0 ) // a secondary name
      sm[buffer] = c;
    else
      add_symbol(buffer, c);
    previous_character = c;
  }

  // read the character pairs
  read_num(&n, sizeof(n), file);
  if (ferror(file))
    throw "Error2 occurred while reading alphabet!\n";
  for( int i=0; i<n; i++) {
    Character lc, uc;
    read_num(&lc, sizeof(lc), file);
    read_num(&uc, sizeof(uc), file);
    insert(Label(lc, uc));
  }
  if (ferror(file))
    throw "Error3 occurred while reading alphabet!\n";
}

// Added by Miikka Silfverberg.
void Alphabet::read( BinaryReader &reader )

{
  //fprintf(stderr, "\n ##### Alphabet::read: #### \n\n");
  char c;
  reader.read(&c,sizeof(c));
  utf8 = (c != 0);

  Character number_of_symbols = 0;
  reader.read(&number_of_symbols,sizeof(number_of_symbols));
  
  Character previous_character=0;

  for( int i=0; i<number_of_symbols; i++) {
    char buffer[BUFFER_SIZE] = ""; // Buffer where the symbol is stored

    Character c;
    reader.read(&c,sizeof(c));

    reader.read_string(&buffer[0], BUFFER_SIZE - 1);
    // ADDED
    if ( c == previous_character && i>0 ) { // a secondary name
      sm[strdup(buffer)] = c;
      //fprintf(stderr, "Alphabet::read: added the secondary name '%s' for number %hu.\n", buffer, c);
    }
    else {
      add_symbol(buffer, c);
      //fprintf(stderr, "Alphabet::read: added the primary name '%s' for number %hu.\n", buffer, c);
    }
    previous_character = c;
  }

  // read the character pairs
  Character number_of_pairs = 0;
  reader.read(&number_of_pairs,sizeof(number_of_pairs));

  for( unsigned int i=0; i<number_of_pairs; i++) {

    Character lc, uc;
    reader.read(&lc,sizeof(lc));
    reader.read(&uc,sizeof(uc));
    insert(Label(lc, uc));

  }

}


/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::compute_score                                        */
/*                                                                 */
/*FE****************************************************************/

int Alphabet::compute_score( Analysis &ana )

{
  // check whether the morpheme boundaries are explicitly marked
  // with <X> tags
  int score=0;
  for( size_t i=0; i<ana.size(); i++ ) {

    // get next symbol
    const char *sym=write_char(ana[i].lower_char());

    if (strcmp(sym,"<X>") == 0)
      score--;
  }
  if (score <  0)
    return score;

  // No explicit morphome boundary markers have been found.
  // Count the number of part-of-speech and PREF tags.
  for( size_t i=0; i<ana.size(); i++ ) {

    // get next symbol
    const char *sym=write_char(ana[i].lower_char());

    // Is it not a multi-character symbol
    if (sym[0] != '<' || sym[1] == 0)
      continue;

    // Is it a POS tag starting with "+" like <+NN>?
    if (sym[1] == '+') {
      const char *t=sym+2;
      for( ; *t >= 'A' && *t <= 'Z'; t++) ;
      if (t > sym+2 && *t == '>')
	return score;
    }

    // Is it a potential POS tag (i.e. all uppercase)?
    const char *t = sym+1;
    for( ; *t >= 'A' && *t <= 'Z'; t++) ;
    if (t == sym+1 || *t != '>')
      continue;

    // uppercase symbol found
    if (strcmp(sym,"<SUFF>") == 0 ||
	strcmp(sym,"<OLDORTH>") == 0 ||
	strcmp(sym,"<NEWORTH>") == 0)
      continue; // not what we are looking for

    if (strcmp(sym,"<V>") == 0 || strcmp(sym,"<ADJ>") == 0) {
      bool is_verb=(strcmp(sym,"<V>")==0);
      // get the next non-empty symbol
      Character c=Label::epsilon;
      size_t k;
      for( k=i+1; k<ana.size(); k++ )
	if ((c = ana[k].lower_char()) != Label::epsilon)
	  break;
      // Is it a participle
      if (c != Label::epsilon) {
	sym = write_char(c);
	if (strcmp(sym,"<OLDORTH>") == 0 || strcmp(sym,"<NEWORTH>") == 0) {
	  for( k++; k<ana.size(); k++ )
	    if ((c = ana[k].lower_char()) != Label::epsilon)
	      break;
	  if (c != Label::epsilon)
	    sym = write_char(c);
	}
	if (is_verb &&
	    (strcmp(sym,"<PPres>") == 0 || strcmp(sym,"<PPast>") == 0))
	  continue; // don't consider participles as complex
	if (!is_verb &&
	    (strcmp(sym,"<Sup>") == 0 || strcmp(sym,"<Comp>") == 0))
	  continue; // don't consider participles as complex
      }
    }
    score--;
  }
  return score;
}



/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::disambiguate                                         */
/*                                                                 */
/*FE****************************************************************/

void Alphabet::disambiguate( vector<Analysis> &analyses )

{
  // compute the scores
  int bestscore=INT_MIN;
  vector<int> score;

  for( size_t i=0; i<analyses.size(); i++ ) {
    score.push_back(compute_score(analyses[i]));
    if (bestscore < score[i])
      bestscore = score[i];
  }

  // delete suboptimal analyses
  size_t k=0;
  for( size_t i=0; i<analyses.size(); i++ )
    if (score[i] == bestscore)
      analyses[k++] = analyses[i];
  analyses.resize(k);
}



/*FA****************************************************************/
/*                                                                 */
/*  Alphabet::print_analysis                                       */
/*                                                                 */
/*FE****************************************************************/

char *Alphabet::print_analysis( Analysis &ana, bool both_layers )

{
  vector<char> ch;

  // for each transition
  for( size_t i=0; i<ana.size(); i++ ) {

    // get the transition label
    Label l=ana[i];
    const char *s;

    // either print the analysis symbol or the whole label
    if (both_layers)
      s = write_label(l);
    else if (l.lower_char() != Label::epsilon)
      s = write_char(l.lower_char());
    else
      continue;

    // copy the characters to the character array
    while (*s)
      ch.push_back(*(s++));
  }
  ch.push_back(0); // terminate the string

  static char *result=NULL;
  if (result != NULL)
    delete[] result;
  result = new char[ch.size()];
  for( size_t i=0; i<ch.size(); i++ )
    result[i] = ch[i];
  
  return result;
}


vector<KeyPair> Alphabet::string_to_keypair_vector(const char *str) {

  vector<KeyPair> result;
  
  int i=0;
  while (true) {
    std::string isymbol="";
    std::string osymbol="";
    while (str[i] != ':' && str[i] != ' ' && str[i] != '\0') {  // read symbol
      if (str[i] == '\\')
	i++;  // skip over backslash used for escaping colon and backslash
      isymbol.append(1, str[i]);
      i++;
    }
    if (str[i] == ' ' || str[i] == '\0')  // identity pair
      osymbol.append(isymbol);
    else {                                // non-identity pair
      i++;     // skip over colon that separates input and output symbols 
      while (str[i] != ' ' && str[i] != '\0') {
	if (str[i] == '\\')
	  i++;  // skip over backslash used for escaping colon and backslash
	osymbol.append(1, str[i]);
	i++;
      }
    }
    Label l( symbol2code(isymbol.c_str()), 
	     symbol2code(osymbol.c_str()) );
    result.push_back(l);
    if (str[i] == '\0' || str[i+1] == '\0') // end of string or single space before end
      break;
    else
      i++;  // skip over space to next symbol pair
  }

  return result;
}
    
      
      

/**** class KeyTable ****/


bool KeyTable::is_key( Key i ) {
  return ( km.find(i) != km.end() ); 
}

bool KeyTable::is_symbol( Symbol s ) {
  return ( sm.find(s) != sm.end() ); 
}

Key KeyTable::get_unused_key( void ) {
  if ( km.begin() == km.end() )
    return 0;
  Key greatest_key_in_use = km.rbegin()->first;
  return greatest_key_in_use + 1;
}

bool KeyTable::is_empty() {
  return ( km.size() == 0 && sm.size() == 0 );
}

void KeyTable::associate_key( Key i, Symbol s ) {
  SymbolMap::iterator it = sm.find(s);
  if ( it != sm.end() ) {  // a key is already associated with the symbol
    if (it->second != i ) {
      fprintf(stderr, "THROW: Another key %u is already associated with the symbol number %u.\n",it->second,s);
      throw "Another key is already associated with the symbol.";
    }
    else
      return; // associating the same key with the same symbol again
  }
    if (!is_key(i)) // associating the key with a (primary) symbol for the first time
      km[i] = s;
    sm[s] = i;
}


// Get key for symbol s in key table T. If s is not found associate it to
// the next free position.
Key KeyTable::add_symbol(Symbol s) {
  Key number;
  if (is_symbol(s))
    number = get_key(s);
  else {
    number = get_unused_key();
    associate_key(number, s);
  }
  return number;
};

 
Key KeyTable::get_key( Symbol s ) {
  SymbolMap::iterator it = sm.find(s);
  if ( it == sm.end() )
    throw "Symbol name not found.";
  return it->second;
}
 
Symbol KeyTable::get_key_symbol( Key i ) {
  KeyMap::iterator it = km.find(i);
  if ( it == km.end() )
    throw "Symbol number not found.";
  return it->second;
}


KeyTable *KeyTable::read( istream& is, Alphabet& alpha ) {
  BinaryReader reader(is);
  Alphabet alpha_r;
  alpha_r.read(reader);
  KeyTable *kt = alpha_r.to_key_table(alpha);
  return kt;
}

KeyTable *KeyTable::read_in_text_format( istream& is, Alphabet& alpha ) 
{
  KeyTable *kt = new KeyTable();
  char buf [256];
  char *buffer = buf;
  while (true) {
    is.getline(buffer, 256);
    if ( is.eof() || strcmp(buffer,"") == 0)
      break;
    char *symbol_name = strtok(buffer,"\t");    
    unsigned short symbol_number = (unsigned short) atoi(strtok(NULL, "\0"));
    //fprintf(stderr, "Read symbol '%s' and number '%hu'.\n", symbol_name, symbol_number);
    kt->associate_key(symbol_number, alpha.add_symbol(symbol_name));
  }
  return kt;
};

KeySet *KeyTable::get_key_set() {
  KeySet *ks = new KeySet();
  KeyMap::iterator it;
  for (it=km.begin(); it != km.end(); it++)
    ks->insert((*it).first);
  return ks;
};

SymbolSet *KeyTable::get_symbol_set() {
  SymbolSet *ss = new SymbolSet();
  SymbolMap::iterator it;
  for (it=sm.begin(); it != sm.end(); it++)
    ss->insert((*it).first);
  return ss;
}

void KeyTable::write( ostream& os, Alphabet& alpha ) {
  Alphabet alpha_w = to_alphabet(alpha);
  alpha_w.store(os);
}

void KeyTable::write_in_text_format( ostream& os, Alphabet& alpha ) {
  Alphabet alpha_w = to_alphabet(alpha);
  hash_map< Key, vector<char*> > names = alpha_w.to_symbol_table();
  for( hash_map<Key, vector<char*> >::const_iterator it=names.begin();
       it!=names.end(); it++ ) {
    unsigned int key = it->first;
    vector<char*> names = it->second;
    for( unsigned int i=0; i<names.size(); i++ )
      os << names[i] << "\t" << key << "\n";
  }
}

vector< std::pair<char*,Key> > KeyTable::to_text_format( Alphabet& alpha ) {
  vector< std::pair<char*,Key> > result;
  Alphabet alpha_w = to_alphabet(alpha);
  //fprintf(stderr, "alpha_w is now:\n");
  //alpha_w.print();
  hash_map< Key, vector<char*> > names = alpha_w.to_symbol_table();
  for( hash_map<Key, vector<char*> >::const_iterator it=names.begin();
       it!=names.end(); it++ ) {
    unsigned int key = it->first;
    vector<char*> names = it->second;
    for( unsigned int i=0; i<names.size(); i++ ) {
      std::pair<char*,Key> name_and_key = std::pair<char*,Key>(names[i], key);
      result.push_back(name_and_key);
    }
  }
  return result;
}

KeyTable::KeyTable( vector< std::pair<char*,Key> > mapping, Alphabet& alpha) {
  for (unsigned int i=0; i<mapping.size(); i++) {
    char *name = mapping[i].first;
    Key key = mapping[i].second;
    //fprintf(stderr, "KeyTable(...): associating key %hu with symbol for name %s\n", key, name);
    associate_key(key, alpha.add_symbol(name));
  }
};

vector< std::pair<char*,Key> > Alphabet::to_text_format() {
  vector< std::pair<char*,Key> > result;
  hash_map< Key, vector<char*> > names = to_symbol_table();
  for( hash_map<Key, vector<char*> >::const_iterator it=names.begin();
       it!=names.end(); it++ ) {
    unsigned int key = it->first;
    vector<char*> names = it->second;
    for( unsigned int i=0; i<names.size(); i++ ) {
      std::pair<char*,Key> name_and_key = std::pair<char*,Key>(names[i], key);
      result.push_back(name_and_key);
    }
  }
  return result;
}


Key KeyTable::harmonize_key( Key old_key, KeyTable *old_table, KeyTable *new_table, Alphabet& alpha ) 
{
  if (old_key == 0)
    return 0;
  if (!old_table->is_key(old_key)) {  // not recommended
    Key foo = new_table->get_unused_key();
    new_table->associate_key(foo+1, alpha.add_symbol("<the_previous_key_does_not_have_a_name>"));
    return foo;
  }	    
  return new_table->add_symbol(old_table->get_key_symbol(old_key));
}

Key KeyTable::harmonize_key_sfst( Key old_key, Alphabet& old_alpha, Alphabet& alpha )
{
  if (old_key == 0)
    return 0;
  return alpha.add_symbol(old_alpha.code2symbol(old_key));
}

void Alphabet::write_in_text_format( ostream& os ) {

  hash_map< Key, vector<char*> > names = to_symbol_table();
  for( hash_map<Key, vector<char*> >::const_iterator it=names.begin();
       it!=names.end(); it++ ) {
    unsigned int key = it->first;
    vector<char*> names = it->second;
    for( unsigned int i=0; i<names.size(); i++ )
      os << names[i] << "\t" << key << "\n";
  }
};

void KeyTable::print(Alphabet& alpha) {
  fprintf(stderr, "Printing KeyTable...\n");
  //fprintf(stderr, "(TheAlphabet contains:)\n");
  //alpha.print();
  for( KeyMap::iterator it=km.begin(); it!=km.end(); it++ ) {
    Key k=it->first;
    Symbol s=it->second;
    const char *name = alpha.code2symbol(s);
    fprintf(stderr, "key: %hu symbol number: %hu name: %s\n", k, s, name);
  }
  for ( SymbolMap::iterator it=sm.begin(); it!=sm.end(); it++ ) {
    Key k2=it->second;
    Symbol s2=it->first;
    const char *name2 = alpha.code2symbol(s2);
    fprintf(stderr, "\nsymbol name: %s number: %hu key: %hu\n", name2, s2, k2);
  }
  fprintf(stderr, "...KeyTable printed.\n");
};


void KeyTable::merge( KeyTable *T ) 
{
  for( KeyMap::iterator it=T->km.begin(); it!=T->km.end(); it++ ) {
    Key k=it->first;
    Symbol s=it->second;
    associate_key(k, s);
  }
  for ( SymbolMap::iterator it=T->sm.begin(); it!=T->sm.end(); it++ ) {
    Key k=it->second;
    Symbol s=it->first;
    associate_key(k, s);
  }
};


Alphabet &KeyTable::to_alphabet( Alphabet& alpha ) {
  //fprintf(stderr, "########## Alphabet::to_key_table: BEGINS ##########\n");
  //fprintf(stderr, "alphabet parameter is:\n");
  //alpha.print();
  //fprintf(stderr, "\n\n\n");

  Alphabet *alphab = new Alphabet();
  for( KeyMap::iterator it=km.begin(); it!=km.end(); it++ ) {
    Key k=it->first;
    Symbol s=it->second;
    const char *name = alpha.code2symbol(s);
    //fprintf(stderr, "\nadding to result: key: %hu name: %s\n", k, name);
    alphab->add( name, k );
  }
  // TURHAA TYÖTÄ ?
  for ( SymbolMap::iterator it=sm.begin(); it!=sm.end(); it++ ) {
    Key k2=it->second;
    Symbol s2=it->first;
    const char *name2 = alpha.code2symbol(s2);
    //fprintf(stderr, "\nadding to result: name: %s key: %hu\n", name2, k2);
    alphab->add_parallel_name( name2, k2 );       
  }

  //fprintf(stderr, "The result is:\n");
  //alphab->print();
  //fprintf(stderr, "########## Alphabet::to_key_table: ENDS ##########\n");

  return *alphab;
};


/*namespace COMMON_ALPHABET {

SymbolPairSet *create_empty_symbolpair_set() { 
  return new Alphabet::LabelSet(); 
}

SymbolPairSet *insert_symbolpair( SymbolPair *p, SymbolPairSet *Pi ) { 
  Pi->insert(*p);
  return Pi;
}

bool has_symbolpair( SymbolPair *p, SymbolPairSet *Pi ) {
  return ( Pi->find(*p) != Pi->end() );
}

}


namespace COMMON_ALPHABET {

SymbolPairIterator begin_pi( SymbolPairSet *Pi ) {
  return Pi->begin();
}

SymbolPairIterator end_pi( SymbolPairSet *Pi ) {
  return Pi->end();
}

size_t size_pi( SymbolPairSet *Pi ) {
  return Pi->size();
}

SymbolPair *get_symbolpair( SymbolPairIterator pi ) {
  const Label label = *pi;
  return new Label( label.lower_char(), label.upper_char() );
}

}


namespace COMMON_ALPHABET {

SymbolPair *define_symbolpair( Symbol s1, Symbol s2 ) {
  return new Label( s1, s2 );
}

Symbol get_input_symbol( SymbolPair *s ) {
  return s->lower_char();
}

Symbol get_output_symbol( SymbolPair *s ) {
  return s->upper_char();
}
  
}*/
