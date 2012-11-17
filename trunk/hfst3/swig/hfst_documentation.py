## Package libhfst 
#
@package libhfst

## A class that represents a path in a transducer
# 
# Has members weight, input and output.
#
class HfstPath:
    pass

## A superclass for all exceptions
class HfstException:
    pass

## documentation (todo)
class HfstTransducerTypeMismatchException(HfstException):
    pass

## documentation (todo)
class ImplementationTypeNotAvailableException(HfstException):
    pass

## documentation (todo)
class FunctionNotImplementedException(HfstException):
    pass

## documentation (todo)
class StreamNotReadableException(HfstException):
    pass

## documentation (todo)
class StreamCannotBeWrittenException(HfstException):
    pass

## documentation (todo)
class StreamIsClosedException(HfstException):
    pass

## documentation (todo)
class EndOfStreamException(HfstException):
    pass

## documentation (todo)
class TransducerIsCyclicException(HfstException):
    pass

## documentation (todo)
class NotTransducerStreamException(HfstException):
    pass

## documentation (todo)
class NotValidAttFormatException(HfstException):
    pass

## documentation (todo)
class NotValidLexcFormatException(HfstException):
    pass

## documentation (todo)
class StateIsNotFinalException(HfstException):
    pass

## documentation (todo)
class ContextTransducersAreNotAutomataException(HfstException):
    pass

## documentation (todo)
class TransducersAreNotAutomataException(HfstException):
    pass

## documentation (todo)
class StateIndexOutOfBoundsException(HfstException):
    pass

## documentation (todo)
class TransducerHeaderException(HfstException):
    pass

## documentation (todo)
class MissingOpenFstInputSymbolTableException(HfstException):
    pass

## documentation (todo)
class TransducerTypeMismatchException(HfstException):
    pass

## documentation (todo)
class EmptySetOfContextsException(HfstException):
    pass

## documentation (todo)
class SpecifiedTypeRequiredException(HfstException):
    pass

## documentation (todo)
class HfstFatalException(HfstException):
    pass

## documentation (todo)
class TransducerHasWrongTypeException(HfstException):
    pass

## documentation (todo)
class IncorrectUtf8CodingException(HfstException):
    pass

## documentation (todo)
class EmptyStringException(HfstException):
    pass

## documentation (todo)
class SymbolNotFoundException(HfstException):
    pass

## documentation (todo)
class MetadataException(HfstException):
    pass

## sfst type
SFST_TYPE = _libhfst.SFST_TYPE

## type
TROPICAL_OPENFST_TYPE = _libhfst.TROPICAL_OPENFST_TYPE

## type
LOG_OPENFST_TYPE = _libhfst.LOG_OPENFST_TYPE

## type
FOMA_TYPE = _libhfst.FOMA_TYPE

## type
HFST_OL_TYPE = _libhfst.HFST_OL_TYPE

## type
HFST_OLW_TYPE = _libhfst.HFST_OLW_TYPE

## type
HFST2_TYPE = _libhfst.HFST2_TYPE

## type
UNSPECIFIED_TYPE = _libhfst.UNSPECIFIED_TYPE

## type
ERROR_TYPE = _libhfst.ERROR_TYPE

## type
TO_INITIAL_STATE = _libhfst.TO_INITIAL_STATE

## type
TO_FINAL_STATE = _libhfst.TO_FINAL_STATE


## A wrapper for file, possibly needed in Windows
class HfstFile:
  ## Close the file
  def close():
      pass
  ## Write \a string to the file
  def write(string):
      pass

## Opens the file \a filename with arguments \a args
# @return HfstFile
def hfst_open(filename, args):
    pass

## Extract a maximum of \a max_num paths in \a transducer following a maximum of \a cycles cycles
# @return HfstTwoLevelPaths
def extract_paths(transducer, max_num=-1, cycles=-1):
    pass
## Extract a maximum of \a max_num paths that obey flag diacritics in \a transducer following a maximum of \a cycles cycles, \a filter_fd defines whether flags are filtered
# @return HfstTwoLevelPaths
def extract_paths_fd(transducer, max_num=-1, cycles=-1, filter_fd=False):
    pass


