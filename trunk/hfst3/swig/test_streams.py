import libhfst
import sys

tr1 = libhfst.HfstTransducer('a', 'b', libhfst.tropical_openfst_type())
tr2 = libhfst.HfstTransducer('c', 'd', libhfst.tropical_openfst_type())
ostr = libhfst.HfstOutputStream('foo.hfst', libhfst.tropical_openfst_type())
ostr.redirect(tr1)
ostr.redirect(tr2)
ostr.close()

istr = libhfst.HfstInputStream('foo.hfst')
transducers_read = 0
while True:
    try:
        tr = libhfst.HfstTransducer(istr)
        transducers_read += 1
        if transducers_read == 1:
            if not tr.compare(tr1):
                print "ERROR: transducer 1 changed."
                sys.exit(1)
        if transducers_read == 2:
            if not tr.compare(tr2):
                print "ERROR: transducer 2 changed."
                sys.exit(1)
    except libhfst.EndOfStreamException:
        break
if transducers_read != 2:
    print "ERROR: wrong number of transducers read."
    sys.exit(1)

