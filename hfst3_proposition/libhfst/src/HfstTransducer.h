#ifndef _HFST_TRANSDUCER_H_
#define _HFST_TRANSDUCER_H_
#include "implementations/SymbolDefs.h"
#include "implementations/GlobalSymbolTable.h"
#include "implementations/SfstTransducer.h"
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/LogWeightTransducer.h"
#include "implementations/FomaTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include <string>
#include <cassert>
#include <iostream>

/** A namespace for the HFST functions and datatypes. */
namespace hfst
{

  using hfst::symbols::KeyTable;
  using hfst::symbols::Key;
  using hfst::symbols::KeyPair;
  using hfst::symbols::KeyMap;
  using hfst::symbols::StringSymbolPair;
  using hfst::symbols::StringSymbolSet;
  using hfst::symbols::KeyPairVector;
  
  using hfst::implementations::SfstTransducer;
  using hfst::implementations::TropicalWeightTransducer;
  using hfst::implementations::TropicalWeightState;
  using hfst::implementations::TropicalWeightStateIterator;
  using hfst::implementations::LogWeightTransducer;
  using hfst::implementations::WeightedStrings;
  using hfst::implementations::WeightedString;
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

      @see HfstTransducer(HfstInputStream &in) **/
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

    /** Create a stream to standard in for reading binary transducers. */
    HfstInputStream(void);
    /** Create a stream to file \a filename in for reading binary transducers. 
	@pre The file exists. Otherwise, an exception is thrown. */
    HfstInputStream(const char* filename);
    ~HfstInputStream(void);
    /** Open this stream. 
	If the stream points to standard in, nothing is done. */
    void open(void);
    /** Close this stream.
	If the stream points to standard in, nothing is done. */
    void close(void);
    /** Whether this stream is open. */
    bool is_open(void);
    /** Whether this stream is at the end. */
    bool is_eof(void);
    /** Whether this stream can be read. */
    bool is_bad(void);
    /** Whether this stream can be read. */
    bool is_good(void);
    /** Whether the next element in the stream is an HfstTransducer. */
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
    /** Create a stream to standard out for writing binary transducers of type \a type. */
    HfstOutputStream(ImplementationType type);  // stdout
    /** Create a stream to file \a filename for writing binary transducers of type \a type. 
	If the file exists, it is overwritten. */
    HfstOutputStream(const std::string &filename,ImplementationType type);
    ~HfstOutputStream(void);  
    /** Write the transducer \a transducer in binary format to this stream. 
	@pre The stream has been opened, unless it points to standard out. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);
    /** Open this stream. 
	If the stream points to standard out, nothing is done. */
    void open(void);
    /** Close this stream. 
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
    KeyTable key_table;
    bool is_trie;

    TransducerImplementation implementation; 

    void harmonize(HfstTransducer &another);
    HfstTransducer &disjunct_as_tries(HfstTransducer &another,
				      ImplementationType type);  

#include "apply_schemas.h"

  public:
    /** An empty transducer, i.e. a transducer that does not recognize any string. 
	@note Use HfstTransducer("") to create an epsilon transducer.
     **/
    HfstTransducer(ImplementationType type);
    /** **/
    HfstTransducer(const std::string& utf8_str, 
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    /** **/
    HfstTransducer(const std::string& upper_utf8_str,
    		   const std::string& lower_utf8_str,
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    /** Read a binary transducer from stream \a in. 
	@pre ( in.is_eof() == in.is_bad() == false && in.is_fst() ). \
	Otherwise, an exception is thrown. 
	@see HfstInputStream **/
    HfstTransducer(HfstInputStream &in);
    /** A deep copy of transducer \a another. **/
    HfstTransducer(const HfstTransducer &another);
    /** An ordinary transducer equivalent to mutable transducer \a t. **/
    HfstTransducer(const HfstMutableTransducer &t);
    /** Delete operator for HfstTransducer. **/
    ~HfstTransducer(void);

    /** A transducer that recognizes the string pair "symbol:symbol". **/
    HfstTransducer(const std::string &symbol, ImplementationType type);
    /** A transducer that recognizes the string pair "isymbol:osymbol". **/
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type);

    /** If two transducers are equivalent, i.e. accept the same input/output string pairs with the same weights. **/
    static bool test_equivalence(HfstTransducer &one, HfstTransducer &another);

    /** Write transducer in AT & T format to FILE \a ofile. 
	@see \ref operator<<(std::ostream &out,HfstTransducer &t) **/
    void write_in_att_format(FILE * ofile);
    /** Read a transducer in AT & T format from FILE \a ifile. **/
    static HfstTransducer &read_in_att_format(FILE * ifile);

    /** Write transducer in AT & T format to file named \a filename.
	If the file exists, it is overwritten. If the file does not exist, it is created. */
    void write_in_att_format(const char * filename);
    /** Read a transducer in AT & T format from file named \a filename. 
	If the file does not exist, an exception is thrown. */
    static HfstTransducer &read_in_att_format(const char * filename);

    /** An equivalent transducer that has no epsilon:epsilon transitions. */
    HfstTransducer &remove_epsilons(ImplementationType type=UNSPECIFIED_TYPE);
    /** An equivalent transducer that has no state with two or more transitions whose input/output pairs are the same. */
    HfstTransducer &determinize(ImplementationType type=UNSPECIFIED_TYPE);
    /** An equivalent transducer with the smallest number of states. */
    HfstTransducer &minimize(ImplementationType type=UNSPECIFIED_TYPE);
    /** \brief A transducer containing \a n best paths of this. 
	In the case of a weighted transducer, best paths are defined as paths with the lowest weight.
	In the case of an unweighted transducer, best paths are defined as paths with the smallest number of transitions, i.e. the shortest paths. */
    HfstTransducer &n_best(int n,ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts any number of this. */
    HfstTransducer &repeat_star(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts one or more of this. */
    HfstTransducer &repeat_plus(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts n times this. */
    HfstTransducer &repeat_n(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts this from zero to n times, inclusive.*/
    HfstTransducer &repeat_n_minus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts this n times or more. */
    HfstTransducer &repeat_n_plus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts this from n to k times, inclusive. */
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k,
                       ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts this or the empty string. */
    HfstTransducer &optionalize(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer where the input and output labels of each transition are swapped. */
    HfstTransducer &invert(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts in reverse order all paths that are accpeted by this. */
    HfstTransducer &reverse(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts the input strings of this. */
    HfstTransducer &input_project(ImplementationType type=UNSPECIFIED_TYPE);
    /** A transducer that accepts the output strings of this. */
    HfstTransducer &output_project(ImplementationType type=UNSPECIFIED_TYPE);

    /** Store to \a results all string pairs that are recognised by the transducer. 
	@pre The transducer is acyclic. 
	@note If the transducer is cyclic, no guarantees are given how the function will
	behave. It might get stuck in an infinite loop or return any number of string pairs. 
	In the case of a cyclic transducer, use #n_best_paths instead. 
	@see #n_best_paths */
    void extract_strings(WeightedStrings<float>::Set &results);

    /** Substitute all symbols \a old_symbol with symbol \a new_symbol. */
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol,
			       ImplementationType type=UNSPECIFIED_TYPE);
    /** Substitute all transitions equal to \a old_symbol_pair with symbol pair \a new_symbol_pair. */
    HfstTransducer &substitute(const StringSymbolPair &old_symbol_pair,
			       const StringSymbolPair &new_symbol_pair);
    /** Substitute all transitions equal to \a symbol_pair with a copy of transducer \a transducer. 
	The copy of the transducer is attached to this->transducer with epsilon transitions on its
	start state and final states (that become ordinary states after substitution). The weight of
	the original transition is copied to the epsilon transition leading to the original start state. */
    HfstTransducer &substitute(const StringSymbolPair &symbol_pair,
			       HfstTransducer &transducer);
    /** Set the weights of all final states to \a weight. */
    HfstTransducer &set_final_weights(float weight);
    /** Transform all transition and state weights according to the function pointer \a func. */
    HfstTransducer &transform_weights(float (*func)(float));

    /** Compose this transducer with another. */
    HfstTransducer &compose(HfstTransducer &another,
			    ImplementationType type=UNSPECIFIED_TYPE);
    /** Concatenate this transducer with another. */
    HfstTransducer &concatenate(HfstTransducer &another,
				ImplementationType type=UNSPECIFIED_TYPE);
    /** Disjunct this transducer and another. */
    HfstTransducer &disjunct(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);
    /** Intersect this transducer and another. */
    HfstTransducer &intersect(HfstTransducer &another,
			      ImplementationType type=UNSPECIFIED_TYPE);
    /** Subtract another transducer from this. */
    HfstTransducer &subtract(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);

    // test
    void test_minimize(void);

    /** The type of the transducer. */
    ImplementationType get_type(void);
    /** Convert this transducer into an equivalent transducer in format \a type. 
	If a weighted transducer is converted into an unweighted one, all weights are lost. 
	In the reverse case, all weights are initialized to zero. */
    HfstTransducer &convert(ImplementationType type);
    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
    friend class HfstOutputStream;
    friend class HfstMutableTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };

  /** \brief A finite-state synchronous transducer that supports iterating through and adding states and transitions.      

     An example:

     HfstMutableTransducer t;  // Creates an empty transducer with one start state that is not final.
     HfstState initial = t.get_initial_state();
     HfstState s = t.add_state();
     t.set_final_weight(s, 0.5);
     t.add_transition(initial, "foo", "bar", 0.3, s);
     // Now t has one non-final initial state and a final state whose weight is 0.5.
     // It also has a transition with input symbol "foo" and output symbol "bar" with weight 0.3
     // leading from the initial state to the final state.
     // It thus accepts the string pair "foo":"bar" with weight 0.8.

     */
  /* This class is basically a wrapper for a TROPICAL_OFST_TYPE HfstTransducer.
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

  /** \brief A state iterator to a mutable transducer. */
  class HfstStateIterator
  {
  protected:
    hfst::implementations::TropicalWeightStateIterator tropical_ofst_iterator;
  public:
    /** Create a state iterator to mutable transducer \a t. */
    HfstStateIterator(const HfstMutableTransducer &t);
    ~HfstStateIterator(void);
    /** Whether the iterator is at end. */
    bool done();
    /** The current state pointed by the iterator. */
    HfstState value();
    /** Advance the iterator to the next state. */
    void next();
  };

  /** \brief A transition in a mutable transducer. 

      @note Transitions are only returned by transition iterators to mutable transducers. */
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
    /** The input symbol of the transition. */
    std::string get_input_symbol(void);
    /** The output symbol of the transition. */
    std::string get_output_symbol(void);
    /** The weight of the transition. */
    HfstWeight get_weight(void);
    /** The target state of the transition. */
    HfstState get_target_state(void);
    friend class HfstTransitionIterator;
  };

  /** \brief A transition iterator to a mutable transducer. */
  class HfstTransitionIterator
  {
  protected:
    hfst::implementations::TropicalWeightTransitionIterator tropical_ofst_iterator;
  public:
    /** Create a transition iterator to state \a s in the mutable transducer \a t. */
    HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s);
    ~HfstTransitionIterator(void);
    /** Whether the iterator is at end. */
    bool done();
    /** The current transition pointed by the iterator. */
    HfstTransition value();
    /** Advance the iterator to the next transition. */
    void next();    
  };


  /** The same as \ref HfstTransducer::write_in_att_format. 
      @see \ref HfstTransducer::write_in_att_format. */
  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);
}



#endif
