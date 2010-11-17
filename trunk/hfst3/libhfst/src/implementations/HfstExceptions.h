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

class SymbolNotDefinedException : 
public HfstSymbolsException {};

class KeyNotDefinedException : 
public HfstSymbolsException {};

class SymbolRedefinedException : 
public HfstSymbolsException {};

/** \brief A superclass for all HFST exceptions. */
class HfstInterfaceException 
{};

/** \brief The implementation type requested is not linked to HFST. */
class ImplementationTypeNotAvailableException :
public HfstInterfaceException {};

/** \brief File cannot be read. */
class FileNotReadableException :
public HfstInterfaceException {};

/** \brief File cannot be written. */
class FileCannotBeWrittenException :
public HfstInterfaceException {};

/** \brief File is closed. */
class FileIsClosedException :
public HfstInterfaceException {};


/** \brief Transducer has wrong type. */
class TransducerHasWrongTypeException :
public HfstInterfaceException {};

/** \brief Transducer has unknown type. */
class TransducerHasUnknownTypeException :
public HfstInterfaceException {};


/** \brief Transducer is cyclic. */
class TransducerIsCyclicException :
public HfstInterfaceException {};

class TransducerHasNoAlphabetException :
public HfstInterfaceException {};

/** \brief The stream does not contain transducers. */
class NotTransducerStreamException :
public HfstInterfaceException {};

/** \brief The stream is not in valid AT&T format. */
class NotValidAttFormatException :
public HfstInterfaceException {};

/** \brief The string is not in valid format. 

    hfst-calculate can throw this exception */
class NotValidStringFormatException :
public HfstInterfaceException {};

class StateTransitionIteratorOutOfRangeExeption : 
public HfstInterfaceException {};

/** \brief State is not final (and cannot have a final weight). */
class StateIsNotFinalException :
public HfstInterfaceException {};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
public HfstInterfaceException {};

class ImpossibleReplaceTypeException :
public HfstInterfaceException {};

class ContextTransducersAreNotAutomataException :
public HfstInterfaceException {};

/* ... */
class ImpossibleTransducerPowerException :
public HfstInterfaceException {};

/* ... */
class TransitionIteratorOutOfRangeException :
public HfstInterfaceException {};

/* ... */
class StateBelongsToAnotherTransducerException :
public HfstInterfaceException {};

/** \brief The StateId argument is not valid.

This exception suggests that the StateId argument is not from HfstStateIterator. */
class StateIndexOutOfBoundsException :
public HfstInterfaceException {};

class TransducerHeaderException :
public HfstInterfaceException {};

/** \brief Transducer has no start state.

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasNoStartStateException :
public HfstInterfaceException {};

/** \brief Transducer has more than one start state. 

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
public HfstInterfaceException {};

/** \brief The transducer arguments do not have the same type. */
class TransducerTypeMismatchException :
public HfstInterfaceException {};

/** \brief The set of transducer pairs is empty. */
class EmptySetOfContextsException :
public HfstInterfaceException {};

/* \brief ... */
class SpecifiedTypeRequiredException :
public HfstInterfaceException {};

/* \brief ... */
class WeightTypeMismatchException :
public HfstInterfaceException {};

/* \brief ... */
class ErrorException :
public HfstInterfaceException {};

} }
#endif
