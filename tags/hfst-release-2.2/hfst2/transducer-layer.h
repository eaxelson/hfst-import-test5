#include <iostream>
using std::cin;
using std::cout;


/** \defgroup TransducerLayer Transducer Layer
 *
 * Datatypes and functions related to transducer calculus and support.
 * This layer depends on the \link KeyLayer Key Layer \endlink.
 */
/*@{*/


/** @name Datatypes */
//@{

/** \brief A finite state transducer (FST).

A FST is a weighted or an unweighted synchronous finite-state transducer
i.e. an automaton representing a set of strings of symbol pairs. */
typedef TransducerHandle TransducerHandle;

//@}



/** @name Constructive and Destructive Functions for Elementary Transducers
 *  
 */
//@{


/** \brief Create an empty transducer.
    
@return A transducer that has one state that is not final and has no transitions,
i.e. does not accept any string. */
TransducerHandle create_empty_transducer();


/** \brief Create an epsilon transducer.

@return A transducer that has one state that is final and has no transitions,
i.e. accepts the epsilon string. */
TransducerHandle create_epsilon_transducer();


/** \brief Create a transducer that accepts one occurrence of key identity pair k:k. 

@return A transducer that has one transition of key identity pair \a k:k. */
TransducerHandle define_transducer( Key k );


/** \brief Create a transducer that accepts one occurrence of key pair \a p. 

@return A transducer that has one transition of key pair \a p. */
TransducerHandle define_transducer( KeyPair *p );


/** \brief Create a transducer that accepts the union of the key identity pairs
    in a set \a ks.

@return A disjunction of all key identity pairs in \a ks. */
TransducerHandle define_transducer( KeySet *ks );


/** \brief Create a transducer that accepts the union of the key pairs
    in a set \a Pi.

@return A disjunction of all key pairs in \a Pi. */
TransducerHandle define_transducer( KeyPairSet *Pi );


/** \brief Define a set of pairs by collecting all key pairs in transducer \a t. 

@return A set of pairs that contains all key pairs that occur
in transitions of transducer \a t. */

KeyPairSet *define_keypair_set( TransducerHandle t );


/** \brief Define a set of keys by collecting all keys in transducer \a t. 

@return A set of keys that contains all keys that occur
in transitions of transducer \a t. */
KeySet *define_key_set( TransducerHandle t );


/** \brief Add weight \a w to transducer \a t.

Set the weights of all final states in transducer \a t to \a w.

@note Does nothing if the underlying library does not support weights. */

TransducerHandle add_weight( TransducerHandle t, float w );


/** \brief Delete transducer \a t. */

void delete_transducer( TransducerHandle t );

//@}



/** @name Automata and Transducer Operations
 *  
 * The following table contains two kinds of operations.
 *
 * 1. The first kind of operations treat KeyPairs as atomic transition
 * labels. The key for epsilon is treated as if it were a normal symbol.
 *
 * 2. The second kind of operations interpret the input and output keys
 * in key pairs as strings. Therefore the input side of pair 0:a is
 * the empty string '' and the identity relation of the empty string ''
 * composes with relation 0:a*. 
 */
//@{

//@}



/** @name Basic Algebraic Functions
 *  
 */
//@{


/** \brief Composition of \a t1 and \a t2. 

@param t1 first transducer in composition operation
@param t2 second transducer in composition operation
@param destructive Whether the argument transducers are deleted.
    
Composition of \a t1 and \a t2 maps "a" to "c" iff \a t1 maps "a" to some "b" and \a t2 maps "b" to "c".

@post The resulting transducer may be nondeterministic and not minimal. 
\a t1 and \a t2 are deleted. */

TransducerHandle compose( TransducerHandle t1, TransducerHandle t2, bool destructive);

TransducerHandle compose( TransducerHandle t1, TransducerHandle t2);


/** \brief Concatenation of \a t1 and \a t2. 
    
If \a t1 maps the string "a" to "b" and \a t2 maps "c" to "d", then concatenation of \a t1 and \a t2 
maps the string "ac" to "bd".

@post The resulting transducer may contain epsilons and be nondeterministic and not minimal. 
\a t1 and \a t2 are deleted.  */

TransducerHandle concatenate( TransducerHandle t1, TransducerHandle t2 );


/** \brief A deep copy of \a t.
    
@post \a t is not deleted. */

TransducerHandle copy(TransducerHandle t);


