import libhfst
import os

if os.path.exists('foofile'):
    os.remove('foofile')

types = [libhfst.SFST_TYPE, libhfst.TROPICAL_OPENFST_TYPE, libhfst.FOMA_TYPE]


# The library required by the implementation type requested is not linked to HFST.
# --------------------------------------------------------------------------------
print "ImplementationTypeNotAvailableException"

for type in types:
    try:
        tr = libhfst.HfstTransducer("foo", "bar", type)
    except libhfst.ImplementationTypeNotAvailableException:
        pass


# Stream cannot be read. 
# ----------------------
#print "StreamNotReadableException"
#
#try:
#    instr = libhfst.HfstInputStream("foofile")
#except libhfst.StreamNotReadableException:
#    print "ERROR: file cannot be read."


# Stream cannot be written. 
# -------------------------
#print "StreamCannotBeWrittenException"
#
#for type in types:
#    try:
#        tr = libhfst.HfstTransducer("foo", type)
#        out = libhfst.HfstOutputStream("testfile", type)
#        out.redirect(tr)
#    except libhfst.StreamCannotBeWrittenException:
#        print "ERROR: file cannot be written."


# Stream is closed.
# -----------------
print "StreamIsClosedException"

for type in types:
    try:
        tr = libhfst.HfstTransducer("foo", type)
        out = libhfst.HfstOutputStream("testfile", type)
        out.close()
        out.redirect(tr)
        assert(False)
    except libhfst.StreamIsClosedException:
        pass


# Transducer is cyclic. 
# ---------------------
print "TransducerIsCyclicException" 

for type in types:
    transducer = libhfst.HfstTransducer("a", "b", type)
    transducer.repeat_star()
    try:
        results = libhfst.detokenize_paths(libhfst.extract_paths(transducer))
        print "The transducer has %i paths" % len(results)
        assert(False)
    except libhfst.TransducerIsCyclicException:
        print "The transducer is cyclic and has an infinite number of paths."


# The stream does not contain transducers. 
# ----------------------------------------
print "NotTransducerStreamException"

foofile = open('foofile', 'wb')
foofile.write('This is a text file.')
foofile.write('Here is another line.')
foofile.write('The file ends here.')
foofile.close()
try:
    instr = libhfst.HfstInputStream("foofile")
except libhfst.NotTransducerStreamException:
    print "file does not contain transducers."


# The stream is not in valid AT&T format. 
# ---------------------------------------
print "NotValidAttFormatException"

testfile_att = open("testfile.att", "wb")
testfile_att.write("0 1 a b")
testfile_att.write("1")
testfile_att.write("c")
testfile_att.close()

for type in types:
    transducers = []
    ifile = open("testfile.att", "rb")
    try:
            t = libhfst.HfstTransducer(ifile, type, "epsilon")
            transducers.append(t)
            print "read one transducer"
    except libhfst.NotValidAttFormatException:
        print "Error reading transducer: not valid AT&T format."
        ifile.close()
        print "Read %i transducers in total." % len(transducers)


# State is not final (and cannot have a final weight). 
# ----------------------------------------------------
print "StateIsNotFinalException"

tr = libhfst.HfstBasicTransducer()
tr.add_state(1)
# An exception is thrown as state number 1 is not final
try:
    w = tr.get_final_weight(1)
    assert(False)
except libhfst.StateIsNotFinalException:
    pass


# Context transducers are not automata.
# -------------------------------------
print "ContextTransducersAreNotAutomataException"

for type in types:
    # The second context transducer is 
    contexts = (libhfst.HfstTransducer("c", type), libhfst.HfstTransducer("c", "d", type))
    mapping = libhfst.HfstTransducer("a", "b", type)
    alphabet = (('a','a'), ('b','b'), ('c','c'), ('d','d'))
    # An exception is thrown
    # rule = libhfst.replace_up(contexts, mapping, True, alphabet)


# The state number argument is not valid.
# ---------------------------------------
print "NotValidStateNumberException"

# tr = libhfst.HfstBasicTransducer()
# tr.add_state(1)
# An exception is thrown as there is no state number 2
# w = tr.get_final_weight(2)


# Two or more transducers do not have the same type.
# --------------------------------------------------
print "TransducerTypeMismatchException"

foo = libhfst.HfstTransducer("foo", libhfst.SFST_TYPE)
bar = libhfst.HfstTransducer("bar", libhfst.FOMA_TYPE)
try:
    foo.disjunct(bar)
    assert(False)
except libhfst.TransducerTypeMismatchException:
    pass


# TODO: The set of transducer pairs is empty. 
# -------------------------------------------
#print "EmptyFooFoo"
#
#contexts = ()  # contexts is empty
#restr = libhfst.restriction(contexts, mapping, alphabet, direction)
