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

/** \brief A superclass for all exceptions. */
class HfstInterfaceException 
{};

/** \brief File cannot be read. */
class FileNotReadableException :
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
class NotValidAttFormat :
public HfstInterfaceException {};

/** \brief The implementation type requested does not support weights. */
class WeightsNotSupportedException :
public HfstInterfaceException {};

class StateTransitionIteratorOutOfRangeExeption : 
public HfstInterfaceException 
{};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
public HfstInterfaceException {};

/** \brief Information might get lost in the conversion requested.

    This exception is thrown if the type conversion of the calling transducer
    could result in lost information. This situation occurs if the ImplementationType parameter
    of a function requires that a weighted transducer (TROPICAL_OFST_TYPE or LOG_OFST_TYPE)
    is converted into an unweighted one (SFST_TYPE or FOMA_TYPE) or that a TROPICAL_OFST_TYPE
    transducer is converted into a LOG_OFST_TYPE transducer or vice versa.

    Use function \ref HfstTransducer::convert, if this is the conversion that is really intended.

\verbatim
    // this causes an error
    tropical_transducer.repeat_star(FOMA_TYPE);
    
    // this is ok
    tropical_transducer.convert(FOMA_TYPE);
    tropical_transducer.repeat_star(FOMA_TYPE);
    tropical_transducer.convert(TROPICAL_OFST_TYPE);
\endverbatim 
*/
class UnsafeConversionException :
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

/** \brief Transducer has no start state.

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasNoStartStateException :
public HfstInterfaceException {};

/** \brief Transducer has more than one start state. 

This exceptions suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
public HfstInterfaceException {};

class TypeMismatchException :
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
