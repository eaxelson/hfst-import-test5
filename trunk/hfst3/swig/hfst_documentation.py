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

## An SFST transducer, unweighted.
SFST_TYPE = _libhfst.SFST_TYPE

## An OpenFst transducer with tropical weights.
TROPICAL_OPENFST_TYPE = _libhfst.TROPICAL_OPENFST_TYPE

## An OpenFst transducer with logarithmic weights (limited support). 
LOG_OPENFST_TYPE = _libhfst.LOG_OPENFST_TYPE

## A foma transducer, unweighted.
FOMA_TYPE = _libhfst.FOMA_TYPE

## An HFST optimized lookup transducer, unweighted.
HFST_OL_TYPE = _libhfst.HFST_OL_TYPE

## An HFST optimized lookup transducer with weights.
HFST_OLW_TYPE = _libhfst.HFST_OLW_TYPE

## HFST2 header present, conversion required.
HFST2_TYPE = _libhfst.HFST2_TYPE

## Format left open by e.g. default constructor.
UNSPECIFIED_TYPE = _libhfst.UNSPECIFIED_TYPE

## Type not recognised. This type might be returned by a function if an error occurs.
ERROR_TYPE = _libhfst.ERROR_TYPE

## Push weights toward initial state.
# @see #hfst_documentation.HfstTransducer.push_weights
TO_INITIAL_STATE = _libhfst.TO_INITIAL_STATE

## Push weights toward final state(s).
# @see #hfst_documentation.HfstTransducer.push_weights
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
# @see #hfst_documentation.HfstBasicTransition
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
    
    ## Disjunct this transducer with a one-path transducer defined by string pair tuple \a spv that has weight \a weight.
    #
    # @pre This graph must be a trie where all weights are in final states,
    #      i.e. all transitions have a zero weight.
    #
    # There is no way to test whether a graph is a trie, so the use
    # of this function is probably limited to fast construction of a lexicon.
    # Here is an example:
    # 
    # \verbatim
    # lexicon = libhfst.HfstBasicTransducer()
    # TOK = libhfst.HfstTokenizer
    # lexicon.disjunct(TOK.tokenize('dog'), 0.3)
    # lexicon.disjunct(TOK.tokenize('cat'), 0.5)
    # lexicon.disjunct(TOK.tokenize('elephant'), 1.6)
    # \endverbatim
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

    ## Harmonize this transducer and \a another.
    #
    # In harmonization the unknown and identity symbols in transitions of both graphs
    # are expanded according to the symbols that are previously unknown to the graph.
    #
    # For example the graphs
    # \verbatim
    # [a:b ?:?]
    # [c:d ? ?:c]\endverbatim
    # are expanded to
    # \verbatim
    # [ a:b [?:? | ?:c | ?:d | c:d | d:c | c:? | d:?] ]
    # [ c:d [? | a | b] [?:c| a:c | b:?] ]\endverbatim
    # when harmonized.
    #
    # The symbol "?" means \@_UNKNOWN_SYMBOL_\@ in either or both sides of a transition
    # (transitions of type [?:x], [x:?] and [?:?]). The transition [?] means [\@_IDENTITY_SYMBOL_\@].
    #
    # @note This function is always called for all transducer arguments of functions
    #       that take two or more graphs as their arguments, unless otherwise said.
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
    # @param symbols A tuple of strings to be removed.
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

## A transition class that consists of a target state, input and output symbols and a a tropical weight.
# @see hfst_documentation.HfstBasicTransducer
class HfstBasicTransition:

    ## Create an HfstBasicTransition leading to target state \a state with input symbol \a input, output symbol \a output and weight \a weight.
    # @param state Number of the target state.
    # @param input The input string.
    # @param output The output string.
    # @param weight The weight.
    # @throws EmptyStringException
    def __init__(self, state, input, output, weight):
        pass

    ## Get the input symbol of the transition.
    def get_input_symbol():
        pass

    ## Get the output symbol of the transition.
    def get_output_symbol():
        pass

    ## Get number of the target state of the transition.
    def get_target_state():
        pass
        
    ## Get the weight of the transition.
    def get_weight():
        pass


## Print an HfstBasicTransition
def print(hfst_basic_transition):
    pass



