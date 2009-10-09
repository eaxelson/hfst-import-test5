#include <vector>
#include <set>

#include "fst.h"
#include "lookup.h"
#include "utf8.h"
#include "alphabet.h"

using std::vector;
using std::set;

struct label_cmp{
  bool operator() (const Label &l1, const Label &l2) const {
    if ( l1.lower_char() == l2.lower_char() )
      return l1.upper_char() < l2.upper_char();
    return l1.lower_char() < l2.lower_char();
  }
};

typedef unsigned short Key;
typedef set<Label,label_cmp> LabelSet;

namespace HFST {

  /* 
     Break down the string input-string into utf-8-characters. Code it 
     according to the KeyTable * kt and the alphabet alpha. Add the symbols 
     not found in alpha and kt. 
     
     None of the arguments are deleted.
  */
  KeyVector * get_key_string( const char * input_string, 
				KeyTable * kt, 
				Alphabet &alpha);

  /*
    Return the univesal language of the symbol-pairs, which exists in some 
    transition in the trie t.

    None of the arguemtns are deleted.
  */
  Transducer * get_trie_universal_language( Transducer * t );

  /*
    Combine the vectors input_level and output_level into a path-transducer.
    The labels of the transition i is formed by the Keys at index i in the
    input vectors. The shorter vector is padded with zeros on the end.
    
    E.g. input <0, 1, 2> and <2, 3> gives the transducer

    0  0  0  2
    1  2  1  3
    2  3  2  0
    3
    
    None of the arguments are deleted.

  */
  Transducer * pair_up(KeyVector * input_level,
		       KeyVector * output_level );

  /*
    Combine the vectors input_level and output_level into a KeyPairVector.
    The KeyPair at index i in the result is formed by the Keys at index i in 
    the input vectors. The shorter vector is padded with zeros on the end.
    
    E.g. input_level = <0, 1, 2> and output_level = <2, 3> gives the 
    KeyPairVector

    <(0,2), (1,3), (2,0)>
    
    None of the arguments are deleted.
  */
  KeyPairVector * combine_string_pair_levels(KeyVector * input_level,
					     KeyVector * output_level);
  
  /*
    Lookup the key strings input_string and output_string belonging to the 
    input-language of the tokenizer transducer tokenizer. Only the first
    output-strings correponding to the input-strings are considered.

    Pair up the results as explaned in the documentation of the function 
    Transducer * pair_up(KeyVector, KeyVector) and return the result.

    None of the arguments are deleted.
  */
  KeyPairVector * tokenize_stringpair( Transducer * tokenizer, 
				       KeyVector * input_string,
				       KeyVector * output_string);
  
  KeyPairVector * longest_prefix_tokenize_pair(Transducer * tokenizer,
					      KeyVector * string_vector1,
					      KeyVector * string_vector2);
  
  /*
    Split down the symbols, given by the KeySet * keys, KeyTable * kt and 
    Alphabet alpha, into strings of utf-8-character keys. Read the strings
    into a trie transducer and return it. It is an error, if a key in keys, 
    doesn't correspond to a symbol in kt corresponding to a symbol-name in 
    alpha.

    If splitting down symbols creates new symbols not defined in alpha and/or 
    kt, the symbols are added and given keys in kt. 

    None of the arguments are deleted.
  */
  Transducer * split_symbols( KeySet * keys, 
			      KeyTable * kt, 
			      Alphabet &TheAlphabet);

  /*
    Join the symbols given by the key string key_string into one string
    symbol and return its Key. The string-symbols corresponding to the Keys
    in key_string are given by the KeyTable kt and Alphabet alpha.

    E.g. Let the Key 1 correspond to the string-symbol a and Key 2 correspond
    to the string-symbol b (as specified by kt and alpha). Then 
    the input key-string <1, 2> gives a string-symbol ab. If ab corresponds to
    the Key 3, 3 is returned.

    If the string-symbol formed doens't have a corresponding symbol number in 
    alpha and/or corresponding Key in kt, these are added.

    It is an error, if some of the keys in key-string don't have string-symbols
    associated to them. 

    None of the arguments are deleted.
  */
  Key join(KeyVector * key_string, KeyTable * kt, Alphabet &alpha);

  Transducer * gather_symbols(Node * n1, Node * n2, Transducer * symbol_maker, 
			      KeyTable * kt, Alphabet &alpha,
			      KeyVector * symbol, Key marker);
  /*
    Return a transducer, which writes the symbol-strings coded by
    split_symbols into the multicharacter symbols corresponding to them.
    The symbol-strings on the input-level are terminated by the marker <M>. 
    Its key is given as parameter. 

    E.g. if split-symbols accepts the path 

    + N o u n

    then the transducer returned will accept the path 

    +:0 N:0 o:0 u:0 n:0 <M>:+Noun 

    None of the arguments are deleted.
  */
  Transducer * multicharacter_symbol_maker( Transducer * split_symbols,
					    KeyTable * kt,
					    Key marker,
					    Alphabet &TheAlphabet);
  

  /* 
     Add the key string key_string as a path to the transducer trie_transducer.

     The transducer trie_transducer should be a trie, i.e. there should
     never be more than one transition into any state and there should be
     no transition into the start state.
     
     None of the arguments are deleted.
  */
  void add_key_string( KeyVector * key_string, 
		       Transducer * trie_transducer);
 

  /* 
     Add the key-pair string key_pair_string as a path to the transducer 
     trie_transducer.

     The transducer trie_transducer should be a trie, i.e. there should
     never be more than one transition into any state and there should be
     no transition into the start state.
     
     None of the arguments are deleted.

  */
  void add_key_pair_string( KeyPairVector * key_pair_string, 
			    Transducer * trie_transducer);
 
  
  /*
    Return a transducer accepting the path given by the key string key_string.

    None of the arguments are deleted.
  */
  Transducer * key_string_to_transducer( KeyVector * key_string );

  /*
    Return a c-string representation of a transducer accepting one path.
    It is required, that the transducer given is a path, i.e. an a-cyclic
    transducer, where each node has at most one transition and the only
    final node has no transitions.

    None of the arguments are deleted.
  */
  char * fst_to_pairstring( Transducer * path,
			    KeyTable * kt,
			    Alphabet &alpha,
			    bool spaces,
			    bool epsilons=true);

  char * vector_to_pairstring( KeyVector * kv,
			       KeyTable * kt,
			       Alphabet &alpha );

  char * vector_to_pairstring( KeyPairVector * kv,
			       KeyTable * kt,
			       Alphabet &alpha );
};
