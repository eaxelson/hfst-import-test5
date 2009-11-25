#include "hfst.h"
#include "hfst_defs_private.h"
#include "./fst/lib/fstlib.h"
#include "fst_intersecting_compose.h"
#include "is-empty-intersection.h"
#include "hwfst-optimized-lookup-transducer.h"
#include "hwfst-read-runtime-transducer.h"
#include "hwfst-lookup.h"
#include "hwfst-tokenize-functions.h"

#include "escape.h"
#include "harmonize.h"
#include "shuffle.h"

#include <string>
#include <iostream>
using namespace std;

#include <fstream>
using std::ifstream;
using std::ofstream;
#include <set>
using std::set;

//include <ext/hash_set>
#include <ext/hash_map>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
//using __gnu_cxx::hash_set;

#include <vector>
using std::vector;

#include <stack>
using std::stack;

#include <utility>
using std::pair;


extern Alphabet TheAlphabet;


/* for make_fst_from_xerox_regexp

#include "xre_wtokenise.h"
#include "xre_wparse.hh"

extern int xrewnerrs;
extern int xrewparse();
extern HWFST::TransducerHandle xrew_transducer;
extern char *xre_data;
extern size_t xre_len;

void xrewerror(char *text)
{
	printf("DEBUG XREW PARSE ERROR %s\n", text);
	xrew_transducer = 0;
	}*/


/** \brief Namespace for weighted functions.

... */

namespace HWFST {

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
    fst::StdVectorFst *left, *right;
    struct contexts_t_ *next;
  } Contexts;

  struct ltstr {
    bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) < 0; }
  };
  
  struct eqstr {
    bool operator()(const char* s1, const char* s2) const
    { return strcmp(s1, s2) == 0; }
  };
  
  typedef set<char*, ltstr> RVarSet;
  typedef hash_map<char*, fst::StdVectorFst*, hash<const char*>, eqstr> VarMap;
  typedef hash_map<char*, Range*, hash<const char*>, eqstr> SVarMap;

  typedef fst::StdArc::StateId StateId;    
  


#ifdef FOO
  // for core extension layer functions
  struct eqtr {
    bool operator()(const fst::StdVectorFst *t1, const fst::StdVectorFst *t2) const
    { return (t1 == t2); }
  };
  struct hashtr {
    size_t operator()(const fst::StdVectorFst *t) const
    { return (size_t)t; }
  };
  struct lessvs {
    bool operator()(const StateId s1, const StateId s2) const
    { return s1 < s2; }
  };

  typedef map<StateId, bool> VisitedStates;
  typedef hash_map<fst::StdVectorFst*, VisitedStates*, hashtr, eqtr> VisitedMap; 
  static VisitedMap VTS; // visited transducer states
#endif

  struct eqtr {
    bool operator()(const fst::StdVectorFst *t1, const fst::StdVectorFst *t2) const
    { return (t1 == t2); }
  };
  struct hashtr {
    size_t operator()(const fst::StdVectorFst *t) const
    { return (size_t)t; }
  };

  typedef set<StateId> VisitedStates;
  typedef hash_map<fst::StdVectorFst*, VisitedStates*, hashtr, eqtr> VisitedMap; 
  static VisitedMap VTS; // visited transducer states



  static VarMap VM;    // Transducer_*
  static SVarMap SVM;  // Range*
  static RVarSet RS;   // char*
  static RVarSet RSS;  // char*

  static int Alphabet_Defined=0;
  void set_alphabet_defined( int i ) { Alphabet_Defined=i; };

  // Alphabet TheAlphabet
  bool UTF8 = true;
  bool Verbose=true;

  // API functions that do not need type conversion

  Character symbol_code( char *s );
  Range *add_value( Symbol c, Range *r );
  Range *add_values( unsigned int c1, unsigned int c2, Range *r );
  Range *add_var_values( char *name, Range *r );
  Range *complement_range( Range *r, KeySet *Pi );
  Range *complement_range( Range *r );
  Range *copy_range_agreement_variable_value( char *name );
  Range *copy_range_variable_value( char *name );
  Ranges *add_range( Range *r, Ranges *rs );
  bool define_range_variable( char *name, Range *r );
  //unsigned int utf8toint( char *s );
  unsigned int utf8_to_int( char *s );
  void read_symbol_table(char *filename);



  // AUXILIARY FUNCTIONS THAT ARE NOT VISIBLE IN API

  // these operations create a new transducer
  fst::StdVectorFst *intersection_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive=false );
  fst::StdVectorFst *negation_( fst::StdVectorFst *t, KeyPairSet *Pi, bool destructive=false );
  fst::StdVectorFst *subtraction_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive=false );
  fst::StdVectorFst *composition_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive=false );
  fst::StdVectorFst *determinize_( fst::StdVectorFst *t, bool destructive=false ); // destructive
  fst::StdVectorFst *minimize_( fst::StdVectorFst *t, bool destructive=true );    // destructive

  // these operations change their argument and return a pointer to it
  fst::StdVectorFst *insert_freely_( fst::StdVectorFst *t, KeyPair *p );
  static fst::StdVectorFst *substitute_with_transducer_( fst::StdVectorFst *t, KeyPair *p, fst::StdVectorFst *tr );
  fst::StdVectorFst *substitute_with_pair_( fst::StdVectorFst *t, KeyPair *p1, KeyPair *p2 );
  fst::StdVectorFst *substitute_key_( fst::StdVectorFst *t, Key k1, Key k2 );

  // other basic operations are used directly with OpenFst interface:
  // fst::Concat, fst::Union, fst::Invert, fst::Reverse, fst::Project, fst::Closure

  fst::StdVectorFst *define_pi_transducer_( KeyPairSet *Pi );
  fst::StdVectorFst *define_pi_transducer_( KeySet *ss );

  // actual implementations of API functions (after type conversion)

  Contexts *add_context_( Contexts *p1, Contexts *p2 );
  Contexts *make_context_( fst::StdVectorFst *t1, fst::StdVectorFst *t2 );
  fst::StdVectorFst *explode_( fst::StdVectorFst *t );
  fst::StdVectorFst *make_mapping_( Ranges *rs1, Ranges *rs2 );
  // fst::StdVectorFst *make_rule_( fst::StdVectorFst *t1, Range *r1, Twol_Type type, Range *r2, fst::StdVectorFst *t2, KeyPairSet *Pi );
  fst::StdVectorFst *new_transducer_( Range *r1, Range *r2, KeyPairSet *Pi=NULL );
  fst::StdVectorFst *read_transducer_( char *filename );
  fst::StdVectorFst *read_words_( char *filename );
  // fst::StdVectorFst *replace_( fst::StdVectorFst *t, Repl_Type type, bool optional );
  // fst::StdVectorFst *replace_in_context_( fst::StdVectorFst *t, Repl_Type type, Contexts *p, bool optional, KeyPairSet *Pi );
  // fst::StdVectorFst *restriction_( fst::StdVectorFst *t, Twol_Type type, Contexts *p, int direction, KeyPairSet *Pi);
  fst::StdVectorFst *result_( fst::StdVectorFst *t, bool invert, bool reverse );
  fst::StdVectorFst *rvar_value_( char *name );
  fst::StdVectorFst *var_value_( char *name );
  bool define_transducer_agreement_variable_( char *name, fst::StdVectorFst *t );
  bool define_transducer_variable_( char *name, fst::StdVectorFst *t );
  void def_alphabet_( fst::StdVectorFst *t ); 
  //void write_to_file_( fst::StdVectorFst *t, char *filename );
  void print_fsa_paths_( fst::StdVectorFst *t);


  KeyPairSet *make_pair_set_( fst::StdVectorFst *t );
  KeyPairSet *make_pair_set( TransducerHandle t );

  
  // helping methods

  char *int_to_utf8( unsigned int );
  unsigned int utf8toint( char **s );
  static void error( const char *message );
  void error2( const char *message, const char *input );
  // Transducer_ *make_fst(char *str);
  // Transducer_ *make_fst_utf(char *str);
  fst::StdVectorFst *get_word(ifstream &is);

  long arcs(fst::StdVectorFst *t);
  long states(fst::StdVectorFst *t);
  bool is_epsilon_(fst::StdVectorFst *t);
  bool is_empty_(fst::StdVectorFst *t);
  bool is_final(fst::StdVectorFst *t, StateId state_id);
  bool is_automaton_(fst::StdVectorFst *t);
  bool is_cyclic_(fst::StdVectorFst *t);
  bool is_infinitely_ambiguous_(fst::StdVectorFst * t, bool output);
  void print_fst(fst::StdVectorFst *t, KeyTable *T=NULL, bool print_weights=false, ostream& ostr=cout, bool use_numbers=false );
  void print_fst_number(fst::StdVectorFst *t);
  fst::StdVectorFst *make_epsilon_();
  fst::StdVectorFst *make_fst(Character ilabel, Character olabel);
  fst::StdVectorFst *make_fst(Character label);
  //fst::StdVectorFst *make_fst_(char *str);
  fst::StdVectorFst *make_fst_utf(char *str);
  fst::SymbolTable *get_symbols(Alphabet &alph);
  //fst::StdVectorFst *pi_machine( Alphabet &alph ); // [.]*
  fst::StdVectorFst *rho_machine( Alphabet &alph ); // [.]
  // fst::StdVectorFst *replace_transducer( fst::StdVectorFst *ct, Character lm, Character rm, Repl_Type type );
  // fst::StdVectorFst *replace_( fst::StdVectorFst *ct, Repl_Type type, bool optional );

  // static fst::StdVectorFst *twol_left_rule( fst::StdVectorFst *lc, Range *lower_range, Range *upper_range, fst::StdVectorFst *rc );
  // static fst::StdVectorFst *twol_right_rule( fst::StdVectorFst *lc, Range *lower_range, Range *upper_range, fst::StdVectorFst *rc );
  static fst::StdVectorFst *cp( Range *lower_range, Range *upper_range );
  static fst::StdVectorFst *anti_cp( Range *lower_range, Range *upper_range, KeyPairSet *Pi );

  static void free_values( Range *r );
  static fst::StdVectorFst *make_transducer( Range *r1, Range *r2, KeyPairSet *Pi=NULL );

  // fst::StdVectorFst *extended_context( fst::StdVectorFst *t, Character m1, Character m2 );
  // fst::StdVectorFst *replace_context( fst::StdVectorFst *t, Character m1, Character m2, KeyPairSet *Pi );

  bool equivalent_( fst::StdVectorFst *t1, fst::StdVectorFst *t2);
  

  void add_epsilon_to_alphabet() {
    TheAlphabet.add_epsilon();
  };


  /*  TransducerHandle
  make_fst_from_xerox_regexp(const char *xre)
  {
    TheAlphabet.utf8 = true;
    xrewnerrs = 0;
    xre_data = strdup(xre);
    xre_len = strlen(xre_data);
    xrewparse();
    if (0 == xrewnerrs)
      {
	return xrew_transducer;
      }
    else
      {
	return 0;
      }
      }*/  
  


  // ********************************************************************
  // ********************************************************************
  //
  //                    HFST: Transducer Layer API
  //
  // ********************************************************************
  // ********************************************************************




  // *********************************************************************
  // ***
  // ***  basic transducer operations that need encoding and decoding  ***
  // ***
  // *********************************************************************


  TransducerHandle intersect( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst* const pResult = intersection_(pT1, pT2, true);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle filter_epsilon_paths( TransducerHandle t ) {
    return NULL;
  }

  fst::StdVectorFst *intersection_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive ) { 

  /* NOTE: Input fsts must be acceptors (transducers with equal input and output labels). 
     One of the acceptors must be label-sorted. */

    fst::StdVectorFst *intersection = new fst::StdVectorFst;

    fst::RmEpsilon(t1);
    fst::RmEpsilon(t2);

    fst::EncodeMapper<fst::StdArc> mapper(0x00011,fst::EncodeType(1)); // 3 = Labels and Weights, ENCODE = 1

    fst::EncodeFst<fst::StdArc> A1(*t1, mapper);
    fst::StdVectorFst Encoded_A1(A1);
    fst::StdVectorFst *pEncoded_A1 = &Encoded_A1;

    fst::EncodeFst<fst::StdArc> A2(*t2, mapper);
    fst::StdVectorFst Encoded_A2(A2);
    fst::StdVectorFst *pEncoded_A2 = &Encoded_A2;

    fst::ArcSort(pEncoded_A1, fst::StdILabelCompare());
    fst::ArcSort(pEncoded_A2, fst::StdILabelCompare());

    fst::Intersect(*pEncoded_A1, *pEncoded_A2, intersection);

    fst::DecodeFst<fst::StdArc> D(*intersection, mapper);
    fst::StdVectorFst Decoded(D);
    fst::StdVectorFst *t = Decoded.Copy(); 
    delete intersection;

    if (destructive) {
      delete t1;
      delete t2;
    }
    return t;
  }


  TransducerHandle subtract( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst* const pResult = subtraction_(pT1, pT2, true);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *subtraction_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive ) {

    /* In function Difference the first argument must be an acceptor; the second argument must be an 
       unweighted, epsilon-free, deterministic acceptor. The output labels of the first 
       acceptor or the input labels of the second acceptor must be sorted. */
    
    fst::StdVectorFst *difference = new fst::StdVectorFst();
    
    fst::RmEpsilon(t1);
    fst::RmEpsilon(t2);

    fst::EncodeMapper<fst::StdArc> mapper1(0x00011,fst::EncodeType(1)); // 3 = Weights and Labels ENCODE = 1

    fst::EncodeFst<fst::StdArc> A1(*t1, &mapper1);
    fst::StdVectorFst Encoded_A1(A1);
    fst::StdVectorFst *pEncoded_A1 = &Encoded_A1;

    fst::EncodeFst<fst::StdArc> A2(*t2, &mapper1);
    fst::StdVectorFst Encoded_A2(A2);
    fst::StdVectorFst *pEncoded_A2 = &Encoded_A2;

    fst::Determinize(*pEncoded_A2, pEncoded_A2);

    fst::ArcSort(pEncoded_A1, fst::StdOLabelCompare());
    fst::ArcSort(pEncoded_A2, fst::StdILabelCompare());

    fst::Difference(*pEncoded_A1, *pEncoded_A2, difference);

    fst::DecodeFst<fst::StdArc> D(*difference, mapper1);
    fst::StdVectorFst Decoded(D);
    fst::StdVectorFst *t = Decoded.Copy();
    delete difference;
      
    if (destructive) {
      delete t1; 
      delete t2;
    }    
    return t;
  }


  TransducerHandle compose( TransducerHandle t1, TransducerHandle t2, bool destructive ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst* const pResult = composition_(pT1, pT2, destructive);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle compose( TransducerHandle t1, TransducerHandle t2) {
    bool destructive = true;
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst* const pResult = composition_(pT1, pT2, destructive);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *composition_( fst::StdVectorFst *t1, fst::StdVectorFst *t2, bool destructive ) {
    fst::StdVectorFst *composition = new fst::StdVectorFst;
    fst::ArcSort(t1, fst::StdOLabelCompare());
    fst::ArcSort(t2, fst::StdILabelCompare());

    fst::Compose(*t1, *t2, composition);
    if (destructive) {
      delete t1;
      delete t2;
    }
    return composition;
  }

  
  TransducerHandle negate( TransducerHandle t, KeyPairSet *Pi ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* const pResult = negation_(pT, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *negation_( fst::StdVectorFst *t, KeyPairSet *Pi, bool destructive ) { // !t = (.*) - t
    
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure( pi, fst::CLOSURE_STAR );
    fst::StdVectorFst *result = subtraction_(pi, t);
    delete pi;
    if (destructive)
      delete t;
    return result;
  }


  // ******* Epsilon removal, determinization and minimization *******

  TransducerHandle remove_epsilons( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::RmEpsilon( pT );
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }

  fst::StdVectorFst *determinize_and_minimize_( fst::StdVectorFst *t, bool min=true, bool destructive=true) {
    fst::RmEpsilon(t);
    fst::EncodeMapper<fst::StdArc> mapper(3,fst::EncodeType(1)); // 3 = Labels and Weights ?, ENCODE = 1
    fst::EncodeFst<fst::StdArc> TEncode(*t, &mapper);
    if (destructive)
      delete t;
    fst::StdVectorFst Encoded_T(TEncode);
    fst::StdVectorFst *Determinized_T = new fst::StdVectorFst();
    fst::Determinize(Encoded_T, Determinized_T);
    if (min)
      fst::Minimize(Determinized_T);
    fst::DecodeFst<fst::StdArc> D1(*Determinized_T,mapper);
    fst::StdVectorFst *DecodedT = new fst::StdVectorFst(D1);
    delete Determinized_T;
    return DecodedT;
  }

  fst::StdVectorFst *determinize_( fst::StdVectorFst *t, bool destructive ) {
    return determinize_and_minimize_(t, false, destructive);
  }

  fst::StdVectorFst *minimize_( fst::StdVectorFst *t, bool destructive ) {
    return determinize_and_minimize_(t, true, destructive);
  }

  TransducerHandle determinize( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pT = determinize_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }

  TransducerHandle minimize( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pT = minimize_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }





  // **********************************************************************
  // ***
  // ***  other basic operations that change their argument and return  ***
  // ***  a pointer to it                                               ***
  // ***
  // **********************************************************************


  fst::StdVectorFst *substitute_key_( fst::StdVectorFst *t, Key k1, Key k2 ) {
    for( int i = 0; i < t->NumStates(); ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();
	if ( arc.ilabel == k1 )
	  arc.ilabel = k2;
	if ( arc.olabel == k1 )
	  arc.olabel = k2;
	it.SetValue(arc);
      }	
    } 
    return t; 
  }

  TransducerHandle substitute_key( TransducerHandle t, Key k1, Key k2, bool ) {
    fst::StdVectorFst *pt = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pt = substitute_key_(pt, k1, k2);
    return PINSTANCE_TO_HANDLE(Transducer, pt);
  }

  fst::StdVectorFst *substitute_key_( fst::StdVectorFst *t, KeySet *ks, Key k2 ) {
    for( int i = 0; i < t->NumStates(); ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();
	if ( ks->find(arc.ilabel) != ks->end() )
	  arc.ilabel = k2;
	if ( ks->find(arc.olabel) != ks->end() )
	  arc.olabel = k2;
	it.SetValue(arc);
      }	
    } 
    return t; 
  }

  TransducerHandle substitute_key( TransducerHandle t, KeySet *ks, Key k2 ) {
    fst::StdVectorFst *pt = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pt = substitute_key_(pt, ks, k2);
    return PINSTANCE_TO_HANDLE(Transducer, pt);
  }


  fst::StdVectorFst *substitute_with_pair_( fst::StdVectorFst *t, KeyPair *p1, KeyPair *p2 ) {
    for( int i = 0; i < t->NumStates(); ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();
	if ( arc.ilabel == get_input_key(p1) && arc.olabel == get_output_key(p1) ) {
	  arc.ilabel = get_input_key(p2);
	  arc.olabel = get_output_key(p2);
	}
	it.SetValue(arc);
      }	
    } 
    return t; 
  }

  TransducerHandle substitute_with_pair( TransducerHandle t, KeyPair *p1, KeyPair *p2 ) {
    fst::StdVectorFst *pt = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pt = substitute_with_pair_(pt, p1, p2);
    return PINSTANCE_TO_HANDLE(Transducer, pt);
  }


  /* Replace symbol s in transducer t with Transducer tr.

  For each arc arc_s in t having input and output symbols equal to s,
  a copy of tr, tr_copy, is created and arc_s
  is replaced by an epsilon transition to the start state of
  tr_copy.
  
  An epsilon transition is added from every final state in tr_copy
  to the original destination state of arc_s. Final states of
  tr_copy are then made non-final. */

  static fst::StdVectorFst *substitute_with_transducer_( fst::StdVectorFst *t,
							 KeyPair *p,
							 fst::StdVectorFst *tr ) {
    if ( is_empty_(tr) )
      error("Third argument in function 'replace_char_new' is an empty transducer.");

    int states = t->NumStates();
    for( int i = 0; i < states; ++i ) {

      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {

	fst::StdArc arc = it.Value();


	// find arcs that must be replaced
	if ( arc.ilabel == get_input_key(p) && 
	     arc.olabel == get_output_key(p) ) {

	  StateId destination_state = arc.nextstate;
	  StateId start_state = t->AddState();

	  // change the label of the arc to epsilon and point the arc to a new state
	  arc.ilabel = Label::epsilon;
	  arc.olabel = Label::epsilon;
	  arc.nextstate = start_state;  
	  // weight remains the same
	  it.SetValue(arc);


	  // add rest of the states to transducer t
	  int states_to_add = tr->NumStates();
	  for (int j=1; j<states_to_add; j++)
	    t->AddState();


	  // go through all states and arcs in replace transducer tr
	  for (fst::StateIterator<fst::StdFst> siter(*tr); !siter.Done(); siter.Next()) {

	    StateId tr_state_id = siter.Value();

	    // final states in tr correspond in t to a non-final state which has
	    // an epsilon transition to original destination state of arc that
	    // is being replaced
	    if ( is_final( tr, tr_state_id ) )
	      t->AddArc( tr_state_id + start_state,
			 fst::StdArc( Label::epsilon,
				      Label::epsilon,
				      tr->Final(tr_state_id),  // final weight is copied to the epsilon transition
				      destination_state
				      )
			 );  

	    for (fst::ArcIterator<fst::StdFst> aiter(*tr, tr_state_id); !aiter.Done(); aiter.Next()) {

	      const fst::StdArc &tr_arc = aiter.Value();

	      // adding arc from state 'tr_state_id+start_state' to state 'tr_arc.nextstate'
	      // copy arcs from tr to t
	      t->AddArc( tr_state_id + start_state, 
			 fst::StdArc( tr_arc.ilabel, 
				      tr_arc.olabel, 
				      tr_arc.weight,  // weight remains the same 
				      tr_arc.nextstate + start_state 
				      ) 
			 );

	    }
	  }
	}
      }
    }
    return t;
  }

  TransducerHandle substitute_with_transducer( TransducerHandle t,
					       KeyPair *p,
					       TransducerHandle tr ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* pTR = HANDLE_TO_PINSTANCE(fst::StdVectorFst, tr);    
    fst::StdVectorFst* pResult = substitute_with_transducer_(pT, p, pTR);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  // for marker creating
  set<Key> define_key_set_( fst::StdVectorFst *t ) {

    set<Key> keys;

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	keys.insert(arc.ilabel);
	keys.insert(arc.olabel);
      }
    }
    return keys;
  }

  set<Key> *define_key_set_pointer_( fst::StdVectorFst *t ) {

    set<Key> *keys = new set<Key>();

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	keys->insert(arc.ilabel);
	keys->insert(arc.olabel);
      }
    }
    return keys;
  }

  KeySet *define_key_set( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    set<Key> *keys = define_key_set_pointer_(pT);
    return keys;
  }

  Key find_unused_key(set<Key> &keys) {
    for( unsigned int i=1; i!=0; i++ ) {
      if( keys.find(i) == keys.end() ) {
	return i;
      }
    }
    throw "ERROR: remove_pair: new marker key could not be created.\n";
  }


  TransducerHandle remove_pair( TransducerHandle t, KeyPair *p ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

    // find a key that is not in use
    set<Key> keys = define_key_set_(pT);
    keys.insert(get_input_key(p));
    keys.insert(get_output_key(p));
    Key marker = find_unused_key(keys);

    KeyPair *eps = define_keypair(Label::epsilon, Label::epsilon);
    KeyPair *mark = define_keypair(marker, marker);
    pT = substitute_with_pair_(pT, eps, mark);
    pT = substitute_with_pair_(pT, p, eps);

    fst::RmEpsilon(pT);
    pT = substitute_with_pair_(pT, mark, eps);
    delete eps;
    delete mark;

    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }

  
  TransducerHandle remove_pairs( TransducerHandle t, KeySet *ss ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

    // find a key that is not in use
    set<Key> keys = define_key_set_(pT);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ )
      keys.insert(get_sigma_key(it));
    Key marker = find_unused_key(keys);

    KeyPair *eps = define_keypair(Label::epsilon, Label::epsilon);
    KeyPair *mark = define_keypair(marker, marker);

    pT = substitute_with_pair_(pT, eps, mark);

    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++ ) {
      KeyPair *ss_pair = define_keypair( get_sigma_key(it), get_sigma_key(it) );
      pT = substitute_with_pair_(pT, ss_pair, eps);
      delete ss_pair;
    }

    fst::RmEpsilon(pT);
    pT = substitute_with_pair_(pT, mark, eps);
    delete eps;
    delete mark;

    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }



  fst::StdVectorFst *insert_freely_( fst::StdVectorFst *t, KeyPair *p ) {
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      t->AddArc(state_id, fst::StdArc(get_input_key(p), get_output_key(p), 0, state_id));
    }
    return t;
  }

  TransducerHandle insert_freely( TransducerHandle t, KeyPair *p ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* pResult = insert_freely_(pT, p);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  // for hfst-calculate
  TransducerHandle insert_freely( TransducerHandle t, Key k1, Key k2 ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    for (fst::StateIterator<fst::StdFst> siter(*pT); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      pT->AddArc(state_id, fst::StdArc(k1, k2, 0, state_id));
    }
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }






  // **********************************
  // ***
  // ***      Creating transducers 
  // ***
  // **********************************


  fst::StdVectorFst *make_empty_() {
    fst::StdVectorFst *empty = new fst::StdVectorFst;
    empty->AddState();
    empty->SetStart(0);
    return empty;
  }

  fst::StdVectorFst make_empty_np() {
    fst::StdVectorFst empty;
    empty.AddState();
    empty.SetStart(0);
    return empty;
  }

  fst::StdVectorFst *make_epsilon_() {
    fst::StdVectorFst *eps = make_empty_();
    eps->SetFinal(0,0);
    return eps;
  }

  fst::StdVectorFst make_epsilon_np() {
    fst::StdVectorFst eps = make_empty_np();
    eps.SetFinal(0,0);
    return eps;
  }

  fst::StdVectorFst *make_fst(Character ilabel, Character olabel) {
    fst::StdVectorFst *t = new fst::StdVectorFst;
    t->AddState();
    t->SetStart(0);
    t->AddArc(0, fst::StdArc(ilabel, olabel, 0, 1));
    t->AddState();
    t->SetFinal(1, 0);
    return t;
  }

  fst::StdVectorFst make_fst_np(Character ilabel, Character olabel) {
    fst::StdVectorFst t;
    t.AddState();
    t.SetStart(0);
    t.AddArc(0, fst::StdArc(ilabel, olabel, 0, 1));
    t.AddState();
    t.SetFinal(1, 0);
    return t;
  }

  fst::StdVectorFst make_fst_np(Character c) {
    return make_fst_np(c, c);
  }

  fst::StdVectorFst *make_fst(Character label) {
    return make_fst(label, label); 
  }

  fst::StdVectorFst *define_pi_transducer_( KeyPairSet *Pi ) {
    fst::StdVectorFst *t = new fst::StdVectorFst;
    t->AddState();
    t->SetStart(0);
    for ( KeyPairIterator it = begin_pi_key(Pi); it != end_pi_key(Pi); it++) {
      KeyPair *p = get_pi_keypair(it);
      Key ikey = get_input_key(p);
      Key okey = get_output_key(p);
      t->AddArc(0, fst::StdArc(ikey, okey, 0, 1));
    }
    t->AddState();
    t->SetFinal(1, 0);
    return t;
  }

  fst::StdVectorFst *define_pi_transducer_( KeySet *ss ) {
    fst::StdVectorFst *t = new fst::StdVectorFst;
    t->AddState();
    t->SetStart(0);
    for ( KeyIterator it = begin_sigma_key(ss); it != end_sigma_key(ss); it++) {
      Key k = get_sigma_key(it);
      t->AddArc(0, fst::StdArc(k, k, 0, 1));
    }
    t->AddState();
    t->SetFinal(1, 0);
    return t;
  }

  KeyPairSet *define_keypair_set_( fst::StdVectorFst *t ) {

    KeyPairSet *pairs = create_empty_keypair_set();

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	// no epsilon pairs
	//if (arc.ilabel != Label::epsilon || arc.olabel != Label::epsilon ) {
	  SymbolPair *pair = define_keypair(arc.ilabel, arc.olabel);
	  if (!has_keypair(pair, pairs))
	    insert_keypair(pair, pairs);
	  else
	    delete pair;
	  //}
      }
    }
    return pairs;
  };
  


  // ****** Creating transducers (interface) ******

  TransducerHandle create_empty_transducer() {
    fst::StdVectorFst *empty = make_empty_();
    return PINSTANCE_TO_HANDLE(Transducer, empty);
  }

  TransducerHandle create_epsilon_transducer() {
    fst::StdVectorFst* const pResult = make_epsilon_();
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle define_transducer( Key k ) {
    fst::StdVectorFst *t = make_fst(k, k);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  TransducerHandle define_transducer( KeyPair *p ) {
    fst::StdVectorFst *t = make_fst( get_input_key(p), get_output_key(p) );
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  TransducerHandle define_transducer( KeySet *ss ) {
    fst::StdVectorFst *pi = define_pi_transducer_(ss);
    return PINSTANCE_TO_HANDLE(Transducer, pi);
  }

  TransducerHandle define_transducer( KeyPairSet *Pi ) {
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pi);
  }

  KeyPairSet *define_keypair_set( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return define_keypair_set_(pT);
  }

  TransducerHandle add_weight( TransducerHandle t, float w ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    for (fst::StateIterator<fst::StdFst> siter(*pT); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      if (is_final(pT, state_id)) {
	pT->SetFinal(state_id, fst::TropicalWeight( w ));
      }
    }
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }

  TransducerHandle modify_weights( TransducerHandle t, float(*modify)(float), bool modify_transitions ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

    for (fst::StateIterator<fst::StdFst> siter(*pT); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      fst::TropicalWeight state_weight = pT->Final(state_id);
      if ( state_weight != fst::TropicalWeight::Zero()) { // state is final
	float old_final_weight = state_weight.Value();
	float new_final_weight = modify(old_final_weight);
	pT->SetFinal(state_id, fst::TropicalWeight(new_final_weight));
      }

      if (modify_transitions) {
	for (fst::MutableArcIterator<fst::StdVectorFst> aiter(pT, state_id); !aiter.Done(); aiter.Next()) {
	  fst::StdArc arc = aiter.Value();
	  float old_arc_weight = (arc.weight).Value();
	  float new_arc_weight = modify(old_arc_weight);
	  arc.weight = fst::TropicalWeight(new_arc_weight);
	  aiter.SetValue(arc);
	}
      }
    }
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }


  void delete_transducer(TransducerHandle t) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    delete pT;
  };
}

