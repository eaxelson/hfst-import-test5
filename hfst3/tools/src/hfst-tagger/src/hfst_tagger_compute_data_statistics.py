#! /usr/bin/python

# @file hfst_tagger_compute_data_statistics.py
#
# @author Miikka Silfverberg
#
# @brief Program for computing the penalties needed for the lexical
# model of an HFST tagger.

#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, version 3 of the Licence.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.



import sys
import tagger_aux
import re
import string

# The maximal length of suffixes used in the guesser.
MAX_SUFFIX_LENGTH = 10

# If verbose is true, the program prints info about what is happening.
verbose=False

# Commandline arguments.
arg_string = " " + string.join(sys.argv, " ") + " "

# FIXME! use python argument parser.
if re.search("-v[^\w]",arg_string) != None or \
   re.search("--verbose[^\w]",arg_string) != None:
    verbose = True

# Read from stdin.
tagger_aux.verbose_print("Reading input file.",verbose)

# Maps (word_form,tag) to its count in training data.
word_form_and_tag_map = tagger_aux.get_pair_counter()

# Maps tag to its count in training data.
entry_tag_map = tagger_aux.get_object_counter()

# Maps (reversed_suffix,tag) to its count in training data.
suffix_and_tag_count_map = tagger_aux.get_pair_counter()

# Maps reversed_suffix to its count in training data.
suffix_count_map = tagger_aux.get_object_counter()

# Maps reversed_suffix to its count in training data. Each tag is
# counted as many times as the corresponding word has suffixes of
# length < MAX_SUFFIX_LENGTH.
tag_count_map = tagger_aux.get_object_counter()

# The total number of suffix occurrences in the training data.
number_of_suffixes = 0.0

# We loop through the file. line_number is the current line number in
# the file.
line_number = 0

# The entire sequence of word forms and tags in the training data.
sequence = []

## CONSTRUCT LEXICAL TABLE.

# Loop through the training data file on line at a time. For each line
# generate all suffixes of the word form and increase suffix and tag
# count, suffix count, tag count and line number.
#
# For each input line, check that the line consists of two fields
# separated by a tab. For each input word form and tag, check that
# they are wellformed utf-8.
tagger_aux.verbose_print("Computing lexical statistics.",verbose)

for line in sys.stdin:
    # Get rid of trailing ws.
    line = line.strip()

    # Increment line number counter.
    line_number += 1

    # Skip empty lines.
    if line == "":
        continue

    # check that the line has two fields and that it is valid utf-8.
    try:
        tagger_aux.check_line(line,2)
    except Exception as e:
        print e.message + " " + str(line_number) + ":"
        print line
        exit(1)

    # Split the line into fields.
    split_line = line.split("\t")

    word_form = split_line[0]
    tag       = split_line[1]
    
    sequence.append(split_line)

    # Reverse the word form in order to construct the reversed
    # suffixes of the word.
    rev_word_form = tagger_aux.reverse(word_form)
    
    # Increment counts of word form and tag occurrences for whole
    # words.
    word_form_and_tag_map[rev_word_form][tag] += 1.0
    entry_tag_map[tag] += 1.0

    # Increment suffix and tag count, suffix count and tag
    # count. Consider only suffixes that are maximally as long as
    # MAX_SUFFIX_LENGTH.
    word_maximal_suffix_length = min(len(rev_word_form),MAX_SUFFIX_LENGTH)

    for i in range(0,word_maximal_suffix_length + 1):

        suffix = rev_word_form[:i]

        suffix_and_tag_count_map[tag][suffix] += 1.0
        suffix_count_map[suffix]              += 1.0
        tag_count_map[tag]                    += 1.0
        number_of_suffixes                    += 1.0

# Compute and display the penalties for word form and tag
# combinations.
tagger_aux.verbose_print("Storing lexical statistics.",verbose)
tagger_aux.verbose_print("P(WORD_FORM | TAG)",verbose)

print "START P(WORD_FORM | TAG)"
tagger_aux.print_conditional_penalties(word_form_and_tag_map,entry_tag_map)
print "STOP P(WORD_FORM | TAG)"

# Compute and display the penalties for suffix and tag combinations.
tagger_aux.verbose_print("P(SUFFIX_AND_TAG | SUFFIX)",verbose)

