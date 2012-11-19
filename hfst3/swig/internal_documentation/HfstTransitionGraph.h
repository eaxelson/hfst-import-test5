    ## A simple transition graph format that consists of
    #    states and transitions between those states.
    #
    # Probably the easiest way to use this template is to choose
    # the implementations #HfstBasicTransducer
    # (HfstTransitionGraph<HfstTropicalTransducerTransitionData>)
    # and #HfstBasicTransition
    # (HfstTransition<HfstTropicalTransducerTransitionData>).
    # The class HfstTropicalTransducerTransitionData contains an input string,
    # an output string and a float weight. HfstBasicTransducer is the 
    # implementation that is used as an example in this documentation.

    An example of creating a HfstBasicTransducer [foo:bar baz:baz] 
    with weight 0.4 from scratch:

\verbatim
  // Create an empty transducer
  // The transducer has initially one start state (number zero) 
  // that is not final
  HfstBasicTransducer fsm;
  // Add two states to the transducer
  fsm.add_state(1);
  fsm.add_state(2);
  // Create a transition [foo:bar] leading to state 1 with weight 0.1 ...
  HfstBasicTransition tr(1, "foo", "bar", 0.1);
  // ... and add it to state zero
  fsm.add_transition(0, tr);
  // Add a transition [baz:baz] with weight 0 from state 1 to state 2 
  fsm.add_transition(1, HfstBasicTransition(2, "baz", "baz", 0.0));
  // Set state 2 as final with weight 0.3
  fsm.set_final_weight(2, 0.3);
\endverbatim

       An example of iterating through a HfstBasicTransducer's states
       and transitions when printing it in AT&T format to stderr:

\verbatim
  // The first state is always number zero.
  unsigned int source_state=0;

  // Go through all states
    for (HfstBasicTransducer::const_iterator it = fsm.begin();
     it != fsm.end(); it++ )
      {
        // Go through all transitions
    for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
           = it->begin(); tr_it != it->end(); tr_it++)
      {
        std::cerr << source_state << "\t"
              << tr_it->get_target_state() << "\t"
              << tr_it->get_input_symbol() << "\t"
              << tr_it->get_output_symbol() << "\t"
              << tr_it->get_weight() << std::endl;
      }

    if (fsm.is_final_state(source_state)) 
      {
        std::cerr << source_state << "\t"
              << fsm.get_final_weight(source_state) << std::endl;
      }
    
    // the next state is numbered source_state + 1  
    source_state++;
      }
\endverbatim

@see #HfstBasicTransducer HfstBasicTransition */
class HfstTransitionGraph;

        ## The states of the graph. */
std::vector<HfstState> states() const;

        ## Create a graph with one initial state that has state number
            zero and is not a final state, i.e. create an empty graph. */
HfstTransitionGraph(void);

    ## The assignment operator. */
HfstTransitionGraph &operator=(const HfstTransitionGraph &graph);

HfstTransitionGraph &assign(const HfstTransitionGraph &graph);

    ## Create a deep copy of HfstTransitionGraph \a graph. */
HfstTransitionGraph(const HfstTransitionGraph &graph);
    
    ## Create an HfstTransitionGraph equivalent to HfstTransducer 
        \a transducer. FIXME: move to a separate file */
HfstTransitionGraph(const hfst::HfstTransducer &transducer);

/* Print the alphabet of the graph to standard error stream. */
void print_alphabet() const; 

        ## Explicitly add \a symbol to the alphabet of the graph.

            @note Usually the user does not have to take care of the alphabet
            of a graph. This function can be useful in some special cases. */
void add_symbol_to_alphabet(const HfstSymbol &symbol);

    ## Remove symbol \a symbol from the alphabet of the graph. 

        @note Use with care, removing symbols that occur in the transitions
        of the graph can have unexpected results. */
void remove_symbol_from_alphabet(const HfstSymbol &symbol);

void remove_symbols_from_alphabet(const HfstSymbolSet &symbols);

    ## Same as #add_symbol_to_alphabet for each symbol in
        \a symbols. */
void add_symbols_to_alphabet(const HfstSymbolSet &symbols);

void add_symbols_to_alphabet(const HfstSymbolPairSet &symbols);

        ## Remove all symbols that do not occur in transitions of
            the graph from its alphabet. 

            Epsilon, unknown and identity \link hfst::String symbols\endlink
            are always included in the alphabet. */
void prune_alphabet();

        ## Get the set of HfstSymbols in the alphabet 
            of the graph. 
        
            The HfstSymbols do not necessarily occur in any transitions
            of the graph. Epsilon, unknown and identity \link 
            hfst::String symbols\endlink are always included in the alphabet. */
const HfstTransitionGraphAlphabet &get_alphabet() const;

        ## Add a new state to this graph and return its number.
        
            @return The next (smallest) free state number. */
HfstState add_state(void);

        ## Add a state \a s to this graph.
 
            If the state already exists, it is not added again.
        All states with state number smaller than \a s are also
        added to the graph if they did not exist before.
            @return \a s*/
HfstState add_state(HfstState s);

    ## Get the biggest state number in use. */
HfstState get_max_state() const;

        ## Add a transition \a transition to state \a s. 

            If state \a s does not exist, it is created. */
    void add_transition(HfstState s, const HfstTransition<C> & transition,
                        bool add_symbols_to_alphabet=true);

        ## Whether state \a s is final. 
        FIXME: return positive infinity instead if not final. */
