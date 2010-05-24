#ifndef _HFST_EXCEPTIONS_H_
#define _HFST_EXCEPTIONS_H_
namespace hfst
{ 
namespace exceptions
{
class HfstSymbolsException {};

class SymbolNotDefinedException : 
public HfstSymbolsException {};

class KeyNotDefinedException : 
public HfstSymbolsException {};

class SymbolRedefinedException : 
public HfstSymbolsException {};

class HfstInterfaceException 
{};

class FileNotReadableException :
public HfstInterfaceException {};

class FileIsClosedException :
public HfstInterfaceException {};

class TransducerHasWrongTypeException :
public HfstInterfaceException {};

class TransducerHasUnknownTypeException :
public HfstInterfaceException {};

class TransducerIsCyclicException :
public HfstInterfaceException {};

class TransducerHasNoAlphabetException :
public HfstInterfaceException {};

class NotTransducerStreamException :
public HfstInterfaceException {};

class StateTransitionIteratorOutOfRangeExeption : 
public HfstInterfaceException 
{};

class FunctionNotImplementedException :
public HfstInterfaceException {};

class ImpossibleTransducerPowerExcpetion :
public HfstInterfaceException {};

class TransitionIteratorOutOfRangeExeption :
public HfstInterfaceException {};

class StateBelongsToAnotherTransducerException :
public HfstInterfaceException {};

class StateIndexOutOfBoundsException :
public HfstInterfaceException {};

class TransducerHasNoStartStateException :
public HfstInterfaceException {};

class TransducerHasMoreThanOneStartStateException :
public HfstInterfaceException {};

class SpecifiedTypeRequiredException :
public HfstInterfaceException {};

class WeightTypeMismatchException :
public HfstInterfaceException {};

class ErrorException :
public HfstInterfaceException {};

} }
#endif
