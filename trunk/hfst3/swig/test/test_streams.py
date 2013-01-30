import libhfst
import sys
import os

def remove_generated_files():
    # fails on MinGW..
    #os.remove('foo.att')
    #os.remove('foo.hfst')
    pass

for ttype in (libhfst.SFST_TYPE, libhfst.TROPICAL_OPENFST_TYPE, libhfst.FOMA_TYPE):

    tr1 = libhfst.HfstTransducer('a', 'b', ttype)
    tr2 = libhfst.HfstTransducer('c', 'd', ttype)
    ostr = libhfst.HfstOutputStream('foo.hfst', tr1.get_type())
    ostr.redirect(tr1)
    ostr.redirect(tr2)
    ostr.close()
    att_file = libhfst.hfst_open('foo.att', 'w')

    print "#1"

    istr = libhfst.HfstInputStream('foo.hfst')
    
    print "#2"

    transducers_read = 0
    while True:
        try:
            print "reading.."
            tr = libhfst.HfstTransducer(istr)
            print "..read"
            transducers_read += 1
            if transducers_read == 1:
                if not tr.compare(tr1):
                    print("ERROR: transducer 1 changed.")
                    remove_generated_files()
                    sys.exit(1)
            if transducers_read == 2:
                if not tr.compare(tr2):
                    print("ERROR: transducer 2 changed.")
                    remove_generated_files()
                    sys.exit(1)
            print "writing.."
            tr.write_in_att_format(att_file)
            print "..wrote"
            if transducers_read < 2:
                att_file.write('--\n')
        except: # libhfst.EndOfStreamException:
            assert(libhfst.hfst_get_exception() == "EndOfStreamException")
            break
    if transducers_read != 2:
        print("ERROR: wrong number of transducers read")
        remove_generated_files()
        sys.exit(1)
    att_file.close()

    print "HERE"

    att_file = libhfst.hfst_open('foo.att', 'r')
    transducers_read = 0
    while True:
        try:
            tr = libhfst.HfstTransducer(att_file, ttype, '@0@')
            transducers_read += 1
            if transducers_read == 1:
                if not tr.compare(tr1):
                    print("ERROR: transducer 1 changed in ATT format.")
                    remove_generated_files()
                    sys.exit(1)
            if transducers_read == 2:
                if not tr.compare(tr2):
                    print("ERROR: transducer 2 changed in ATT format.")
                    remove_generated_files()
                    sys.exit(1)
        except: # libhfst.EndOfStreamException:
            break
    if transducers_read != 2:
        print("ERROR: wrong number of ATT transducers read")
        remove_generated_files()
        sys.exit(1)
    att_file.close()

remove_generated_files()
