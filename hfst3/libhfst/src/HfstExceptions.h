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

#ifndef _HFST_EXCEPTIONS_H_
#define _HFST_EXCEPTIONS_H_

#include <string>

/** @file HfstExceptions.h
    \brief Exceptions that are thrown by the HFST API. */

namespace hfst
{ 

  /** \brief Namespace for HFST exceptions. */
namespace exceptions
{


/** \brief A superclass for all HFST exceptions. */
class HfstInterfaceException {
 protected:
  std::string message;
 public:
  /** \brief Create an exception with an empty message. */
  HfstInterfaceException();
  /** \brief Create an exception with message \a msg. */
  HfstInterfaceException(const std::string msg);
  /** \brief Get the exception message. */
  std::string get_message();
};


/** \brief A superclass for all exceptions that are thrown when an 
    argument does not satisfy preconditions. */
class HfstArgumentException :
public HfstInterfaceException {};

/** \brief A superclass for exceptions that are thrown when 
    a transducer in AT&T format cannot be parsed or a binary
    transducer does not follow the proper format. */
class HfstInputException :
public HfstInterfaceException {};

/** \brief A stream cannot be read or written or is not open.

An example:
\verbatim
// Read a transducer from file "testfile1" and write it to file "testfile2".
try {
  HfstInputStream in("testfile1");
  HfstTransducer t(in);
  HfstOutputStream out("testfile2");
  out << t;
} 
// File does not exist or some other error has happened
catch (HfstStreamException e) {
  fprintf(stderr, "Error in file handling.\n");
  exit(1);
}
// The file does not contain transducers or the transducer header is malformed
catch (HfstInputException e) {
  fprintf(stderr, "Error in transducer format.\n");
  exit(1);
}
\endverbatim
*/
class HfstStreamException :
public HfstInterfaceException {};

/** \brief The library required by the implementation type requested 
    is not linked to HFST. 

An example:
\verbatim
try {
  HfstTransducer("foo", "bar", type);
} catch (ImplementationTypeNotAvailableException e) {
  fprintf(stderr, "ERROR: Type requested is not available.\n");
} 
\endverbatim
*/
class ImplementationTypeNotAvailableException :
public HfstInterfaceException {};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
protected HfstInterfaceException {
 public:
 FunctionNotImplementedException(const std::string msg) : 
  HfstInterfaceException(msg) {};
 FunctionNotImplementedException() : 
  HfstInterfaceException() {};
};



/** \brief Stream cannot be read. 

An example:
\verbatim
try {
  HfstInputStream in("foofile");
} catch (StreamNotReadableException e) {
  fprintf(stderr, "ERROR: file cannot be read.\n");
}  
\endverbatim
*/
class StreamNotReadableException :
public HfstStreamException {};

/** \brief Stream cannot be written. 

An example:
\verbatim
try {
  HfstTransducer tr("foo", FOMA_TYPE);
  HfstOutputStream out("testfile");
  out << tr;
} catch (StreamCannotBeWrittenException e) {
  fprintf(stderr, "ERROR: file cannot be written.\n");
}  
\endverbatim
*/
class StreamCannotBeWrittenException :
public HfstStreamException {};

/** \brief Stream is closed. 

    An example:

\verbatim
try {
  HfstTransducer tr("foo", LOG_OPENFST_TYPE);
  HfstOutputStream out("testfile");
  out.close();
  out << tr;
} catch (StreamIsClosedException e) {
  fprintf(stderr, "ERROR: stream to file is closed.\n");
}  
/endverbatim
*/
class StreamIsClosedException :
public HfstStreamException {};



/** \brief Transducer is cyclic. 

    thrown by HfstTransducer::extract_strings. An example:
\verbatim
HfstTransducer transducer("a", "b", TROPICAL_OPENFST_TYPE);
transducer.repeat_star();
try {
  WeightedPaths<float>::Set results;
  transducer.extract_strings(results);
  fprintf(stderr, "The transducer has %i paths\n", results.size());
} catch (TransducerIsCyclicException e) {
    fprintf(stderr, "The transducer is cyclic and has an infinite number of paths.\n");
}
\endverbatim
*/
class TransducerIsCyclicException :
public HfstArgumentException {};

/** \brief The stream does not contain transducers. 

    An example. The file "foofile" contains
\verbatim
This is a text file.
Here is another line.
The file ends here.
\endverbatim

When we try to read it, an exception will be thrown:

\verbatim
try {
  HfstInputStream in("foofile");
} catch (NotTransducerStreamException e) {
  fprintf(stderr, "ERROR: file does not contain transducers.\n");
}  
\endverbatim
*/
class NotTransducerStreamException :
public HfstInputException {};

/** \brief The stream is not in valid AT&T format. 

    An example. The file "testfile.att" contains

\verbatim
0       1       a      b
1
c
\verbatim

When we try to read it, an exception is thrown:

\verbatim
std::vector<HfstTransducer> transducers;
FILE * ifile = fopen("testfile.att", "rb");
try {
  while (not eof(ifile))
    {
    HfstTransducer t(ifile, TROPICAL_OPENFST_TYPE, "epsilon");
    transducers.push_back(t);
    printf("read one transducer\n");
    }
} catch (NotValidAttFormatException e) {
    printf("Error reading transducer: not valid AT&T format.\n"); }
fclose(ifile);
fprintf(stderr, "Read %i transducers in total.\n", (int)transducers.size());
\endverbatim


    thrown by #hfst::HfstTransducer::HfstTransducer(FILE *, ImplementationType, const std::string&);
*/
class NotValidAttFormatException :
public HfstInputException {};

/** \brief State is not final (and cannot have a final weight). 

    An example:

\verbatim
HfstBasicTransducer tr;
tr.add_state(1);
// An exception is thrown as state number 1 is not final
float w = tr.get_final_weight(1);
\endverbatim

You should use function is_final_state if you are not sure whether a
state is final.

    Thrown by HfstInternalTransducer::get_final_weight. */
class StateIsNotFinalException :
public HfstArgumentException {};


/** \brief Context transducers are not automata.

    This exception is thrown by
    hfst::rules::replace_up(HfstTransducerPair&, HfstTransducer&, bool, StringPairSet&) 
    when either context transducer does not have equivalent input and
    output symbols in all its transitions.

    An example:

\verbatim
ImplementationType type = SFST_TYPE;
// The second context transducer is 
HfstTransducerPair contexts(HfstTransducer("c", type),
                            HfstTransducer("c", "d", type));
HfstTransducer mapping("a", "b", type);
StringPairSet alphabet;
alphabet.insert(StringPair("a", "a"));
alphabet.insert(StringPair("b", "b"));
alphabet.insert(StringPair("c", "c"));
alphabet.insert(StringPair("d", "d"));
hfst::rules::replace_up(contexts, mapping, true, alphabet);
\endverbatim

*/
class ContextTransducersAreNotAutomataException :
public HfstArgumentException {};

/** \brief The StateId argument is not valid.

    An example:

\verbatim
HfstBasicTransducer tr;
tr.add_state(1);
// An exception is thrown as there is no state number 2
float w = tr.get_final_weight(2);
\endverbatim
*/
class StateIndexOutOfBoundsException :
public HfstArgumentException {};

/** \brief Transducer has a malformed HFST header. 

    Thrown by HfstTransducer(HfstInputStream&). */
class TransducerHeaderException :
public HfstInputException {};

/** \brief An OpenFst transducer does not have an input symbol table. 

    When converting from OpenFst to tropical HFST, the OpenFst transducer
    must have at least an input symbol table. If the output symbol table
    is missing, it is assumed to be equivalent to the input symbol table.

    Thrown by HfstTransducer(HfstInputStream&)
*/
class MissingOpenFstInputSymbolTableException :
public HfstInputException {};

/** \brief The calling and/or called transducer do not have the same type. 

An example:
\verbatim
HfstTransducer foo("foo", SFST_TYPE);
HfstTransducer bar("bar", FOMA_TYPE);
foo.disjunct(bar);   // an exception is thrown 
\endverbatim
*/
class TransducerTypeMismatchException :
public HfstInterfaceException {};


/** \brief The set of transducer pairs is empty. 

    Thrown by rule functions in namespace #hfst::rules. An example:

\verbatim
    HfstTransducerPairVector contexts; // contexts is empty
    HfstTransducer rest = hfst::rules::restriction
      (contexts, mapping, alphabet, twol_type, direction); 
\endverbatim
*/
class EmptySetOfContextsException :
public HfstArgumentException {};


/* \brief The type of a transducer is not specified.

   This exception is thrown when an ImplementationType argument
   is ERROR_TYPE.

   @see ImplementationType
 */
class SpecifiedTypeRequiredException :
public HfstInterfaceException {};



/** \brief An error happened probably due to a bug in the HFST code. */
class HfstFatalException :
public HfstInterfaceException {};

/** \brief Transducer has wrong type. 

    This exception suggests that an HfstTransducer has not been properly
    initialized, probably due to a bug in the HFST library. Alternatively
    the default constructor of HfstTransducer has been called at some point. 

    @see hfst::HfstTransducer() */
class TransducerHasWrongTypeException :
public HfstFatalException {};

/** \brief Symbol is being redefined.

    This exception suggests that there is something wrong in the HFST code. */
class SymbolRedefinedException : 
public HfstFatalException {};

/** \brief Transducer has no start state.

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasNoStartStateException :
public HfstFatalException {};

/** \brief Transducer has more than one start state. 

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
public HfstFatalException {};


} }
#endif
