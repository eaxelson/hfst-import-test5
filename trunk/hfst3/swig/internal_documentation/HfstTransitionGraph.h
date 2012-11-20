## Disjunct this transducer with a one-path transducer defined by string pair tuple \a spv that has weight \a weight. 
#
# @pre This graph must be a trie where all weights are in final states, 
#      i.e. all transitions have a zero weight. 
#  
# There is no way to test whether a graph is a trie, so the use
# of this function is probably limited to fast construction of a lexicon. 
# Here is an example: 
#  
# \verbatim
# lexicon = libhfst.HfstBasicTransducer()
# TOK = libhfst.HfstTokenizer
# lexicon.disjunct(TOK.tokenize('dog'), 0.3)
# lexicon.disjunct(TOK.tokenize('cat'), 0.5)
# lexicon.disjunct(TOK.tokenize('elephant'), 1.6)
# \endverbatim

