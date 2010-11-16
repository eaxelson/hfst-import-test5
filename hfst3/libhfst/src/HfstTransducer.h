//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.
#ifndef _HFST_TRANSDUCER_H_
#define _HFST_TRANSDUCER_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include "implementations/SymbolDefs.h"

#if HAVE_SFST
#include "implementations/SfstTransducer.h"
#endif

#if HAVE_OPENFST
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/LogWeightTransducer.h"
#endif

#if HAVE_FOMA
#include "implementations/FomaTransducer.h"
#endif

#include "implementations/HfstAlphabet.h"
#include "implementations/HfstOlTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include "implementations/HfstExceptions.h"

#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <set>


/** \brief A namespace for HFST functions and datatypes. */
namespace hfst
{

/** \mainpage 

HFST - The Helsinki Finite-State Transducer technology is intended for creating and manipulating weighted or unweighted synchronic transducers implementing regular relations.

Currently HFST has been implemented using the SFST, OpenFst and foma software libraries.
Other versions may be added in some future release. 
SFST and foma implementations are unweighted and OpenFst implementation is weighted.
More info on SFST tools is in http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html,
OpenFst in http://www.openfst.org and foma in http://www.aclweb.org/anthology/E/E09/E09-2008.pdf.

The examples use Xerox transducer notations ( http://www.xrce.xerox.com/Research-Development/Publications/1997-005/(language) ).

The HFST API is written in the namespace #hfst that contains the following classes and namespaces:

   - HfstTransducer: a class for creating transducers and performing operations on them.

   - \link hfst::rules rules\endlink: a namespace that contains functions to create two-level, replace, restriction and coercion rules.

   - HfstInputStream and HfstOutputStream: classes for writing and reading binary transducers.

   - HfstMutableTransducer, HfstTransition, HfstTransitionIterator and HfstStateIterator: classes for creating transducers from scratch and iterating through their states and transitions.

   - HfstGrammar ?

   - \link hfst::exceptions exceptions\endlink: a namespace for exceptions.

*/

  class HfstCompiler;
  class HfstTransducer;

  using hfst::implementations::HfstOlTransducer;
  using hfst::WeightedPaths;
  using hfst::WeightedPath;

#if HAVE_SFST
  using hfst::implementations::SfstTransducer;
#endif

#if HAVE_OPENFST
  using hfst::implementations::TropicalWeightTransducer;
  using hfst::implementations::TropicalWeightState;
  using hfst::implementations::TropicalWeightStateIterator;
  using hfst::implementations::LogWeightTransducer;
#endif

#if HAVE_FOMA
  using hfst::implementations::FomaTransducer;
#endif


  // ENUMS AND TYPEDEFS...

  /** \brief The type of an HfstTransducer. */
  enum ImplementationType 
  {
    SFST_TYPE, /**< An SFST transducer, unweighted. */
    TROPICAL_OFST_TYPE, /**< An OpenFst transducer with tropical weights. */
    LOG_OFST_TYPE, /**< An OpenFst transducer with logarithmic weights. */
    FOMA_TYPE, /**< A foma transducer, unweighted. */
    HFST_OL_TYPE, /**< An HFST optimized lookup transducer, unweighted */
    HFST_OLW_TYPE, /**< An HFST optimized lookup transducer with weights */
    HFST2_TYPE, /**< HFST2 header present, conversion required */
    UNSPECIFIED_TYPE, /**< Format left open by e.g. default constructor */
    ERROR_TYPE /**< Type not recognised. 
		  This type might be returned by a function if an error occurs. */ 
  };

  /** \brief The type of a push operation.
      @see hfst::implementations::HfstTransducer::push_weights */
  enum PushType
  { 
    TO_INITIAL_STATE /**< Push weights towards initial state. */,
    TO_FINAL_STATE /**< Push weights towards final state(s). */ 
  };

  //! @brief A pair of transducers
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::pair <HfstTransducer,HfstTransducer> HfstTransducerPair;
  //! @brief A set of transducer pairs
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::set <HfstTransducerPair> HfstTransducerPairSet;
  //! @brief A vector of transducer pairs
  //!
  //! Used by functions in namespace \link hfst::rules rules\endlink
  typedef std::vector <HfstTransducerPair> HfstTransducerPairVector;

  typedef std::vector<std::string> HfstArcPath;
  //! @brief A path of one level of arcs with collected weight,
  //!
  //! Used as the source and result data type for lookups and downs.
  typedef std::pair<HfstArcPath,float> HfstLookupPath;
  //! @brief A set of simple paths.
  //!
  //! Used as return type of lookup with multiple, unique results.
  typedef std::set<HfstLookupPath> HfstLookupPaths;

#if HAVE_MUTABLE
  class HfstMutableTransducer;
#endif
  typedef class hfst::implementations::HfstInternalTransducer HfstMutableTransducer;
  typedef class hfst::implementations::HfstTransition HfstTransition;
  typedef class hfst::implementations::HfstStateIterator HfstStateIterator;
  typedef class hfst::implementations::HfstTransitionIterator HfstTransitionIterator;
  typedef hfst::implementations::HfstState HfstState;

  // ...ENUMS AND TYPEDEFS


  // TESTING AND OPTIMIZATION...

  enum MinimizationAlgorithm { HOPCROFT, BRZOZOWSKI };
  /* Which minimization algorithm is used. 
     In foma, Hopcroft is always used. 
     In OpenFst and SFST, the default algorithm is Brzozowski. */
  void set_minimization_algorithm(MinimizationAlgorithm);
  MinimizationAlgorithm get_minimization_algorithm(); 

