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
namespace hfst
{ 

  /** \brief Namespace for HFST exceptions. */
namespace exceptions
{
class HfstSymbolsException {};


/** \brief A superclass for all HFST exceptions. */
class HfstInterfaceException {};

/** \brief ... */
class HfstArgumentException :
public HfstInterfaceException {};

/** \brief The library required by the implementation type requested is not linked to HFST. */
class ImplementationTypeNotAvailableException :
public HfstInterfaceException {};

/** \brief ... */
class HfstInputException :
public HfstInterfaceException {};


/** \brief ... */
class HfstFileException :
public HfstInterfaceException {};

/** \brief File cannot be read. */
class FileNotReadableException :
//public HfstInterfaceException {};
public HfstFileException {};

/** \brief File cannot be written. */
class FileCannotBeWrittenException :
//public HfstInterfaceException {};
public HfstFileException {};

/** \brief File is closed. */
class FileIsClosedException :
//public HfstInterfaceException {};
public HfstFileException {};


/* \brief Transducer has unknown type. */
//class TransducerHasUnknownTypeException :
//public HfstInterfaceException {};


/** \brief Transducer is cyclic. 

    thrown by */
class TransducerIsCyclicException :
//public HfstInterfaceException {};
public HfstArgumentException {};

/** \brief The stream does not contain transducers. */
class NotTransducerStreamException :
//public HfstInterfaceException {};
public HfstInputException {};

/** \brief The stream is not in valid AT&T format. 

    thrown by #hfst::HfstTransducer(FILE *, ImplementationType, const std::string&);
*/
class NotValidAttFormatException :
//public HfstInterfaceException {};
public HfstInputException {};

/** \brief The string is not in valid format. 

    The tool hfst-calculate can throw this exception */
class NotValidStringFormatException :
//public HfstInterfaceException {};
public HfstInputException {};

/** \brief State is not final (and cannot have a final weight). 

    Thrown by HfstMutableTransducer::get_final_weight. */
class StateIsNotFinalException :
//public HfstInterfaceException {};
public HfstArgumentException {};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
public HfstInterfaceException {};


/** \brief Context transducers are not automata.

    Thrown by hfst::rules::replace_in_context. */
class ContextTransducersAreNotAutomataException :
//public HfstInterfaceException {};
public HfstArgumentException {};

/** \brief Transducer cannot be catenated the number of times requested.

    Thrown by HfstTransducer::repeat_n and similar functions. */
class ImpossibleTransducerPowerException :
//public HfstInterfaceException {};
public HfstArgumentException {};

/** \brief The StateId argument is not valid.

    This exception suggests that a StateId argument is not from HfstStateIterator. */
class StateIndexOutOfBoundsException :
//public HfstInterfaceException {};
public HfstArgumentException {};

/** \brief Transducer has a malformed HFST header. 

    Thrown by HfstTransducer(HfstInputStream&). */
class TransducerHeaderException :
//public HfstInterfaceException {};
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
//public HfstInterfaceException {};
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


/** \brief ... */
class HfstFatalException :
public HfstInterfaceException {};

/** \brief Transducer has wrong type. */
class TransducerHasWrongTypeException :
public HfstFatalException {};

/** \brief Symbol is being redefined.

    This exception suggests that there is something wrong in the HFST code. */
class SymbolRedefinedException : 
//public HfstSymbolsException {};
public HfstFatalException {};

/** \brief Transducer has no start state.

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasNoStartStateException :
//public HfstInterfaceException {};
public HfstFatalException {};

/** \brief Transducer has more than one start state. 

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
//public HfstInterfaceException {};
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
