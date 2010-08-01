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
#include "implementations/SymbolDefs.h"
#include "implementations/SfstTransducer.h"
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/LogWeightTransducer.h"
#include "implementations/FomaTransducer.h"
#include "implementations/HfstOlTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include "implementations/HfstExceptions.h"
//#include "implementations/SFST/src/interface.h"
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <set>

/** \mainpage 

HFST - The Helsinki Finite-State Transducer technology is intended for creating and manipulating weighted or unweighted synchronic transducers implementing regular relations.

Currently HFST has been implemented using the SFST, OpenFst and foma software libraries.
Other versions may be added in some future release. 
The SFST and foma implementations are unweighted and the OpenFst implementations weighted.
More info on SFST tools is in http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html,
OpenFst in http://www.openfst.org and foma in http://www.aclweb.org/anthology/E/E09/E09-2008.pdf.

The examples use Xerox transducer notations ( http://www.xrce.xerox.com/Research-Development/Publications/1997-005/(language) ).

*/


/** \brief A namespace for the HFST functions and datatypes. */
namespace hfst
{
  
  using hfst::implementations::SfstTransducer;
  using hfst::implementations::TropicalWeightTransducer;
  using hfst::implementations::TropicalWeightState;
  using hfst::implementations::TropicalWeightStateIterator;
  using hfst::implementations::LogWeightTransducer;
  using hfst::implementations::HfstOlTransducer;
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
    HFST_OL_TYPE, /**< An HFST optimized lookup transducer, unweighted */
    HFST_OLW_TYPE, /**< An HFST optimized lookup transducer with weights */
    UNSPECIFIED_TYPE,
    ERROR_TYPE /**< Type not recognised. 
		  This type might be returned by a function if an error occurs. */
  };

  class HfstCompiler;

  class HfstTransducer;

  typedef std::pair <HfstTransducer,HfstTransducer> HfstTransducerPair;
  typedef std::set <HfstTransducerPair> HfstTransducerPairSet;

  class HfstMutableTransducer;

  /** \brief A stream for reading binary transducers. 

      An example:
\verbatim
      try {
        HfstInputStream in("testfile");
       } catch (FileNotReadableException e) {
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
      hfst::implementations::HfstOlInputStream * hfst_ol;
    };

    ImplementationType type;
    StreamImplementation implementation;
    bool stream_has_headers;
    bool header_eaten;
    void read_transducer(HfstTransducer &t);
    ImplementationType stream_fst_type(const char *filename);
    int read_library_header(std::istream &in);
    ImplementationType read_version_3_0_fst_type(std::istream &in);
    ImplementationType guess_fst_type(std::istream &in);
  public:

    /** \brief Create a stream to standard in for reading binary transducers. */
    HfstInputStream(void);
    /** \brief Create a stream to file \a filename in for reading binary transducers. 

	@pre The file exists. Otherwise, an exception is thrown.
	@throws hfst::exceptions::FileNotReadableException */
    HfstInputStream(const char* filename);
    ~HfstInputStream(void);
    /** \brief Open the stream. 

	If the stream points to standard in, nothing is done. */
    void open(void);
    /** \brief Close the stream.

	If the stream points to standard in, nothing is done. */
    void close(void);
    /** \brief Whether the stream is open. */
    bool is_open(void);
    /** \brief Whether the stream is at the end. */
    bool is_eof(void);
    /** \brief Whether badbit is set. */
    bool is_bad(void);
    /** \brief Whether the state of the stream is good for input operations. */
    bool is_good(void);
    /** \brief Whether the next element in the stream is an HfstTransducer. */
    bool is_fst(void);
    
    /** \brief The type of the first transducer in the stream. */
    ImplementationType get_type(void) const;
    friend class HfstTransducer;
  };

  /** \brief A stream for writing binary transducers. 

      An example:
\verbatim
      HfstOutputStream out("testfile", FOMA_TYPE);
      out.open();
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
      hfst::implementations::LogWeightOutputStream * log_ofst;
      hfst::implementations::TropicalWeightOutputStream * tropical_ofst;
      hfst::implementations::SfstOutputStream * sfst;
      hfst::implementations::FomaOutputStream * foma;
      hfst::implementations::HfstOlOutputStream * hfst_ol;
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
    /** \brief Write the transducer \a transducer in binary format to the stream. 

	@pre The stream has been opened, unless it points to standard out. */
    HfstOutputStream &operator<< (HfstTransducer &transducer);
    /** \brief Open the stream. 

	If the stream points to standard out, nothing is done. */
    void open(void);
    /** \brief Close the stream. 

	If the stream points to standard out, nothing is done. */
    void close(void);
  };


  /** \brief A handle to a state in a HfstMutableTransducer. **/
  typedef hfst::implementations::StateId HfstState;

  /** \brief A synchronous finite-state transducer.

    \section argument_handling Argument handling

    Transducer functions modify their calling object and return a pointer to it, unless otherwise said.
\verbatim
    transducer.reverse();                                    // these lines do
    transducer = transducer.reverse();                       // the same thing

    tr1 = tr1.disjunct(tr2);                                 // tr2 is not modified, but a copy of it is disjuncted with t1

    tr.reverse().determinize().reverse().determinize();   // a chain of functions is possible
\endverbatim

    \section implementation_types Implementation types

    Currently, an HfstTransducer has four implementation types as defined by the enumeration hfst::ImplementationType.
    When an HfstTransducer is created, its type is defined with an ImplementationType argument.
    For functions that take a transducer as an argument, the type of the calling transducer
    must be the same as the type of the argument transducer:
\verbatim
    log_transducer.disjunct(sfst_transducer);                         // this will cause an error
    log_transducer.convert(SFST_TYPE).disjunct(sfst_transducer);     // this works, but weights are lost in the conversion
    log_transducer.disjunct(sfst_transducer.convert(LOG_OFST_TYPE)); // this works, information is not lost
\endverbatim

    \section creating_transducers Creating transducers

    With HfstTransducer constructors it is possible to create empty, epsilon, one-transition and single-path transducers.
    Transducers can also be created from scratch with hfst::HfstMutableTransducer and converted to an HfstTransducer.
    More complex transducers can be combined from simple ones with various functions.

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
      hfst_ol::Transducer * hfst_ol;
      hfst::implementations::StdVectorFst * internal; 
    };
    
    static hfst::implementations::SfstTransducer sfst_interface;
    static hfst::implementations::TropicalWeightTransducer tropical_ofst_interface;
    static hfst::implementations::LogWeightTransducer log_ofst_interface;
    static hfst::implementations::FomaTransducer foma_interface;
    static hfst::implementations::HfstOlTransducer hfst_ol_interface;

    ImplementationType type;

    bool anonymous;
    bool is_trie;

    TransducerImplementation implementation; 

    void harmonize(HfstTransducer &another);
    HfstTransducer &disjunct_as_tries(HfstTransducer &another,
				      ImplementationType type);  

#include "apply_schemas.h"

    bool static is_safe_conversion(ImplementationType original, ImplementationType conversion);

  public:
    HfstTransducer();

    /** \brief Create an empty transducer, i.e. a transducer that does not recognize any string. 

	@note Use HfstTransducer("") to create an epsilon transducer.
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


    HfstTransducer(const StringPairSet & sps, ImplementationType type);

    /** \brief Read a binary transducer from transducer stream \a in. 

	@pre ( in.is_eof() == in.is_bad() == false && in.is_fst() ).
	Otherwise, an exception is thrown.
	@throws hfst::exceptions::NotTransducerStreamException
	@see HfstInputStream **/
    HfstTransducer(HfstInputStream &in);
    /** \brief Create a deep copy of transducer \a another. **/
    HfstTransducer(const HfstTransducer &another);
    /** \brief Create an ordinary transducer equivalent to mutable transducer \a t. **/
    HfstTransducer(const HfstMutableTransducer &t);
    /** \brief Delete operator for HfstTransducer. **/
    ~HfstTransducer(void);

    /** \brief Create a transducer that recognizes the string pair [symbol:symbol]. The type of the transducer is defined by \a type. **/
    HfstTransducer(const std::string &symbol, ImplementationType type);
    /** \brief Create a transducer that recognizes the string pair [isymbol:osymbol]. The type of the transducer is defined by \a type. **/
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type);


    /* TEST */
    HfstTransducer(unsigned int number, ImplementationType type);
    HfstTransducer(unsigned int inumber, unsigned int onumber, ImplementationType type);


    /** \brief Whether transducers \a tr1 and \a tr2 are equivalent.

	Two transducers are equivalent iff they accept the same input/output string pairs with the same weights. **/
    static bool are_equivalent(const HfstTransducer &tr1, const HfstTransducer &tr2);

    /** \brief Write the transducer in AT & T format to FILE \a ofile. 

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

    /** \brief Create a transducer of type \a type as defined in AT & T format in FILE \a ifile.
	
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

        The example lists four transducers in AT&T format: one transducer accepting the string pair "foo:bar", one
	epsilon transducer, one empty transducer and one transducer accepting any number of 'a's. The transducers
	can be read with the following commands (from a file named "testfile.att"):
\verbatim
FILE * ifile = fopen("testfile.att", "rb");
try {
  while (not eof(ifile))
    {
    HfstTransducer t = HfstTransducer::read_in_att_format(ifile);
    printf("read one transducer\n");
    }
} catch (NotValidAttFormatException e) {
    printf("Error reading transducer: not valid AT&T format.\n"); }
fclose(ifile);
\endverbatim

@throws hfst::exceptions::NotValidAttFormatException
@see write_in_att_format
**/
    static HfstTransducer &read_in_att_format(FILE * ifile, ImplementationType type);

    /** \brief \brief Write the transducer in AT & T format to FILE named \a filename. 

	If the file exists, it is overwritten. If the file does not exist, it is created. 

	@see write_in_att_format(FILE*) */
    void write_in_att_format(const char * filename);

    /** \brief Create a transducer of type \a type as defined in AT & T format in file named \a filename.

	@pre The file exists, otherwise an exception is thrown.
	@see read_in_att_format(FILE*,ImplementationType)
	@throws hfst::exceptions::FileNotReadableException hfst::exceptions::NotValidAttFormatException */
    static HfstTransducer &read_in_att_format(const char * filename, ImplementationType type);

    /** \brief Remove all epsilon:epsilon transitions from this transducer. */
    HfstTransducer &remove_epsilons();
    /** \brief Determinize this transducer.

	Determinizing a transducer yields an equivalent transducer that has
	no state with two or more transitions whose input:output symbol pairs are the same. */
    HfstTransducer &determinize();
    /** \brief Minimize this transducer.

	Minimizing a transducer yields an equivalent transducer with the smallest number of states. 
     
	@bug OpenFst's minimization algorithm seems to add epsilon transitions to weighted transducers. */
    HfstTransducer &minimize();
    /** \brief Extract \a n best paths of this transducer. 

	In the case of a weighted transducer (TROPICAL_OFST_TYPE or LOG_OFST_TYPE), best paths are defined as paths with the lowest weight.
	In the case of an unweighted transducer (SFST_TYPE or FOMA_TYPE), best paths are defined as paths with the smallest number of transitions, i.e. the shortest paths. 

        This function is not implemented for FOMA_TYPE or SFST_TYPE.
	If this function is called by an HfstTransducer of type FOMA_TYPE or SFST_TYPE, it is converted to TROPICAL_OFST,
	strings are extracted and it is converted back to FOMA_TYPE or SFST_TYPE.
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

    /** \brief Store to \a results all string pairs that are recognised by the transducer. 

	An example:
\verbatim
	WeightedPaths<float>::Set results;
	transducer.extract_strings(results);
	for (WeightedPaths<float>::Set::iterator it = results.begin();
	  it != results.end(); it++)
	  {
	    WeightedPath<float> wp = *it;
	    // print each string pair and its weight
	    printf("%s:%s\t%f\n", wp.istring, wp.ostring, wp.weight);
	  }
\endverbatim

        This function is not implemented for FOMA_TYPE.
	If this function is called by an HfstTransducer of type FOMA_TYPE, it is converted to TROPICAL_OFST,
	strings are extracted and it is converted back to FOMA_TYPE.

	@pre The transducer is acyclic. 
	@note If the transducer is cyclic, no guarantees are given how the function will
	behave. It might get stuck in an infinite loop or return any number of string pairs. 
	In the case of a cyclic transducer, use #n_best instead. 
	@see #n_best */
    void extract_strings(WeightedPaths<float>::Set &results);
    
    /** \brief Store to \a results string pairs that are recognized by the transducer
               and are not invalidated by flag diacritic rules, optionally filtering
               the flag diacritics themselves out of the result strings
  The same conditions that apply for the extract_strings function apply also for this one.
  @see extract_strings */
    void extract_strings_fd(WeightedPaths<float>::Set &results, bool filter_fd=true);

    /** \brief Freely insert symbol pair \a symbol_pair into the transducer. */

    HfstTransducer &insert_freely(const StringPair &symbol_pair);

    /** \brief Substitute all transition symbols equal to \a old_symbol with symbol \a new_symbol. 

	The transition weights remain the same. */
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol);
			      
    /** \brief Substitute all transition symbol pairs equal to \a old_symbol_pair with \a new_symbol_pair. 

	Implemented only for TROPICAL_OFST_TYPE and LOG_OFST_TYPE.
	The transition weights remain the same.

	If this function is called by an unweighted HfstTransducer, it is converted to weighted one,
	substitution is done and it is converted back to the original format.

     */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPair &new_symbol_pair);

    /** \brief Substitute all transitions whose symbol pairs are equal to \a old_symbol_pair 
	with a set of transitions whose symbol pairs are equal to \a new_symbol_pair_set. 

	Implemented only for TROPICAL_OFST_TYPE and LOG_OFST_TYPE.
	The weight of the original transition is copied to all new transitions.

	If this function is called by an unweighted HfstTransducer (SFST_TYPE or FOMA_TYPE), it is converted to TROPICAL_OFST,
	substitution is done and it is converted back to the original format.

     */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPairSet &new_symbol_pair_set);

    /** \brief Substitute all transitions equal to \a symbol_pair with a copy of transducer \a transducer. 

	Implemented only for TROPICAL_OFST_TYPE and LOG_OFST_TYPE.
	A copy of \a transducer is attached between the source and target states of the original transition with epsilon transitions.
	The weight of the original transition is copied to the epsilon transition leaving from the source state.

	If this function is called by an unweighted HfstTransducer (SFST_TYPE or FOMA_TYPE), it is converted to TROPICAL_OFST,
	substitution is done and it is converted back to the original format.

     */
    HfstTransducer &substitute(const StringPair &symbol_pair,
			       HfstTransducer &transducer);

    /** \brief Set the weights of all final states to \a weight. 

	If the HfstTransducer is of unweighted type (SFST_TYPE or FOMA_TYPE), nothing is done.
    */
    HfstTransducer &set_final_weights(float weight);

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

    If the HfstTransducer is of unweighted type (SFST_TYPE or FOMA_TYPE), nothing is done.
    */
    HfstTransducer &transform_weights(float (*func)(float));

    /** \brief Compose this transducer with \a another. */
    HfstTransducer &compose(const HfstTransducer &another);

    /** \brief Concatenate this transducer with \a another. */
    HfstTransducer &concatenate(const HfstTransducer &another);

    /** \brief Disjunct this transducer with \a another. */
    HfstTransducer &disjunct(const HfstTransducer &another);

    /** \brief Intersect this transducer with \a another. */
    HfstTransducer &intersect(const HfstTransducer &another);

    /** \brief Subtract transducer \a another from this transducer. */
    HfstTransducer &subtract(const HfstTransducer &another);


    /* \brief Compose this transducer with another. */
    static HfstTransducer &compose(HfstTransducer &t1, HfstTransducer &t2,
				   ImplementationType type=UNSPECIFIED_TYPE);
    /* \brief Concatenate this transducer and another. */
    static HfstTransducer &concatenate(HfstTransducer &t1, HfstTransducer &t2, 
				       ImplementationType type=UNSPECIFIED_TYPE);
    /* \brief Disjunct this transducer and another. */
    static HfstTransducer &disjunct(HfstTransducer &t1, HfstTransducer &t2,
				    ImplementationType type=UNSPECIFIED_TYPE);
    /* \brief Intersect this transducer and another. */
    static HfstTransducer &intersect(HfstTransducer &t1, HfstTransducer &t2,
				     ImplementationType type=UNSPECIFIED_TYPE);
    /* \brief Subtract another transducer from this transducer. */
    static HfstTransducer &subtract(HfstTransducer &t1, HfstTransducer &t2,
				    ImplementationType type=UNSPECIFIED_TYPE);


    // test
    void test_minimize(void);
    
    /** \brief Whether \a t is cyclic. */
    bool is_cyclic(void) const;

    /** \brief The type of the transducer. */
    ImplementationType get_type(void) const;

    /** \brief Convert this transducer into an equivalent transducer in format \a type. 

	If a weighted transducer is converted into an unweighted one, all weights are lost. 
	In the reverse case, all weights are initialized to the semiring's one. */
    HfstTransducer &convert(ImplementationType type);

    /* \brief Create a new transducer equivalent to \a t in format \a type. 

	@see convert(ImplementationType type) */
    static HfstTransducer &convert(const HfstTransducer &t, ImplementationType type);

    HfstTransducer &operator=(const HfstTransducer &another);
    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
    friend class HfstOutputStream;
    friend class HfstMutableTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;

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
  class HfstMutableTransducer
  {
  protected:
    HfstTransducer transducer;
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
	between states \a source and \a target in this mutable transducer. */
    void add_transition(HfstState source, std::string isymbol, std::string osymbol, float w, HfstState target);
    /* friend classes */
    friend class HfstTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };

  /** \brief A state iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
  class HfstStateIterator
  {
  protected:
    hfst::implementations::TropicalWeightStateIterator tropical_ofst_iterator;
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

  /** \brief A transition in an HfstMutableTransducer. 

      Transitions are only returned by an \ref HfstTransitionIterator. 

      For an example of using transitions, see \ref iterator_example "this". 

      @see HfstTransitionIterator HfstMutableTransducer */
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

  /** \brief A transition iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
  class HfstTransitionIterator
  {
  protected:
    hfst::implementations::TropicalWeightTransitionIterator tropical_ofst_iterator;
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


  /** \brief The same as write_in_att_format(FILE*) 

      @see HfstTransducer::write_in_att_format(FILE*) */
  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);

  namespace rules
  {
    enum ReplaceType {REPL_UP, REPL_DOWN, REPL_RIGHT, REPL_LEFT};

    /* helping methods */
    HfstTransducer universal_fst(const StringPairSet &alphabet, ImplementationType type);
    HfstTransducer negation_fst(const HfstTransducer &t, const StringPairSet &alphabet);

    HfstTransducer replace(HfstTransducer &t, ReplaceType repl_type, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_transducer(HfstTransducer &t, std::string lm, std::string rm, ReplaceType repl_type, StringPairSet &alphabet);
    HfstTransducer replace_context(HfstTransducer &t, std::string m1, std::string m2, StringPairSet &alphabet);
    HfstTransducer replace_in_context(HfstTransducerPair &context, ReplaceType repl_type, HfstTransducer &t, bool optional, StringPairSet &alphabet);


    /** \brief ... */
    HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);
    HfstTransducer two_level_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);
    HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);

    HfstTransducer replace_up(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_down(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_right(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_left(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    HfstTransducer replace_up(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_down(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_right(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);
    HfstTransducer replace_left(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    HfstTransducer restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer surface_restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_restriction(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
    HfstTransducer deep_restriction_and_coercion(HfstTransducerPairSet &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
  }

}



#endif
