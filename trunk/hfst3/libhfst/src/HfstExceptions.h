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
class HfstSymbolsException {};


/** \brief A superclass for all HFST exceptions. */
class HfstInterfaceException {
 protected:
  std::string message;
 public:
  HfstInterfaceException();
  HfstInterfaceException(const std::string msg);
  std::string get_message();
};

/** \brief An argument does not satisfy preconditions. */
class HfstArgumentException :
public HfstInterfaceException {};

/** \brief The library required by the implementation type requested is not linked to HFST. 

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

/** \brief Error in transducer, text stream or string format. */
class HfstInputException :
public HfstInterfaceException {};


/** \brief Error in file handling. 

An example:
\verbatim
try {
  HfstInputStream in("testfile1");
  HfstTransducer t(in);
  HfstOutputStream out("testfile2");
  out << t;
} catch (HfstFileException e) {
    fprintf(stderr, "Error in file handling.\n");
    exit(1);
}
\endverbatim
*/
class HfstFileException :
public HfstInterfaceException {};

/** \brief File cannot be read. 

An example:
\verbatim
try {
  HfstInputStream in("testfile");
} catch (FileNotReadableException e) {
  fprintf(stderr, "ERROR: file cannot be read.\n");
}  
\endverbatim
*/
class FileNotReadableException :
public HfstFileException {};

/** \brief File cannot be written. */
class FileCannotBeWrittenException :
public HfstFileException {};

/** \brief File is closed. */
class FileIsClosedException :
public HfstFileException {};


/* \brief Transducer has unknown type. */
//class TransducerHasUnknownTypeException :
//public HfstInterfaceException {};


/** \brief Transducer is cyclic. 

    thrown by HfstTransducer::extract_strings. An example:
\verbatim
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

/** \brief The stream does not contain transducers. */
class NotTransducerStreamException :
public HfstInputException {};

/** \brief The stream is not in valid AT&T format. 

    thrown by #hfst::HfstTransducer::HfstTransducer(FILE *, ImplementationType, const std::string&);
*/
class NotValidAttFormatException :
public HfstInputException {};

/** \brief The string is not in valid format. 

    The tool hfst-calculate can throw this exception */
class NotValidStringFormatException :
public HfstInputException {};

/** \brief State is not final (and cannot have a final weight). 

    Thrown by HfstInternalTransducer::get_final_weight. */
class StateIsNotFinalException :
public HfstArgumentException {};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
protected HfstInterfaceException {
 public:
 FunctionNotImplementedException(const std::string msg) : HfstInterfaceException(msg) {};
 FunctionNotImplementedException() : HfstInterfaceException() {};

};


/** \brief Context transducers are not automata.

    Thrown by #hfst::rules::replace_up(HfstTransducerPair&, HfstTransducer&, bool, StringPairSet&) and similar functions. */
class ContextTransducersAreNotAutomataException :
public HfstArgumentException {};

/** \brief Transducer cannot be catenated the number of times requested.

    Thrown by HfstTransducer::repeat_n and similar functions. */
class ImpossibleTransducerPowerException :
public HfstArgumentException {};

/** \brief The StateId argument is not valid.

    This exception suggests that a StateId argument is not from HfstStateIterator. */
class StateIndexOutOfBoundsException :
public HfstArgumentException {};

/** \brief Transducer has a malformed HFST header. 

    Thrown by HfstTransducer(HfstInputStream&). */
class TransducerHeaderException :
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

    Thrown by rule functions in namespace #hfst::rules */
class EmptySetOfContextsException :
public HfstArgumentException {};

/* \brief The type of a transducer is not specified. */
class SpecifiedTypeRequiredException :
public HfstInterfaceException {};


/* \brief ... */
//class WeightTypeMismatchException :
//public HfstInterfaceException {};


/* \brief Something went wrong... */
class ErrorException :
public HfstInterfaceException {};


/** \brief An error happened probably due to a bug in the HFST code. */
class HfstFatalException :
public HfstInterfaceException {};

/** \brief Transducer has wrong type. */
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


// hfst-calculate throws
class ImpossibleReplaceTypeException :
public HfstInterfaceException {};

//class SymbolNotDefinedException : 
//public HfstSymbolsException {};
//class KeyNotDefinedException : 
//public HfstSymbolsException {};
//class StateTransitionIteratorOutOfRangeExeption : 
//public HfstInterfaceException {};
//class TransducerHasNoAlphabetException :
//public HfstInterfaceException {};
//class TransitionIteratorOutOfRangeException :
//public HfstInterfaceException {};
//class StateBelongsToAnotherTransducerException :
//public HfstInterfaceException {};


} }
#endif