  // for testing
  void set_harmonize_smaller(bool);
  bool get_harmonize_smaller(void);

  /* Whether unknown and identity symbols are used. By default, they are used.
     These symbols are always reserved for use and included in alphabets of transducers,
     but code optimization is possible if it is known that they do not appear
     in transducer transitions. */
  void set_unknown_symbols_in_use(bool);
  bool get_unknown_symbols_in_use();

  // ...TESTING AND OPTIMIZATION


  /** \brief A stream for reading binary transducers. 

      An example:
\verbatim
      try {
        HfstInputStream in("testfile");
       } catch (FileNotReadableException e) {
         printf("ERROR: File does not exist.\n");
         exit(1);
       }
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
#if HAVE_SFST
      hfst::implementations::SfstInputStream * sfst;
#endif
#if HAVE_OPENFST
      hfst::implementations::TropicalWeightInputStream * tropical_ofst;
      hfst::implementations::LogWeightInputStream * log_ofst;
#endif
#if HAVE_FOMA
      hfst::implementations::FomaInputStream * foma;
#endif
      hfst::implementations::HfstOlInputStream * hfst_ol;
    };

    StreamImplementation implementation; // the backend inplementation
    ImplementationType type;             // implementation type
    std::string name;                    // name of next transducer, given in the hfst header
    unsigned int bytes_to_skip;          // how many bytes have been already read by the function
                                         // processing the hfst header
    std::string filename;                // the name of the file, if stdin, name is ""

    /* A special case where an OpenFst transducer has no symbol tables but an
       SFST alphabet is appended at the end. Should not occur very often, but
       possible when converting old transducers into version 3.0. transducers.. */
    bool hfst_version_2_weighted_transducer;
 
    /* the stream that the reading operations use
       this stream is used when reading the first transducer, when the type of the transducer is not known
       and thus there is no backend implementation whose reading functions could be used
       if input_stream==NULL, the backend implementation is used */
    std::istream * input_stream;

    /* Basic stream operators, work on input_stream (if not NULL) or on the stream implementation. */
    char stream_get(); // extract one character from the stream
    void stream_unget(char c); // return character c to the stream
    bool stream_eof(); // whether the stream is at end
    std::string stream_getstring(); // get a string from the stream
    char stream_peek();     // return the next character in the stream without extracting it
    /* The stream implementation ignores n bytes. */
    void ignore(unsigned int n);

    /* The type of a transducer not supported directly by HFST version 3.0 
       but which can occur in conversion functions. */
    enum TransducerType { 
      HFST_VERSION_2_WEIGHTED, /* See the above variable. */
      HFST_VERSION_2_UNWEIGHTED_WITHOUT_ALPHABET, /* An SFST transducer with no alphabet,
						     not supported. */
      HFST_VERSION_2_UNWEIGHTED, /* Old header + ordinary SFST transducer. */               
      OPENFST_, /* An OpenFst transducer, can cause problems if it does not have symbol tables. */
      SFST_,  /* An SFST transducer. */
      FOMA_, /* A foma transducer. */
      ERROR_TYPE_ /* Transducer type not recognized. */
    };

    /* Read a transducer from the stream. */
    void read_transducer(HfstTransducer &t);
    /* Type of next transducer in the stream. */
    ImplementationType stream_fst_type();

    // methods used by function stream_fst_type
    TransducerType guess_fst_type(int &bytes_read);
    bool read_hfst_header(int &bytes_read);
    bool read_library_header(int &bytes_read);
    int get_header_size(int &bytes_read);                        
    StringPairVector get_header_data(int header_size);                
    void process_header_data(StringPairVector &header_data, bool warnings=false); 
    bool set_implementation_specific_header_data(StringPairVector &data, unsigned int index);
    bool read_library_header_beta(int &bytes_read);
    ImplementationType get_fst_type_beta(int &bytes_read); 

  public:

    /** \brief Create a stream to standard in for reading binary transducers. */
    HfstInputStream(void);

    /** \brief Open a stream to file \a filename for reading binary transducers. 

	@pre The file exists. Otherwise, an exception is thrown.
	@throws hfst::exceptions::FileNotReadableException */
    HfstInputStream(const char* filename);

    /** Delete the stream. */
    ~HfstInputStream(void);

    /** \brief Close the stream.

	If the stream points to standard in, nothing is done. */
    void close(void);

    /** \brief Whether the stream is at the end. */
    bool is_eof(void);
    /** \brief Whether badbit is set. */
    bool is_bad(void);
    /** \brief Whether the state of the stream is good for input operations. */
    bool is_good(void);
    /** \brief Whether the next element in the stream is an HfstTransducer. */
    bool is_fst(void);
    
    /** \brief The type of the first transducer in the stream. 

	By default, all transducers in a stream have the same type, else
	an hfst::exceptions::TransducerTypeMismatchException is thrown. */
    ImplementationType get_type(void) const;

