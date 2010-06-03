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
class NotValidAttFormatException :
public HfstInterfaceException {};

class StateTransitionIteratorOutOfRangeExeption : 
public HfstInterfaceException 
{};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
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

This exception suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
public HfstInterfaceException {};

/** \brief The transducer arguments do not have the same type. */
class TransducerTypeMismatchException :
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
