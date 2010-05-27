#ifndef _HFST_TRANSDUCER_H_
#define _HFST_TRANSDUCER_H_
#include "implementations/SymbolDefs.h"
#include "implementations/SfstTransducer.h"
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/LogWeightTransducer.h"
#include "implementations/FomaTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include "implementations/HfstExceptions.h"
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <set>

/** \mainpage 

    \section first_section Parameter handling

    All transducer functions come in two forms, static and non-static.
    In the case of static unary and binary operators, a new transducer is created
    and the argument(s) is/are not modified:
\verbatim
    HfstTransducer t_rev = reverse(t);         // t remains unchanged
    HfstTransducer t_disj = disjunct(t1, t2);  // t1 and t2 remain unchanged
\endverbatim

    In the case of non-static unary operators, the transducer itself is modified and
    returned by the function.
    In the case of non-static binary operators, the argument remains unmodified but
    the transducer itself is modified and returned by the function:
\verbatim
    t = t->reverse(t);       // t is modifed
    t1 = t1->disjunct(t2);   // t2 remains unchanged, a copy of t2 is disjuncted with t1
\endverbatim

    All transducer functions take an \ref hfst::ImplementationType parameter, that defines what underlying
    library is used. The default value of this parameter is \ref hfst::UNSPECIFIED_TYPE meaning that
    the type of the calling transducer (in the case of non-static functions) or the (first) argument 
    (in the case of static functions) is chosen as the implementation type of that function and the 
    return value is also of the same type:
\verbatim
    HfstTransducer t_rev = reverse(t_tropical_ofst);             // t_rev has type TROPICAL_OFST_TYPE 
    HfstTransducer t_rev = reverse(t_tropical_ofst, FOMA_TYPE);  // t_rev has type FOMA_TYPE 
    t_tropical_ofst->reverse();                                  // t_tropical_ofst is modified but still has type TROPICAL_OFST_TYPE 
    t_tropical_ofst->reverse(FOMA_TYPE);                         // the type of t_tropical_ofst is changed to FOMA_TYPE

    HfstTransducer t_disj = disjunct(t_tropical_ofst, t_foma);                   // t_disj has type TROPICAL_OFST
    HfstTransducer t_disj = disjunct(t_tropical_ofst, t_foma, "LOG_OFST_TYPE");  // t_disj has type LOG_OFST_TYPE
    t_tropical_ofst->disjunct(t_foma);                                           // t_tropical_ofst is modified but still has type TROPICAL_OFST
    t_tropical_ofst->disjunct(t_foma, "LOG_OFST_TYPE");                          // the type of t_tropical_ofst is changed to LOG_OFST_TYPE
\endverbatim

    The implementation types may be converted by using the function \ref hfst::HfstTransducer::convert :
\verbatim
    HfstTransducer t_sfst = convert(t_tropical_ofst, "SFST_TYPE");  // t_sfst has type SFST_TYPE
    t_tropical_ofst->convert("SFST_TYPE");                          // the type of t_tropical_ofst is changed to SFST_TYPE
\endverbatim

    \section second_section Creating transducers

    The class \ref hfst::HfstTransducer offers constructors for empty, epsilon, one-transition and path transducers.
    More complex transducers can be created by combining these transducers with various operations.
    It is also possible to read transducers in AT&T format from a stream and convert them into binary
    format with \ref hfst::HfstTransducer::read_in_att_format. Transducers can also be created from scratch with functions in \ref hfst::HfstMutableTransducer.

    HfstTransducers consist of states, one of which is the initial state and transitions between these states.
    A state can be final or non-final. The transitions consist of an input and an output symbol (strings) and, in the case
    of TROPICAL_OFST_TYPE and LOG_OFST_TYPE, a weight (float). Final states also have a weight in these cases.

    Special symbols are &amp;_EPSILON_SYMBOL_&amp; (reserved for epsilon), &amp;_UNKNOWN_SYMBOL_&amp; (reserved for unknown) and
    &amp;_IDENTITY_SYMBOL_&amp; (reserved for identity). More on unknown and identity symbols...

*/


/** \brief A namespace for the HFST functions and datatypes. */
namespace hfst
{
  
