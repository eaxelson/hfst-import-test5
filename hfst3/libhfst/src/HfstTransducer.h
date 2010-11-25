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

#include "HfstDataTypes.h"
#include "SymbolDefs.h"

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

#include "HfstAlphabet.h"
#include "implementations/HfstOlTransducer.h"
#include "HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include "HfstExceptions.h"

#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <set>

/** @file HfstTransducer.h
    \brief Declarations of HFST API functions and datatypes. 

    This file must be included to a program that uses the HFST API. */

/** \brief A namespace for HFST functions and datatypes. */
namespace hfst
{

/** \mainpage 

HFST - The Helsinki Finite-State Transducer technology is intended for creating and manipulating weighted or unweighted synchronic transducers implementing regular relations.
Currently HFST has been implemented using the 
<a href="http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html">SFST</a>, 
<a href="http:://www.openfst.org">OpenFst</a> and 
<a href="http://foma.sourceforge.net/">foma</a> software libraries.
Other versions may be added in some future release. 
SFST and foma implementations are unweighted and OpenFst implementation is weighted.
The examples given in this documentation use <a href="http://www.xrce.xerox.com/Research-Development/Publications/1997-005/(language)">Xerox transducer notation</a>.
<!-- More info on SFST tools is in http://www.ims.uni-stuttgart.de/projekte/gramotron/SOFTWARE/SFST.html,
OpenFst in http://www.openfst.org and foma in http://www.aclweb.org/anthology/E/E09/E09-2008.pdf. -->


<b>TODO: A quick start</b>

The HFST API is written in the namespace #hfst that contains the following classes:

   - HfstTransducer: A class for creating transducers and performing operations on them.

   - HfstInputStream and HfstOutputStream: Classes for writing and reading binary transducers.

   - HfstMutableTransducer, HfstTransition, HfstTransitionIterator and HfstStateIterator: Classes for creating transducers from scratch and iterating through their states and transitions.

   - HfstTokenizer: A class used in creating transducers from UTF-8 strings.

   - HfstGrammar: A class used in \link HfstTransducer::compose_intersect intersecting composition\endlink.

and the following namespaces:

   - \link hfst::rules rules\endlink: A namespace that contains functions to create two-level, replace, restriction and coercion rules.

   - \link hfst::xfst xfst\endlink: ?

   - \link hfst::lexc lexc\endlink: A namespace for Xerox LexC related specific functions and classes.

   - \link hfst::xre xre\endlink: A namespace for functions related to regular expression parsing.

   - \link hfst::exceptions exceptions\endlink: A namespace for exceptions.

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

  // New transducer type
#if HAVE_FOO
  using hfst::implementations::FooTransducer;
#endif


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



  /** \brief A synchronous finite-state transducer.

    \section argument_handling Argument handling

    Transducer functions modify their calling object and return a reference to it, unless otherwise said.
    Transducer arguments are usually not modified.
\verbatim
    transducer.reverse();                                    // transducer is now reversed

    tr1.disjunct(tr2);                                       // tr2 is not modified, but a copy of it is disjuncted with t1

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
HfstTransducer tr1("@_UNKNOWN_SYMBOL_@", "foo", type);
// tr1 is [ @_UNKNOWN_SYMBOL_@:foo ]

HfstTransducer tr2("@_IDENTITY_SYMBOL_@", type);
HfstTransducer tmp("bar", type);
tr2.concatenate(tmp); 
// tr is [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ ] [ bar:bar ] ]

tr1.disjunct(tr2);
// tr1 is expanded to [ @_UNKNOWN_SYMBOL_@:foo | bar:foo ]
// tr2 is expanded to [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ | foo:foo ] [ bar:bar ] ]
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

#if HAVE_FOO
      hfst::implementations::FooTransducer * foo;
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

#if HAVE_FOO
    static hfst::implementations::FooTransducer foo_interface;
#endif

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
       std::string ustring = "foobar";
       HfstTokenizer TOK;
       HfstTransducer tr(ustring, TOK, LOG_OFST_TYPE);
       // tr now contains one path [f o o b a r]
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
@see #write_in_att_format(FILE*,bool)const String
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

    /** \brief Remove all <i>epsilon:epsilon</i> transitions from the transducer. */
    HfstTransducer &remove_epsilons();

    /** \brief Determinize the transducer.

	Determinizing a transducer yields an equivalent transducer that has
	no state with two or more transitions whose input:output symbol pairs are the same. */
    HfstTransducer &determinize();

