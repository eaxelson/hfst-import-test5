
#include <cassert>

#include "../hfst.h"
#include "fst-lex.h"
#include "fst-regression.h"
#include "hfst-optimized-lookup-transducer.h"
#include "read_runtime_transducer.h"
#include "lookup.h"
#include "tokenize_functions.h"
#include "../hfst_defs_private.h"
#include "fst.h"
#include <vector>

using std::vector;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <set>
using std::set;

#include <map>
using std::map;

//#include "transducer-conversion.h"

Alphabet TheAlphabet;

/** \brief Namespace for HFST functions.
    
Unless otherwise specified:

-  the functions delete their Transducer argument(s)
-  the functions do not change their other arguments

<!-- Differences in HFST and HWFST are documented in https://kitwiki.csc.fi/twiki/bin/view/KitWiki/OMorFiWithOpenFst. -->

<!-- 
HFST function arguments are named in the following way according to their type:

- #Key: c, c1, c2, ...
- #KeyPair:
- #KeyPairSet
- #Range
- #Ranges
- #TransducerHandle: t, t1, t2, ...
- #ContextsHandle: p, p1, p2, ... 
-->

Xerox transducer notations (http://www.xrce.xerox.com/competencies/content-analysis/fsCompiler/fssyntax.html) are used in the examples with the following added notation:

- <tt>[.]</tt> &nbsp; The wildcard symbol. Allows any single key pair defined in the KeyPairSet argument of a function.

<!-- The wildcard symbol. Allows any single mapping defined in the alphabet. More use examples in #new_transducer. - Range< a(1), a(2), ... a(n) > &nbsp; A Range struct containing Characters for symbols a(1) ... a(n). More info in #Range. 
- Ranges< Range(1), Range(2), ... Range(n) > &nbsp; A Ranges struct containing Range structs Range(1) ... Range(n). More info in #Ranges.
- Contexts< t(1), t(2); t(3), t(4); ... ; t(n-1), t(n) > &nbsp; A ContextsHandle struct
containing Transducer_ pairs left(1),right(1) ... left(n/2),right(n/2). More info in #ContextsHandle. 
-->

*/

namespace HFST {

  char * new_string(size_t lgth)
  {
    return (char*)(calloc(sizeof(char),lgth+1));
  }

  char * string_copy(const char * str)
  {
    if (str == NULL)
      {
	throw "NULL char * given as input to HFST::string_copy().";
      }
    char * new_str = new_string(strlen(str));
    if (new_str == NULL)
      {
	throw std::bad_alloc();
      }
    return strcpy(new_str,str);
  }

  typedef struct contexts_t_ {
    Transducer *left;
    Transducer *right;
    contexts_t_ *next;
  } Contexts;
  
  struct ltstr {
    bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) < 0; }
  };
  
  struct eqstr {
    bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) == 0; }
  };
  
  // hfst-calculate -spesific typedefs
  typedef set<char*, ltstr> RVarSet;  
  typedef hash_map<char*, Transducer*, hash<const char*>, eqstr> VarMap;
  typedef hash_map<char*, Range*, hash<const char*>, eqstr> SVarMap;

  //Alphabet TheAlphabet;
  bool UTF8 = true;
  bool Verbose=true;


  void delete_range(Range *t) {
    for(Range *r=t; r!=NULL;) {
      Range *tmp=r->next;
      delete r;
      r=tmp;
    }
    return;
  }

  void delete_ranges(Ranges *t) {
    for(Ranges *r=t; r!=NULL;) {
      Ranges *tmp=r->next;
      delete_range(r->range);
      delete r;
      r=tmp;
    }
    return;
  }


  // For core extension layer
  struct lessnode {
    bool operator()(const Node* n1, const Node* n2) const
    { return (size_t)n1 < (size_t)n2; }
  };
  typedef set<Node*, lessnode> VisitedNodes;
  static VisitedNodes VN; // visited transducer states



  // hfst-calculate -spesific variables
  static VarMap VM;
  static SVarMap SVM;
  static RVarSet RS;
  static RVarSet RSS;

  static int Alphabet_Defined=1;
  void set_alphabet_defined( int i ) { Alphabet_Defined=i; };

  vector<char*> string_paths(TransducerHandle t, bool spaces);
  TransducerHandle best_paths( TransducerHandle t, int number_of_forms, bool unique);  
  
  // public functions that do not need an interface
  
  Character character_code( unsigned int c );
  Character symbol_code( char *s );
  Range *add_value( Symbol c, Range *r );
  Range *add_values( unsigned int c1, unsigned int c2, Range *r );
  Range *add_var_values( char *name, Range *r );
  Range *complement_range( Range *r, KeySet *Si );
  Range *complement_range( Range *r );
  Range *copy_range_agreement_variable_value( char *name );
  Range *copy_range_variable_value( char *name );
  Ranges *add_range( Range *r, Ranges *rs );
  bool define_range_variable( char *name, Range *r );
  //unsigned int utf8toint( char *s );
  void read_symbol_table(char *filename);

  unsigned int utf8toint( char *s );
  unsigned int utf8toint( char **s );
  char *int2utf8( unsigned int sym );

  unsigned int utf8_to_int( char *s ) {
    return utf8toint( s );
  }
  unsigned int utf8_to_int( char **s ) {
    return utf8toint( s );
  }



  // public functions that need an interface

  Contexts *add_context_( Contexts *p1, Contexts *p2 );         
  Contexts *make_context_( Transducer *t1, Transducer *t2 );   
  Transducer *catenate_( Transducer *t1, Transducer *t2 );     
  Transducer *composition_( Transducer *t1, Transducer *t2, bool destructive=true );  
  Transducer *conjunction_( Transducer *t1, Transducer *t2 );  
  Transducer *disjunction_( Transducer *t1, Transducer *t2 );   
  Transducer *explode_( Transducer *t );                       
  Transducer *freely_insert_( Transducer *t, Character c1, Character c2 );  
  Transducer *lower_level_( Transducer *t );                   
  Transducer *make_mapping_( Ranges *rs1, Ranges *rs2 );       
  Transducer *make_rule_( Transducer *t1, Range *r1, Twol_Type type, Range *r2, Transducer *t2, KeyPairSet *Pi );  
  Transducer *minimise_( Transducer *t );                      
  Transducer *negation_( Transducer *t, KeyPairSet *Pi );                       
  Transducer *new_transducer_( Range *r1, Range *r2, KeyPairSet *Pi=NULL );         
  Transducer *optional_( Transducer *t );                      
  Transducer *read_transducer_( char *filename, bool alpha );              
  Transducer *read_words_( char *filename );                   
  Transducer *repetition_( Transducer *t );                    
  Transducer *repetition2_( Transducer *t );                   
  Transducer *replace_( Transducer *t, Repl_Type type, bool optional, KeyPairSet *Pi );   
  Transducer *replace_in_context_( Transducer *t, Repl_Type type, Contexts *p, bool optional, KeyPairSet *Pi );  
  Transducer *restriction_( Transducer *t, Twol_Type type, Contexts *p, int direction, KeyPairSet *Pi );  
  Transducer *result_( Transducer *t, bool invert, bool reverse );            
  Transducer *copy_transducer_agreement_variable_value_( char *name );                       
  Transducer *subtraction_( Transducer *t1, Transducer *t2 );  
  Transducer *switch_levels_( Transducer *t );                 
  Transducer *upper_level_( Transducer *t );                   
  Transducer *copy_transducervariable_value_( char *name );                        
  bool define_transducer_agreement_variable_( char *name, Transducer *t );                 
  bool define_transducer_variable_( char *name, Transducer *t );                  
  void def_alphabet_( Transducer *t );                          
  void write_to_file_( Transducer *t, char *filename );
  void write_to_file_( Transducer *t, ostream &out ); //added by Miikka Silfverberg
  // Transducer *light_composition_(Transducer *t1, Transducer *t2);  
  // void print_fsa_paths_(Transducer *t);                        



  /*  TransducerHandle
  make_fst_from_xerox_regexp(const char *xre)
  {
    TheAlphabet.utf8 = true;
    xrenerrs = 0;
    // xre_data is never freed because it is global stuff
    xre_data = strdup(xre);
    xre_len = strlen(xre_data);
    xreparse();
    if (0 == xrenerrs)
      {
	return xre_transducer;
      }
    else
      {
	return 0;
      }
      }*/
  


  void add_epsilon_to_alphabet() {
    TheAlphabet.add_epsilon();
  };

  KeyVectorVector * lookup_all(TransducerHandle t,
			       KeyVector * input_string,
			       KeySet * skip_symbols) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    if (skip_symbols != NULL)
      {
	return find_all_output_strings( pT,input_string, skip_symbols);
      }
    else
      {
	KeySet ks;
	return find_all_output_strings( pT,input_string, &ks);
      }
  }

  KeyVector * lookup_first(TransducerHandle t,
			   KeyVector * input_string,
			   KeySet * skip_symbols) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    return find_first_output_string( pT,input_string );

  }

  KeyVector * longest_match_lookup( TransducerHandle t,
				    KeyVector * input_string ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer,t);
    return longest_prefix_tokenization(pT,input_string);
  }

  TransducerHandle 
  longest_match_tokenizer_with_markers( TransducerHandle T,
					TransducerHandle universal,
					Key output,
					KeyTable * kt) {
    
    // The mark 0:<M>
    KeyPair * mark_pair = define_keypair(0,output);
    TransducerHandle mark = define_transducer(mark_pair);

    universal = subtract(universal,
			 create_epsilon_transducer());

    // universal2 = (.* 0:<M>)+ and universal1 = (.* 0:<M>)*
    TransducerHandle universal2 = minimize(repeat_plus(disjunct(universal,copy(mark))));
    TransducerHandle universal1 = minimize(repeat_star(copy(universal2)));

    // All strings of tokens separated by marks.
    // T* (0:<M> T*)* 0:<M>
    TransducerHandle all_marked_strings = minimize(concatenate(
						   concatenate(copy(T),
							    repeat_star(concatenate(
										    copy(mark),
										    copy(T)))),
						   copy(mark)));

    // (. <M>:0) * ( ( T << <M>:0 ) & ( (. <M>:0)+ <M>:0 (. <M>:0)+) )
    TransducerHandle splitted_multichar_symbols = 
      concatenate(copy(universal1),					    
	       intersect(insert_freely(T,mark_pair),
			   concatenate(copy(universal2),
				    concatenate(mark,
					     copy(universal2)))));


    delete mark_pair;    
    delete_transducer(universal2);
    
    // The result is the difference of all marked strings of multicharacter symbols and
    // those marked strings, where a non-maxiaml prefix has been marked.
    return subtract(all_marked_strings,
			     minimize(concatenate(splitted_multichar_symbols,
					       universal1))); 

  }


  TransducerHandle longest_match_tokenizer( KeySet * keys,
					    KeyTable * kt ) {
    // Split the symbols in the KeySet * keys into strings of utf-8-symbol keys
    // and make a trie out of them.
    TransducerHandle multicharacter_symbol_sequences = 
      PINSTANCE_TO_HANDLE(Transducer,split_symbols( keys, 
						    kt, 
						    TheAlphabet));

    // The same as multicharacter_symbol_sequences except as a SFST-transducer.
    Transducer * multi_char_syms = 
      HANDLE_TO_PINSTANCE(Transducer,multicharacter_symbol_sequences);
    
    // Construct the universal language of utf-8-symbol keys, which make up 
    // the multi-character symbols.
    TransducerHandle universal = 
      PINSTANCE_TO_HANDLE(Transducer,
			  get_trie_universal_language(multi_char_syms));

    // Get an unused key, which may be used as the marker of the tokenizer.
    Symbol s = define_symbol("HFST::LONGEST::MATCH");
    Key marker;
    if ( is_symbol(s,kt) ) {
      marker = get_key(s,kt);
    }
    else {
      marker = get_unused_key( kt );
      associate_key(marker,kt,s);
    }

    // Construct a transducer, which may be used to write a sequence of
    // single symbol keys into the key, which corresponds to the 
    // multi-character symbol made up by the single character symbols.
    TransducerHandle sequences_to_symbols = 
      PINSTANCE_TO_HANDLE(Transducer,
			  multicharacter_symbol_maker( multi_char_syms, 
						       kt, 
						       marker,
						       TheAlphabet));
    sequences_to_symbols = 
      minimize(repeat_star(sequences_to_symbols));

    // Construct a longest match tokenizer from the multi-character symbols in 
    // the KeySet * keys with the marker Key marker. 
    TransducerHandle longest_match_tokenizer_fst =
      longest_match_tokenizer_with_markers( multicharacter_symbol_sequences,
					    universal,
					    marker,
					    kt);

    // Return a longest match tokenizer, that writes each symbol into the 
    // multi-character symbol corresponding to it.
    return compose(longest_match_tokenizer_fst,sequences_to_symbols);
  }

  // Get all keys not equal to 0 defined in the KeyTable kt.
  static KeySet * get_table_keys(KeyTable * kt) 
  {
    KeySet * ks = new KeySet;
    size_t upper_bound = kt->get_unused_key();
    for (size_t i = 0; i < upper_bound; ++i) 
      {
	if ( kt->is_key(i) ) {
	  ks->insert(i);
	}
      }
    return ks;
  }
  
  // Return a KeyTable * new_kt, where epsilon has been recoded i.e.:
  //
  // new_kt.get_symbol(key) == new_kt.get_symbol(key), 
  //   when key != 0 and key < kt.get_unused_key(),
  //
  // new_kt.get_key_symbol(0) = define_symbol(epsilon_replacement)
  //
  // new_kt.get_key_symbol(kt.get_unused_key()) == kt.get_key_symbol(0)
  //
  // Main use in tokenization. 
  KeyTable * recode_key_table(KeyTable * kt, const char * epsilon_replacement){
    Key epsilon = 0;
    KeyTable * new_kt = create_key_table();
    Symbol epsilon_symbol = define_symbol(epsilon_replacement);
    associate_key(epsilon,new_kt,epsilon_symbol);
    for (Key k = 1; k < kt->get_unused_key(); ++k) {
      if (kt->is_key(k))
	{
	  Symbol s = get_key_symbol(k,kt);
	  associate_key(k,new_kt,s);
	}
    }
    associate_key(new_kt->get_unused_key(),
		  new_kt,
		  get_key_symbol(epsilon,kt));
    return new_kt;
  }

  KeyTable * minimize_key_table(KeyTable * key_table, TransducerHandle t)
  {
    if (key_table == NULL) { return NULL; }
    KeySet * t_keys = define_key_set(t);
    HFST::KeyTable * new_key_table = create_key_table();
    new_key_table->associate_key(0,0);
    Key new_k = 1;
    for (Key k = 0; k < key_table->get_unused_key(); ++k)
      {
	if (t_keys->find(k) != t_keys->end() and (not new_key_table->is_key(k)))
	  { associate_key(new_k++,new_key_table,key_table->get_key_symbol(k)); }
      }
    return new_key_table;
  }
  TransducerHandle longest_match_tokenizer2( KeySet * keys,
					     KeyTable * kt );

  TransducerHandle longest_match_tokenizer2( KeyTable * kt) {
    KeySet * kt_keys = get_table_keys(kt);
    TransducerHandle tokenizer = longest_match_tokenizer2(kt_keys,kt);
    delete kt_keys;
    return tokenizer;
  }

  TransducerHandle longest_match_tokenizer2( KeySet * keys,
					     KeyTable * kt ) {
    Transducer * multi_char_symbol_sequences =
      split_symbols(keys,kt,TheAlphabet);
    Transducer * symbols_to_sequences =
      multicharacter_symbol_maker( multi_char_symbol_sequences,
				   kt,
				   0,
				   TheAlphabet);
    delete multi_char_symbol_sequences;

    return PINSTANCE_TO_HANDLE(Transducer,symbols_to_sequences);
  }


  void insert_keypairs(KeyPairSet * kp, Alphabet &alpha) {
    for ( KeyPairIterator it = begin_pi_key(kp);
	  it != end_pi_key(kp);
	  ++it ) 
      alpha.insert(**it);
  }

  void display_symbols(TransducerHandle t) {
    //Transducer * tr = HANDLE_TO_PINSTANCE(Transducer, t);
    unsigned short i = 0;
    const char * str;
    while ( (str = TheAlphabet.code2symbol(i)) != NULL ) {
      printf("%u %s\n",i,str);
      ++i;
    }
  }

  typedef std::set<HFST::Key> InputKeySet;
  typedef std::map<HFST::Key,HFST::Key> KeyTranslationMap;
  typedef std::set<HFST::Symbol> SymbolSet;
  
  void add_input_symbols(Node * n,
			 InputKeySet * input_symbols,
			 NodeSet &visited_nodes)
  {
    for(ArcsIter aiter(n->arcs());
	aiter;
	aiter++)
      {
	Arc a = *aiter;
	input_symbols->insert(a.label().lower_char());
	if (visited_nodes.find(a.target_node()) == visited_nodes.end())
	  {
	    visited_nodes.insert(a.target_node());
	    add_input_symbols(a.target_node(),
			      input_symbols,
			      visited_nodes);
	  }
      }
  }
  
  const HFST::Symbol EPSILON = 0;
  
  InputKeySet * get_input_keys(Transducer * t)
  {
    InputKeySet * input_key_set = new InputKeySet;
    input_key_set->insert(EPSILON);
    NodeSet visited_nodes;
    add_input_symbols(t->root_node(),
		      input_key_set,
		      visited_nodes);
    return input_key_set;
  }
  
  SymbolSet * get_symbols(InputKeySet * input_keys, HFST::KeyTable * kt)
  {
    SymbolSet * input_symbols = new SymbolSet;
    for (InputKeySet::iterator it = input_keys->begin();
	 it != input_keys->end();
	 ++it)
      {
	HFST::Key k = *it;
	input_symbols->insert(HFST::get_key_symbol(k,kt));
      }
    return input_symbols;
  }

  typedef std::map<Key,unsigned int> KeyCountMap;
  
  HFST::KeyTable * lift_input_keys(HFST::KeyTable * kt,
				   KeyCountMap &input_key_counts)
  {
    std::multimap<unsigned int,Key> count_keys;
    for (KeyCountMap::iterator it = input_key_counts.begin();
	 it != input_key_counts.end();
	 ++it)
      { count_keys.insert(std::pair<unsigned int,Key>(it->second,it->first)); }
    KeyTable * new_key_table = create_key_table();
    associate_key(0,new_key_table,kt->get_key_symbol(0));
    Key k = 1;
    for (std::multimap<unsigned int,Key>::reverse_iterator it = count_keys.rbegin();
	 it != count_keys.rend(); ++it)
      {
	if (it->second != 0)
	  { 
	    associate_key(k++,new_key_table,kt->get_key_symbol(it->second)); }
      }
    for (Key kk = 1; kk != kt->get_unused_key(); ++kk)
      {
	if (input_key_counts.find(kk) == input_key_counts.end())
	  { 
	    associate_key(k++,new_key_table,kt->get_key_symbol(kk)); }
      }
    return new_key_table;
  }
  

  void get_input_frequency(KeyCountMap &key_count_map,Transducer * t)
  {
    key_count_map[0] = 1;
    NodeNumbering num(*t);
    for (size_t i = 0; i < num.number_of_nodes(); ++i)
      {
	Node * n = num.get_node(i);
	std::set<Key> input_keys;
	for (ArcsIter aiter(n->arcs()); aiter; aiter++)
	  { Arc * a = aiter; input_keys.insert(a->label().lower_char()); }
	for (std::set<Key>::iterator it = input_keys.begin();
	     it != input_keys.end(); ++it)
	  { ++key_count_map[*it]; }
      }
  }
  
  HFST::KeyTable * reorder_key_table(HFST::KeyTable * kt, Transducer * t)
  {
    InputKeySet * input_keys = get_input_keys(t);
    input_keys->insert(0);
    KeyCountMap key_count_map;
    get_input_frequency(key_count_map,t);
    HFST::KeyTable * new_kt = lift_input_keys(kt,key_count_map);
    delete input_keys;
    return new_kt;
  }
  
  void write_runtime_transducer( TransducerHandle input, KeyTable * key_table, FILE * outstream) {    
    Transducer * t = HANDLE_TO_PINSTANCE(Transducer,input); 
    HFST::KeyTable * new_kt = reorder_key_table(key_table,t);
    HFST::TransducerHandle new_input =
      HFST::harmonize_transducer(input,key_table,new_kt);
    HFST::delete_transducer(input);
    t = HANDLE_TO_PINSTANCE(Transducer,new_input); 
    HfstFst fst(t,
		new_kt,
		outstream);
  };

  TransducerHandle read_runtime_transducer( FILE * f ) {
    RuntimeReader r(f);
    Transducer * t = r.get_result();
    return PINSTANCE_TO_HANDLE(Transducer, t); 
  }

  /* Miikka Silfverberg will do this */
  KeyVector * tokenize_string( TransducerHandle tokenizer, const char * string, 
				    KeyTable *T) {
    vector<Key> * string_vector = get_key_string(string,T,TheAlphabet);
    vector<Key> * tokenized_string = lookup_first(tokenizer,string_vector);
    delete string_vector;
    return tokenized_string;
  };

  KeyVector * longest_match_tokenize( TransducerHandle tokenizer,
				      const char * string,
				      KeyTable * kt ) {
    vector<Key> * string_vector = get_key_string(string,kt,TheAlphabet);
    vector<Key> * tokenized_string = 
      longest_match_lookup(tokenizer,string_vector);
    delete string_vector;
    return tokenized_string;
  }

  KeyPairVector * longest_match_tokenize_pair( TransducerHandle tokenizer,
					   const char * string1,
					   const char * string2,
					   KeyTable * kt ) {
    KeyVector * string_vector1 = get_key_string(string1,kt,TheAlphabet);
    KeyVector * string_vector2 = get_key_string(string2,kt,TheAlphabet);
    if ((string_vector1 == NULL) or (string_vector2 == NULL)) {
      delete string_vector1;
      delete string_vector2;
      return NULL;
    }
    Transducer * pT = 
      HANDLE_TO_PINSTANCE(Transducer,tokenizer);
    KeyPairVector * tokenized_string = 
      longest_prefix_tokenize_pair(pT,
				   string_vector1,
				   string_vector2);
    delete string_vector1;
    delete string_vector2;
    return tokenized_string;
  }

  /* Miikka Silfverberg will do this */
  KeyPairVector * tokenize_pair_string( TransducerHandle tokenizer, char * pair_string, KeyTable *T ) {
    assert(false);
  }

  /* Miikka Silfverberg will do this */
  KeyPairVector * tokenize_string_pair( TransducerHandle tokenizer, 
					 const char * i1, 
					 const char * i2, 
					 KeyTable *T ) {
    Transducer * tr = HANDLE_TO_PINSTANCE(Transducer, tokenizer);
    KeyVector * input_string = get_key_string(i1, T, TheAlphabet);
    KeyVector * output_string = get_key_string(i2, T, TheAlphabet);
    if ((input_string == NULL) or (output_string == NULL)) {
      delete input_string;
      delete output_string;
      return NULL;
    }
    KeyPairVector * result = 
      tokenize_stringpair(tr,input_string,output_string);
    delete input_string;
    delete output_string;
    return result;
  };



  /* KeyPairSet-to-Alphabet conversion. Needed in functions that originally
     used TheAlphabet. */
  Alphabet keypair_set_to_alphabet(KeyPairSet *Pi) {
    Alphabet alpha = Alphabet();
    for (KeyPairIterator it=begin_pi_key(Pi); it!=end_pi_key(Pi); it++) {
      KeyPair *kp = get_pi_keypair(it);
      Label label(get_input_key(kp), get_output_key(kp));
      alpha.insert(label);
      //alpha.add_symbol(TheAlphabet.code2symbol(get_input_key(kp)), get_input_key(kp));
      //alpha.add_symbol(TheAlphabet.code2symbol(get_output_key(kp)), get_output_key(kp));
    }
    //fprintf(stderr, "keypair_set_to_alphabet ends, the result is:\n");
    //alpha.print();
    return alpha;
  }
  
  TransducerHandle repeat_n(TransducerHandle t, int n) {
    if (n==0) {
      // delete_transducer(t); 
      return create_epsilon_transducer();
    }    
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer *pResult = &(pT->copy());
    for (int i=1; i<n; i++) {
      Transducer *tmp = pResult;
      pResult = &(*pResult + *pT);
      delete tmp;
    }
    // delete pT;
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  TransducerHandle repeat_le_n(TransducerHandle t, int n) {
    if (n==0) {
      //delete_transducer(t);
      return create_epsilon_transducer();
    }
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    pT->root_node()->set_final(1);
    Transducer *pResult = &(pT->copy());
    for (int i=1; i<n; i++) {
      Transducer *tmp = pResult;
      pResult = &(*pResult + *pT);
      delete tmp;
    }
    // delete pT;
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  } 



  /* Redefine the alphabet. */

  void change_pair_set( Alphabet& alpha, KeyPairSet *Pi ) {
    alpha.clear_char_pairs();
    for ( KeyPairIterator it = begin_pi_key(Pi); it != end_pi_key(Pi); it++ ) {
      KeyPair *pair = get_pi_keypair(it);
      alpha.insert(*pair);
    }
  }
  
  /* Make a transducer accepting all pairs in the PairSet. */

  Transducer *pairset_to_transducer( KeyPairSet *Pi ) {
    
    Transducer *t=new Transducer();
    Node *node=t->new_node();
    node->set_final(1);

    /*for (::Alphabet::PublicLabelSet::const_iterator it = Pi->begin(); it!=Pi->end(); it++)
      t->root_node()->add_arc( *it, node, t );*/
    return t;

  }


  /*FA****************************************************************/
  /*                                                                 */
  /*  error                                                          */
  /*                                                                 */
  /*FE****************************************************************/
  
  static void error( const char *message ) {
    cerr << "\nError: " << message << "\naborted.\n";
    exit(1);
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  error2                                                         */
  /*                                                                 */
  /*FE****************************************************************/
  
  void error2( char *message, char *input ) {
    cerr << "\nError: " << message << ": " << input << "\naborted.\n";
    exit(1);
  }
  
  // ADDED

  void add_symbol_to_alphabet( const char *name, unsigned short p ) {
    TheAlphabet.add_symbol( name, p );
  }
  void add_parallel_name( const char *name, unsigned short p ) {
    TheAlphabet.add_parallel_name( name, p );
  }

  
  /*FA****************************************************************/
  /*                                                                 */
  /*  symbol_code                                                    */
  /*                                                                 */
  /*FE****************************************************************/
  
  Character symbol_code( char *symbol )
    
  {
    int c=TheAlphabet.symbol2code(symbol);
    if (c == EOF)
      c = TheAlphabet.add_symbol( symbol );
    free(symbol);
    return (Character)c;
  }

  Character symbol_code2( char *symbol )
    
  {
    int c=TheAlphabet.symbol2code(symbol);
    if (c == EOF)
      c = TheAlphabet.add_symbol( symbol );
    return (Character)c;
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  character_code                                                 */
  /*                                                                 */
  /*FE****************************************************************/
  
  Character character_code( unsigned int uc )
    
  {
    if (TheAlphabet.utf8)
      return symbol_code(strdup(int2utf8(uc)));
    
    unsigned char *buffer=(unsigned char*)malloc(2);
    buffer[0] = (unsigned char)uc;
    buffer[1] = 0;
    return symbol_code((char*)buffer);
  }

  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  add_value                                                      */
  /*                                                                 */
  /*FE****************************************************************/
  
  Range *add_value( Symbol c, Range *r )
    
  {
    Range *result=new Range;
    result->character = c;
    result->next = r;
    return result;
  }

  Range *insert_value( Symbol c, Range *r ) {

    // make a new Range that contains Symbol c
    Range *range_c = new Range;
    range_c->character = c;
    range_c->next = NULL;

    // r is an empty Range
    if (r == NULL)
      return range_c;

    // find the last Range in r and insert range_c after it 
    Range *last_range = r;
    while(last_range->next != NULL)
      last_range = last_range->next;
    last_range->next = range_c;

    return r;
  }

  Range *create_empty_range() {
    return NULL;
  }
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  add_values                                                     */
  /*                                                                 */
  /*FE****************************************************************/
  
  Range *add_values( unsigned int c1, unsigned int c2, Range *r )
    
  {
    for( unsigned int c=c2; c>=c1; c-- )
      r = add_value(character_code(c), r);
    return r;
  }

  // Range *insert_values( unsigned int c1, unsigned int c2, Range *r ) {
  // This is a symbol layer function, probably not useful as such

  
  /*FA****************************************************************/
  /*                                                                 */
  /*  append_values                                                  */
  /*                                                                 */
  /*FE****************************************************************/
  
  static Range *append_values( Range *r2, Range *r )
    
  {
    if (r2 == NULL)
      return r;
    return add_value(r2->character, append_values(r2->next, r));
  }

  Range *append_range( Range *r1, Range *r2 ) {

    // find the last Range in r1 
    Range *last_range = r1;
    while(last_range->next != NULL)
      last_range = last_range->next;
    // append r2 to r1
    last_range->next = r2;
    return r1;
  }    
    
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  add_var_values                                                 */
  /*                                                                 */
  /*FE****************************************************************/
  
  Range *copy_range_variable_value( char *name );
  
  Range *add_var_values( char *name, Range *r )
    
  {
    return append_values(copy_range_variable_value(name), r);
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  in_range                                                       */
  /*                                                                 */
  /*FE****************************************************************/
  
  static bool in_range( unsigned int c, Range *r )
    
  {
    while (r) {
      if (r->character == c)
	return true;
      r = r->next;
    }
    return false;
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  free_values                                                    */
  /*                                                                 */
  /*FE****************************************************************/
  
  static void free_values( Range *r )
    
  {
    while (r) {
      Range *tmp=r;
      r = r->next;
      delete tmp;
    }
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  copy_values                                                    */
  /*                                                                 */
  /*FE****************************************************************/
  
  static Range *copy_values( const Range *r )
    
  {
    if (r == NULL)
      return NULL;
    return add_value( r->character, copy_values(r->next));
  }
  
  
  /*FA****************************************************************/
  /*                                                                 */
  /*  complement_range                                               */
  /*                                                                 */
  /*FE****************************************************************/
  
  Range *complement_range( Range *r, KeySet *Si ) {
    
    vector<Character> sym;
    for( Range *p=r; p; p=p->next)
      sym.push_back( p->character );
    
    vector<Key> complement;
    for( KeyIterator it=begin_sigma_key(Si); it!=end_sigma_key(Si); it++ ) {
      Key k = get_sigma_key(it);
      if (k != Label::epsilon) {
	size_t i;
	for( i=0; i<sym.size(); i++ )
	  if (sym[i] == k)
	    break;
	if (i == sym.size())
	  complement.push_back(k);
      }
    }
    sym.swap(complement);
    
    Range *result=NULL;
    for( size_t i=0; i<sym.size(); i++ ) {
      Range *tmp = new Range;
      tmp->character = sym[i];
      tmp->next = result;
      result = tmp;
    }
    
    if(result == NULL)
      error("Complement character range is empty.");
    return result;
  }


  // for hfst-calculate
  Range *complement_range( Range *r ) {

    if(!Alphabet_Defined)
      error("Alphabet must be defined when using complement range");
  
    vector<Character> sym;
    for( Range *p=r; p; p=p->next)
      sym.push_back( p->character );

      TheAlphabet.complement(sym);
    
    Range *result=NULL;
    for( size_t i=0; i<sym.size(); i++ ) {
      Range *tmp = new Range;
      tmp->character = sym[i];
      tmp->next = result;
      result = tmp;
    }

    if(result == NULL)
      error("Complement character range is empty.");
    return result;
  }

  /*FA****************************************************************/
  /*                                                                 */
  /*  make_transducer                                                */
  /*                                                                 */
  /*FE****************************************************************/
  
  static Transducer *make_transducer( Range *r1, Range *r2, KeyPairSet *Pi=NULL )
    
  {
    Transducer *t=new Transducer();
    Node *node=t->new_node();
    node->set_final(1);
    
    if (r1 == NULL || r2 == NULL) {
      //if (!Alphabet_Defined)
      if (Pi == NULL)
	error("The wildcard symbol '.' requires the definition of an alphabet");

      // one of the ranges was '.'
      Alphabet pi_alpha = keypair_set_to_alphabet(Pi);

      for(Alphabet::const_iterator it=pi_alpha.begin(); 
	  it!=pi_alpha.end(); it++)
	if ((r1 == NULL || in_range(it->lower_char(), r1)) &&
	    (r2 == NULL || in_range(it->upper_char(), r2)))
	  t->root_node()->add_arc( *it, node, t );
    }
    else {
      for (;;) {
	Label l(r1->character, r2->character);
	// TheAlphabet.insert(l);
	t->root_node()->add_arc( l, node, t );
	if (!r1->next && !r2->next)
	  break;
	if (r1->next)
	  r1 = r1->next;
	if (r2->next)
	  r2 = r2->next;
      }
    }
    
    return t;
  }
  

  KeyPairSet *define_pair_range_final(Range *r1, Range *r2) {
    KeyPairSet *kps = create_empty_keypair_set();
    for (;;) {
      KeyPair *kp = define_keypair(r1->character, r2->character);
      kps = insert_keypair(kp, kps);
      if (!r1->next && !r2->next)
	break;
      if (r1->next)
	r1 = r1->next;
      if (r2->next)
	r2 = r2->next;
    }
    return kps;
  }

  KeyPairSet *define_pair_range_epsilon(Range *r1, Range *r2) {
    KeyPairSet *kps = create_empty_keypair_set();
    while (r1 && r2) {
      KeyPair *kp = define_keypair(r1->character, r2->character);
      kps = insert_keypair(kp, kps);
      r1 = r1->next;
      r2 = r2->next;
    }
    while (r1) {
      KeyPair *kp = define_keypair(r1->character, Label::epsilon);
      kps = insert_keypair(kp, kps);
      r1 = r1->next;
    }
    while (r2) {
      KeyPair *kp = define_keypair(Label::epsilon, r2->character);
      kps = insert_keypair(kp, kps);
      r2 = r2->next;
    }
    return kps;
  }

  KeyPairSet *define_pair_range(Range *r1, Range *r2, bool final) {
    if ( r1 == NULL || r2 == NULL )
      error("define_pair_range: the argument cannot be an empty range");
    if (final)
      return define_pair_range_final(r1, r2);
    return define_pair_range_epsilon(r1, r2);
  }



  // ******* Creating transducers *******

  TransducerHandle create_empty_transducer() {
    Transducer *t = new Transducer();
    //t->alphabet.copy(TheAlphabet);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  static Transducer *empty_transducer() {
    Transducer *t=new Transducer();
    t->root_node()->set_final(1);    
    return t;
  }
  
  TransducerHandle create_epsilon_transducer() {
    Transducer *t = empty_transducer();
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  static Transducer *one_label_transducer( Label l ) {
    Transducer *t = new Transducer();
    Node *last = t->new_node();
    t->root_node()->add_arc( l, last, t );
    last->set_final(1);
    return t;
  }

  TransducerHandle define_transducer( KeyPair *Pi ) {
    Transducer *t = one_label_transducer(*Pi);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  TransducerHandle define_transducer( Key k ) {
    Label l = Label(k,k);
    Transducer *t = one_label_transducer(l);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  Transducer *define_pi_transducer_( KeyPairSet *Pi ) {
    Transducer *t=new Transducer();
    Node *node = t->new_node();
    node->set_final(1);
    for (KeyPairIterator it = Pi->begin(); it!=Pi->end(); it++)
      (t->root_node())->add_arc( *get_pi_keypair(it), node, t);
    return t;
  }
  
  Transducer *make_filter_transducer( Key eps1, Key eps2, KeySet *ss ) {

    Transducer *t=new Transducer();
    t->root_node()->set_final(1);
    Label l = Label(eps2,eps1);
    (t->root_node())->add_arc( l, t->root_node(), t);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
      Key k = get_sigma_key(it);
      if (k != Label::epsilon) {
	Label l = Label(k,k);
	(t->root_node())->add_arc( l, t->root_node(), t);
      }
    }    

    Node *node1 = t->new_node();
    node1->set_final(1);
    Label l1 = Label(eps1,eps1);
    (t->root_node())->add_arc( l1, node1, t);
    node1->add_arc( l1, node1, t);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
      Key k = get_sigma_key(it);
      if (k != Label::epsilon) {
	Label l = Label(k,k);
	node1->add_arc( l, t->root_node(), t);
      }
    }    

    Node *node2 = t->new_node();
    node2->set_final(1);
    Label l2 = Label(eps2,eps2);
    (t->root_node())->add_arc( l2, node2, t);
    node2->add_arc( l2, node2, t);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
      Key k = get_sigma_key(it);
      if (k != Label::epsilon) {
	Label l = Label(k,k);
	node2->add_arc( l, t->root_node(), t);
      }
    }    

    //fprintf(stderr, "make_filter_transducer:\n");
    //print_operator_number(cerr, *t);
    return t;
  }

  TransducerHandle define_transducer( KeyPairSet *Pi ) {
    Transducer *t = define_pi_transducer_(Pi);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  Transducer *define_pi_transducer_( KeySet *ss ) {
    Transducer *t=new Transducer();
    Node *node = t->new_node();
    node->set_final(1);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
      Key k = get_sigma_key(it);
      Label l = Label(k,k);
      (t->root_node())->add_arc( l, node, t);
    }
    return t;
  }
  
  TransducerHandle define_transducer( KeySet *ss ) {
    Transducer *t = define_pi_transducer_(ss);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }


  KeyPairSet *define_keypair_set( TransducerHandle t ) {
    Transducer* pT = HANDLE_TO_PINSTANCE(Transducer, t);

    // user must explode and minimize
    pT->alphabet.clear_char_pairs();
    pT->complete_alphabet();         // epsilons are not inserted
    
    KeyPairSet *pairs = create_empty_keypair_set();
    
    for ( ::Alphabet::const_iterator it = pT->alphabet.begin(); it != pT->alphabet.end(); it++ ) {
      //if (it->lower_char() != Label::epsilon || it->upper_char() != Label::epsilon) {
      SymbolPair *pair = define_keypair( it->lower_char(), it->upper_char() );
      if (!has_keypair( pair, pairs )) {
	insert_keypair(pair, pairs);
	//fprintf(stderr, "define_keypair_set: added pair %hu:%hu\n", it->lower_char(), it->upper_char() );
      }
      else
	delete pair;
      //}
    }
    return pairs;
  }


/*FA****************************************************************/
/*                                                                 */
/*  new_transducer                                                 */
/*                                                                 */
/*FE****************************************************************/

Transducer *new_transducer_( Range *r1, Range *r2, KeyPairSet *Pi )

{
  Transducer *t=make_transducer( r1, r2, Pi );
  if (r1 != r2)
    free_values(r1);
  free_values(r2);
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  read_words                                                     */
/*                                                                 */
/*FE****************************************************************/

Transducer *read_words_( char *filename )

{
  if (Verbose)
    fprintf(stderr,"\nreading words from %s...", filename);
  ifstream is(filename);
  if (!is.is_open()) {
    static char message[1000];
    sprintf(message,"Error: Cannot open file \"%s\"!",filename);
    throw message;
  }
  Transducer *t = new Transducer(is, &TheAlphabet, Verbose);
  is.close();
  TheAlphabet.insert_symbols(t->alphabet);
  if (Verbose)
    fprintf(stderr,"finished\n");
  return t;
}

// ADDED
void read_symbol_table_text( istream& is ) {
  TheAlphabet.read_symbol_table_text( is );
  
}


/*FA****************************************************************/
/*                                                                 */
/*  read_transducer                                                */
/*                                                                 */
/*FE****************************************************************/

Transducer *read_transducer_( char *filename, bool alpha )

{
  if (Verbose)
    fprintf(stderr,"\nreading transducer from %s...", filename);

  /*FILE *file = fopen(filename,"rb");
  if (file == NULL) {
    static char message[1000];
    sprintf(message,"Error: Cannot open file \"%s\"!",filename);
    throw message;
  }
  Transducer t(file, true, false, alpha);
  fclose(file);*/

  ifstream is(filename);
  if (!is.is_open()) {
    static char message[1000];
    sprintf(message,"Error: Cannot open file \"%s\"!",filename);
    throw message;
  }
  Transducer t(is, true, false, alpha);
  is.close();

  if (t.alphabet.utf8 != TheAlphabet.utf8) {
    static char message[1000];
    sprintf(message,"Error: incompatible character encoding in file \"%s\"!",
	    filename);
    throw message;
  }
  Transducer *nt;
  if (alpha) {
    nt = &t.copy(false, &TheAlphabet);
    TheAlphabet.insert_symbols(nt->alphabet);
  }
  else
    nt = &t.copy(false, NULL);
  if (Verbose)
    fprintf(stderr,"finished\n");
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  def_alphabet                                                   */
/*                                                                 */
/*FE****************************************************************/

void def_alphabet_( Transducer *t )

{
  t = explode_(t);
  t = minimise_(t);
  t->alphabet.clear_char_pairs();
  t->complete_alphabet();
  TheAlphabet.clear_char_pairs();
  TheAlphabet.copy(t->alphabet);
  Alphabet_Defined = 1;
  delete t;
}


/*FA****************************************************************/
/*                                                                 */
/*  def_svar  definition of a value range variable                 */
/*                                                                 */
/*FE****************************************************************/

bool define_range_variable( char *name, Range *r )

{
  // delete the old value of the variable
  SVarMap::iterator it=SVM.find(name);
  if (it != SVM.end()) {
    char *n=it->first;
    Range *v=it->second;
    SVM.erase(it);
    delete v;
    free(n);
  }
  SVM[name] = r;
  return r == NULL;
}


/*FA****************************************************************/
/*                                                                 */
/*  svar_value                                                     */
/*                                                                 */
/*FE****************************************************************/

Range *copy_range_variable_value( char *name )

{
  SVarMap::iterator it=SVM.find(name);
  if (it == SVM.end())
    error2((char*)"undefined variable", name);
  free(name);
  return copy_values(it->second);
}


/*FA****************************************************************/
/*                                                                 */
/*  rsvar_value                                                    */
/*                                                                 */
/*FE****************************************************************/

Range *copy_range_agreement_variable_value( char *name )

{
  if (RSS.find(name) == RSS.end())
    RSS.insert(strdup(name));
  return add_value(symbol_code(name), NULL);
}


/*FA****************************************************************/
/*                                                                 */
/*  def_var  definition of a transducer variable                   */
/*                                                                 */
/*FE****************************************************************/

bool define_transducer_variable_( char *name, Transducer *t )

{
  // delete the old value of the variable
  VarMap::iterator it=VM.find(name);
  if (it != VM.end()) {
    char *n=it->first;
    Transducer *v=it->second;
    VM.erase(it);
    delete v;
    free(n);
  }

  t = explode_(t);
  t = minimise_(t);

  VM[name] = t;
  return t->is_empty();
}


/*FA****************************************************************/
/*                                                                 */
/*  def_rvar  definition of an agreement variable for automata     */
/*                                                                 */
/*FE****************************************************************/

bool define_transducer_agreement_variable_( char *name, Transducer *t )

{
  if (t->is_cyclic())
    error2((char*)"cyclic transducer assigned to", name);
  return define_transducer_variable_( name, t );
}


/*FA****************************************************************/
/*                                                                 */
/*  var_value                                                      */
/*                                                                 */
/*FE****************************************************************/

Transducer *copy_transducer_variable_value_( char *name )

{
  VarMap::iterator it=VM.find(name);
  if (it == VM.end())
    error2((char*)"undefined variable", name);
  free(name);
  return &(it->second->copy());
}


/*FA****************************************************************/
/*                                                                 */
/*  rvar_value                                                     */
/*                                                                 */
/*FE****************************************************************/

Transducer *copy_transducer_agreement_variable_value_( char *name )

{
  if (RS.find(name) == RS.end())
    RS.insert(strdup(name));
  Range *r=add_value(symbol_code(name), NULL);
  return new_transducer_(r,r);
}


/*FA****************************************************************/
/*                                                                 */
/*  explode                                                        */
/*                                                                 */
/*FE****************************************************************/

Transducer *explode_( Transducer *t )

{
  if (RS.size() == 0 && RSS.size() == 0)
    return t;

  t = minimise_(t);

  vector<char*> name;
  for( RVarSet::iterator it=RS.begin(); it!=RS.end(); it++)
    name.push_back(*it);
  RS.clear();

  // replace all agreement variables
  for( size_t i=0; i<name.size(); i++ ) {
    Transducer *nt = NULL;
    Label l(TheAlphabet.symbol2code(name[i]));
    Transducer *vt=copy_transducer_variable_value_(name[i]);

    // enumerate all paths of the transducer
    vector<Transducer*> it;
    vt->enumerate_paths(it);
    delete vt;

    // insert each path
    for( size_t i=0; i<it.size(); i++ ) {

      // insertion
      Transducer *t1 = &t->splice(l, it[i]);
      delete it[i];
      
      if (nt == NULL)
	nt = t1;
      else
	nt = disjunction_(nt, t1);
    }
    delete t;
    t = nt;
  }
  
  name.clear();
  for( RVarSet::iterator it=RSS.begin(); it!=RSS.end(); it++)
    name.push_back(*it);
  RSS.clear();

  // replace all agreement variables
  for( size_t i=0; i<name.size(); i++ ) {
    Transducer *nt = NULL;
    Character c=TheAlphabet.symbol2code(name[i]);
    Range *r=copy_range_variable_value(name[i]);

    // insert each character
    while (r != NULL) {

      // insertion
      Transducer *t1 = &t->replace_char(c, r->character);

      if (nt == NULL)
	nt = t1;
      else
	nt = disjunction_(nt, t1);

      Range *next = r->next;
      delete r;
      r = next;
    }
    delete t;
    t = nt;
  }

  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  catenate                                                       */
/*                                                                 */
/*FE****************************************************************/

Transducer *catenate_( Transducer *t1, Transducer *t2 )

{
  Transducer *t = &(*t1 + *t2);
  delete t1;
  delete t2;
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  add_range                                                      */
/*                                                                 */
/*FE****************************************************************/

Ranges *add_range( Range *r, Ranges *l )

{
  Ranges *result = new Ranges;
  result->range = r;
  result->next = l;
  return result;
}

Ranges *insert_range( Range *r, Ranges *rs ) {

  // make a new Ranges that contains Range r
  Ranges *ranges_r = new Ranges;
  ranges_r->range = r;
  ranges_r->next = NULL;
  
  // rs is an empty Ranges
  if (rs == NULL)
    return ranges_r;
  
  // find the last Ranges in rs and insert ranges_r after it 
  Ranges *last_ranges = rs;
  while(last_ranges->next != NULL)
    last_ranges = last_ranges->next;
  last_ranges->next = ranges_r;
  
  return rs;
}

KeyPairSet *define_pair_ranges( Ranges *rs1, Ranges *rs2 ) {
  KeyPairSet *kps_result = create_empty_keypair_set();
  while (rs1 && rs2) {
    KeyPairSet *kps = define_pair_range(rs1->range, rs2->range, false);
    for(KeyPairIterator it = begin_pi_key(kps); it != end_pi_key(kps); it++)
      kps_result = insert_keypair( get_pi_keypair(it), kps_result );
    rs1 = rs1->next;
    rs2 = rs2->next;
  }
  return kps_result;
}

Ranges *create_empty_ranges() {
  return NULL;
}


/*FA****************************************************************/
/*                                                                 */
/*  make_mapping                                                   */
/*                                                                 */
/*FE****************************************************************/

Transducer *make_mapping_( Ranges *l1, Ranges *l2 )

{
  Ranges *tmp1=l1;
  Ranges *tmp2=l2;

  Transducer *t=new Transducer();
  Node *node=t->root_node();
  while (l1 && l2) {
    Node *nn=t->new_node();
    for( Range *r1=l1->range; r1; r1=r1->next )
      for( Range *r2=l2->range; r2; r2=r2->next )
	node->add_arc( Label(r1->character, r2->character), nn, t );
    node = nn;
    l1 = l1->next;
    l2 = l2->next;
  }
  while (l1) {
    Node *nn=t->new_node();
    for( Range *r1=l1->range; r1; r1=r1->next )
      node->add_arc( Label(r1->character, Label::epsilon), nn, t );
    node = nn;
    l1 = l1->next;
  }
  while (l2) {
    Node *nn=t->new_node();
    for( Range *r2=l2->range; r2; r2=r2->next )
      node->add_arc( Label(Label::epsilon, r2->character), nn, t );
    node = nn;
    l2 = l2->next;
  }
  node->set_final(1);
  delete_ranges(tmp1); // ###
  delete_ranges(tmp2); // ###
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  disjunction                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *disjunction_( Transducer *t1, Transducer *t2 )

{
  Transducer *t = &(*t1 | *t2);
  delete t1;
  delete t2;
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  conjunction                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *conjunction_( Transducer *t1, Transducer *t2 )

{
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of conjunction!\n";
  Transducer *t = &(*t1 & *t2);
  delete t1;
  delete t2;
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  subtraction                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *subtraction_( Transducer *t1, Transducer *t2 )

{
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of conjunction!\n";
  Transducer *t = &(*t1 / *t2);
  delete t1;
  delete t2;
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  composition                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *composition_( Transducer *t1, Transducer *t2, bool destructive )

{
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of composition!\n";
  Transducer *t = &(*t1 || *t2);
  if (destructive) { delete t1; delete t2; }
  return t;
}

/*FA****************************************************************/
/*                                                                 */
/*  freely_insert                                                  */
/*                                                                 */
/*FE****************************************************************/

Transducer *freely_insert_( Transducer *t, Character lc, Character uc )

{
  return &t->freely_insert(Label(lc,uc));
}


/*FA****************************************************************/
/*                                                                 */
/*  negation                                                       */
/*                                                                 */
/*FE****************************************************************/

Transducer *negation_( Transducer *t, KeyPairSet *Pi )

{
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of negation!\n";

  Transducer *pi = define_pi_transducer_(Pi);
  return subtraction_(pi, t);

  /*Alphabet pi_alpha = keypair_set_to_alphabet(Pi);
  //if (!Alphabet_Defined)
  //  error("Negation requires the definition of an alphabet");
  t->alphabet.clear_char_pairs();
  t->alphabet.copy(pi_alpha);

  Transducer *nt = &(!*t);
  delete t;
  return nt;*/
}


/*FA****************************************************************/
/*                                                                 */
/*  upper_level                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *upper_level_( Transducer *t )

{
  Transducer *nt = &t->upper_level();
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  lower_level                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *lower_level_( Transducer *t )

{
  Transducer *nt = &t->lower_level();
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  minimise                                                       */
/*                                                                 */
/*FE****************************************************************/

Transducer *minimise_( Transducer *t )

{
  //t->alphabet.copy(TheAlphabet); ???
  Transducer *nt = &t->minimise( Verbose );
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  switch_levels                                                  */
/*                                                                 */
/*FE****************************************************************/

Transducer *switch_levels_( Transducer *t )

{
  Transducer *nt = &t->switch_levels();
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  repetition                                                     */
/*                                                                 */
/*FE****************************************************************/

Transducer *repetition_( Transducer *t )

{
  Transducer *nt = &(t->kleene_star());
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  repetition2                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *repetition2_( Transducer *t )

{
  Transducer *t1 = &(t->kleene_star());
  Transducer *nt = &(*t + *t1);
  delete t;
  delete t1;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  optional                                                       */
/*                                                                 */
/*FE****************************************************************/

Transducer *optional_( Transducer *t )

{
  Transducer *nt = &(t->copy());
  nt->root_node()->set_final(1);
  delete t;
  return nt;
}


/*FA****************************************************************/
/*                                                                 */
/*  add_pi_transitions                                             */
/*                                                                 */
/*FE****************************************************************/

static void add_pi_transitions( Transducer *t, Node *node, Alphabet &alph )

{
  for( Alphabet::const_iterator it=alph.begin(); it!=alph.end(); it++)
    node->add_arc( *it, node, t );
}



/*FA****************************************************************/
/*                                                                 */
/*  pi_machine                                                     */
/*                                                                 */
/*FE****************************************************************/

static Transducer *pi_machine( Alphabet &alph )

{
  Transducer *t=new Transducer();
  t->root_node()->set_final(1);
  add_pi_transitions( t, t->root_node(), alph );
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  cp                                                             */
/*                                                                 */
/*FE****************************************************************/

static Transducer *cp( Range *lower_range, Range *upper_range, KeyPairSet *Pi=NULL )

{
  return make_transducer(lower_range, upper_range, Pi);
}


/*FA****************************************************************/
/*                                                                 */
/*  anti_cp                                                        */
/*                                                                 */
/*FE****************************************************************/

static Transducer *anti_cp( Range *lower_range, Range *upper_range, KeyPairSet *Pi )

{
  Transducer *cpt = cp(lower_range, upper_range, Pi);
  Transducer *t=new Transducer();
  Node *node=t->new_node();

  node->set_final(1);
  Alphabet pi_alpha = keypair_set_to_alphabet(Pi);
  for(Alphabet::const_iterator it=pi_alpha.begin(); 
      it!=pi_alpha.end(); it++){
    Label l=*it;
    if (in_range(l.lower_char(), lower_range) &&
	!cpt->root_node()->target_node(l))
      t->root_node()->add_arc( l, node, t );
  }
  if (in_range(Label::epsilon, lower_range) &&
      !cpt->root_node()->target_node(Label()))
      t->root_node()->add_arc( Label(), node, t );

  delete cpt;
  return t;
}


/*FA****************************************************************/
/*                                                                 */
/*  twol_right_rule                                                */
/*                                                                 */
/*FE****************************************************************/

static Transducer *twol_right_rule( Transducer *lc, Range *lower_range, 
				   Range *upper_range, Transducer *rc, KeyPairSet *Pi )

{
  Alphabet pi_alpha = keypair_set_to_alphabet(Pi);

  // Build the rule transducer
  Transducer *cpt = cp(lower_range, upper_range, Pi);
  Transducer *pi=pi_machine(pi_alpha);

  // First unwanted language

  lc->alphabet.copy(pi_alpha);
  Transducer *notlc = &(!*lc);
  Transducer *tmp = &(*notlc + *cpt);
  delete notlc;
  Transducer *t1 = &(*tmp + *pi);
  delete tmp;

  // Second unwanted language
  rc->alphabet.copy(pi_alpha);
  Transducer *notrc = &(!*rc);
  tmp = &(*cpt + *notrc);
  delete notrc;
  Transducer *t2 = &(*pi + *tmp);
  delete tmp;

  tmp = &(*t1|*t2); 
  delete t1; 
  delete t2; 

  tmp->alphabet.copy(pi_alpha);
  t1 = &(!*tmp); 
  delete tmp; 

  delete pi;  // #####
  delete cpt; // #####


  return t1;
}


/*FA****************************************************************/
/*                                                                 */
/*  twol_left_rule                                                 */
/*                                                                 */
/*FE****************************************************************/

static Transducer *twol_left_rule( Transducer *lc, Range *lower_range,
				   Range *upper_range, Transducer *rc, KeyPairSet *Pi )

{

  Alphabet pi_alpha = keypair_set_to_alphabet(Pi);
  // check for problematic insertion operations like "$L <> <= a $R"
  // where either $L or $R includes the empty string
  if (in_range(Label::epsilon, lower_range)) {
    if (lc->generates_empty_string())
      error("in two level rule: insertion operation with deletable left context!");
    if (rc->generates_empty_string())
      error("in two level rule: insertion operation with deletable right context!");
    cerr << "\nWarning: two level rule used for insertion operation (might produce unexpected results)\n";
  }
  
  // Build the rule transducer
  Transducer *t1 = anti_cp(lower_range, upper_range, Pi);
  
  // Add the left context;
  Transducer *t2 = &(*lc + *t1);
  delete t1;

  // Add the right context;
  t1 = &(*t2 + *rc);
  delete t2;

  // Form the complement
  t1->alphabet.copy(pi_alpha);
  t2 = &(!*t1);
  delete t1;

  return t2;
}


/*FA****************************************************************/
/*                                                                 */
/*  make_rule                                                      */
/*                                                                 */
/*FE****************************************************************/

Transducer *make_rule_( Transducer *lc, Range *lower_range, Twol_Type type, 
			Range *upper_range, Transducer *rc, KeyPairSet *Pi )

{
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of replacement rule!\n";

  if (!Alphabet_Defined)
    error("Two level rules require the definition of an alphabet");

  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  // expand the left and the right contexts to their full length
  Transducer *pi=pi_machine(alpha_pi);

  if (lc == NULL)
    lc = pi_machine(alpha_pi);
  else {
    Transducer *tmp = &(*pi + *lc);
    delete lc;
    lc = tmp;
  }
  if (rc == NULL)
    rc = pi_machine(alpha_pi);
  else {
    Transducer *tmp = &(*rc + *pi);
    delete rc;
    rc = tmp;
  }
  delete pi;

  Transducer *result = NULL;

  switch (type) {
  case twol_left:
    result = twol_left_rule(lc, lower_range, upper_range, rc, Pi);
    break;
  case twol_right:
    result = twol_right_rule(lc, lower_range, upper_range, rc, Pi);
    break;
  case twol_both:
    {
      Transducer *t1 = twol_left_rule(lc, lower_range, upper_range, rc, Pi);
      Transducer *t2 = twol_right_rule(lc, lower_range, upper_range, rc, Pi);
      result = &(*t1 & *t2);
      delete t1;
      delete t2;
    }
  }
  delete lc;
  delete rc;
  if (lower_range != upper_range)
    free_values(lower_range);
  free_values(upper_range);

  return minimise_(result);
}


/*FA****************************************************************/
/*                                                                 */
/*  make_context                                                   */
/*                                                                 */
/*FE****************************************************************/

Contexts *make_context_( Transducer *l, Transducer *r )

{
  if (l == NULL)
    l = empty_transducer();
  if (r == NULL)
    r = empty_transducer();

  Contexts *c=new Contexts();
  c->left = l;
  c->right = r;
  c->next = NULL;

  return c;
}


/*FA****************************************************************/
/*                                                                 */
/*  add_context                                                    */
/*                                                                 */
/*FE****************************************************************/

Contexts *add_context_( Contexts *nc, Contexts *c )

{
  nc->next = c;
  return nc;
}


/*FA****************************************************************/
/*                                                                 */
/*  restriction_transducer                                         */
/*                                                                 */
/*FE****************************************************************/

static Transducer *restriction_transducer( Transducer *l1, Transducer *l2,
					   Character marker, KeyPairSet *Pi )

{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);
  l1->alphabet.copy(alpha_pi);
  Transducer *t1 = &(*l1 / *l2);

  Transducer *t2 = &t1->replace_char(marker, Label::epsilon);
  delete t1;

  t2->alphabet.copy(alpha_pi);
  t1 = &(!*t2);
  delete t2;

  return t1;
}


/*FA****************************************************************/
/*                                                                 */
/*  marker_transducer                                              */
/*                                                                 */
/*FE****************************************************************/

// alpha argument added by Erik Axelson
static Transducer *marker_transducer( Transducer *t, Contexts *c,
				      Character &marker, Alphabet& alpha_pi )
{
  // create the marker symbol
  // collect all keys that are used in arguments
  Alphabet alpha = Alphabet();  
  alpha.copy(alpha_pi);
  t->complete_alphabet();
  alpha.copy(t->alphabet);
  Contexts *C = c;
  while(C) {
    if (C->left) {
      C->left->complete_alphabet();
      alpha.copy(C->left->alphabet);
    }
    if(C->right) {
      C->right->complete_alphabet();
      alpha.copy(C->right->alphabet);
    }
    C = C->next;
  }  
  // find unused keys
  marker = alpha.new_marker();
  //marker = TheAlphabet.new_marker();
  Transducer *result = one_label_transducer( Label(marker) );

  // build the alphabet with a new marker
  result->alphabet.insert_symbols(t->alphabet);
  while (c) {
    result->alphabet.insert_symbols(c->left->alphabet);
    result->alphabet.insert_symbols(c->right->alphabet);
    c = c->next;
  }

  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  center_transducer                                              */
/*                                                                 */
/*FE****************************************************************/

static Transducer *center_transducer( Transducer *t, Transducer *pi, 
				      Transducer *mt )
{
  // create the concatenation   pi + mt + *t + mt + pi
  Transducer *t1=&(*pi + *mt);
  Transducer *t2=&(*t1 + *t);
  delete t1;
  t1 = &(*t2 + *mt);
  delete t2;
  t2 = &(*t1 + *pi);
  delete t1;
  return t2;
}


/*FA****************************************************************/
/*                                                                 */
/*  context_transducer                                             */
/*                                                                 */
/*FE****************************************************************/

static Transducer *context_transducer( Transducer *t, Transducer *pi,
				       Transducer *mt, Contexts *c )
{
  // pi + left[i] + mt + pi + mt + right[i] + pi
  
  Transducer *t1 = &(*mt + *t);
  Transducer *tmp = &(*t1 + *mt);
  delete t1;
  Transducer *result=NULL;
  Contexts *c_tmp=NULL;

  while (c) {
    t1 = &(*pi + *c->left);
    delete c->left;  // #####
    Transducer *t2 = &(*t1 + *tmp);
    delete t1;
    t1 = &(*t2 + *c->right);
    delete c->right; // #####
    delete t2;
    t2 = &(*t1 + *pi);
    delete t1;

    if (result) {
      t1 = &(*result | *t2);
      delete t2;
      result = t1;
    }
    else 
      result = t2;

    c_tmp=c;
    c = c->next;
    delete c_tmp;  // #####
  }
  delete tmp;

  return result;
}



/*FA****************************************************************/
/*                                                                 */
/*  result_transducer                                              */
/*                                                                 */
/*FE****************************************************************/

static Transducer *result_transducer( Transducer *l1, Transducer *l2,
				      Twol_Type type, Character marker,
				      KeyPairSet *Pi )
{
  Transducer *result=NULL;
  if (type == twol_right)
    result = restriction_transducer( l1, l2, marker, Pi );
  else if (type == twol_left)
    result = restriction_transducer( l2, l1, marker, Pi );
  else if (type == twol_both) {
    Transducer *t1 = restriction_transducer( l1, l2, marker, Pi );
    Transducer *t2 = restriction_transducer( l2, l1, marker, Pi );
    result = &(*t1 & *t2);
    delete t1;
    delete t2;
  }

  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  restriction                                                    */
/*                                                                 */
/*FE****************************************************************/

Transducer *restriction_( Transducer *t, Twol_Type type, Contexts *c, 
			  int direction, KeyPairSet *Pi )
{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  Character marker;
  Transducer *mt=marker_transducer( t, c, marker, alpha_pi );
  Transducer *pi=pi_machine(alpha_pi);
  Transducer *l1=center_transducer( t, pi, mt );

  Transducer *tmp;
  if (direction == 0)
    tmp = pi;
  else if (direction == 1) {
    // compute  _t || .*
    Transducer *t1 = &t->lower_level();
    tmp = &(*t1 || *pi);
    delete t1;
  }
  else {
    // compute  ^t || .*
    Transducer *t1 = &t->upper_level();
    tmp = &(*pi || *t1);
    delete t1;
  }
  delete t;

  Transducer *l2=context_transducer( tmp, pi, mt, c );
  if (tmp != pi)
    delete tmp;
  delete pi;
  delete mt;

  Transducer *result=result_transducer( l1, l2, type, marker, Pi );
  delete l1;
  delete l2;

  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  constrain_boundary_transducer                                  */
/*                                                                 */
/*FE****************************************************************/

Transducer *constrain_boundary_transducer( Character leftm, Character rightm,
					   KeyPairSet *Pi )

{
  // create the transducer  (.|<L>|<R>)*

  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);
  Transducer *tmp=pi_machine(alpha_pi);

  // create the transducer  (.|<L>|<R>)* <L><R> (.|<L>|<R>)*
  Node *root = tmp->root_node();
  Node *node = tmp->new_node();
  Node *last = tmp->new_node();

  root->set_final(0);
  last->set_final(1);

  root->add_arc( Label(leftm), node, tmp);
  node->add_arc( Label(rightm), last, tmp);

  add_pi_transitions( tmp, last, alpha_pi );

  // create the transducer  !((.|<L>|<R>)* <L><R> (.|<L>|<R>)*)
  tmp->alphabet.copy(alpha_pi);
  Transducer *result = &(!*tmp);
  delete tmp;

  return result;
}


/*FA****************************************************************/
/*                                                                 */
/*  extended_context                                               */
/*                                                                 */
/*FE****************************************************************/

Transducer *extended_context( Transducer *t, Character m1, Character m2,
			      KeyPairSet *Pi )

{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  if (t == NULL) // empty context
    return pi_machine(alpha_pi);

  // Extended context transducer

  // <R> >> (<L> >> $T$)
  Transducer *tmp=&t->freely_insert( Label(m1) );
  delete t;
  t = &tmp->freely_insert( Label(m2) );
  delete tmp;

  // .* (<R> >> (<L> >> $T$))
  add_pi_transitions( t, t->root_node(), alpha_pi );

  // !(.*<L>)
  tmp = one_label_transducer(Label(m1));
  add_pi_transitions( tmp, tmp->root_node(), alpha_pi );
  tmp->alphabet.copy(alpha_pi);
  Transducer *t2 = &(!*tmp);
  delete tmp;
    
  // .* (<R> >> (<L> >> $T$)) || !(.*<L>)
  tmp = &(*t || *t2);
  delete t;
  delete t2;

  return tmp;
}


/*FA****************************************************************/
/*                                                                 */
/*  replace_context                                                */
/*                                                                 */
/*FE****************************************************************/

Transducer *replace_context( Transducer *t, Character m1, Character m2,
			     KeyPairSet *Pi )

{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  // $C$ = .* (<L> >> (<R> >> $T$))
  Transducer *ct = extended_context(t, m1, m2, Pi);

  // <R>*<L> .*
  Transducer *mt = one_label_transducer(Label(m1));
  mt->root_node()->add_arc(Label(m2), mt->root_node(), mt );
  add_pi_transitions(mt, mt->root_node()->target_node(Label(m1)),alpha_pi);

  ct->alphabet.copy(alpha_pi);
  Transducer *no_ct = &!*ct;

  mt->alphabet.copy(alpha_pi);
  Transducer *no_mt = &!*mt;

  Transducer *t1 = &(*no_ct + *mt);
  delete no_ct;
  delete mt;

  Transducer *t2 = &(*ct + *no_mt);
  delete ct;
  delete no_mt;

  Transducer *tmp = &(*t1 | *t2);
  delete t1;
  delete t2;

  tmp->alphabet.copy(alpha_pi);
  t1 = &!*tmp;
  delete tmp;

  return t1;
}


/*FA****************************************************************/
/*                                                                 */
/*  make_optional                                                  */
/*                                                                 */
/*FE****************************************************************/

static Transducer *make_optional_( Transducer *t, KeyPairSet *Pi )

{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);
  Transducer *t1 = pi_machine(alpha_pi);
  Transducer *t2 = &(*t | *t1);
  delete t;
  delete t1;
  return t2;
}



/*FA****************************************************************/
/*                                                                 */
/*  replace                                                        */
/*                                                                 */
/*FE****************************************************************/

Transducer *replace_( Transducer *ct, Repl_Type type, bool optional, KeyPairSet *Pi )

{
  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  // compute the no-center transducer
  Transducer *tmp;

  if (type == repl_up)
    // _ct
    tmp = &ct->lower_level();
  else if (type == repl_down)
    // ^ct
    tmp = &ct->upper_level();
  else
    error("Invalid type of replace operator");

  // .* _ct
  add_pi_transitions( tmp, tmp->root_node(), alpha_pi );

  // .*  _ct .*
  Transducer *t2 = pi_machine(alpha_pi);
  Transducer *t3 = &(*tmp + *t2);
  delete tmp;
  delete t2;

  // no_ct = !(.*  _ct .*)
  t3->alphabet.copy(alpha_pi);
  Transducer *no_ct = &(!*t3);
  delete t3;

  // compute the unconditional replacement transducer

  // no-ct ct
  tmp = &(*no_ct + *ct);
  delete ct;

  // (no-ct ct)*
  t2 = &(tmp->kleene_star());
  delete tmp;

  // (no-ct ct)* no-ct
  tmp = &(*t2 + *no_ct);
  delete t2;
  delete no_ct;

  if (optional)
    tmp = make_optional_(tmp, Pi);

  return tmp;
}


/*FA****************************************************************/
/*                                                                 */
/*  replace_transducer                                             */
/*                                                                 */
/*FE****************************************************************/

Transducer *replace_transducer( Transducer *ct, Character lm, Character rm, 
				Repl_Type type, KeyPairSet *Pi )
{
  // insert boundary markers into the center transducer

  // <L> >> (<R> >> $Center$)
  Transducer *tmp = &ct->freely_insert(Label(lm));
  delete ct;
  ct = &tmp->freely_insert(Label(rm));
  delete tmp;
  
  // add surrounding boundary markers to the center transducer

  // <L> (<L> >> (<R> >> $Center$))
  Transducer *t2 = one_label_transducer( Label(lm) );
  tmp = &(*t2 + *ct);
  delete t2;
  delete ct;
      
  // $CenterB$ = <L> (<L> >> (<R> >> $Center$)) <R>
  t2 = one_label_transducer( Label(rm) );
  ct = &(*tmp + *t2);
  delete tmp;
  delete t2;

  return replace_(ct, type, false, Pi);
}


void insert_to_keyset(KeySet *ks, KeySet *i) {

  for ( KeyIterator it = begin_sigma_key(i); it != end_sigma_key(i); it++ )
    ks->insert(get_sigma_key(it));
}

void insert_to_keyset(KeySet *ks, KeyPairSet *i) {

  for ( KeyPairIterator it = i->begin(); it != i->end(); it++ ) {
    ks->insert((*it)->upper_char());
    ks->insert((*it)->lower_char());
  }
}


/*FA****************************************************************/
/*                                                                 */
/*  replace_in_context                                             */
/*                                                                 */
/*FE****************************************************************/

Transducer *replace_in_context_( Transducer *t, Repl_Type type, Contexts *c,
				 bool optional, KeyPairSet *Pi )
{

  //fprintf(stderr, "### replace_in_context begins... \n");

  Alphabet alpha_pi = keypair_set_to_alphabet(Pi);

  if (!Alphabet_Defined)
    error("The replace operators require the definition of an alphabet");

  if (!c->left->is_automaton() || !c->right->is_automaton())
    error("The replace operators require automata as context expressions!");


  // create the marker symbols
  // collect all keys that are used in arguments
  KeySet *ks = new KeySet();
  insert_to_keyset(ks, Pi);
  KeySet *ks1 = define_key_set(PINSTANCE_TO_HANDLE(Transducer, t));
  insert_to_keyset(ks, ks1);
  ks1->clear(); delete ks1; // ###
  Contexts *C = c;
  while(C) {
    if (C->left) {
      KeySet *ks2 = define_key_set(PINSTANCE_TO_HANDLE(Transducer, C->left));
      insert_to_keyset(ks, ks2);
      ks2->clear(); delete ks2; // ###
    }
    if(C->right) {
      KeySet *ks3 = define_key_set(PINSTANCE_TO_HANDLE(Transducer, C->right));
      insert_to_keyset(ks, ks3);
      ks3->clear(); delete ks3; // ###
    }
    C = C->next;
  }

  // find unused keys
  Key leftm=0;
  for (Key k=1; k != 0; k++) {
    if (ks->find(k) == ks->end()) {
      leftm=k;
      break;
    }
  }
  if (leftm == 0) {
      fprintf(stderr, "ERROR: replace_in_context: marker key could not be created\n");
      exit(1);
  }
  Key rightm=0;
  for (Key k=leftm+1; k != 0; k++) {
    if (ks->find(k) == ks->end()) {
      rightm=k;
      break;
    }
  }
  if (rightm == 0) {
      fprintf(stderr, "ERROR: replace_in_context: marker key could not be created\n");
      exit(1);
  }
  ks->clear(); delete ks; // ###

  //fprintf(stderr, "### new markers: %hu and %hu\n", leftm, rightm);

  /////////////////////////////////////////////////////////////
  // Create the insert boundary transducer (.|<>:<L>|<>:<R>)*
  /////////////////////////////////////////////////////////////
  
  Transducer *ibt=pi_machine(alpha_pi);
  Node *root=ibt->root_node();
  root->add_arc( Label(Label::epsilon, leftm), root, ibt);
  root->add_arc( Label(Label::epsilon, rightm),root, ibt);

  /////////////////////////////////////////////////////////////
  // Create the remove boundary transducer (.|<L>:<>|<R>:<>)*
  /////////////////////////////////////////////////////////////

  Transducer *rbt=pi_machine(alpha_pi);
  root = rbt->root_node();
  root->add_arc( Label(leftm, Label::epsilon), root, rbt);
  root->add_arc( Label(rightm,Label::epsilon), root, rbt);

  // Add the markers to the alphabet AND TO Pi
  alpha_pi.insert(Label(leftm));
  alpha_pi.insert(Label(rightm));
  KeyPair *leftm_keypair = define_keypair(leftm, leftm); // memory leaks?
  KeyPair *rightm_keypair = define_keypair(rightm, rightm);
  insert_keypair(leftm_keypair, Pi);
  insert_keypair(rightm_keypair, Pi);

  /////////////////////////////////////////////////////////////
  // Create the constrain boundary transducer !(.*<L><R>.*)
  /////////////////////////////////////////////////////////////

  Transducer *cbt=constrain_boundary_transducer(leftm, rightm, Pi);

  /////////////////////////////////////////////////////////////
  // Create the extended context transducers
  /////////////////////////////////////////////////////////////

  // left context transducer:  .* (<R> >> (<L> >> $T$)) || !(.*<L>)
  Transducer *lct = replace_context(c->left, leftm, rightm, Pi);
  //delete c->left; replace_context deletes its argument

  // right context transducer:  (<R> >> (<L> >> $T$)) .* || !(<R>.*)
  Transducer *tmp = &c->right->reverse();
  delete c->right;
  delete c;  // #####
  Transducer *t2 = replace_context(tmp, rightm, leftm, Pi);
  Transducer *rct = &t2->reverse();
  delete t2;

  /////////////////////////////////////////////////////////////
  // unconditional replace transducer
  /////////////////////////////////////////////////////////////

  Transducer *rt;
  if (type == repl_up || type == repl_right || type == repl_left)
    rt = replace_transducer( t, leftm, rightm, repl_up, Pi );
  else
    rt = replace_transducer( t, leftm, rightm, repl_down, Pi );

  /////////////////////////////////////////////////////////////
  // build the conditional replacement transducer
  /////////////////////////////////////////////////////////////

  tmp = ibt;
  tmp = &(*ibt || *cbt);
  delete(ibt);
  delete(cbt);

  if (type == repl_up || type == repl_right) {
    t2 = &(*tmp || *rct);
    delete tmp;
    delete rct;
    tmp = t2;
  }
  if (type == repl_up || type == repl_left) {
    t2 = &(*tmp || *lct);
    delete tmp;
    delete lct;
    tmp = t2;
  }

  t2 = &(*tmp || *rt);
  delete tmp;
  delete rt;
  tmp = t2;
    
  if (type == repl_down || type == repl_right) {
    t2 = &(*tmp || *lct);
    delete tmp;
    delete lct;
    tmp = t2;
  }
  if (type == repl_down || type == repl_left) {
    t2 = &(*tmp || *rct);
    delete tmp;
    delete rct;
    tmp = t2;
  }
  
  t2 = &(*tmp || *rbt);
  delete tmp;
  delete rbt;

  // Remove the markers from the alphabet   AND FROM Pi
  alpha_pi.delete_markers();
  Pi->erase(leftm_keypair);
  Pi->erase(rightm_keypair);
  delete leftm_keypair; delete rightm_keypair;  // ####

  if (optional)
    t2 = make_optional_(t2, Pi);

  return t2;
}


/*FA****************************************************************/
/*                                                                 */
/*  add_alphabet                                                   */
/*                                                                 */
/*FE****************************************************************/

void add_alphabet( Transducer *t )

{
  t->alphabet.copy(TheAlphabet);
  t->complete_alphabet();
}


/*FA****************************************************************/
/*                                                                 */
/*  store_transducer                                               */
/*                                                                 */
/*FE****************************************************************/

void store_transducer( Transducer *t, char *filename )

{
  if (filename == NULL)
    cout << *t;
  else {
    ofstream os(filename);
    os << *t;
    os.close();
  }
}

/*FA****************************************************************/
/*                                                                 */
/*  write_to_file                                                  */
/*                                                                 */
/*FE****************************************************************/

void write_to_file_( Transducer *t, char *filename, bool alpha)

{
  FILE *file;
  if ((file = fopen(filename,"wb")) == NULL) {
    fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", filename);
    exit(1);
  }

  t = explode_(t);
  if (alpha)
    add_alphabet(t);
  /*else
    t->keys_to_alphabet();*/
  t = minimise_(t);
  t->store(file, alpha);
  fclose(file);
}

void write_to_file_( Transducer *t, FILE *file )

{
  t = explode_(t);
  add_alphabet(t);
  t = minimise_(t);
  t->store(file);
}

/*FA****************************************************************/
/*                                                                 */
/*  result                                                         */
/*                                                                 */
/*FE****************************************************************/

Transducer *result_( Transducer *t, bool switch_flag, bool reverse )

{
  t = explode_(t);

  // delete the variable values
  vector<char*> s;
  for( VarMap::iterator it=VM.begin(); it != VM.end(); it++ ) {
    s.push_back(it->first);
    delete it->second;
    it->second = NULL;
  }
  VM.clear();
  for( size_t i=0; i<s.size(); i++ )
    free(s[i]);
  s.clear();

  if (switch_flag)
    t = switch_levels_(t);
  if (reverse)
    t = &t->reverse();
  add_alphabet(t);
  t->alphabet.copy(TheAlphabet);  // ADDED
  t = minimise_(t);
  return t;
}

void delete_transducer(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  delete pT;
};
  
vector<char*> * get_forms( Transducer * t );

// does not work with different symbol tables
TransducerHandle disjunct_transducers_as_tries( TransducerHandle t1, TransducerHandle t2 ) {
  Transducer *pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
  Transducer *pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
  //vector<char*> *forms = get_forms(pT2);
  vector<vector<Label> > paths;
  pT2->enumerate_paths(paths);
  delete pT2;
  for (unsigned int i=0; i<paths.size(); i++)
    pT1->add_path(paths[i]);
  //for (unsigned int i=0; i<paths.size(); i++)
  //  delete paths[i];
    /*for (unsigned int i=0; i<forms->size(); i++)
      pT1->add_string(forms->at(i), true);
      for (unsigned int i=0; i<forms->size(); i++)
      delete forms->at(i);
      delete forms;*/
  return PINSTANCE_TO_HANDLE(Transducer, pT1);
}

TransducerHandle disjunct_as_trie( TransducerHandle t, KeyVector * key_string, float weight, bool sum_weights ) {
  Transducer * pT = 
    HANDLE_TO_PINSTANCE(Transducer,t);
  add_key_string(key_string,pT);
  return t;
}

TransducerHandle disjunct_as_trie( TransducerHandle t, KeyPairVector * key_string, float weight, bool sum_weights ) {
  Transducer * pT = 
    HANDLE_TO_PINSTANCE(Transducer,t);
  add_key_pair_string(key_string,pT);
  return t;
}

  TransducerHandle make_replace_in_context( TransducerHandle t, Repl_Type type, ContextsHandle p, 
					    bool optional, KeyPairSet *Pi ) {
    //fprintf(stderr, "make_replace_in_context beginning...\n");
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Contexts *P = new Contexts();
    P->left = HANDLE_TO_PINSTANCE(Transducer, p->left);
    P->right = HANDLE_TO_PINSTANCE(Transducer, p->right);
    delete p; // ####
    Transducer * pResult = replace_in_context_(pT, type, P, optional, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle negate( TransducerHandle t, KeyPairSet *Pi ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = negation_(pT, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle insert_freely( TransducerHandle t, KeyPair *p ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = freely_insert_(pT, get_input_key(p), get_output_key(p));
    delete pT; // #####
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

// for hfst-calculate
TransducerHandle insert_freely( TransducerHandle t, Key k1, Key k2 ) {
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer* const pResult = freely_insert_(pT, k1, k2);
  delete pT; // #####
  return PINSTANCE_TO_HANDLE(Transducer, pResult);
}


  TransducerHandle make_rule( TransducerHandle t1, Range *r1, Twol_Type type,
			      Range *r2, TransducerHandle t2, KeyPairSet *Pi ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = make_rule_(pT1, r1, type, r2, pT2, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle make_rule( TransducerHandle t1, KeyPairSet *mappings, Twol_Type type,
			      TransducerHandle t2, KeyPairSet *Pi ) {
    Range *r1 = create_empty_range();
    Range *r2 = create_empty_range();
    for ( KeyPairIterator it = begin_pi_key(mappings); it != end_pi_key(mappings); it++ ) {
      KeyPair *kp = get_pi_keypair(it);
      r1 = insert_value(get_input_key(kp), r1);
      r2 = insert_value(get_output_key(kp), r2);
    }
    return make_rule( t1, r1, type, r2, t2, Pi );
  }


/* TransducerHandle make_rule( TransducerHandle t1, TransducerHandle r, Twol_Type type,
			      TransducerHandle t2, KeyPairSet *Pi ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);

    Transducer* pR = HANDLE_TO_PINSTANCE(Transducer, r);
    Transducer* tmp = pR; 
    pR = &pR->reverse();
    delete tmp;
    Range *r1 = NULL;
    Range *r2 = NULL;

    Node *node = pR->root_node();
    while(!node->is_final()) {
      for (ArcsIter it(node->arcs()); it; it++) {
	Arc a = *it;
	Label l = a.label();
	r1 = add_value( l.lower_char(), r1 );
	r2 = add_value( l.upper_char(), r2 );
	node = a.target_node();
      }
    }

    Transducer* const pResult = make_rule_(pT1, r1, type, r2, pT2, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

*/
  TransducerHandle make_restriction( TransducerHandle t, Twol_Type type, ContextsHandle p,
				     int direction, KeyPairSet *Pi ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);

    Contexts *P = make_context_(HANDLE_TO_PINSTANCE(Transducer, p->left),
				HANDLE_TO_PINSTANCE(Transducer, p->right));
    Contexts *P_old = P;
    
    ContextsHandle tmp=p;
    p = p->next;
    delete tmp;
    while(p) {
      Contexts *Padd = make_context_(HANDLE_TO_PINSTANCE(Transducer, p->left),
				     HANDLE_TO_PINSTANCE(Transducer, p->right));
       P = add_context_(P, Padd);
      P = Padd;
      tmp=p;
      p = p->next;
      delete tmp;
    }
    P = add_context_(P, NULL);
    P = P_old;
    
    Transducer * pResult = restriction_(pT, type, P, direction, Pi);
    
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle make_replace( TransducerHandle t, Repl_Type type, bool optional, KeyPairSet *Pi ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = replace_(pT, type, optional, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle read_words( char* filename ) {
    Transducer* const pResult = read_words_(filename);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle explode( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = explode_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle minimize( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = minimise_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

TransducerHandle push_weights( TransducerHandle t, bool initial ) {
  return t;
}

TransducerHandle push_labels( TransducerHandle t, bool initial ) {
  return t;
}

TransducerHandle modify_weights( TransducerHandle t,
				 float(*modify)(float),
				 bool modify_transitions ) {
  return t;
}


//TransducerHandle unweighted_to_weighted( TransducerHandle t ) {
//  return to_weighted(t);
//}

//TransducerHandle weighted_to_unweighted( TransducerHandle t ) {
//  return to_unweighted(t);
//}


  TransducerHandle determinize( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer *pResult = &pT->determinise();
    delete pT;
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  Key find_unused_key(set<Key> &keys);


TransducerHandle compose( TransducerHandle t1, TransducerHandle t2, bool destructive) {
    bool filter=false;
    if (filter) {
      KeySet *ks1 = define_key_set(t1);
      KeySet *ks2 = define_key_set(t2);
      for ( KeyIterator it = begin_sigma_key(ks1); it != end_sigma_key(ks1); it++ )
	ks2->insert(get_sigma_key(it));
      Key eps1 = find_unused_key(*ks2);
      //fprintf(stderr, "first unused key: %hu\n", eps1);
      ks2->insert(eps1);
      Key eps2 = find_unused_key(*ks2);
      //fprintf(stderr, "second unused key: %hu\n", eps2);
      TransducerHandle T1, T2;
      if (destructive) {
	T1 = substitute_key(t1,Label::epsilon,eps2,true);
	T2 = substitute_key(t2,Label::epsilon,eps1,true);
      }
      else {
	T1 = substitute_key(copy(t1),Label::epsilon,eps2,true);
	T2 = substitute_key(copy(t2),Label::epsilon,eps1,true);
      }

      Transducer *filter_transducer = make_filter_transducer(eps1, eps2, ks1);
      Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, T1);
      Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, T2);
      Transducer *tmp = composition_(pT1, filter_transducer, false); // should be true !!!
      Transducer *pResult = composition_(tmp, pT2, true);
      return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }
    else {
      Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
      Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
      Transducer* const pResult = composition_(pT1, pT2, destructive);
      return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }
  }

TransducerHandle compose( TransducerHandle t1, TransducerHandle t2) {
    bool destructive = true;
    bool filter=false;
    if (filter) {
      KeySet *ks1 = define_key_set(t1);
      KeySet *ks2 = define_key_set(t2);
      for ( KeyIterator it = begin_sigma_key(ks1); it != end_sigma_key(ks1); it++ )
	ks2->insert(get_sigma_key(it));
      Key eps1 = find_unused_key(*ks2);
      //fprintf(stderr, "first unused key: %hu\n", eps1);
      ks2->insert(eps1);
      Key eps2 = find_unused_key(*ks2);
      //fprintf(stderr, "second unused key: %hu\n", eps2);
      TransducerHandle T1, T2;
      if (destructive) {
	T1 = substitute_key(t1,Label::epsilon,eps2,true);
	T2 = substitute_key(t2,Label::epsilon,eps1,true);
      }
      else {
	T1 = substitute_key(copy(t1),Label::epsilon,eps2,true);
	T2 = substitute_key(copy(t2),Label::epsilon,eps1,true);
      }

      Transducer *filter_transducer = make_filter_transducer(eps1, eps2, ks1);
      Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, T1);
      Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, T2);
      Transducer *tmp = composition_(pT1, filter_transducer, false); // should be true !!!
      Transducer *pResult = composition_(tmp, pT2, true);
      return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }
    else {
      Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
      Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
      Transducer* const pResult = composition_(pT1, pT2, destructive);
      return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }
  }

  TransducerHandle repeat_star( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = repetition_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }
  
  TransducerHandle repeat_plus( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = repetition2_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }
  
Transducer *shuffle( Transducer *t1, Transducer *t2 ) {
  if (RS.size() > 0 || RSS.size() > 0)
    cerr << "\nWarning: agreement operation inside of shuffle!\n";
  Transducer *t = &(*t1 && *t2);
  delete t1;
  delete t2;
  return t;
}

  TransducerHandle shuffle( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = shuffle(pT1, pT2);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

/*TransducerHandle optionalize( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = optional_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }*/

TransducerHandle optionalize( TransducerHandle t ) {
  return disjunct(t, create_epsilon_transducer());
}

  
  TransducerHandle concatenate( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = catenate_(pT1, pT2);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle reverse( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* pResult = &pT->reverse();
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle invert( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = switch_levels_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle extract_output_language( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = upper_level_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }
  
  TransducerHandle extract_input_language( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = lower_level_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

TransducerHandle add_input_language( TransducerHandle t, KeyPairSet *s ) {
  Transducer* pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer *pi=new Transducer();
  pi->root_node()->set_final(1);
  for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
    KeyPair *kp = get_pi_keypair(it);
    if (get_input_key(kp) != Label::epsilon)
      (pi->root_node())->add_arc(*kp, pi->root_node(), pi);
  }
  Transducer *comp = composition_(pT, pi);
  for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
    KeyPair *kp = get_pi_keypair(it);
    if (get_input_key(kp) == Label::epsilon) {
      Transducer *tmp = comp;
      comp = freely_insert_(comp, get_input_key(kp), get_output_key(kp));
      delete tmp;
    }
  }
  return PINSTANCE_TO_HANDLE(Transducer, comp);
}


TransducerHandle add_output_language( TransducerHandle t, KeyPairSet *s ) {
  Transducer* pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer *pi=new Transducer();
  pi->root_node()->set_final(1);
  for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
    KeyPair *kp = get_pi_keypair(it);
    if (get_output_key(kp) != Label::epsilon)
      (pi->root_node())->add_arc(*kp, pi->root_node(), pi);
  }
  Transducer *comp = composition_(pi, pT);
  for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
    KeyPair *kp = get_pi_keypair(it);
    if (get_output_key(kp) == Label::epsilon) {
      Transducer *tmp = comp;
      comp = freely_insert_(comp, get_input_key(kp), get_output_key(kp));
      delete tmp;
    }
  }
  return PINSTANCE_TO_HANDLE(Transducer, comp);
}

  TransducerHandle intersect( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = conjunction_(pT1, pT2);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle subtract( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = subtraction_(pT1, pT2);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle disjunct( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Transducer* const pResult = disjunction_(pT1, pT2);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  Transducer *filter_epsilon_paths_( Transducer *t ) {

    Transducer *filter = new Transducer();
    filter->root_node()->set_final(1);
    Node *node=filter->new_node();
    node->set_final(1);

    filter->root_node()->add_arc( Label(ANYKEY,ANYKEY), filter->root_node(), filter );
    filter->root_node()->add_arc( Label(ANYKEY,Label::epsilon), filter->root_node(), filter );
    filter->root_node()->add_arc( Label(Label::epsilon,ANYKEY), node, filter );
    node->add_arc( Label(Label::epsilon,ANYKEY), node, filter);
    node->add_arc( Label(ANYKEY,ANYKEY), filter->root_node(), filter);

    Transducer *T = &(t->filter_epsilon_paths(*filter));
    delete t;
    delete filter;
    return T;
  }

  TransducerHandle filter_epsilon_paths( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = filter_epsilon_paths_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

/*TransducerHandle read_transducer(FILE *file) {
    TheAlphabet.utf8 = UTF8;
    //fprintf(stderr, "read_transducer: reading transducer from FILE 'stdin':\n");
    Transducer tr(file, true);
    Transducer *copy_of_t = &tr.copy(false, &TheAlphabet);
    TheAlphabet.insert_symbols(copy_of_t->alphabet);
    //fprintf(stderr, "... read.\n");
    return PINSTANCE_TO_HANDLE(Transducer, copy_of_t);
    }*/


TransducerHandle read_transducer_legacy( istream &in ) {

  in.peek();
  if (in.eof())
    return NULL;

  TheAlphabet.utf8 = UTF8;
  Transducer tr;
  //fprintf(stderr, "reading...\n");
  tr.read_stream( in );
  //cout << tr;
  //fprintf(stderr, "copying...\n");
  Transducer *copy_of_t = &tr.copy(false, &TheAlphabet);  // &TheAlphabet added
  //fprintf(stderr, "inserting symbols...\n");
  TheAlphabet.insert_symbols(copy_of_t->alphabet);
  //fprintf(stderr, "... all done.\n");
  return PINSTANCE_TO_HANDLE(Transducer, copy_of_t);
  }

bool has_alphabet(istream& is);

TransducerHandle read_transducer( istream &is ) 
{
  if (is.eof())
    return NULL;
  Transducer *tr = new Transducer();
  if (!has_alphabet(is)) { // read just the transducer
    is.get(); // remove 'A' from stream (indicates that there is no alphabet)
    tr->read_stream(is, false);
  }
  else { // read the transducer, skip the alphabet;
    tr->read_stream(is, true);
    tr->alphabet.clear();
  }
  return PINSTANCE_TO_HANDLE(Transducer, tr);
}


TransducerHandle read_transducer( istream &is, KeyTable *T ) 
{
  if (is.eof())
    return NULL;
  Transducer *tr = new Transducer();
  if (!has_alphabet(is)) { // read just the transducer
    is.get(); // remove 'A' from stream (indicates that there is no alphabet)
    tr->read_stream(is, false);
  }
  else { // read the transducer and the alphabet;
    //fprintf(stderr, "The transducer has alphabet\n");
    tr->read_stream(is, true);
    
    KeyTable *T_old;
    if (T)
      T_old = tr->alphabet.to_key_table( TheAlphabet ); 
    else
      T_old = TheAlphabet.to_key_table( TheAlphabet );

    tr->alphabet.clear();

    if (T->is_empty()) {
      T->merge(T_old);
    }
    else {
      tr->harmonize(T_old, T, TheAlphabet);
    }
    //delete T_old;
  }
  return PINSTANCE_TO_HANDLE(Transducer, tr);
}

/*KeyTable *read_key_table( istream &is ) {
  BinaryReader reader(is);
  Alphabet alpha;
  alpha.read(reader);
  KeyTable *T = alpha.to_key_table(TheAlphabet);
  return T;
  }*/


TransducerHandle read_transducer( const char *filename ) {
  ifstream is(filename);
  TransducerHandle result = read_transducer(is);
  is.close();
  return result;
}

/* Crate a copy of t that is harmonized according to key table T. 
   A NULL T indicates that harmonization is done according to TheAlphabet. */

Transducer *harmonize_transducer_( Transducer *t, KeyTable *T ) {
  // harmonize the alphabet (secondary names are ignored)
  Alphabet alpha;
  Transducer *copy_of_t;
  if (T)
    alpha = T->to_alphabet(TheAlphabet);
  else
    alpha.copy(TheAlphabet);
  copy_of_t = &t->copy(false, &alpha);
  
  
  // If a symbol is not used in transitions of the transducer and not in T
  // it will miss from T. This might be a problem with epsilon, so it is added.
  if ( alpha.code2symbol(Label::epsilon) == NULL &&
       copy_of_t->alphabet.code2symbol(Label::epsilon) == NULL ) {
    const char *epsname = t->alphabet.code2symbol(Label::epsilon);
    if (epsname != NULL)
      alpha.add_symbol(epsname, Label::epsilon);
  }
    
  alpha.insert_symbols(copy_of_t->alphabet);

  // add to key table and global symbol table 
  // primary symbols that are used in transducer tr
  //if (T) {
  alpha.add_primary_symbols( T, TheAlphabet );
    //}
  return copy_of_t;
}


/* Create a copy of t harmonized according to T_new. */
TransducerHandle harmonize_transducer( TransducerHandle t, KeyTable *T_old, KeyTable *T_new ) 
{
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  // T_old becomes the alphabet of t
  pT->alphabet.clear();
  Alphabet t_old_alpha = T_old->to_alphabet(TheAlphabet);
  pT->alphabet.copy(t_old_alpha);
  Transducer *pResult = harmonize_transducer_(pT, T_new);
  return PINSTANCE_TO_HANDLE(Transducer, pResult);
}


TransducerHandle read_transducer_harmonizing_it( KeyTable *T, istream &is ) {
  int c=is.peek();
  if (is.eof())
    return NULL;
  if (c == 'A') {
    fprintf(stderr, "throwing: Cannot harmonize a transducer that has no alphabet.");
    throw "Cannot harmonize a transducer that has no alphabet.";
  }
  
  // read the transducer and the alphabet
  Transducer tr = Transducer();
  //fprintf(stderr, "tr.read_stream next...\n");
  tr.read_stream(is, true);
  //fprintf(stderr, "tr.read_stream done...\n");

  Transducer *t_harmonized = harmonize_transducer_(&tr, T);

#ifdef FOO
  // harmonize the alphabet (secondary names are ignored)
  Alphabet alpha;
  if (T)
    alpha = T->to_alphabet(TheAlphabet);
  else
    alpha.copy(TheAlphabet);
  //fprintf(stderr, "\nthe alphabet of the read transducer:\n");
  //tr.alphabet.print();
  //fprintf(stderr, "making copy_of_t:\n");
  Transducer *copy_of_t = &tr.copy(false, &alpha);
  //fprintf(stderr, "\n alphabet of copy_of_t:\n");
  //copy_of_t->alphabet.print();                     

  // If a symbol is not used in transitions of the transducer and not in T
  // it will miss from T. This might be a problem with epsilon, so it is added.
  if ( alpha.code2symbol(Label::epsilon) == NULL &&
       copy_of_t->alphabet.code2symbol(Label::epsilon) == NULL ) {
    const char *epsname = tr.alphabet.code2symbol(Label::epsilon);
    if (epsname != NULL)
      alpha.add_symbol(epsname, Label::epsilon);
  }
    
  alpha.insert_symbols(copy_of_t->alphabet);
  //fprintf(stderr, "\n alpha after 'insert_symbols':\n");
  //alpha.print();

  // add to key table and global symbol table 
  // primary symbols that are used in transducer tr
  //fprintf(stderr, "adding to key table and global symbol table primary symbols that are used in transducer tr:\n"); 
  if (true) {
    //alpha:sta lisataan T:hen ja TheAlphabet:iin kaikki primaariset symbolit;
    alpha.add_primary_symbols( T, TheAlphabet );
  }
#endif

  //if (T)
  //  delete &alpha;


  return PINSTANCE_TO_HANDLE(Transducer, t_harmonized);  
}

TransducerHandle read_transducer( const char *filename, KeyTable *T ) {
  ifstream is(filename);
  TransducerHandle result = read_transducer(is, T);
  is.close();
  return result;
}


/* Use read_format before this to be sure of the
   transducer format or if end of stream is reached. */

bool has_alphabet(istream& is) {
  char c = is.peek();
  if (c == 'A')
    return false;
  return true;
};

bool has_symbol_table(istream& is) {
  return has_alphabet(is);
}

// For hfst-calculate: read transducer from file 'filename'
// and if it has an alphabet, harmonize the transducer
// according to TheAlphabet.

TransducerHandle read_transducer_and_harmonize( char *filename ) {
  TransducerHandle result;
  // here was a problem?
  ifstream is(filename);
  if ( has_alphabet(is) ) {
    result = read_transducer_harmonizing_it(NULL, is);  // KeyTable == NULL -> harmonize according to TheAlphabet
  }
  else
    result = read_transducer(is);
  is.close();
  return result;
};

// old sfst format not supported
TransducerHandle read_transducer_number(istream& is) 
{
  if (is.eof())
    return NULL;

  //Transducer t(is, false, true, true, true);  
  //Transducer *pResult = &t.copy();
  Transducer *pResult = new Transducer();
  pResult->read_transducer_text_new(is, true, NULL, TheAlphabet);
  //fprintf(stderr, "read_transducer_number: returning:\n");
  //print_operator_number(cerr, *pResult);
  return PINSTANCE_TO_HANDLE(Transducer, pResult);
}

TransducerHandle read_transducer_text(istream& is, KeyTable *T, bool sfst) {

  //if (feof(file))
  if (is.eof())
    return NULL;

  //Alphabet alpha;
  //if (T)
  //  alpha = T->to_alphabet(TheAlphabet);
  //else
  //  alpha.copy(TheAlphabet);

  //Transducer t(is, false, !sfst, true, false, T);
  Transducer *pResult = new Transducer();
  pResult->read_transducer_text_new(is, false, T, TheAlphabet);

  //if (T)
  //  delete &alpha;
  return PINSTANCE_TO_HANDLE(Transducer, pResult);
}


/*TransducerHandle read_transducer_text( char *filename, KeyTable *T, bool sfst ) {
    FILE *file;
    if ((file = fopen(filename,"rb")) == NULL) {
      fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", filename);
      exit(1);
    }
    TransducerHandle result = read_transducer_text( file, T, sfst );
    fclose(file);
    return result;
    }*/

  TransducerHandle result( TransducerHandle t , bool invert, bool reverse ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer* const pResult = result_(pT, invert, reverse);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  void write_to_file_legacy( TransducerHandle t, char *filename, bool alpha ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    write_to_file_( pT, filename, alpha );
  }

// for hfst-calculate
TransducerHandle explode_and_minimise( TransducerHandle T) {
    Transducer* t = HANDLE_TO_PINSTANCE(Transducer, T);
    t = explode_(t);
    add_alphabet(t);
    t = minimise_(t);
    return PINSTANCE_TO_HANDLE(Transducer, t);
}


/* Write transducer t to ostream os. 
   alpha defines whether alphabet is appended at the end of transducer.
   KeyTable T defines the mapping between numbers and symbols.
   If alpha is false, T can be NULL. prop defines whether a properties
   header is written with the transducer. */

void write_transducer_( Transducer *t, ostream& os, bool alpha, KeyTable *T, bool prop=true ) {

  // t = explode_(t);
  // add_alphabet(t);
  // t = minimise_(t);

  if (alpha) {
    t->alphabet.clear();
    Alphabet t_alphabet;
    if (T)
      t_alphabet = T->to_alphabet(TheAlphabet);
    else
      t_alphabet.copy(TheAlphabet);
    //fprintf(stderr, "\nto_alphabet done....\n");
    t->alphabet.copy(t_alphabet);
    //fprintf(stderr, "\nalphabet copied....\n");
    t->complete_alphabet();
    //fprintf(stderr, "\nalphabet completed....\n");
  }
  //fprintf(stderr, "\nstoring the transducer....\n");
  t->store(os, alpha, prop);
  //fprintf(stderr, "\ntransducer stored....\n");
  //delete t;
}

/* Added by Miikka Silfverberg and Erik Axelson */
void write_transducer( TransducerHandle t, ostream &os, bool backwards_compatibility ) {
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  if (backwards_compatibility) {
    KeyTable *T_empty = create_key_table();
    write_transducer_(pT, os, true, T_empty, false);
  }
  else
    write_transducer_(pT, os, false, NULL, true);
}
void write_transducer( TransducerHandle t, KeyTable *T, ostream &os, bool backwards_compatibility ) {
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  write_transducer_(pT, os, true, T, !backwards_compatibility);
}
void write_transducer( TransducerHandle t, const char *filename, bool backwards_compatibility ) {
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  ofstream os(filename);
  if (backwards_compatibility) {
    KeyTable *T_empty = create_key_table();
    write_transducer_(pT, os, true, T_empty, false);
  }
  else
    write_transducer_(pT, os, false, NULL, true);
  os.close();
}
void write_transducer( TransducerHandle t, const char *filename, KeyTable *T, bool backwards_compatibility ) {
  Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
  ofstream os(filename);
  write_transducer_(pT, os, true, T, !backwards_compatibility);
  os.close();
}



  TransducerHandle copy_transducer_variable_value( char *name ) {
    Transducer* const pResult = copy_transducer_variable_value_(name);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle copy_transducer_agreement_variable_value( char *name ) {
    Transducer* const pResult = copy_transducer_agreement_variable_value_(name);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  ContextsHandle make_context( TransducerHandle t1, TransducerHandle t2 ) {
    Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
    Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
    Contexts* const pResult = make_context_(pT1, pT2);

    ContextsHandle result = new contexts_t();
    result->left = PINSTANCE_TO_HANDLE(Transducer, pResult->left);
    result->right = PINSTANCE_TO_HANDLE(Transducer, pResult->right);
    delete pResult;  // #####
    return result;
  }

  ContextsHandle append_context( ContextsHandle p1, ContextsHandle p2 ) {
    p1->next = p2;
    return p1;
  }

  bool define_transducer_variable( char *name, TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    return define_transducer_variable_(name, pT);
  }

  bool define_transducer_agreement_variable( char *name, TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    return define_transducer_agreement_variable_(name, pT);
  }

  void def_alphabet( TransducerHandle t ) {
    Transducer* const pT = HANDLE_TO_PINSTANCE(Transducer, t);
    def_alphabet_(pT);
  }

  TransducerHandle make_mapping( Ranges *rs1, Ranges *rs2 ) {
    Transducer* const pResult = make_mapping_( rs1, rs2 );
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle new_transducer( Range *r1, Range *r2, KeyPairSet *Pi ) {
    Transducer *t = make_transducer( r1, r2, Pi );
    if (r1 != r2)
      free_values(r1);
    free_values(r2);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

//TransducerHandle new_transducer( Range *r1, Range *r2, int weight ) {
//    fprintf(stderr, "Warning: using weighted version of 'new_transducer' with SFST. ");
//    fprintf(stderr, "Weight parameter is ignored.\n");
//    return new_transducer(r1, r2);
//  }

//TransducerHandle sigma_fst() {
//  Transducer *t = make_transducer(NULL, NULL);
//  return PINSTANCE_TO_HANDLE(Transducer, t);
//

void print_transducer(TransducerHandle t, KeyTable *T, bool weights, ostream& ostr, bool old) {
  assert(t != NULL);
  if (weights == true) {
    //fprintf(stderr, "Warning: using 'print_fst' with weight==true in SFST. ");
    //fprintf(stderr, "Weight parameter is ignored.\n");
  }
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  //add_alphabet(pT);
  Alphabet alpha;
  if (T) {
    //fprintf(stderr, "hfst: calling to_alphabet...\n");
    alpha = T->to_alphabet(TheAlphabet);
    //fprintf(stderr, "HFST::print_transducer: alpha is now:\n");
    //alpha.print();
  }
  else
    alpha.copy(TheAlphabet);

  if (old) {
    Transducer *copy_of_pT = &pT->copy(false, &alpha);
    ostr << *copy_of_pT;
    delete copy_of_pT;
  }
  else
    print_operator(ostr, *pT, alpha);  // TheAlphabet
}


void print_transducer_number(TransducerHandle t, bool weights, ostream& ostr) {
  assert(t != NULL);
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  //add_alphabet(pT);
  print_operator_number(ostr, *pT);
}



void write_separator(ostream& os) {
  os << "--\n";
}

void print_fst_old(TransducerHandle t, ostream& ostr) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  add_alphabet(pT);
  ostr << *pT;
}

TransducerHandle make_fst(char *str) {
  TheAlphabet.utf8 = true;
  Transducer t(str, &TheAlphabet, false);
  Transducer *result = &t.copy();
  return PINSTANCE_TO_HANDLE(Transducer, result);
}

TransducerHandle copy(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer *copy_of_t = &(pT->copy());
  return PINSTANCE_TO_HANDLE(Transducer, copy_of_t);
}



// ******* Testing transducers *******

bool are_equivalent( TransducerHandle t1, TransducerHandle t2 ) {
  if (is_minimal(t1) && is_minimal(t2)) {
    if (!is_subset(t1, t2))
      return false;
    if (!is_subset(t2, t1))
      return false;
    return true;
  }
		  
  Transducer* const pT1 = HANDLE_TO_PINSTANCE(Transducer, t1);
  Transducer* const pT2 = HANDLE_TO_PINSTANCE(Transducer, t2);
  //Transducer *p = &(*pT2).copy(false, &(pT1->alphabet));
  return (*pT1 == *pT2);
}

// *** Added by Miikka Silfverberg *** //

  struct NodePair {
    Node * first;
    Node * second;
  };

  struct compare_NodePairs {
    bool operator() ( const NodePair &pair1, const NodePair &pair2) const {
      if ( pair1.first == pair2.first )
	return pair1.second < pair2.second;
      return pair1.first < pair2.first;
    }
  };

  struct compare_Labels {
    bool operator() ( const Label &l1, const Label &l2 ) const {
      if ( l1.upper_char() == l2.upper_char() )
	return l1.lower_char() < l2.lower_char();
      return l1.upper_char() < l2.upper_char();
    } 
  };

  bool non_empty_intersection( Node * n1, Node * n2, set<NodePair,compare_NodePairs> &handled_nodes) {

    if ( n1->is_final() and n2->is_final() )
      return true;

    map<Label,Node*,compare_Labels> n1_labels;
    for ( ArcsIter it( n1->arcs() ); it ; it++ ) {
      Arc a = *it;
      n1_labels[ a.label() ] = a.target_node();
    }

    for ( ArcsIter it ( n2->arcs() ); it; it++ ) {
      Arc a = *it;
      Label l = a.label();

      if ( n1_labels.find(l) == n1_labels.end() ) 
	continue;
      
      NodePair nodes;
      nodes.first = n1_labels[ l ];
      nodes.second = a.target_node();

      if ( handled_nodes.find( nodes ) != handled_nodes.end() ) {
	continue;
      }

      handled_nodes.insert( nodes );

      if ( non_empty_intersection( nodes.first, nodes.second, handled_nodes ) )
	return true;

    }

    return false;

  }


  bool is_empty_intersection( TransducerHandle t1, TransducerHandle t2 ) {
    
    Transducer * pT1 = HANDLE_TO_PINSTANCE( Transducer, t1); 
    Transducer * pT2 = HANDLE_TO_PINSTANCE( Transducer, t2);
    set<NodePair,compare_NodePairs> handled_nodes;

    NodePair root_nodes;
    root_nodes.first = pT1->root_node();
    root_nodes.second = pT2->root_node();

    handled_nodes.insert( root_nodes );

    return not non_empty_intersection( pT1->root_node(), pT2->root_node(), handled_nodes );
  }


  bool empty_difference( Node * n1, Node * n2, set<NodePair,compare_NodePairs> &handled_nodes) {

    set<NodePair,compare_NodePairs> next_node_pairs;

    for ( ArcsIter as( n1->arcs() ); as; as++ ) {

      Arc a = *as;
      Label l = a.label();
      
      Node * target2 = n2->target_node(l);

      if ( target2 == NULL ) {
	return false;
      } 
      
      Node * target1 = a.target_node();
      
      if ( target1->is_final() and ( not target2->is_final() ) ) {
	return false;
      }

      NodePair p;
      p.first = target1;
      p.second = target2;
      
      next_node_pairs.insert(p);
    }

    for ( set<NodePair,compare_NodePairs>::iterator it = next_node_pairs.begin();
	  it != next_node_pairs.end();
	  ++it ) {
      if ( handled_nodes.find( *it ) == handled_nodes.end() ) {
	handled_nodes.insert(*it);
	if ( not empty_difference( (*it).first, (*it).second, handled_nodes) ) {
	  return false;
	}
      } 
    }
    return true;

  }

  bool is_subset( TransducerHandle t1, TransducerHandle t2 ) {

    Transducer * pT1 = HANDLE_TO_PINSTANCE( Transducer, t1); 
    Transducer * pT2 = HANDLE_TO_PINSTANCE( Transducer, t2);
    set<NodePair,compare_NodePairs> handled_nodes;

    NodePair root_nodes;
    root_nodes.first = pT1->root_node();
    root_nodes.second = pT2->root_node();

    handled_nodes.insert( root_nodes );

    return empty_difference( pT1->root_node(), pT2->root_node(), handled_nodes);

  }


bool are_disjoint( TransducerHandle t1, TransducerHandle t2 ) {
  return is_empty_intersection( t1, t2 );
}

bool is_automaton(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return pT->is_automaton();
}

bool is_cyclic(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return pT->is_cyclic();
}

bool is_infinitely_ambiguous(TransducerHandle t, bool output, KeyVector * str)
{
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer,t);
  if (str == NULL)
    {
      return pT->is_infinitely_ambiguous(output);
    }
  else
    {
      return has_infinitely_many_analyses(pT,output,str);
    }
}

bool is_deterministic(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return pT->is_deterministic();
}

bool is_empty(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return pT->is_empty();
}

bool is_epsilon(TransducerHandle t) {
  TransducerHandle eps = create_epsilon_transducer();
  bool result = are_equivalent(t, eps);
  delete eps;
  return result;
}

bool is_minimal(TransducerHandle t) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return pT->is_minimised();
}


TransducerHandle add_weight( TransducerHandle t, float w ) {
  return t;
}

float get_weight( TransducerHandle t ) {
  return (float)0;
}


/*  const char * path_to_string( Transducer *t, KeyTable *T ) {
    Alphabet alpha = T->to_alphabet(TheAlphabet);
    string path = std::string("");

    Arcs *arcs = t->root_node()->arcs();
    while (arcs) {
      for( ArcsIter p(arcs); p; p++ ) {
	Arc *arc=p;
	const char *isymbol = alpha.code2symbol((arc->label()).lower_char());
	const char *osymbol = alpha.code2symbol((arc->label()).upper_char());
	for (int i=0; isymbol[i] != '\0'; i++) {
	  if (isymbol[i] == ':' || isymbol[i] == '\\')
	    path.append(1, '\\');
	  path.append(1, isymbol[i]);
	}
	if (strcmp(isymbol, osymbol) != 0) {
	  path.append(1, ':');
	  for (int i=0; osymbol[i] != '\0'; i++) {
	    if (osymbol[i] == ':' || osymbol[i] == '\\')
	      path.append(1, '\\');
	    path.append(1, osymbol[i]);
	  }
	}
      arcs = arc->target_node()->arcs();
      }
    }
    return path.c_str();
    }*/


char * transducer_to_pairstring( TransducerHandle t, KeyTable *T, bool spaces, bool epsilons) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  //Alphabet alpha = T->to_alphabet(TheAlphabet);
  //char *res = to_string(*pT, alpha, spaces);
  //return res;
  return fst_to_pairstring(pT,T,TheAlphabet,spaces,epsilons);
}

char * key_vector_to_pairstring( KeyVector * kv, KeyTable * kt ) {
  return vector_to_pairstring(kv,kt,TheAlphabet);
}

char * key_pair_vector_to_pairstring( KeyPairVector * kv, KeyTable * kt ) {
  return vector_to_pairstring(kv,kt,TheAlphabet);
}

TransducerHandle pairstring_to_transducer( const char *str, KeyTable *T )
{
  Alphabet alpha = T->to_alphabet(TheAlphabet);
  Transducer *pT = to_transducer(str, alpha);
  return PINSTANCE_TO_HANDLE(Transducer, pT);
}
 

/*void path_to_string( std::string& path, Node *node, NodeNumbering &index, 
		     long vmark, Alphabet &alpha ) {
  if (!node->was_visited( vmark )) {
    Arcs *arcs=node->arcs();
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      char *isymbol = strdup( alpha.code2symbol((arc->label).lower_char()) );
      char *osymbol = strdup( alpha.code2symbol((arc->label).upper_char()) );
      if (strcmp(isymbol, ":") == 0 ||
	  strcmp(isymbol, "\\") == 0)
	path.append("\\");
      path.append(isymbol);
      if (strcmp(isymbol, osymbol) != 0) {
	path.append(":");
	if (strcmp(osymbol, ":") == 0 ||
	    strcmp(osymbol, "\\") == 0)
	  path.append("\\");
	path.append(osymbol);
      }
    }
    for( ArcsIter p(arcs); p; p++ ) {
      Arc *arc=p;
      path_to_string( path, arc->target_node(), index, vmark, alpha );
    }
  }
}


char * path_to_string( Transducer *t ) {
  Alphabet alpha = T->to_alphabet(TheAlphabet);
  string path = std::string("");
  NodeNumbering index(*t);
  t->incr_vmark();
  path_to_string( path, t->root_node(), index, t->vmark, alpha );
  return path;
}


 
char * path_to_string( TransducerHandle t, KeyTable *T ) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  return path_to_string(pT, T);
  }*/



void print_transducer_paths(TransducerHandle t, FILE *outfile) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  pT->print_strings(outfile, true);
}

TransducerHandle insert_pair_string(TransducerHandle t, char *str, float weight) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  pT->add_string(str, true);
  return PINSTANCE_TO_HANDLE(Transducer, pT);
}


/*******************************************************************/
/*                                                                 */
/*     File: utf8.C                                                */
/*   Author: Helmut Schmid                                         */
/*  Purpose:                                                       */
/*  Created: Mon Sep  5 17:49:16 2005                              */
/* Modified: Thu May 11 17:15:36 2006 (schmid)                     */
/*                                                                 */
/*******************************************************************/

// #include "utf8.h"


const unsigned char get3LSbits=7;
const unsigned char get4LSbits=15;
const unsigned char get5LSbits=31;
const unsigned char get6LSbits=63;

const unsigned char set1MSbits=128;
const unsigned char set2MSbits=192;
const unsigned char set3MSbits=224;
const unsigned char set4MSbits=240;



char *int_to_utf8( unsigned int c ) {
  return int2utf8( c );
}

/*******************************************************************/
/*                                                                 */
/*  int2utf8                                                       */
/*                                                                 */
/*******************************************************************/

char *int2utf8( unsigned int sym )

{
  static unsigned char ch[5];

  if (sym < 128) {
    // 1-byte UTF8 symbol, 7 bits
    ch[0] = sym;
    ch[1] = 0;
  }
  
  else if (sym < 2048) {
    // 2-byte UTF8 symbol, 5+6 bits
    ch[0] = (sym >> 6) | set2MSbits;
    ch[1] = (sym & get6LSbits) | set1MSbits;
    ch[2] = 0;
  }
  
  else if (sym < 65536) {
    // 3-byte UTF8 symbol, 4+6+6 bits
    ch[0] = (sym >> 12) | set3MSbits;
    ch[1] = ((sym >> 6) & get6LSbits) | set1MSbits;
    ch[2] = (sym & get6LSbits) | set1MSbits;
    ch[3] = 0;
  }
  
  else if (sym < 2097152) {
    // 4-byte UTF8 symbol, 3+6+6+6 bits
    ch[0] = (sym >> 18) | set4MSbits;
    ch[1] = ((sym >> 12) & get6LSbits) | set1MSbits;
    ch[2] = ((sym >> 6) & get6LSbits) | set1MSbits;
    ch[3] = (sym & get6LSbits) | set1MSbits;
    ch[4] = 0;
  }
  
  else
    return NULL;

  return (char*)ch;
}

/*******************************************************************/
/*                                                                 */
/*  utf8toint                                                      */
/*                                                                 */
/*******************************************************************/

unsigned int utf8toint( char **s )

{
  int bytes_to_come;
  unsigned int result=0;
  unsigned char c=(unsigned char)**s;

  if (c >= (unsigned char)set4MSbits) { // 1111xxxx
    bytes_to_come = 3;
    result = (result << 3) | (c & get3LSbits);
  }
      
  else if (c >= (unsigned char) set3MSbits) { // 1110xxxx
    // start of a three-byte symbol
    bytes_to_come = 2;
    result = (result << 4) | (c & get4LSbits);
  }
      
  else if (c >= (unsigned char) set2MSbits) { // 1100xxxx
    // start of a two-byte symbol
    bytes_to_come = 1;
    result = (result << 5) | (c & get5LSbits);
  }
      
  else if (c < (unsigned char) set1MSbits) { // 0100xxxx
    // one-byte symbol
    bytes_to_come = 0;
    result = c;
  }

  else
    return 0; // error

  while (bytes_to_come > 0) {
    bytes_to_come--;
    (*s)++;
    c = (unsigned char)**s;
    if (c < (unsigned char) set2MSbits &&
	c >= (unsigned char) set1MSbits)    // 1000xxxx
      {
	result = (result << 6) | (c & get6LSbits);
      }
    else
      return 0;
  }

  (*s)++;
  return result;
}


/*******************************************************************/
/*                                                                 */
/*  utf8toint                                                      */
/*                                                                 */
/*******************************************************************/

unsigned int utf8toint( char *s )

{
  unsigned int result = utf8toint( &s );
  if (*s == 0) // all bytes converted?
    return result;
  return 0;
}

/*******************************************************************/
/*                                                                 */
/*     File: utf8.h                                                */
/*   Author: Helmut Schmid                                         */
/*  Purpose:                                                       */
/*  Created: Mon Sep  5 17:49:16 2005                              */
/* Modified: Thu Apr 27 10:13:09 2006 (schmid)                     */
/*                                                                 */
/*******************************************************************/





Node * best_forms1( Node * n , NodeNumbering &node_numbers, int visited[], int &number_of_forms, Transducer &new_t) {
  
  if ( number_of_forms == 0 )
    return NULL;

  assert( n );
  Arcs * arcs = n->arcs();
  
  if ( arcs->is_empty() ) {
    
    /* Final state with no continuing transitions. */
    if ( n->is_final() ) {
      Node * final = new_t.new_node();
      final->set_final(1);
      --number_of_forms;
      return final;
    }

    /*
      Non-final state with no continuing transitions. This branch
      of the generation process dies.
    */
    visited[ node_numbers[ n ] ] = 1;
    return NULL;
  }
  
  /* Don't visit this state again during this branch of the generation process. */
  visited[ node_numbers[ n ] ] = 1;

  Node * next = new_t.new_node();
  if ( n->is_final() ) { --number_of_forms; next->set_final(1); }

  for (ArcsIter it(arcs); it; it++) {

    if ( number_of_forms == 0 )
      return next;

    Arc arc = *it;
    Node * target_node = arc.target_node();
    Label l = arc.label();

    /* Don't visit states, that have been visited during this branch 
       of the generation process. If the node is a final node, this is a path. */
    if ( visited [ node_numbers[ target_node ] ] == 1 ) {
      if ( target_node->is_final() ) {
	Node * final = new_t.new_node();
	final->set_final(1);
	next->add_arc( l, final, &new_t );
	--number_of_forms;
      }
      continue;
    }
    
    /* Get the continuation for the surface forms beginning with label l */
    Node * new_branch = best_forms1( target_node, node_numbers, visited, number_of_forms, new_t);

    /* Empty language. */
    if ( new_branch == NULL ) { continue; }

    next->add_arc( l, new_branch, &new_t ); 
    
  }

  visited[ node_numbers[ n ] ] = 0;

  if ( number_of_forms == 0 )
    return next;

  if ( next->arcs()->is_empty() ) 
    next = NULL;

  return next;

}; 

/*
  \fn vector<vector<Label>*> * get_surface_forms( Transducer * t )
  \brief Return a vector containing at most twenty surface-forms generated by the transducer \e t.
  
  \param t a transducer.
  \return \e forms a vector containing twenty surface forms generated by \e t.

  \pre \e t is set.

*/

TransducerHandle find_best_paths( TransducerHandle t, int number_of_forms, bool unique) {

  if (unique)
    t = minimize(copy(t));
  
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);

  Transducer * new_t = new Transducer();
  new_t->alphabet.copy(TheAlphabet);

  if ( number_of_forms == 0 )
    return PINSTANCE_TO_HANDLE(Transducer, new_t);

  NodeNumbering num(*pT);
  int visited[ num.number_of_nodes() ];

  for ( unsigned int i = 0; i < num.number_of_nodes(); ++i )
    visited[i] = 0;

  Node * pT_root = pT->root_node();

  visited[ num[ pT_root ] ] = 1;

  Arcs * arcs = pT_root->arcs();

  Node * new_t_root = new_t->root_node();
  
  for ( ArcsIter it( arcs ); it ; it++ ) {
    if ( number_of_forms  == 0 )
      return PINSTANCE_TO_HANDLE(Transducer, new_t);

    Label l = (*it).label();
    Node * n = (*it).target_node();

    Node * next = best_forms1( n, num, visited, number_of_forms, *new_t );
    new_t_root->add_arc(l, next, new_t);
  }

  //print_operator(cerr, *new_t, new_t->alphabet);
  return PINSTANCE_TO_HANDLE(Transducer, new_t);
}

void add_continuation(vector<Label>::reverse_iterator it, vector<Label>::reverse_iterator end, Transducer *t, Node *n) {

  while ( it != end ) {
    Node * nn = t->new_node();
    n->add_arc(*it,nn,t);
    n = nn;
    ++it;
  }
  n->set_final(1);
}

vector<vector<Label>*> * get_forms1( Node * n , NodeNumbering &node_numbers, int visited[] );

void add_str_transducer(TransducerHandle trie, TransducerHandle path) {

  Transducer * trieP = HANDLE_TO_PINSTANCE(Transducer, trie);
  Transducer * pathP = HANDLE_TO_PINSTANCE(Transducer, path);

  NodeNumbering num(*pathP);
  int v[num.number_of_nodes()];
  for ( unsigned int i = 0; i < num.number_of_nodes(); ++i ) {
    v[i] = 0;
  }

  vector<vector<Label>*> * p = get_forms1(pathP->root_node(),num,v);
  delete_transducer(path);

  Node * trie_node = trieP->root_node();

  for( vector<Label>::reverse_iterator it = p->at(0)->rbegin(); it != p->at(0)->rend(); ++it ) {

    Label l = *it;
    Node * target = trie_node->target_node(l);
    if ( target == NULL ) {
      add_continuation(it,p->at(0)->rend(),trieP,trie_node);
      break;
    }
    trie_node = target;

  }

  PINSTANCE_TO_HANDLE(Transducer,trieP);
}

TransducerHandle rand_path(TransducerHandle t) {

  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  NodeNumbering num(*pT);
  int visited[ num.number_of_nodes() ];
  int broken[ num.number_of_nodes() ];

  for ( unsigned int i = 0; i < num.number_of_nodes(); ++i ) {
    visited[i] = 0;
    broken[i] = 0;
  }

  Transducer * path = new Transducer();
  path->alphabet.copy(TheAlphabet);
  
  Node * current_t_node = pT->root_node();
  Node * current_path_node = path->root_node();

  Node * last_penultimate_node = path->root_node();
  Label last_label(0);

  while (1) {
    
    visited[ num[current_t_node] ] = 1;
    vector<Arc> t_transitions;
    for ( ArcsIter it( current_t_node->arcs() ); it; it++) 
      t_transitions.push_back(*it);
    
    bool next_transition_exists = false;

    while ( not t_transitions.empty() ) {
      unsigned int index = rand() % t_transitions.size();
      Arc arc = t_transitions.at(index);
      t_transitions.erase(t_transitions.begin()+index);

      Node * t_target = arc.target_node();
      
      if ( t_target->is_final() ) {
	if ( (rand() % 4) == 0 ) {
	  Node * final = path->new_node();
	  final->set_final(1);
	  current_path_node->add_arc(arc.label(),final,path);
	  return PINSTANCE_TO_HANDLE(Transducer,path);
	}
	else {
	  last_penultimate_node = current_path_node;
	  last_label = arc.label();
	  //continue;
	}
      }
      

      if ( broken[ num[ t_target ] ] == 0 ) {

	if ( visited[ num[ t_target ] ] == 1 ) 
	  if ( (rand() % 4) == 0 )
	    broken[ num[ t_target ] ] = 1;
	
	Node * next = path->new_node();
	current_path_node->add_arc(arc.label(),next,path);
	current_path_node = next;
	current_t_node = t_target;
	next_transition_exists = true;
	break;
      }

      if ( visited[ num[ t_target ] ] == 1 ) 
	if ( (rand() % 4) == 0 )
	  broken[ num[ t_target ] ] = 1;
      
    }

    if ( not next_transition_exists ) {
      Node * final = path->new_node();
      final->set_final(1);
      last_penultimate_node->add_arc(last_label,final,path);
      return PINSTANCE_TO_HANDLE(Transducer,path);
    } 

  }
  
  return PINSTANCE_TO_HANDLE(Transducer,path);

};


TransducerHandle find_random_paths(TransducerHandle t, int number_of_forms, bool unique) {

  if (unique)
    t = minimize(copy(t));
  
  srand((unsigned int)(time(0)));
  
  TransducerHandle res = create_empty_transducer();
  
  while (number_of_forms > 0) {
    TransducerHandle path = rand_path(t);
    
    unsigned int i = 0;

    while ( (not is_empty_intersection(path,res)) and (i < 5) ) {
      delete_transducer(path);
      path = rand_path(t);
      ++i;
    }
    
    add_str_transducer(res,path);

    --number_of_forms;
    
  }
  return res;  
}

vector<vector<Label>*> * get_forms1( Node * n , NodeNumbering &node_numbers, int visited[] ) {
  
  assert( n );
  Arcs * arcs = n->arcs();
  
  if ( arcs->is_empty() ) {
    
    /* Final state with no continuing transitions. */
    if ( n->is_final() ) {
      vector<vector<Label>*> * v = new vector<vector<Label>*>;
      v->push_back( new vector<Label> );
      return v;
    }

    /*
      Non-final state with no continuing transitions. This branch
      of the generation process dies.
    */
    visited[ node_numbers[ n ] ] = 1;
    return NULL;
  }
  
  /* Don't visit this state again during this branch of the generation process. */
  visited[ node_numbers[ n ] ] = 1;

  vector<vector<Label>*> * paths = new vector<vector<Label>*>;
  vector<vector<Label>*> * continuations = NULL;

  for (ArcsIter it(arcs); it; it++) {

    Arc arc = *it;
    Node * target_node = arc.target_node();
    Label l = arc.label();

    /* Don't visit states, that have been visited during this branch 
       of the generation process. */
    if ( visited [ node_numbers[ target_node ] ] == 1 ) { continue; }
    
    /* Get the continuation for the surface forms that beginning l.upper_char() */
    continuations = get_forms1( arc.target_node(), node_numbers, visited);

    /* Empty language. */
    if ( not continuations ) { continue; }
    
    /* Build the surface-form (backwards). Return maximally 20 surface forms. */
    for( vector<vector<Label>*>::iterator cont = continuations->begin(); cont != continuations->end(); ++cont ){
      
      (*cont)->push_back( l );
      paths->push_back( *cont );
      
    }
    
    delete continuations;

    
  }
  
  /* If this is a final state, every surface-string ending here is a valid surface form. */
  if ( n->is_final() ) { paths->push_back( new vector<Label> ); }

  /* We're returning the empty language. */
  if ( paths->size() == 0 ) { delete paths; paths = NULL; }

  /* This branch is now dead, so it's okay to visit it again at some point. */
  visited[ node_numbers[ n ] ] = 0;

  return paths;

}

/*
  \fn vector<vector<Character>*> * get_forms( Transducer * t )
  \brief Return a vector containing at most twenty surface-forms generated by the transducer \e t.
  
  \param t a transducer.
  \return \e forms a vector containing twenty surface forms generated by \e t.

  \pre \e t is set.

*/
vector<char*> * get_forms( Transducer * t ) {

  assert( t );

  NodeNumbering node_numbers = NodeNumbering(*t);

  int visited[node_numbers.number_of_nodes()];

  for ( unsigned int i = 0; i < node_numbers.number_of_nodes(); i++ )
    visited[i] = 0;

  vector<vector<Label>*> * forms = get_forms1( t->root_node(), node_numbers, visited );

  if ( not forms ) { forms = new vector<vector<Label>*>; }

  vector<char*> *text_forms = new vector<char*>;

  for ( vector<vector<Label>*>::iterator it = forms->begin(); it != forms->end(); ++it ) {

    char * text_form = (char*)(malloc(sizeof(char[9*(*it)->size()])));
    *text_form = '\0';

    for( vector<Label>::reverse_iterator jt = (*it)->rbegin(); jt != (*it)->rend(); ++jt ) {
      if ( (*jt).lower_char() != (*jt).upper_char() ){
	strcat( text_form, TheAlphabet.code2symbol((*jt).lower_char()) );
	strcat( text_form, ":");
	strcat( text_form, TheAlphabet.code2symbol((*jt).upper_char()) );
      }
      else {
	strcat( text_form, TheAlphabet.code2symbol((*jt).lower_char()));
      }
    }

    delete (*it);
    text_forms->push_back(text_form);

  }

  delete forms;
  return text_forms;
}

vector<char*> string_paths(TransducerHandle t, bool spaces) {

  if ( is_cyclic(t) ) {
    throw "ERROR: The transducer is cyclic\n\n";
  }  
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);  
  return *get_forms(pT);
}

vector <TransducerHandle> find_all_paths( TransducerHandle t, bool unique ) {
  if ( is_cyclic(t) ) {
    throw "ERROR: The transducer is cyclic\n\n";
  }  
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  if (unique)
    pT = &pT->determinise();
  vector<Transducer*> paths;
  pT->enumerate_paths(paths);
  if (unique)
    delete pT;
  vector<TransducerHandle> *result = new vector<TransducerHandle>();
  for (unsigned int i=0; i<paths.size(); i++)
    result->push_back( PINSTANCE_TO_HANDLE(Transducer, paths[i]) );
  return *result;
} 

KeySet * get_flag_diacritics(KeyTable * kt)
{
  if ( kt == NULL)
    {
      return NULL;
    }
  KeySet * flag_diacritics = new KeySet;
  for (Key k = 0;
       k < get_unused_key(kt);
       ++k)
    {
      Symbol s = get_key_symbol(k, kt);
      const char * symbol_name = get_symbol_name(s);
      if (parse_flag_diacritic(symbol_name))
	{
	  flag_diacritics->insert(k);
	}
    }
  return flag_diacritics;
}

TransducerHandle intersecting_composition(TransducerHandle lexicon,  vector<TransducerHandle> * rules, KeyTable * kt) {
  KeySet * flag_diacritics = get_flag_diacritics(kt);
  Transducer *pLexicon = HANDLE_TO_PINSTANCE(Transducer, lexicon);
  vector<Transducer*> pRules;
  for (vector<TransducerHandle>::const_iterator it = rules->begin(); it != rules->end(); ++it ) {
    Transducer * pRule = HANDLE_TO_PINSTANCE(Transducer, *it);
    pRules.push_back(pRule);
  }
  Transducer * res = intersecting_composition( *pLexicon, &pRules, flag_diacritics, false, NULL, false);
  delete flag_diacritics;
  return PINSTANCE_TO_HANDLE(Transducer, res);

};

void derivations(const vector<Character> * analysis, const vector<vector<Character>*> * surface_forms, const vector<TransducerHandle> * Rules, 
		 vector<char*> * rule_names, const Alphabet &sigma) {
  vector<Transducer*> pRules;
  for(vector<TransducerHandle>::const_iterator it = Rules->begin(); it != Rules->end(); ++it )
    pRules.push_back(HANDLE_TO_PINSTANCE(Transducer, *it));

  derivations(analysis,surface_forms, &pRules, rule_names, sigma);

}

// #include "../condition-functions.h"

//char *att2sfst(char *str, size_t s) {
//  return ::att2sfst(str, s);
//}

// MOVE TO COMMMON NAMESPACE
int read_format(istream& is) {
  char c = is.peek();
  if (is.eof())
    return -1;

  int format=0;
  bool char_extracted=false;
  char A=0;
  if (c == 'A') {
    A = is.get();
    char_extracted=true;
    c = is.peek();
  }

  switch (c) {
    case 'a': { format=0; break; }
    case 'P': { format=0; break; } // properties header
    case 'c': { format=2; break; }
    case -42: { format=1; break; }
    default: { format=-2; break; }
  }
  
  if (char_extracted)
    is.putback(A);
  return format;
}



TransducerHandle substitute_key( TransducerHandle t, Key k1, Key k2, bool ignore_epsilon_pairs ) {
  Transducer * pt = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer * res = &(pt->replace_char(k1,k2,ignore_epsilon_pairs)); // ignore epsilon pairs
  delete pt;
  return PINSTANCE_TO_HANDLE(Transducer, res);
};

TransducerHandle substitute_key( TransducerHandle t, KeySet *ks, Key k2 ) {
  Transducer * pt = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer * res = &(pt->replace_char(ks,k2));
  delete pt;
  return PINSTANCE_TO_HANDLE(Transducer, res);
};

TransducerHandle substitute_with_transducer( TransducerHandle t, KeyPair *s, TransducerHandle tr ) {
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer * pTR = HANDLE_TO_PINSTANCE(Transducer, tr);
  Transducer * res = &(pT->splice(*s,pTR));
  delete pT;
  return PINSTANCE_TO_HANDLE(Transducer, res);
};

TransducerHandle substitute_with_pair( TransducerHandle t, KeyPair *s1, KeyPair *s2 ) {
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer * pTR = one_label_transducer(*s2);
  Transducer * res = &(pT->splice(*s1,pTR));
  delete pT;
  delete pTR;
  return PINSTANCE_TO_HANDLE(Transducer, res);
};

Key find_unused_key(set<Key> &keys) {
  for( unsigned int i=1; i!=0; i++ ) {
    if( keys.find(i) == keys.end() ) {
      return i;
    }
  }
  throw "ERROR: remove_pair: new marker key could not be created.\n";
}

TransducerHandle remove_pair( TransducerHandle t, KeyPair *p ) {
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  set<Key> keys = pT->define_key_set();
  keys.insert(get_input_key(p));
  keys.insert(get_output_key(p));

  Key marker = find_unused_key(keys);

  KeyPair *eps = define_keypair(Label::epsilon, Label::epsilon);
  KeyPair *mark = define_keypair(marker, marker);

  t = substitute_with_pair(t, eps, mark);
  t = substitute_with_pair(t, p, eps);
  t = remove_epsilons(t);
  t = substitute_with_pair(t, mark, eps);

  delete eps;  
  delete mark;
  return t;
};
  
KeySet *define_key_set( TransducerHandle t ) {
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  set<Key> *keys = pT->define_key_set_pointer();
  return keys;
}


TransducerHandle remove_pairs( TransducerHandle t, KeySet *ss ) {
  Transducer * pT = HANDLE_TO_PINSTANCE(Transducer, t);
  set<Key> keys = pT->define_key_set();
  for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ )
    keys.insert(get_sigma_key(it));
  Key marker = find_unused_key(keys);

  KeyPair *eps = define_keypair(Label::epsilon, Label::epsilon);
  KeyPair *mark = define_keypair(marker, marker);

  t = substitute_with_pair(t, eps, mark);

  for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
    KeyPair *ss_pair = define_keypair(get_sigma_key(it), get_sigma_key(it));
    t = substitute_with_pair(t, ss_pair, eps);
    delete ss_pair;
  }

  t = remove_epsilons(t);
  t = substitute_with_pair(t, mark, eps);
  
  delete eps;  
  delete mark;
  return t;

};

TransducerHandle create_remove_epsilons_test_transducer() {
  Transducer *t = new Transducer;
  t->root_node()->set_final(1);
  Node *n = t->new_node();
  t->root_node()->add_arc(Label(0,1), n, t);
  t->alphabet.add_symbol("<>",0);
  t->alphabet.add_symbol("a",1);
  return PINSTANCE_TO_HANDLE(Transducer, t);
};

TransducerHandle remove_epsilons( TransducerHandle t ) {
  Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
  Transducer *pResult = &(pT->remove_epsilons());
  delete pT;
  return PINSTANCE_TO_HANDLE(Transducer, pResult);
};

}

#include "make-compact.h"

namespace HFST {
  /* Store transducer t in compact format to FILE file. t is deleted. */
  void store_as_compact(TransducerHandle t, FILE *file) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    MakeCompactTransducer ca(*pT);
    delete pT;
    ca.store(file);
  }
  
}

namespace HFST {

  CompactTransducer *CT;

  void read_compact_transducer_for_look_up(FILE *file) {
    CT = new CompactTransducer(file);
  }

  void look_up(char *str, FILE *outfile, bool print_hits_only) {
    CT->both_layers = false;
    CT->simplest_only = false;
    vector<CAnalysis> analyses;
    CT->analyze_string(str, analyses);
    for (unsigned int i=0; i<analyses.size(); i++) {
      fputs(CT->print_analysis(analyses[i]), outfile);
      fprintf(outfile, "\n");
    }
    if (!print_hits_only && analyses.size() == 0)
      fprintf(outfile, "no result for %s\n", str);
  }

  // #include "alphabet-functions-implemented.h"

}

// ÃÂÃÂ¶

namespace HFST {    
  

  // *** Defining states ***
  
  State create_state( TransducerHandle t ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    State s;
    s.node = pT->new_node();
    return s;
  }

  State set_final_state( State s, TransducerHandle t, float w ) {
    s.node->set_final(1);
    return s;
  }


  // *** Defining transitions ***

  void define_transition( TransducerHandle t, State orig, KeyPair *p, State target, float weight ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    orig.node->add_arc( *p, target.node, pT ); 
  }


  // *** Using states ***

  State get_initial_state( TransducerHandle t ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    State s;
    s.node = pT->root_node();
    return s;
  }

  bool is_final_state( State s, TransducerHandle t ) {
    return s.node->is_final();
  }

  float get_final_weight( State s, TransducerHandle t ) {
    return (float)0;
  }


  // *** Using transitions ***
  
  KeyPair *get_transition_keypair( Transition tr ) {
    return define_keypair( tr.label().lower_char(),
			   tr.label().upper_char() );
  }

  State get_transition_to( Transition tr ) {
    State s;
    s.node = tr.target_node();
    return s;
  }

  float get_transition_weight( Transition tr ) {
    return (float)0;
  }


  // *** Iterators over transitions ***

  TransitionIterator begin_ti( TransducerHandle t, State s ) {
    ArcsIter it(s.node->arcs());
    return it;
  }

  void next_ti( TransitionIterator &ti ) {
    ti++;
  }

  bool is_end_ti( TransitionIterator ti ) {
    return (ti == NULL);
  }

  Transition get_ti_transition( TransitionIterator ti ) {
    return *ti;
  }

  //size_t size_ti(TransitionIterator ti) {
  //  return 
  //}


  /* \brief Set the state s as marked. */
  State set_marked_state( State s, TransducerHandle t ) {
    VN.insert(s.node);
    return s;
  }
  
  /* \brief Unset the state s as marked. */ 	 
  State unset_marked_state( State s, TransducerHandle t ) {
    VN.erase(s.node);
    return s;
  }
  
  /* \brief Whether state s has been marked. */ 	
  bool is_marked( State s, TransducerHandle t ) {
    VisitedNodes::const_iterator it = VN.find(s.node);
    if (it == VN.end())
      return false;
    return true;
  }



  // *** Transducer properties ***

  void print_properties( TransducerHandle t, ostream &os ) {
    Transducer *pT = HANDLE_TO_PINSTANCE(Transducer, t);
    Transducer *pT_copy = &(pT->copy());
    properties props = pT_copy->calculate_properties();
    delete pT_copy;
    props.print(os);
  }

}


