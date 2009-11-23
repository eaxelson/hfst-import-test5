#include <iostream> 

using std::cin; 
using std::cout;

char * string_copy(const char * str);

/** \defgroup SymbolLayer Symbol Layer
 *
 * Datatypes and functions related to symbols and the relation between symbols
 * and keys.
 */
/*@{*/



/** \name Datatypes for Symbols and Symbol Alphabets
 *
 */
//@{

/** \brief A handle for a symbol name, i.e. a string.

Symbol is the type of a handle for such a symbol that could occur in cell of an
input or output tape or as input or output labels of transitions in
transducers, or of a special-use symbols that do not occur on tapes but occur
only as input or output transition labels having a special interpretation, e.g.
any, default, failure, etc., which is indicated by an attribute of the
transducer.

There is a global, session-spesific table of Symbol-to-string relations, called
the the global symbol cache.  In the symbol cache, one Symbol is associated with one
string and for one string there is one Symbol representing it, i.e. the
relation between strings and Symbols is one-to-one.  */

typedef Symbol Symbol;

/** \brief A set of symbols aka an alphabet of symbols. */
typedef SymbolSet SymbolSet;

/** \brief Iterator over the symbols in a SymbolSet. */
typedef SymbolIterator SymbolIterator;

/** \brief A pair of symbols representing a transition in a transducer. */
typedef SymbolPair SymbolPair;

/** \brief A set of symbol pairs aka an alphabet of symbol pairs. */
typedef SymbolPairSet SymbolPairSet;

/** \brief Iterator over the set of symbol pairs in a SymbolPairSet. */
typedef SymbolPairIterator SymbolPairIterator;

/** \brief A table for storing Key-to-Symbol associations. 

A key can be associated to several symbols but a symbol is associated to only one
key.

*/
typedef KeyTable KeyTable;

//@}




/** \name Defining and Using Symbols
 * 
 */
//@{


/** \brief Define a symbol with name \a s. */
Symbol define_symbol( const char *s );

/** \brief Whether the string \a s indicates a name for a symbol. */
bool is_symbol( const char *s );

/** \brief Find the symbol for the symbol name \a s.

@pre \a s must refer to a symbol name. Use #is_symbol to check this if you are
not sure. */ 
Symbol get_symbol( const char *s );

/** \brief Find the symbol name for the symbol \a s. */
const char * get_symbol_name( Symbol s ); 

/** \brief Whether the symbol \a s1 is identical to symbol \a s2. */
bool is_equal( Symbol s1, Symbol s2 );

//@}



/** \name Defining and Using Alphabets of Symbols
 *
 */
//@{

/** \brief Define an empty set of symbols. */
SymbolSet *create_empty_symbol_set();

/** \brief Insert \a s into the set of symbols \a Si and return the updated set. */
SymbolSet *insert_symbol( Symbol s, SymbolSet *Si );

/** \brief Whether symbol \a s is a member of the set of symbols \a Si. */
bool has_symbol( Symbol s, SymbolSet *Si );

//@}


/** \name Iterators over Symbols
 * 
 */
//@{

/** \brief Beginning of the iterator for the symbol set \a Si. */
SymbolIterator begin_sigma_symbol( SymbolSet *Si );

/** \brief End of the iterator for the symbol set \a Si. */
SymbolIterator end_sigma_symbol( SymbolSet *Si );

/** \brief Size of the iterator for the symbol set \a Si. */
size_t size_sigma_symbol( SymbolSet *Si );

/** \brief Get the symbol pointed by the symbol iterator \a si. */
Symbol get_sigma_symbol( SymbolIterator Si );

//@}


/** \name Defining and Using Symbol Pairs
 * 
 */
//@{

/** \brief Define a symbol pair with input symbol \a s1 and output symbol \a s2. */
SymbolPair *define_symbolpair( Symbol s1, Symbol s2 );

/** \brief Get the input symbol of SymbolPair \a s. */
Symbol get_input_symbol( SymbolPair *s );

/** \brief Get the output symbol of SymbolPair \a s. */
Symbol get_output_symbol( SymbolPair *s );

//@}


/** \name Defining and Using Alphabets of Symbol Pairs
 * 
 */
//@{

/** \brief Define an empty set of symbol pairs. */
SymbolPairSet *create_empty_symbolpair_set();

/** \brief Insert \a p into the set of symbol pairs \a Pi and return the updated set.
 * */
SymbolPairSet *insert_symbolpair( SymbolPair *p, SymbolPairSet *Pi );

/** \brief Whether symbol pair \a p is a member of the set of symbol pairs \a Pi. */
bool has_symbolpair( SymbolPair *p, SymbolPairSet *Pi );