//#include "transducer-conversion.h"

namespace HWFST {



  // *******************************************************************
  // ***  
  // ***  Basic operations 
  // ***  
  // *******************************************************************


  TransducerHandle concatenate( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::Concat(pT1, *pT2);
    delete pT2;
    return PINSTANCE_TO_HANDLE(Transducer, pT1);
  }

  TransducerHandle copy(TransducerHandle t) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t); 
    fst::StdVectorFst* const copy_of_t = pT->Copy();
    return PINSTANCE_TO_HANDLE(Transducer, copy_of_t);
  }

  TransducerHandle disjunct( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::Union(pT1, *pT2);
    delete pT2;
    return PINSTANCE_TO_HANDLE(Transducer, pT1);
  }

  TransducerHandle optionalize( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Union(pT, make_epsilon_np());
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }

  TransducerHandle repeat_n(TransducerHandle t, int n) {
    if (n==0)
      return create_epsilon_transducer();
    else {
      fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
      fst::StdVectorFst* t_copy = pT->Copy();
      for (int i=1; i<n; i++)
	fst::Concat(pT, *t_copy);
      delete t_copy;
      fst::RmEpsilon(pT);
      return PINSTANCE_TO_HANDLE(Transducer, pT);
    }
  }
  
  TransducerHandle repeat_le_n(TransducerHandle t, int n) {
    if (n==0)
      return create_epsilon_transducer();
    else {
      fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
      fst::Union(pT, make_epsilon_np());
      fst::StdVectorFst* t_copy = pT->Copy();
      for (int i=1; i<n; i++)
	fst::Concat(pT, *t_copy);
      delete t_copy;
      fst::RmEpsilon(pT);
      return PINSTANCE_TO_HANDLE(Transducer, pT);
    }
  }  

  TransducerHandle repeat_star( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Closure(pT, fst::CLOSURE_STAR);
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }
  

  TransducerHandle repeat_plus( TransducerHandle t ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Closure(pT, fst::CLOSURE_PLUS);
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }



  TransducerHandle shuffle( TransducerHandle t1, TransducerHandle t2 ) {
    if (RS.size() > 0 || RSS.size() > 0)
      cerr << "\nWarning: agreement operation inside of shuffle!\n";
    fst::StdVectorFst* pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst *pResult = shuffle_(pT1, pT2);
    delete pT1;
    delete pT2;
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }
  

  TransducerHandle reverse( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* const pResult = new fst::StdVectorFst();
    fst::Reverse(*pT, pResult);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  TransducerHandle invert( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Invert(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }


  TransducerHandle extract_output_language( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Project( pT, fst::PROJECT_OUTPUT );
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }


  TransducerHandle extract_input_language( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::Project( pT, fst::PROJECT_INPUT );
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }


  TransducerHandle add_input_language( TransducerHandle t, KeyPairSet *s ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst *pi=new fst::StdVectorFst;
    pi->AddState();
    pi->SetStart(0);
    pi->SetFinal(0, 0);
    for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
      KeyPair *kp = get_pi_keypair(it);
      if (get_input_key(kp) != Label::epsilon) {
	Key ikey = get_input_key(kp);
	Key okey = get_output_key(kp);
	pi->AddArc(0, fst::StdArc(ikey, okey, 0, 0));
      }
    }
    fst::StdVectorFst *comp = composition_(pT, pi, true);
    
    for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
      KeyPair *kp = get_pi_keypair(it);
      if (get_input_key(kp) == Label::epsilon)
	comp = insert_freely_(comp, kp);
    }
    return PINSTANCE_TO_HANDLE(Transducer, comp);
  }
  
  
  TransducerHandle add_output_language( TransducerHandle t, KeyPairSet *s ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst *pi=new fst::StdVectorFst;
    pi->AddState();
    pi->SetStart(0);
    pi->SetFinal(0, 0);
    for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
      KeyPair *kp = get_pi_keypair(it);
      if (get_output_key(kp) != Label::epsilon) {
	Key ikey = get_input_key(kp);
	Key okey = get_output_key(kp);
	pi->AddArc(0, fst::StdArc(ikey, okey, 0, 0));
      }
    }
    fst::StdVectorFst *comp = composition_(pi, pT, true);
    
    for (KeyPairIterator it = s->begin(); it!=s->end(); it++) {
      KeyPair *kp = get_pi_keypair(it);
      if (get_output_key(kp) == Label::epsilon)
	comp = insert_freely_(comp, kp);
    }
    return PINSTANCE_TO_HANDLE(Transducer, comp);
  }
  


  // *******************************************************************
  // ***  
  // ***  Optimizing, converting and testing functions 
  // ***  
  // *******************************************************************


  TransducerHandle push_weights( TransducerHandle t, bool initial ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    if (initial)
      fst::Push( pT, fst::ReweightType(0));
    else  // final
      fst::Push( pT, fst::ReweightType(1));
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }	

  TransducerHandle push_labels( TransducerHandle t, bool initial ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* pResult = new fst::StdVectorFst();
    if (initial)
      fst::Push<fst::StdArc, fst::REWEIGHT_TO_INITIAL>(*pT, pResult, fst::kPushLabels);
    else  // final
      fst::Push<fst::StdArc, fst::REWEIGHT_TO_FINAL>(*pT, pResult, fst::kPushLabels);
    delete pT;
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  TransducerHandle find_best_paths(TransducerHandle t, int n, bool unique) {
    if (n==0)
      return create_empty_transducer();
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    if (is_empty_(pT))
      return create_empty_transducer();

    fst::StdVectorFst *paths = new fst::StdVectorFst;
    fst::ShortestPath(*pT, paths, n, unique);
    return PINSTANCE_TO_HANDLE(Transducer, paths);
  }

  TransducerHandle find_random_paths(TransducerHandle t, int n, bool b) {
    //return find_best_paths(t,n,false);
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst * paths = new fst::StdVectorFst;
    time_t timer;
    fst::StdArcSelector selector;
    fst::RandGenOptions<fst::StdArcSelector> opts(selector);
    for (int i = 0; i < n; ++i)
      {
	fst::StdVectorFst * path = new fst::StdVectorFst;
	fst::RandGen<StdArc,fst::StdArcSelector> (*pT,path,opts);
	fst::Union(paths,*path);
      }
    t = PINSTANCE_TO_HANDLE(Transducer,paths);
    return t;
  }
  
  typedef vector< stack<fst::StdArc> > PathVector;
  PathVector *find_paths(fst::StdVectorFst *t, vector<float> *final_weights, bool connect=false);
  vector<fst::StdVectorFst*> paths_to_transducers(PathVector pv, vector<float> *final_weights);

  vector<fst::StdVectorFst*> find_all_paths_( fst::StdVectorFst *t, bool unique ) {
    if (is_empty_(t)) {
      vector<fst::StdVectorFst*> empty;
      return empty;
    }

    if (is_cyclic_(t))
      throw "ERROR: argument transducer is cyclic.\n";

    fst::StdVectorFst *T = t;
    if (unique)
      T = determinize_(t, false);
    vector<float> *final_weights = new vector<float>();
    PathVector *pv = find_paths(T, final_weights);
    if (unique)
      delete T;
    vector<fst::StdVectorFst*> transducers = paths_to_transducers(*pv, final_weights);
    pv->clear();
    delete pv;
    final_weights->clear();
    delete final_weights; // #####
    return transducers;
  }

  vector<TransducerHandle> find_all_paths( TransducerHandle t, bool unique ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    vector<fst::StdVectorFst*> transducers = find_all_paths_(pT, unique);
    vector<TransducerHandle> *result = new vector<TransducerHandle>();
    for (unsigned int i=0; i<transducers.size(); i++)
      result->push_back( PINSTANCE_TO_HANDLE(Transducer, transducers[i]) );
    return *result;
  }

  /*vector< vector<KeyPair> > find_all_paths( TransducerHandle t, bool unique ) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    vector<fst::StdVectorFst*> transducers = find_all_paths_(pT, unique);
    vector<TransducerHandle> *result = new vector<TransducerHandle>();
    for (unsigned int i=0; i<transducers.size(); i++)
      result->push_back( PINSTANCE_TO_HANDLE(Transducer, transducers[i]) );
    return *result;
    }*/


  /* Test if for every arc <ilabel>:<olabel> in transducer t <ilabel> == <olabel>. */
  bool is_automaton_(fst::StdVectorFst *t) {
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	if (arc.ilabel != arc.olabel)
	  return false;
      }
    }
    return true;
  }

  /* Test if t is cyclic. */
  bool is_cyclic_(fst::StdVectorFst *t) {
    vector<float> *final_weights = new vector<float>();
    PathVector *pv = find_paths(t, final_weights);
    final_weights->clear();  // #####
    delete final_weights;    // #####
    if (pv == NULL)
      return true;
    pv->clear();  // #####
    delete pv;
    return false;
  }

  typedef fst::StateId StateId;
  typedef std::set<fst::StateId> StateSet;    

  bool is_infinitely_ambiguous_state(StateId id,
				     fst::StdVectorFst * t,
				     StateSet &state_successors,
				     bool output)
  {
    if (state_successors.find(id) != state_successors.end())
      {
	return true;
      }
    state_successors.insert(id);
    for (fst::ArcIterator<fst::StdFst> aiter(*t, id); 
	 !aiter.Done(); 
	 aiter.Next()) 
      {
	StdArc a = aiter.Value();
	StateId follower_id = a.nextstate;
	    
	if (output and (a.ilabel == 0))
	  {
	    if (is_infinitely_ambiguous_state(follower_id,
					      t,
					      state_successors,
					      output))
	      {
		return true;
	      }
	  }
	else if ((not output) and (a.olabel == 0))
	  {
	    if (is_infinitely_ambiguous_state(follower_id,
					      t,
					      state_successors,
					      output))
	      {
		return true;
	      }
	  }
      }
    state_successors.erase(id);
    return false;
  }

  bool is_infinitely_ambiguous_(fst::StdVectorFst * t, bool output)
  {
    StateSet state_successors;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) 
      {
	StateId id = siter.Value();
	if (is_infinitely_ambiguous_state(id,t,state_successors,output))
	  {
	    return true;
	  }
      }
    return false;
  }


  bool is_deterministic_(fst::StdVectorFst *t, bool omit_root_node=false) {
    
    fst::ArcSort(t, fst::StdIOLabelCompare());  // Compare class added by Erik Axelson
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      if (!(omit_root_node && (state_id == t->Start()))) {
	fst::Label previous_ilabel=0; fst::Label previous_olabel=0; // value never used
	bool first_arc = true;
	for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	  const fst::StdArc &arc = aiter.Value();
	  // epsilon transition?
	  if ( arc.ilabel == 0 &&
	       arc.olabel == 0 )
	    return false;
	  // similar transition twice? (what about weight?)
	  if (!first_arc) {
	    if ( previous_ilabel == arc.ilabel &&
		 previous_olabel == arc.olabel )
	      return false;
	  }
	  else
	    first_arc = false;
	  previous_ilabel = arc.ilabel;
	  previous_olabel = arc.olabel;
	}
      }
    }
    return true;
  }

  bool find_final_state( fst::StdVectorFst *t, StateId state_id, bool visited[] ) {
    if (visited[state_id])
      return false;
    if (is_final(t, state_id))
      return true;
    visited[state_id] = true;
    for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      if (find_final_state(t, arc.nextstate, visited))
	return true;
    }
    return false;
  }
      

  /* Is t the empty transducer. */
  bool is_empty_(fst::StdVectorFst *t) {    
    if (t->Start() == -1)  // OpenFst representation for a _minimal_ empty transducer 
      return true;    
    // try to find an end state with depth-first search
    unsigned int num_states = t->NumStates();
    bool visited[num_states];
    for (unsigned int i=0; i<num_states; i++)
      visited[i] = false;
    if ( find_final_state(t, t->Start(), visited) )
      return false;
    return true;
  }

  bool is_epsilon_(fst::StdVectorFst *t) {
    int states = 0;
    int arcs = 0;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      states++;
      if (states == 2)
	return false;
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {

	arcs++;
      }
    }
    if (arcs == 0 && states == 1) { 
      if (is_final(t, 0))
	return true;
    }

    return false;
  }

  bool is_minimal_(fst::StdVectorFst *t) {
    return false;
  }

  bool equivalent_( fst::StdVectorFst *t1, fst::StdVectorFst *t2 ) {
    fst::RmEpsilon(t1);
    fst::RmEpsilon(t2);
    fst::EncodeMapper<fst::StdArc> mapper(0x00011,fst::EncodeType(1));
    fst::EncodeFst<fst::StdArc> TEncode1(*t1, &mapper);
    fst::StdVectorFst Encoded_T1(TEncode1);
    fst::EncodeFst<fst::StdArc> TEncode2(*t2, &mapper);
    fst::StdVectorFst Encoded_T2(TEncode2);
    Determinize(Encoded_T1, &Encoded_T1);
    Determinize(Encoded_T2, &Encoded_T2);
    return fst::Equivalent(Encoded_T1, Encoded_T2);
  }

  
  // *** API functions ***
  
  bool is_automaton(TransducerHandle t) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_automaton_(pT);
  }

  bool is_cyclic(TransducerHandle t) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_cyclic_(pT);
  }

  typedef std::map<StateId,unsigned int> NodeMap;
  
  bool has_infinitely_many_continuations(fst::StdVectorFst * tr,
					 StateId n,
					 bool output,
					 KeyVector * str,
					 unsigned int index, 
					 NodeMap &nodes)
{
  
  bool has_old_index = false;
  unsigned int old_index = 0;
  
  if (nodes.find(n) != nodes.end())
    {
      has_old_index = true;
      old_index = nodes[n];
      if (index == old_index)
	{
	  return true;
	}
    }
  nodes[n] = index;

  if (output)
    { 
      for (fst::ArcIterator<fst::StdFst> aiter(*tr, n); 
	   !aiter.Done(); 
	   aiter.Next()) 
	{

	  const fst::StdArc &arc = aiter.Value();
	  if ((index < str->size()) and
	      (arc.ilabel == str->at(index)))
	    {
	      if (has_infinitely_many_continuations(tr,
						    arc.nextstate,
						    output,
						    str,
						    index+1,
						    nodes))
		{
		  return true;
		}
	    }
	  else if (arc.ilabel == 0)
	    {
	      if (has_infinitely_many_continuations(tr,
						    arc.nextstate,
						    output,
						    str,
						    index,
						    nodes))
		{
		  return true;
		}
	    }
	}
    }
  else
    {
      for (fst::ArcIterator<fst::StdFst> aiter(*tr, n); 
	   !aiter.Done(); 
	   aiter.Next()) 
	{

	  const fst::StdArc &arc = aiter.Value();
	  if ((index < str->size()) and
	      (arc.olabel == str->at(index)))
	    {
	      if (has_infinitely_many_continuations(tr,
						    arc.nextstate,
						    output,
						    str,
						    index+1,
						    nodes))
		{
		  return true;
		}
	    }
	  else if (arc.olabel == 0)
	    {
	      if (has_infinitely_many_continuations(tr,
						    arc.nextstate,
						    output,
						    str,
						    index,
						    nodes))
		{
		  return true;
		}
	    }
	}
    }
  if (has_old_index)
    {
      nodes[n] = old_index;
    }
  else
    {
      nodes.erase(n);
    }
  return false;
}


  bool is_infinitely_ambiguous(TransducerHandle t, bool output, KeyVector * str)
  {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    if (str == NULL)
      {
	return is_infinitely_ambiguous_(pT,output);
      }
    else
      {
	NodeMap nodes;
	return has_infinitely_many_continuations(pT,
						 pT->Start(),
						 output,
						 str,
						 0,
						 nodes);
      }
  }

  bool is_deterministic( TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_deterministic_(pT);
  }

  bool is_empty(TransducerHandle t) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_empty_(pT);
  }

  bool is_epsilon(TransducerHandle t) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_epsilon_(pT);
  }

  bool is_minimal( TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_minimal_(pT);
  }

  // in file 'is-empty-intersection.C'
  // bool is_empty_intersection( fst::StdVectorFst *t1, fst::StdVectorFst *t2 );

  struct NodePair {
    StateId first;
    StateId second; 
  };
  
  struct compare_NodePairs {
    bool operator() ( const NodePair &p1, const NodePair &p2 ) const {
      if ( p1.first == p2.first ) 
	return p1.second < p2.second;
      else 
	return p1.first < p2.first;
    }
  };
  
  struct SymPair {
    Label input;
    Label output;
  };
  
  struct compare_SymPairs {
    bool operator() (const SymPair &p1, const SymPair &p2) const {
      if (p1.input == p2.input) 
	return p1.output < p2.output;
      
      else
	return p1.input < p2.input;
    }
    
  };
  
  bool are_disjoint( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst * pT1 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t1); 
    fst::StdVectorFst * pT2 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t2);
    return is_empty_intersection( pT1, pT2 );
  }

  bool empty_difference(fst::StdVectorFst * t1,
			fst::StdVectorFst * t2,
			StateId n1, 
			StateId n2, 
			set<NodePair,compare_NodePairs> &handled_nodes) {

    
    map<SymPair,StateId,compare_SymPairs> n1_labels;
    map<SymPair,StateId,compare_SymPairs> n2_labels;

    for ( fst::ArcIterator<fst::StdVectorFst> as(*t1,n1); 
	  not as.Done(); 
	  as.Next() ) {

      const fst::StdArc a = as.Value();
      SymPair s;
      s.input = a.ilabel;
      s.output = a.olabel;

      n1_labels[ s ] = a.nextstate;

    }

    for ( fst::ArcIterator<fst::StdVectorFst> as(*t2,n2); 
	  not as.Done(); 
	  as.Next() ) {

      const fst::StdArc a = as.Value();
      SymPair s;
      s.input = a.ilabel;
      s.output = a.olabel;

      n2_labels[ s ] = a.nextstate;

    }
    
    //if ( n1_labels.size() != n2_labels.size() ) { return false; }

    set<NodePair,compare_NodePairs> next_pairs;

    for( map<SymPair,StateId,compare_SymPairs>::iterator it = 
	   n1_labels.begin();
	 it != n1_labels.end();
	 ++it ) {
      
      SymPair s = it->first;

      if ( n2_labels.find(s) == n2_labels.end() )
	return false;

      StateId target1 = it->second;
      StateId target2 = n2_labels[ s ];
      
      NodePair p;
      p.first = target1;
      p.second = target2;

      if ( t1->Final(target1) != fst::TropicalWeight::Zero() )
	if ( t2->Final(target2) == fst::TropicalWeight::Zero()  )
	  return false;

      next_pairs.insert(p);
	
    }
    
    for ( set<NodePair,compare_NodePairs>::iterator it = next_pairs.begin();
	  it != next_pairs.end();
	  ++it ) {
      if ( handled_nodes.find(*it) == handled_nodes.end() ) {
	handled_nodes.insert(*it);
	if ( not empty_difference(t1,t2,it->first,it->second,handled_nodes) )
	  return false;
      }
    }
    
    return true;

  }

  bool is_subset( TransducerHandle t1, TransducerHandle t2 ) {

    fst::StdVectorFst * pT1 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t1); 
    fst::StdVectorFst * pT2 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t2);
    set<NodePair,compare_NodePairs> handled_nodes;

    NodePair root_nodes;
    root_nodes.first = pT1->Start();
    root_nodes.second = pT2->Start();

    handled_nodes.insert( root_nodes );

    if (pT1->Final(pT1->Start()) == fst::TropicalWeight::Zero() )
      if ( pT2->Final(pT2->Start()) != fst::TropicalWeight::Zero() )
	return false;

    return empty_difference( pT1, pT2, pT1->Start(), pT2->Start(), handled_nodes);

  }

  bool are_equivalent(TransducerHandle t1, TransducerHandle t2) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    return equivalent_(pT1, pT2);
  }






  // *******************************************************************
  // ***  
  // ***  Basic binary Input/Output functions
  // ***  
  // *******************************************************************


  // MOVE TO COMMMON NAMESPACE?
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
    case 'P': { format=0; break; } // sfst properties header
    case 'c': { format=2; break; }
    case -42: { format=1; break; }
    default: { format=-2; break; }
    }
    
    if (char_extracted)
      is.putback(A);
    return format;
  }


  TransducerHandle read_transducer( istream &is ) {
    int c=is.peek();
    if (is.eof())
      return NULL;
    
    fst::StdVectorFst *t;
    
    // 'cin' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);
    
    if (c == 'A') { // read the transducer, skip the alphabet; 
      (void)is.get();
      t = fst::StdVectorFst::Read(is, fst::FstReadOptions());
      BinaryReader reader(is);
      Alphabet *alpha = new Alphabet();
      alpha->read(reader);
      delete alpha;
    }
    else { // read just the transducer
      t = fst::StdVectorFst::Read(is, fst::FstReadOptions());
    }
    pthread_mutex_unlock(&the_mutex);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }

  TransducerHandle read_transducer( const char* filename, KeyTable* kt ) {
	ifstream is(filename);
	TransducerHandle res = read_transducer(is, kt);
	is.close();
	return res;
 }
  
  TransducerHandle read_transducer( /* const */ char* filename ) {
    ifstream is(filename);
    TransducerHandle result = read_transducer(is);
    is.close();
    return result;
  }

  void write_transducer( TransducerHandle t, ostream& os, bool backwards_compatibility ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t); 
    
    // 'cout' and 'cerr' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);
    pT->Write(os, fst::FstWriteOptions());
    pthread_mutex_unlock(&the_mutex);
  } 

  void write_transducer( TransducerHandle t, const char *filename, bool backwards_compatibility ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pT->Write(filename);
  }


  KeyVectorVector * lookup_all(TransducerHandle t,
			       KeyVector * input_string,
			       KeySet * skip_symbols ) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    if (skip_symbols != NULL)
      {
	return find_all_output_strings( *pT, input_string, skip_symbols );
      }
    else
      {
	KeySet ks;
	return find_all_output_strings( *pT, input_string, &ks );
      }
  }
  
  KeyVector * lookup_first(TransducerHandle t,
			   KeyVector * input_string,
			   KeySet * skip_symbols ) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    return find_first_output_string( *pT, input_string );
  }

  KeyVector * longest_match_lookup( TransducerHandle t,
				    KeyVector * input_string ) {
    fst::StdVectorFst * tokenizer = 
      HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    return longest_prefix_tokenization(*tokenizer,
				       input_string);
  }


  // *******************************************************************
  // ***  
  // ***  Reading symbol strings and transducers
  // ***  
  // *******************************************************************

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
    fst::StdVectorFst * multi_char_syms = 
      HANDLE_TO_PINSTANCE(fst::StdVectorFst,multicharacter_symbol_sequences);
    
    // Construct the universal language of utf-8-symbol keys, which make up 
    // the multi-character symbols.
    TransducerHandle universal = 
      PINSTANCE_TO_HANDLE(Transducer,
			  get_trie_universal_language(*multi_char_syms));

    // Get an unused key, which may be used as the marker of the tokenizer.
    Symbol s = define_symbol("HWFST::LONGEST::MATCH");
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
			  multicharacter_symbol_maker( *multi_char_syms, 
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
    Key new_k = 0;
    for (Key k = 0; k < key_table->get_unused_key(); ++k)
      {
	if (key_table->is_key(k))
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
    fst::StdVectorFst * multi_char_symbol_sequences =
      split_symbols(keys,kt,TheAlphabet);

    fst::StdVectorFst * symbols_to_sequences =
      multicharacter_symbol_maker( *multi_char_symbol_sequences,
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


  typedef std::set<HWFST::Key> InputKeySet;
  typedef std::map<HWFST::Key,HWFST::Key> KeyTranslationMap;
  typedef std::set<HWFST::Symbol> SymbolSet;
  
  void add_input_symbols(StateId n,
			 InputKeySet * input_symbols,
			 StateIdSet &visited_nodes,
			 TransduceR * tr)
  {
    for(ArcIterator aiter(*tr,n);
	not aiter.Done();
	aiter.Next())
      {
	StdArc a = aiter.Value();
	input_symbols->insert(a.ilabel);
	if (visited_nodes.find(a.nextstate) == visited_nodes.end())
	  {
	    visited_nodes.insert(a.nextstate);
	    add_input_symbols(a.nextstate,
			      input_symbols,
			      visited_nodes,
			      tr);
	  }
      }
  }
  
  const HWFST::Symbol EPSILON = 0;
  
  InputKeySet * get_input_keys(TransduceR * t)
  {
    InputKeySet * input_key_set = new InputKeySet;
    input_key_set->insert(EPSILON);
    StateIdSet visited_nodes;
    add_input_symbols(t->Start(),
		      input_key_set,
		      visited_nodes,
		      t);
    return input_key_set;
  }
  
  SymbolSet * hwfst_get_symbols(InputKeySet * input_keys, HWFST::KeyTable * kt)
  {
    SymbolSet * input_symbols = new SymbolSet;
    for (InputKeySet::iterator it = input_keys->begin();
	 it != input_keys->end();
	 ++it)
      {
	HWFST::Key k = *it;
	input_symbols->insert(HWFST::get_key_symbol(k,kt));
      }
    return input_symbols;
  }
  
  HWFST::KeyTable * hwfst_lift_input_keys(HWFST::KeyTable * kt,
					  InputKeySet * input_keys)
  {
    SymbolSet * input_symbols = hwfst_get_symbols(input_keys,kt);
    HWFST::KeyTable * new_kt = HWFST::create_key_table();
    HWFST::Key unused_key = 0;
    
    SymbolSet only_output_symbols;
    HWFST::Symbol last_symbol = 0;
    
    for (SymbolSet::iterator it = input_symbols->begin();
	 it != input_symbols->end();
	 ++it)
      {
	HWFST::Symbol s = *it;
	while (last_symbol < s)
	  {
	    if ( HWFST::is_symbol(last_symbol,kt) )
	      {
		only_output_symbols.insert(last_symbol);
	      }
	    ++last_symbol;
	  }
	HWFST::associate_key(unused_key,new_kt,s);
	++last_symbol;
	++unused_key;
      }
    delete input_symbols;
    for (SymbolSet::iterator it = only_output_symbols.begin();
       it != only_output_symbols.end();
	 ++it)
      {
	HWFST::Symbol s = *it;
	HWFST::associate_key(unused_key,new_kt,s);
	++unused_key;
      }
  return new_kt;
  }
  
  HWFST::KeyTable * reorder_key_table(HWFST::KeyTable * kt, TransduceR * t)
  {
    InputKeySet * input_keys = get_input_keys(t);
    HWFST::KeyTable * new_kt = hwfst_lift_input_keys(kt, input_keys);
    delete input_keys;
    return new_kt;
  }
  
  void write_runtime_transducer(TransducerHandle input, 
				KeyTable * key_table, 
				FILE * outstream) {
    TransduceR * t = HANDLE_TO_PINSTANCE(TransduceR,input); 
    HWFST::KeyTable * new_kt = reorder_key_table(key_table,t);
    HWFST::TransducerHandle new_input =
      HWFST::harmonize_transducer(input,key_table,new_kt);
    HWFST::delete_transducer(input);
    t = HANDLE_TO_PINSTANCE(TransduceR,new_input); 
    HwfstFst fst(t,
		 new_kt,
		 outstream);
  };

  TransducerHandle read_runtime_transducer( FILE * f) {
    Alphabet alpha;
    RuntimeReader r(f,alpha);
    Transducer * t = r.get_result();
    return PINSTANCE_TO_HANDLE(Transducer, t); 
  }
  
  /* Miikka Silfverberg will do this */
  KeyVector * tokenize_string( TransducerHandle tokenizer, 
			       const char * string, 
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
    return NULL;
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
    fst::StdVectorFst * pT = 
      HANDLE_TO_PINSTANCE(fst::StdVectorFst,tokenizer);
	
    KeyPairVector * tokenized_string = 
      longest_prefix_tokenize_pair(*pT,
				   string_vector1,
				   string_vector2);
    delete string_vector1;
    delete string_vector2;
    return tokenized_string;
  }
  
  /* Miikka Silfverberg will do this */
  /*TransducerHandle tokenize_pair_string( TransducerHandle t, char * input, KeyTable *T ) {
    assert(false);
    };*/

  /* Miikka Silfverberg will do this */
  KeyPairVector * tokenize_string_pair( TransducerHandle tokenizer, 
					 const char * i1, 
					 const char * i2, 
					 KeyTable *T ) {
    fst::StdVectorFst * tr = HANDLE_TO_PINSTANCE(fst::StdVectorFst,tokenizer);
    KeyVector *  input_string = get_key_string(i1, T, TheAlphabet);
    KeyVector * output_string = get_key_string(i2, T, TheAlphabet);
    if ( (input_string == NULL) or (output_string == NULL) ){
      delete input_string;
      delete output_string;
      return NULL;
    }
    KeyPairVector * result = NULL;
    result = tokenize_stringpair_to_vector(*tr,input_string,output_string);
    delete input_string;
    delete output_string;
    return result;
  };


  /* Transducers t1 and and t2 are both tries. Copy from transducer t2 arc arc2
     and all states and arcs that can be reached from state state_id2 through
     arc2 to transducer t1 so that the copy of arc2 leaves from state state_id1. */

  void add_tree( fst::StdVectorFst *t1, StateId state_id1,
		 fst::StdVectorFst *t2, StateId state_id2, fst::StdArc arc2 ) {

    StateId new_state1 =  t1->AddState();
    t1->AddArc( state_id1,
		fst::StdArc( arc2.ilabel,
			     arc2.olabel,
			     0,
			     new_state1 ) );  // weights only in final states in tries?

    //if ( is_final(t2, arc2.nextstate) )
    //  t1->SetFinal( new_state1, t2->Final(arc2.nextstate).Value() ); 

    t1->SetFinal( new_state1, fst::Plus( t2->Final(arc2.nextstate), t1->Final(new_state1) ) );
    
    for(fst::ArcIterator<fst::StdFst> aiter(*t2, arc2.nextstate); !aiter.Done(); aiter.Next() )
      add_tree( t1, new_state1, t2, arc2.nextstate, aiter.Value() );
  }

  void check_state_and_arcs( fst::StdVectorFst *t1, StateId state_id1,
			     fst::StdVectorFst *t2, StateId state_id2  ) {

    //if ( is_final( t2, state_id2 ) )
    //  t1->SetFinal( state_id1, t2->Final(state_id2).Value() );

    t1->SetFinal( state_id1, fst::Plus( t2->Final(state_id2), t1->Final(state_id1) ) );

    for (fst::ArcIterator<fst::StdFst> aiter2(*t2, state_id2); !aiter2.Done(); aiter2.Next()) {
      const fst::StdArc &arc2 = aiter2.Value();
      bool matching_arc_found = false;
      StateId next_state1 = 0;

      for (fst::ArcIterator<fst::StdFst> aiter1(*t1, state_id1); !aiter1.Done(); aiter1.Next())  {
	const fst::StdArc &arc1 = aiter1.Value();
	if ( arc1.ilabel == arc2.ilabel &&
	     arc1.olabel == arc2.olabel ) { // weights only in final states
	  matching_arc_found = true;
	  next_state1 = arc1.nextstate;
	  break;
	}
      }

      if (matching_arc_found)
	check_state_and_arcs( t1, next_state1, t2, arc2.nextstate );
      else
	add_tree( t1, state_id1, t2, state_id2, arc2 );
    }
  }


  fst::StdVectorFst *disjunct_transducers_as_tries( fst::StdVectorFst *t1, fst::StdVectorFst *t2 ) {
    check_state_and_arcs( t1, t1->Start(), t2, t2->Start() );
    delete t2;
    return t1;
  }

  // TEST IF THIS WORKS WITH EMPTY TRANSDUCER !!!
  TransducerHandle disjunct_transducers_as_tries( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst *pT1 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t1 );
    fst::StdVectorFst *pT2 = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t2 );
    return PINSTANCE_TO_HANDLE( Transducer, disjunct_transducers_as_tries( pT1, pT2 ) );
  }

  /*TransducerHandle insert_pair_string( TransducerHandle t, char *s, KeyTable *T ) {
    return NULL;
    }

  TransducerHandle read_words( char *filename, KeyTable *T, TransducerHandle tokenizer ) {
  return NULL;
    }*/

  TransducerHandle disjunct_as_trie( TransducerHandle t, KeyVector * key_string, float weight, bool sum_weights ) {
    fst::StdVectorFst * pT = 
      HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    add_key_string(key_string,*pT,weight,sum_weights);
    return t;
  }

  TransducerHandle disjunct_as_trie( TransducerHandle t, 
				     KeyPairVector * key_string,
				     float weight,
				     bool sum_weights ) {
    fst::StdVectorFst * pT = 
      HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    add_key_pair_string(key_string,*pT,weight,sum_weights);
    return t;
  }

  fst::SymbolTable *key_table_to_ofst_symbol_table(KeyTable *T) {
    fst::SymbolTable *symbols = new fst::SymbolTable("unnamed");
    vector< pair<char*,Key> > chars_and_keys; 
    if (T) {
      //fprintf(stderr, "KeyTable T is now:\n");
      //T->print(TheAlphabet);
      chars_and_keys = T->to_text_format(TheAlphabet);
    }
    else // use TheAlphabet
      chars_and_keys = TheAlphabet.to_text_format();
    for (unsigned int i=0; i<chars_and_keys.size(); i++) {
      //fprintf(stderr, " ..adding symbol %s to key %hu..\n", chars_and_keys[i].first, chars_and_keys[i].second);
      symbols->AddSymbol(chars_and_keys[i].first, chars_and_keys[i].second);
    }
    return symbols;
  }

  /*KeyTable *ofst_symbol_table_to_key_table(fst::SymbolTable *symbols) {
    hash_map<string, int64> mapping = symbols->GetHashMap();
    vector< std::pair<char*,Key> > vector_mapping;
    for (hash_map<string, int64>::const_iterator it = mapping.begin(); it!=mapping.end(); it++) {
      std::pair<char*, Key> a_pair = std::pair<char*, Key>(strdup(it->first.c_str()), it->second); 
      vector_mapping.push_back(a_pair);
    }
    KeyTable *result = new KeyTable(vector_mapping, TheAlphabet);
    return result;
    }*/

  KeyTable *ofst_symbol_table_to_key_table(fst::SymbolTable *symbols) { 
    vector< std::pair<char*,Key> > vector_mapping;
    for (fst::SymbolTableIterator it(*symbols); !it.Done(); it.Next()) {
      std::pair<char*, Key> a_pair = std::pair<char*, Key>(strdup(it.Symbol()), it.Value()); 
      vector_mapping.push_back(a_pair);
    }
    KeyTable *result = new KeyTable(vector_mapping, TheAlphabet);
    return result;
  }

  // Transform the input symbol table of t to KeyTable and set the input and output
  // symbols tables of t unspecified. If no input symbol table is defined, return NULL;
  KeyTable *to_hwfst_compatible_format( TransducerHandle t ) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    KeyTable *T;
    fst::SymbolTable *symbols = pT->InputSymbols();
    if (!symbols)
      T = NULL;
    else
      T = ofst_symbol_table_to_key_table(symbols);
    pT->SetInputSymbols(NULL);
    pT->SetOutputSymbols(NULL);
    return T;
  }

  void set_ofst_symbol_table(TransducerHandle t, KeyTable *T, bool input) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst,t);
    fst::SymbolTable *table = key_table_to_ofst_symbol_table(T);      
    if (input)
      pT->SetInputSymbols(table);
    else
      pT->SetOutputSymbols(table);      
  }
  
  bool has_alphabet(istream& is);

  TransducerHandle read_transducer_not_harmonize( istream &is, KeyTable *T )
  {
    if (is.eof())
      return NULL;

    // read the transducer and the alphabet
    fst::StdVectorFst *t;
    
    // 'cin' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);

    if (!has_alphabet(is)) { // read just the transducer
      t = fst::StdVectorFst::Read(is, fst::FstReadOptions());     
    }
    else { // read the transducer and the alphabet;
      is.get(); // remove 'A' from stream (indicates that there is alphabet)
      t = fst::StdVectorFst::Read(is, fst::FstReadOptions());     
      BinaryReader reader(is);
      Alphabet *alpha_t = new Alphabet();
      alpha_t->read(reader);
      //fprintf(stderr, "read_transducer: alpha_t is now:\n");
      //alpha_t->print();
      //fprintf(stderr, "read_transducer: to_key_table\n");
      KeyTable *T_read = alpha_t->to_key_table( TheAlphabet );
      //fprintf(stderr, "read_transducer: to_key_table done:\n");
      //T->print(TheAlphabet);
      T->merge(T_read);
      delete T_read;
      delete alpha_t;
    }
    
    pthread_mutex_unlock(&the_mutex);

    return PINSTANCE_TO_HANDLE(Transducer, t);
  }


  TransducerHandle read_transducer( istream &is, KeyTable *T ) {
    int c=is.peek();
    if (is.eof())
      return NULL;
    if (c != 'A') {
      //throw "Cannot harmonize a transducer that has no symbol table.";
      return read_transducer(is);
    }
    
    // read the transducer and the alphabet
    fst::StdVectorFst *t;
    
    // 'cin' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);
    
    (void)is.get();
    t = fst::StdVectorFst::Read(is, fst::FstReadOptions());
    BinaryReader reader(is);
    Alphabet *alpha_t = new Alphabet();
    alpha_t->read(reader);
    
    pthread_mutex_unlock(&the_mutex);

    // no need to harmonize
    if (T && T->is_empty()) {
      KeyTable *table = alpha_t->to_key_table(TheAlphabet);
      //fprintf(stderr, "KeyTable *table:\n");
      //table->print(TheAlphabet);
      T->merge(table);
      //fprintf(stderr, "KeyTable *T:\n");
      //T->print(TheAlphabet);
      delete table;
      return PINSTANCE_TO_HANDLE(Transducer, t);  
    }

    // harmonize the alphabet (secondary names are ignored)
    if (T) {
      KeyTable *table_t = alpha_t->to_key_table(TheAlphabet);
      t = harmonize( t, table_t, T, TheAlphabet );
      delete table_t;
    }
    else {
      t = harmonize_sfst( t, *alpha_t, TheAlphabet );
    }

    delete alpha_t;

    // If a symbol is not used in transitions of the transducer and not in T
    // it will  from T. This might be a problem with epsilon, so it is added.
    //if ( alpha.code2symbol(Label::epsilon) == NULL ) {
    //  const char *epsname = alpha_t->code2symbol(Label::epsilon);
    //  if (epsname != NULL)
    //	alpha.add_symbol(epsname, Label::epsilon);
    //}

    // add to the key table and the global symbol table 
    // primary symbols that are used in transducer tr
    // but not mentioned in the key table
    //if (true) {
    //  alpha.add_primary_symbols( T, TheAlphabet );
    //}    
    return PINSTANCE_TO_HANDLE(Transducer, t);  
  }

  TransducerHandle harmonize_transducer( TransducerHandle t, KeyTable *T_old, KeyTable *T_new ) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t);
    fst::StdVectorFst *pResult = harmonize( pT->Copy(), T_old, T_new, TheAlphabet );
    return PINSTANCE_TO_HANDLE(Transducer, pResult);  
  }


  TransducerHandle read_transducer( /*const*/ char *filename, KeyTable *T ) {
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
      return true;
    return false;
  };

  bool has_symbol_table(istream &is) {
    return has_alphabet(is);
  }

  bool has_ofst_input_symbol_table(TransducerHandle t) {
    fst::StdVectorFst * pT = HANDLE_TO_PINSTANCE( fst::StdVectorFst, t);
    return (pT->InputSymbols() != NULL);
  }

  // For hfst-calculate: read transducer from file 'filename'
  // and if it has an alphabet, harmonize the transducer
  // according to TheAlphabet.

  TransducerHandle read_transducer_and_harmonize( char *filename ) {
    TransducerHandle result;
    ifstream is(filename);
    if ( has_alphabet(is) )
      result = read_transducer(is, NULL);  // KeyTable == NULL -> harmonize according to TheAlphabet
    else
      result = read_transducer(is);
    is.close();
    return result;
  };





  // *******************************************************************
  // ***  
  // ***  Writing symbol strings and transducers
  // ***  
  // *******************************************************************


  vector<char*> paths_to_strings(PathVector pv, vector<float> *final_weights,
				 KeyTable *T, bool spaces);

  /* Retrieve all paths from initial to final state in transducer t in text format.
     The parameter spaces indicates whether the print names of symbol pairs are separated by spaces.      
     For example, a transducer <tt>[a b:c d]</tt> yields "ab:cd" if spaces == false,
     and "a b:c d" if spaces == true.
     An empty transducer yields an empty vector, an epsilon transducer a vector with one
     element, i.e. "<>".
     Returned vector elements are arrays, so they must be deleted with 'delete []buf', where char* buf
     points to an element in the vector.
     @pre \a t is not cyclic.
     @post \a t is not deleted. */
  vector<char*> string_paths( TransducerHandle t, KeyTable *T, bool spaces ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    vector<float> *final_weights = new vector<float>();
    PathVector *pv = find_paths(pT, final_weights);
    return paths_to_strings(*pv, final_weights, T, spaces);
  }


  void print_fst( fst::StdVectorFst *t, KeyTable *T, bool print_weights, ostream& ostr, bool use_numbers );
  void print_fst_old(fst::StdVectorFst *t, KeyTable *T, ostream& ostr);
  
}

namespace HWFST {

  void print_properties( TransducerHandle t, ostream &os ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::FstInfo<fst::StdArc> fstinfo(*pT,true);
    fst::PrintFstInfo<fst::StdArc>(fstinfo, os); 
  }

  void print_transducer( TransducerHandle t, KeyTable *T, bool print_weights, 
			 ostream& ostr, bool old ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    if (old)
      print_fst_old(pT, T, ostr);
    else
      print_fst(pT, T, print_weights, ostr, false);
  }

  void print_transducer_number( TransducerHandle t, bool print_weights, ostream& ostr ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    print_fst(pT, NULL, print_weights, ostr, true);
  }

  void print_state(fst::StdVectorFst *t, StateId state_id, StateId initial_state_id, Alphabet &alpha, bool print_weights, ostream& ostr, bool use_numbers) {

    StateId state_id_printed=state_id;
    // if initial state is not numbered as zero,
    // swap the numbers of initial state and state number zero
    if (initial_state_id != 0) {
      if (state_id == initial_state_id)
	state_id_printed=0;
      else if (state_id == 0)
	state_id_printed=initial_state_id;
    }
    if (is_final(t, state_id)) {  // a final state
      ostr << state_id_printed;
      if (print_weights) {
	ostr << "\t" << (t->Final(state_id)).Value();  // changed to float
      }
      ostr << "\n";
    }
    for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      StateId dest_id = arc.nextstate;
      StateId dest_id_printed=dest_id;
      if (initial_state_id != 0) {
	if (dest_id == initial_state_id)
	  dest_id_printed=0;
	else if (dest_id == 0)
	  dest_id_printed=initial_state_id;
      }
      
      if (!use_numbers) {
	const char *ilabel = alpha.code2symbol(arc.ilabel);
	const char *olabel = alpha.code2symbol(arc.olabel);	  
	ostr << state_id_printed << "\t" << dest_id_printed << "\t";
	if (ilabel)
	  COMMON::escape_and_print(ilabel, ostr, true, false, false);
	else
	  ostr << "\\" << arc.ilabel;
	ostr << "\t";
	if (olabel)
	  COMMON::escape_and_print(olabel, ostr, true, false, false);
	else
	  ostr << "\\" << arc.olabel;
      }
      else
	ostr << state_id_printed << "\t" << dest_id_printed << "\t" << arc.ilabel << "\t" << arc.olabel;
      if (print_weights) {
	ostr << "\t" << (arc.weight).Value();
      }
      ostr << "\n";
    }
  }

  void print_fst( fst::StdVectorFst *t, KeyTable *T, bool print_weights, ostream& ostr, bool use_numbers ) {    

    if(!t) {
      fprintf(stderr, "'print_fst': Transducer is NULL.\n");
      return;
    }    
    Alphabet alpha;
    if (T != NULL) {
      alpha = T->to_alphabet(TheAlphabet);
    }
    else {
      alpha.copy(TheAlphabet);
    }
    
    // 'cout' and 'cerr' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);
    
    // make sure that initial state is number zero
    StateId initial_state_id = t->Start();

    print_state(t, initial_state_id, initial_state_id, alpha, print_weights, ostr, use_numbers);

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      if (state_id != initial_state_id)
	print_state(t, state_id, initial_state_id, alpha, print_weights, ostr, use_numbers);
    }
    pthread_mutex_unlock(&the_mutex);
  }




  void print_fst_old(fst::StdVectorFst *t, KeyTable *T, ostream& ostr) {
    if(!t) {
      fprintf(stderr, "'print_fst': Transducer is NULL.\n");
      return;
    }
    if(!T) {
      fprintf(stderr, "'print_transducer': KeyTable is NULL. Number format not supported in old sfst format.\n");
      return;
    }    
    Alphabet alpha = T->to_alphabet(TheAlphabet);

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      if (is_final(t, state_id)) {
	ostr << "final\t";
	ostr << state_id << "\n";
      }
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	const char *ilabel = alpha.code2symbol(arc.ilabel);
	const char *olabel = alpha.code2symbol(arc.olabel);
	StateId dest_id = arc.nextstate;
	ostr << state_id << "\t";
	COMMON::escape_and_print(ilabel, ostr, true, true, true);
	ostr << ":";
	COMMON::escape_and_print(olabel, ostr, true, true, true);
	ostr << "\t" << dest_id << "\n";
      }
    }
  }



  void paths_to_print(PathVector &pv, vector<float> *final_weights, FILE *outfile, KeyTable *T, bool escape, bool spaces);

  /* Print all paths from initial to final state in transducer t to FILE outfile. */
  void print_transducer_paths( TransducerHandle t, FILE *outfile, KeyTable *T ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

    vector<float> *final_weights = new vector<float>();
    PathVector *pv = find_paths(pT, final_weights, false);
    if (pv == NULL) { // cyclic transducer
      printf("ERROR: print_fst_paths: argument transducer is cyclic.\n");
      exit(1);
    }
    paths_to_print(*pv, final_weights, outfile, T, true, false);
  }

  void complete_alphabet( fst::StdVectorFst *t, Alphabet& alpha );

  /* Write t in binary form to output stream os.
     symbols indicates the name of the key table that is stored with the transducer. */
  void write_transducer( TransducerHandle t, KeyTable *T, ostream& os, bool backwards_compatibility ) {
    if (backwards_compatibility) {
      set_ofst_symbol_table(t, T, true);
      set_ofst_symbol_table(t, T, false);
      fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

      // 'cout' and 'cerr' must be protected because OpenFst uses library 'pthread'
      pthread_mutex_t the_mutex;
      #ifdef _PTHREADS_DRAFT4
      pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
      #else
      pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
      #endif

      pthread_mutex_lock(&the_mutex); 

      pT->Write(os, fst::FstWriteOptions());

      pthread_mutex_unlock(&the_mutex);
      return;
    }

    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t); 
    
    Alphabet t_alphabet;
    if (T)
      t_alphabet = T->to_alphabet(TheAlphabet);
    else
      t_alphabet.copy(TheAlphabet);
    complete_alphabet(pT, t_alphabet);

    // 'cout' and 'cerr' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif

    pthread_mutex_lock(&the_mutex);

    os.put('A');
    pT->Write(os, fst::FstWriteOptions());
    t_alphabet.store(os);

    pthread_mutex_unlock(&the_mutex);
    //delete t; // pT?
  }


  /* Write transducer t to file filename. */
  void write_transducer( TransducerHandle t, const char *filename, KeyTable *T, bool backwards_compatibility ) {
    if (backwards_compatibility)
      throw "ERROR: backwards compatibility is not possible if a symbol table is written with an OpenFst transducer";
    ofstream os(filename);
    write_transducer( t, T, os );
    os.close();
  }















  // *******************************************************************
  // ***  
  // ***  Miscellaneous functions not visible in the API
  // ***  
  // ******************************************************************* 



  /* Get the number of arcs in Transducer_ t. */
  long arcs(fst::StdVectorFst *t) {
    long arcs = 0;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	arcs++;
      }
    }
    return arcs;
  }

  /* Get the number of states in Transducer_ t. */
  long states(fst::StdVectorFst *t) {
    long states = 0;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next())
      states++;      
    return states;
  }

  /* Test if state 'state_id' in Transducer_ t is final. */
  bool is_final(fst::StdVectorFst *t, StateId state_id) {
    fst::StdVectorFst::Weight final = t->Final(state_id);
    if (final != fst::StdVectorFst::Weight::Zero())
      return true;
    return false;
  }
 
  void print_paths(PathVector pv);
  

  /* Redefine the alphabet. */

  void change_pair_set( Alphabet& alpha, KeyPairSet& Pi ) {
    alpha.clear_char_pairs();
    //alpha.insert_symbol_pairs( Pi );   not defined !!!!!!
  }

  



  /* \brief Write transducer text form separator "--\n" to ostream \a os.

  More on transducer text forms in #read_transducer_text. */
  void write_separator(ostream& os) {

    // 'cout' and 'cerr' must be protected because OpenFst uses library 'pthread'
    pthread_mutex_t the_mutex;
    #ifdef _PTHREADS_DRAFT4
    pthread_mutex_init(&the_mutex, pthread_mutexattr_default);
    #else
    pthread_mutex_init(&the_mutex, (pthread_mutexattr_t *)NULL);
    #endif
    pthread_mutex_lock(&the_mutex);
    os << "--\n";
    pthread_mutex_unlock(&the_mutex);
  }
  

  /* new version of 'make_fst' */
  fst::StdVectorFst *makeFst(char *str) {
    vector<Label> *string_labels = new vector<Label>();
    TheAlphabet.string2labelseq(str, *string_labels);
    
    fst::StdVectorFst *result = make_empty_();
    StateId current_state = 0;
    
    for (unsigned int i=0; i<string_labels->size(); i++) {
      Label l = (*string_labels)[i];      
      StateId new_state = result->AddState();
      result->AddArc(current_state, fst::StdArc(l.lower_char(), l.upper_char(), 0, new_state));
      current_state = new_state;
    }
    result->SetFinal(current_state, 0);
    string_labels->clear();
    delete string_labels;
    return result;
  }


  /* Make a transducer defined in \a str.

  \a str can contain multicharacter symbols, colons and unicode characters. 
  For example, 'a:bcd:<tag>Ã¶' is interpreted as 'a:b c:c d:<tag> ä:ä' (small a with two dots). 
  Invalid values of str (e.g. 'ab:') can result in an unexpected return value. 

  @pre All symbols in \a str must be defined in the alphabet. */
  
  TransducerHandle make_fst(char *str) {
    fst::StdVectorFst* const pResult = makeFst(str);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  vector<Label> *string2labelvector(char *str) {

    //fst::StdVectorFst *result = NULL;
    vector<Label> *result = new vector<Label>();
    int ilabel=0;                // if 'a:b', a is stored in ilabel
    bool find_olabel = false;  // after ':' gets value true
    
    for(int i=0;; i++) {
      char c = str[i];
      char *symbol=NULL;  // pointer to a symbol just read

      // ----- end of string -----
      if (c == '\0')
	break;

      // ----- multicharacter symbol -----
      if (c == '<') { 
	char multichar[100];
	int j=0;
	while (str[i] != '>') {
	  multichar[j] = str[i];
	  i++;
	  j++;
	}
	multichar[j] = str[i];
	char *c_ = &(multichar[0]);
	c_[j+1] = '\0';
	symbol = c_;
      }

      // ----- ordinary single-byte symbol -----
      else if (!(c & 128)) { // 0xxx xxxx

	char *c_ = &c; // 
	c_[1] = '\0';  // ensure that the length of c_ is 1
	symbol = c_;
      }

      // ----- ordinary multi-byte symbol -----
      else { // 1xxx xxxx
	if (c & 64) { // 11xx xxxx
	  if (!(c & 32)) { // 110x xxxx  // ----- two-byte symbol -----
	    char two_char[2];
	    two_char[0] = str[i];
	    i++;
	    two_char[1] = str[i];
	    char *two_char_ = &(two_char[0]);
	    two_char_[2] = '\0';
	    symbol = two_char_;
	  }
	  
	  else { // 111x xxxx
	    if (!(c & 16)) { // 1110 xxxx   // ----- three-byte symbol -----

	      char three_char[3];
	      three_char[0] = str[i];
	      i++;
	      three_char[1] = str[i];
	      i++;
	      three_char[2] = str[i];
	      char *three_char_ = &(three_char[0]);
	      three_char_[3] = '\0';
	      symbol = three_char_;
	    }
	    else { // 1111 xxxx
	      if (!(c & 8)) { // 1111 0xxx  // ----- four-byte symbol -----

		char four_char[4];
		four_char[0] = str[i];
		i++;
		four_char[1] = str[i];
		i++;
		four_char[2] = str[i];
		i++;
		four_char[3] = str[i];
		char *four_char_ = &(four_char[0]);
		four_char_[4] = '\0';
		symbol = four_char_;
	      }
	      else // 1111 1xxx (not allowed in utf-8)
		error("The lexicon file is not valid utf-8! (byte of type 1111 1xxx is not allowed)");
	    }
	  }
	}
	else // 10xx xxxx  // if text is valid utf-8, this case is dealt with already in the previous cases 
	  error("The lexicon file is not valid utf-8! (continuing byte of type 10xx xxxx in wrong place)");
      }

      // ----- the symbol is handled here -----

      if (find_olabel) {       // ----- the symbol just read is an output label
	Label new_label = Label(ilabel, TheAlphabet.symbol2code(symbol));
	result->push_back(new_label);
	find_olabel = false;   // output label has been found
      }
      else {
	if (str[i+1] == ':') { // ----- the symbol just read is an input label
	  ilabel = TheAlphabet.symbol2code(symbol);
	  i++;
	  find_olabel = true;  // output label must be found
	}
	else {                   // ----- the symbol just read is both input and output label
	  Label new_label = Label(TheAlphabet.symbol2code(symbol));
	  result->push_back(new_label);
	}
      }      
    }
    return result;
  }      


  fst::StdVectorFst *get_word( ifstream &is );

  /* Calculates disjunction of words from line a to b in stream is. 
     If the number of words is greater than 100, the function calls itself for both
     halves of words ands returns the disjunction of both results from recursive function calls. */
  fst::StdVectorFst *tree_disjunct(int a, int b, ifstream &is) {
    if ((1+b-a) > 100) {
      int size = (1+b-a)/2;
      fst::StdVectorFst *t1 = tree_disjunct(a, a+size, is);
      fst::StdVectorFst *t2 = tree_disjunct(a+size+1, b, is);
      fst::Union(t1, *t2);
      delete t2;
      return t1;
    }
    fst::StdVectorFst *result = NULL;
    for(int i=a; i<(b+1); i++) {
      fst::StdVectorFst *new_word = get_word(is);
      if (result != NULL && new_word != NULL) {
	fst::Union(result, *new_word);
	delete new_word;
      }
      else if (result == NULL && new_word != NULL)
	result = new_word;
      else if (result != NULL && new_word == NULL)
	break;
      else
	return NULL;
    }
    return result;
  }

  /* Get next line in stream is in transducer form. If end of stream is reached,
     NULL is returned. */
  fst::StdVectorFst *get_word(ifstream &is) {
  

    int n=0;
    char buffer[10000];


    if (is.getline(buffer, 10000)) {
      if (1 && ++n % 10000 == 0) { // 1 (true) was originally verbose
	//if (n == 10000)
	//   fprintf(stderr, "\n");
	//fprintf(stderr, "\r%i words", n);
      }
      // delete final whitespace characters
      int l;
      for( l=strlen(buffer)-1; l>=0; l-- )
	if ((buffer[l] != ' ' && buffer[l] != '\t' && buffer[l] != '\r') ||
	    (l > 0 && buffer[l-1] == '\\'))
	  break;
      buffer[l+1] = 0;
      
      //fprintf(stderr, "read_words: read word %s\n", buffer);
      return makeFst(buffer);
    }
    return NULL;
  }





  /* \brief Add string \a str to transducer \a t. 

  \note
  OpenFst implementation is not yet ready.

  The result is a pair string trie. The result points to \a t after it has
  been added the string \a str.

  @pre All symbols in \a str are defined in the alphabet (input or output symbol in a symbol pair). \a t is either an empty
  transducer (made with #make_empty_fst) or a pair string trie.

  @post \a t is not deleted. \a str is deleted? [at least it cannot be const in SFST] */

  TransducerHandle insert_pair_string(TransducerHandle t, char *str, float weight) {

    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    //vector<Label> *string_labels = string2labelvector(str);
    vector<Label> *string_labels = new vector<Label>();
    TheAlphabet.string2labelseq(str, *string_labels);
    
    typedef fst::StdArc::StateId StateId;
    StateId current_state = pT->Start();
    bool matching_arc = false; // whether matching arc was found in the string tree
    int first_label_index;     // index of first label that must be added to the tree
    
    // how far matching arcs can be found in the string tree
    
    for (unsigned int i=0; i<string_labels->size(); i++) {
      Label l = (*string_labels)[i];
      matching_arc = false;
      
      for (fst::ArcIterator<fst::StdFst> aiter(*pT, current_state); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	if (arc.ilabel == l.lower_char() &&  // matching arc found 
	    arc.olabel == l.upper_char() ) {
	  matching_arc = true;
	  current_state = arc.nextstate;                 // move to next state
	  break;
	}
      }
      if (!matching_arc) { // matching arc not found
	first_label_index = i;
	break;
      }
    }

    if (matching_arc) { // no need to make new branches (all string labels added)
      if (!is_final(pT, current_state))  // if string is not already in tree
	pT->SetFinal(current_state, 0);
      return PINSTANCE_TO_HANDLE(Transducer, pT);;
    }

    // matching arc not found -> make new branch

    for (unsigned int i=first_label_index; i<string_labels->size(); i++) {
      Label l = string_labels->at(i);

      StateId new_state = pT->AddState();
      pT->AddArc(current_state, fst::StdArc(l.lower_char(), l.upper_char(), 0, new_state));
      current_state = new_state;        // move to new state
    }
    pT->SetFinal(current_state, weight);

    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }      
    

  fst::StdVectorFst *add_string(fst::StdVectorFst *t, char *str, float weight=0) {

    fst::StdVectorFst* pT = t;
    //vector<Label> *string_labels = string2labelvector(str);
    vector<Label> *string_labels = new vector<Label>();
    TheAlphabet.string2labelseq(str, *string_labels);
    
    StateId current_state = pT->Start();
    bool matching_arc = false; // whether matching arc was found in the string tree
    int first_label_index;     // index of the first label that must be added to the tree

    // how far matching arcs can be found in the trie

    for (unsigned int i=0; i<string_labels->size(); i++) {
      Label l = string_labels->at(i);
      matching_arc = false;

      for (fst::ArcIterator<fst::StdFst> aiter(*pT, current_state); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	if (arc.ilabel == l.lower_char() &&  // matching arc found 
	    arc.olabel == l.upper_char() ) {
	  matching_arc = true;
	  current_state = arc.nextstate;                 // move to next state
	  break;
	}
      }
      if (!matching_arc) { // matching arc not found
	first_label_index = i;
	break;
      }
    }
    
    if (matching_arc) { // no need to make new branches (all string labels added)
      if (!is_final(pT, current_state))  // if string is not already in tree
	pT->SetFinal(current_state, weight);
      return pT;
    }
    
    // matching arc not found -> make new branch
    
    for (unsigned int i=first_label_index; i<string_labels->size(); i++) {
      Label l = string_labels->at(i);
      
      StateId new_state = pT->AddState();
      pT->AddArc(current_state, fst::StdArc(l.lower_char(), l.upper_char(), 0, new_state));
      current_state = new_state;        // move to new state
    }
    pT->SetFinal(current_state, weight);
    
    return pT;
  }      
  

  /* \brief Minimal disjunction of all words listed in file \a filename.
      
  @pre Words are listed one word per line. Multicharacter symbols
  are enclosed in angle brackets.

  Words can have multicharacter symbols (they must be enclosed in
  angle brackets), utf8 characters and ':' symbols.
  For example a word 'ab:Bc:<><symbol>' is interpreted as a transducer
  that maps string 'abc<symbol>' to string 'aB<symbol>'.

  @param filename The name of the file where words are listed.

  @post \a filename remains unchanged. File \a filename remains unchanged. */

  TransducerHandle read_words( char* filename ) {
    fst::StdVectorFst* const pResult = read_words_(filename);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  
  // OLD VERSION
  fst::StdVectorFst *read_words_( char *filename ) {

    if (Verbose)
      fprintf(stderr,"\nreading words from %s...\n", filename);
    ifstream is(filename);
    if (!is.is_open()) {
      static char message[1000];
      sprintf(message,"Error: Cannot open file \"%s\"!",filename);
      throw message;
    }
    int rows=0;
    char buffer[10000];
    while(is.getline(buffer, 10000))
      rows++;
    is.close();

    ifstream is2(filename);

    /* Disjunction and minimisation are done in recursive way in order to
       avoid minimising the whole resulting transducer every time new words are added.
       Minimisation cannot be done after all words are disjuncted, as disjunction takes
       more time when argument transducers are big. */
    fst::StdVectorFst *result = tree_disjunct(0, rows-1, is2);
    
    is2.close();
    return result;
  }


  typedef vector< stack<fst::StdArc> > PathVector;
  //PathVector *find_paths(Transducer_ *t);

  //static fst::StdVectorFst *replace_char(Transducer_ *t, Character a, Character b);
  //static fst::StdVectorFst *replace_char(Transducer_ *t, Character a, stack<fst::StdArc> b);

  
  /* \brief Replace agreement variables in \a t with all their possible values.
      
  Find out all possible combinations of different agreement variable values in transducer \a t.
  Compute the value of \a t with all combinations by replacing each agreement variable with its 
  value in that combination. Disjunct all the values that \a t can get and minimise the result.

  Examples:

  If  agreement variable <tt>TRANSDUCER = [a | b | c]</tt>  and  transducer <tt>T = [TRANSDUCER X TRANSDUCER]</tt>,
  explode(T) is equal to <BR>
  <tt>[a X a] | [b X b] | [c X c]</tt>

  If  agreement variable <tt>RANGE = Range<a, b, c></tt>  and  transducer <tt>T = [RANGE RANGE:X RANGE]</tt>,
  explode(T) is equal to <BR>
  <tt>[a a:X a] | [b b:X b] | [c c:X c]</tt>

  If  agreement variable <tt>TRANSDUCER = [a b] | c</tt>  and  agreement variable <tt>RANGE = Range<D,E></tt>  and  
  transducer <tt>T = [TRANSDUCER RANGE X <>:RANGE TRANSDUCER]</tt>,
  explode(T) is equal to <BR> 
  <tt>[ D a X <>:a D  |  D b X <>:b D  |  D c X <>:c D  |  E a X <>:a E  |  E b X <>:b E  |  E c X <>:c E ]</tt>

  \note
  In OpenFst implementation, agreement variables are replaced through composition,
  which can make the resulting transducer asynchronous.
  More info in https://kitwiki.csc.fi/twiki/bin/view/KitWiki/OMorFiWithOpenFst.

  @post The resulting transducer is deterministic, minimimal and has
  no epsilon-transitions but might be asynchronous. \a t is deleted.
  */

  TransducerHandle explode( TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* const pResult = explode_(pT);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *explode_( fst::StdVectorFst *t ) {

    if (RS.size() == 0 && RSS.size() == 0)
      return t;    
    fst::StdVectorFst *T = minimize_(t);
    
    vector<char*> name;
    // all names representing a transducer agreement variable 
    for( RVarSet::iterator it=RS.begin(); it!=RS.end(); it++)
      name.push_back(*it);
    RS.clear();
    
    // replace all keys with the corresponding transducer agreement variables
    for( size_t i=0; i<name.size(); i++ ) {

      Key variable_key = TheAlphabet.symbol2code(name[i]);
      fst::StdVectorFst *agreement_transducer = var_value_(name[i]);
      
      // enumerate all paths of the agreement transducer
      vector<fst::StdVectorFst*> paths = find_all_paths_(agreement_transducer, true);
      delete agreement_transducer;

      fst::StdVectorFst *all_substitutions = make_empty_();
      // insert each path
      for( size_t j=0; j<paths.size(); j++ ) {
	
	KeyPair *kp = define_keypair(variable_key, variable_key);
	fst::StdVectorFst *substitution = substitute_with_transducer_(T->Copy(), kp, paths[j]);
	delete kp;
	delete paths[j];
	
	fst::Union(all_substitutions, *substitution);
	delete substitution;
      }
      delete T;
      T = all_substitutions;
    }

    
    name.clear();
    // all names representing a range agreement variable 
    for( RVarSet::iterator it=RSS.begin(); it!=RSS.end(); it++)
      name.push_back(*it);
    RSS.clear();
    
    // replace all keys with the corresponding range agreement variables
    for( size_t i=0; i<name.size(); i++ ) {

      Key variable_key = TheAlphabet.symbol2code(name[i]);
      Range *r=copy_range_variable_value(name[i]);
      
      fst::StdVectorFst *all_substitutions = make_empty_();
      // insert all keys in the agreement range
      while (r != NULL) {	
	fst::StdVectorFst *substitution = substitute_key_( T->Copy(), variable_key, r->character );
	
	fst::Union(all_substitutions, *substitution);
	delete substitution;

	Range *next = r->next;
	delete r;
	r = next;
      }
      delete T;
      T = all_substitutions;
    }
    return T;
  };




  /* \brief Composition of word transducer t1 and model transducer t2.

  TEST VERSION. ONLY IN HWFST. 

  t1 is one-path transducer (representing an input word) and t2 is deterministic,
  minimal and input label sorted (representing the language model).

  @see sort_input
  */

  TransducerHandle word_composition( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst *pResult = new fst::StdVectorFst;
    fst::Compose(*pT1, *pT2, pResult);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  /* \brief Sort the input labels of arcs in transducer t. 

  TEST VERSION. ONLY IN HWFST. */
  TransducerHandle sort_input(TransducerHandle t) {
    fst::StdVectorFst* pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::ArcSort(pT, fst::StdILabelCompare());
    return PINSTANCE_TO_HANDLE(Transducer, pT);
  }










    

  


  


  









  /* \brief Read the symbol table from file \a filename. 

\note
  Function is not included in SFST functions as symbol tables are stored within transducers in SFST. 

  By default, the symbol table is stored in file
  'symbol_table' every time function #write_to_file is called. 

  If binary transducers
  created in program A must be used in program B, call read_symbol_table("symbol_table")
  in beginning of program B. It makes sure that all symbols that are used in both A and B
  are coded similarily.

  Another way to make different programs interoperable is to define all possible symbols 
  in the beginning of every program similarily, for example by defining a Range variable:

  ALL_SYMBOLS = Range<a,b,c ... X,Y,Z>

  @param filename The name of the file where the symbol table has been written.

  @post \a filename remains unchanged. File \a filename remains unchanged. */

  void read_symbol_table( char *filename ) {
    fst::SymbolTable *table = fst::SymbolTable::Read(filename);
    fst::SymbolTableIterator it(*table);
    for(; !it.Done(); it.Next()) {
      //fprintf(stderr, "read_symbol_table: read symbol %s and code %i.\n", it.Symbol(), it.Value());
      char const *c = it.Symbol();
      TheAlphabet.add_symbol(c, it.Value());
    } 
    Alphabet_Defined = true;
  }
      
/* \brief Read symbol table in text format from file filename. */

void read_symbol_table_text( istream& is ) {
  TheAlphabet.read_symbol_table_text( is );
}


  /*  \brief Explode and minimise \a t. \a invert defines whether input and output levels are switched.

       @param t Transducer to be exploded and minimised.
       @param invert A switch flag: whether input and output levels of transducer \a t are switched
       between exploding and minimisation.

       @post \a t is deleted. Transducer and Range variables and their values are deleted. 

       @see explode minimise */  

  TransducerHandle result( TransducerHandle t, bool invert, bool reverse ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* const pResult = result_(pT, invert, reverse);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *result_( fst::StdVectorFst *t, bool invert, bool reverse ) {
    
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
    
    if (invert) // a switch flag
      fst::Invert(t);
    if (reverse)
      fst::Reverse(*t, t);

    // push weights towards initial state
    fst::Push( t, fst::ReweightType(0));
    
    t = minimize_(t);
    return t;
  }
  


  // ------------- functions in 'interface.C' copied as such (they operate with types that are defined as enums or structs) ----------------

  // helping methods declared here
  static Range *append_values( Range *r2, Range *r );
  static Range *copy_values( const Range *r );
  

  /* \brief Define a Range variable \a name with value \a r.
 
  The variable can be a normal or an agreement variable. In SFST Range variables are named 
  '#NAME#' and Range agreement variables '#=NAME#', where NAME is the name of the variable.
  You do not have to use this convention, but make sure that Range agreement variables do not
  get mixed with other symbols. For example, naming a Range agreement variable 'a' or 'agr_var'
  may cause problems if symbol 'a' or multicharacter symbol 'agr_var' are already in use.

  If function has been called earlier with the same \a name argument, the old
  Range value is deleted:

  \code 
  def_svar("VAR", range1);
  
  def_svar("VAR", range2);
  // range1 is now NULL
  
  def_svar("VAR", range3);
  // range2 is now NULL; 
  \endcode
  
  Use #svar_value to get a copy of the value of a normal Range variable and #rsvar_value to get
  a Range representing the value of a Range agreement variable. 
  
  @return Whether r is NULL-valued. A true return value probably indicates
  a mistake in the user's program.
  
  @post \a r is not deleted. \a name remains unchanged. Previous Range value assigned to a variable with the same name is deleted. */
  
  bool define_range_variable( char *name, Range *r ) {
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
  
  /* \brief Add Range \a r to Ranges \a rs.

  @pre r is not NULL.

  @param r Range to be added.
  @param rs Ranges where \a r is added. Can be NULL.

  @post \a r and \a rs are not deleted. 
  Resulting Ranges has pointers to \a r and \a rs.
  
  An example:

  \code
  // a Ranges that contains Range structs Range<a,c> and Range<d,f>, i.e. a Ranges struct Ranges<Range<a,c>, Range<d,f>>.

  Range *r1 = NULL;
  r1 = add_value( character_code('a') , r1 );
  r1 = add_value( character_code('c') , r1 );

  Range *r2 = NULL;
  r2 = add_value( character_code('d') , r2 );
  r2 = add_value( character_code('f') , r2 );

  Ranges *rs = NULL;
  rs = add_range(r1, rs);
  rs = add_range(r2, rs);
  \endcode 
  */

  Ranges *add_range( Range *r, Ranges *rs ) {
    Ranges *result = new Ranges;
    result->range = r;
    result->next = rs;
    return result;
  }

  /* \brief Add Character \a c to Range \a r. 

  @param c Character to be added to Range \a r.
  @param r Range where Character is added. Can be NULL.

  @post \a r is not deleted. The resulting Range has a pointer to \a r. 

  An example:

  \code
  // a Range that contains symbols 'a' and 'c'.
  Range *r = NULL;
  r = add_value( character_code('a') , r );
  r = add_value( character_code('c') , r );
  \endcode
 */

  Range *add_value( Symbol c, Range *r ) {
    //printf("Function 'add_value' started...\n");
    Range *result=new Range;
    result->character = c;
    result->next = r;
    //printf("Function 'add_value' ending, returning value of type 'Range'...\n");
    return result;
  }

  Character character_code( unsigned int c );

  /* \brief Add characters from \a c1 to \a c2, inclusive, to Range \a r. 

  With uft8 characters, call #utf8toint for utf8 character before passing it
  to this function.
  
  Examples:
  \code
  Range r1 = NULL;
  r1 = add_values('c', 'h', r1);
  // r1 now has Characters for symbols c, d, e, f, g and h
  
  r1 = add_values(utf8toint("Ã¤"), utf8toint("Ã§"), r1);
  // r1 now has Characters for symbols c, d, e, f, g, h and unicode characters 'adieresis', 'atilde', 'aring' and 'ccedilla'.
  \endcode
  
  @pre \a c1 is no greater than \a c2.
  
  @param c1 Lower limit for Characters that are added to \a r.
  @param c2 Upper limit for Characters that are added to \a r.
  @param r Range where Characters are added. Can be NULL.
  
  @post \a r is not deleted. The resulting Range has a pointer to \a r. */

  Range *add_values( unsigned int c1, unsigned int c2, Range *r ) {
    for( unsigned int c=c2; c>=c1; c-- )
      r = add_value(character_code(c), r);
    return r;
  }
  
  /* \brief Add a copy of the value of Range variable \a name to Range \a r. 

  @pre Range variable \a name has been assigned a value.

  @param name The name of the Range variable that is added to \a r.
  @param r Range where a copy of the value of Range variable \a is added. Can be NULL.

  @post \a r is not deleted. The resulting Range has a pointer to \a r. \a name is freed. */

  Range *add_var_values( char *name, Range *r ) {
    return append_values(copy_range_variable_value(name), r);
  }
  
  /* \brief A copy of the value of Range variable \a name. 

  @pre Variable \a name is defined.

  @post \a name is freed. */

  Range *copy_range_variable_value( char *name ) {
    SVarMap::iterator it=SVM.find(name);
    if (it == SVM.end())
      error2("undefined variable", name);
    free(name);
    //if (it->second == NULL)
    //printf("svar_value: it->second is NULL!\n");
    return copy_values(it->second);
  }
  
  static Range *append_values( Range *r2, Range *r ) {   // HELPING METHOD
    if (r2 == NULL)
      return r;
    return add_value(r2->character, append_values(r2->next, r));
  }
  
  static Range *copy_values( const Range *r ) {     // HELPING METHOD
    //printf("copy_values: ");
    if (r == NULL) {
      //printf(" return value is NULL\n");
      return NULL;
    }
    return add_value( r->character, copy_values(r->next));
  }

  /* \brief A copy of the value of transducer variable \a name. 

  @pre Variable \a name is defined.

  @post \a name is freed. */

  TransducerHandle copy_transducer_variable_value( char *name ) {
    fst::StdVectorFst* const pResult = var_value_(name);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *var_value_( char *name ) {
    VarMap::iterator it=VM.find(name);
    if (it == VM.end())
      error2("undefined variable", name);
    free(name);
    return (it->second->Copy());  // (originally return &(it->second->copy()); )  StdVectorFst.Copy() used instead
  }
  
  /* \brief A transducer representing a transducer agreement variable \a name. 

  For example, if \a name is 'TR1', returns a transducer mapping multicharacter symbol 'TR1' to itself.

  The actual values of TR1 are computed when #explode is called for a transducer where
  rvar_value_("TR1") is used.

  An example:
  \code
  // value of transducer tr1 is  [ a | b | c ]  and value of tr2 is  [X]
  def_rvar_("TR1", tr1);
  
  TransducerHandle T = catenate_( rvar_value_("TR1"), catenate_(tr2, rvar_value_("TR1")) )
  // value of T is now  [ TR1 X TR1 ]
  
  T = explode(T); 
  // value of T is now  [a X a] | [b X b] | [c X c]
  \endcode
  
  @post \a name is freed. */
  
  TransducerHandle copy_transducer_agreement_variable_value( char *name ) {
    fst::StdVectorFst* const pResult = rvar_value_(name);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  fst::StdVectorFst *rvar_value_( char *name ) {
    if (RS.find(name) == RS.end())
      RS.insert(strdup(name));
    Range *r=add_value(symbol_code(name), NULL); // symbol_code frees
    return new_transducer_(r,r);
  }

  /* \brief A Range representing Range agreement variable \a name. 

  For example, if \a name is 'RANGE1', returns a Range with one multicharacter symbol 'RANGE1'.

  The actual values of RANGE1 are computed when #explode is called for a transducer where
  rsvar_value("RANGE1") is used. 

  An example:
  \code
  // r1 has earlier been assigned value Range<a,b,c>
  def_svar("RANGE1", r1);
  fst::StdVectorFst *tr = new_transducer_(rsvar_value("RANGE1") , add_value(character_code('X'), NULL) );
  // value of tr is now [ RANGE1:X ]
  
  tr = catenate_(tr, new_transducer_(add_value(character_code('Y'), NULL), rsvar_value("RANGE1")));
  // value of tr is now [ RANGE1:X Y:RANGE1 ]
  
  tr = explode_(tr);
  // value of tr is now [a:X Y:a] | [b:X Y:b] | [c:X Y:c] 
  \endcode
  
  
  @post \a name is freed. */
  Range *copy_range_agreement_variable_value( char *name ) {
    if (RSS.find(name) == RSS.end())
      RSS.insert(strdup(name));
    return add_value(symbol_code(name), NULL); // symbol_code frees
  }

  
  /* \brief Complement Range of \a r. 

  Complement Range of \a r contains all Characters defined in the alphabet
  that can be mapped onto themselves and are not included in \a r.
  For example, if the alphabet is defined as
  <tt>[ a | a:b | b | b:c | c | d:e | e ]</tt>, complement range of 
  <tt>Range<a, b></tt> is <tt>Range<c, e></tt>.

  @pre The alphabet is defined. Resulting Range cannot be empty, i.e.
  Range \a r cannot contain all possible symbols. 

  @param r If NULL, a Range containing all symbols defined in the alphabet is returned. 

  @post \a r is deleted. */

  Range *complement_range( Range *r, KeySet *Si ) {  // defined in HFST
    
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
  


  // *** hfst-calculate functions ***

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


  Character symbol_code( char *symbol ) {
    int c=TheAlphabet.symbol2code(symbol);
    if (c == EOF) {
      c = TheAlphabet.add_symbol( symbol );
    }
    free(symbol);
    return (Character)c;
  }

  Character character_code( unsigned int c ) {    
    if (UTF8)
      return symbol_code(strdup(int_to_utf8(c)));    
    unsigned char *buffer=(unsigned char*)malloc(2);
    buffer[0] = (unsigned char)c;
    buffer[1] = 0;
    return symbol_code((char*)buffer);
  }




  /* Legacy function for hfst-calculate. Collects all symbols and pairs in
     transducer t and adds them to TheAlphabet. */

  void def_alphabet( fst::StdVectorFst *t ) {
    fst::RmEpsilon(t);
    //t = explode_(t);
    t = minimize_(t);
    TheAlphabet.clear_char_pairs();

    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	const char *ilabel = TheAlphabet.code2symbol(arc.ilabel);
	const char *olabel = TheAlphabet.code2symbol(arc.olabel);
	Character ic = TheAlphabet.add_symbol(ilabel);
	Character oc = TheAlphabet.add_symbol(olabel);
	Label new_label = Label(ic, oc);
	TheAlphabet.insert(new_label);  // UNINITIALIZED?
      }
    }    
    Alphabet_Defined = 1;
    delete t;
    // symbol_table not written
  }
  


  /* \brief Character code for \a symbol. 
      \a symbol is a multicharacter symbol or a utf8 character longer than one byte.

      \a symbol does not need to be defined in alphabet. If \a symbol has never been used before
      (i.e. #character_code or #symbol_code has not been called with argument equal to \a symbol),
      a new character code is chosen for \a symbol.

  In SFST, multicharacter symbols are enclosed in angle brackets. You do not have to use this convention
  but make sure that multicharacter symbols and agreement variables do not get mixed. For example,
  a multicharacter symbol 'symb' and a transducer or Range agreement variable 'symb' will get mixed.
  Naming them instead '<symb>' and '$=symb$' or '#=symb#' will not cause problems.

  This function can also be used to get 
  Character code for epsilon with argument '<>'. More on epsilon in #Character.
  With utf8 characters, call 'strdup' for argument symbol before passing it to #symbol_code:
  symbol_code(strdup(UTF8CHARACTER)); 

  Examples:
  \code 
  char *SYMB = "multicharacter_symbol";
  char *symb = (char*) malloc(22);
  for (int i=0; i<22; i++)
  symb[i] = SYMB[i];
  Character symbol = symbol_code(symb);
  
  char *eps = (char*) malloc(3);
  eps[0] = '<';
  eps[1] = '>';
  eps[2] = '\0';
  Character epsilon = symbol_code(eps);
  
  char *auml = (char*) malloc(3);
  auml[0] = 'Ã';
  auml[1] = '¤';
  auml[2] = '\0';
  Character utf_auml = symbol_code(strdup(auml));  // if utf8 is used
  \endcode
  
  @pre If \a symbol is a multicharacter symbol, it does not contain other than basic ascii characters
  if utf8 mode is used. If \a symbol is a utf8 character, it is valid utf8. 
  
  @post \a symbol is freed. */
  
  
  /* \brief Character code for \a symbol. \a symbol is a multicharacter symbol or a utf8 character longer than one byte.
      
  The function works in the same way as #symbol_code, but does not free \a symbol.
  As a result, \a symbol needs not be created with 'malloc'.
  
  Examples:
  \code 
  char *SYMB = "multicharacter_symbol";
  Character symbol = symbol_code(SYMB);
  delete SYMB;
  
  Character epsilon = symbol_code("<>");
  
  Character utf_auml = symbol_code(strdup("Ã¤"));  // if utf8 is used
  \endcode
  
  @post \a symbol is not freed or deleted.
  
  */
  


  //           (they operate with functions defined in class SFST::Transducer)

  // helping method(s) declared here
  static bool in_range( unsigned int c, Range *r );

  
  /* \brief Define a transducer variable \a name with value \a t. 

  In SFST, transducer variables are named '$NAME$', where NAME is the name of variable.
  You do not have to use this convention.

  \a t is exploded (#explode) before assigning it as value of \a name. 

  Example:
  \code
  // transducer tr1 has been created earlier 
  if (def_var_("TR1", tr1))
    printf("Warning: assignment of empty transducer to 'TR1'\n");
  else
    printf("Transducer_ variable 'TR1' was assigned a non-empty value.\n");
  \endcode
  
  Use #var_value to get a copy of the value of a transducer variable. 
  
  If function has been called earlier with the same \a name argument, the old
  transducer value is deleted:
  
  \code 
  def_var_("VAR", transducer1);
  
  def_var_("VAR", transducer2);
  // transducer1 is now NULL
  
  def_var_("VAR", transducer3);
  // transducer2 is now NULL; 
  \endcode
  
  @post \a t is deleted. \a name remains unchanged. Previous transducer value assigned to a variable with the same name is deleted.

  @return Whether \a t is empty. */

  bool define_transducer_variable( char *name, TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return define_transducer_variable_(name, pT);
  }

  bool define_transducer_variable_( char *name, fst::StdVectorFst *t ) {  // OK

    // delete the old value of the variable
    VarMap::iterator it=VM.find(name);
    if (it != VM.end()) {
      char *n=it->first;
      fst::StdVectorFst *v=it->second;
      VM.erase(it);
      delete v;
      free(n);
    }
    
    t = explode_(t);
    t = minimize_(t); //  minimise_(t)
    
    VM[name] = t;

    if (is_empty_(t))
      return true;
    else
      return false;
  }
  
  /* \brief Define a transducer agreement variable \a name with value \a t. 

  \a t is exploded (#explode) before assigning it as value of \a name. 

  In SFST, transducer agreement variables are named '$=NAME$', where NAME is the name
  of the transducer. You do not have to use this convention but make sure that transducer
  agreement variables do not get mixed with other symbols. For example, naming a transducer
  agreement variable 'a' or 'agr_var' if symbol 'a' or multicharacter symbol 'agr_var'
  is already in use may cause problems.

  If function has been called earlier with the same \a name argument, the old
  transducer value is deleted:
  
  \code 
  def_rvar_("VAR", transducer1);
  
  def_rvar_("VAR", transducer2);
  // transducer1 is now NULL
  
  def_rvar_("VAR", transducer3);
  // transducer2 is now NULL; 
  \endcode
  
  Example:
  \code
  // fst::StdVectorFst tr1 has been created earlier 
  if (def_rvar_("=TR1", tr1))
    printf("Warning: assignment of empty transducer to 'TR1'\n");
  else
    printf("Transducer_ agreement variable '=TR1' was assigned a non-empty value.\n");
  \endcode
  
  Use #rvar_value to get a transducer representing the value of a transducer agreement variable. 

  @pre \a t is acyclic. 

  @post \a name remains unchanged. \a t is deleted. Previous transducer value assigned to a variable with the same name is deleted.

  @return Whether \a t is empty. */

  bool define_transducer_agreement_variable( char *name, TransducerHandle t ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return define_transducer_agreement_variable_(name, pT);
  }

  bool define_transducer_agreement_variable_( char *name, fst::StdVectorFst *t ) {

    //printf("Function 'def_rvar' called, with var name of '%s'\n", name);
    if (is_cyclic_(t))
      error2("cyclic transducer assigned to", name);
    
    return define_transducer_variable_( name, t );
  }
  
  /* \brief Define the alphabet by collecting all symbol pairs in transducer \a t.
      
  \a t is exploded (#explode) before collecting the symbol pairs. 

  For example, transducer <tt>[ a b [ c | d ] e e:f [ f | b ] g  <symbol> ]</tt> given as argument
  defines an alphabet that contains symbol pairs a:a, b:b, c:c, d:d, e:e, e:f, f:f, g:g and 
  <symbol>:<symbol>.

  Alphabet is a list of all possible symbol pairs.
  Definition of alphabet is needed in #complement_range, #negation, #make_rule, #replace, #replace_in_context,
  #restriction and (if one of the arguments or both is/are the wildcard) #new_transducer.

  There is no actual 'any' character in HFST (any character that does not have to be defined anywhere).
  For example, if alphabet is defined as <tt>[ a a:b b b:c c ]</tt>, the "universal" language <tt>[.*]</tt> is equal to
  <tt>[ a | a:b | b | b:c | c ]*</tt>. Then <tt>[ a:b .o. b:c ] & [.*]</tt> is empty because <tt>[.*]</tt> does not include
  mapping 'a:c' which is the result of composition <tt>[ a:b .o. b:c ]</tt>.

  \note
  Epsilon is never included in the alphabet, because #def_alphabet calls #minimise, which
  removes epsilons. For instance, a transducer defining alphabet as <tt>[ a 0 b ]</tt> is equal
  to <tt>[ a b ]</tt>. A transducer <tt>[ a . b ]</tt> is interpreted as <tt>[ aab | abb ]</tt>
  under both alphabet definitions.

  @post \a t is deleted. */



  /* Collect all symbol pairs in t to alpha. */
  void complete_alphabet( fst::StdVectorFst *t, Alphabet& alpha ) {

    typedef fst::StdArc::StateId StateId;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	Label label = Label(arc.ilabel, arc.olabel);
	alpha.insert(label); // UNINITIALIZED?
      }
    }
  }





  /* \brief A transducer that maps Ranges \a rs1 to Ranges \a rs2.

  The ith Range in Ranges \a rs1 is mapped to the ith Range in Ranges \a rs2 and all
  mappings are concatenated. If Ranges structs
  do not have the same length, the Range structs still left in the longer Ranges 
  are mapped to epsilon one by one.

  For example: 
  - Ranges<a,b,c> : Ranges<d,e> &nbsp; means &nbsp; <tt>[a:d b:e c:0]</tt>
  - Ranges<Range<A,a>,b,c> : Ranges<d,Range<E,e>> &nbsp; means &nbsp;
  Range<A,a>:d b:Range<E,e> c:0 &nbsp; which means
  <tt>[A:d | a:d] [b:E | b:e] [c:0]</tt>

  More examples of Range mappings in function #new_transducer.

  @param rs1 Input side of Transducer.
  @param rs2 Output side of Transducer. 

  @post \a rs1 and \a rs2 are deleted.  */

  // for hfst-calculate
  TransducerHandle make_mapping( Ranges *rs1, Ranges *rs2 ) {
    fst::StdVectorFst* const pResult = make_mapping_( rs1, rs2 );
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


fst::StdVectorFst *make_mapping_( Ranges *l1, Ranges *l2 ) {

  Ranges *tmp1=l1;
  Ranges *tmp2=l2;

  fst::StdVectorFst *t = new fst::StdVectorFst();
  StateId node = t->AddState();
  t->SetStart(node);
  while (l1 && l2) {
    StateId nn = t->AddState();
    for( Range *r1=l1->range; r1; r1=r1->next )
      for( Range *r2=l2->range; r2; r2=r2->next )
	//node->add_arc( Label(r1->character, r2->character), nn, t );
	t->AddArc(node, fst::StdArc(r1->character, r2->character, 0, nn));
    node = nn;
    l1 = l1->next;
    l2 = l2->next;
  }
  while (l1) {
    StateId nn=t->AddState();
    for( Range *r1=l1->range; r1; r1=r1->next )
      //node->add_arc( Label(r1->character, Label::epsilon), nn, t );
      t->AddArc(node, fst::StdArc(r1->character, Label::epsilon, 0, nn));
    node = nn;
    l1 = l1->next;
  }
  while (l2) {
    StateId nn=t->AddState();
    for( Range *r2=l2->range; r2; r2=r2->next )
      //node->add_arc( Label(Label::epsilon, r2->character), nn, t );
      t->AddArc(node, fst::StdArc(Label::epsilon, r2->character, 0, nn));
    node = nn;
    l2 = l2->next;
  }
  t->SetFinal(node, 0);
  delete_ranges(tmp1);
  delete_ranges(tmp2);
  return t;

}






  //fst::StdVectorFst *make_mapping_( Ranges *rs1, Ranges *rs2 ) {
    
    // functions disjunction2 and catenate2 must be defined so that NULL value is handled properly: 
    // dis(NULL, a) == a, cat(NULL, a) == a
    
    /* Variable 'disj' holds the transformations that must be disjuncted with each other and variable 'cat' the
       transformations that must be catenate2d with each other. */

    /*fst::StdVectorFst *result=NULL, *disj=NULL, *cate=NULL;
    while (rs1 && rs2) {
      for( Range *R1=rs1->range; R1; R1=R1->next )
	for( Range *R2=rs2->range; R2; R2=R2->next ) {
	  disj = make_fst(R1->character, R2->character);
	  cate = dis(cate, disj);
	}   
      result = cat(result, cate);
      cate = NULL;
      rs1 = rs1->next;
      rs2 = rs2->next;
    }
    while (rs1) {
      for( Range *R1=rs1->range; R1; R1=R1->next ) {
	disj = make_fst(R1->character, 0);
	cate = dis(cate, disj);
      }
      result = cat(result, cate);
      cate = NULL;
      rs1 = rs1->next;
    }
    while (rs2) {
      for( Range *R2=rs2->range; R2; R2=R2->next ) {
	disj = make_fst(0, R2->character);
	cate = dis(cate, disj);
      }
      result = cat(result, cate);
      cate = NULL;
      rs2 = rs2->next;
    }
    return result;
    }*/

  /* \brief A transducer that maps Range \a r1 to Range \a r2. 

  @pre If r1 and/or r2 is/are NULL, the alphabet is defined.

  @param r1 Input side of transducer. NULL value means the wildcard symbol '.'. 
  @param r2 Output side of transducer. NULL value means the wildcard symbol '.'. 

  The i:th Character in Range r1 is mapped to the i:th Character in Range r2 and all
  mappings are disjuncted. If Ranges
  do not have the same length, the last Character in the shorter Range is mapped to all Characters
  still left in the longer Range one by one. 

  For example: 
  - Range<a,b,c> : Range<d,e,f>  means  <tt>[ a:d | b:e | c:f ]</tt>
  - Range<a,b,c,d> : Range<e,f>  means  <tt>[ a:e | b:f | c:f | d:f ]</tt>
  - Range<a,b> : Range<c,d,e,f>  means  <tt>[ a:c | b:d | b:e | b:f ]</tt>

  If \a r1 is the wildcard <tt>[.]</tt> and \a r2 includes Characters (symbols) from x(1) to x(n) , the resulting transducer
  performs any single mapping allowed by the alphabet where upper (output) level symbol
  is any of the Characters from x(1) to x(n).

  For example, if alphabet is defined as <tt>[ a a:b b c c:d d ]</tt>, 
  - <tt>[a:.]</tt> &nbsp; is &nbsp; <tt>[ a:a | a:b ]</tt> 
  - <tt>[.:b]</tt> &nbsp; is &nbsp; <tt>[ a:b | b:b ]</tt> 
  - <tt>[Range<a,c> : .]</tt> &nbsp; is &nbsp; <tt>[ a | a:b | c | c:d ]</tt> 
  - <tt>[. : Range<b,d>]</tt> &nbsp; is &nbsp; <tt>[ a:b | b | c:d | d ]</tt> 
  - <tt>[.]</tt> (same as <tt>[.:.]</tt>) &nbsp; is &nbsp; <tt>[ a | a:b | b | c | c:d | d ]</tt> 

  (A Range comprising just one Character is noted just by that Character (symbol): Range<x> is same as x).

  An example:

  \code
  // A transducer that maps a to b
  TransducerHandle t1 = new_transducer( add_value(character_code('a'), NULL) , add_value(character_code('b'), NULL) );
  
  // A transducer that maps multicharacter symbol '<symbol_1>' to epsilon 
  // char *symbol1 contains '<symbol_1>' and char *eps '<>'. 
  TransducerHandle t2 = new_transducer( add_value(symbol_code(symbol1), NULL) , add_value(symbol_code(eps), NULL) );
  
  // A transducer that maps Range<a, c, e, g> to Range<b, d>
  
  Range r1 = NULL;
  Range r2 = NULL;
  
  char chars1[4] = { 'a', 'c', 'e', 'g' }; 
  char chars2[2] = { 'b', 'd' };
  
  for (int i=0; i<4; i++)
    r1 = add_value(character_code(chars1[i]), r1);
  
  for (int i=0; i<2; i++)
    r2 = add_value(character_code(chars2[i]), r2);
  
  TransducerHandle t3 = new_transducer( r1 , r2 );
  \endcode
  
  @post \a r1 and \a r2 are deleted. */

  TransducerHandle new_transducer( Range *r1, Range *r2, KeyPairSet *Pi ) {
    fst::StdVectorFst *t = make_transducer( r1, r2, Pi );
    if (r1 != r2)
      free_values(r1);
    free_values(r2);
    return PINSTANCE_TO_HANDLE(Transducer, t);
  }







  /* \brief Weighted version of #new_transducer for test use. 

  Implemented only in OpenFst. In SFST, the weight argument is ignored. */

  /*TransducerHandle new_transducer( Range *r1, Range *r2, int weight ) {
    fst::StdVectorFst *t = make_transducer( r1, r2);
    if (r1 != r2)
      free_values(r1);
    free_values(r2);
    t->SetFinal(1, weight);
    return PINSTANCE_TO_HANDLE(Transducer, t);
    }*/

  fst::StdVectorFst *new_transducer_( Range *r1, Range *r2, KeyPairSet *Pi ) {
    fst::StdVectorFst *t = make_transducer( r1, r2, Pi );
    if (r1 != r2)
      free_values(r1);
    free_values(r2);
    return t;
  }

  static fst::StdVectorFst *make_transducer( Range *r1, Range *r2, KeyPairSet *Pi ) {
    fst::StdVectorFst *result=NULL;

    //fprintf(stderr, "hwfst: make_transducer starts...\n");

    if (r1 == NULL || r2 == NULL) { // [abc]:.
      //if (!Alphabet_Defined)

      if (Pi == NULL)
	error("The wildcard symbol '.' requires the definition of an alphabet of key (symbol) pairs");      
      // one of the ranges was '.'
      
      result = new fst::StdVectorFst();
      result->AddState();
      result->SetStart(0);

      for(KeyPairIterator it=begin_pi_key(Pi); it!=end_pi_key(Pi); it++) {
	KeyPair *kp = get_pi_keypair(it);
	Key input_key = get_input_key(kp);
	Key output_key = get_output_key(kp);
	//fprintf(stderr, "hwfst: make_transducer_: add key pair %hu:%hu?\n", input_key, output_key);
	if ( (r1 == NULL || in_range(input_key, r1)) && (r2 == NULL || in_range(output_key, r2)) ) {
	  result->AddArc(0, fst::StdArc(input_key, output_key, 0, 1));
	  //fprintf(stderr, " ...added.\n");
	}
      }
      result->AddState();
      result->SetFinal(1,0);      
    }    
    else { // [abc]:[de]      
      result = make_fst(r1->character, r2->character);
      for (;;) {      
	if (!r1->next && !r2->next)
	  break;
	if (r1->next)
	  r1 = r1->next;
	if (r2->next)
	  r2 = r2->next;      
	result->AddArc(0, fst::StdArc(r1->character, r2->character, 0, 1));
      }
    }
    //if (!result)
    //  return make_epsilon_();
    //else
    return result;
  }

  // a method called by function 'new_transducer'
  /*static fst::StdVectorFst *make_transducer( Range *r1, Range *r2 ) {  // MUUTETTU
    fst::StdVectorFst *result=NULL;

    if (r1 == NULL || r2 == NULL) { // [abc]:.
      if (!Alphabet_Defined)
	error("The wildcard symbol '.' requires the definition of an alphabet");      
      // one of the ranges was '.'
      
      result = new fst::StdVectorFst();
      result->AddState();
      result->SetStart(0);

      fst::StdVectorFst *t=NULL;
      for(Alphabet::const_iterator it=TheAlphabet.begin(); it!=TheAlphabet.end(); it++) {
	if ((r1 == NULL || in_range(it->lower_char(), r1)) && (r2 == NULL || in_range(it->upper_char(), r2))) {
	  result->AddArc(0, fst::StdArc(it->lower_char(), it->upper_char(), 0, 1));
	}
      }
      result->AddState();
      result->SetFinal(1,0);      
    }    
    else { // [abc]:[de]      
      result = make_fst(r1->character, r2->character);
      for (;;) {      
	if (!r1->next && !r2->next)
	  break;
	if (r1->next)
	  r1 = r1->next;
	if (r2->next)
	  r2 = r2->next;      
	fst::StdVectorFst *t = make_fst(r1->character, r2->character);
	fst::StdVectorFst *tmp = result;
	result = dis(result, t);
	delete tmp;
      }
    }
    if (!result)
      return make_epsilon_();
    else
      return result;
      }*/

  
  // a method called by function 'new_transducer'
  static void free_values( Range *r ) {  // OK
    //printf("Method 'free_values' started...\n");
    while (r) {
      Range *tmp=r;
      r = r->next;
      delete tmp;
    }
  }
  
  static bool in_range( unsigned int c, Range *r ) {
    while (r) {
      if (r->character == c)
	return true;
      r = r->next;
    }
    return false;
  }


  // ------- ERROR REPORTING --------

  static void error( const char *message ) {
    cerr << "\nError: " << message << "\naborted.\n";
    exit(1);
  }

  void error2( const char *message, const char *input ) {  // COPIED
    cerr << "\nError: " << message << ": " << input << "\naborted.\n";
    exit(1);
  }






  //------------------------------------------------------//
  //                                                      //
  //  METHODS FOR TRANSFORMING PATHS INTO TRANSDUCERS AND //
  //  STRINGS AND PRINTING THEM.                          //
  //                                                      //
  //------------------------------------------------------//


  typedef stack<fst::StdArc> ArcStack;              // a path is presented as a stack of arcs 
  typedef vector< stack<fst::StdArc> > PathVector;  // a vector holding all possible paths
  // note: a path's final weight must be stored in a separate structure



  /* Transform a vector of paths into a vector of tranducers. */
  vector<fst::StdVectorFst*> paths_to_transducers(PathVector pv, vector<float> *final_weights) {
    
    vector<fst::StdVectorFst*> transducers = vector<fst::StdVectorFst*>();

    //for(PathVector::iterator it = pv.begin(); it != pv.end(); it++) {
    for( unsigned int i=0; i<pv.size(); i++) {
      fst::StdVectorFst *t = new fst::StdVectorFst;
      t->AddState();
      t->SetStart(0);
      int n=0;

      while(!pv[i].empty()) {
	fst::StdArc arc = pv[i].top();
	t->AddArc(n, fst::StdArc(arc.ilabel, arc.olabel, arc.weight, n+1));
	t->AddState();
	n++;
	pv[i].pop();
      }
      t->SetFinal(n, final_weights->at(i));
      transducers.push_back(t);
    }
    return transducers;
  }

  float get_weight( fst::StdVectorFst *t ) {
    float path_weight=0;
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	path_weight = path_weight + (arc.weight).Value();
      }
      if(is_final(t, state_id))
	path_weight = path_weight + (t->Final(state_id)).Value();
    }
    return path_weight;
  }

  float get_weight( TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return get_weight(pT);
  }

  char * path_to_string( fst::StdVectorFst *t, KeyTable *T, bool spaces, bool epsilons ) {

    Alphabet alpha = T->to_alphabet(TheAlphabet);
    string path = std::string("");
    for (fst::StateIterator<fst::StdFst> siter(*t); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {

	const fst::StdArc &arc = aiter.Value();
	const char *isymbol = alpha.code2symbol(arc.ilabel);
	const char *osymbol = alpha.code2symbol(arc.olabel);
	if (!epsilons && arc.ilabel == Label::epsilon && arc.olabel == Label::epsilon) {
	  break;
	}
	if (path.length() > 0 && spaces)
	  path.append(1, ' ');

	for (int i=0; isymbol[i] != '\0'; i++) {
	  if (isymbol[i] == ':' || isymbol[i] == '\\' ||
	      ( spaces && isymbol[i] == ' ') )
	    path.append(1, '\\');
	  path.append(1, isymbol[i]);
	}
	if (strcmp(isymbol, osymbol) != 0) {
	  path.append(1, ':');
	  for (int i=0; osymbol[i] != '\0'; i++) {
	    if (osymbol[i] == ':' || osymbol[i] == '\\' ||
		( spaces && isymbol[i] == ' ') )
	      path.append(1, '\\');
	    path.append(1, osymbol[i]);
	  }
	}
	// path_weight = path_weight + (arc.weight).Value();
      }
      // if(is_final(t, state_id))
      // path_weight = path_weight + (t->Final(state_id)).Value();
    }
    return strdup(path.c_str());
  }
  
  char * key_vector_to_pairstring( KeyVector * kv, KeyTable * kt ) {
    return vector_to_pairstring(kv,kt,TheAlphabet);
  }

  char * key_pair_vector_to_pairstring( KeyPairVector * kv, KeyTable * kt ) {
    return vector_to_pairstring(kv,kt,TheAlphabet);
  }
  char * transducer_to_pairstring( TransducerHandle t, KeyTable *T, bool spaces, bool epsilons ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    if (is_empty_(pT)) {
      //std::string result="\\empty_transducer";
      //return strdup(result.c_str());
      return NULL;
    }
    char *result = path_to_string(pT, T, spaces, epsilons);
    /*if (strcmp(result, "") == 0) {
      delete result;
      if (is_key(Label::epsilon, T))
	return strdup(get_symbol_name(get_key_symbol(Label::epsilon, T)));
      else {
	std::string res = "\\epsilon";
	return strdup(res.c_str());
      }
      }*/
    return result;
  }
  
  fst::StdVectorFst *keypair_vector_to_transducer_(vector<KeyPair> &pairs) {

    fst::StdVectorFst *t = new fst::StdVectorFst;
    t->AddState();
    t->SetStart(0);

    unsigned int last_state = pairs.size();
    for(unsigned int i=0; i<last_state; i++) {
      Label kp = pairs[i];
      t->AddArc(i, fst::StdArc(kp.lower_char(), kp.upper_char(), 0, i+1));
      t->AddState();
    }
    t->SetFinal(last_state, 0);
    return t;
  }


  fst::StdVectorFst *to_transducer_( const char *str, Alphabet &alpha ) {    
    //if (strcmp(str, "\\empty_transducer") == 0 )
    //  return make_empty_();
    if (strcmp(str, "") == 0 )
      return make_epsilon_();
    vector<KeyPair> pairs = alpha.string_to_keypair_vector(str);
    return keypair_vector_to_transducer_(pairs);
  }

  
  TransducerHandle pairstring_to_transducer( const char *str, KeyTable *T ) 
{
  Alphabet alpha = T->to_alphabet(TheAlphabet);
  fst::StdVectorFst *pT = to_transducer_(str, alpha);
  return PINSTANCE_TO_HANDLE(Transducer, pT);
}
  
  
  // void escape( char *s, std::string& escaped, bool space, bool colon, bool brackets);
  
  /* Get a string representation of all paths in 'pv'.
     'T' indicates the key-to-symbol mapping.
     'spaces' indicates if a space is written between the transitions.
     All special symbols excluding the space and brackets are escaped. */
  vector<char*> paths_to_strings( PathVector pv, vector<float> *final_weights,
				  KeyTable *T, bool spaces ) {
    // vector<float> *path_weights 

    Alphabet alpha = T->to_alphabet(TheAlphabet);
    vector<char*> result = vector<char*>();

    // for(PathVector::iterator it = pv.begin(); it != pv.end(); it++) {
    for(unsigned int i=0; i<pv.size(); i++) {
      // for each path, create a string representing it 
      string one_path = std::string("");
      bool first = true;
      float string_weight=0;

      while(!pv[i].empty()) {
	// for each transition in a path,
	fst::StdArc arc = pv[i].top();

	// append space to the string, if needed
	if(spaces && !first) {
	  one_path.append(" ");
	  first = false;
	}

	// append the input symbol
	char *isymbol = strdup( alpha.code2symbol(arc.ilabel) );
	COMMON::escape_and_add(isymbol, one_path, false, true, false);

	// append a colon and the output symbol, if it is not identical
	// to the input symbol
	if (arc.ilabel != arc.olabel) {
	  one_path.append(":");
	  char *osymbol = strdup(alpha.code2symbol(arc.olabel));
	  COMMON::escape_and_add(osymbol, one_path, false, true, false);
	}

	// add the transition's weight to total weight
	string_weight = string_weight + (arc.weight).Value();

	pv[i].pop();
      }
      // add the paths's final weight to total weight
      string_weight = string_weight + final_weights->at(i);
      //path_weights->push_back(string_weight);
      char buffer[10];
      sprintf(buffer, "\t%4.2f", string_weight);
      char *pbuffer = buffer;
      one_path.append(pbuffer);

      // add the string to the vector of strings
      char *one_path_array = new char [one_path.size()+1];
      strcpy(one_path_array, one_path.c_str());
      result.push_back(one_path_array);
    }
    return result;
  }


  /* Print a string representation of all paths in 'pv' to 'outfile'.
     'T' indicates the key-to-symbol mapping.
     'spaces' indicates if a space is printed between the transitions.
     All special symbols excluding the space and brackets are escaped. */
  void paths_to_print(PathVector &pv, vector<float> *final_weights, FILE *outfile, KeyTable *T, bool escape, bool spaces) {

    Alphabet alpha = T->to_alphabet(TheAlphabet);
    fputc('\n', outfile);

    // for(PathVector::iterator it = pv.begin(); it != pv.end(); it++) {
    for(unsigned int i=0; i<pv.size(); i++) {
      // for each path,
      bool first = true;
      float string_weight=0;

      while(!pv[i].empty()) {
	// for each transition in a path,
	fst::StdArc arc = pv[i].top();

	// print space, if needed
	if(spaces && !first) {
	  fputc(' ', outfile);
	  first = false;
	}

	// print the input symbol
	const char *isymbol = alpha.code2symbol(arc.ilabel);
	if (escape)
	  COMMON::escape_and_print(isymbol, outfile, false, true, false);
	else
	  fputs(isymbol, outfile);

	// print a colon and the output symbol, if it is not identical
	// to the input symbol
	if (arc.ilabel != arc.olabel) {
	  fputc(':', outfile);
	  const char *osymbol = alpha.code2symbol(arc.olabel);
	  COMMON::escape_and_print(osymbol, outfile, false, true, false);
	}

	// add the transition's weight to total weight
	string_weight = string_weight + (arc.weight).Value();

	pv[i].pop();
      }
      // add the paths's final weight to total weight
      string_weight = string_weight + final_weights->at(i);
      fprintf(outfile, "\t%4.2f\n", string_weight);
    }
  }




  //------------------------------------------------------//
  //                                                      //
  //  METHODS FOR FINDING ALL PATHS THROUGH A TRANSDUCER  //
  //                                                      //
  //------------------------------------------------------//


  /* Adds an arc to all paths in a PathVector. */
  void add_arc(PathVector& pv, fst::StdArc arc) {
    /*if (pv.empty()) {
      ArcStack arc_stack = ArcStack(); 
      arc_stack.push(arc);
      pv.push_back(arc_stack);
    }
    else {*/
      for(PathVector::iterator it = pv.begin(); it != pv.end(); it++)
	it->push(arc);
      //}
  }
  
  /* Add contents of pv2 to pv1. */
  void add_paths(PathVector& pv1, PathVector& pv2) {
    for(PathVector::iterator it = pv2.begin(); it != pv2.end(); it++)
      pv1.push_back(*it);
  }


  /* A recursive algorithm that finds all paths from state 'state_id' 
     to a final state in a transducer t. Recursion ends when a final state 
     is reached and an empty PathVector is returned. An empty ArcStack in the 
     result PathVector indicates that state 'state_id' is final, i.e. an
     epsilon path is one possible path. NULL value indicates that a cycle is detected
     and that there is an infinite number of paths. */
  PathVector *find_paths( fst::StdVectorFst *t, StateId state_id, bool calling_path[], vector<float> *final_weights ) {
    
    // Check that state 'state_id' is not included in calling path.
    // If it is, transducer is cyclic, and a NULL value is returned.
    
    if (calling_path[state_id])
      return NULL;
    
    PathVector *result = new PathVector();
    
    // go through all arcs in this state
    for (fst::ArcIterator<fst::StdFst> aiter(*t, state_id); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      // find all paths leading from this arc's destination state to a final state
      calling_path[state_id] = true;
      PathVector *paths;

      paths = find_paths(t, arc.nextstate, calling_path, final_weights);

      if (paths == NULL) // a cycle detected, propagate the NULL value to calling function
	return NULL;

      // add this arc to paths
      add_arc(*paths, arc);
      // add to the result all paths leading from this state to a final state
      add_paths(*result, *paths);
      paths->clear(); // ######
      delete paths;   // ######
    }
    if (result->empty()) { // final end-state reached (non-final end-states not allowed)
      ArcStack empty_stack = ArcStack();
      result->push_back(empty_stack);
      final_weights->push_back((t->Final(state_id)).Value());
    } 
    else {
      if (is_final(t, state_id)) {  // final non-end-state
	PathVector empty = PathVector();
	ArcStack empty_stack = ArcStack();
	empty.push_back(empty_stack);
	final_weights->push_back((t->Final(state_id)).Value());
	add_paths(*result, empty);
      }
    }
    calling_path[state_id] = false;
    return result;
  }
  

  /* A recursive algorithm that finds all paths from initial state to a final state
     in transducer t. NULL value indicates a cycle in transducer t, i.e. an infinite
     number of paths. An empty path vector indicates that Transducer t is the empty
     transducer.

     For a path at index i in the returned PathVector, the corresponding final weight
     is stored at index i in 'final_weights'. 

     'connect' defines whether the transducer is trimmed so that it does not contain
     states that are not on a path to a final state. */

  PathVector *find_paths(fst::StdVectorFst *t, vector<float> *final_weights, bool connect) {
    
    fst::StdVectorFst *T;
    if (connect) {
      // trim the transducer
      T = t->Copy();
      fst::Connect(T);
    }
    else
      T = t;
    
    PathVector *pv=NULL;
    
    if (is_empty_(T)) {
      pv = new PathVector();
    }
    else {
      bool calling_path [T->NumStates()];
      for (int i=0; i<T->NumStates(); i++)
	calling_path[i] = false;
      pv = find_paths(T, T->Start(), calling_path, final_weights);
      if (pv == NULL) // a cycle detected
	return NULL;
      // change possible empty path (an empty ArcStack) to an epsilon arc (<>:<>)
      for(PathVector::iterator it = (*pv).begin(); it != (*pv).end(); it++) {
	if((*it).empty()) {
	  fst::StdArc arc = fst::StdArc(Label::epsilon, Label::epsilon, 0, 1);
	  (*it).push(arc);
	}
      }
    }
    return pv;
  }
  




  





  /******************************************************************/
  /*                                                                 */
  /*     File: utf8.C                                                */
  /*   Author: Helmut Schmid                                         */
  /*  Purpose:                                                       */
  /*  Created: Mon Sep  5 17:49:16 2005                              */
  /* Modified: Thu May 11 17:15:36 2006 (schmid)                     */
  /*                                                                 */
  /******************************************************************/
  
  const unsigned char get3LSbits=7;
  const unsigned char get4LSbits=15;
  const unsigned char get5LSbits=31;
  const unsigned char get6LSbits=63;
  
  const unsigned char set1MSbits=128;
  const unsigned char set2MSbits=192;
  const unsigned char set3MSbits=224;
  const unsigned char set4MSbits=240;
  
  
  char *int_to_utf8( unsigned int sym ) {
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
  
  unsigned int utf8_to_int( char **s ) {
    return utf8toint( s );
  }

  /* Get the integer value of utf8 character. */
  unsigned int utf8toint( char **s ) {
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
  
  /* \brief Integer value for a utf8 character \a s. 
  Use with #add_values. 
  @pre \a s is valid utf8.
  @post \a s remains unchanged. */

  unsigned int utf8_to_int( char *s ) {
    unsigned int result = utf8toint( &s );
    if (*s == 0) // all bytes converted?
      return result;
    return 0;
  }

  static void error_message( size_t line );  


  /*FA****************************************************************/
  /*                                                                 */
  /*  next_string                                                    */
  /*                                                                 */
  /*FE****************************************************************/
  
  char *next_string_unquote( char* &s, size_t line ) {

    if (s == NULL)
      return NULL;
    
    // scan the input up to the next tab or newline character
    // and unquote symbols preceded by a backslash
    char *p = s;
    char *q = s;
    while (*q!=0 && *q!='\t' && *q!='\n' && *q!='\r') {
      if (*q == '\\')
	q++;
      *(p++) = *(q++);
    }
    if (p == s)
      error_message(line); // no string found

    char *result=s;
    // skip over following whitespace
    while (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r')
      q++;
    
    if (*q == 0)
      s = NULL; // end of string was reached
    else
      s = q;  // move the string pointer s
    
    *p = 0; // mark the end of the result string
    
    return result;
  }


  // next string in s 

  char *next_string( char* &s, size_t line ) {

    if (s == NULL)
      return NULL;
    
    // scan the input up to the next tab or newline character
    // and unquote symbols preceded by a backslash
    char *p = s;
    char *q = s;
    while (*q!=0 && *q!='\t' && *q!='\n' && *q!='\r') {
      //if (*q == '\\')
      //  q++;
      *(p++) = *(q++);
    }
    if (p == s)
      error_message(line); // no string found

    char *result=s;
    // skip over following whitespace
    while (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r')
      q++;
    
    if (*q == 0)
      s = NULL; // end of string was reached
    else
      s = q;  // move the string pointer s
    
    *p = 0; // mark the end of the result string
    
    return result;
  }


















  fst::StdVectorFst *read_transducer_text_old( istream& is, KeyTable *T );
  fst::StdVectorFst *read_transducer_text_new( istream& is, KeyTable *T=NULL, bool numbers=false );

  /* \brief Make a transducer as defined in text form in \a file. \a sfst defines
      whether SFST text format is used.
      
  An example of a transducer file:

  \verbatim
  AT&T                                       AT&T UNWEIGHTED                     SFST                         

  0      0                                   0                                   final  0                     
  0      1      a      a      0.3            0      1      a      a              0      a      1
  0      2      b      b      0              0      2      b      b              0      b      2
  1      0      c      C      0.5            1      0      c      C              1      c:C    0
  2      1      c      c      0              2      1      c      c              2      c      1
  2      0      a      A      1.2            2      0      a      A              2      a:A    0
  2      2      d      D      1.65           2      2      d      D              2      d:D    2
  2      0.5                                 2                                   final  2
  \endverbatim     

  The lines in the transducer file are of type <br>
  originating_node TAB input_symbol:output_symbol TAB destination_node OR
  final TAB final_node<br>
  in SFST format and of type <br>
  originating_node TAB destination_node TAB input_symbol TAB output_symbol (TAB transition_weight) OR
  final_node (TAB final_weight) in AT&T format.
 
  When AT&T format is used in HFST, weights are ignored.
  When SFST or AT&T unweighted format is used in HWFST, weights are set to zero.

  \note
  Weighted version does not yet work correctly.

  @pre \a file does not contain empty lines.

  @post \a file is not closed. Contents of \a file are not changed.

  */

  TransducerHandle read_transducer_text( istream& is, KeyTable *T, bool sfst ) {
    if (!sfst)
      return PINSTANCE_TO_HANDLE(Transducer, read_transducer_text_new(is, T, false));
    else
      return PINSTANCE_TO_HANDLE(Transducer, read_transducer_text_old(is, T));
  }

  // old sfst format not supported
  TransducerHandle read_transducer_number( istream& is ) {
    return PINSTANCE_TO_HANDLE(Transducer, read_transducer_text_new(is, NULL, true));
  }

  
  /*TransducerHandle read_transducer_text(FILE *file, bool sfst) {
    char *table_name = "symbol_table";

    // fprintf(stderr, "reading symbol_table... ");
    read_symbol_table(table_name);
    // fprintf(stderr, " ...symbol_table read\n");
    fst::StdVectorFst* pResult;
    if (sfst)
      pResult = read_transducer_text_old(file);
    else
      pResult = read_transducer_text_(file);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);    
    }*/

}


// in file fst.C, borrowed from SFST
char * next_string_sfst( char* &s, size_t line );

namespace HWFST {

  /* read transducer in old SFST format */
  fst::StdVectorFst *read_transducer_text_old( istream& is, KeyTable *T ) {

    //if (feof(file))
    if (is.eof())
      return NULL;

    Alphabet alpha = T->to_alphabet(TheAlphabet);

    fst::StdVectorFst *result = new fst::StdVectorFst;
    result->AddState();
    result->SetStart(0);

    vector<StateId> final_nodes;
    long largest_state_id = 0;

    char buffer[10000];
    //for( size_t line=0; fgets(buffer, 10000, file ); line++ ) {
    for( size_t line=0; is.getline(buffer, 10000); line++ ) {
      char *p = buffer;
      char *s1 = ::next_string_sfst(p, line);
      if (strcmp(s1, "final") == 0) {
	s1 = ::next_string_sfst(p, line);
	char *foo;
	final_nodes.push_back(strtol(s1, &foo, 10));
      }
      else {
	char *foo;
	while (largest_state_id < strtol(s1, &foo, 10)) // add new states
	  largest_state_id = result->AddState(); 
	char *s2 = ::next_string_sfst(p, line);
	Label l = alpha.next_label( s2, 2 );
	if (*s2 != 0 || l == Label::epsilon)
	  error_message( line );
	s2 = ::next_string_sfst(p, line);
	while (largest_state_id < strtol(s2, &foo, 10)) // add new states
	  largest_state_id = result->AddState(); 
	result->AddArc(strtol(s1, &foo, 10), fst::StdArc(l.lower_char(), l.upper_char(), 0, strtol(s2, &foo, 10)));
      }
    }

    for (unsigned int i=0; i < final_nodes.size(); i++)
      result->SetFinal(final_nodes[i], 0);
    return result;
  }


  TransducerHandle intersecting_composition( TransducerHandle lexicon, 
					     vector<TransducerHandle> * rules,
					     KeyTable * k) {

    fst::StdVectorFst* pLexicon = HANDLE_TO_PINSTANCE(fst::StdVectorFst, lexicon);
    
    vector<fst::StdVectorFst*> pRules;
    for( vector<TransducerHandle>::iterator it = rules->begin();
	 it != rules->end(); ++it )
      pRules.push_back( HANDLE_TO_PINSTANCE( fst::StdVectorFst, *it ) );
    KeyTable * skip_symbol_table = gather_flag_diacritic_table(k);
    KeySet * skip_symbols = get_key_set(skip_symbol_table);
    delete skip_symbol_table;
    fst::StdVectorFst * result = fst::Composer( *pLexicon, pRules, *skip_symbols )();
    delete skip_symbols;
    delete pLexicon;
    for( vector<fst::StdVectorFst*>::iterator it = pRules.begin(); it != pRules.end(); ++it )
      delete *it;

    return PINSTANCE_TO_HANDLE(Transducer, result);

  };

}

// in file fst.C
char * next_string_att( char* &s, size_t line );  

namespace HWFST {

  // Read transducer in text format from istream is.
  fst::StdVectorFst *read_transducer_text_new( istream& is, KeyTable *T, bool numbers ) {

    //fprintf(stderr, "read_transducer_text_new...\n");

    if (is.eof()) {
      return NULL;
    }

    fst::StdVectorFst *result = new fst::StdVectorFst;
    result->AddState();
    result->SetStart(0);

    vector<StateId> final_nodes;
    vector<float> final_weights;
    long largest_state_id = 0;

    char buffer[10000];
    for( size_t line=0; is.getline(buffer, 10000); line++ ) {
      char *p = buffer;
      if ( strncmp(p, "--", (size_t)2) == 0  ||  strcmp(p, "") == 0 ) {
	break;
      }
      if ( buffer[0] != '%' ) {  // not a comment line
	char *s1 = strtok(p, "\t\n");  // originating state or final state
	char *s2 = strtok(NULL, "\t\n");  // destination state or optional final weight
	char *s3 = strtok(NULL, "\t");  // input symbol or NULL
	
	char *s4; char *s5;
	
	if (s2 == NULL) {  // final state without weight
	  char *foo;
	  final_nodes.push_back(strtol(s1, &foo, 10));
	  final_weights.push_back(0.0);
	}
	else if (s3 == NULL) {  // final state and final weight
	  char *foo;
	  final_nodes.push_back(strtol(s1, &foo, 10));
	  final_weights.push_back((float)strtod(s2, NULL));
	}
	else {   // originating state, destination state, input symbol, output symbol and optional transition weight
	  s4 = strtok(NULL, "\t\n");  // output symbol 
	  s5 = strtok(NULL, "\n");    // optional transition weight or empty string or NULL
	  
	  //fprintf(stderr, "Read line: %s\t%s\t%s\t%s\t%s\n",s1, s2, s3, s4, s5);

	  char *foo;
	  
	  // create new states up to max{originating_state, destination_state} 
	  while (largest_state_id < strtol(s1, &foo, 10) || largest_state_id < strtol(s2, &foo, 10)) 
	    largest_state_id = result->AddState();

	  const char *s3_unesc;
	  const char *s4_unesc;
	  if (!numbers) {
	    // unescape the symbols (e.g. backslash plus t becomes tabulator)
	    std::string s3_unescaped;
	    COMMON::unescape_and_add( s3, s3_unescaped);
	    s3_unesc = strdup(s3_unescaped.c_str());
	    std::string s4_unescaped;
	    COMMON::unescape_and_add( s4, s4_unescaped);
	    s4_unesc = strdup(s4_unescaped.c_str());
	  }
	  //fprintf(stderr, " ..s3_unesc and s4_unesc are '' and ''\n", s3_unesc, s4_unesc);
	  
	  Key input_number;
	  Key output_number;
	  if (numbers) {  // use numbers
	    //fprintf(stderr, " ..using numbers\n");
	    input_number = (unsigned int)atoi(s3);
	    output_number = (unsigned int)atoi(s4);
	  }
	  else if (T) {  // use keys defined in T, if not found, add to next free position
	    Symbol input_symbol = define_symbol(s3_unesc);
	    Symbol output_symbol = define_symbol(s4_unesc);
	    input_number = T->add_symbol(input_symbol);
	    output_number = T->add_symbol(output_symbol);
	    delete s3_unesc; delete s4_unesc;
	    //fprintf(stderr, " .. input: '%s' (%hu) output: '%s' (%hu)\n", s3_unesc, input_number, s4_unesc, output_number);
	  }
	  else { // use TheAlphabet
	    input_number = TheAlphabet.add_symbol(s3_unesc);
	    output_number = TheAlphabet.add_symbol(s4_unesc);
	    delete s3_unesc; delete s4_unesc;
	    //fprintf(stderr, " .. input (using TheAlphabet): '%s' (%hu) output: '%s' (%hu)\n", s3_unesc, input_number, s4_unesc, output_number);
	  }	    


	  if (s5 == NULL || strcmp(s5,"") == 0) {  // no transition weight
	    result->AddArc(strtol(s1, &foo, 10),
			   fst::StdArc(input_number,
				       output_number,
				       0.0, 
				       strtol(s2, &foo, 10)));
	  }
	  else {  // transition weight
	    result->AddArc(strtol(s1, &foo, 10), 
			   fst::StdArc(input_number, 
				       output_number, 
				       fst::TropicalWeight((float)strtod(s5, NULL)), 
				       strtol(s2, &foo, 10)));
	  }
	}
      }
    }
    
    // set final nodes and their weights
    for (unsigned int i=0; i < final_nodes.size(); i++)  
      result->SetFinal(final_nodes[i], final_weights[i]);
    
    return result;    
  }


  //TransducerHandle read_transducer_text( istream& is ) {
  //  return PINSTANCE_TO_HANDLE(Transducer, read_transducer_text_new(is));
  //}

  /* read transducer in AT&T format */
  fst::StdVectorFst *read_transducer_text_( istream& is, KeyTable *T, bool numbers ) {

    if (is.eof()) {
      return NULL;
    }

    Alphabet alpha;

    if (!numbers) {
      if (T) {
	alpha = T->to_alphabet(TheAlphabet);
      }
      else {
	alpha.copy(TheAlphabet);
      }
    }

    fst::StdVectorFst *result = new fst::StdVectorFst;
    result->AddState();
    result->SetStart(0);

    vector<StateId> final_nodes;
    vector<float> final_weights;
    long largest_state_id = 0;

    char buffer[10000];
    for( size_t line=0; is.getline(buffer, 10000); line++ ) {
      char *p = buffer;
      if ( strncmp(p, "--", (size_t)2) == 0  ||  strcmp(p, "") == 0 ) {
	break;
      }
      if ( buffer[0] != '%' ) {  // not a comment line
	char *s1 = ::next_string_att(p, line);  // originating state or final state
	char *s2 = ::next_string_att(p, line);  // destination state or optional final weight
	char *s3 = ::next_string_att(p, line);  // input symbol or NULL
	
	char *s4; char *s5;
	
	if (s2 == NULL) {  // final state without weight
	  char *foo;
	  final_nodes.push_back(strtol(s1, &foo, 10));
	  final_weights.push_back(0.0);
	}
	else if (s3 == NULL) {  // final state and final weight
	  char *foo;
	  final_nodes.push_back(strtol(s1, &foo, 10));
	  final_weights.push_back((float)strtod(s2, NULL));
	}
	else {   // originating state, destination state, input symbol, output symbol and optional transition weight
	  s4 = ::next_string_att(p, line);  // output symbol 
	  s5 = ::next_string_att(p, line);  // optional transition weight or NULL
	  
	  char *foo;
	  
	  // create new states up to max{originating_state, destination_state} 
	  while (largest_state_id < strtol(s1, &foo, 10) || largest_state_id < strtol(s2, &foo, 10)) 
	    largest_state_id = result->AddState();

	  if (!numbers) {
	    // unescape the symbols (e.g. backslash plus t becomes tabulator)
	    std::string s3_unescaped;
	    COMMON::unescape_and_add( s3, s3_unescaped);
	    const char *s3_unesc = s3_unescaped.c_str();
	    std::string s4_unescaped;
	    COMMON::unescape_and_add( s4, s4_unescaped);
	    const char *s4_unesc = s4_unescaped.c_str();
	    
	    // fprintf(stderr, "\n ----- read label pair '%s' and '%s' \n (in unescaped form '%s' and '%s')\n", s3, s4, s3_unesc, s4_unesc );
	    
	    if (s5 == NULL) {  // no transition weight
	      result->AddArc(strtol(s1, &foo, 10),
			     fst::StdArc(alpha.symbol2code(s3_unesc), 
					 alpha.symbol2code(s4_unesc), 
					 0.0, 
					 strtol(s2, &foo, 10)));
	    }
	    else {  // transition weight
	      result->AddArc(strtol(s1, &foo, 10), 
			     fst::StdArc(alpha.symbol2code(s3_unesc), 
					 alpha.symbol2code(s4_unesc), 
					 fst::TropicalWeight((float)strtod(s5, NULL)), 
					 strtol(s2, &foo, 10)));
	    }
	  }
	  else {
	    if (s5 == NULL) {  // no transition weight
	      result->AddArc(strtol(s1, &foo, 10),
			     fst::StdArc((unsigned int)atoi(s3), 
					 (unsigned int)atoi(s4), 
					 0.0, 
					 strtol(s2, &foo, 10)));
	    }
	    else {  // transition weight
	      result->AddArc(strtol(s1, &foo, 10), 
			     fst::StdArc((unsigned int)atoi(s3), 
					 (unsigned int)atoi(s4), 
					 fst::TropicalWeight((float)strtod(s5, NULL)), 
					 strtol(s2, &foo, 10)));
	    }
	  }
	}
      }
    }
    
    // set final nodes and their weights
    for (unsigned int i=0; i < final_nodes.size(); i++)  
      result->SetFinal(final_nodes[i], final_weights[i]);
    
    return result;
  }


  /*FA****************************************************************/
  /*                                                                 */
  /*  error_message                                                  */
  /*                                                                 */
  /*FE****************************************************************/
  
  static void error_message( size_t line ) {
    static char message[1000];
    sprintf(message, "Error: in line %u of text transducer file", 
	    (unsigned int)line);
    throw message;
  }





























  // ********************************************************************
  // ********************************************************************
  //
  //                    HFST: Rule Operator API
  //
  // ********************************************************************
  // ********************************************************************


  /* \brief ContextsHandle with left context \a t1 and right context \a t2. 

  @param t1 Left context. If NULL, interpreted as an empty transducer.
  @param t2 Right context. If NULL, interpreted as an empty transducer. 

  @post \a t1 and \a t2 are not deleted. The resulting ContextsHandle has pointers to \a t1 and \a t2. */

  ContextsHandle make_context( TransducerHandle t1, TransducerHandle t2 ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    Contexts* const pResult = make_context_(pT1, pT2);

    ContextsHandle result = new contexts_t();
    result->left = PINSTANCE_TO_HANDLE(Transducer, pResult->left);
    result->right = PINSTANCE_TO_HANDLE(Transducer, pResult->right);
    delete pResult;
    return result;
  }

  Contexts *make_context_( fst::StdVectorFst *t1, fst::StdVectorFst *t2 ) {
    if (t1 == NULL)
      t1 = make_epsilon_();
    if (t2 == NULL)
      t2 = make_epsilon_();
    
    Contexts *c=new Contexts();
    c->left = t1;
    c->right = t2;
    c->next = NULL;
    
    return c;
  }

  /* \brief Add Contexts \a p2 to Contexts \a p1. 

  @pre p1 is not NULL.

  @param p1 Contexts where \a p2 is added.
  @param p2 Contexts to be added. Can be NULL.

  @result Contexts \a p1 after \a p2 has been added to it.

  @post \a p1 and \a p2 are not deleted. Resulting Contexts has a pointer to \a p2. */

  ContextsHandle append_context( ContextsHandle p1, ContextsHandle p2 ) {
    p1->next = p2;
    return p1;
  }

  Contexts *add_context_( Contexts *p1, Contexts *p2 ) {
    p1->next = p2;
    return p1;
  }


  fst::StdVectorFst *make_rule_( fst::StdVectorFst *t1, Range *r1, Twol_Type type, Range *r2, fst::StdVectorFst *t2, KeyPairSet *Pi );
  fst::StdVectorFst *replace_transducer( fst::StdVectorFst *ct, Key lm, Key rm,
					 Repl_Type type, KeyPairSet *Pi );
  fst::StdVectorFst *replace_transducer( fst::StdVectorFst *ct, Character lm, Character rm, Repl_Type type, KeyPairSet *Pi );
  fst::StdVectorFst *replace_( fst::StdVectorFst *ct, Repl_Type type, bool optional, KeyPairSet *Pi );

  static fst::StdVectorFst *twol_left_rule( fst::StdVectorFst *lc, Range *lower_range, Range *upper_range, fst::StdVectorFst *rc, KeyPairSet *Pi );
  static fst::StdVectorFst *twol_right_rule( fst::StdVectorFst *lc, Range *lower_range, Range *upper_range, fst::StdVectorFst *rc, KeyPairSet *Pi );
  fst::StdVectorFst *restriction_( fst::StdVectorFst *t, Twol_Type type, Contexts *p, 
				   int direction, KeyPairSet *Pi );
  static fst::StdVectorFst *cp( Range *lower_range, Range *upper_range );
  static fst::StdVectorFst *anti_cp( Range *lower_range, Range *upper_range, KeyPairSet *Pi );

  static void free_values( Range *r );

  fst::StdVectorFst *extended_context( fst::StdVectorFst *t, Character m1, Character m2 );
  fst::StdVectorFst *replace_context( fst::StdVectorFst *t, Character m1, Character m2, KeyPairSet *Pi );
  fst::StdVectorFst *replace_in_context_( fst::StdVectorFst *t, Repl_Type type, Contexts *p, bool optional, KeyPairSet *Pi );


  TransducerHandle make_replace( TransducerHandle t, Repl_Type type, bool optional, KeyPairSet *Pi ) {
    fst::StdVectorFst* const pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    fst::StdVectorFst* const pResult = replace_(pT, type, optional, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }
  
  fst::StdVectorFst *replace_( fst::StdVectorFst *t, Repl_Type type, bool optional, KeyPairSet *Pi ) {

    // compute the no-center transducer
    fst::StdVectorFst *projection = t->Copy();
    
    if (type == repl_up)
      fst::Project( projection, fst::PROJECT_INPUT );
    else if (type == repl_down)
      fst::Project( projection, fst::PROJECT_OUTPUT );
    else
      error("Invalid type of replace operator");
    
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure( pi, fst::CLOSURE_STAR );
   
    // .*  _t .*
    fst::StdVectorFst *t1 = pi->Copy();
    fst::Concat(t1, *projection);
    fst::Concat(t1, *pi);
    delete projection;

    // no_t = !(.*  _t .*)
    fst::StdVectorFst *no_t = negation_(t1, Pi, true);
    
    // compute the unconditional replacement transducer
    
    // (no_t t)*
    fst::StdVectorFst *t2 = no_t->Copy();
    fst::Concat(t2, *t);
    fst::Closure(t2, fst::CLOSURE_STAR);
    
    // (no_t t)* no_t
    fst::Concat(t2, *no_t);
    delete no_t;
    
    if (optional) {
      fst::Union(t2, *pi);
    }
    delete pi;

    fst::RmEpsilon(t2);
    return t2;
  }
  
  
  TransducerHandle make_replace_in_context( TransducerHandle t, Repl_Type type, ContextsHandle p, 
					    bool optional, KeyPairSet *Pi ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    Contexts *P = new Contexts();
    P->left = HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->left);
    P->right = HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->right);
    delete p;  // #######
    fst::StdVectorFst const * pResult = replace_in_context_(pT, type, P, optional, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  Key create_new_marker(fst::StdVectorFst *t, Contexts *p, KeyPairSet *Pi, Key k);

  fst::StdVectorFst *replace_in_context_( fst::StdVectorFst *t, Repl_Type type, Contexts *p,
					  bool optional, KeyPairSet *Pi ) {

    // redefine the alphabet
    //change_pair_set( TheAlphabet, *Pi );
    //if (!Alphabet_Defined)
    //  error("The replace operators require the definition of an alphabet");
    
    if (!is_automaton_(p->left) || !is_automaton_(p->right))
      error("The replace operators require automata as context expressions!");
    
    // create the marker symbols
    //Character leftm = TheAlphabet.new_marker();
    //Character rightm = TheAlphabet.new_marker();
    Key leftm = create_new_marker(t, p, Pi, 0);
    Key rightm = create_new_marker(t, p, Pi, leftm);
    
    // Create the insert boundary transducer (.|<>:<L>|<>:<R>)*    
    fst::StdVectorFst *ibt = define_pi_transducer_(Pi);
    ibt->AddArc(0, fst::StdArc(Label::epsilon, leftm, 0, 1));
    ibt->AddArc(0, fst::StdArc(Label::epsilon, rightm, 0, 1));
    fst::Closure(ibt, fst::CLOSURE_STAR);
    fst::RmEpsilon(ibt);

    // Create the remove boundary transducer (.|<L>:<>|<R>:<>)*    
    fst::StdVectorFst *rbt = define_pi_transducer_(Pi);
    rbt->AddArc(0, fst::StdArc(leftm, Label::epsilon, 0, 1));
    rbt->AddArc(0, fst::StdArc(rightm, Label::epsilon, 0, 1));
    fst::Closure(rbt, fst::CLOSURE_STAR);
    fst::RmEpsilon(rbt);

    // Add the markers to the alphabet
    //TheAlphabet.insert(Label(leftm));
    //TheAlphabet.insert(Label(rightm));
    KeyPair *leftm_kp = define_keypair(leftm, leftm);
    KeyPair *rightm_kp = define_keypair(rightm, rightm);
    Pi = insert_keypair( leftm_kp, Pi );
    Pi = insert_keypair( rightm_kp, Pi );

    
    // Create the constrain boundary transducer !(.*<L><R>.*)    
    // !( (.|<L>|<R>)* <L><R> (.|<L>|<R>)* )

    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure(pi, fst::CLOSURE_STAR);
    fst::StdVectorFst *cbt = pi->Copy();
    fst::Concat(cbt, make_fst_np(leftm));
    fst::Concat(cbt, make_fst_np(rightm));
    fst::Concat(cbt, *pi);
    //delete pi;
    cbt = negation_(cbt, Pi, true);
    fst::RmEpsilon(cbt);
    
    
    // Create the extended context transducers

    /* !( (!ct mt) | (ct !mt) ), where

    ct = .* (<L> >> (<R> >> $T$)) || !(.* <L>)
    mt = <R>* <L> .*   
    */

    // left context transducer .* (<R> >> (<L> >> $T$)) || !(.*<L>)    
    fst::StdVectorFst *lct = replace_context(p->left, leftm, rightm, Pi);
    delete p->left;

    // right context transducer:  (<R> >> (<L> >> $T$)) .* || !(<R>.*) // $T$ = c->right.reverse()
    fst::StdVectorFst *tmp = (p->right)->Copy();
    fst::StdVectorFst *right_reversed = new fst::StdVectorFst;
    fst::Reverse(*tmp, right_reversed);
    delete tmp;
    delete p->right;
    delete p;

    tmp = replace_context(right_reversed, rightm, leftm, Pi);
    delete right_reversed;

    fst::StdVectorFst *rct = new fst::StdVectorFst();
    fst::Reverse(*tmp, rct);
    delete tmp;


    // unconditional replace transducer
    
    fst::StdVectorFst *rt;
    if (type == repl_up || type == repl_right || type == repl_left)
      rt = replace_transducer( t, leftm, rightm, repl_up, Pi );
    else
      rt = replace_transducer( t, leftm, rightm, repl_down, Pi );
    delete t;

    
    // build the conditional replacement transducer
    
    fst::StdVectorFst *result = composition_(ibt, cbt, true);

    if (type == repl_up || type == repl_right) {
      tmp = result;
      result = composition_(result, rct);
      delete tmp;
    }
    if (type == repl_up || type == repl_left) {
      tmp = result;
      result = composition_(result, lct);
      delete tmp;
    }
    
    result = composition_(result, rt, true);
    
    if (type == repl_down || type == repl_right) {
      result = composition_(result, lct, true);
      lct=NULL;
    }
    if (type == repl_down || type == repl_left) {
      result = composition_(result, rct, true);
      rct=NULL;
    }
    
    if (lct!=NULL)
      delete lct;
    if (rct!=NULL)
      delete rct;

    result = composition_(result, rbt, true);
    
    // Remove the markers from the alphabet
    //TheAlphabet.delete_markers();
    
    Pi->erase(leftm_kp);
    Pi->erase(rightm_kp);
    delete leftm_kp;
    delete rightm_kp;
    delete pi;
    
    if (optional) {
      //fst::Union(result, make_epsilon_np());
      fst::StdVectorFst *pi_without_markers = define_pi_transducer_(Pi);
      fst::Closure(pi_without_markers, fst::CLOSURE_STAR);
      fst::Union(result, *pi_without_markers);
      delete pi_without_markers;
    }

    return minimize_(result);
  }  






  
  TransducerHandle make_rule( TransducerHandle t1, Range *r1, Twol_Type type,
			      Range *r2, TransducerHandle t2, KeyPairSet *Pi ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);
    fst::StdVectorFst* const pResult = make_rule_(pT1, r1, type, r2, pT2, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }


  /*TransducerHandle make_rule( TransducerHandle t1, TransducerHandle r, Twol_Type type,
			      TransducerHandle t2, KeyPairSet *Pi ) {
    fst::StdVectorFst* const pT1 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t1);
    fst::StdVectorFst* const pT2 = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t2);

    fst::StdVectorFst* const pR = HANDLE_TO_PINSTANCE(fst::StdVectorFst, r);
    fst::Reverse(*pR, pR);
    Range *r1 = NULL;
    Range *r2 = NULL;

    for (fst::StateIterator<fst::StdFst> siter(*pR); !siter.Done(); siter.Next()) {
      StateId state_id = siter.Value();
      for (fst::ArcIterator<fst::StdFst> aiter(*pR, state_id); !aiter.Done(); aiter.Next()) {
	const fst::StdArc &arc = aiter.Value();
	r1 = add_value( arc.ilabel, r1 );
	r2 = add_value( arc.olabel, r2 );
      }
    }

    fst::StdVectorFst* const pResult = make_rule_(pT1, r1, type, r2, pT2, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
    }*/


  TransducerHandle make_rule( TransducerHandle t1, KeyPairSet *mappings, Twol_Type type,
			      TransducerHandle t2, KeyPairSet *Pi ) {
    Range *r1 = ::HFST::create_empty_range();
    Range *r2 = ::HFST::create_empty_range();
    for ( KeyPairIterator it = begin_pi_key(mappings); it != end_pi_key(mappings); it++ ) {
      KeyPair *kp = get_pi_keypair(it);
      r1 = ::HFST::insert_value(get_input_key(kp), r1);
      r2 = ::HFST::insert_value(get_output_key(kp), r2);
    }
    return make_rule( t1, r1, type, r2, t2, Pi );
  }

  fst::StdVectorFst *make_rule_( fst::StdVectorFst *t1, Range *r1, Twol_Type type,
				 Range *r2, fst::StdVectorFst *t2, KeyPairSet *Pi ) {

    // change_pair_set( TheAlphabet, *Pi );
    
    Range *lower_range = r1;
    Range *upper_range = r2;

    if (RS.size() > 0 || RSS.size() > 0)
      fprintf(stderr, "\nWarning: agreement operation inside of replacement rule!\n");
    
    //if (!Alphabet_Defined)
    //  error("Two level rules require the definition of an alphabet");

    // expand the left and the right contexts to their full length
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure(pi, fst::CLOSURE_STAR);

    if (t1 == NULL)
      t1 = pi->Copy();
    else {
      fst::StdVectorFst *tmp = pi->Copy();
      fst::Concat(tmp, *t1);
      delete t1;
      t1 = tmp;
    }
    
    if (t2 == NULL)
      t2 = pi;
    else {
      fst::Concat(t2, *pi);
      delete pi;
    }


    fst::StdVectorFst *result = NULL;

    switch (type) {
    case twol_left:
      result = twol_left_rule(t1, lower_range, upper_range, t2, Pi);
      break;
    case twol_right:
      result = twol_right_rule(t1, lower_range, upper_range, t2, Pi);
      break;
    case twol_both:
      {
	fst::StdVectorFst *tr1 = twol_left_rule(t1, lower_range, upper_range, t2, Pi);
	fst::StdVectorFst *tr2 = twol_right_rule(t1, lower_range, upper_range, t2, Pi);
	result = intersection_(tr1, tr2, true);
      }
    }
    delete t1; delete t2; // #####

    if (lower_range != upper_range)
      free_values(lower_range);
    free_values(upper_range);
    
    result = minimize_(result);
    return result;
  }



  static fst::StdVectorFst *restriction_transducer( fst::StdVectorFst *l1, fst::StdVectorFst *l2,
						    Character marker, KeyPairSet *Pi );
  //fst::StdVectorFst *marker_transducer( fst::StdVectorFst *t, Contexts *c,
  //				Character &marker );
  static fst::StdVectorFst *center_transducer( fst::StdVectorFst *t, fst::StdVectorFst *pi, 
					fst::StdVectorFst *mt );
  static fst::StdVectorFst *context_transducer( fst::StdVectorFst *t, fst::StdVectorFst *pi,
					 fst::StdVectorFst *mt, Contexts *c );
  static fst::StdVectorFst *result_transducer( fst::StdVectorFst *l1, fst::StdVectorFst *l2,
					Twol_Type type, Character marker, KeyPairSet *Pi );



  TransducerHandle make_restriction( TransducerHandle t, Twol_Type type, ContextsHandle p,
				     int direction, KeyPairSet *Pi ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);

    Contexts *P = make_context_(HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->left),
				HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->right));
    Contexts *P_old = P;
    ContextsHandle tmp=p;
    p = p->next;
    delete tmp;
    while(p) {
      Contexts *Padd = make_context_(HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->left),
				     HANDLE_TO_PINSTANCE(fst::StdVectorFst, p->right));
      P = add_context_(P, Padd);
      P = Padd;
      tmp=p;
      p = p->next;
      delete p;
    }
    P = add_context_(P, NULL);
    P = P_old;
    
    fst::StdVectorFst const * pResult = restriction_(pT, type, P, direction, Pi);
    return PINSTANCE_TO_HANDLE(Transducer, pResult);
  }

  Key create_new_marker(fst::StdVectorFst *t, Contexts *p, KeyPairSet *Pi, Key k);

  // void delete_contexts( Contexts *p ); ???

  fst::StdVectorFst *restriction_( fst::StdVectorFst *t, Twol_Type type, Contexts *p, 
				   int direction, KeyPairSet *Pi ) {
    //Key marker;
    //fst::StdVectorFst *mt=marker_transducer( t, p, marker );
    Key marker = create_new_marker(t, p, Pi, 0);
    fst::StdVectorFst *mt = make_fst(marker);
    //Transducer_ *pi=pi_machine(TheAlphabet);
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure(pi, fst::CLOSURE_STAR);
    fst::StdVectorFst *l1=center_transducer( t, pi, mt );
    
    fst::StdVectorFst *tmp;
    if (direction == 0)
      tmp = pi;
    else if (direction == 1) {
      fst::Project( t, fst::PROJECT_INPUT );
      tmp = composition_(t, pi);
    }
    else {
      fst::Project( t, fst::PROJECT_OUTPUT );
      tmp = composition_(t, pi);
    }    
    fst::StdVectorFst *l2 = context_transducer(tmp, pi, mt, p);    
    fst::StdVectorFst *result = result_transducer(l1, l2, type, marker, Pi);
    if (tmp != pi)
      delete pi;
    delete tmp; delete l1; delete l2;  delete mt; delete t;
    return result;
  }



  //static fst::StdVectorFst *replace_char(fst::StdVectorFst *t, Character a, Character b);
  void print_arcs(stack<fst::StdArc> arc_stack);


  Key create_new_marker(fst::StdVectorFst *t, Contexts *p, KeyPairSet *Pi, Key k) {
    // collect to a set of keys all keys that are used in the arguments
    set<Key> keys = define_key_set_( t);    
    Contexts *P = p;
    while(P) {
      set<Key> left_keys = define_key_set_(P->left);
      set<Key> right_keys = define_key_set_(P->right);
      keys.insert(left_keys.begin(), left_keys.end());
      keys.insert(right_keys.begin(), right_keys.end());
      P = P->next;
    }
    for (KeyPairIterator it = begin_pi_key(Pi); it != end_pi_key(Pi); it++) {
      KeyPair *kp = get_pi_keypair(it);
      keys.insert( get_input_key(kp) );
      keys.insert( get_output_key(kp) );
    }
    keys.insert(k);
    // find a key that is not in use
    for( unsigned int i=1; i!=0; i++ ) {
      if( keys.find(i) == keys.end() )
	return i;
    }
    throw "ERROR: new marker key could not be created.\n";
  }



  fst::StdVectorFst *replace_char( fst::StdVectorFst *t, Character a, stack<fst::StdArc> b ) {

    if (b.empty())
      error("Third argument in function 'replace_char_new' is an empty stack.");

    int states = t->NumStates();
    for( int i = 0; i < states; ++i ) {
      for (fst::MutableArcIterator<fst::StdVectorFst> it(t,i);
	   not it.Done();
	   it.Next()) {
	fst::StdArc arc = it.Value();

	if ( arc.ilabel == a && arc.olabel == a ) {
	  fst::StdArc new_arc = b.top();
	  arc.ilabel = new_arc.ilabel;
	  arc.olabel = new_arc.olabel;
	  b.pop();
	  if(!b.empty()) {
	    
	  }
	}

	if ( arc.ilabel == a && arc.olabel == a ) {
	  while(!b.empty()) {
	    fst::StdArc new_arc = b.top();
	    arc.ilabel = new_arc.ilabel;
	    arc.olabel = new_arc.olabel;

	    b.pop();
	    (void)t->AddState();
	  }	  
	}
	it.SetValue(arc);
      }	
    }
    return t;
  }




  /* create [ l1 - l2 ], replace marker with epsilon, negate with respect to Pi. */
  static fst::StdVectorFst *restriction_transducer( fst::StdVectorFst *l1, fst::StdVectorFst *l2,
						    Key marker, KeyPairSet *Pi ) {
    fst::StdVectorFst *t1 = subtraction_(l1, l2);    
    t1 = substitute_key_(t1, marker, Label::epsilon);
    t1 = negation_(t1, Pi, true);
    return t1;
  }


  /*fst::StdVectorFst *marker_transducer( fst::StdVectorFst *t, Contexts *c,
					       Character &marker ) {
    marker = TheAlphabet.new_marker();
    fst::StdVectorFst *result = make_fst(marker, marker);
    return result;
    }*/


  /* create the concatenation  [ pi  mt  t  mt  pi ] */
  static fst::StdVectorFst *center_transducer( fst::StdVectorFst *t, fst::StdVectorFst *pi, 
					       fst::StdVectorFst *mt ) {
    fst::StdVectorFst *result = pi->Copy();
    fst::Concat(result, *mt);
    fst::Concat(result, *t);
    fst::Concat(result, *mt);
    fst::Concat(result, *pi);
    fst::RmEpsilon(result);
    return result;
  }

  /* Create [ pi + left_context + mt + pi + mt + right_context + pi ]
     for all contexts in c and disjunct them. */
  static fst::StdVectorFst *context_transducer( fst::StdVectorFst *t, fst::StdVectorFst *pi,
						fst::StdVectorFst *mt, Contexts *c ) {    
    // [ mt t mt ]
    fst::StdVectorFst *tmp = mt->Copy();
    fst::Concat(tmp, *t);
    fst::Concat(tmp, *mt);

    Contexts *tmp_c;
    fst::StdVectorFst *result = NULL;    
    while (c) {      
      // pi + c->left + tmp + c->right + pi
      fst::StdVectorFst *t2 = pi->Copy();
      fst::Concat(t2, *(c->left));
      fst::Concat(t2, *tmp);
      fst::Concat(t2, *(c->right));
      fst::Concat(t2, *pi);
      delete c->left; delete c->right; 

      if (result) {
	fst::Union(result, *t2);
	delete t2;
      }
      else
	result = t2;
      tmp_c=c;
      c = c->next;
      delete tmp_c;
    }
    delete tmp;
    return result;
  }


  static fst::StdVectorFst *result_transducer( fst::StdVectorFst *l1, fst::StdVectorFst *l2,
					       Twol_Type type, Character marker, KeyPairSet *Pi ) {
    if (type == twol_right)
      return restriction_transducer( l1, l2, marker, Pi );
    else if (type == twol_left)
      return restriction_transducer( l2, l1, marker, Pi );
    else if (type == twol_both) {
      fst::StdVectorFst *t1 = restriction_transducer( l1, l2, marker, Pi );
      fst::StdVectorFst *t2 = restriction_transducer( l2, l1, marker, Pi );
      return intersection_(t1, t2, true);
    }
    else
      throw "ERROR: result_transducer: invalid type\n";
  }


  // returns replace_( <L> (<L> >> (<R> >> $Center$)) <R>, type, false)
  fst::StdVectorFst *replace_transducer( fst::StdVectorFst *ct, Key lm, Key rm,
					 Repl_Type type, KeyPairSet *Pi ) {
    fst::StdVectorFst *Ct = ct->Copy();

    // insert boundary markers into the center transducer
    KeyPair *lmlm = define_keypair(lm, lm);
    KeyPair *rmrm = define_keypair(rm, rm);
    Ct = insert_freely_(Ct, rmrm);
    Ct = insert_freely_(Ct, lmlm);
    delete lmlm;
    delete rmrm;
    
    // add surrounding boundary markers to the center transducer
    //Ct = make_fst(lm, lm) + Ct + make_fst(rm, rm));
    fst::StdVectorFst *t = make_fst(lm, lm);
    fst::Concat(t, *Ct);
    delete Ct;
    fst::Concat(t, make_fst_np(rm, rm));

    fst::StdVectorFst *result = replace_(t, type, false, Pi);
    delete t;
    return result;
  }


  /* Whether a transducer with two states has an arc from initial to final state
     that has an input epsilon. */

  bool has_input_epsilon( fst::StdVectorFst *t ) {
    for (fst::ArcIterator<fst::StdFst> aiter(*t, t->Start()); !aiter.Done(); aiter.Next()) {
      const fst::StdArc &arc = aiter.Value();
      if (Label::epsilon == arc.ilabel)
	return true;
    }
    return false;
  }

  /* Whether t is a transducer that has only two states, an initial and a final one,
     and arcs only from initial to final state. */

  bool is_range_fst( fst::StdVectorFst *t ) {
    if ( t->NumStates() != 2 )
      return false;
    if ( is_cyclic_(t) )
      return false;
    if ( is_final( t, t->Start() ) )
      return false;
    return true;
  }



  static bool in_range( unsigned int c, Range *r );

  static bool generates_empty_string( fst::StdVectorFst *t ) {
    fst::Minimize(t);
    if ( is_final(t, t->Start()) )
      return true;
    return false;
  }

  static fst::StdVectorFst *twol_left_rule( fst::StdVectorFst *lc, Range *lower_range,
					    Range *upper_range, fst::StdVectorFst *rc,
					    KeyPairSet *Pi ) {
    // check for problematic insertion operations like "$L <> <= a $R"
    // where either $L or $R includes the empty string
    if (in_range(Label::epsilon, lower_range)) {
      if (generates_empty_string(lc))
	error("in two level rule: insertion operation with deletable left context!");
      if (generates_empty_string(rc))
	error("in two level rule: insertion operation with deletable right context!");
      cerr << "\nWarning: two level rule used for insertion operation (might produce unexpected results)\n";
    }

    // Build the rule transducer
    fst::StdVectorFst *t1 = anti_cp(lower_range, upper_range, Pi);    

    // Add the left and right contexts;  // lc + anti_cp + rc
    fst::StdVectorFst *t2 = lc->Copy();
    fst::Concat(t2, *t1);
    delete t1;
    fst::Concat(t2, *rc);
    fst::RmEpsilon(t2);

    //now created: lc + anti_cp + rc (.* l (a:. & !a:b) r .*)    
    return negation_(t2, Pi, true);
  }


  
  static fst::StdVectorFst *twol_right_rule( fst::StdVectorFst *lc, Range *lower_range,
					     Range *upper_range, fst::StdVectorFst *rc,
					     KeyPairSet *Pi ) {
    // Build the rule transducer
    fst::StdVectorFst *cpt = cp(lower_range, upper_range);
    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure(pi, fst::CLOSURE_STAR);
    
    // First unwanted language    !lc cpt pi
    fst::StdVectorFst *t1 = negation_(lc, Pi);
    fst::Concat(t1, *cpt);
    fst::Concat(t1, *pi);
    
    // Second unwanted language   pi cpt !rc 
    fst::StdVectorFst *t2 = pi;
    fst::Concat(t2, *cpt);
    delete cpt;
    fst::StdVectorFst *tmp = negation_(rc, Pi);
    fst::Concat(t2, *tmp);
    delete tmp;
   
    // negation of the union of unwanted languages
    fst::Union(t1, *t2);
    delete t2;
    fst::RmEpsilon(t1);

    return negation_(t1, Pi, true);
  }



  static fst::StdVectorFst *cp( Range *lower_range, Range *upper_range ) {
    return make_transducer(lower_range, upper_range);
  }

  static fst::StdVectorFst *anti_cp( Range *lower_range, Range *upper_range, KeyPairSet *Pi ) { // (a:. & !a:b) 
    Range *empty = NULL; 
    fst::StdVectorFst *t1 = make_transducer(lower_range, empty, Pi);

    fst::StdVectorFst *t2 = make_transducer(lower_range, upper_range);
    t2 = negation_(t2, Pi, true);
    
    fst::StdVectorFst *result = intersection_(t1, t2, true);
    return result;
  }


  fst::StdVectorFst *extended_context( fst::StdVectorFst *t, Key m1, Key m2, KeyPairSet *Pi ) {

    fst::StdVectorFst *T = t->Copy();
    // m1 >> (m2 >> t)
    KeyPair *m1m1 = define_keypair(m1, m1);
    KeyPair *m2m2 = define_keypair(m2, m2);
    T = insert_freely_( insert_freely_(T, m2m2), m1m1);
    delete m1m1;
    delete m2m2;
    // .* (m1 >> (m2 >> t))
    fst::StdVectorFst *t1 = define_pi_transducer_(Pi);
    fst::Closure(t1, fst::CLOSURE_STAR);
    fst::Concat(t1, *T);
    delete T;
    // !(.* m1)
    fst::StdVectorFst *t2 = define_pi_transducer_(Pi);
    fst::Closure(t2, fst::CLOSURE_STAR);
    fst::Concat(t2, make_fst_np(m1));
    t2 = negation_(t2, Pi, true);
    // .* (m1 >> (m2 >> t)) || !(.* m1)
    fst::StdVectorFst *result = composition_(t1, t2, true);
    
    return result;
  }

  /* !( (!ct mt) | (ct !mt) ), where

    ct = .* (m1 >> (m2 >> t)) || !(.* m1)
    mt = m2* m1 .*   
  */
  fst::StdVectorFst *replace_context( fst::StdVectorFst *t, Key m1, Key m2, KeyPairSet *Pi ) {

    fst::StdVectorFst *pi = define_pi_transducer_(Pi);
    fst::Closure(pi, fst::CLOSURE_STAR);
    // .* (m1 >> (m2 >> t)) || !(.* m1)
    fst::StdVectorFst *ct = extended_context(t, m1, m2, Pi);
    // m2* m1 .*
    fst::StdVectorFst *mt = make_fst(m2);
    fst::Closure(mt, fst::CLOSURE_STAR);
    fst::Concat(mt, make_fst_np(m1));
    fst::Concat(mt, *pi);
    delete pi;
    // !ct mt
    fst::StdVectorFst *t1 = negation_(ct, Pi);
    fst::Concat(t1, *mt);
    // ct !mt
    fst::StdVectorFst *t2 = ct;
    mt = negation_(mt, Pi, true);
    fst::Concat(t2, *mt);
    delete mt;
    // !( (!ct mt) | (ct !mt) )
    fst::Union(t1, *t2);
    fst::RmEpsilon(t1);
    delete t2;
    t1 = negation_(t1, Pi, true);
    return t1;
  }

  // Range and Ranges functions

  Range *create_empty_range() {
    return HFST::create_empty_range(); };
  Range *insert_value( Symbol c, Range *r ) {
    return HFST::insert_value(c, r); };
  Range *append_range( Range *r1, Range *r2 ) {
    return HFST::append_range(r1, r2); };
  //Range *complement_range( Range *r, KeySet *Si ) {
  //  return HFST::complement_range(r, (HFST::KeySet)Si); };
  KeyPairSet *define_pair_range(Range *r1, Range *r2, bool final) {
    return HFST::define_pair_range(r1, r2, final); };
  
  Ranges *create_empty_ranges() {
    return HFST::create_empty_ranges(); };
  Ranges *insert_range( Range *r, Ranges *rs ) {
    return HFST::insert_range(r, rs); };
  KeyPairSet *define_pair_ranges( Ranges *rs1, Ranges *rs2 ) {
    return HFST::define_pair_ranges(rs1, rs2); };

}


namespace HWFST {

  typedef fst::StdArc::StateId StateId;    


  // *** Defining states ***
  
  State create_state( TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    State s;
    s.state_id = pT->AddState();
    return s;
  }

  State set_final_state( State s, TransducerHandle t, float w ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pT->SetFinal(s.state_id, fst::TropicalWeight(w));
    return s;
  }


  // *** Defining transitions ***

  void define_transition( TransducerHandle t, State orig, KeyPair *p, State target, float w ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    pT->AddArc( orig.state_id, 
		fst::StdArc( get_input_key(p), get_output_key(p),
			     fst::TropicalWeight( w ), target.state_id ) ); 
  }


  // *** Using states ***

  State get_initial_state( TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    State s;
    s.state_id = pT->Start();
    return s;
  }

  bool is_final_state( State s, TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return is_final( pT, s.state_id );
  }

  float get_final_weight( State s, TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    return pT->Final(s.state_id).Value();
  }


  // *** Using transitions ( transitions come from TransitionIterator ) ***
  
  KeyPair *get_transition_keypair( Transition tr ) {
    return define_keypair(tr.ilabel, tr.olabel);
  }
  
  State get_transition_to( Transition tr ) {
    State s;
    s.state_id = tr.nextstate;
    return s;
  }
  
  float get_transition_weight( Transition tr ) {
    return tr.weight.Value();
  }


  // *** Iterators over transitions ***
  
  TransitionIterator begin_ti( TransducerHandle t, State s ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);    
    fst::ArcIterator<fst::StdFst> aiter(*pT, s.state_id);    
    return aiter;
  }

  void next_ti( TransitionIterator &ti ) {
    ti.Next();
  }

  bool is_end_ti( TransitionIterator ti ) {
    return ti.Done();
  }

  Transition get_ti_transition( TransitionIterator ti ) {
    return ti.Value();
  }



  /* \brief Set the state s as marked. */
  State set_marked_state( State s, TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    VisitedMap::const_iterator it = VTS.find(pT);
    if (it == VTS.end()) {
      VisitedStates *vs = new VisitedStates();
      VTS[pT]=vs;
      vs->insert(s.state_id);
      return s;
    }
    VisitedStates *vs = it->second;
    vs->insert(s.state_id);
    return s;
  }
  

  /* \brief Unset the state s as marked. */ 	 
  State unset_marked_state( State s, TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    VisitedMap::const_iterator it = VTS.find(pT);
    if (it == VTS.end())
      return s;
    VisitedStates *vs = it->second;
    VisitedStates::const_iterator it2 = vs->find(s.state_id);
    if (it2 == vs->end())
      return s;
    vs->erase(it2);
    return s;
  }
  

  /* \brief Whether state s has been marked. */ 	
  bool is_marked( State s, TransducerHandle t ) {
    fst::StdVectorFst *pT = HANDLE_TO_PINSTANCE(fst::StdVectorFst, t);
    VisitedMap::const_iterator it = VTS.find(pT);
    if (it == VTS.end())
      return false;
    VisitedStates *vs = it->second;
    VisitedStates::const_iterator it2 = vs->find(s.state_id);
    if (it2 == vs->end())
      return false;
    return true;
  }

}