/** \brief Disjunction of \a t1 and \a t2.
    
Disjunction of \a t1 and \a t2 maps "a" to "b" iff \a t1 and/or \a t2 maps "a" to "b".

@post \a t1 and \a t2 are deleted. The resulting transducer may have epsilons. */

TransducerHandle disjunct( TransducerHandle t1, TransducerHandle t2 );


/** \brief Disjunction of \a t1 and \a t2 that are both tries. The resulting transducer is also a trie. */

TransducerHandle disjunct_transducers_as_tries( TransducerHandle t1, TransducerHandle t2 );

/** \brief Add the KeyVector * \a key_string as a path to the trie \a t. 

@param t             The trie where the path is added.
@param key_string    A KeyVector representing the path.
@param weight        The weight of the path.
@param sum_weights   If the path already exists with a different weight: whether (1) a new path is added or (2) the weight of the old path is modified by summing the weights. 
*/

TransducerHandle disjunct_as_trie( TransducerHandle t, KeyVector * key_string, float weight=0, bool sum_weights=false );

/** \brief Add the KeyPairVector * \a key_pair_string as a path to the trie \a t. 

@see disjunct_as_trie */

TransducerHandle disjunct_as_trie( TransducerHandle t, 
				   KeyPairVector * key_pair_string, float weight=0, bool sum_weights=false );

/** \brief Extract the input language of \a t. 

Maps "A" to "A" iff \a t maps "A" to some string "B".
For example the argument transducer <tt>[ a:b [c:d | e:f] ]</tt> 
gives the result <tt>[ a:a [c:c | e:e] ]</tt>. 

@post \a t is deleted. */

TransducerHandle extract_input_language( TransducerHandle t );


/** \brief Extract the output language of \a t. 
    
Maps "B" to "B" iff \a t maps some string "A" to "B".
For example the argument transducer <tt>[ a:b [c:d | e:f] ]</tt> 
gives the result <tt>[ b:b [d:d | f:f] ]</tt>. 

@post \a t is deleted. */

TransducerHandle extract_output_language( TransducerHandle t );


/** \brief Intersection of \a t1 and \a t2.
    
Maps string S_1 to string S_2 (length of both strings is n) 
iff both \a t1 and \a t2 map S_1 to S_2 by aligning the i:th symbol
in S_1 with the i:th symbol in S_2 for all i in n.

For example, <tt>[a:b]</tt> and <tt>[a:0 0:b]</tt> both map a to b,
but their intersection is empty because the alignment is different. 

@post \a t1 and \a t2 are deleted. */

TransducerHandle intersect( TransducerHandle t1, TransducerHandle t2 );


/** \brief The intersecting composition of \a t with the transducers in \a v.

Intersecting composition of the transducer \a t and the transducers in the vector 
\a v is equivalent to the composition of \a t with the intersection of the transducers 
in \a v. If kt is not NULL, then the keys in kt, which correspond to Xerox-style flag-diacritics
will be treated as epsilons, i.e. the will be in the result-transducer, but will not affect the
rules in any way.

@post The transducer \a t is deleted.
@post The transducers in the vector \a v are deleted.
@post The vector \a v is deleted.
@post The result is not minimized.
*/
TransducerHandle 
intersecting_composition( TransducerHandle t, 
			  vector<TransducerHandle> * v, KeyTable * kt=NULL);

/** \brief Switch input and output in the transition pairs of transducer \a t. 
    
Inversion of \a t maps "A" to "B" iff \a t maps "B" to "A". 
For example inversion of <tt>[a:b c:d e:f]</tt> is <tt>[b:a d:c f:e]</tt>.

@post \a t is deleted. */

TransducerHandle invert( TransducerHandle t );


/** \brief Complement of \a t with regard to a set of key pairs \a Pi. 
    
Complement is computed by subtraction: <tt>~t = [.*] - t</tt> 

Complement of transducer <tt>[t]</tt> maps the string 'a_1a_2 ... a_n' to 'b_1b_2 ... b_n' iff \a Pi contains
'a_i:b_i' for all 1 <= i <= n, and <tt>t & [a_1:b_1 a_2:b_2 ... a_n:b_n]</tt>
is the empty transducer. Either a_i or b_i is allowed to be the empty symbol.

The same that has been said of alignment in function #intersect goes for negation.
For example, both <tt>[a:b]</tt> and <tt>[a:0 0:b]</tt> map the string "a" to "b", but the complement of
<tt>[a:b]</tt> nevertheless includes '<tt>a:0 0:b</tt>' because the alignment is different. 

@post \a t is deleted. */