  using hfst::implementations::SfstTransducer;
  using hfst::implementations::TropicalWeightTransducer;
  using hfst::implementations::TropicalWeightState;
  using hfst::implementations::TropicalWeightStateIterator;
  using hfst::implementations::LogWeightTransducer;
  using hfst::WeightedPaths;
  using hfst::WeightedPath;
  using hfst::implementations::FomaTransducer;







  /** \brief The type of an HfstTransducer. */
  enum ImplementationType
  {
    SFST_TYPE, /**< An SFST transducer, unweighted. */
    TROPICAL_OFST_TYPE, /**< An OpenFst transducer with tropical weights. */
    LOG_OFST_TYPE, /**< An OpenFst transducer with logarithmic weights. */
    FOMA_TYPE, /**< A foma transducer, unweighted. */
    UNSPECIFIED_TYPE, /**< No type specified, internal default type will be used.  This type can be given as a parameter to a constructor or a function,
			 if the implementation type is not relevant. */
    ERROR_TYPE /**< Type not recognised. 
		  This type might be returned by a function if an error occurs. */
  };


  class HfstTransducer;

  class HfstMutableTransducer;

  /** \brief A stream for reading binary transducers. 

      An example:
\verbatim
      try {
        HfstInputStream in("testfile");
       } catch (HfstInterfaceException e) {
         printf("ERROR: File does not exist.\n");
         exit(1);
       }
       in.open();
       int n=0;
       while (not in.is_eof()) {
         if (in.is_bad()) {
	   printf("ERROR: Stream to file cannot be read.\n");
	   exit(1); 
	 }
	 if (not in.is_fst()) {
	   printf("ERROR: Stream to file cannot be read.\n");
	   exit(1); 
	 }
         HfstTransducer t(in);
	 printf("One transducer succesfully read.\n");
	 n++;
       }
       printf("\nRead %i transducers in total.\n", n);
       in.close();
\endverbatim

      @see HfstTransducer::HfstTransducer(HfstInputStream &in) **/
  class HfstInputStream
  {
  protected:

    union StreamImplementation
    {
      hfst::implementations::SfstInputStream * sfst;
      hfst::implementations::TropicalWeightInputStream * tropical_ofst;
      hfst::implementations::LogWeightInputStream * log_ofst;
      hfst::implementations::FomaInputStream * foma;
    };

    ImplementationType type;
    StreamImplementation implementation;
    bool has_header;
    void read_transducer(HfstTransducer &t);
    ImplementationType stream_fst_type(const char *filename);
    int read_library_header(std::istream &in);
    ImplementationType read_version_3_0_fst_type(std::istream &in);
  public:

    /** \brief Create a stream to standard in for reading binary transducers. */
    HfstInputStream(void);
    /** \brief Create a stream to file \a filename in for reading binary transducers. 

	@pre The file exists. Otherwise, an exception is thrown. */
    HfstInputStream(const char* filename);
    ~HfstInputStream(void);
    /** \brief Open this stream. 

	If the stream points to standard in, nothing is done. */
    void open(void);
    /** \brief Close this stream.

	If the stream points to standard in, nothing is done. */
    void close(void);
    /** \brief Whether this stream is open. */
    bool is_open(void);
    /** \brief Whether this stream is at the end. */
    bool is_eof(void);
    /** \brief Whether this stream can be read. */
    bool is_bad(void);
    /** \brief Whether this stream can be read. */
    bool is_good(void);
    /** \brief Whether the next element in the stream is an HfstTransducer. */
    bool is_fst(void);
    friend class HfstTransducer;
  };

  /** \brief A stream for writing binary transducers. **/
  class HfstOutputStream
  {
  protected:
    union StreamImplementation
    {
      hfst::implementations::LogWeightOutputStream * log_ofst;
      hfst::implementations::TropicalWeightOutputStream * tropical_ofst;
      hfst::implementations::SfstOutputStream * sfst;
      hfst::implementations::FomaOutputStream * foma;
    };
    ImplementationType type;
    StreamImplementation implementation;