//@}


/** \name Iterators over Symbol Pairs
 * 
 */
//@{

/** \brief Beginning of the iterator for the symbol pair set \a Pi. */
SymbolPairIterator begin_pi_symbol( SymbolPairSet *Pi );

/** \brief End of the iterator for the symbol pair set \a Pi. */
SymbolPairIterator end_pi_symbol( SymbolPairSet *Pi );

/** \brief Size of the iterator for the symbol pair set \a Pi. */
size_t size_pi_symbol( SymbolPairSet *Pi );

/** \brief Get the symbol pair pointed by the symbol pair iterator \a pi. */
SymbolPair *get_pi_symbolpair( SymbolPairIterator pi );

//@}


/** \name Defining the Connection between Symbols and Transducer Keys. 
 * The relation 1:N between keys and symbols is useful for dealing with equivalence
 * classes of symbols.
 */
//@{



/** \brief Create an empty key table.

The result has no associations defined between symbols and keys. */
KeyTable *create_key_table();

/** \brief Whether \a i indicates an existing key in key table \a T. */
bool is_key( Key i, KeyTable *T );

/** \brief Whether \a s indicates an existing symbol in key table \a T. */
bool is_symbol( Symbol s, KeyTable *T );

/** \brief Associate the key \a i in the key table \a T with the symbol \a s.

The symbol that is first associated with a key, becomes the primary symbol for
that key. If key \a i has already been associated with one or more symbol(s)
not equal to \a s, the symbol \a s becomes a parallel symbol for the key \a i.
*/ void associate_key( Key i, KeyTable *T, Symbol s );

/** \brief Find the key for the symbol \a s in key table \a T. */
Key get_key( Symbol s, KeyTable *T );

/** \brief Return a Key which hasn't been associated to any symbol in key 
    table \a T.
*/
Key get_unused_key( KeyTable * T );

/** \brief Find a symbol for the key \a i in key table \a T.

If there are several symbols associated with the key, the primary symbol (the
symbol that was first associated with the key) is returned. */ 
Symbol get_key_symbol( Key i, KeyTable *T );

/** \brief A set of keys in key table \a T. */
KeySet *get_key_set( KeyTable *T );

/** \brief A set of symbols in key table \a T. */
SymbolSet *get_symbol_set( KeyTable *T );


/** \brief Read a symbol table from istream \a is and transform it to a key table. \a binary defines whether the symbol table is in binary or text format. 

Key table and symbol table are two ways of representing key-to-string mappings.
Key tables are used during a session and symbol tables when moving or storing
information between sessions.

During a session, a key table associates keys to symbol handles and the global
symbol cache associates symbol handles to strings.

Between sessions, a symbol table associates keys directly to strings,
as there is no symbol cache.

A symbol table in OpenFst text format lists each symbol name and its associated
key on one line. The symbol name and the associated key are separated by a tabulator.
If several symbol names are associated to the same key, the one listed first is 
considered the primary print name for that key. 

An example:

\verbatim

KeyTable          Global symbol cache      Symbol table            Symbol table in text format     
--------          -------------------      ------------            ---------------------------

Key  Symbol       Symbol    string         Key   string            <> TAB 0
                                                                   <eps> TAB 0
 0     0, 1         0         "<>"          0      "<>", "<eps>"   a TAB 1 
 1     2            1         "<eps>"       1      "a"             b TAB 2
 2     4            2         "a"           2      "b"             c TAB 3
 3     5            3         "A"           3      "c" 
                    4         "b"
                    5         "c"
                    6         "d"

\endverbatim

*/
KeyTable *read_symbol_table( istream& is, bool binary=false ); 	 


/** \brief Transform the key table \a T to a symbol table and write it to ostream \a os. \a binary defines whether the symbol table is written in binary or text format. 

@see read_symbol_table */
void write_symbol_table( KeyTable *T, ostream& os, bool binary=false ); 	 

/** \brief Return a new key table only including those key/symbol pairs which correspond to flag-diacritic symbol names. 

Flag-diacritic symbol names begin and end with an '@'. */
KeyTable * gather_flag_diacritic_table( KeyTable * kt );

/** \brief Return a key table, containing only keys in \a t. 

  You need to harmonize \a t after this operation. */
KeyTable * minimize_key_table(KeyTable * key_table,TransducerHandle t);
//@}




/** \name Reading Symbol Strings and Transducers
 * 
 * Read transducers
 * 
 * (1) in text format from pair strings and input streams and
 * 
 * (2) in binary format from files and input streams so that the keys used in
 * the transducer are harmonized according to a key table.
 *
 */