    friend class HfstTransducer;
  };

  /** \brief A stream for writing binary transducers. 

      An example:
\verbatim
      HfstOutputStream out("testfile", FOMA_TYPE);
      out << foma_transducer1 
          << foma_transducer2 
          << foma_transducer3;
      out.close();
\endverbatim
   **/
  class HfstOutputStream
  {
  protected:
    union StreamImplementation
    {
#if HAVE_OPENFST
      hfst::implementations::LogWeightOutputStream * log_ofst;
      hfst::implementations::TropicalWeightOutputStream * tropical_ofst;
#endif
#if HAVE_SFST
      hfst::implementations::SfstOutputStream * sfst;
#endif
#if HAVE_FOMA
      hfst::implementations::FomaOutputStream * foma;
#endif
      hfst::implementations::HfstOlOutputStream * hfst_ol;
    };
    ImplementationType type; // type of the stream implementation
    bool hfst_format;  // whether an hfst header is written before every transducer
    StreamImplementation implementation; // backend implementation

    // write data to stream
    void write(const std::string &s);
    void write(const std::vector<char> &s);
    void write(const char &c);

    // append string s to vector str and a '\0'
    static void append(std::vector<char> &str, const std::string &s);

    void append_hfst_header_data(std::vector<char> &header);
    void append_implementation_specific_header_data(std::vector<char> &header, HfstTransducer &transducer);


  public:

    /** \brief Create a stream to standard out for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format. */
    HfstOutputStream(ImplementationType type, bool hfst_format=true);

    /** \brief Open a stream to file \a filename for writing binary transducers of type \a type. 
	\a hfst_format defines whether transducers are written in hfst format or as such in their backend format.

	If the file exists, it is overwritten. */
    HfstOutputStream(const std::string &filename, ImplementationType type, bool hfst_format=true);

    /** \brief Delete the stream. */
    ~HfstOutputStream(void);  

    /** \brief Write the transducer \a transducer in binary format to the stream. 

	By default, all transducers must have the same type, else an
	hfst::exceptions::TransducerHasWrongTypeException is thrown. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);

    /** \brief Close the stream. 

	If the stream points to standard out, nothing is done. */
    void close(void);
  };


  typedef std::vector<HfstTransducer> HfstTransducerVector;

#if HAVE_OPENFST
  /** \brief A set of transducers used in function compose_intersect. 

      @see HfstTransducer::compose_intersect */
  class HfstGrammar
  {
  protected:
    HfstTransducer &first_rule;
    hfst::implementations::Grammar * grammar;
    hfst::implementations::TransducerVector transducer_vector;
  public:
    /** \brief Convert \a rule_vector into an HfstGrammar. */
    HfstGrammar(HfstTransducerVector &rule_vector);
    /** \brief Convert \a rule into an HfstGrammar. */
    HfstGrammar(HfstTransducer &rule);
    /** \brief Get first rule of this grammar. */
    HfstTransducer get_first_rule(void);
    /** \brief Delete this grammar. */
    ~HfstGrammar(void);
    friend class HfstTransducer;
  };
#endif


#if HAVE_MUTABLE
  /** \brief A handle to a state in a HfstMutableTransducer. **/
  typedef unsigned int HfstState;