  public:
    /** \brief Create a stream to standard out for writing binary transducers of type \a type. */
    HfstOutputStream(ImplementationType type);  // stdout
    /** \brief Create a stream to file \a filename for writing binary transducers of type \a type. 

	If the file exists, it is overwritten. */
    HfstOutputStream(const std::string &filename,ImplementationType type);
    ~HfstOutputStream(void);  
    /** \brief Write the transducer \a transducer in binary format to this stream. 

	@pre The stream has been opened, unless it points to standard out. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);
    /** \brief Open this stream. 

	If the stream points to standard out, nothing is done. */
    void open(void);
    /** \brief Close this stream. 

	If the stream points to standard out, nothing is done. */
    void close(void);
  };


  /** A handle for a state in a HfstMutableTransducer. **/
  typedef hfst::implementations::StateId HfstState;
  /** An internal weight is represented as a float in the interface. */
  typedef float HfstWeight;

  /** \brief A synchronous finite-state transducer.

    Currently, an HFST transducer has four implementation types as defined by
    the enumeration \ref ImplementationType.
  */
  class HfstTransducer
  {
  protected:
    union TransducerImplementation
    {
      hfst::implementations::Transducer * sfst;
      hfst::implementations::StdVectorFst * tropical_ofst;
      hfst::implementations::LogFst * log_ofst;
      fsm * foma;
      hfst::implementations::StdVectorFst * internal; 
    };
    
    static hfst::implementations::SfstTransducer sfst_interface;
    static hfst::implementations::TropicalWeightTransducer tropical_ofst_interface;
    static hfst::implementations::LogWeightTransducer log_ofst_interface;
    static hfst::implementations::FomaTransducer foma_interface;

    ImplementationType type;

    bool anonymous;
    //KeyTable key_table;
    bool is_trie;

    TransducerImplementation implementation; 

    void harmonize(HfstTransducer &another);
    HfstTransducer &disjunct_as_tries(HfstTransducer &another,
				      ImplementationType type);  

#include "apply_schemas.h"

