#!/bin/bash
# Convert the att format from xfst into the equivalent format of HFST.
perl -pe "s/(\s)\+(\s)/\1\@_IDENTITY_SYMBOL_\@\2/g;" |  # identity
perl -pe "s/(\s)\+$/\1\@_IDENTITY_SYMBOL_\@/g;" |   # identity
perl -pe "s/\*\*OTHER\*\*/\@_UNKNOWN_SYMBOL_\@/g;" |    # unknown
perl -pe "s/\*\*EPSILON\*\*/\@_EPSILON_SYMBOL_\@/g;"    # epsilon