    /** \brief Minimize the transducer.

	Minimizing a transducer yields an equivalent transducer with the smallest number of states. 
     
	@bug OpenFst's minimization algorithm seems to add epsilon transitions to weighted transducers? */
    HfstTransducer &minimize();

    /** \brief Extract \a n best paths of the transducer. 

	In the case of a weighted transducer (#TROPICAL_OFST_TYPE or #LOG_OFST_TYPE), best paths are defined as paths with the lowest weight.
	In the case of an unweighted transducer (#SFST_TYPE or #FOMA_TYPE), best paths are defined as paths with the smallest number of transitions, i.e. the shortest paths. 

        This function is not implemented for #FOMA_TYPE or #SFST_TYPE.
	If this function is called by an HfstTransducer of type #FOMA_TYPE or #SFST_TYPE, it is converted to #TROPICAL_OFST_TYPE,
	strings are extracted and it is converted back to #FOMA_TYPE or #SFST_TYPE.
    */
    HfstTransducer &n_best(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from zero to infinity. */
    HfstTransducer &repeat_star();

    /** \brief A concatenation of N transducers where N is any number from one to infinity. */
    HfstTransducer &repeat_plus();

    /** \brief A concatenation of \a n transducers. */
    HfstTransducer &repeat_n(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from zero to \a n, inclusive.*/
    HfstTransducer &repeat_n_minus(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from \a n to infinity, inclusive.*/
    HfstTransducer &repeat_n_plus(unsigned int n);

    /** \brief A concatenation of N transducers where N is any number from \a n to \a k, inclusive.*/
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k);

    /** \brief Disjunct the transducer with an epsilon transducer. */
    HfstTransducer &optionalize();

    /** \brief Swap the input and output labels of each transition in the transducer. */
    HfstTransducer &invert();

    /** \brief Reverse the transducer. 

	A reverted transducer accepts the string "n(0) n(1) ... n(N)" iff the original
	transducer accepts the string "n(N) n(N-1) ... n(0)" */
    HfstTransducer &reverse();

    /** \brief Extract the input language of the transducer. 

	All transition symbol pairs <i>isymbol:osymbol</i> are changed to <i>isymbol:isymbol</i>. */
    HfstTransducer &input_project();

    /** \brief Extract the output language of the transducer.

	All transition symbol pairs <i>isymbol:osymbol</i> are changed to <i>osymbol:osymbol</i>. */
    HfstTransducer &output_project();

    /* \brief Call \a callback with some or all string pairs recognized by the transducer?

	If the callback returns false the search will be terminated. The \a cycles parameter
	indicates how many times a cycle will be followed, with negative numbers
	indicating unlimited. Note that if the transducer is cyclic and cycles aren't capped,
	the search will not end until the callback returns false. */
    void extract_strings(ExtractStringsCb& callback, int cycles=-1);
	  
    std::vector<HfstTransducer*> extract_paths();

	  /** \brief Extract a maximum of \a max_num string pairs that are recognized by the transducer
	      following a maximum of \a cycles cycles and store the string pairs into \a results.

	The total number of resulting strings is capped at \a max_num, with 0 or negative
	indicating unlimited. The \a cycles parameter indicates how many times a cycle
	will be followed, with negative numbers indicating unlimited. If this function
	is called on a cyclic transducer with unlimited values for both \a max_num and
	\a cycles, an exception will be thrown.
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

  The same conditions that apply for the #extract_strings function apply also for this one.
  Flag diacritics are of the form @[A-Z][.][A-Z]+([.][A-Z]+)?@ An example:
\verbatim
TODO...
\endverbatim
  @throws hfst::exceptions::TransducerIsCyclicException
  @see extract_strings(WeightedPaths<float>::Set&, int, int) */
    void extract_strings_fd(WeightedPaths<float>::Set &results, int max_num=-1, int cycles=-1, bool filter_fd=true);

    /** \brief Freely insert symbol pair \a symbol_pair into the transducer. */
    HfstTransducer &insert_freely(const StringPair &symbol_pair);

    /** \brief Substitute all transition pairs <i>isymbol</i>:<i>osymbol</i> as defined in function \a func. 

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

    /** \brief Substitute all transitions equal to \a old_symbol_pair 
	with a set of transitions equal to \a new_symbol_pair_set. 

	The weight of the original transition is copied to all new transitions.

	Implemented only for #TROPICAL_OFST_TYPE and #LOG_OFST_TYPE.
	If this function is called by an unweighted HfstTransducer (#SFST_TYPE or #FOMA_TYPE), it is converted to #TROPICAL_OFST_TYPE,
	substitution is done and it is converted back to the original format.

	@see String
     */
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPairSet &new_symbol_pair_set);

    /** \brief Substitute all transitions equal to \a symbol_pair with a copy of transducer \a transducer. 

	A copy of \a transducer is attached (using epsilon transitions) between the source and target states of the transition to be substituted.
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

    /** \brief Transform all transition and state weights as defined in \a func. 

     An example:
\verbatim
float func(float f) { 
  return 2*f + 0.5; 
}

...

// All transition and final weights are multiplied by two and summed with 0.5.
HfstTransducer t_transformed;
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
	all transducers one by one and then composing this transducer with the intersection. 

	@see HfstGrammar */
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

    
    /** \brief Whether the transducer is cyclic. */
    bool is_cyclic(void) const;

    /** \brief The implementation type of the transducer. */
    ImplementationType get_type(void) const;


    /* TEST */
    static float get_profile_seconds(ImplementationType type);
    /* Get all symbol pairs that occur in the transitions of the transducer. */
    StringPairSet get_symbol_pairs();
    /* Whether HFST is linked to the transducer library needed by implementation type \a type. */
    static bool is_implementation_type_available(ImplementationType type);
    /* Create a tokenizer that recognizes all symbols that occur in the transducer. */
    HfstTokenizer create_tokenizer();

    /** \brief Convert the transducer into an equivalent transducer in format \a type. 

	If a weighted transducer is converted into an unweighted one, all weights are lost. 
	In the reverse case, all weights are initialized to the semiring's one. */
    HfstTransducer &convert(ImplementationType type);

    /* \brief Create a new transducer equivalent to \a t in format \a type. 

	@see convert(ImplementationType type) */
    static HfstTransducer &convert(const HfstTransducer &t, ImplementationType type);


    //! @brief Lookup or apply a single string \a s and store a maximum of 
    //! \a limit results to \a results.
    //!
    //! Traverse all paths on logical first level of the transducer to produce
    //! all possible outputs on the second.
    //! This is in effect a fast composition of single path from left
    //! hand side.
    //!
    //! @param results  output parameter to store unique results
    //! @param s  string to look up
    //! @param limit  number of strings to extract. -1 tries to extract all and
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

    //! @brief Lookdown a single string \a s and store a maximum of 
    //! \a limit results to \a results.
    //!
    //! Traverse all paths on logical second level of the transducer to produce
    //! all possible inputs on the first.
    //! This is in effect a fast composition of single
    //! path from left hand side.
    //!
    //! @param results  output parameter to store unique results
    //! @param s  string to look down
    //! @param tok  tokenizer to split string in arcs?
    //! @param limit  number of strings to extract. -1 tries to extract all and
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

    //! @brief Whether lookup of path \a s will have infinite results.
    bool is_lookup_infinitely_ambiguous(const HfstLookupPath& s);

    //! @brief Whether lookdown of path \a s will have infinite results.
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

    /* Used by hfst-calculate. */
    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet,
			       TwolType twol_type, int direction ); 