//@{


/** \brief Create a left to right longest match tokenizer for symbols in key 
 *  set \a ks.
 *
 *  The keytable \a kt should contain the letters which make up the symbols 
 *  for keys in \a ks. The keyset \a ks should not contain the key epsilon! 
 *  The resulting transducer can be composed with other transducers to 
 *  accomplish tokenization.
 */
TransducerHandle longest_match_tokenizer( KeySet * ks, KeyTable * kt );

/** \brief Create a left to right longest match tokenizer for symbols in key 
 *  set \a ks.
 *
 *  The keytable \a kt should contain the letters which make up its 
 *  multicharacter symbols. Tokenization can be accomplished using functions 
 *  \a longest_match_tokenize and \a longest_match_tokenize_pair.
 */
TransducerHandle longest_match_tokenizer2( KeyTable * kt );

/** \brief Replace the epsilon in kt, with epsilon_replacement.

When tokenizing input-strings, the strings should never contain a substring
matching the symbol name of the epsilon key in the KeyTable used in 
tokenization. Therefore the epsilons in the tokenizer should be replaced by
an internal epsilon-symbol, which is unlikely to occur in real input-strings.

recode_key_table returns a KeyTable, which is the same as kt, except the key
0 corresponds to the internal epsilon symbol name epsilon_replacement and the
original epsilon symbol name corresponds to the first unused key in kt.

*/
KeyTable * recode_key_table(KeyTable * kt, const char * epsilon_replacement);

//! @brief Change 2 strings to a transducer aligned character by character
//! according to tokenisation by @a tokeniser.
//! The path(s) of result of composition of of string’s UTF-8 representations
//! against tokeniser are paired up to a new tokeniser from beginning to end.
//! Empty spaces in the end are filled with ε’s
//!
//! E.g. strings @a cat @a dog are aligned as c:d a:o g:t. Strings
//! @a ääliö @a ääliöitä are aligned as ä ä l i ö ε:i ε:t ε:ä. And 
//! @a talo+NOUN+SINGULAR+NOMINATIVE @a talo as t a l o +NOUN:ε +SINGULAR:ε
//! +NOMINATIVE:ε, given that tokeniser and keytable contains those symbols.
//!
//! If specific alignment is required, it is possible to specify ε’s manually
//! using the string for ε that is defined in @a inputKeys.
//!
//! A tokeniser @a tokeniser may be built manually using or with functions, such
//! as @c longestMatchTokeniser(...)
//!
//! @param tokeniser A transducer that, upon composing leftwards against
//!		transducer made of UTF-8 characters of string, results in acyclic
//!		tokenisation(s) of original path.
//! @param upper UTF-8 encoded string for input side of transducer.
//! @param lower UTF-8 encoded string for output side of transducer.
//! @param inputKeys @c KeyTable that matches mapping of UTF-8 characters on 
//!		input side of @a tokeniser.
//! @returns Transducer that contains as paths all possible aligned
//!		tokenisation(s) of @a upper : @a lower.
//! @todo does not support ambiguous tokenisations (i.e. with more than one
//! path.
KeyPairVector * tokenize_string_pair(TransducerHandle tokeniser, const
char * upper, const char * lower, KeyTable* inputKeys);

//! @brief Change a string @a s into identity pair transducer as tokenised by
//! @a tokeniser.
//!
//! E.g. a string cat will be tokenised as transducer c a t, given that
//! @a tokeniser creates tokens for c, a, and t.
//!
//! @param tokeniser A transducer that, upon composing leftwards against
//!		transducer made of UTF-8 characters of string, results in acyclic
//!		tokenisation(s) of original path.
//! @param string UTF-8 encoded string for transducer pairs.
//! @param inputKeys @c KeyTable that matches mapping of UTF-8 characters on 
//!		input side of @a tokeniser.
//! @returns Transducer that contains as paths of @a s tokenised with
//!		@a tokeniser.
//! @todo does not support ambiguous tokenisations (i.e. with more than one
//! path.
KeyVector *  tokenize_string(TransducerHandle tokeniser, 
			     const char * string, 
			     KeyTable* inputKeys);

/** \brief Use \a tokenizer to tokenize \a string.
 * 
 *  The transducer \a tokenizer should be created using the function 
 *  \a longest_match_tokenizer2.
 *  The key table \a inputKeys should contain all characters in \a string and 
 *  be compatible with \a tokenizer.
 */
KeyVector * longest_match_tokenize(TransducerHandle tokenizer, 
				   const char * string, 
				   KeyTable* inputKeys);

