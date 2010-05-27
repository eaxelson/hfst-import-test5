#ifndef _HFST_EXCEPTIONS_H_
#define _HFST_EXCEPTIONS_H_
namespace hfst
{ 

  /** \brief Namespase for exceptions. */
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

class StateTransitionIteratorOutOfRangeExeption : 
public HfstInterfaceException 
{};

/** \brief Function has not been implemented (yet). */
class FunctionNotImplementedException :
public HfstInterfaceException {};

/** \brief ... */
class ImpossibleTransducerPowerException :
public HfstInterfaceException {};

/** \brief ... */
class TransitionIteratorOutOfRangeException :
public HfstInterfaceException {};

/** \brief ... */
class StateBelongsToAnotherTransducerException :
public HfstInterfaceException {};

/** \brief ... */
class StateIndexOutOfBoundsException :
public HfstInterfaceException {};

/** \brief Transducer has no start state.

This exceptions suggests that there is something wrong in the HFST code. */
class TransducerHasNoStartStateException :
public HfstInterfaceException {};

/** \brief Transducer has more than one start state. 

This exceptions suggests that there is something wrong in the HFST code. */
class TransducerHasMoreThanOneStartStateException :
public HfstInterfaceException {};

/** \brief ... */
class SpecifiedTypeRequiredException :
public HfstInterfaceException {};

/** \brief ... */
class WeightTypeMismatchException :
public HfstInterfaceException {};

/** \brief ... */
class ErrorException :
public HfstInterfaceException {};

} }
#endif