    /** \brief A transducer that obligatorily performs the mappings defined by \a mappings in the context \a context
	when the alphabet is \a alphabet. 

	For example, a transducer yielded by the following arguments
\verbatim
context = pair( [c|d], [e] )
mappings = set(a:b)
alphabet = set(a, a:b, b, c, d, e, ...)
\endverbatim
	obligatorily maps the symbol a to b if c or d precedes and e follows. (Elsewhere,
	the mapping of a to b is optional)
	This expression is identical to ![.* [c|d] [a:. & !a:b] [e] .*]
	Note that the alphabet must contain the pair a:b here.
	
	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>
    */
    HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);

    /** \brief A transducer that allows the mappings defined by \a mappings only in the context \a context,
	when the alphabet is \a alphabet. 
	
	If called with the same arguments as in the example of #two_level_if, the transducer
	allows the mapping of symbol a to b only if c or d precedes and e follows. The
	mapping of a to b is optional in this context but cannot occur in any other context.
	The expression is equivalent to ![  [ ![.* [c|d]] a:b .* ] | [ .* a:b ![[e] .*] ]  ]
	
	@see #two_level_if
    */
    HfstTransducer two_level_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);

    /** \brief A transducer that always performs the mappings defined by \a mappings in the context \a context
	and only in that context, when the alphabet is \a alphabet. 

	If called with the same arguments as in the example of #two_level_if, the transducer
	maps symbol a to b only and only if c or d precedes and e follows.
	The mapping of a to b is obligatory in this context and cannot occur in any other context.
	The expression is equivalent to ![.* [c|d] [a:. & !a:b] [e] .*]  &
	![  [ ![.* [c|d]] a:b .* ] | [ .* a:b ![[e] .*] ]  ]
	
	@see #two_level_if
    */
    HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet);



    /** \brief A transducer that performs an upward mapping \a mapping in the context \a context when the alphabet is \a alphabet.
	\a optional defines whether the mapping is optional. 

	Each substring s of the input string which is in the input language
	of the transducer \a mapping and whose left context is matched by the expression
	[.* l] (where l is the first element of \a context) and whose right context is matched by [r .*] 
	(where r is the second element in the context) is mapped to the respective
	surface strings defined by transducer \a mapping. Any other character is mapped to
	the characters specified in \a alphabet. The left and right contexts must
	be automata (i.e. transducers which map strings onto themselves).

	For example, a transducer yielded by the following arguments
\verbatim
context = pair( [c], [c] )
mappings = [ a:b a:b ]
alphabet = set(a, b, c)
\endverbatim
	would map the string "caacac" to "cbbcac".

	Note that the alphabet must contain the characters a and b, but not the pair
	a:b (unless this replacement is to be allowed everywhere in the context).

	Note that replace operations (unlike the two-level rules) have to be combined by composition
	rather than intersection.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>
    */
    HfstTransducer replace_up(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /** \brief The same as replace_up, but matching is done on the output side of \a mapping 

	@see replace_up <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer replace_down(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /** \brief The same as replace_up, but left context matching is done on the output side of \a mapping
	and right context on the input side of \a mapping 

	@see replace_up <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer replace_right(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /** \brief The same as replace_up, but left context matching is done on the input side of \a mapping
	and right context on the output side of \a mapping. 

	@see replace_up <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer replace_left(HfstTransducerPair &context, HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /** \brief The same as replace_up but \a mapping is performed in every context. 

	@see replace_up */
    HfstTransducer replace_up(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);

    /** \brief The same as #replace_down(HfstTransducerPair&, HfstTransducer&, bool, StringPairSet&)
	but \a mapping is performed in every context.

	@see replace_up */
    HfstTransducer replace_down(HfstTransducer &mapping, bool optional, StringPairSet &alphabet);



    /** \brief A transducer that allows any (substring) mapping defined by \a mapping
	only if it occurs in any of the contexts in \a contexts. Symbols outside of the matching
	substrings are mapped to any symbol allowed by \a alphabet. 

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that requires that one of the mappings defined by \a mapping
	must occur in each context in \a contexts. Symbols outside of the matching
	substrings are mapped to any symbol allowed by \a alphabet.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that is equivalent to the intersection of restriction and coercion
	and requires that the mappings defined by \a mapping occur always and only in the
	given contexts in \a contexts. Symbols outside of the matching
	substrings are mapped to any symbol allowed by \a alphabet.

	@see #restriction(HfstTransducerPairVector&, HfstTransducer&, StringPairSet&) #coercion <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a> */
    HfstTransducer restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that specifies that a string from the input language of the
	transducer \a mapping may only be mapped to one of its output strings (according
	to transducer \a mapping) if it appears in any of the contexts in \a contexts.
	Symbols outside of the matching substrings are mapped to any symbol allowed by \a alphabet.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer surface_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that specifies that a string from the input language of the transducer
	\a mapping always has to the mapped to one of its output strings according to
	transducer \a mapping if it appears in any of the contexts in \a contexts.
	Symbols outside of the matching substrings are mapped to any symbol allowed by \a alphabet.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer surface_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that is equivalent to the intersection of surface_restriction and surface_coercion.

	@see #surface_restriction #surface_coercion <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer surface_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that specifies that a string from the output language of the transducer
	\a mapping may only be mapped to one of its input strings (according to transducer \a mappings)
	if it appears in any of the contexts in \a contexts.
	Symbols outside of the matching substrings are mapped to any symbol allowed by \a alphabet.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer deep_restriction(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that specifies that a string from the output language of the transducer
	\a mapping always has to be mapped to one of its input strings (according to transducer \a mappings)
	if it appears in any of the contexts in \a contexts.
	Symbols outside of the matching substrings are mapped to any symbol allowed by \a alphabet.

	@see <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer deep_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);

    /** \brief A transducer that is equivalent to the intersection of deep_restriction and deep_coercion.

	@see #deep_restriction #deep_coercion <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">SFST manual</a>. */
    HfstTransducer deep_restriction_and_coercion(HfstTransducerPairVector &contexts, HfstTransducer &mapping, StringPairSet &alphabet);
  }

}


// vim: set ft=cpp.doxygen:
#endif