print "START P(SUFFIX_AND_TAG | SUFFIX)"
tagger_aux.print_conditional_penalties(suffix_and_tag_count_map, 
                                       suffix_count_map,True)
print "STOP P(SUFFIX_AND_TAG | SUFFIX)"

# Compute and display the penalties for suffixes.
tagger_aux.verbose_print("P(SUFFIX)",verbose)

print "START P(SUFFIX)"
tagger_aux.print_penalties(suffix_count_map, number_of_suffixes)
print "STOP P(SUFFIX)"

# Compute and display the penalties for tags.
tagger_aux.verbose_print("P(TAG)",verbose)
print "START P(TAG)"
number_of_tags = number_of_suffixes
tagger_aux.print_penalties(tag_count_map, number_of_suffixes)
print "STOP P(TAG)"

## CONSTRUCT TAG SEQUENCE TABLE.

# Typical HMM simplifiers.
trigram_enumerator = [[0,1], [0,1], [0,1]]
trigram_denominator = [[0,1], [0,1], [0,0]]
trigram_enumerator_simplifier = tagger_aux.SequenceSimplifier\
    (trigram_enumerator)
trigram_denominator_simplifier = tagger_aux.SequenceSimplifier\
    (trigram_denominator)
trigram_enumerator_counter = tagger_aux.get_pair_counter()
trigram_denominator_counter = tagger_aux.get_object_counter()

bigram_enumerator = [[0,1], [0,1]]
bigram_denominator = [[0,1], [0,0]]
bigram_enumerator_simplifier = tagger_aux.SequenceSimplifier(bigram_enumerator)
bigram_denominator_simplifier = tagger_aux.SequenceSimplifier\
    (bigram_denominator)
bigram_enumerator_counter = tagger_aux.get_pair_counter()
bigram_denominator_counter = tagger_aux.get_object_counter()

unigram_enumerator = [[0,1]]
unigram_denominator = [[0,0]]
unigram_enumerator_simplifier = tagger_aux.SequenceSimplifier\
    (unigram_enumerator)
unigram_denominator_simplifier = tagger_aux.SequenceSimplifier\
    (unigram_denominator)
unigram_enumerator_counter = tagger_aux.get_pair_counter()
unigram_denominator_counter = tagger_aux.get_object_counter()

tagger_aux.verbose_print("Computing sequence statistics.",verbose)
for i in range(len(sequence)):

    try:

        trigram_enumerator_counter \
            [trigram_enumerator_simplifier.simplify(i,sequence)] \
            [trigram_denominator_simplifier.simplify(i,sequence)] += 1.0

        trigram_denominator_counter \
            [trigram_denominator_simplifier.simplify(i,sequence)] += 1.0

        bigram_enumerator_counter \
            [bigram_enumerator_simplifier.simplify(i,sequence)] \
            [bigram_denominator_simplifier.simplify(i,sequence)] += 1.0

        bigram_denominator_counter \
            [bigram_denominator_simplifier.simplify(i,sequence)] += 1.0

        unigram_enumerator_counter \
            [unigram_enumerator_simplifier.simplify(i,sequence)] \
            [unigram_denominator_simplifier.simplify(i,sequence)] += 1.0

        unigram_denominator_counter \
            [unigram_denominator_simplifier.simplify(i,sequence)] += 1.0
    
    except tagger_aux.ReachesSequenceEnd:

        break

tagger_aux.verbose_print("Storing sequence statistics.",verbose)
tagger_aux.verbose_print("P(T_1, T_2, T_3 | T_1, T_2)",verbose)
print "START P(T_1, T_2, T_3 | T_1, T_2)"
tagger_aux.print_conditional_penalties(trigram_enumerator_counter,
                                       trigram_denominator_counter)
print "STOP P(T_1, T_2, T_3 | T_1, T_2)"

# Compute and display the penalties for suffix and tag combinations.
tagger_aux.verbose_print("P(T_1, T_2 | T_1)",verbose)
print "START P(T_1, T_2 | T_1)"
tagger_aux.print_conditional_penalties(bigram_enumerator_counter,
                                       bigram_denominator_counter)
print "STOP P(T_1, T_2 | T_1)"

# Compute and display the penalties for suffixes.
tagger_aux.verbose_print("P(T_1)",verbose)
print "START P(T_1)"
tagger_aux.print_conditional_penalties(unigram_enumerator_counter,
                                       unigram_denominator_counter)
print "STOP P(T_1)"