class HfstBasicTransducer:

    ## Returns HfstState
    def add_state():
        pass

    ## Returns HfstState
    # @param state The number of the state that is added
    def add_state(state):
        pass

    ## Returns a tuple of ints, const
    def states():
        pass

    ## void
    # @ param symbol The string to be added
    def add_symbol_to_alphabet(symbol):
        pass

    ## void, tuple of symbols
    def add_symbols_to_alphabet(symbols):
        pass

    ## void state number \a state, HfstBasicTransition \a transition, \a add_symbols_to_alphabet 
    def add_transition(state, transition, add_symbols_to_alphabet=True):
        pass
    
    ## Add the path \a stringpairpath which has total weight of \a weight to this transducer.
    # @param stringpairpath A tuple(?) of string pairs 
    # @return HfstBasicTransducer &
    def disjunct(stringpairpath, weight):
        pass

    ## returns const StringSet &, const
    def get_alphabet():
        pass

    ## returns float, const, throw (StateIsNotFinalException)
    def get_final_weight(state):
        pass

    ## returns HfstState, const
    def get_max_state():
        pass

    ## returns HfstBasicTransducer, HfstBasicTransducer
    def harmonize(another):
        pass

    ##
    def __init__(self):
        pass

    ## const HfstBasicTransducer&
    def __init__(self, graph):
        pass

    ## const HfstTransducer&
    def __init__(self, transducer):
        pass

    ## Returns &HfstTransducer, StringPair, float
    def insert_freely(symbol_pair, weight):
        pass

    ## Returns &HfstTransducer, StringPairSet, float
    def insert_freely(symbol_pairs, weight):
        pass

    ## Returns &HfstTransducer, const HfstBasicTransducer&
    def insert_freely(graph):
        pass

    ## returns Bool, HfstState, const
    def is_final_state(state):
        pass

    ## returns HfstBasicTransducer&, const HfstBasicTransducer &
    def assign(graph):
        pass
    
    ## const HfstTransitions &, HfstState, const
    def transitions(state):
        pass
    
    ## void
    def prune_alphabet():
        pass
    
    ## void, const std::string&
    def remove_symbol_from_alphabet(symbol):
        pass
    
    ## void, HfstState, const float&
    def set_final_weight(state, weight):
        pass
    
    ## returns HfstBasicTransducer&
    def sort_arcs():
        pass
    
    ## returns HfstBasicTransducer&, const std::string&, same, bool, bool
    def substitute(old_symbol, new_symbol, input_side=True, output_side=True):
        pass
    
    ## returns HfstBasicTransducer&, const HfstSymbolSubstitutions & 
    def substitute_symbols(substitutions):
        pass
    
    ## returns HfstBasicTransducer&, const HfstSymbolPairSubstitutions 
    def substitute_symbol_pairs(&substitutions):
        pass
    
    ## returns HfstBasicTransducer&, const StringPair&, const StringPairSet&
    def substitute(sp, sps):
        pass
    
    ## returns HfstBasicTransducer&, const StringPair &old_pair, const StringPair &new_pair)
    def substitute(old_pair, new_pair):
        pass
        
    ## returns HfstBasicTransducer&, const StringPair &sp, const HfstBasicTransducer &graph)
    def substitute(sp, graph):
        pass
    
    ## void, std::ostream &os, bool write_weights=true) ???
    def write_in_att_format(os, write_weights=True):
        pass
    
    ## void, (FILE *file, bool write_weights=true)
    def write_in_att_format(file, write_weights=True):
        pass
    
    ## void, FILE *file, bool write_weights=true)
    def write_in_att_format_number(file, write_weights=True):
        pass


## Returns HfstBasicTransducer, (std::istream &is, std::string epsilon_symbol, unsigned int & linecount) throw (NotValidAttFormatException);
def read_in_att_format(is, epsilon_symbol, linecount):
    pass

## Returns HfstBasicTransducer, FILE *file, std::string epsilon_symbol, unsigned int & linecount) throw (NotValidAttFormatException);
def read_in_att_format (file, epsilon_symbol, linecount):
    pass

## Returns HfstBasicTransducer, HfstFile &file, std::string epsilon_symbol, unsigned int & linecount) throw (NotValidAttFormatException);
def read_in_att_format (file, epsilon_symbol, linecount):
    pass

## Print an HfstBasicTransducer
def print(hfst_basic_transducer):
    pass


class HfstBasicTransition:

    ## HfstState s, const std::string &input, const std::string &output, float weight) throw (EmptyStringException)
    def __init__(self, state, input, output, weight):
        pass

    ## string, const
    def get_input_symbol():
        pass

    ## string, const
    def get_output_symbol():
        pass

    ## HfstState, const
    def get_target_state():
        pass
        
    ## float, const
    def get_weight():
        pass


## Print an HfstBasicTransition
def print(hfst_basic_transition):
    pass
