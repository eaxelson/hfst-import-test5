 # @brief (Not implemented) Lookdown a single string \a s and return 
# a maximum of \a limit results.
#
# Traverse all paths on logical second level of the transducer to produce
# all possible inputs on the first.
# This is in effect a fast composition of single
# path from left hand side.
#
# @param s  string to look down
# <!-- @param tok  tokenizer to split string in arcs? -->
# @param limit  number of strings to extract. -1 tries to extract all and
#             may get stuck if infinitely ambiguous
# @return  output parameter to store unique results
# @todo todo
# HfstOneLevelPaths * lookdown(const StringVector& s,
# ssize_t limit = -1) const;
# 
# @brief (Not implemented) Lookdown a single string minding 
# flag diacritics properly.
# 
# This is a version of lookdown that handles flag diacritics as epsilons
# and validates the sequences prior to outputting.
#
# @sa lookdown
# @todo todo
# HfstOneLevelPaths * lookdown_fd(StringVector& s,
# ssize_t limit = -1) const;
# 

# 
# @brief (Not implemented) Whether lookdown of path \a s will have
# infinite results.
# @todo todo
# bool is_lookdown_infinitely_ambiguous(const StringVector& s) const;


## TODO
## Create universal pair transducer of \a type.
# *
# *  The transducer has only one state, and it accepts:
# *  Identity:Identity, Unknown:Unknown, Unknown:Epsilon and Epsilon:Unknown
# *
# *  Transducer weight is 0.
# 
# static HfstTransducer universal_pair ( ImplementationType type )
# 
## Create identity pair transducer of \a type.
# *
# * The transducer has only one state, and it accepts:
# * Identity:Identity
# *
# * Transducer weight is 0.
# 
# static HfstTransducer identity_pair ( ImplementationType type )

 
# // *** For commandline programs. ***
# 
# /* For each flag diacritic fd that is included in the alphabet of
# transducer \a another but not in the alphabet of this transducer,
# insert freely a transition fd:fd in this transducer. 
# void insert_freely_missing_flags_from
# (const HfstTransducer &another)
# 
# /*
# If both \a this and \a another contain flag diacritics, replace flag
# diacritic @X.Y.(.Z)@ by @X.Y_1(.Z)@ in \a this and replace it by
# @X.Y_2(.Z)@ in \a another. 
# 
# If \a insert_renamed_flags is true, then the flags from \a this are 
# inserted freely in \a another and vice versa after replacing.
# 
# void harmonize_flag_diacritics(HfstTransducer &another,
# bool insert_renamed_flags=true)
#
# /* Whether the alphabet of transducer \a another includes flag diacritics
# that are not included in the alphabet of this transducer. 
# bool check_for_missing_flags_in(const HfstTransducer &another) const;
# 
# /* Return true if \a this has flag diacritics in the alphabet. 
# bool has_flag_diacritics(void) const; 
  



 
 
 
# // ***** THE PUBLIC INTERFACE *****
# 
## A transducer that obligatorily performs the mappings 
# defined by \a mappings in the context \a context
# when the alphabet is \a alphabet. 
# 
# @param context A pair of transducers where the first transducer
# defines the left context and the second transducer
# the right context.
# @param mappings A set of mappings that the resulting transducer
# will perform in the context given in \a context.
# @param alphabet The set of symbol pairs that defines the alphabet
# (see the example).
# 
# For example, a transducer yielded by the following arguments
# \verbatim
# context = pair( [c|d], [e] )
# mappings = set(a:b)
# alphabet = set(a, a:b, b, c, d, e, ...)
# \endverbatim
# obligatorily maps the symbol a to b if c or d precedes 
# and e follows. (Elsewhere,
# the mapping of a to b is optional)
# This expression is identical to ![.* [c|d] [a:. & !a:b] [e] .*]
# Note that the alphabet must contain the pair a:b here.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>
# 
# HfstTransducer two_level_if(HfstTransducerPair &context, 
# StringPairSet &mappings, 
# StringPairSet &alphabet)
# 
## A transducer that allows the mappings defined by 
# \a mappings only in the context \a context,
# when the alphabet is \a alphabet. 
# 
# If called with the same arguments as in the example of 
# #two_level_if, the transducer
# allows the mapping of symbol a to b only if c or d precedes 
# and e follows. The
# mapping of a to b is optional in this context but cannot occur 
# in any other context.
# The expression is equivalent to 
# ![  [ ![.* [c|d]] a:b .* ] | [ .* a:b ![[e] .*] ]  ]
# 
# @see #two_level_if
# 
# HfstTransducer two_level_only_if(HfstTransducerPair &context, 
# StringPairSet &mappings, 
# StringPairSet &alphabet)
# 
## A transducer that always performs the mappings defined 
# by \a mappings in the context \a context
# and only in that context, when the alphabet is \a alphabet. 
# 
# If called with the same arguments as in the example of 
# #two_level_if, the transducer
# maps symbol a to b only and only if c or d precedes and e follows.
# The mapping of a to b is obligatory in this context and cannot 
# occur in any other context.
# The expression is equivalent to ![.* [c|d] [a:. & !a:b] [e] .*]  &
# ![  [ ![.* [c|d]] a:b .* ] | [ .* a:b ![[e] .*] ]  ]
# 
# @see #two_level_if
# 
# HfstTransducer two_level_if_and_only_if(HfstTransducerPair &context, 
# StringPairSet &mappings, 
# StringPairSet &alphabet)
# 
# 
## A transducer that performs an upward mapping \a mapping 
# in the context \a context when the alphabet is \a alphabet.
# \a optional defines whether the mapping is optional. 
# 
# @param context A pair of transducers where the first transducer
# defines the left context and the second transducer
# the right context. Both transducers must be automata,
# i.e. map strings onto themselves.
# @param mapping The mapping that the resulting transducer
# will perform in the context given in \a context.
# @param optional Whether the mapping is optional.
# @param alphabet The set of symbol pairs that defines the alphabet
# (see the explanation below).
# 
# Each substring s of the input string which is in the input language
# of the transducer \a mapping and whose left context is matched 
# by the expression
# [.* l] (where l is the first element of \a context) and 
# whose right context is matched by [r .*] 
# (where r is the second element in the context) is mapped 
# to the respective
# surface strings defined by transducer \a mapping. Any other 
# character is mapped to
# the characters specified in \a alphabet. The left and right 
# contexts must
# be automata (i.e. transducers which map strings onto themselves).
# 
# For example, a transducer yielded by the following arguments
# \verbatim
# context = pair( [c], [c] )
# mappings = [ a:b a:b ]
# alphabet = set(a, b, c)
# \endverbatim
# would map the string "caacac" to "cbbcac".
# 
# Note that the alphabet must contain the characters a and b, 
# but not the pair
# a:b (unless this replacement is to be allowed everywhere 
# in the context).
# 
# Note that replace operations (unlike the two-level rules) 
# have to be combined by composition
# rather than intersection.
# 
# @throws ContextTransducersAreNotAutomataException
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>
# 
# HfstTransducer replace_up(HfstTransducerPair &context, 
# HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
## The same as replace_up, but matching is done on 
# the output side of \a mapping 
# 
# @see replace_up 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer replace_down(HfstTransducerPair &context, 
# HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
# HfstTransducer replace_down_karttunen(HfstTransducerPair &context, 
# HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
## The same as replace_up, but left context matching is 
# done on the output side of \a mapping
# and right context on the input side of \a mapping 
# 
# @see replace_up 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer replace_right(HfstTransducerPair &context, 
# HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
## The same as replace_up, but left context matching is 
# done on the input side of \a mapping
# and right context on the output side of \a mapping. 
# 
# @see replace_up 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer replace_left(HfstTransducerPair &context, 
# HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
# 
## The same as replace_up but \a mapping is performed 
# in every context. 
# 
# @see replace_up 
# HfstTransducer replace_up(HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
## The same as replace_down(HfstTransducerPair&, HfstTransducer&, bool, StringPairSet&)
# but \a mapping is performed in every context.
# 
# @see replace_up 
# HfstTransducer replace_down(HfstTransducer &mapping, 
# bool optional, 
# StringPairSet &alphabet)
# 
## Inversion of the replace_up and the result needs to be composed on the upper side
# *   of the input language.
# *
# *   B <- A is the inversion of A -> B.
# *
# *   \a Mapping is performed in every context.
# *
# *   @see replace_up 
# HfstTransducer left_replace_up(     HfstTransducer          &mapping,
# bool                optional,
# StringPairSet       &alphabet)
# 
## Inversion of the replace_up and the result needs to be composed on the upper side
# *   of the input language.
# *
# *   B <- A is the inversion of A -> B.
# *
# *   @see replace_up 
# HfstTransducer left_replace_up( HfstTransducerPair  &context,
# HfstTransducer      &mapping,
# bool                optional,
# StringPairSet       &alphabet)
## Inversion of the replace_up and the result needs to be composed on the upper side
# *   of the input language. However, matching is done on the output side of \a mapping
# *
# *   @see replace_up 
# HfstTransducer left_replace_down(HfstTransducerPair &context,
# HfstTransducer         &mapping,
# bool                           optional,
# StringPairSet          &alphabet)
# 
## Inversion of the replace_up and the result needs to be composed on the upper side
# *         of the input language. However, matching is done on the output side of \a mapping
# *
# *         @see replace_up 
# HfstTransducer left_replace_down_karttunen( HfstTransducerPair      &context,
# HfstTransducer          &mapping,
# bool                            optional,
# StringPairSet           &alphabet)
# 
## Inversion of the replace_up and the result needs to be composed on the upper side
# *   of the input language. However, left context matching is done on the input side of \a mapping
# *   and right context on the output side of \a mapping.
# *
# *   @see replace_up 
# HfstTransducer left_replace_left(HfstTransducerPair &context,
# HfstTransducer         &mapping,
# bool                           optional,
# StringPairSet          &alphabet)
# 
## Inversion of the replace_up and the result needs to be composed on the upper side
# *   of the input language. However, left context matching is done on the output side of \a mapping
# *   and right context on the input side of \a mapping.
# *
# *   @see replace_up 
# HfstTransducer left_replace_right(HfstTransducerPair        &context,
# HfstTransducer                &mapping,
# bool                          optional,
# StringPairSet         &alphabet)
# 
# 
# 
# 
# 
## A transducer that allows any (substring) mapping defined 
# by \a mapping
# only if it occurs in any of the contexts in \a contexts. 
# Symbols outside of the matching
# substrings are mapped to any symbol allowed by \a alphabet. 
# 
# @throws EmptySetOfContextsException
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer restriction(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that requires that one of the mappings 
# defined by \a mapping
# must occur in each context in \a contexts. Symbols outside of 
# the matching
# substrings are mapped to any symbol allowed by \a alphabet.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer coercion(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that is equivalent to the intersection 
# of restriction and coercion
# and requires that the mappings defined by \a mapping occur 
# always and only in the
# given contexts in \a contexts. Symbols outside of the matching
# substrings are mapped to any symbol allowed by \a alphabet.
# 
# @see
# restriction(HfstTransducerPairVector&, HfstTransducer&, StringPairSet&) 
# #coercion 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a> 
# HfstTransducer restriction_and_coercion(HfstTransducerPairVector &contexts,
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that specifies that a string from 
# the input language of the
# transducer \a mapping may only be mapped to one of its 
# output strings (according
# to transducer \a mapping) if it appears in any of the contexts 
# in \a contexts.
# Symbols outside of the matching substrings are mapped
# to any symbol allowed by \a alphabet.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer surface_restriction(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that specifies that a string from 
# the input language of the transducer
# \a mapping always has to the mapped to one of its output 
# strings according to
# transducer \a mapping if it appears in any of the contexts 
# in \a contexts.
# Symbols outside of the matching substrings are mapped to 
# any symbol allowed by \a alphabet.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer surface_coercion(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that is equivalent to the intersection of 
# surface_restriction and surface_coercion.
# 
# @see #surface_restriction #surface_coercion 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer surface_restriction_and_coercion
# (HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that specifies that a string from 
# the output language of the transducer
# \a mapping may only be mapped to one of its input strings 
# (according to transducer \a mappings)
# if it appears in any of the contexts in \a contexts.
# Symbols outside of the matching substrings are mapped 
# to any symbol allowed by \a alphabet.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer deep_restriction(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that specifies that a string from 
# the output language of the transducer
# \a mapping always has to be mapped to one of its input strings 
# (according to transducer \a mappings)
# if it appears in any of the contexts in \a contexts.
# Symbols outside of the matching substrings are mapped 
# to any symbol allowed by \a alphabet.
# 
# @see
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer deep_coercion(HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)
# 
## A transducer that is equivalent to the intersection 
# of deep_restriction and deep_coercion.
# 
# @see #deep_restriction #deep_coercion 
# <a href="ftp://ftp.ims.uni-stuttgart.de/pub/corpora/SFST/SFST-Manual.pdf">
# SFST manual</a>. 
# HfstTransducer deep_restriction_and_coercion
# (HfstTransducerPairVector &contexts, 
# HfstTransducer &mapping, 
# StringPairSet &alphabet)

