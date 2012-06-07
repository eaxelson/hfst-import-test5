#! /bin/bash

BINDIR/hfst-open-input-file-for-tagger $@ |
BINDIR/hfst-tagger-hfst_tagger_compute_data_statistics.py |
BINDIR/hfst-build-tagger $@