TransducerHandle negate( TransducerHandle t, KeyPairSet *Pi );


/** \brief Disjunction of \a t and epsilon. 

@post \a t is deleted. */

TransducerHandle optionalize( TransducerHandle t );


/** \brief Transducer \a t repeated at most \a n times. */

TransducerHandle repeat_le_n( TransducerHandle t, int n );


/** \brief \a t catenated \a n times.
    
@param t Transducer to be catenated.
@param n How many times \a t is catenated. If \a n == 0, an epsilon transducer is returned.

@post \a t is not deleted. [actually it cannot be used] */

TransducerHandle repeat_n( TransducerHandle t, int n );


/** \brief Transducer \a t + 
    
Transducer that accepts one or more \a t. 

@post \a t is deleted. */

TransducerHandle repeat_plus( TransducerHandle t );


/** \brief Transducer \a t * 
    
Transducer that accepts any number of \a t. 

@post \a t is deleted. */

TransducerHandle repeat_star( TransducerHandle t );


/** \brief Reverse transducer \a t */

TransducerHandle reverse( TransducerHandle t );


/** \brief \a t1 minus \a t2. 
    
\a t1 minus \a t2 is the transducer which accepts the strings accepted
by \a t1 that are not accepted by \a t2. The same that has been said of alignment in 
function #intersect goes for subtraction.

For example, both <tt>[a:b]</tt> and <tt>[a:0 0:b]</tt> map the string a to b, but the result of
<tt>a:b - [a:0 0:b]</tt> is nevertheless <tt>[a:b]</tt> because the alignment is different. 

@post \a t1 and \a t2 are deleted. */

TransducerHandle subtract( TransducerHandle t1, TransducerHandle t2 );


//@}




/** @name Substitution, Insertion and Removal functions
 *  
 */
//@{

/** \brief Add input language to \a t using a set of feasible pairs in \a Pi. 

Equivalent to a composition of transducer \a t and a transducer accepting
any number of pairs in \a Pi. 
*/

TransducerHandle add_input_language( TransducerHandle t, KeyPairSet *Pi );

/** \brief Add output language to \a t using a set of feasible pairs in \a Pi. 

Equivalent to a composition of a transducer accepting any number of
pairs in \a Pi and transducer \a t. 
*/

TransducerHandle add_output_language( TransducerHandle t, KeyPairSet *Pi );

/** \brief Freely insert key pair \a p into \a t. 
    
For example, freely inserting the key pair <tt>x:y</tt> into
the transducer <tt>[a b]</tt> is equivalent to
<tt>[x:y]* a [x:y]* b [x:y]*</tt>.

@post \a t is deleted. */

TransducerHandle insert_freely( TransducerHandle t, KeyPair *p );

/** \brief Remove transitions that are equal to key pair \a p. */
TransducerHandle remove_pair (TransducerHandle t, KeyPair *p); 

/** \brief Remove transitions where a key from \a ks is used on both the input and output sides. */
TransducerHandle remove_pairs (TransducerHandle t, KeySet *ks); 

/** \brief Shuffle \a t1 and \a t2.

... */
TransducerHandle shuffle( TransducerHandle t1, TransducerHandle t2 );

/** \brief In all transitions, substitute key \a k1 with key \a k2. 

@param t Transducer where substitution is made. 
@param k1 The key that is substituted.
@param k2 The substituting key.
@param ignore_epsilon_pairs Whether keys in epsilon:epsilon transitions are not substituted. 
*/
TransducerHandle substitute_key( TransducerHandle t, Key k1, Key k2, bool ignore_epsilon_pairs=false ); 

/** \brief In all transitions, if a key is equal to some key in key set \a ks, substitute it with key \a k2. */
TransducerHandle substitute_key( TransducerHandle t, KeySet *ks, Key k2 ); 

/** \brief Substitute all transitions equal to \a p1 with a copy of \a p2. */
TransducerHandle substitute_with_pair ( TransducerHandle t, KeyPair *p1, KeyPair *p2); 

/** \brief Substitute all transitions in transducer \a t equal to \a p with a copy of transducer \a tr. */
TransducerHandle substitute_with_transducer ( TransducerHandle t, KeyPair *p, TransducerHandle tr); 

//@}







