#! /usr/bin/python

# Program for computing the penalties needed for the lexical model of
# an HFST tagger.

import sys
import tagger_aux

# The maximal length of suffixes used in the guesser.
MAX_SUFFIX_LENGTH = 10

# Read from stdin.
file_contents = sys.stdin.read()

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

# The entire sequence of tags and word forms in the training data.
sequence = []

## CONSTRUCTY LEXICAL TABLE.

# Loop through the training data file on line at a time. For each line
# generate all suffixes of the word form and increase suffix and tag
# count, suffix count, tag count and line number.
#
# For each input line, check that the line consists of two fields
# separated by a tab. For each input word form and tag, check that
# they are wellformed utf-8.
for line in file_contents.split("\n"):

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
    for i in range(0,min(len(rev_word_form),MAX_SUFFIX_LENGTH) + 1):

        suffix = rev_word_form[:i]

        suffix_and_tag_count_map[suffix][tag] += 1.0
        suffix_count_map[suffix]              += 1.0
        tag_count_map[tag]                    += 1.0
        number_of_suffixes                    += 1.0

# Compute and display the penalties for word form and tag
# combinations.
print "START P(WORD_FORM | TAG)"
tagger_aux.print_conditional_penalties(word_form_and_tag_map,entry_tag_map)
print "STOP P(WORD_FORM | TAG)"

# Compute and display the penalties for suffix and tag combinations.
print "START P(SUFFIX_AND_TAG | SUFFIX)"
tagger_aux.print_conditional_penalties(suffix_and_tag_count_map, tag_count_map)
print "STOP P(SUFFIX_AND_TAG | SUFFIX)"

# Compute and display the penalties for suffixes.
print "START P(SUFFIX)"
tagger_aux.print_penalties(suffix_count_map, number_of_suffixes)
print "STOP P(SUFFIX)"

# Compute and display the penalties for tags.
print "START P(TAG)"
number_of_tags = number_of_suffixes
tagger_aux.print_penalties(tag_count_map, number_of_suffixes)
print "STOP P(TAG)"

## CONSTRUCT TAG SEQUENCE TABLE.

# Typical HMM simplifiers.
trigram_enumerator = [[0,1]] * 3
trigram_denominator = [[0,1]] * 2 + [[0,0]]
trigram_enumerator_simplifier = tagger_aux.SequenceSimplifier\
    (trigram_enumerator)
trigram_denominator_simplifier = tagger_aux.SequenceSimplifier\
    (trigram_denominator)
trigram_enumerator_counter = tagger_aux.get_pair_counter()
trigram_denominator_counter = tagger_aux.get_object_counter()

bigram_enumerator = [[0,1]] * 2
bigram_denominator = [[0,1]] + [[0,0]]
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

print "START P(T_1, T_2, T_3 | T_1, T_2)"
tagger_aux.print_conditional_penalties(trigram_enumerator_counter,
                                       trigram_denominator_counter)
print "STOP P(T_1, T_2, T_3 | T_1, T_2)"

# Compute and display the penalties for suffix and tag combinations.
print "START P(T_1, T_2 | T_1)"
tagger_aux.print_conditional_penalties(bigram_enumerator_counter,
                                       bigram_denominator_counter)
print "STOP P(T_1, T_2 | T_1)"

# Compute and display the penalties for suffixes.
print "START P(T_1)"
tagger_aux.print_conditional_penalties(unigram_enumerator_counter,
                                       unigram_denominator_counter)
print "STOP P(T_1)"


