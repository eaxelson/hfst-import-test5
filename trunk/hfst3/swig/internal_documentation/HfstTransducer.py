## TODO
def extract_paths(callback, cycles=-1):
    pass
 
## TODO: Extract a maximum of \a max_num paths that are recognized by the transducer following a maximum of \a cycles cycles.
# 
# @return The extracted paths. 
# @param max_num The total number of resulting strings is capped at \a max_num, with 0 or negative indicating unlimited. 
# @param cycles Indicates how many times a cycle will be followed, with negative numbers indicating unlimited.
# 
# This is a version of extract_paths that handles flag diacritics 
# as ordinary symbols and does not validate the sequences prior to
# outputting as opposed to #extract_paths_fd(paths, int, int, bool) const.
# 
# If this function is called on a cyclic transducer with unlimited
# values for both \a max_num and \a cycles, an exception will be thrown.
# 
# This example
# 
# \verbatim
# type = libhfst.SFST_TYPE
# tr1 = libhfst.HfstTransducer("a", "b", type)
# tr1.repeat_star()
# tr2 = libhfst.HfstTransducer("c", "d", type)
# tr2.repeat_star()
# tr1.concatenate(tr2).minimize()
# HfstTwoLevelPaths results;
# results tr1.extract_paths(results, MAX_NUM, CYCLES)
# 
# # Go through all paths.
# for (HfstTwoLevelPaths::const_iterator it = results.begin()
# it != results.end() it++)
# {
# std::string istring;
# std::string ostring;
# 
# for (StringPairVector::const_iterator IT = it->second.begin()
# IT != it->second.end() IT++)
# {
# istring.append(IT->first)
# ostring.append(IT->second)
# }
# # Print input and output strings of each path
# std::cerr << istring << ":" << ostring; 
# # and optionally the weight of the path.
# #std::cerr << "\t" << it->first;
# std::cerr << std::endl; 
# }
# \endverbatim
# 
# prints with values MAX_NUM == -1 and CYCLES == 1 all paths
# that have no consecutive cycles:
# 
# \verbatim
# a : b
# ac : bd
# acc : bdd
# c : d
# cc : dd
# \endverbatim
# 
# and with values MAX_NUM == 7 and CYCLES == 2 a maximum of 7 paths
# that follow a cycle a maximum of 2 times (there are 11 such paths,
# but MAX_NUM limits their number to 7):
# 
# \verbatim
# a : b
# aa : bb
# aac : bbd
# aacc : bbdd
# c : d
# cc : dd
# ccc : ddd
# \endverbatim
# 
# @bug Does not work for HFST_OL_TYPE or HFST_OLW_TYPE?
# @throws TransducerIsCyclicException
# @see #n_best 
# @see hfst::HfstTransducer::extract_paths_fd(hfst::HfstTwoLevelPaths&, int, int, bool) const
# 
def extract_paths(max_num=-1, cycles=-1):
    pass
 
# TODO: void extract_random_paths
# (HfstTwoLevelPaths &results, int max_num) const;
# 
# void extract_random_paths_fd
# (HfstTwoLevelPaths &results, int max_num, bool filter_fd) const;
# 
# /* \brief Call \a callback with extracted strings that are not 
# invalidated by flag diacritic rules.
# 
# @see extract_paths(HfstTwoLevelPaths&, int, int) 
# void extract_paths_fd
# (ExtractStringsCb& callback, int cycles=-1, bool filter_fd=true) const;
# 
# public:
## Extract a maximum of \a max_num paths that are 
# recognized by the transducer and are not invalidated by flag 
# diacritic rules following a maximum of \a cycles cycles
# and store the paths into \a results. \a filter_fd defines whether
# the flag diacritics themselves are filtered out of the result strings.
# 
# @param results  The extracted paths are inserted here. 
# @param max_num  The total number of resulting strings is capped at 
# \a max_num, with 0 or negative indicating unlimited. 
# @param cycles  Indicates how many times a cycle will be followed, with
# negative numbers indicating unlimited.
# @param filter_fd  Whether the flag diacritics are filtered out of the
# result strings.
# 
# If this function is called on a cyclic transducer with unlimited
# values for both \a max_num and \a cycles, an exception will be thrown.
# 
# Flag diacritics are of the form @[PNDRCU][.][A-Z]+([.][A-Z]+)?@. 
# 
# For example the transducer 
# 
# \verbatim
# [[@U.FEATURE.FOO@ foo] | [@U.FEATURE.BAR@ bar]]  |  [[foo @U.FEATURE.FOO@] | [bar @U.FEATURE.BAR@]]
# \endverbatim
# 
# will yield the paths <CODE>[foo foo]</CODE> and <CODE>[bar bar]</CODE>.
# <CODE>[foo bar]</CODE> and <CODE>[bar foo]</CODE> are invalidated
# by the flag diacritics so thay will not be included in \a results.
# 
# 
# @bug Does not work for HFST_OL_TYPE or HFST_OLW_TYPE?
# @throws TransducerIsCyclicException
# @see extract_paths(HfstTwoLevelPaths&, int, int) const 
# void extract_paths_fd
# (HfstTwoLevelPaths &results, int max_num=-1, int cycles=-1, 
# bool filter_fd=true) const;
# 
# @brief Lookup or apply a single tokenized string \a s and
# return a maximum of \a limit results.
# 
# This is a version of lookup that handles flag diacritics as ordinary
# symbols and does not validate the sequences prior to outputting.
# Currently, this function calls lookup_fd.
#
# @todo Handle flag diacritics as ordinary symbols instead of calling
#       lookup_fd.
# @sa lookup_fd
# HfstOneLevelPaths * lookup(const StringVector& s,
# ssize_t limit = -1) const;
# 
# @brief Lookup or apply a single string \a s and
# return a maximum of \a limit results.
# 
# This is an overloaded lookup function that leaves tokenizing to the
# transducer.
# HfstOneLevelPaths * lookup(const std::string & s,
# ssize_t limit = -1) const;
# 
# @brief Lookup or apply a single string \a s minding flag diacritics
# properly and store a maximum of \a limit results to \a results.
#
# Traverse all paths on logical first level of the transducer to produce
# all possible outputs on the second.
# This is in effect a fast composition of single path from left
# hand side.
#
# This is a version of lookup that handles flag diacritics as epsilons
# and validates the sequences prior to outputting.
# Epsilons on the second level are represented by empty strings
# in \a results. For an example of flag diacritics, see
# #hfst::HfstTransducer::extract_paths_fd(hfst::HfstTwoLevelPaths&, int, int, bool) const
# 
#
# @pre The transducer must be of type #HFST_OL_TYPE or #HFST_OLW_TYPE.
#      This function is not implemented for other transducer types.
#
# @param s  String to look up. The weight is ignored.
# @param limit  (Currently ignored.) Number of strings to look up. 
#               -1 tries to look up all and may get stuck 
#               if infinitely ambiguous.
# \return{A pointer to a HfstOneLevelPaths container allocated by callee}
# 
# @see HfstTokenizer::tokenize_one_level
# @see is_lookup_infinitely_ambiguous(const StringVector&) const
#
# @todo Do not ignore argument \a limit.
#
# HfstOneLevelPaths * lookup_fd(const StringVector& s,
# ssize_t limit = -1) const;
 
