 
## Base class for HfstExceptions. Holds its own name and the file and line number where it was thrown.

 

## Two or more HfstTransducers are not of the same type.
# HFST_EXCEPTION_CHILD_DECLARATION(HfstTransducerTypeMismatchException);

 
## The library required by the implementation type requested is not linked to HFST. 
# 
# An example:
# \verbatim
# try:
#     tr = libhfst.HfstTransducer("foo", "bar", type)
# except ImplementationTypeNotAvailableException:
#     print "ERROR: Type requested is not available."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(ImplementationTypeNotAvailableException);

 
## Function has not been implemented (yet).
# HFST_EXCEPTION_CHILD_DECLARATION(FunctionNotImplementedException);
 

## Stream cannot be read. 
# 
# Thrown by
# libhfst.HfstTransducer(in) and
# libhfst.HfstTransducer(file, type, epsilon)
# 
# An example:
# \verbatim
# try;
#     in = libhfst.HfstInputStream("foofile")
# except StreamNotReadableException:
#     print "ERROR: file cannot be read."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(StreamNotReadableException);

 
## Stream cannot be written. 
# 
# Thrown by #redirect and #write_in_att_format
# 
# An example:
# \verbatim
# try:
#     tr = libhfst.HfstTransducer("foo", libhfst.FOMA_TYPE)
#     out = HfstOutputStream("testfile")
#     out.redirect(tr)
# except StreamCannotBeWrittenException e:
#     print "ERROR: file cannot be written."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(StreamCannotBeWrittenException);

 
## Stream is closed. 
# 
# Thrown by #write_in_att_format
# #HfstTransducer(file, type, epsilon)
# #HfstTransducer(in)
# #HfstOutputStream::redirect
# 
# An example:
# 
# \verbatim
# try:
#     tr = libhfst.HfstTransducer("foo", tropical.TROPICAL_OPENFST_TYPE)
#     out = libhfst.HfstOutputStream("testfile")
#     out.close()
#     out,redirect(tr)
# except StreamIsClosedException:
#     print "ERROR: stream to file is closed."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(StreamIsClosedException);

 
## The stream is at end.
# 
#     Thrown by
#     #HfstTransducer(in)
#     #HfstInputStream() 
#     #HfstInputStream(filename)
# HFST_EXCEPTION_CHILD_DECLARATION(EndOfStreamException);

 
## Transducer is cyclic. 
# 
#     Thrown by #HfstTransducer.extract_paths and
#     #HfstTransducer.extract_paths_fd. An example:
# \verbatim
# transducer = libhfst.HfstTransducer("a", "b", libhfst.TROPICAL_OPENFST_TYPE)
# transducer.repeat_star()
# try:
#     results = libhfst.detokenize_paths(libhfst.extract_paths())
#     print "The transducer has %i paths" % len(results)
# except TransducerIsCyclicException:
#     print "The transducer is cyclic and has an infinite number of paths."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(TransducerIsCyclicException);
 
 
## The stream does not contain transducers. 
# 
#     Thrown by 
#     #HfstTransducer(istr)
#     #HfstInputStream() 
#     #HfstInputStream(filename)
# 
#     An example. The file "foofile" contains
# \verbatim
# This is a text file.
# Here is another line.
# The file ends here.
# \endverbatim
# 
# When we try to read it, an exception will be thrown:
# 
# \verbatim
# try:
#     in = libhfst.HfstInputStream("foofile")
# except NotTransducerStreamException:
#     print "ERROR: file does not contain transducers."
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(NotTransducerStreamException);

 
## The stream is not in valid AT&T format. 
# 
#     An example. The file "testfile.att" contains
# 
# \verbatim
# 0       1       a      b
# 1
# c
# \verbatim
# 
# When we try to read it, an exception is thrown:
# 
# \verbatim
# transducers = []
# ifile = open("testfile.att", "rb")
# try:
#     while not ifile.eof():    
#         t = libhfst.HfstTransducer(ifile, libhfst.TROPICAL_OPENFST_TYPE, "epsilon")
#         transducers.append(t)
#         print "read one transducer"
# except NotValidAttFormatException:
#     print "Error reading transducer: not valid AT&T format."
# ifile.close()
# print "Read %i transducers in total." % len(transducers)
# \endverbatim
#
#     thrown by 
#     #HfstTransducer(file, type, epsilon)
# HFST_EXCEPTION_CHILD_DECLARATION(NotValidAttFormatException);
 
 
# HFST_EXCEPTION_CHILD_DECLARATION(NotValidLexcFormatException);

 
## State is not final (and cannot have a final weight). 
# 
#     An example:
# 
# \verbatim
# tr = libhfst.HfstBasicTransducer()
# tr.add_state(1)
# # An exception is thrown as state number 1 is not final
# w = tr.get_final_weight(1)
# \endverbatim
# 
# You should use function #is_final_state if you are not sure whether a
# state is final.
# 
# Thrown by #HfstBasicTransducer get_final_weight. */
# HFST_EXCEPTION_CHILD_DECLARATION(StateIsNotFinalException); 
 
 
## Context transducers are not automata.
# 
#     This exception is thrown by
#     #replace_up(contexts, mapping, optional, alphabet)
#     when either context transducer does not have equivalent input and
#     output symbols in all its transitions.
# 
#     An example:
# 
# \verbatim
# type = libhfst.SFST_TYPE
# # The second context transducer is 
# contexts = (libhfst.HfstTransducer("c", type), libhfst.HfstTransducer("c", "d", type))
# mapping = libhfst.HfstTransducer("a", "b", type)
# alphabet = ('a','a'), ('b','b'), ('c','c'), ('d','d')
# # An exception is thrown
# rule = libhfst.replace_up(contexts, mapping, True, alphabet)
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(ContextTransducersAreNotAutomataException);