/** @name Optimizing and Converting Functions
 *  
 */
//@{


/** \brief Determinize \a t. 

Remove transitions whose input and output symbols are epsilons
from \a t and determinize it. After determinization no state in \a t has two transitions with
equal input and output labels.

@post \a t is deleted. */

TransducerHandle determinize( TransducerHandle t );


/** \brief Find all paths from initial to final state in transducer \a t. 
 * \a unique defines whether \a t is determinized before finding paths.

 @param t The transducer where best paths are searched. Cannot be cyclic.
 @param unique Whether equal paths are included only once (i.e whether \a t is determinized before finding paths). */

vector<TransducerHandle> find_all_paths( TransducerHandle t, bool unique=false );


/** \brief \a n best paths from initial to final state in transducer \a t. \a unique defines whether equal paths are included only once. 
    
Returns \a n paths with smallest weight. If \a t is unweighted, returns 
\a n paths that are found first (not necessarily the shortest ones).

@param t The transducer where best paths are searched. Can be cyclic. 
@param n Number of paths that are returned. 
@param unique Whether equal paths are included only once (i.e whether \a t is determinized before finding paths). 

@result A disjunction of \a n best paths. */
  
TransducerHandle find_best_paths(TransducerHandle t, int n, bool unique=false);


/** \brief For unweighted transducers: find a maximum of \a max_number random paths in transducer \a t.
 * \a unique defines whether equal paths are included only once.  

    For weighted transducer: the same as #find_best_paths.
    For unweighted transducers, the paths returned and their number may vary randomly.

    @param t The transducer where best paths are searched. Can be cyclic.
    @param max_number A maximum number of paths that are returned.  
    @param unique Whether \a t is determinized before finding paths. */

TransducerHandle find_random_paths(TransducerHandle t, int max_number, bool unique=false);


/** \brief Minimize \a t. 
    
Remove epsilons from \a t and determinize and minimize it.

@post \a t is deleted.
@see remove_epsilons determinize */

TransducerHandle minimize( TransducerHandle t );


/** \brief Push weights in transducer \a t towards the initial state, if \a initial is true, otherwise towards the final state.

@note Does nothing if the underlying implementation does not support weights. */

TransducerHandle push_weights( TransducerHandle t, bool initial );



/** \brief Modify final weights of transducer \a t according to function \a modify. \a modify_transition_weights defines whether transition weights are modified as well. */

TransducerHandle modify_weights( TransducerHandle t, float(*modify)(float), bool modify_transition_weights=false );


/** \brief Remove from \a t transitions whose input and output labels are epsilons.

Create an equivalent transducer that has no transitions whose input and output labels are epsilons. */

TransducerHandle remove_epsilons( TransducerHandle t );


/* \brief Convert an unweighted transducer to a weighted one. 

@param t An unweighted transducer (of type %HFST::%TransducerHandle)
@return An equivalent weighted transducer (of type %HWFST::%TransducerHandle) with all weights initialized to zero
@post \a t is not deleted.
*/

//TransducerHandle unweighted_to_weighted( TransducerHandle t );


/* \brief Convert a weighted transducer to an unweighted one. 

@param t A weighted transducer (of type %HWFST::%TransducerHandle)
@return An equivalent unweighted transducer (of type %HFST::%TransducerHandle)
@post \a t is not deleted.
*/

//TransducerHandle weighted_to_unweighted( TransducerHandle t );


//@}




/** @name Testing Functions
 *  
 * These functions do not delete their arguments.
 */
//@{


/** \brief Whether \a t1 and \a t2 are equivalent.
    
The same that has been said of alignment in function #intersect
goes for #are_equivalent.
For example, both <tt>[a:b]</tt> and <tt>[a:0 0:b]</tt> map the string
"a" to "b", but they are not equivalent because the alignment is different. 

@post \a t1 and \a t2 are not deleted. */

bool are_equivalent(TransducerHandle t1, TransducerHandle t2);


/** \brief Whether \a t1 and \a t2 have an empty intersection. 

@pre \a t1 and \a t2 are deterministic
@post \a t1 and \a t2 are not deleted. */

bool are_disjoint( TransducerHandle t1, TransducerHandle t2 );


/** \brief The total weight of one-path transducer \a t.

In HWFST: The sum of all transition weights and the final weight
in transducer \a t.
In HFST: Returns always zero. 

@pre \a t is a one-path transducer. */

float get_weight(TransducerHandle t);