# @brief Lookup or apply a single string \a s minding flag diacritics
# properly and store a maximum of \a limit results to \a results.
#
# This is an overloaded lookup_fd that leaves tokenizing to the
# transducer.
#
# @param s  String to look up. The weight is ignored.
# @param limit  (Currently ignored.) Number of strings to look up. 
#               -1 tries to look up all and may get stuck 
#               if infinitely ambiguous.
# \return{A pointer to a HfstOneLevelPaths container allocated by callee}
#
#
#@sa lookup_fd
# HfstOneLevelPaths * lookup_fd(const std::string& s,
# ssize_t limit = -1) const;
# 
# @brief Lookup or apply a single string \a s and store a maximum of 
# \a limit results to \a results. \a tok defined how \a s is tokenized.
#
#
# This function is the same as 
# #lookup(const StringVector&, ssize_t) const
# but lookup is not done using a string and a tokenizer instead of
# a StringVector.
# HfstOneLevelPaths * lookup(const HfstTokenizer& tok,
# const std::string &s, ssize_t limit = -1) const;
# 
# @brief Lookup or apply a single string \a s minding flag diacritics 
# properly and store a maximum of \a limit results to \a results. 
# \a tok defines how s is tokenized.
#
# The same as 
# #lookup_fd(const StringVector&, ssize_t) const 
# but uses a tokenizer and a string instead of a StringVector.
#
# HfstOneLevelPaths * lookup_fd(
# const HfstTokenizer& tok,
# const std::string &s, ssize_t limit = -1) const;
# 
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
# @brief Whether lookup of path \a s will have infinite results.
#
# Currently, this function will return whether the transducer
# is infinitely ambiguous on any lookup path found in the transducer,
# i.e. the argument \a s is ignored.
#
# @todo Do not ignore the argument s
# @see lookup(HfstOneLevelPaths&, const StringVector&, ssize_t) const
# bool is_lookup_infinitely_ambiguous(const StringVector& s) const;
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


## Compile a lexc file in file \a filename into an HfstTransducer
# of type \a type and return the transducer. 
# static HfstTransducer * read_lexc(const std::string &filename,
# ImplementationType type)
# 
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
  
## A namespace for functions that create two-level, replace, 
# restriction and coercion rule transducers. 
# namespace rules
# {
# enum ReplaceType {REPL_UP, REPL_DOWN, REPL_RIGHT, REPL_LEFT,
# REPL_DOWN_KARTTUNEN};
# enum TwolType {twol_right, twol_left, twol_both};
# 
# /* helping methods 
# HfstTransducer universal_fst
# (const StringPairSet &alphabet, ImplementationType type)
# HfstTransducer negation_fst
# (const HfstTransducer &t, const StringPairSet &alphabet)
# 
# HfstTransducer replace
# (HfstTransducer &t, ReplaceType repl_type, bool optional, 
# StringPairSet &alphabet)
# HfstTransducer replace_transducer
# (HfstTransducer &t, std::string lm, std::string rm, 
# ReplaceType repl_type, StringPairSet &alphabet)
# HfstTransducer replace_context
# (HfstTransducer &t, std::string m1, std::string m2, 
# StringPairSet &alphabet)
# HfstTransducer replace_in_context
# (HfstTransducerPair &context, ReplaceType repl_type, 
# HfstTransducer &t, bool optional, StringPairSet &alphabet)
# 
# /* Used by hfst-calculate. 
# HfstTransducer restriction
# (HfstTransducerPairVector &contexts, HfstTransducer &mapping, 
# StringPairSet &alphabet,        TwolType twol_type, int direction ) 
# 
# 
# 
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

