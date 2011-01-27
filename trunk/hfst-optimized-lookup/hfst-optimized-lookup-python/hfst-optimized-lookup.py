#!/usr/bin/python

import sys
import os
from shared import Header, Alphabet, LetterTrie
from transducer import Transducer
from transducer import TransducerW

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: python HfstRuntimeReader FILE"
        sys.exit()
    transducerfile = open(sys.argv[1], "rb")
    header = Header(transducerfile)
    print "header read"
    alphabet = Alphabet(transducerfile, header.number_of_symbols)
    print "alphabet read"
    if header.weighted:
        transducer = TransducerW(transducerfile, header, alphabet)
    else:
        transducer = Transducer(transducerfile, header, alphabet)
    print "transducer ready"
    print

    while True:
        try:
            str = raw_input()
        except EOFError:
            sys.exit(0)
        print str + ":"
        if transducer.analyze(str):
            transducer.printAnalyses()
            print
        else:
            # tokenization failed
            pass