## Transducers are not automata.
# 
#     This exception is thrown by
#     #cross_product(transducer) 
#     when either input transducer does not have equivalent input and
#     output symbols in all its transitions.
# HFST_EXCEPTION_CHILD_DECLARATION(TransducersAreNotAutomataException);

 
## The state number argument is not valid.
# 
#     An example:
# 
# \verbatim
# tr = libhfst.HfstBasicTransducer()
# tr.add_state(1)
# # An exception is thrown as there is no state number 2
# w = tr.get_final_weight(2)
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(StateIndexOutOfBoundsException);
 
 
## Transducer has a malformed HFST header. 
# 
#     Thrown by hfst::HfstTransducer(HfstInputStream&)
#     hfst::HfstInputStream()
#     hfst::HfstInputStream(const std::string&)
# HFST_EXCEPTION_CHILD_DECLARATION(TransducerHeaderException);
 
 
## An OpenFst transducer does not have an input symbol table. 
# 
#     When converting from OpenFst to tropical or log HFST, the OpenFst transducer
#     must have at least an input symbol table. If the output symbol table
#     is missing, it is assumed to be equivalent to the input symbol table.
# 
#     Thrown by hfst::HfstTransducer(HfstInputStream&)
# HFST_EXCEPTION_CHILD_DECLARATION(MissingOpenFstInputSymbolTableException);
 
 
## Two or more transducers do not have the same type.
# 
#     This can happen if (1) the calling and called transducer in a binary
#     operation, (2) two transducers in a pair of transducers,
#     (3) two consecutive transducers coming from an HfstInputStream or
#     (4) two transducers in a function taking two or more transducers as
#     arguments do not have the same type.
# 
#     Thrown e.g. by
#     HfstTransducer::disjunct(const HfstTransducer&)
#     rules::two_level_if
#     HfstTransducer(HfstInputStream&)
# 
# An example:
# \verbatim
# foo = libhfst.HfstTransducer("foo", libhfst.SFST_TYPE)
# bar = libhfst.HfstTransducer("bar", libhfst.FOMA_TYPE)
# foo.disjunct(bar)   # an exception is thrown 
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(TransducerTypeMismatchException);
 
 
## TODO: The set of transducer pairs is empty. 
# 
#     Thrown by rule functions. An example:
# 
# \verbatim
#     contexts = ,  # contexts is empty
#     restr = libhfst.restriction(contexts, mapping, alphabet, twol_type, direction); 
# \endverbatim
# HFST_EXCEPTION_CHILD_DECLARATION(EmptySetOfContextsException);


## The type of a transducer is not specified.
# 
#    This exception is thrown when an implementation type argument
#    is ERROR_TYPE.
# HFST_EXCEPTION_CHILD_DECLARATION(SpecifiedTypeRequiredException);

 
## An error happened probably due to a bug in the HFST code. */
# HFST_EXCEPTION_CHILD_DECLARATION(HfstFatalException);
 
 
## Transducer has wrong type. 
# 
#     This exception suggests that an HfstTransducer has not been properly
#     initialized, probably due to a bug in the HFST library. Alternatively
#     the default constructor of HfstTransducer has been called at some point. 
# 
#     @see hfst::HfstTransducer()
# HFST_EXCEPTION_CHILD_DECLARATION(TransducerHasWrongTypeException);

 
## String is not valid utf-8. 
# 
#     This exception suggests that an input string is not valid utf8.
# 
# HFST_EXCEPTION_CHILD_DECLARATION(IncorrectUtf8CodingException);

 
# HFST_EXCEPTION_CHILD_DECLARATION(EmptyStringException);

 
# HFST_EXCEPTION_CHILD_DECLARATION(SymbolNotFoundException);


# HFST_EXCEPTION_CHILD_DECLARATION(MetadataException);
