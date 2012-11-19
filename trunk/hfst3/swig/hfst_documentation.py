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


## A simple transducer class with tropical weights.
#
#    An example of creating an HfstBasicTransducer [foo:bar baz:baz] 
#    with weight 0.4 from scratch:
#
# \verbatim
#  # Create an empty transducer
#  # The transducer has initially one start state (number zero) 
#  # that is not final
#  fsm = libhfst.HfstBasicTransducer()
#  # Add two states to the transducer
#  fsm.add_state(1)
#  fsm.add_state(2)
#  # Create a transition [foo:bar] leading to state 1 with weight 0.1 ...
#  tr = libhfst.HfstBasicTransition(1, 'foo', 'bar', 0.1)
#  # ... and add it to state zero
#  fsm.add_transition(0, tr)
#  # Add a transition [baz:baz] with weight 0 from state 1 to state 2 
#  fsm.add_transition(1, libhfst.HfstBasicTransition(2, 'baz', 'baz', 0.0))
#  # Set state 2 as final with weight 0.3
#  fsm.set_final_weight(2, 0.3)
# \endverbatim
#
#    An example of iterating through the states and transitions of the above transducer
#    when printing them in AT&T format to standard output:
#
# \verbatim
#  # Go through all states
#  for state in fsm.states(): 
#        # Go through all transitions
#        for transition in fsm.transitions(state):
#              print "%i %i %s %s %f" % (state, transition.get_target_state(), transition.get_input_symbol(), transition.get_output_symbol(), transition,get_weight())
#
#  if fsm.is_final_state(source_state): 
#        print "%i %f" % (state, fsm.get_final_weight(state))
# \endverbatim
#
# @see #HfstBasicTransducer #HfstBasicTransition
class HfstBasicTransducer:


    ## Add a new state to this transducer and return its number.      
    #  @return The next (smallest) free state number.
    def add_state():
        pass

    ## Add a state \a s to this graph.
    # If the state already exists, it is not added again.
    # All states with state number smaller than \a s are also
    # added to the transducer if they did not exist before.
    # @return \a state
    def add_state(state):
        pass

    ## The states of the transducer.
    # @return A tuple of state numbers.
    def states():
        pass

    ## Explicitly add \a symbol to the alphabet of the graph.
    # @note Usually the user does not have to take care of the alphabet
    #       of a graph. This function can be useful in some special cases.
    # @ param symbol The string to be added.
    def add_symbol_to_alphabet(symbol):
        pass

    ## Explicitly add \a symbols to the alphabet of the graph.
    # @note Usually the user does not have to take care of the alphabet
    #       of a graph. This function can be useful in some special cases.
    # @param symbols A tuple of strings to be added.
    def add_symbols_to_alphabet(symbols):
        pass

    ## Add a transition \a transition to state \a state, \a add_symbols_to_alphabet defines whether the transition symbols are added to the alphabet. 
    # If state \a state does not exist, it is created.
    def add_transition(state, transition, add_symbols_to_alphabet=True):
        pass
    
    ## TODO
    def disjunct(stringpairpath, weight):
        pass

    ## The symbols in the alphabet of the transducer.     
    # The symbols do not necessarily occur in any transitions of the transducer. 
    # Epsilon, unknown and identity symbols are always included in the alphabet.
    # @return A tuple of symbols (strings).
    def get_alphabet():
        pass

    ## Get the final weight of state \a state in this transducer.
    def get_final_weight(state):
        pass

    ## Get the biggest state number in use. 
    # @return The biggest state number in use.
    def get_max_state():
        pass

    ## TODO
    def harmonize(another):
        pass

    ## Create a transducer with one initial state that has state number zero and is not a final state, i.e. create an empty transducer.
    def __init__(self):
        pass

    ## Create a deep copy of HfstBasicTransducer \a transducer.
    def __init__(self, transducer):
        pass

    ## Create a transducer equivalent to HfstTransducer \a transducer.
    def __init__(self, transducer):
        pass

    ## Create a transducer as defined in file \a file in AT&T format.
    # TODO: An example.
    # @pre \a file is not at end, otherwise an exception is thrown.
    # @note Multiple AT&T transducer definitions are separated with the line "--".
    def __init__(self, file):
        pass

    ## Insert freely any number of \a symbol_pair in the transducer with weight \a weight.
    # @param symbol_pair A string pair to be inserted.
    # @param weight The weight of the inserted symbol pair.
    def insert_freely(symbol_pair, weight):
        pass

    ## Insert freely any number of any symbol in \a symbol_pairs in the graph with weight \a weight.
    # @param symbol_pairs A tuple of string pairs to be inserted.
    # @param weight The weight of the inserted symbol pair.
    def insert_freely(symbol_pairs, weight):
        pass

   ## Insert freely any number of \a transducer in this transducer.
   # param transducer An HfstBasicTransducer to be inserted.
    def insert_freely(transducer):
        pass

    ## Whether state \a state is final. 
    def is_final_state(state):
        pass

    ## The assignment operator.
    # @return This transducer.
    def assign(transducer):
        pass
    
   ## Get the transitions of state \a state in this transducer. 
   # If the state does not exist, a @a StateIndexOutOfBoundsException is thrown.
   # @return A tuple of state numbers.
    def transitions(state):
        pass

   ## Remove all symbols that do not occur in transitions of the transudcer from its alphabet. 
   #  Epsilon, unknown and identity symbols are always included in the alphabet.
    def prune_alphabet():
        pass
    
    ## Remove symbol \a symbol from the alphabet of the graph. 
    #  @note Use with care, removing symbols that occur in the transitions of the graph can have unexpected results.
    # @param symbol The string to be removed.
    def remove_symbol_from_alphabet(symbol):
        pass
    
    ## Remove symbols \a symbols from the alphabet of the graph. 
    #  @note Use with care, removing symbols that occur in the transitions of the graph can have unexpected results.
    # @param symbol A tuple of strings to be removed.
    def remove_symbols_from_alphabet(symbols):
        pass

    ## Set the final weight of state \a state in this transducer to \a weight. 
    # If the state does not exist, it is created.
    def set_final_weight(state, weight):
        pass
    
    ## Sort the arcs of this transducer according to input and output symbols.
    # @return This transducer.
    def sort_arcs():
        pass

    ## Substitute \a old_symbol with \a new_symbol in all transitions. \a input_side and \a output_side define whether the substitution is made on input and output sides.
    # @return This transducer.
    def substitute(old_symbol, new_symbol, input_side=True, output_side=True):
        pass
    
    ## Substitute all transition symbols as defined in \a substitutions.
    # For each transition symbol x, \a substitutions is searched and if a mapping x -> X is found,
    # the transition symbol x is replaced with X. If no mapping is found, the transition remains the same.
    # The weights remain the same.
    # @param substitutions A dictionary that maps symbols (strings) to symbols (strings).
    def substitute_symbols(substitutions):
        pass
    
    ## Substitute all transitions as defined in \a substitutions.
    # For each transition x:y, \a substitutions is searched and if a mapping x:y -> X:Y is found,
    # the transition x:y is replaced with X:Y. If no mapping is found, the transition remains the same.
    # The weights remain the same.
    # @param substitutions A dictionary that maps transitions (string pairs) to transitions (string pairs).
    def substitute_symbol_pairs(substitutions):
        pass
    
    ## Substitute all transitions \a sp with a set of transitions \a sps.
    # The weights remain the same.
    # @param sp A transition (string pair) to be substituted.
    # @param sps A tuple of substituting transitions (string pairs).
    def substitute(sp, sps):
        pass
    
    ## Substitute all transitions \a old_pair with \a new_pair.
    # @param old_pair The transition (string pair) to be substituted.
    # @param new_pair The substituting transition (string pair).
    def substitute(old_pair, new_pair):
        pass
        
    ## Substitute all transitions equal to \a sp with a copy of \a transducer
    #
    # Copies of \a transducer are attached to this graph with epsilon transitions.
    #
    # The weights of the transitions to be substituted are copied
    # to epsilon transitions leaving from the source state of
    # the transitions to be substituted to the initial state
    # of a copy of \a transducer.
    #
    # The final weights in \a transducer are copied to epsilon transitions leading from
    # the final states (after substitution non-final states)
    # of \a transducer to target states of transitions equal to \a sp
    # (that are substituted) in this transducer.
    #
    # @param sp The transition (string pair) to be substituted.
    # @param transducer The substituting transducer.
    def substitute(sp, transducer):
        pass
    
    ## Write this transducer in AT&T format to file \a file, \a write_weights defines whether weights are written.
    def write_in_att_format(file, write_weights=True):
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







    