#endif

  /** \brief A synchronous finite-state transducer.

    \section argument_handling Argument handling

    Transducer functions modify their calling object and return a pointer to it, unless otherwise said.
\verbatim
    transducer.reverse();                                    // these lines do
    transducer = transducer.reverse();                       // the same thing

    tr1 = tr1.disjunct(tr2);                                 // tr2 is not modified, but a copy of it is disjuncted with t1

    tr.reverse().determinize().reverse().determinize();      // a chain of functions is possible
\endverbatim

    \section implementation_types Implementation types

    Currently, an HfstTransducer has four implementation types as defined by the enumeration ImplementationType.
    When an HfstTransducer is created, its type is defined with an ImplementationType argument.
    For functions that take a transducer as an argument, the type of the calling transducer
    must be the same as the type of the argument transducer:
\verbatim
    log_transducer.disjunct(sfst_transducer);                        // this will cause an error
    log_transducer.convert(SFST_TYPE).disjunct(sfst_transducer);     // this works, but weights are lost in the conversion
    log_transducer.disjunct(sfst_transducer.convert(LOG_OFST_TYPE)); // this works, information is not lost
\endverbatim

    \section creating_transducers Creating transducers

    With HfstTransducer constructors it is possible to create empty, epsilon, one-transition and single-path transducers.
    Transducers can also be created from scratch with HfstMutableTransducer and converted to an HfstTransducer.
    More complex transducers can be combined from simple ones with various functions.
    
    <a name="symbols"></a> 
    \section special_symbols Special symbols

    The HFST transducers support transitions with epsilon, unknown and identity symbols.
    The special symbols are explained in documentation of datatype #String.

An example:
\verbatim
ImplementationType type = SFST_TYPE;
HfstTransducer unk_eps("@_UNKNOWN_SYMBOL_@", "foo");
HfstTransducer id_bar("@_IDENTITY_SYMBOL_@");
HfstTransducer bar("bar");
id_bar.concatenate(bar); 

// id_bar is expanded to [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ | foo:foo ] [ bar:bar ] ]
// unk_eps is expanded to [ @_UNKNOWN_SYMBOL_@:foo | bar:foo ]
unk_eps.disjunct(id);
\endverbatim

  */
  class HfstTransducer
  {
  protected:
    union TransducerImplementation
    {
#if HAVE_SFST
      hfst::implementations::Transducer * sfst;
#endif
#if HAVE_OPENFST
      hfst::implementations::StdVectorFst * tropical_ofst;
      hfst::implementations::LogFst * log_ofst;
#endif
#if HAVE_FOMA
      fsm * foma;
#endif
      hfst_ol::Transducer * hfst_ol;
#if HAVE_OPENFST
      hfst::implementations::StdVectorFst * internal;
#endif 
    };
    
    // interfaces through which the backend implementations can be accessed
#if HAVE_SFST
    static hfst::implementations::SfstTransducer sfst_interface;
#endif
#if HAVE_OPENFST
    static hfst::implementations::TropicalWeightTransducer tropical_ofst_interface;
    static hfst::implementations::LogWeightTransducer log_ofst_interface;
#endif
#if HAVE_FOMA
    static hfst::implementations::FomaTransducer foma_interface;
#endif
    static hfst::implementations::HfstOlTransducer hfst_ol_interface;

    ImplementationType type; // the backend implementation type of the transducer

    bool anonymous; // currently not used
    bool is_trie;   // currently not used
    std::string name; // the name of the transducer

    TransducerImplementation implementation; // the backend implementation

    unsigned int number_of_states() const;

    /* Harmonize transducers this and another. In harmonization, the symbol-to-number
       correspondencies of this transducer are recoded so that they are equivalent to the ones
       used in transducer another. Then the unknown and identity symbols are expanded
       in both transducers. */
    void harmonize(HfstTransducer &another);

    // currently not implemented, TODO
    HfstTransducer &disjunct_as_tries(HfstTransducer &another,
				      ImplementationType type);  

#include "apply_schemas.h"

    // whether the conversion requested can be done without losing any information
    bool static is_safe_conversion(ImplementationType original, ImplementationType conversion);

  public:
    HfstTransducer();

    /** \brief Create an empty transducer, i.e. a transducer that does not recognize any string.
	The type of the transducer is befined by \a type.

	@note Use HfstTransducer("@_EPSILON_SYMBOL_@") to create an epsilon transducer.
     **/
    HfstTransducer(ImplementationType type);

    /** \brief Create a transducer by tokenizing the utf8 string \a utf8_string
	with tokenizer \a multichar_symbol_tokenizer.
	The type of the transducer is defined by \a type. 

	\a utf8_str is read one token at a time and for each token a new transition is created in the resulting
	transducer. The input and output symbols of that transition are the same as the token read.

	An example:
\verbatim
       std::string input = "foo";
	std::string output = "bar";
	HfstTokenizer TOK;
       HfstTransducer tr(input, output, TOK, LOG_OFST_TYPE);
	// tr now contains one path [f:b o:a o:r]
\endverbatim

	@see HfstTokenizer **/
    HfstTransducer(const std::string& utf8_str, 
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);

    /** \brief Create a transducer by tokenizing 
	the utf8 input string \a input_utf8_string
	and output string \a output_utf8_string
	with tokenizer \a multichar_symbol_tokenizer.
	The type of the transducer is defined by \a type. 

	\a input_utf8_str and \a output_utf8_str are read one token at a time and for each token a new transition 
	is created in the resulting transducer. The input and output symbols of that transition are the same as 
	the input and output tokens read. If either string contains less tokens than another, epsilons are used
	as transition symbols for the shorter string.

	An example:
\verbatim
       std::string input = "foo";
	std::string output = "barr";
	HfstTokenizer TOK;
       HfstTransducer tr(input, output, TOK, SFST_TYPE);
	// tr now contains one path [f:b o:a o:r 0:r]
\endverbatim

	@see HfstTokenizer **/
    HfstTransducer(const std::string& input_utf8_str,
    		   const std::string& output_utf8_str,
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);

    HfstTransducer(const StringPairSet & sps, ImplementationType type, bool cyclic=false);
    HfstTransducer(const StringPairVector & spv, ImplementationType type);
    HfstTransducer(const std::vector<StringPairSet> & spsv, ImplementationType type);

    /** \brief Read a binary transducer from transducer stream \a in. 

	@pre ( in.is_eof() == in.is_bad() == false && in.is_fst() ).
	Otherwise, an exception is thrown.
	@throws hfst::exceptions::NotTransducerStreamException
	@see HfstInputStream **/
    HfstTransducer(HfstInputStream &in);
    /** \brief Create a deep copy of transducer \a another. **/
    HfstTransducer(const HfstTransducer &another);

    /** \brief Create an ordinary transducer equivalent to mutable transducer \a t.
	The type of the transducer is defined by \a type.  **/
    HfstTransducer(const HfstMutableTransducer &t, ImplementationType type);

    /** \brief Delete the HfstTransducer. **/
    ~HfstTransducer(void);

    /** \brief Create a transducer that recognizes the string pair [symbol:symbol]. The type of the transducer is defined by \a type. 

	@see String **/
    HfstTransducer(const std::string &symbol, ImplementationType type);
    /** \brief Create a transducer that recognizes the string pair [isymbol:osymbol]. The type of the transducer is defined by \a type. 

	@see String **/
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type);


    /* TEST */
    //HfstTransducer(unsigned int number, ImplementationType type);
    //HfstTransducer(unsigned int inumber, unsigned int onumber, ImplementationType type);
    void print_alphabet();

    /** \brief Set the name of the transducer as \a name. */
    void set_name(std::string &name);
    /** \brief Get the name of the transducer. */
    std::string get_name();

    /** \brief Whether this transducer and \a another are equivalent.

	Two transducers are equivalent iff they accept the same input/output string pairs with the same weights
	and the same alignments. 
    */
    bool compare(const HfstTransducer &another) const;

    //static bool are_equivalent(const HfstTransducer &tr1, const HfstTransducer &tr2);


    /** \brief Write the transducer in AT&T format to FILE \a ofile. \a write_weights
	defines whether weights are written.

	If several transducers are written in the same file, they must be separated by a line
	of two consecutive hyphens "--".

An example:
\verbatim
ImplementationType type = FOMA_TYPE;
HfstTransducer foobar("foo","bar",type);
HfstTransducer epsilon("@_EPSILON_SYMBOL_@",type);
HfstTransducer empty(type);
HfstTransducer a_star("a",type);
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

	@see hfst::operator<<(std::ostream &out,HfstTransducer &t) HfstTransducer(FILE*, ImplementationType, const std::string&) */
    void write_in_att_format(FILE * ofile, bool write_weights=true) const;

    /** \brief Create a transducer of type \a type as defined in AT&T format in FILE \a ifile.
	\a epsilon_symbol defines how epsilons are represented.
	
	Lines are of the form "source_state TAB destination_state TAB input_symbol TAB output_symbol (TAB weight)"
	or "final_state (TAB weight)". If several transducers are listed in the same file, they are separated
	by lines of two consecutive hyphens "--".

An example:
\verbatim
0      1      foo      bar      0.3
1      0.5
--
0      0.0
--
--
0      0.0
0      0      a        <eps>    0.2
\endverbatim

        The example lists four transducers in AT&T format: one transducer accepting the string pair "foo:bar", one
	epsilon transducer, one empty transducer and one transducer that accepts any number of 'a's and produces an empty string
	in all cases. The transducers
	can be read with the following commands (from a file named "testfile.att"):
\verbatim
std::vector<HfstTransducer> transducers;
FILE * ifile = fopen("testfile.att", "rb");
try {
  while (not eof(ifile))
    {
    HfstTransducer t(ifile, TROPICAL_OFST_TYPE, "<eps>");
    transducers.push_back(t);
    printf("read one transducer\n");
    }
} catch (NotValidAttFormatException e) {
    printf("Error reading transducer: not valid AT&T format.\n"); }
fclose(ifile);
fprintf(stderr, "Read %i transducers in total.\n", (int)transducers.size());
\endverbatim

Epsilon will be represented as "@_EPSILON_SYMBOL_@" in the resulting transducer.
The argument \a epsilon_symbol only denotes how epsilons are represented in \a ifile.

@throws hfst::exceptions::NotValidAttFormatException
@see write_in_att_format(FILE*,bool) String
**/
    HfstTransducer(FILE * ifile, ImplementationType type, const std::string &epsilon_symbol);

    static HfstTransducer &read_in_att_format(FILE * ifile, ImplementationType type, const std::string &epsilon_symbol);

    /** \brief \brief Write the transducer in AT&T format to FILE named \a filename. \a write_weights
	defines whether weights are written.

	If the file exists, it is overwritten. If the file does not exist, it is created. 

	@see #write_in_att_format */
    void write_in_att_format(const char * filename, bool write_weights=true) const;

    /* \brief Create a transducer of type \a type as defined in AT & T format in file named \a filename.
	\a epsilon_symbol defines how epsilons are represented.

	@pre The file exists, otherwise an exception is thrown.
	@see HfstTransducer(FILE, ImplementationType, const std::string&)
	@throws hfst::exceptions::FileNotReadableException hfst::exceptions::NotValidAttFormatException */
    static HfstTransducer &read_in_att_format(const char * filename, ImplementationType type, const std::string &epsilon_symbol);

    /** \brief Remove all epsilon:epsilon transitions from this transducer. */
    HfstTransducer &remove_epsilons();

    /** \brief Determinize this transducer.

	Determinizing a transducer yields an equivalent transducer that has
	no state with two or more transitions whose input:output symbol pairs are the same. */
    HfstTransducer &determinize();

    /** \brief Minimize this transducer.

	Minimizing a transducer yields an equivalent transducer with the smallest number of states. 
     
	@bug OpenFst's minimization algorithm seems to add epsilon transitions to weighted transducers? */
    HfstTransducer &minimize();

    /** \brief Extract \a n best paths of this transducer. 

	In the case of a weighted transducer (#TROPICAL_OFST_TYPE or #LOG_OFST_TYPE), best paths are defined as paths with the lowest weight.
	In the case of an unweighted transducer (#SFST_TYPE or #FOMA_TYPE), best paths are defined as paths with the smallest number of transitions, i.e. the shortest paths. 

        This function is not implemented for #FOMA_TYPE or #SFST_TYPE.
	If this function is called by an HfstTransducer of type #FOMA_TYPE or #SFST_TYPE, it is converted to #TROPICAL_OFST_TYPE,
	strings are extracted and it is converted back to #FOMA_TYPE or #SFST_TYPE.
    */
    HfstTransducer &n_best(unsigned int n);

    /** \brief A concatenation of n transducers where n is any number from zero to infinity. */
    HfstTransducer &repeat_star();

    /** \brief A concatenation of n transducers where n is any number from one to infinity. */
    HfstTransducer &repeat_plus();

    /** \brief A concatenation of \a n transducers. */
    HfstTransducer &repeat_n(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from zero to \a n, inclusive.*/
    HfstTransducer &repeat_n_minus(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from \a n to infinity, inclusive.*/
    HfstTransducer &repeat_n_plus(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from \a n to \a k, inclusive.*/
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k);

    /** \brief Disjunct this transducer with an epsilon transducer. */
    HfstTransducer &optionalize();

    /** \brief Swap the input and output labels of each transition in this transducer. */
    HfstTransducer &invert();

    /** \brief Reverse this transducer. 

	A reverted transducer accepts the string "n(0) n(1) ... n(N)" iff the original
	transducer accepts the string "n(N) n(N-1) ... n(0)" */
    HfstTransducer &reverse();

    /** \brief Extract the input language of this transducer. 

	All transition symbol pairs "isymbol:osymbol" are changed to "isymbol:isymbol". */
    HfstTransducer &input_project();

    /** \brief Extract the output language of this transducer.

	All transition symbol pairs "isymbol:osymbol" are changed to "osymbol:osymbol". */
    HfstTransducer &output_project();

    /* \brief Call \a callback with some or all string pairs recognized by the transducer?

	If the callback returns false the search will be terminated. The \a cycles parameter
	indicates how many times a cycle will be followed, with negative numbers
	indicating unlimited. Note that if the transducer is cyclic and cycles aren't capped,
	the search will not end until the callback returns false. */
    void extract_strings(ExtractStringsCb& callback, int cycles=-1);
	  
    std::vector<HfstTransducer*> extract_paths();

	  /** \brief Extract a maximum of \a max_num string pairs that are recognized by the transducer
	      following a maximum of \a cycles cycles and store them into \a results.

	The total number of resulting strings is capped at \a max_num, with 0 or negative
	indicating unlimited. The \a cycles parameter indicates how many times a cycle
	will be followed, with negative numbers indicating unlimited. If this function
	is called on a cyclic transducer with unlimited values for both max_num and
	cycles, an exception will be thrown.
	@throws hfst::exceptions::TransducerIsCyclicException
	@see #n_best */
    void extract_strings(WeightedPaths<float>::Set &results, int max_num=-1, int cycles=-1);
    
    /* \brief Call \a callback with extracted strings that are not invalidated by
               flag diacritic rules.
	@see #extract_strings(WeightedPaths<float>::Set&, int, int) */             
    void extract_strings_fd(ExtractStringsCb& callback, int cycles=-1, bool filter_fd=true);
    
    /** \brief Store to \a results string pairs that are recognized by the transducer
               and are not invalidated by flag diacritic rules, optionally filtering
               the flag diacritics themselves out of the result strings.
  The same conditions that apply for the extract_strings function apply also for this one.
  @throws hfst::exceptions::TransducerIsCyclicException
  @see #extract_strings(WeightedPaths<float>::Set&, int, int) */
    void extract_strings_fd(WeightedPaths<float>::Set &results, int max_num=-1, int cycles=-1, bool filter_fd=true);

    /** \brief Freely insert symbol pair \a symbol_pair into the transducer. */
    HfstTransducer &insert_freely(const StringPair &symbol_pair);

    /** \brief Substitute all transition pairs <i>isymbol</i>:<i>osymbol</i> according to function \a func. 

	An example:
\verbatim
void function(std::string &isymbol, std::string &osymbol) {
  if (osymbol.compare(isymbol) != 0)
    return;
  if (osymbol.compare("a") == 0 ||
      osymbol.compare("o") == 0 ||
      osymbol.compare("u") == 0)
    osymbol = std::string("<back_wovel>");
  if (osymbol.compare("e") == 0 ||
      osymbol.compare("i") == 0)
    osymbol = std::string("<front_wovel>");
}

...

// For all transitions in transducer t whose input and output wovels are equivalent, substitute the output wovel
// with a symbol that defines whether the wovel in question is a front or back wovel.
t.substitute(&func);
\endverbatim       	

@see String
*/
    HfstTransducer &substitute(void (*func)(std::string &isymbol, std::string &osymbol));

    /** \brief Substitute all transition symbols equal to \a old_symbol with symbol \a new_symbol.
	\a input_side and \a output_side define whether the substitution is made on input and output sides.

	The transition weights remain the same. 

	@see String */
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol,
			       bool input_side=true,
			       bool output_side=true);
						      
    /** \brief Substitute all transition symbol pairs equal to \a old_symbol_pair with \a new_symbol_pair. 

	The transition weights remain the same.

	Implemented only for #TROPICAL_OFST_TYPE and #LOG_OFST_TYPE.
	If this function is called by an unweighted HfstTransducer, it is converted to weighted one,
	substitution is done and it is converted back to the original format.

	@see String
     */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPair &new_symbol_pair);

    /** \brief Substitute all transitions whose symbol pairs are equal to \a old_symbol_pair 
	with a set of transitions whose symbol pairs are equal to \a new_symbol_pair_set. 

	The weight of the original transition is copied to all new transitions.

	Implemented only for #TROPICAL_OFST_TYPE and #LOG_OFST_TYPE.
	If this function is called by an unweighted HfstTransducer (#SFST_TYPE or #FOMA_TYPE), it is converted to #TROPICAL_OFST_TYPE,
	substitution is done and it is converted back to the original format.

	@see String
     */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPairSet &new_symbol_pair_set);

    /** \brief Substitute all transitions equal to \a symbol_pair with a copy of transducer \a transducer. 

	A copy of \a transducer is attached between the source and target states of the original transition with epsilon transitions.
	The weight of the original transition is copied to the epsilon transition leaving from the source state.

	Implemented only for #TROPICAL_OFST_TYPE and #LOG_OFST_TYPE.
	If this function is called by an unweighted HfstTransducer (#SFST_TYPE or #FOMA_TYPE), it is converted to #TROPICAL_OFST_TYPE,
	substitution is done and it is converted back to the original format.

	@see String
     */
    HfstTransducer &substitute(const StringPair &symbol_pair,
			       HfstTransducer &transducer);

    /** \brief Set the weights of all final states to \a weight. 

	If the HfstTransducer is of unweighted type (#SFST_TYPE or #FOMA_TYPE), nothing is done.
    */
    HfstTransducer &set_final_weights(float weight);

    /** \brief Transform all transition and state weights according to the function pointer \a func. 

     An example:
\verbatim
    float func(float f) { 
      return 2*f + 0.5; 
    }

    ...

    // All transition and final weights are multiplied by two and summed with 0.5.
    HfstTransducer t_transformed =
      t.transform_weights(&func);
\endverbatim 

    If the HfstTransducer is of unweighted type (#SFST_TYPE or #FOMA_TYPE), nothing is done.
    */
    HfstTransducer &transform_weights(float (*func)(float));

    /** \brief Push weights towards initial or final state(s) as defined by \a type.

	If the HfstTransducer is of unweighted type (#SFST_TYPE or #FOMA_TYPE), nothing is done.
	@see PushType
    */
    HfstTransducer &push_weights(PushType type);

    /** \brief Compose this transducer with \a another. */
    HfstTransducer &compose(const HfstTransducer &another);