  public:
    /** \brief Create an empty transducer, i.e. a transducer that does not recognize any string. 

	@note Use HfstTransducer("") to create an epsilon transducer.
     **/
    HfstTransducer(ImplementationType type);
    /** \brief Create a transducer by tokenizing the utf8 string \a utf8_string
	with tokenizer \a multichar_symbol_tokenizer.
	The type of the transducer is defined by \a type. **/
    HfstTransducer(const std::string& utf8_str, 
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    /** \brief Create a transducer by tokenizing 
	the utf8 input string \a input_utf8_string
	and output string \a output_utf8_string
	with tokenizer \a multichar_symbol_tokenizer.
	The type of the transducer is defined by \a type. **/
    HfstTransducer(const std::string& input_utf8_str,
    		   const std::string& output_utf8_str,
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    /** \brief Read a binary transducer from stream \a in. 

	@pre ( in.is_eof() == in.is_bad() == false && in.is_fst() ).
	Otherwise, an exception is thrown. 
	@see HfstInputStream **/
    HfstTransducer(HfstInputStream &in);
    /** \brief A deep copy of transducer \a another. **/
    HfstTransducer(const HfstTransducer &another);
    /** \brief An ordinary transducer equivalent to mutable transducer \a t. **/
    HfstTransducer(const HfstMutableTransducer &t);
    /** \brief Delete operator for HfstTransducer. **/
    ~HfstTransducer(void);

    /** \brief A transducer that recognizes the string pair "symbol:symbol". **/
    HfstTransducer(const std::string &symbol, ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that recognizes the string pair "isymbol:osymbol". **/
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type=UNSPECIFIED_TYPE);

    /** \brief If two transducers are equivalent, i.e. accept the same input/output string pairs with the same weights. **/
    static bool test_equivalence(const HfstTransducer &one, const HfstTransducer &another);

    /** \brief Write transducer in AT & T format to FILE \a ofile. 

	If several transducers are written in the same file, they must be separated by a line
	of two consecutive hyphens "--".

An example:
\verbatim
HfstTransducer foobar("foo","bar");
HfstTransducer epsilon("");
HfstTransducer empty();
HfstTransducer a_star("a");
a_star.repeat_star();

FILE * ofile = fopen("testfile.att", "wb");
foobar.write_in_att_format(ofile);
fprintf(ofile, "--\n");
epsilon.write_in_att_format(ofile);
fprintf(ofile, "--\n");
empty.write_in_att_format(ofile);
fprintf(ofile, "--\n");
a_star.write_in_att_format(ofile);
fclose(ofile);
\endverbatim

This will yield a file "testfile.att" that looks as follows:
\verbatim
0    1    foo  bar  0.0
1    0.0
--
0    0.0
--
--
0    0.0
0    0    a    a    0.0
\endverbatim

	@see hfst::operator<<(std::ostream &out,HfstTransducer &t) read_in_att_format **/
    void write_in_att_format(FILE * ofile);
    /** \brief Read a transducer in AT & T format from FILE \a ifile. 
	
	Lines are of the form "source_state TAB destination_state TAB input_symbol TAB output_symbol (TAB weight)"
	or "final_state (TAB weight)". If several transducers are listed in the same file, they are separated
	by lines of two consecutive hyphens "--".

An example:
\verbatim
0    1    foo  bar  0.3
1    0.5
--
0    0.0
--
--
0    0.0
0    0    a    a    0.2
\endverbatim

        The example lists four transducers in AT&T format: one transducers accepting the string pair "foo:bar", one
	epsilon transducer, one empty transducer and one transducer accepting any number of 'a's. The transducers
	can be read with the following commands from file named "testfile.att":
\verbatim
FILE * ifile = fopen("testfile.att", "rb");
while (not eof(ifile))
  {
  HfstTransducer t = HfstTransducer::read_in_att_format(ifile);
  }
fclose(ifile);
\endverbatim

@see write_in_att_format
**/
    static HfstTransducer &read_in_att_format(FILE * ifile, ImplementationType type=UNSPECIFIED_TYPE );

    /** \brief Write transducer in AT & T format to file named \a filename.

	If the file exists, it is overwritten. If the file does not exist, it is created. */
    void write_in_att_format(const char * filename);
    /** \brief Read a transducer in AT & T format from file named \a filename. 

	If the file does not exist, an exception is thrown. */
    static HfstTransducer &read_in_att_format(const char * filename, ImplementationType type=UNSPECIFIED_TYPE );

    /** \brief An equivalent transducer that has no epsilon:epsilon transitions. */
    HfstTransducer &remove_epsilons(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief An equivalent transducer that has no state with two or more transitions whose input/output pairs are the same. */
    HfstTransducer &determinize(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief An equivalent transducer with the smallest number of states. */
    HfstTransducer &minimize(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer containing \a n best paths of this. 

	In the case of a weighted transducer (TROPICAL_OFST_TYPE or LOG_OFST_TYPE), best paths are defined as paths with the lowest weight.
	In the case of an unweighted transducer (SFST_TYPE or FOMA_TYPE), best paths are defined as paths with the smallest number of transitions, i.e. the shortest paths. */
    HfstTransducer &n_best(int n,ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts any number of this. */
    HfstTransducer &repeat_star(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts one or more of this. */
    HfstTransducer &repeat_plus(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts n times this. */
    HfstTransducer &repeat_n(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts this from zero to n times, inclusive.*/
    HfstTransducer &repeat_n_minus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts this n times or more. */
    HfstTransducer &repeat_n_plus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts this from n to k times, inclusive. */
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts this or the empty string. */
    HfstTransducer &optionalize(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer where the input and output labels of each transition are swapped. */
    HfstTransducer &invert(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts in reverse order all paths that are accepted by this. */
    HfstTransducer &reverse(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts the input strings of this. */
    HfstTransducer &input_project(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer that accepts the output strings of this. */
    HfstTransducer &output_project(ImplementationType type=UNSPECIFIED_TYPE);

    /** \brief Store to \a results all string pairs that are recognised by the transducer. 

	@pre The transducer is acyclic. 
	@note If the transducer is cyclic, no guarantees are given how the function will
	behave. It might get stuck in an infinite loop or return any number of string pairs. 
	In the case of a cyclic transducer, use #n_best instead. 
	@see #n_best */
    void extract_strings(WeightedPaths<float>::Set &results, ImplementationType type=UNSPECIFIED_TYPE);

    /** \brief Substitute all symbols \a old_symbol with symbol \a new_symbol. */
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol,
			       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Substitute all transitions equal to \a old_symbol_pair with symbol pair \a new_symbol_pair. */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPair &new_symbol_pair,
			       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Substitute all transitions equal to \a symbol_pair with a copy of transducer \a transducer. 

	The copy of the transducer is attached to this->transducer with epsilon transitions on its
	start state and final states (that become ordinary states after substitution). The weight of
	the original transition is copied to the epsilon transition leading to the original start state. */
    HfstTransducer &substitute(const StringPair &symbol_pair,
			       HfstTransducer &transducer,
			       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Set the weights of all final states to \a weight. */
    HfstTransducer &set_final_weights(float weight,
				      ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Transform all transition and state weights according to the function pointer \a func. 

     An example:
\verbatim
    float func(float f) { 
      return 2*f + 0.5; 
    }

    // All transition and final weights are multiplied by two and summed with 0.5.
    HfstTransducer t_transformed =
      t.transform_weights(&func);
\endverbatim 
    */
    HfstTransducer &transform_weights(float (*func)(float),
				      ImplementationType=UNSPECIFIED_TYPE);


    /** \brief Compose this transducer with another. */
    HfstTransducer &compose(HfstTransducer &another,
			    ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Concatenate this transducer and another. */
    HfstTransducer &concatenate(HfstTransducer &another,
				ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Disjunct this transducer and another. */
    HfstTransducer &disjunct(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Intersect this transducer and another. */
    HfstTransducer &intersect(HfstTransducer &another,
			      ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Subtract another transducer from this. */
    HfstTransducer &subtract(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);


    /** \brief Compose this transducer with another. */
    static HfstTransducer &compose(HfstTransducer &t1, HfstTransducer &t2,
				   ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Concatenate this transducer and another. */
    static HfstTransducer &concatenate(HfstTransducer &t1, HfstTransducer &t2, 
				       ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Disjunct this transducer and another. */
    static HfstTransducer &disjunct(HfstTransducer &t1, HfstTransducer &t2,
				    ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Intersect this transducer and another. */
    static HfstTransducer &intersect(HfstTransducer &t1, HfstTransducer &t2,
				     ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief Subtract another transducer from this. */
    static HfstTransducer &subtract(HfstTransducer &t1, HfstTransducer &t2,
				    ImplementationType type=UNSPECIFIED_TYPE);


    // test
    void test_minimize(void);

    /** \brief The type of the transducer. */
    ImplementationType get_type(void);

    /** \brief Convert this transducer into an equivalent transducer in format \a type. 

	If a weighted transducer is converted into an unweighted one, all weights are lost. 
	In the reverse case, all weights are initialized to zero. */
    HfstTransducer &convert(ImplementationType type);

    /** \brief Create a new transducer equivalent to \a t in format \a type. 

	@see convert(ImplementationType type) */
    static HfstTransducer &convert(const HfstTransducer &t, ImplementationType type);

    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
    friend class HfstOutputStream;
    friend class HfstMutableTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };

  /** \brief A finite-state synchronous transducer that supports iterating through and adding states and transitions.      


     An example of constructing a transducer from scratch:
\verbatim
     HfstMutableTransducer t;       // Create an empty transducer with one start state that is not final.
     HfstState initial = 
        t.get_initial_state();      // Get a handle for the start state
     HfstState s = t.add_state();   // Add a second state that is initially not final
     t.set_final_weight(s, 0.5);    // Set the second state final with weight 0.5
     t.add_transition(initial,      // Add a transition leading from the first state
                     "foo",         // with input symbol "foo"
		      "bar",         // and output symbol "bar"
		      0.3,           // with weight 0.3
		      s);            // to the second state

     // Now t accepts the string pair "foo":"bar" with weight 0.8.
\endverbatim

     \anchor iterator_example An example of iterating through states and transitions:
\verbatim
// print a transducer in AT&T format to standard out
void print(HfstMutableTransducer &t)
{
  HfstStateIterator state_it(t);
  while (not state_it.done()) 
    {
      HfstState s = state_it.value();
      HfstTransitionIterator transition_it(t,s);
      while (not transition_it.done()) 
        {
          HfstTransition tr = transition_it.value();
          cout << s << "\t" << tr.get_target_state() << "\t"
	        << tr.get_input_symbol() << "\t" 
               << tr.get_output_symbol()
	        << "\t" << tr.get_weight() << "\n";
          transition_it.next();
        }
      if ( t.is_final(s) )
        cout << s << "\t" << t.get_final_weight(s) << "\n";
      state_it.next();
    }
  return;
}
\endverbatim

     @see HfstStateIterator HfstTransitionIterator
     */
  /*! \internal This class is basically a wrapper for a TROPICAL_OFST_TYPE HfstTransducer.
     Since creating new states and transitions and modifying them is easiest
     in OpenFst, it is chosen as the only implementation type.
     A separate mutable and iterable transducer class is also safer because
     it does not interact with other operations. */
  class HfstMutableTransducer
  {
  protected:
    HfstTransducer transducer;
  public:
    /** Construct an empty mutable transducer. 
	I.e. a transducer with one non-final initial state. c*/
    HfstMutableTransducer(void);
    /** Construct a mutable transducer equivalent to \a t. */
    HfstMutableTransducer(const HfstTransducer &t);
    /** A deep copy of mutable transducer \a t. */
    HfstMutableTransducer(const HfstMutableTransducer &t);
    /** Delete mutable transducer. */
    ~HfstMutableTransducer(void);
    /** Add a state to this mutable transducer and return a handle to the state. */
    HfstState add_state();
    /** Set the value of the final weight of state \a s in this mutable transducer to \a w. */
    void set_final_weight(HfstState s, float w);
    /** If state \a s in this mutable transducer is final. */
    bool is_final(HfstState s);
    /** Return a handle to the initial state in this mutable transducer. */
    HfstState get_initial_state();
    /** The final weight of state \a s in this mutable transducer. 
	@pre State \a s must be final. */
    float get_final_weight(HfstState s);
    /** Add transition with input and output symbols \a isymbol and \a osymbol and weight \a w
	between states \a source and \a target in this mutable transducer. */
    void add_transition(HfstState source, std::string isymbol, std::string osymbol, float w, HfstState target);
    /* friend classes */
    friend class HfstTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };

  /** \brief A state iterator to a mutable transducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
  class HfstStateIterator
  {
  protected:
    hfst::implementations::TropicalWeightStateIterator tropical_ofst_iterator;
  public:
    /** \brief Create a state iterator to mutable transducer \a t. */
    HfstStateIterator(const HfstMutableTransducer &t);
    ~HfstStateIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current state pointed by the iterator. */
    HfstState value();
    /** \brief Advance the iterator to the next state. */
    void next();
  };

  /** \brief A transition in a mutable transducer. 

      Transitions are only returned by an \ref HfstTransitionIterator. 

      For an example of using transitions, see \ref iterator_example "this". */
  class HfstTransition
  {
  protected:
    std::string isymbol;
    std::string osymbol;
    HfstWeight weight;
    HfstState target_state;
    HfstTransition(std::string isymbol, std::string osymbol, HfstWeight weight, HfstState target_state);
  public:
    ~HfstTransition(void);
    /** \brief The input symbol of the transition. */
    std::string get_input_symbol(void);
    /** \brief The output symbol of the transition. */
    std::string get_output_symbol(void);
    /** \brief The weight of the transition. */
    HfstWeight get_weight(void);
    /** \brief The target state of the transition. */
    HfstState get_target_state(void);
    friend class HfstTransitionIterator;
  };

  /** \brief A transition iterator to a mutable transducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
  class HfstTransitionIterator
  {
  protected:
    hfst::implementations::TropicalWeightTransitionIterator tropical_ofst_iterator;
  public:
    /** \brief Create a transition iterator to state \a s in the mutable transducer \a t. */
    HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s);
    ~HfstTransitionIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current transition pointed by the iterator. */
    HfstTransition value();
    /** \brief Advance the iterator to the next transition. */
    void next();    
  };


  /** \brief The same as \ref HfstTransducer::write_in_att_format */
  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);
}



#endif