## A synchronous finite-state transducer.
# 
# \section argument_handling Argument handling
# 
# Transducer functions modify their calling object and return 
# a reference to the calling object after modification, 
# unless otherwise mentioned.
# Transducer arguments are usually not modified.
# \verbatim
# # transducer is reversed
# transducer.reverse()
# # transducer2 is not modified, but a copy of it is disjuncted with
# # transducer1 
# transducer1.disjunct(transducer2)                                       
# # a chain of functions is possible
# transducer.reverse().determinize().reverse().determinize()      
# \endverbatim
# 
# \section implementation_types Implementation types
# 
# Currently, an HfstTransducer has three implementation types that are well supported.
# When an HfstTransducer is created, its type is defined with an argument.
# For functions that take a transducer as an argument, the type of the calling transducer
# must be the same as the type of the argument transducer:
# \verbatim
# # this will cause an error
# tropical_transducer.disjunct(sfst_transducer)
# # this works, but weights are lost in the conversion
# tropical_transducer.convert(libhfst.SFST_TYPE).disjunct(sfst_transducer)     
# # this works, information is not lost
# tropical_transducer.disjunct(sfst_transducer.convert(libhfst.TROPICAL_OPENFST_TYPE)) 
# \endverbatim
# 
# \section creating_transducers Creating transducers
# 
# With HfstTransducer constructors it is possible to create empty, 
# epsilon, one-transition and single-path transducers.
# Transducers can also be created from scratch with #hfst_documentation.HfstBasicTransducer
# and converted to an HfstTransducer.
# More complex transducers can be combined from simple ones with various functions.
# 
# <a name="symbols"></a> 
# \section special_symbols Special symbols
# 
# The HFST transducers support transitions with epsilon, unknown and identity symbols.
# The special symbols are explained in TODO
# 
# An example:
# \verbatim
# # In the xerox formalism used here, "?" means the unknown symbol
# # and "?:?" the identity pair 
# 
# tr1 = libhfst.HfstBasicTransducer()
# tr1.add_state(1)
# tr1.set_final_weight(1, 0)
# tr1.add_transition(0, libhfst.HfstBasicTransition(1, '@_UNKNOWN_SYMBOL_@, 'foo', 0) )
# 
# # tr1 is now [ ?:foo ]
# 
# tr2 = libhfst.HfstBasicTransducer tr2
# tr2.add_state(1)
# tr2.add_state(2)
# tr2.set_final_weight(2, 0)
# tr2.add_transition(0, libhfst.HfstBasicTransition(1, '@_IDENTITY_SYMBOL_@', '@_IDENTITY_SYMBOL_@', 0) )
# tr2.add_transition(1, libhfst.HfstBasicTransition(2, 'bar', 'bar', 0) )
# 
# # tr2 is now [ [ ?:? ] [ bar:bar ] ]
# 
# type = libhfst.SFST_TYPE
# Tr1 = HfstTransducer (tr1, type)
# Tr2 = HfstTransducer (tr2, type)
# Tr1.disjunct(Tr2)
# 
# # Tr1 is now [ [ ?:foo | bar:foo ]  |  [[ ?:? | foo:foo ] [ bar:bar ]] ] 
# \endverbatim
class HfstTransducer:

    ## Whether HFST is linked to the transducer library needed by implementation type \a type.
    def is_implementation_type_available(type):
        pass

    ## Create an uninitialized transducer (use with care). 
    # 
    # @note This constructor leaves the backend implementation variable
    # uninitialized. An uninitialized transducer is likely to cause a
    # TransducerHasWrongTypeException at some point unless it is given
    # a value at some point. 
    def __init__(self):
        pass

    ## Read a binary transducer from transducer stream \a in. 
    # 
    # The stream can contain tranducers or OpenFst, foma or SFST
    # type in their native format, i.e. without an HFST header. 
    # If the backend implementations are used as such, they are converted into HFST transducers.
    # 
    # For more information on transducer conversions and the HFST header
    # structure, see 
    # <a href="https://kitwiki.csc.fi/twiki/bin/view/KitWiki/HfstTransducerHeader">here</a>.
    # 
    # @pre ((in.is_eof() == in.is_bad() == False) and in.is_fst() ), Otherwise, an exception is thrown.
    # 
    # @throws NotTransducerStreamException 
    # @throws StreamNotReadableException
    # @throws StreamIsClosedException 
    # @throws TransducerTypeMismatchException
    # @throws MissingOpenFstInputSymbolTableException
    # 
    # @see hfst_documentation.HfstInputStream 
    # HfstTransducer(HfstInputStream &in)
    def __init__(self, in):
        pass

    ## Create a deep copy of transducer \a another. 
    # HfstTransducer(const HfstTransducer &another)
    def __init__(self, another):
        pass

    ## Create a transducer by tokenizing the utf8 string \a utf8_string with tokenizer \a multichar_symbol_tokenizer. The type of the transducer is defined by \a type. 
    # 
    # \a utf8_str is read one token at a time and for each token 
    # a new transition is created in the resulting
    # transducer. The input and output symbols of that transition are 
    # the same as the token read.
    # 
    # An example:
    # \verbatim
    # ustring = 'foobar'
    # TOK = libhfst.HfstTokenizer()
    # tr = libfhst.HfstTransducer(ustring, TOK, FOMA_TYPE)
    # # tr now contains one path [f o o b a r]
    # \endverbatim
    # 
    # @see hfst_documentation.HfstTokenizer 
    def __init__(self, utf8_str, multichar_symbol_tokenizer, type):
        pass

    ## Create a transducer by tokenizing  the utf8 input string \a input_utf8_string
    # and output string \a output_utf8_string with tokenizer \a multichar_symbol_tokenizer.
    # The type of the transducer is defined by \a type. 
    # 
    # \a input_utf8_str and \a output_utf8_str are read one token at a time
    # and for each token a new transition is created in the resulting transducer. 
    # The input and output symbols of that transition are the same as 
    # the input and output tokens read. If either string contains less tokens
    # than another, epsilons are used as transition symbols for the shorter string.
    # 
    # An example:
    # \verbatim
    # input = 'foo'
    # output = 'barr'
    # TOK = libhfst.HfstTokenizer()
    # tr = libhfst.HfstTransducer(input, output, TOK, libhfst.SFST_TYPE)
    # # tr now contains one path [f:b o:a o:r 0:r]
    # \endverbatim
    # 
    # @see hfst_documentation.HfstTokenizer 
    def __init__(self, input_utf8_str, output_utf8_str, multichar_symbol_tokenizer, type):
        pass

    ## Create an HFST transducer equivalent to HfstBasicTransducer \a t. The type of the created transducer is defined by \a type.  
    def __init__(self, t, type):
        pass

    ## Create an empty transducer, i.e. a transducer that does not recognize any string. The type of the transducer is defined by \a type.
    # 
    # @note Use HfstTransducer("@_EPSILON_SYMBOL_@") to create an epsilon transducer. 
    def __init__(self, type):
        pass

    ## Create a transducer that recognizes the string pair &lt;"symbol","symbol"&gt;, i.e. [symbol:symbol]. The type of the transducer is defined by \a type. 
    def __init__(self, symbol, type):
        pass

    ## Create a transducer that recognizes the string pair &lt;"isymbol","osymbol"&gt;, i.e [isymbol:osymbol]. The type of the transducer is defined by \a type. 
    def __init__(self, isymbol, osymbol, type):
        pass

    ## Create a transducer of type \a type as defined in AT&T format in file \a ifile. \a epsilon_symbol defines how epsilons are represented.
    # 
    # In AT&T format, the transition lines are of the form:
    # 
    # \verbatim 
    # [0-9]+[\w]+[0-9]+[\w]+[^\w]+[\w]+[^\w]([\w]+(-)[0-9]+(\.[0-9]+)) 
    # \endverbatim
    # 
    # and final state lines:
    # 
    # \verbatim
    # [0-9]+[\w]+([\w]+(-)[0-9]+(\.[0-9]+))
    # \endverbatim
    # 
    # If several transducers are listed in the same file, they are separated by lines of 
    # two consecutive hyphens "--". If the weight (<tt>([\\w]+(-)[0-9]+(\.[0-9]+))</tt>) 
    # is missing, the transition or final state is given a zero weight.
    # 
    # NOTE: If transition symbols contains spaces, they must be escaped
    # as "@_SPACE_@" because spaces are used as field separators.
    # Both "@0@" and "@_EPSILON_SYMBOL_@" are always interpreted as
    # epsilons.
    # 
    # 
    # An example:
    # \verbatim
    # 0      1      foo      bar      0.3
    # 1      0.5
    # --
    # 0      0.0
    # --
    # --
    # 0      0.0
    # 0      0      a        <eps>    0.2
    # \endverbatim
    # 
    # The example lists four transducers in AT&T format: 
    # one transducer accepting the string pair &lt;"foo","bar"&gt;, one
    # epsilon transducer, one empty transducer and one transducer 
    # that accepts any number of 'a's and produces an empty string
    # in all cases. The transducers can be read with the following commands (from a file named 
    # "testfile.att"):
    # \verbatim
    # transducers = []
    # ifile = open("testfile.att", "rb")
    # try:
    #     while (not ifile.eof()):
    #         t = libhfst.HfstTransducer(ifile, libhfst.TROPICAL_OPENFST_TYPE, "<eps>")
    #         transducers.append(t)
    #         print("read one transducer")
    # except NotValidAttFormatException:
    #     print("Error reading transducer: not valid AT&T format.")
    # ifile.close()
    # print "Read %i transducers in total" % len(transducers)
    # \endverbatim
    # 
    # Epsilon will be represented as "@_EPSILON_SYMBOL_@" in the resulting transducer.
    # The argument \a epsilon_symbol only denotes how epsilons are represented 
    # in \a ifile.
    # 
    # @throws NotValidAttFormatException 
    # @throws StreamNotReadableException
    # @throws StreamIsClosedException
    # @throws EndOfStreamException
    # @see #hfst_documentation.write_in_att_format 
    # 
    # HfstTransducer(HfstFile &ifile, ImplementationType type, const std::string &epsilon_symbol)
    def __init__(self, ifile, type, epsilon_symbol):
        pass

    ## Create a transducer that recognizes (any number of, if \a cyclic is True) the union of string pairs in \a sps. The type of the transducer is defined by \a type. 
    # @param sps A tuple of consecutive string input/output pairs
    # @param type The type of the transducer.
    # @param cyclic Whether the transducer recognizes any number (from zero to infinity, inclusive) of consecutive string pairs in \a sps. 
    def __init__(self, sps, type, cyclic=False):
        pass
 
    ## Create a transducer that recognizes the concatenation of string pairs in \a spv. The type of the transducer is defined by \a type.
    # @param spv A tuple of string pairs.
    # @param type The type of the transducer. 
    def __init__(self, spv, type):
        pass
 
    ## Create a transducer that recognizes the concatenation of the unions of string pairs in string pair sets in \a spsv. The type of the transducer is defined by \a type. 
    # @param spvs A tuple of tuples of string input/output pairs.
    # @param type The type of the transducer.
    def __init__(self, spsv, type)

    ## Rename the transducer \a name. 
    # @see get_name 
    def set_name(name):
        pass
 
    ## Get the name of the transducer. 
    # @see set_name 
    def get_name():
        pass

    ## Set arbitrary string property \a property to \a value.
    #
    # set_property("name") equals set_name(string&).
    #
    # @note  While this function is capable of creating endless amounts of arbitrary metadata, it is suggested that property names are
    # drawn from central repository, or prefixed with "x-". A property that does not follow this convention may affect the behavior of
    # transducer in future releases.
    def set_property(property, value):
        pass

    ## Get arbitrary string propert @a property.
    # get_property("name") works like get_name.
    def get_property(property):
        pass

    ## Get all properties from the transducer.
    # @return A dictionary of string-to-string mappings.
    def get_properties():
        pass

    ## Get the alphabet of the transducer. 
    # 
    # The alphabet is defined as the set of symbols known to the transducer. 
    # @return A tuple of strings.
    def get_alphabet():
        pass

    ## Explicitly insert \a symbol to the alphabet of the transducer. 
    # 
    # @note Usually this function is not needed since new symbols are
    # added to the alphabet by default. 
    def insert_to_alphabet(symbol):
        pass
 
    ## Remove \a symbol from the alphabet of the transducer. CURRENTLY NOT IMPLEMENTED.
    # 
    # @pre \a symbol does not occur in any transition of the transducer.
    # @note Use with care, removing a symbol that occurs in a transition
    # of the transducer can have unexpected results. 
    def remove_from_alphabet(symbol):
        pass

    ## Whether the transducer is cyclic. 
    def is_cyclic(void):
        pass

    ## The implementation type of the transducer. 
    def get_type(void):
        pass
 
    ## Whether this transducer and \a another are equivalent.
    # 
    # Two transducers are equivalent iff they accept the same input/output
    # string pairs with the same weights and the same alignments. 
    def compare(another):
        pass

    ## Remove all <i>epsilon:epsilon</i> transitions from the transducer so that the resulting transducer is equivalent to the original one. 
    def remove_epsilons():
        pass

    ## Determinize the transducer.
    # 
    # Determinizing a transducer yields an equivalent transducer that has
    # no state with two or more transitions whose input:output symbol
    # pairs are the same. 
    def determinize():
        pass
    
    ## Minimize the transducer.
    # 
    # Minimizing a transducer yields an equivalent transducer with 
    # the smallest number of states. 
    # 
    # @bug OpenFst's minimization algorithm seems to add epsilon transitions to weighted transducers? 
    def minimize():
        pass
    
    ## Extract \a n best paths of the transducer. 
    # 
    # In the case of a weighted transducer (#TROPICAL_OPENFST_TYPE or 
    # #LOG_OPENFST_TYPE), best paths are defined as paths with 
    # the lowest weight.
    # In the case of an unweighted transducer (#SFST_TYPE or #FOMA_TYPE), 
    # the function returns random paths.
    # 
    # This function is not implemented for #FOMA_TYPE or #SFST_TYPE.
    # If this function is called by an HfstTransducer of type #FOMA_TYPE 
    # or #SFST_TYPE, it is converted to #TROPICAL_OPENFST_TYPE,
    # paths are extracted and it is converted back to #FOMA_TYPE or 
    # #SFST_TYPE. If HFST is not linked to OpenFst library, an
    # ImplementationTypeNotAvailableException is thrown.
    def n_best(n):
        pass
    
    ## A concatenation of N transducers where N is any number from zero to infinity. 
    def repeat_star():
        pass
    
    ## A concatenation of N transducers where N is any number from one to infinity. 
    def repeat_plus():
        pass
    
    ## A concatenation of \a n transducers. 
    def repeat_n(unsigned int n):
        pass
    
    ## A concatenation of N transducers where N is any number from zero to \a n, inclusive.
    def repeat_n_minus(n):
        pass
    
    ## A concatenation of N transducers where N is any number from \a n to infinity, inclusive.
    def repeat_n_plus(n):
        pass
    
    ## A concatenation of N transducers where N is any number from \a n to \a k, inclusive.
    def repeat_n_to_k(n, k):
        pass
    
    ## Disjunct the transducer with an epsilon transducer. 
    def optionalize():
        pass
    
    ## Swap the input and output symbols of each transition in the transducer. 
    def invert():
        pass
    
    ## Reverse the transducer. 
    # 
    # A reverted transducer accepts the string "n(0) n(1) ... n(N)" 
    # iff the original
    # transducer accepts the string "n(N) n(N-1) ... n(0)" 
    def reverse():
        pass
    
    ## Extract the input language of the transducer. 
    # 
    # All transition symbol pairs <i>isymbol:osymbol</i> are changed 
    # to <i>isymbol:isymbol</i>. 
    def input_project():
        pass
    
    ## Extract the output language of the transducer.
    # 
    # All transition symbol pairs <i>isymbol:osymbol</i> are changed 
    # to <i>osymbol:osymbol</i>. 
    def output_project():
        pass
    
    ## Compose this transducer with \a another. 
    def compose(another):
        pass
    
    ## Compose this transducer with the intersection of
    # transducers in \a v. If \a invert is true, then compose the
    # intersection of the transducers in \a v with this transducer.
    # 
    # The algorithm used by this function is faster than intersecting 
    # all transducers one by one and then composing this transducer 
    # with the intersection. 
    # 
    # @pre The transducers in \a v are deterministic and epsilon-free.
    # @param v A tuple of transducers.
    def compose_intersect(v, invert=False):
        pass
    
    ## Concatenate this transducer with \a another. 
    def concatenate(another):
        pass
    
    ## Disjunct this transducer with \a another. 
    def disjunct(another):
        pass

    ## Intersect this transducer with \a another. 
    def intersect(another):
        pass
    
    ## Subtract transducer \a another from this transducer. 
    def subtract(another):
        pass

    ## Convert the transducer into an equivalent transducer in format \a type. 
    # 
    # If a weighted transducer is converted into an unweighted one, 
    # all weights are lost. In the reverse case, all weights are initialized to the 
    # semiring's one. 
    # 
    # A transducer of type #libhfst.SFST_TYPE, #libhfst.TROPICAL_OPENFST_TYPE,
    # #libhfst.LOG_OPENFST_TYPE or #FOMA_TYPE can be converted into an 
    # #libhfst.HFST_OL_TYPE or #libhfst.HFST_OLW_TYPE transducer, but an #libhfst.HFST_OL_TYPE
    # or #libhfst.HFST_OLW_TYPE transducer cannot be converted to any other type.
    # 
    # @note For conversion between HfstBasicTransducer and HfstTransducer,
    # see #HfstTransducer(t, type) and #HfstBasicTransducer(transducer).
    def convert(type, options=""):
        pass
    
    ## Write the transducer in AT&T format to file \a ofile, \a write_weights defines whether weights are written.
    # 
    # The fields in the resulting AT&T format are separated by tabulator characters.
    # 
    # NOTE: If the transition symbols contain space characters,the spaces are printed as "@_SPACE_@" because
    # whitespace characters are used as field separators in AT&T format. Epsilon symbols are printed as "@0@".
    # 
    # If several transducers are written in the same file, they must be separated by a line of two consecutive hyphens "--", so that
    # they will be read correctly by HfstTransducer(file, type, epsilon).
    # 
    # An example:
    # \verbatim
    # type = libhfst.FOMA_TYPE
    # foobar = libhfst.HfstTransducer("foo","bar",type)
    # epsilon = libhfst.HfstTransducer("@_EPSILON_SYMBOL_@",type)
    # empty = libhfst.HfstTransducer(type)
    # a_star = libhfst.HfstTransducer("a",type)
    # a_star.repeat_star()
    # 
    # ofile = open("testfile.att", "wb")
    # foobar.write_in_att_format(ofile)
    # ofile.write("--\n")
    # epsilon.write_in_att_format(ofile)
    # ofile.write("--\n")
    # empty.write_in_att_format(ofile)
    # ofile.write("--\n")
    # a_star.write_in_att_format(ofile)
    # ofile.close()
    # \endverbatim
    # 
    # This will yield a file "testfile.att" that looks as follows:
    # \verbatim
    # 0    1    foo  bar  0.0
    # 1    0.0
    # --
    # 0    0.0
    # --
    # --
    # 0    0.0
    # 0    0    a    a    0.0
    # \endverbatim
    # 
    # @throws StreamCannotBeWrittenException 
    # @throws StreamIsClosedException
    # 
    # @see redirect(out, t)
    # @see HfstTransducer(file, type, epsilon) 
    def write_in_att_format(ofile, write_weights=True):
        pass

    ## Write the transducer in AT&T format to file named \a filename. \a write_weights defines whether weights are written.
    # 
    # If the file exists, it is overwritten. If the file does not exist, it is created. 
    def write_in_att_format(filename, write_weights=True):
        pass

    ## Make priority union of this transducer with \a another.
    #
    # For the operation t1.priority_union(t2), the result is a union of t1 and t2,
    # except that whenever t1 and t2 have the same string on left side,
    # the path in t2 overrides the path in t1.
    # 
    # Example
    # </verbatim>
    # Transducer 1 (t1):
    # a : a
    # b : b
    # 
    # Transducer 2 (t2):
    # b : B
    # c : C
    # 
    # Result ( t1.priority_union(t2) ):
    # a : a
    # b : B
    # c : C
    # </endverbatim>
    # For more information, read: www.fsmbook.com
    #  
    def priority_union(another):
        pass
    
    ## Make cross product of this transducer with \a another. It pairs every string of this with every string of \a another.
    # If strings are not the same length, epsilon padding will be added in the end of the shorter string.
    # @pre Both transducers must be automata, i.e. map strings onto themselves.
    def cross_product(another):
        pass
    
    ## Shuffle this transducer with transducer \@ another.
    # 
    # If transducer A accepts string "foo" and transducer B string "bar",
    # the transducer that results from shuffling A and B accepts all strings
    # [(f|b)(o|a)(o|r)].
    #   
    # @pre Both transducers must be automata, i.e. map strings onto themselves.
    def shuffle(another):
        pass
        
    ## Freely insert symbol pair \a symbol_pair into the transducer. 
    # 
    # To each state in this transducer is added a transition that 
    # leads from that state to itself with input and output symbols 
    # defined by \a symbol_pair.
    # @param symbol_pair A string pair to be inserted.
    def insert_freely(symbol_pair):
        pass
    
    ## Freely insert a copy of \a tr into the transducer. 
    # 
    # A copy of \a tr is attached with epsilon transitions 
    # to each state of this transducer. After the operation, for each 
    # state S in this transducer, there is an epsilon transition 
    # that leads from state S to the initial state of \a tr, 
    # and for each final state of \a tr, there is an epsilon transition
    # that leads from that final state to state S in this transducer.
    # The weights of the final states in \a tr are copied to the 
    # epsilon transitions leading to state S.
    # 
    # Implemented only for HfstBasicTransducer. 
    # Conversion is carried out for an HfstTransducer, if this function
    # is called.
    # 
    def insert_freely(tr):
        pass
    
    ## Set the weights of all final states to \a weight. 
    # If the HfstTransducer is of unweighted type (#libhfst.SFST_TYPE or #libhfst.FOMA_TYPE), nothing is done.
    def set_final_weights(float weight):
        pass
    
    ## Push weights towards initial or final state(s) 
    # as defined by \a type.
    # 
    # If the HfstTransducer is of unweighted type 
    # (#SFST_TYPE or #FOMA_TYPE), nothing is done.
    # @see libhfst.TO_INITIAL_STATE libhfst.TO_FINAL_STATE
    # 
    def push_weights(PushType type):
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

    ## Lookup or apply a single tokenized string \a tok_input and return a maximum of \a limit results.
    # 
    # TODO: This is a version of lookup that handles flag diacritics as ordinary
    # symbols and does not validate the sequences prior to outputting. Currently, this function calls lookup_fd.
    #
    # @todo Handle flag diacritics as ordinary symbols instead of calling lookup_fd.
    # @sa lookup_fd
    # @return HfstOneLevelPaths pointer
    # @param tok_input A tuple of consecutive symbols (strings).
    # @param limit Number of strings to look up. -1 tries to look up all and may get stuck if infinitely ambiguous.
    def lookup(tok_input, limit=-1):
        pass
    
    # @brief Lookup or apply a single string \a input and return a maximum of \a limit results.
    # 
    # This is an overloaded lookup function that leaves tokenizing to the transducer.
    # @return HfstOneLevelPaths pointer
    # @see #lookup(tok_string, limit=-1)
    def lookup(input, limit=-1):
        pass
    
    ## Lookup or apply a single string \a input and return a maximum of \a limit results. \a tok defined how \a s is tokenized.
    #
    # This is an overloaded lookup function that leaves tokenizing to \a tok.
    # @return HfstOneLevelPaths pointer
    # @see #lookup(tok_string, limit=-1)
    def lookup(tok, input, limit=-1):
        pass
    
    
    ## Lookup or apply a single string \a tok_input minding flag diacritics properly and return a maximum of \a limit results.
    #
    # Traverse all paths on logical first level of the transducer to produce
    # all possible outputs on the second.
    # This is in effect a fast composition of single path from left
    # hand side.
    #
    # This is a version of lookup that handles flag diacritics as epsilons
    # and validates the sequences prior to outputting.
    # Epsilons on the second level are represented by empty strings
    # in the results.
    #
    # @pre The transducer must be of type #libhfst.HFST_OL_TYPE or #libhfst.HFST_OLW_TYPE. This function is not implemented for other transducer types.
    #
    # @param tok_input  A tuple of consecutive symbols (strings) to look up.
    # @param limit  (Currently ignored.) Number of strings to look up. -1 tries to look up all and may get stuck if infinitely ambiguous.
    # 
    # @see #libhfst.is_lookup_infinitely_ambiguous
    # @return HfstOneLevelPaths pointer
    #
    # @todo Do not ignore argument \a limit.
    def lookup_fd(tok_input, limit = -1):
        pass
    
    ## Lookup or apply a single string \a s minding flag diacritics properly and return a maximum of \a limit results.
    #
    # This is an overloaded lookup function that leaves tokenizing to the transducer.
    # @return HfstOneLevelPaths pointer
    # @see #lookup(tok_string, limit=-1)
    def lookup_fd(input, limit = -1):
        pass
    
    
    ## Lookup or apply a single string \a input minding flag diacritics properly and return a maximum of \a limit results. \a tok defines how s is tokenized.
    #
    # This is an overloaded lookup function that leaves tokenizing to \a tok.
    # @return HfstOneLevelPaths pointer
    # @see #lookup(tok_string, limit=-1)
    def lookup_fd(tok, input, limit = -1):
        pass

    ## Whether lookup of path \a input will have infinite results.
    #
    # Currently, this function will return whether the transducer
    # is infinitely ambiguous on any lookup path found in the transducer,
    # i.e. the argument \a input is ignored.
    #
    # @todo Do not ignore the argument \a input
    # @see #libhfst.lookup(tok_input, limit=-1)
    def is_lookup_infinitely_ambiguous(tok_input):
        pass