#if HAVE_OPENFST
    /** \brief Compose this transducer with the intersection of rule transducers in \a grammar. 

	The algorithm used by this function is faster than intersecting 
	all transducers one by one and then composing this transducer with the intersection. */
    HfstTransducer &compose_intersect(HfstGrammar &grammar);
#endif

    /** \brief Concatenate this transducer with \a another. */
    HfstTransducer &concatenate(const HfstTransducer &another);

    /** \brief Disjunct this transducer with \a another. */
    HfstTransducer &disjunct(const HfstTransducer &another);

    HfstTransducer &disjunct(const StringPairVector &spv);

    /** \brief Intersect this transducer with \a another. */
    HfstTransducer &intersect(const HfstTransducer &another);

    /** \brief Subtract transducer \a another from this transducer. */
    HfstTransducer &subtract(const HfstTransducer &another);

    
    /** \brief Whether \a t is cyclic. */
    bool is_cyclic(void) const;

    /** \brief The implementation type of the transducer. */
    ImplementationType get_type(void) const;


    /* TEST */
    static float get_profile_seconds(ImplementationType type);
    /* Get all symbol pairs that occur in the transitions of this transducer. */
    StringPairSet get_symbol_pairs();
    /* Whether HFST is linked to the transducer library needed by implementation type \a type. */
    static bool is_implementation_type_available(ImplementationType type);
    /* Create a tokenizer that recognizes all symbols that occur in this transducer. */
    HfstTokenizer create_tokenizer();

    /** \brief Convert this transducer into an equivalent transducer in format \a type. 

	If a weighted transducer is converted into an unweighted one, all weights are lost. 
	In the reverse case, all weights are initialized to the semiring's one. */
    HfstTransducer &convert(ImplementationType type);

    /* \brief Create a new transducer equivalent to \a t in format \a type. 

	@see convert(ImplementationType type) */
    static HfstTransducer &convert(const HfstTransducer &t, ImplementationType type);


    //! @brief Lookup or apply a single string.
    //!
    //! Traverse all paths on logical first level of the transducer to produce
    //! all possible outputs on the second.
    //! This is in effect a fast composition of single path from left
    //! hand side.
    //!
    //! @arg results  output parameter to store unique results
    //! @arg s  string to look up
    //! @arg limit  number of strings to extract. -1 tries to extract all and
    //!             may get stuck if infinitely ambiguous
    void lookup(HfstLookupPaths& results, const HfstLookupPath& s,
                ssize_t limit = -1);

    //! @brief Lookup or apply a single string minding flag diacritics properly.
    //! 
    //! This is a version of lookup that handles flag diacritics as epsilons
    //! and validates the sequences prior to outputting.
    //!
    //! @sa lookup
    void lookup_fd(HfstLookupPaths& results, const HfstLookupPath& s,
                   ssize_t limit = -1);

    //! @brief Lookdown a single string.
    //!
    //! Traverse all paths on logical second level of the transducer to produce
    //! all possible inputs on the first.
    //! This is in effect a fast composition of single
    //! path from left hand side.
    //!
    //! @arg results  output parameter to store unique results
    //! @arg s  string to look down
    //! @arg tok  tokenizer to split string in arcs
    //! @arg limit  number of strings to extract. -1 tries to extract all and
    //!             may get stuck if infinitely ambiguous
    void lookdown(HfstLookupPaths& results, const HfstLookupPath& s,
                  ssize_t limit = -1);

    //! @brief Lookdown a single string minding flag diacritics properly.
    //! 
    //! This is a version of lookdown that handles flag diacritics as epsilons
    //! and validates the sequences prior to outputting.
    //!
    //! @sa lookdown
    void lookdown_fd(HfstLookupPaths& results, HfstLookupPath& s,
                     ssize_t limit = -1);

    //! @brief Whether lookup will have infinite results.
    bool is_lookup_infinitely_ambiguous(const HfstLookupPath& s);

    //! @brief Whether lookdown will have infinite results.
    bool is_lookdown_infinitely_ambiguous(const HfstLookupPath& s);

    HfstTransducer &operator=(const HfstTransducer &another);
    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
    friend class HfstOutputStream;
    friend class hfst::implementations::HfstInternalTransducer;