bool is_final_state(HfstState s) const;

        /** Get the final weight of state \a s in this graph. */
typename C::WeightType get_final_weight(HfstState s) const;

        ## Set the final weight of state \a s in this graph 
            to \a weight. 

            If the state does not exist, it is created. */
        void set_final_weight(HfstState s, 
                              const typename C::WeightType & weight);
    
        ## Sort the arcs of this transducer according to input and
            output symbols. */
HfstTransitionGraph &sort_arcs(void);


        ## Get the set of transitions of state \a s in this graph. 

            If the state does not exist, a @a StateIndexOutOfBoundsException
            is thrown.
        */
const HfstTransitions & operator[](HfstState s) const;
        
        ## Alternative name for operator[].

            Python interface uses this function as '[]' is not a legal name.

            @see operator[]
         */
const HfstTransitions & transitions(HfstState s) const;

        ## Write the graph in AT&T format to ostream \a os.
            \a write_weights defines whether weights are printed. */
void write_in_att_format(std::ostream &os, bool write_weights=true);

        ## Write the graph in AT&T format to FILE \a file.
            \a write_weights defines whether weights are printed. */
void write_in_att_format(FILE *file, bool write_weights=true);

        ## Write the graph in AT&T format to FILE \a file using numbers
            instead of symbol names.
            \a write_weights defines whether weights are printed. */
void write_in_att_format_number(FILE *file, bool write_weights=true);


        ## Create an HfstTransitionGraph as defined in AT&T 
            transducer format in istream \a is. \a epsilon_symbol 
            defines how epsilon is represented. 
            @pre \a is not at end, otherwise an exception is thrown. 
            @note Multiple AT&T transducer definitions are separated with 
            the line "--". */
        static HfstTransitionGraph read_in_att_format
          (std::istream &is,
           std::string epsilon_symbol,
           unsigned int & linecount);

        ## Create an HfstTransitionGraph as defined 
            in AT&T transducer format in FILE \a file. 
            \a epsilon_symbol defines how epsilon is represented. 
            @pre \a is not at end, otherwise an exception is thrown. 
            @note Multiple AT&T transducer definitions are separated with 
            the line "--". */
        static HfstTransitionGraph read_in_att_format
          (FILE *file, 
           std::string epsilon_symbol,
           unsigned int & linecount);

        static HfstTransitionGraph read_in_att_format
          (HfstFile &file, 
           std::string epsilon_symbol,
           unsigned int & linecount);

        ## Substitute \a old_symbol with \a new_symbol in 
            all transitions. \a input_side and \a output_side define
            whether the substitution is made on input and output sides. */
        HfstTransitionGraph &
          substitute(const HfstSymbol &old_symbol, 
                     const HfstSymbol  &new_symbol,
                     bool input_side=true, 
                     bool output_side=true);

        ## Substitute all transitions as defined in \a substitutions.
            
            For each transition x:y, \a substitutions is searched and if 
            a mapping x:y -> X:Y is found, the transition x:y is replaced
            with X:Y. If no mapping is found, the transition remains the same.
         */
        HfstTransitionGraph &substitute
        (const HfstSymbolPairSubstitutions &substitutions);

        ## Substitute all transitions \a sp with a set of transitions
            \a sps. */
        HfstTransitionGraph &substitute
        (const HfstSymbolPair &sp, const HfstSymbolPairSet &sps);
  
        ## Substitute all transitions \a old_pair with 
            \a new_pair. */
        HfstTransitionGraph &substitute
          (const HfstSymbolPair &old_pair, 
           const HfstSymbolPair &new_pair);

        ## Substitute all transitions \a old_symbol : \a new_symbol
            with a copy of \a graph.

            Copies of \a graph are attached to this graph with
            epsilon transitions. 
            
            The weights of the transitions to be substituted are copied
            to epsilon transitions leaving from the source state of
            the transitions to be substituted to the initial state
            of a copy of \a graph.

            The final weights in \a 
            graph are copied to epsilon transitions leading from
            the final states (after substitution non-final states)
            of \a graph to target states of transitions
            \a old_symbol : \a new_symbol (that are substituted)
            in this graph.            
        */
        HfstTransitionGraph &
          substitute(const HfstSymbolPair &sp, 
                     const HfstTransitionGraph &graph);

        ## Insert freely any number of \a symbol_pair in 
            the graph with weight \a weight. */
        HfstTransitionGraph &insert_freely
        (const HfstSymbolPair &symbol_pair, typename C::WeightType weight);

        ## Insert freely any number of any symbol in \a symbol_pairs in 
            the graph with weight \a weight. */
        HfstTransitionGraph &insert_freely
          (const HfstSymbolPairSet &symbol_pairs, 
           typename C::WeightType weight);
        
        ## Insert freely any number of \a graph in this
            graph. */
        HfstTransitionGraph &insert_freely
        (const HfstTransitionGraph &graph);

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

   
    ## An HfstTransitionGraph with transitions of type 
    HfstTropicalTransducerTransitionData and weight type float.
    
    This is probably the most useful kind of HfstTransitionGraph. */
    typedef HfstTransitionGraph <HfstTropicalTransducerTransitionData> 
      HfstBasicTransducer;
    
    ## A specialization for faster conversion. */
    typedef HfstTransitionGraph <HfstFastTransitionData> 
      HfstFastTransducer;