## Return the HfstTransducer pointed by \a transducer_ptr.
def ptrvalue(transducer_ptr):
    pass

## Extract a maximum of \a max_num paths that are recognized by \a transducer following a maximum of \a cycles cycles.
# 
# @return An HfstTwoLevelPaths that contains the extracted paths.
# @param max_num The total number of resulting strings is capped at \a max_num, with 0 or negative indicating unlimited. 
# @param cycles Indicates how many times a cycle will be followed, with negative numbers indicating unlimited.
# 
# This is a version of extract_paths that handles flag diacritics 
# as ordinary symbols and does not validate the sequences prior to
# outputting as opposed to #libhfst.extract_paths_fd(transducer, int, int, bool)
# 
# If this function is called on a cyclic transducer with unlimited
# values for both \a max_num and \a cycles, an exception will be thrown.
# 
# This example
# 
# \verbatim
# type = libhfst.SFST_TYPE
# tr1 = libhfst.HfstTransducer("a", "b", type)
# tr1.repeat_star()
# tr2 = libhfst.HfstTransducer("c", "d", type)
# tr2.repeat_star()
# tr1.concatenate(tr2).minimize()
# results = libhfst.detokenize_paths(libhfst.extract_paths(tr1, MAX_NUM, CYCLES))
# 
# for path in results:
#     print "%s : &s" % (path.input, path.output)
# \endverbatim
# 
# prints with values MAX_NUM == -1 and CYCLES == 1 all paths
# that have no consecutive cycles:
# 
# \verbatim
# a : b
# ac : bd
# acc : bdd
# c : d
# cc : dd
# \endverbatim
# 
# and with values MAX_NUM == 7 and CYCLES == 2 a maximum of 7 paths
# that follow a cycle a maximum of 2 times (there are 11 such paths,
# but MAX_NUM limits their number to 7):
# 
# \verbatim
# a : b
# aa : bb
# aac : bbd
# aacc : bbdd
# c : d
# cc : dd
# ccc : ddd
# \endverbatim
# 
# @bug Does not work for HFST_OL_TYPE or HFST_OLW_TYPE?
# @throws TransducerIsCyclicException
# @see #n_best 
# @see libhfst.extract_paths_fd(transducer, max_num=-1, cycles=-1, filter_fd)
# 
def extract_paths(transducer, max_num=-1, cycles=-1):
    pass