#if HAVE_OPENFST
    friend class HfstGrammar;
#endif
    friend class HfstCompiler;
  };

  /** \brief A finite-state synchronous transducer that supports iterating through and adding states and transitions.      


     An example of constructing a transducer from scratch:
\verbatim
     HfstMutableTransducer t;       // Create an empty transducer with one start state that is not final.
     HfstState initial = 
        t.get_initial_state();      // Get a handle for the start state.
     HfstState s = t.add_state();   // Add a second state that is initially not final.
     t.set_final_weight(s, 0.5);    // Set the second state final with weight 0.5.
     t.add_transition(initial,      // Add a transition leading from the first state
                     "foo",         // with input symbol "foo",
		      "bar",         // output symbol "bar"
		      0.3,           // and weight 0.3
		      s);            // to the second state.

     // Now t accepts the string pair "foo":"bar" with weight 0.8.
\endverbatim

     \anchor iterator_example An example of iterating through states and transitions:
\verbatim
// Print a transducer in AT&T format to standard out.
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
          cout << s << "\t" << tr.target_state << "\t"
	        << tr.isymbol << "\t" 
               << tr.osymbol
	        << "\t" << tr.weight << "\n";
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
#if HAVE_MUTABLE
  class HfstMutableTransducer
  {
  protected:
    hfst::implementations::HfstInternalTransducer transducer;
  public:
    /** \brief Create an empty mutable transducer, 
	i.e. a transducer with one non-final initial state. */
    HfstMutableTransducer(void);
    /** \brief Create a mutable transducer equivalent to \a t. */
    HfstMutableTransducer(const HfstTransducer &t);
    /** \brief Create a deep copy of mutable transducer \a t. */
    HfstMutableTransducer(const HfstMutableTransducer &t);
    /** \brief Delete mutable transducer. */
    ~HfstMutableTransducer(void);
    /** \brief Add a state to this mutable transducer and return a handle to that state. */
    HfstState add_state();
    /** \brief Set the value of the final weight of state \a s in this mutable transducer to \a w. */
    void set_final_weight(HfstState s, float w);
    /** \brief Whether state \a s in this mutable transducer is final. */
    bool is_final(HfstState s);
    /** \brief Return a handle to the initial state in this mutable transducer. */
    HfstState get_initial_state();
    /** \brief The final weight of state \a s in this mutable transducer. 
	@pre State \a s must be final. */
    float get_final_weight(HfstState s);
    /** \brief Add transition with input and output symbols \a isymbol and \a osymbol and weight \a w
	between states \a source and \a target in this mutable transducer. 

	@see String */
    void add_transition(HfstState source, std::string isymbol, std::string osymbol, float w, HfstState target);
    /* friend classes */
    friend class HfstTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };
#endif

  /** \brief A state iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstStateIterator
  {
  protected:
    std::set<unsigned int> state_set;
  public:
    /** \brief Create a state iterator for mutable transducer \a t. */
    HfstStateIterator(const HfstMutableTransducer &t);
    ~HfstStateIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current state pointed by the iterator. */
    HfstState value();
    /** \brief Advance the iterator to the next state. */
    void next();
  };
#endif

  /** \brief A transition in an HfstMutableTransducer. 

      Transitions are only returned by an \ref HfstTransitionIterator. 

      For an example of using transitions, see \ref iterator_example "this". 

      @see HfstTransitionIterator HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstTransition
  {
  public:
    /** \brief The input symbol of the transition. */
    std::string isymbol;
    /** \brief The output symbol of the transition. */
    std::string osymbol;
    /** \brief The weight of the transition. */
    float weight;
    /** \brief The target state of the transition. */
    HfstState target_state;
  protected:
    HfstTransition(std::string isymbol, std::string osymbol, float weight, HfstState target_state);
  public:
    ~HfstTransition(void);
    /* \brief The input symbol of the transition. */
    std::string get_input_symbol(void);
    /* \brief The output symbol of the transition. */
    std::string get_output_symbol(void);
    /* \brief The weight of the transition. */
    float get_weight(void);
    /* \brief The target state of the transition. */
    HfstState get_target_state(void);
    friend class HfstTransitionIterator;
  };