/** \brief Use \a tokenizer to tokenize \a string1 and \a string2 and align 
 *  the tokenized strings to a key pair vector.
 * 
 *  The transducer \a tokenizer should be created using the function 
 *  \a longest_match_tokenizer2. The key table \a inputKeys should contain 
 *  all characters in \a string1 and \a string2 and be compatible with 
 *  \a tokenizer. The tokenized strings will be aligned into a key pair vector.
 *  The shorter one of the tokenized strings will be padded with zeroes
 *  at the end.
 */
KeyPairVector * longest_match_tokenize_pair(TransducerHandle tokenizer, 
					    const char * string1, 
					    const char * string2, 
					    KeyTable* inputKeys);

//! @brief Tokenise with @a tokeniser a string @a s 
//! of individual characters and colon separated pairs into transducer.
//!
//! E.g. a string cat+pl:s will be made to c a t +pl:s given that 
//! @a tokeniser creates such tokens.
//!
//! @param tokeniser A transducer that, upon composing leftwards against
//!		transducer made of UTF-8 characters of string, results in acyclic
//!		tokenisation(s) of original path.
//! @param pairs UTF-8 encoded string for transducer
//! @param inputKeys @c KeyTable that matches mapping of UTF-8 characters on 
//!		input side of @a tokeniser.
//! @returns Transducer that contains as paths all possible aligned
//!		tokenisation(s) of @a upper : @a lower.
//! @todo does not support ambiguous tokenisations (i.e. with more than one
//! path.
KeyPairVector * tokenize_pair_string(TransducerHandle tokeniser, char *pairs, KeyTable* inputKeys);

/** \brief Create a one-path transducer as defined in pairstring form in \a str
 * using the symbols defined in key table \a T.

The transitions must be written one after another separated by a space. (For automatic tokenization of symbols, see #tokenize_pair_string.) 
If the input and output symbols are not equal, they are separated by a colon.
If the backslash '\\' and colon ':' are part of a symbol name, they must
be escaped as "\\" and "\:".

For example the string <tt>"a:\: cd:e"</tt> represents a transducer with
consecutive transitions mapping "a" to ":" and "cd" to "e".

@see #transducer_to_pairstring  */ TransducerHandle
pairstring_to_transducer( const char *str, KeyTable *T );


/** \brief Make a transducer as defined in text form in istream \a is using the
 * key-to-printname relations defined in key table \a T. The parameter \a sfst
 * defines whether SFST text format is used, otherwise AT&T format is used. 

In At&T and SFST format, the newline, horizontal tab, carriage return, vertical
tab, formfeed, bell character, backspace, backslash and space must be escaped
as "\n", "\t", "\r", "\v", "\f" "\a", "\b", "\\" and "\0x20".  In SFST format,
the colon and angle brackets must be escaped as "\:", "\<" and "\>".

An example of a transducer file:

\verbatim 
AT&T                                       AT&T UNWEIGHTED               SFST                         

0      0                                   0                             final  0
0      1      a      aa     0.3            0      1      a      aa       0      a:aa   1
0      2      b      b      0              0      2      b      b        0      b      2
1      0      c      C      0.5            1      0      c      C        1      c:C    0
2      1      \n     c      0              2      1      \n     c        2      \n:c   1
2      0      a      A      1.2            2      0      a      A        2      a:A    0
2      2      d      D      1.65           2      2      d      D        2      d:D    2
2      0.5                                 2                             final  2 
\endverbatim     

  The syntax of the lines in the text format is one of the following in the
  AT&T format: 
  - originating_node TAB destination_node TAB input_symbol TAB
  output_symbol (TAB transition_weight)
  - final_node (TAB final_weight)

  and one of the following in sfst format: 
  - originating_node TAB input_symbol:output_symbol TAB destination_node
  - final TAB final_node

  When AT&T format is used in HFST, weights are ignored.  When SFST or AT&T
  unweighted format is used in HWFST, weights are set to zero.


@pre All printnames used in the text format representation of the transducer
must be in the key table \a T.  
@return A transducer as defined in \a is. If end of
stream is reached, NULL.  
@see #print_transducer */ 
TransducerHandle read_transducer_text( istream& is, KeyTable *T, bool sfst=false );


/** \brief Whether the transducer coming from istream \a is has a symbol table stored
 * with it. 

@pre The transducer is in valid format and the end of stream has not been
reached. Use #read_format to check this. */ 
bool has_symbol_table(istream &is);