## Extract a maximum of \a max_num paths that are recognized by the transducer and are not invalidated by flag diacritic rules following a maximum of \a cycles cycles. \a filter_fd defines whether the flag diacritics themselves are filtered out of the result strings.
#
# @return An HfstTwoLevelPaths that contains the extracted paths.
# @param max_num The total number of resulting strings is capped at \a max_num, with 0 or negative indicating unlimited. 
# @param cycles Indicates how many times a cycle will be followed, with negative numbers indicating unlimited.
# @param filter_fd  Whether the flag diacritics are filtered out of the result strings.
# 
# If this function is called on a cyclic transducer with unlimited
# values for both \a max_num and \a cycles, an exception will be thrown.
# 
# Flag diacritics are of the form @[PNDRCU][.][A-Z]+([.][A-Z]+)?@. 
# 
# For example the transducer 
# 
# \verbatim
# [[@U.FEATURE.FOO@ foo] | [@U.FEATURE.BAR@ bar]]  |  [[foo @U.FEATURE.FOO@] | [bar @U.FEATURE.BAR@]]
# \endverbatim
# 
# will yield the paths <CODE>[foo foo]</CODE> and <CODE>[bar bar]</CODE>.
# <CODE>[foo bar]</CODE> and <CODE>[bar foo]</CODE> are invalidated
# by the flag diacritics so thay will not be included in \a results.
# 
# 
# @bug Does not work for HFST_OL_TYPE or HFST_OLW_TYPE?
# @throws TransducerIsCyclicException
# @see libhfst.extract_paths(transducer, max_num=-1, cycles=-1) 
def extract_paths_fd(transducer, max_num=-1, cycles=-1, filter_fd=False):
    pass

## Detokenize \a tokenized_paths.
#
# Concatenate all transition symbols on input and output levels for each path in \a tokenized_paths.
# 
# @param tokenized_paths An HfstOneLevelPaths pointer or an HfstTwoLevelPaths.
# @return A tuple of HfstPaths.
def detokenize_paths(tokenized_paths):
    pass

## Detokenize and remove all flag diacritics from \a tokenized_paths.
#
# Concatenate all transition symbols except flag diacritics on input and output levels for each path in \a tokenized_paths.
# 
# @param tokenized_paths An HfstOneLevelPaths pointer or an HfstTwoLevelPaths.
# @return A tuple of HfstPaths.
def detokenize_and_purge_paths(tokenized_paths):
    pass


 
# TODO: void extract_random_paths
# (HfstTwoLevelPaths &results, int max_num) const;
# 
# void extract_random_paths_fd
# (HfstTwoLevelPaths &results, int max_num, bool filter_fd) const;
# 
# /* \brief Call \a callback with extracted strings that are not 
# invalidated by flag diacritic rules.
# 
# @see extract_paths(HfstTwoLevelPaths&, int, int) 
# void extract_paths_fd
# (ExtractStringsCb& callback, int cycles=-1, bool filter_fd=true) const;


 

