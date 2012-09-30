import libhfst
import sys

ttype = 0
if sys.argv[1] == 'sfst':
    ttype = libhfst.sfst_type()
elif sys.argv[1] == 'openfst':
    ttype = libhfst.tropical_openfst_type()
elif sys.argv[1] == 'foma':
    ttype = libhfst.foma_type()
else:
    print "ERROR: could not parse transducer format argument."
    sys.exit(1)
    
transducers_in_stream = int(sys.argv[2])

istr = libhfst.HfstInputStream()
ostr = libhfst.HfstOutputStream(ttype)
transducers_read = 0
transducers_written = 0

while True:
    try:
        tr = libhfst.HfstTransducer(istr)
        transducers_read += 1
        ostr.redirect(tr)
        transducers_written += 1
    except libhfst.EndOfStreamException:
        break;
if transducers_read != transducers_in_stream:
    print "ERROR: wrong numer of transducers read"
    sys.exit(1)

istr.close()
ostr.close()
