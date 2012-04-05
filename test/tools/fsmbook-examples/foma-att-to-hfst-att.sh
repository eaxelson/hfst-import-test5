#!/bin/bash
# Convert the att format from foma into the equivalent format of HFST.
perl -pe "s/\*\*EPSILON\*\*/\@_EPSILON_SYMBOL_\@/g;"
perl -pe "s/\\\\_/\@_SPACE_\@/g;"  | # space
perl -pe "s/ /\@_SPACE_\@/g;"  # space