/** \brief Read a transducer in binary form from input stream \a is and harmonize it
 * according to the key table \a T. 

Following notations are used: \a Ts = the transducer read from istream \a is
and \a S = the symbol table of transducer Tr.

Harmonization is done in the following way:

If \a T is empty (made with #create_key_table), S is copied to T as such and
all keys used in \a Ts remain the same i.e. no harmonization is done.

If \a T is not empty, the harmonization goes as follows. For each input and output
key in a transition in \a Ts, a corresponding primary print
name is looked in \a S. A corresponding key value
for this print name is then looked in \a T and the original input
or output key is replaced with this key. Epsilon keys are copied as such (the
primary name of epsilon is thus defined solely by \a T). If a primary print
name used in \a Ts is not found in \a T, it is added to \a T and to
the global symbol cache to the next free position.

Some special cases: (1) If a key used in \a Ts is not found in \a S, it is replaced
by next free key in \a T, but it is not added to \a T as it has no print name 
(the side effect is that the key after next free key in \a T is associated with
a dummy Symbol, so it is recommended that all keys used in \a Ts are in \a S.) 
(2) Keys defined in \a S that are not used in \a Ts are not copied to \a T. 

@pre The transducer read from istream is must have a symbol table stored with it. 
@result The harmonized version of the transducer read from istream is.
If end of stream is reached, NULL. */
TransducerHandle read_transducer( istream &is, KeyTable *T );

/** \brief Harmonize transducer \a t that uses key table \a T_old according to key table \a T _new.

    @see read_transducer
 */
TransducerHandle harmonize_transducer( TransducerHandle t, KeyTable *T_old, KeyTable *T_new );


//@}


/** \name Writing Symbol Strings and Transducers
 * 
 * Write transducers 
 *
 * (1) in text format into pair strings and output streams and
 * 
 * (2) in binary format to output streams so that the print names
 * associated to keys are stored with the transducer.
 */
//@{


/** \brief A pairstring representation of one-path transducer \a t using the
 * symbols defined in key table \a T. \a spaces defines whether pairs are separated
 * by spaces.

The transitions are printed one after another, separated by spaces if so 
requested. If the input and output symbols are not equal, they are separated
by a colon. If the backslash '\\' and colon ':' are part of a symbol print
name, they are escaped as "\\" and "\:". 

The empty transducer is represented by "\empty_transducer" and the epsilon
transducer as "EPS" where EPS is the symbol name for epsilon
(#pairstring_to_transducer recognizes "" as the epsilon transducer, but "EPS"
is a more user-friendly notation). If the symbol name for epsilon is not
defined, "\epsilon" is returned.

@see #pairstring_to_transducer */
char * transducer_to_pairstring( TransducerHandle t, KeyTable *T, bool spaces=true, bool print_epsilons=true );

/** \brief Print transducer \a t in text format using the symbols defined in key
 * table \a T.
 * The parameter \a print_weights indicates whether weights are included, the
 * output stream ostr indicates where printing is directed.  Parameter \a old
 * indicates whether transducer \a t should be printed in old SFST text format
 * instead of AT&T format.
    
 In HFST the \a print_weight parameter is ignored.

 In At&T and SFST format, the newline, horizontal tab, carriage return,
 vertical tab, formfeed, bell character, backspace, backslash and space are
 printed as "\n", "\t", "\r", "\v", "\f" "\a", "\b", "\\" and "\0x20".  In
 SFST format, the colon and angle brackets are printed as "\:", "\<" and
 "\>".

@see #read_transducer_text
*/
void print_transducer( TransducerHandle t, KeyTable *T, bool print_weights=false, ostream &ostr=std::cout, bool old=false );


/** \brief Write \a t in binary form to output stream \a os. Key table \a T is stored
 * with the transducer. 

@param  t Transducer to be written
@param  T Key table that is stored with the transducer
@param  os Where transducer is written
@param  backwards_compatibility Whether the transducer is written in SFST/OpenFst compatible format. */
void write_transducer( TransducerHandle t, KeyTable *T, ostream &os=std::cout, bool backwards_compatibility=false );


/** \brief Write a transducer \a t with key table \a kt into file \a output_file. Write its symbols into the file with name symbol_file_name. */
void write_runtime_transducer( TransducerHandle t, KeyTable * kt, FILE * output_file);

//@}

/*@}*/

/* \brief Write transducer \a t to file filename. Key table \a T is stored with the
 * transducer. 

@see write_transducer */
void write_transducer( TransducerHandle t, const char *filename, KeyTable *T, bool backwards_compatibility=false );


void print_key_table(KeyTable *T);

/* \brief Read a binary transducer from file \a filename and harmonize it
 * according to the key table \a T.

@pre The transducer that is read must have a key table stored with it. */
TransducerHandle read_transducer( const char *filename, KeyTable *T );

// vim: set ft=cpp.doxygen:

