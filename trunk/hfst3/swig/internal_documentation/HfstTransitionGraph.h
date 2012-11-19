## Insert freely any number of \a symbol_pair in the transducer with weight \a weight.
# @param symbol_pair A string pair to be inserted.
# @param weight The weight of the inserted symbol pair.

## Insert freely any number of any symbol in \a symbol_pairs in the graph with weight \a weight.
# @param symbol_pairs A tuple of string pairs to be inserted.
# @param weight The weight of the inserted symbol pair.
        
## Insert freely any number of \a transducer in this transducer.
# param transducer An HfstBasicTransducer to be inserted.



        ## Harmonize this HfstTransitionGraph and \a another.

            In harmonization the unknown and identity symbols in 
            transitions of both graphs are expanded according to
            the symbols that are previously unknown to the graph. 

            For example the graphs
\verbatim 
   [a:b ?:?]
   [c:d ? ?:c]
\endverbatim
            are expanded to
\verbatim
   [ a:b [?:? | ?:c | ?:d | c:d | d:c | c:? | d:?] ] 
   [ c:d [? | a | b] [?:c| a:c | b:?] ]
\endverbatim
            when harmonized.
             The symbol "?" means \@_UNKNOWN_SYMBOL_\@ in either or 
            both sides of a transition 
            (transitions of type [?:x], [x:?] and [?:?]).
            The transition [?] means [\@_IDENTITY_SYMBOL_\@].

            @note This function is always called for arguments of functions
            that take two or more graphs as their arguments, unless otherwise
            said.
        */
HfstTransitionGraph &harmonize(HfstTransitionGraph &another);
        
        ## Disjunct this graph with a one-path graph 
            defined by string pair vector \a spv with weight \a weight. 
            
            @pre This graph must be a trie where all weights are in
            final states, i.e. all transitions have a zero weight. 

            There is no way to test whether a graph is a trie, so the use
            of this function is probably limited to fast construction 
            of a lexicon. Here is an example: 

            \verbatim
            HfstBasicTransducer lexicon;
            HfstTokenizer TOK;
            lexicon.disjunct(TOK.tokenize("dog"), 0.3);
            lexicon.disjunct(TOK.tokenize("cat"), 0.5);
            lexicon.disjunct(TOK.tokenize("elephant"), 1.6);
            \endverbatim

        */
        HfstTransitionGraph &disjunct
        (const StringPairVector &spv, typename C::WeightType weight);