/** \brief Whether for every transition in \a t the input symbol is the same as the output symbol.
    
@post \a t is not deleted. */

bool is_automaton(TransducerHandle t);


/** \brief Whether \a t is cyclic. 
    
@post \a t is not deleted. */

bool is_cyclic(TransducerHandle t);


/** \brief Whether \a t has infinitely many output strings for some input string (or for a certain input
    string \a kv), if \a output is true and whether it has infinitely many input strings for some
    output string (or for a certain output string \a kv), if \a output is false.

@post \a t is not deleted. */


bool is_infinitely_ambiguous(TransducerHandle t, bool output=true, KeyVector *kv=NULL);

/** \brief Whether \a t is deterministic. */

bool is_deterministic( TransducerHandle t );


/** \brief Whether \a t is the empty transducer.
    
\a t is the empty transducer if it is equivalent to a transducer
that has one state that is not final and has no transitions.

@post \a t is not deleted. */

bool is_empty(TransducerHandle t);


/** \brief Whether \a t is the epsilon transducer.
    
\a t is the epsilon transducer if it equivalent to a transducer that 
has one state that is final and has no transitions. 

@post \a t is not deleted. */

bool is_epsilon(TransducerHandle t);


/** \brief Whether \a t is a minimal transducer. */

bool is_minimal( TransducerHandle t );


/** \brief Whether \a t1 is a subset of \a t2.  */

bool is_subset( TransducerHandle t1, TransducerHandle t2 );


/** \brief Look up the output-strings corresponding to string \a input_string.
    Return an empty vector, if there are no output-strings for input-string. 
    For pairs \a s:x in \a t, where \s is in \a skip_symbols, treat \a s as 
    epsilon (i.e. write \a x in the output without consuming characters in 
    \a input).
*/
KeyVectorVector * lookup_all(TransducerHandle t,
			     vector<Key> * input_string,
			     KeySet * skip_symbols = NULL);

/** \brief Look up the first found output-string corresponding to string 
    \a input_string. Return NULL, if there are no output-strings for 
    input-string.
    For pairs \a s:x in \a t, where \s is in \a skip_symbols, treat \a s as 
    epsilon (i.e. write \a x in the output without consuming characters in 
    \a input).
*/
KeyVector * lookup_first(TransducerHandle t,
			 vector<Key> * input_string,
			 KeySet * skip_symbols = NULL);

//@}









/** @name Input/Output Functions
 * Transducers in this layer are not aware of any restrictions
 * to their alphabet or any attributes. Thus the file format for
 * these transducers neither lists the known keys nor the properties
 * of the transducer.
 */
//@{


/** \brief Read the format of the next transducer in the input stream \a is.
    @return
    - 0 == ordinary unweighted transducer (HFST)
    - 1 == weighted transducer (HWFST)
    - 2 == compact unweighted transducer (HFST)
    - -1 == the end of file has been reached
    - -2 == transducer format is unknown or an error has occurred
    @post \a is is not changed. */
int read_format( istream &is=cin );

/** \brief Read transducer in binary form from input stream \a is.  */
TransducerHandle read_transducer( istream &is=cin );

/** \brief Read a binary transducer from file \a filename. */
TransducerHandle read_transducer( const char *filename );

/** \brief Read a transducer in AT&T number format from istream \a is. 
@see #read_transducer_text */
TransducerHandle read_transducer_number(istream &is);

/** \brief Write Transducer \a t in binary form to ostream \a os. 

@param t The transducer that is written.
@param os The output stream where the transducer is written.
@param backwards_compatibility Whether the transducer is stored in SFST/OpenFst compatible format.
*/
void write_transducer( TransducerHandle t, ostream &os=cout, bool backwards_compatibility=false );

/** \brief Write transducer \a t to file \a filename. 

@see write_transducer */
void write_transducer( TransducerHandle t, const char *filename, bool backwards_compatibility=false );


/** \brief Print transducer \a t in number format to ostream \a os. \a print_weights defines whether weights are printed.

@see #print_transducer */
void print_transducer_number( TransducerHandle t, bool print_weights=true, ostream &os=cout );

//@}

/*@}*/


void display_symbols( TransducerHandle t);
TransducerHandle read_runtime_transducer( FILE * f );

//TransducerHandle filter_epsilon_paths( TransducerHandle t );
// this is used in push-weights...
//TransducerHandle push_labels( TransducerHandle t, bool initial );