#endif

  /** \brief A transition iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstTransitionIterator
  {
  protected:
    std::set<hfst::implementations::InternalTransducerLine> transition_set;
  public:
    /** \brief Create a transition iterator for state \a s in the mutable transducer \a t. */
    HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s);
    ~HfstTransitionIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current transition pointed by the iterator. */
    HfstTransition value();
    /** \brief Advance the iterator to the next transition. */
    void next();    
  };
#endif

  /** \brief The same as HfstTransducer::write_in_att_format 

      @see HfstTransducer::write_in_att_format */
  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);

  /** \brief A namespace for functions that create two-level, replace, restriction and coercion rule transducers. */
  namespace rules
  {
    enum ReplaceType {REPL_UP, REPL_DOWN, REPL_RIGHT, REPL_LEFT};
    enum TwolType {twol_right, twol_left, twol_both};

    /* helping methods */
    HfstTransducer universal_fst(const StringPairSet &alphabet, ImplementationType type);
    HfstTransducer negation_fst(const HfstTransducer &t, const StringPairSet &alphabet);

    HfstTransducer replace(HfstTransducer &t, ReplaceType repl_type, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_transducer(HfstTransducer &t, std::string lm, std::string rm, ReplaceType repl_type, StringPairSet &alphabet);
    HfstTransducer replace_context(HfstTransducer &t, std::string m1, std::string m2, StringPairSet &alphabet);
    HfstTransducer replace_in_context(HfstTransducerPair &context, ReplaceType repl_type, HfstTransducer &t, bool optional, StringPairSet &alphabet);


    // maybe enum TWO_LEVEL_RULE { IF_RULE, ONLY_IF_RULE, IF_AND_ONLY_IF_RULE }
    /** \brief ... */
    HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);
    HfstTransducer two_level_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);
    HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);

    // maybe enum REPLACE_RULE { REPLACE_UP, REPLACE_DOWN, REPLACE_RIGHT, REPLACE_LEFT }
    HfstTransducer replace_up(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_down(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_right(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_left(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    HfstTransducer replace_up(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_down(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /* Used by hfst-calculate. */
    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet,
			       TwolType twol_type, int direction ); 

    // maybe enum ?
    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
  }

}



#endif
