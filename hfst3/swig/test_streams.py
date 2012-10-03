import libhfst
import sys

for ttype in (libhfst.sfst_type(), libhfst.tropical_openfst_type(), libhfst.foma_type()):

    tr1 = libhfst.HfstTransducer('a', 'b', ttype)
    tr2 = libhfst.HfstTransducer('c', 'd', ttype)
    ostr = libhfst.HfstOutputStream('foo.hfst', tr1.get_type())
    ostr.redirect(tr1)
    ostr.redirect(tr2)
    ostr.close()
    att_file = libhfst.hfst_open('foo.att', 'w')

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
            tr.write_in_att_format(att_file) # FAILS on Windows
            if transducers_read < 2:
                att_file.write('--\n')
        except libhfst.EndOfStreamException:
            break
    if transducers_read != 2:
        print "ERROR: wrong number of transducers read"
        sys.exit(1)
    att_file.close()

    att_file = libhfst.hfst_open('foo.att', 'r')
    transducers_read = 0
    while True:
        try:
            tr = libhfst.HfstTransducer(att_file, ttype, '@0@')
            transducers_read += 1
            if transducers_read == 1:
                if not tr.compare(tr1):
                    print "ERROR: transducer 1 changed in ATT format."
                    sys.exit(1)
            if transducers_read == 2:
                if not tr.compare(tr2):
                    print "ERROR: transducer 2 changed in ATT format."
                    sys.exit(1)
        except libhfst.EndOfStreamException:
            break
    if transducers_read != 2:
        print "ERROR: wrong number of ATT transducers read"
        sys.exit(1)
    att_file.close()
