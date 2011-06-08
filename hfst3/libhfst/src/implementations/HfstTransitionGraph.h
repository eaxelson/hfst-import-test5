#ifndef _HFST_TRANSITION_GRAPH_H_
#define _HFST_TRANSITION_GRAPH_H_

#include <cstdio>
#include <string>
#include <set>
#include "../HfstSymbolDefs.h"
#include "../HfstExceptionDefs.h"
#include "ConvertTransducerFormat.h"
#include "../HfstDataTypes.h"
#include <cassert>
#include <iostream>
#include <algorithm>

#include "HfstTropicalTransducerTransitionData.h"
#include "HfstFastTransitionData.h"

/** @file HfstTransitionGraph.h
    @brief Declaration of classes needed by HFST's 
    own simple transducer format. */

namespace hfst {

  /** @brief A namespace for all code that forms a bridge between
      backend libraries and HFST.

      Most code in this namespace is not intended to be accessed
      by the user of the HFST interface. An exception is HFST's
      own transducer class hfst::HfstTransitionGraph and classes that
      are needed to use it.
  */
  namespace implementations {

    /** @brief The number of a state in an HfstTransitionGraph. */
    typedef unsigned int HfstState;

    /** @brief A transition that consists of a target state and 
        transition data represented by class C. 

       The easiest way to use this template is to choose the 
       the implementation #HfstBasicTransition which is compatible with
       #HfstBasicTransducer.

       @see HfstBasicTransition
    */
    template <class C> class HfstTransition 
      {
      protected:
        HfstState target_state;
        C transition_data;
      public:

        /** @brief Create a transition leading to state zero with input and
            output symbols and weight as given by default constructors
            of C::SymbolType and C::WeightType. */
        HfstTransition(): target_state(0)
          {}

        /** @brief Create a transition leading to state \a s with input symbol
            \a isymbol, output_symbol \a osymbol and weight \a weight. */
        HfstTransition(HfstState s, 
                       typename C::SymbolType isymbol, 
                       typename C::SymbolType osymbol, 
                       typename C::WeightType weight):
        target_state(s), transition_data(isymbol, osymbol, weight)
          {}

        /** @brief Create a deep copy of transition \a another. */
      HfstTransition(const HfstTransition<C> &another): 
        target_state(another.target_state), 
          transition_data(another.transition_data) 
          {}

        /** @brief Whether this transition is less than transition \a
            another. Needed for storing transitions in a set. */
        bool operator<(const HfstTransition<C> &another) const {
          if (target_state == another.target_state)
            return (transition_data < another.transition_data);
          return (target_state < another.target_state);
        }

        /** @brief Assign this transition the same value as transition 
            \a another. */
        void operator=(const HfstTransition<C> &another) {
          target_state = another.target_state;
          transition_data = another.transition_data;
        }

        /** @brief Get the target state of the transition. */
        HfstState get_target_state() const {
          return target_state;
        }

        /** @brief Get the transition data of the transition. */
        const C & get_transition_data() const {
          return transition_data;
        }
	
        /** @brief Get the input symbol of the transition. */
        typename C::SymbolType get_input_symbol() const {
          return transition_data.get_input_symbol();
        }

	unsigned int get_input_number() const {
	  return transition_data.input_number;
	}
	
        /** @brief Get the output symbol of the transition. */
        typename C::SymbolType get_output_symbol() const {
          return transition_data.get_output_symbol();
        }

        /** @brief Get the weight of the transition. */
        typename C::WeightType get_weight() const {
          return transition_data.get_weight();
        }

        friend class ComposeIntersectFst;
        friend class ComposeIntersectLexicon;
        friend class ComposeIntersectRule;
        friend class ComposeIntersectRulePair;
      };

    /** @brief An HfstTransition with transition data of type
        HfstTropicalTransducerTransitionData. 

        This implementation is compatible with #HfstBasicTransducer.

        @see HfstTropicalTransducerTransitionData HfstBasicTransducer */
    typedef HfstTransition<HfstTropicalTransducerTransitionData> 
      HfstBasicTransition;

    typedef HfstTransition<HfstFastTransitionData> HfstFastTransition;

    /** @brief A simple transition graph format that consists of
        states and transitions between those states.

       An HfstTransitionGraph contains two maps. One maps each state
       to a set of that state's transitions (that are of type class 
       HfstTransition<class C>). The other maps each final state to its 
       final weight (that is of type class W). Class C must use the weight
       type W. A state's transition (class HfstTransition<class C>) contains
       a target state and a transition data field (that is of type class C).

       Probably the easiest way to use this template is to choose
       the implementations #HfstBasicTransducer
       (HfstTransitionGraph<HfstTropicalTransducerTransitionData, float>)
       and #HfstBasicTransition
       (HfstTransition<HfstTropicalTransducerTransitionData>).
       The class HfstTropicalTransducerTransitionData contains an input string,
       an output string and a float weight. HfstBasicTransducer is the 
       implementation that is used as an example in this documentation.

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
  // Go through all states
  for (HfstBasicTransducer::iterator it = fsm.begin();
       it != fsm.end(); it++)
    {
      // Go through the set of transitions in each state
      for (HfstBasicTransducer::HfstTransitionSet::iterator tr_it = 
             it->second.begin();
           tr_it != it->second.end(); tr_it++)
        {
          fprintf(stderr, "%i\t%i\t%s\t%s\t%f\n",
                  it->first,
                  tr_it->get_target_state(),
                  tr_it->get_input_symbol().c_str(),
                  tr_it->get_output_symbol().c_str(),
                  tr_it->get_weight()
                  );
        }
      if (fsm.is_final_state(it->first))
        {
          fprintf(stderr, "%i\t%f\n",
                  it->first,
                  fsm.get_final_weight(it->first));
        }
    }
\endverbatim

       @see #HfstBasicTransducer HfstBasicTransition */
    template <class C, class W> class HfstTransitionGraph 
      {
      public:
        /** @brief A vector of transitions of a state in an 
	    HfstTransitionGraph. */
        typedef std::vector<HfstTransition<C> > HfstTransitions;

	typedef std::pair<typename C::SymbolType, 
	  typename C::SymbolType> HfstSymbolPair; 

      protected:
        typedef std::vector<HfstTransitions>
          HfstStates;
        HfstStates state_vector;
        typedef std::map<HfstState,W> FinalWeightMap;
        FinalWeightMap final_weight_map;
        typedef std::set<typename C::SymbolType> HfstTransitionGraphAlphabet;
        HfstTransitionGraphAlphabet alphabet;

      public:
        /** @brief An iterator type that points to the map of states 
            in the graph. 

            The value pointed by the iterator is of type 
            std::pair<HfstState, HfstTransitions >. */
        typedef typename HfstStates::iterator iterator;
        /** @brief A const iterator type that points to the map of states
            in the graph.

            The value pointed by the iterator is of type 
            std::pair<HfstState, HfstTransitions >. */
        typedef typename HfstStates::const_iterator const_iterator;

        /** @brief Create a graph with one initial state that has state
            number zero and is not a final state, i.e. an empty graph. */
        HfstTransitionGraph(void) {
          initialize_alphabet(alphabet);
	  HfstTransitions tr;
          state_vector.push_back(tr);
        }

	// FIXME: the default should be enough
	HfstTransitionGraph &operator=(const HfstTransitionGraph &graph)
	  {
	    if (this == &graph)
	      return *this;
	    state_vector = graph.state_vector;
	    final_weight_map = graph.final_weight_map;
	    alphabet = graph.alphabet;
	    assert(alphabet.count(typename C::SymbolType()) == 0); // TEST
	    return *this;
	  }

        /** @brief Create a deep copy of HfstTransitionGraph \a graph. */
        HfstTransitionGraph(const HfstTransitionGraph &graph) {
          state_vector = graph.state_vector;
          final_weight_map = graph.final_weight_map;
          alphabet = graph.alphabet;
	  assert(alphabet.count(typename C::SymbolType()) == 0);
        }

        /** @brief Create an HfstTransitionGraph equivalent to HfstTransducer 
            \a transducer. FIXME: move to a separate file */
        HfstTransitionGraph(const hfst::HfstTransducer &transducer) {
          HfstTransitionGraph<HfstTropicalTransducerTransitionData, float>
            *fsm = ConversionFunctions::
              hfst_transducer_to_hfst_basic_transducer(transducer);
          state_vector = fsm->state_vector;
          final_weight_map = fsm->final_weight_map;
          alphabet = fsm->alphabet;
          delete fsm;
        }

	void initialize_alphabet(HfstTransitionGraphAlphabet &alpha) {
	  alpha.insert(C::get_epsilon());
	  alpha.insert(C::get_unknown());
	  alpha.insert(C::get_identity());
	}

        /** @brief Explicitly add a symbol to the alphabet of the graph.

            Usually the user does not have to take care of the alphabet
            of a graph. This function can be useful in some special cases. */
        void add_symbol_to_alphabet(const typename C::SymbolType &symbol) {
          alphabet.insert(symbol);
        }

        /** @brief Remove all symbols that do not occur in transitions of
            the graph from its alphabet. 

            Epsilon, unknown and identity \link hfst::String symbols\endlink
            are always included in the alphabet. */
        void prune_alphabet() {

          // Which symbols occur in the graph
          HfstTransitionGraphAlphabet symbols_found;
          initialize_alphabet(symbols_found); /* special symbols are 
                                                 always known */

          for (iterator it = begin(); it != end(); it++)
            {
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();
                  
                  symbols_found.insert(data.get_input_symbol());
                  symbols_found.insert(data.get_output_symbol());
                }
            }
          
          // Which symbols in the graph's alphabet did not occur in 
          // the graph
          HfstTransitionGraphAlphabet symbols_not_found;

          for (typename HfstTransitionGraphAlphabet::iterator it 
                 = alphabet.begin();
               it != alphabet.end(); it++) 
            {
              if (symbols_found.find(*it) == symbols_found.end())
                symbols_not_found.insert(*it);
            }

          // Remove the symbols that did not occur in the graph
          // from its alphabet
          for (typename HfstTransitionGraphAlphabet::iterator it 
                 = symbols_not_found.begin();
               it != symbols_not_found.end(); it++)
            {
              alphabet.erase(*it);
            }
        }

        /** @brief Get the set of SymbolTypes in the alphabet 
            of the graph. 

            The SymbolTypes do not necessarily occur in any transitions
            of the graph. Epsilon, unknown and identity \link 
            hfst::String symbols\endlink are always included in the alphabet. */
        const std::set<typename C::SymbolType> &get_alphabet() const {
          return alphabet;
        }

        /** @brief Add a new state to this graph and return its number.

            @return The next (smallest) free state number. */
        HfstState add_state(void) {
	  HfstTransitions tr;
	  state_vector.push_back(tr);
	  return state_vector.size()-1;
	}

        /** @brief Add a state \a s to this graph.
 
            If the state already exists, it is not added again. 
            @return \a s*/
        HfstState add_state(HfstState s) {
	  while(state_vector.size() <= s) {
	    HfstTransitions tr;
	    state_vector.push_back(tr);
	  }
          return s;
        }

	/** @brief Get the biggest state number. */
	HfstState get_max_state() const {
	  return state_vector.size()-1;
	}

	void initialize_state_vector
	  (unsigned int number_of_states)
	{
	  state_vector.reserve(number_of_states);
	}

	void initialize_transition_vector
	  (unsigned int state_number, unsigned int number_of_transitions)
	{
	  add_state(state_number);
	  state_vector[state_number].reserve(number_of_transitions);
	}

        /** @brief Add a transition \a transition to state \a s. 

            If state \a s does not exist, it is created. */
        void add_transition(HfstState s, const HfstTransition<C> & transition) {

          C data = transition.get_transition_data();

	  // FIXME: throw an exception in constructor of HfstTransition?
	  //assert(not data.get_input_symbol().empty());
	  //assert(not data.get_output_symbol().empty());

          add_state(s);
          add_state(transition.get_target_state());
          alphabet.insert(data.get_input_symbol());
          alphabet.insert(data.get_output_symbol());
          state_vector[s].push_back(transition);
        }

        /** @brief Whether state \a s is final. 
	    FIXME: return positive infinity instead if not final. */
        bool is_final_state(HfstState s) const {
          return (final_weight_map.find(s) != final_weight_map.end());
        }

        /** Get the final weight of state \a s in this graph. */
        W get_final_weight(HfstState s) const {
          if (final_weight_map.find(s) != final_weight_map.end())
            return final_weight_map.find(s)->second;
          HFST_THROW(StateIsNotFinalException);
        }

        /** @brief Set the final weight of state \a s in this graph 
            to \a weight. 

            If the state does not exist, it is created. */
        void set_final_weight(HfstState s, const W & weight) {
	  add_state(s);
          final_weight_map[s] = weight;
        }

        /** @brief Get an iterator to the beginning of the map of states in 
            the graph. 

            For an example, see #HfstTransitionGraph */
        iterator begin() { return state_vector.begin(); }

        /** @brief Get a const iterator to the beginning of the map of 
            states in the graph. */
        const_iterator begin() const { return state_vector.begin(); }

        /** @brief Get an iterator to the end of the map of states in
            the graph. */
        iterator end() { return state_vector.end(); }

        /** @brief Get a const iterator to the end of the map of states in
            the graph. */
        const_iterator end() const { return state_vector.end(); }


        /** @brief Get the set of transitions of state \a s in this graph. 

            If the state does not exist, a @a StateIndexOutOfBoundsException
            is thrown.
        */
        const HfstTransitions & operator[](HfstState s) const
        {
          if (s >= state_vector.size()) { 
	    HFST_THROW(StateIndexOutOfBoundsException); }
          return state_vector[s];
        }        

      public:
        /** @brief Get the set of transitions of state \a s in this graph. 
	    
            If the state does not exist, it is created. The created
            state has an empty set of transitions. */
        HfstTransitions & operator[](HfstState s) {
	  add_state(s);
          return state_vector[s];
        }        

      public:
        /* TODO: Change state numbers s1 to s2 and vice versa. */
        void swap_state_numbers(HfstState /*s1*/, HfstState /*s2*/) {
          HFST_THROW(FunctionNotImplementedException);
        }

        /* Replace all strings \a str1 in \a symbol with \a str2. */
        static std::string replace_all(std::string symbol, 
                           const std::string &str1,
                           const std::string &str2)
        {
          size_t pos = symbol.find(str1);
          while (pos != std::string::npos) // while there are str1:s to replace
            {
              symbol.erase(pos, str1.size()); // erase str1
              symbol.insert(pos, str2);       // insert str2 instead
              pos = symbol.find               // find next str1
                (str1, pos+str2.size());      
            }
          return symbol;
        }


        /** @brief Write the graph in AT&T format to ostream \a os.
            \a write_weights defines whether weights are printed. */
        void write_in_att_format(std::ostream &os, bool write_weights=true) 
        {
	  unsigned int source_state=0;
          for (iterator it = begin(); it != end(); it++)
            {
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();
                  
                  os <<  source_state << "\t" 
                     <<  tr_it->get_target_state() << "\t"
                // replace all spaces, epsilons and tabs
                     <<  
		    replace_all
		    (replace_all
		     (replace_all(data.get_input_symbol(), 
				  " ", "@_SPACE_@"),
		      "@_EPSILON_SYMBOL_@", "@0@"),
		     "\t", "@_TAB_@")
                     << "\t"
                     <<  
		    replace_all
		    (replace_all
		     (replace_all(data.get_output_symbol(), 
				  " ", "@_SPACE_@"),
		      "@_EPSILON_SYMBOL_@", "@0@"),
		     "\t", "@_TAB_@");
                  if (write_weights)
                    os <<  "\t" << data.get_weight(); 
                  os << "\n";
                }
              if (is_final_state(source_state))
                {
                  os <<  source_state;
                  if (write_weights)
                    os << "\t" <<  get_final_weight(source_state);
                  os << "\n";
                }
	      source_state++;
            }          
        }

        /** @brief Write the graph in AT&T format to FILE \a file.
            \a write_weights defines whether weights are printed. */
        void write_in_att_format(FILE *file, bool write_weights=true) 
        {
	  unsigned int source_state=0;
          for (iterator it = begin(); it != end(); it++)
            {
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();
                  
                  fprintf(file, "%i\t%i\t%s\t%s",
                          source_state,
                          tr_it->get_target_state(),
                  // replace all spaces and epsilons
			  replace_all
			  (replace_all
			   (replace_all(data.get_input_symbol(), 
					" ", "@_SPACE_@"),
			    "@_EPSILON_SYMBOL_@", "@0@"),
			   "\t", "@_TAB_@").c_str(),
			  replace_all
			  (replace_all
			   (replace_all(data.get_output_symbol(),
					" ", "@_SPACE_@"),
			    "@_EPSILON_SYMBOL_@", "@0@"),
			   "\t", "@_TAB_@").c_str());

                  if (write_weights)
                    fprintf(file, "\t%f",
                            data.get_weight()); 
                  fprintf(file, "\n");
                }
              if (is_final_state(source_state))
                {
                  fprintf(file, "%i", source_state);
                  if (write_weights)
                    fprintf(file, "\t%f", 
                            get_final_weight(source_state));
                  fprintf(file, "\n");
                }
	      source_state++;
            }          
        }

        /* Create an HfstTransitionGraph as defined in AT&T format 
           in istream \a is or FILE \a file. \a epsilon_symbol defines
           how epsilon is represented. 

           The functions is called by functions 
           read_in_att_format(istream&, std::string) and
           read_in_att_format(FILE*, std::string). 
           If \a file is NULL, it is ignored and \a is is used.
           If \a file is not NULL, it is used and \a is is ignored. */
        static HfstTransitionGraph read_in_att_format
          (std::istream &is,
           FILE *file,
           std::string epsilon_symbol=
           std::string("@_EPSILON_SYMBOL_@")) {

          HfstTransitionGraph retval;
          char line [255];
          while(true) {

            if (file == NULL) { /* we use streams */
              if (not is.getline(line,255).eof())
                break;
            }
            else { /* we use FILEs */            
              if (NULL == fgets(line, 255, file))
                break;
            }

	    // an empty line signifying an empty transducer,
	    // a special case that is accepted if it is the only
	    // transducer in the stream
	    if (line[0] == '\0' ||
		(line[0] == '\n' && line[1] == '\0')) {
	      // make sure that the end-of-file is reached
	      if (file == NULL)
		is.get();
	      else
		fgetc(file);
	      break;
	    }

            if (*line == '-') // transducer separator line is "--"
              return retval;

            // scan one line that can have a maximum of five fields
            char a1 [100]; char a2 [100]; char a3 [100]; 
            char a4 [100]; char a5 [100];
            // how many fields could be parsed
            //int n = sscanf(line, "%s\t%s\t%s\t%s\t%s", a1, a2, a3, a4, a5);
            int n = sscanf(line, "%s%s%s%s%s", a1, a2, a3, a4, a5);

            // set value of weight
            float weight = 0;
            if (n == 2) // a final state line with weight
              weight = atof(a2);
            if (n == 5) // a transition line with weight
              weight = atof(a5);
            
            if (n == 1 || n == 2)  // a final state line
              retval.set_final_weight( atoi(a1), weight );
            
            else if (n == 4 || n == 5) { // a transition line
              std::string input_symbol=std::string(a3);
              std::string output_symbol=std::string(a4);

              // replace "@_SPACE_@"s with " " and "@0@"s with 
              // "@_EPSILON_SYMBOL_@"
              input_symbol 
		= replace_all 
		(replace_all
		 (replace_all
		  (replace_all(input_symbol, "@_SPACE_@", " "),
		   "@0@", "@_EPSILON_SYMBOL_@"),
		  "@_TAB_@", "\t"),
		 "@_COLON_@", ":");
	      output_symbol 
		= replace_all
		(replace_all
		 (replace_all
		  (replace_all(output_symbol, "@_SPACE_@", " "),
		   "@0@", "@_EPSILON_SYMBOL_@"),
		  "@_TAB_@", "\t"),
		 "@_COLON_@", ":");

              if (epsilon_symbol.compare(input_symbol) == 0)
                input_symbol="@_EPSILON_SYMBOL_@";
              if (epsilon_symbol.compare(output_symbol) == 0)
                output_symbol="@_EPSILON_SYMBOL_@";
              
              HfstTransition <C> tr( atoi(a2), input_symbol, 
                                      output_symbol, weight );
              retval.add_transition( atoi(a1), tr );
            }
            
            else  {  // line could not be parsed
              std::string message(line);
              HFST_THROW_MESSAGE
                (NotValidAttFormatException,
                 message);
            }    
          }
          return retval;
        }


        /** @brief Create an HfstTransitionGraph as defined in AT&T 
            transducer format in istream \a is. \a epsilon_symbol 
            defines how epsilon is represented. 
            @pre \a is not at end, otherwise an exception is thrown. 
            @note Multiple AT&T transducer definitions are separated with 
            the line "--". */
        static HfstTransitionGraph read_in_att_format
          (std::istream &is,
           std::string epsilon_symbol=
             std::string("@_EPSILON_SYMBOL_@")) 
        {
          return read_in_att_format
            (is, NULL /* a dummy variable */,
             epsilon_symbol);
        }

        /** @brief Create an HfstTransitionGraph as defined 
            in AT&T transducer format in FILE \a file. 
            \a epsilon_symbol defines how epsilon is represented. 
            @pre \a is not at end, otherwise an exception is thrown. 
            @note Multiple AT&T transducer definitions are separated with 
            the line "--". */
        static HfstTransitionGraph read_in_att_format
          (FILE *file, 
           std::string epsilon_symbol=
             std::string("@_EPSILON_SYMBOL_@")) 
        {
          return read_in_att_format
            (std::cin /* a dummy variable */,
             file, epsilon_symbol);
        }


        /** @brief Sort the arcs of this transducer according to input and
            output symbols.
	*/
        HfstTransitionGraph &sort_arcs(void)
	  {
	    for (typename HfstStates::iterator it = state_vector.begin();
		 it != state_vector.end();
		 ++it)
	      {
		HfstTransitions &transitions = *it;
		std::sort<typename HfstTransitions::iterator>
		  (transitions.begin(),transitions.end());
	      }
	    return *this;
	  }
	

        /* ----------------------------
              Substitution functions
           ---------------------------- */

      protected:

        /* -------------------------------------------------------
              A class used by function substitute(substituter&) 
           -------------------------------------------------------  */
        struct substituter {

          /* Whether one symbol is substituted with another symbol. */
          bool substitute_symbol;
          std::string old_symbol;
          std::string new_symbol;
          bool input_side;
          bool output_side;

          /* Whether a symbol pair is substituted with a set of symbol pairs. */
          bool substitute_symbol_pair;
          StringPair SP;
          StringPairSet SPS;

          /* Whether substitution is made according to a function. */
          bool substitute_using_function;
          bool (*func) (const StringPair &sp, StringPairSet &sps);

          /* Whether substitution is expanding unknown and identity symbols. */
          bool substitute_expand;
          StringSet unknown_set;

          /* Create a substituter that substitutes one symbol with
             another symbol. */
          substituter(const std::string &old_symbol_,
                      const std::string &new_symbol_, 
                      bool input_side_=true,
                      bool output_side_=true)
          {
            old_symbol = old_symbol_;
            new_symbol = new_symbol_;
            input_side = input_side_;
            output_side = output_side_;
            substitute_symbol =true;
          }

          /* Create a substituter that substitutes a symbol pair with
             a set of symbol pairs. */
          substituter(const StringPair &sp, const StringPairSet &sps)
          {
            SP = sp;
            SPS = sps;
            substitute_symbol = false;
            substitute_symbol_pair = true;
          }

          /* Create a substituter that substitutes transitions according
             to a function. */
          substituter(bool (*func_) (const StringPair &sp, 
                                     StringPairSet &sps))
          {
            func = func_;
            substitute_symbol = false;
            substitute_symbol_pair = false;
            substitute_using_function = true;
          }

          /* Create a substituter that expands unknown and identity symbols. */
          substituter(HfstTransitionGraphAlphabet &alpha)
          {
            for (typename HfstTransitionGraphAlphabet::const_iterator it
                   = alpha.begin; it != alpha.end(); it++)
              {
                unknown_set.insert(*it);
              }
            substitute_symbol = false;
            substitute_symbol_pair = false;
            substitute_using_function = false;
            substitute_expand=true;
          }


          /* Stores to \a sps the transitions with which the transition \a sp
             must be substituted and returns whether any substitutions must
             be made, i.e. whether any transitions were inserted in \a sps. */
          bool substitute(const StringPair &sp, StringPairSet &sps)
          {
            if (substitute_symbol)
              {
                std::string isymbol = sp.first;
                std::string osymbol = sp.second;
                bool substitution_made=false;
                
                if (input_side && isymbol.compare(old_symbol) == 0) {
                  isymbol = new_symbol;
                  substitution_made=true;
                }
                if (output_side && osymbol.compare(old_symbol) == 0) {
                  osymbol = new_symbol;
                  substitution_made=true;
                }
                
                if (substitution_made) {
		  if (false)
		    fprintf(stderr, "substituting %s:%s with %s:%s\n",
			    sp.first.c_str(), sp.second.c_str(), 
			    isymbol.c_str(), osymbol.c_str());
                  sps.insert(StringPair(isymbol, osymbol));
                  return true;
                }
                return false;
              }

            if (substitute_symbol_pair)
              {
                if ( sp.first.compare(SP.first) == 0 &&
                     sp.second.compare(SP.second) == 0 )
                  {
                    for (StringPairSet::const_iterator it = SPS.begin();
                         it != SPS.end(); it ++)
                      {
                        sps.insert(*it);
                      }
                    return true;
                  }
                return false;
              }

            if (substitute_using_function)
              {
                return func(sp, sps);
              }
            
            if (substitute_expand)
              {
                // Identity
                if (sp.first.compare("@_IDENTITY_SYMBOL_@") == 0 &&
                    sp.second.compare("@_IDENTITY_SYMBOL_@") == 0)
                  {
                    for (StringSet::const_iterator it = unknown_set.begin();
                         it != unknown_set.end(); it++)
                      {
                        sps.insert(StringPair(*it, *it));
                      }
                    sps.insert(StringPair("@_IDENTITY_SYMBOL_@", 
                                          "@_IDENTITY_SYMBOL_@"));
                    return true;
                  }

                // Unknown to unknown
                if (sp.first.compare("@_UNKNOWN_SYMBOL_@") == 0 &&
                    sp.second.compare("@_UNKNOWN_SYMBOL_@") == 0)
                  {
                    for (StringSet::const_iterator it1 = unknown_set.begin();
                         it1 != unknown_set.end(); it1++)
                      {
                        for (StringSet::const_iterator it2 
                               = unknown_set.begin();
                             it2 != unknown_set.end(); it2++)
                          {
                            if (it1->compare(*it2) != 0)
                              {
                                sps.insert(StringPair(*it1, *it2));
                              }
                            // add transitions x:? and ?:x
                            sps.insert(StringPair(*it1, "@_UNKNOWN_SYMBOL_@"));
                            sps.insert(StringPair("@_UNKNOWN_SYMBOL_@", *it1));
                          }
                      }
                    sps.insert(StringPair("@_UNKNOWN_SYMBOL_@", 
                                          "@_UNKNOWN_SYMBOL_@"));
                    return true;
                  }

                // Unknown to not unknown
                if (sp.first.compare("@_UNKNOWN_SYMBOL_@") == 0)
                  {
                    for (StringSet::const_iterator it = unknown_set.begin();
                         it != unknown_set.end(); it++)
                      {
                        sps.insert(StringPair(*it, sp.second));
                      }
                    sps.insert(StringPair("@_UNKNOWN_SYMBOL_@", sp.second));
                    return true;
                  }

                // Not unknown to unknown
                if (sp.second.compare("@_UNKNOWN_SYMBOL_@") == 0)
                  {
                    for (StringSet::const_iterator it = unknown_set.begin();
                         it != unknown_set.end(); it++)
                      {
                        sps.insert(StringPair(sp.first, *it));
                      }
                    sps.insert(StringPair(sp.first, "@_UNKNOWN_SYMBOL_@"));
                    return true;
                  }

                // Other cases, no need to expand.
                return false;
              }

            return false;            
          }
        };

	/* A function that performs in-place-substitution in the graph. */

	void substitute_(const std::string &old_symbol, 
			 const std::string &new_symbol,
			 bool input_side=true, 
			 bool output_side=true)
	{
          // Go through all states
          for (iterator it = begin(); it != end(); it++)
            {
	      // Go through all transitions
              for (unsigned int i=0; i < it->size(); i++)
                {
		  HfstTransition<C> &tr_it = it->operator[](i);

		  std::string substituting_input_symbol
		    = tr_it.get_input_symbol();
		  std::string substituting_output_symbol
		    = tr_it.get_output_symbol();
		  bool substitution_made=false;
		  
		  if (input_side &&
		      tr_it.get_input_symbol() == old_symbol) {
		    substituting_input_symbol = new_symbol;
		    substitution_made=true;
		  }
		  if (output_side &&
		      tr_it.get_output_symbol() == old_symbol) {
		    substituting_output_symbol = new_symbol;
		    substitution_made=true;
		  }

		  if (substitution_made) {

		    HfstTransition<C> tr
		      (tr_it.get_target_state(),
		       substituting_input_symbol,
		       substituting_output_symbol,
		       tr_it.get_weight());

		    it->operator[](i) = tr;
		  }
		  
		} // all transitions gone through
	    } // all states gone through
	  return;
	}

	/* A function that performs in-place-substitution in the graph. */

	void substitute_(const StringPair &old_sp, 
			 const StringPair &new_sp)
	{
          // Go through all states
          for (iterator it = begin(); it != end(); it++)
            {
	      // Go through all transitions
              for (unsigned int i=0; i < it->size(); i++)
                {
		  HfstTransition<C> &tr_it = it->operator[](i);

		  if (tr_it.get_input_symbol() == old_sp.first &&
		      tr_it.get_output_symbol() == old_sp.second)
		    {
		      
		      HfstTransition<C> tr
			(tr_it.get_target_state(),
			 new_sp.first,
			 new_sp.second,
			 tr_it.get_weight());
		      
		      it->operator[](i) = tr;
		    }		  
		} // all transitions gone through
	    } // all states gone through
	  return;
	}


        /* ------------------------------------------------------------
           A function used by the public substitution functions. 
           Substitute all transitions according to substituter \a subs. 
           ------------------------------------------------------------ */
        void substitute(substituter &subs) { 

	  /*std::string message
	    ("HfstTransitionGraph::substitute(substituter &subs) called");
	    HFST_THROW_MESSAGE(HfstFatalException, message);*/

          // Go through all states
          for (iterator it = begin(); it != end(); it++)
            {
              // The transitions that are substituted, i.e. removed
              std::vector<typename HfstTransitions::iterator> 
                old_transitions;
              // The substituting transitions that are added
              HfstTransitions new_transitions;

              // Go through all transitions
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();

                  // Whether there is anything to substitute 
                  // in this transition
                  StringPair sp(data.get_input_symbol(), 
                                data.get_output_symbol());
                  StringPairSet sps;

                  // Find out whether there is a need to substitute
                  // and which are the substituting transitions
                  bool substitution_made = 
                    subs.substitute(sp, sps);

                  // If there is something to substitute,
                  if (substitution_made) {

                    // schedule the new transitions to be added
                    for (StringPairSet::const_iterator sps_it = sps.begin();
                         sps_it != sps.end(); sps_it++)
                      {
                        HfstTransition <C> new_transition
                          (tr_it->get_target_state(),
                           sps_it->first,
                           sps_it->second,
                           data.get_weight());
                        
                        new_transitions.push_back(new_transition);
			
			/*fprintf(stderr, "adding transition %i %i %s %s\n",
				it->first,
				new_transition.get_target_state(),
				new_transition.get_input_symbol().c_str(),
				new_transition.get_output_symbol().c_str());*/

                      }

                    // and the old transition to be deleted
                    old_transitions.push_back(tr_it);
                  }
		  else 
		    { // DEBUG
		      /*fprintf
			(stderr, "keeping transition %i %i %s %s\n",
			 it->first,
			 tr_it->get_target_state(),
			 data.get_input_symbol().c_str(),
			 data.get_output_symbol().c_str());*/ 
		    }

                  // (one transition gone through)
                }
              // (all transitions in a state gone through)

	      //fprintf(stderr, "all transitions in a state gone through:\n");
	      //this->write_in_att_format(stderr, true);

              // Remove the substituted transitions
              for (int i = (int)old_transitions.size()-1; 
		   i >= 0; i--) {

		/*fprintf(stderr, "removing transition %i %i %s %s\n", // DEBUG
			it->first,
			old_transitions.at(i)->get_target_state(),
			old_transitions.at(i)->get_input_symbol().c_str(),
			old_transitions.at(i)->get_output_symbol().c_str());*/

                it->erase(old_transitions.at(i));
              }

              // and add the substituting transitions
              for (typename HfstTransitions::iterator IT 
                     = new_transitions.begin();
                   IT != new_transitions.end(); IT++) {
                it->push_back(*IT);
              }

              // (all transitions in a state substituted)
            }
          // (all states handled)

        }


      public:

        /* ----------------------------------------
              The public substitution functions.
           ---------------------------------------- */

        /** @brief Substitute \a old_symbol with \a new_symbol in 
            all transitions. \a input_side and \a output_side define
            whether the substitution is made on input and output sides. 

            @todo Unknown and identity symbols must be handled correctly */
        HfstTransitionGraph &
          substitute(const std::string &old_symbol, 
                     const std::string &new_symbol,
                     bool input_side=true, 
                     bool output_side=true) {

	  if (old_symbol == "" || new_symbol == "")
	    HFST_THROW_MESSAGE
	      (EmptyStringException,
	       "HfstTransitionGraph::substitute");

          // If a symbol is substituted with itself, do nothing.
          if (old_symbol.compare(new_symbol) == 0)
            return *this;
          // If the old symbol is not known to the graph, do nothing.
          if (alphabet.find(old_symbol) == alphabet.end())
            return *this;

          // Remove the symbol to be substituted from the alphabet
          // if the substitution is made on both sides.
          if (input_side && output_side) {
            /* Special symbols are always included in the alphabet */
            if (old_symbol.compare("@_EPSILON_SYMBOL_@") != 0 && 
                old_symbol.compare("@_UNKNOWN_SYMBOL_@") != 0 &&
                old_symbol.compare("@_IDENTITY_SYMBOL_@") != 0)
              alphabet.erase(old_symbol);
          }
          // Insert the substituting symbol to the alphabet.
          alphabet.insert(new_symbol);

	  /*
          // Create a substituter
          substituter subs
            (old_symbol, new_symbol,
             input_side, output_side);
          // and perform the substitutions
          substitute(subs);
	  */

	  substitute_(old_symbol, new_symbol, input_side, output_side);

          return *this;
        }

        /** @brief Substitute all transitions \a sp with a set of transitions
            \a sps. */
        HfstTransitionGraph &substitute
          (const StringPair &sp, const StringPairSet &sps) 
	  {
	    if (sp.first == "" || sp.second == "")
	      HFST_THROW_MESSAGE
		(EmptyStringException,
		 "HfstTransitionGraph::substitute");
	    for (StringPairSet::const_iterator it = sps.begin();
		 it != sps.end(); it++)
	      {
		if (it->first == "" || it->second == "")
		  HFST_THROW_MESSAGE
		    (EmptyStringException,
		     "HfstTransitionGraph::substitute");
	      }

	  /*std::string msg("HfstTransitionGraph &substitute"
			  "(const StringPair &sp, const StringPairSet &sps) ");
			  HFST_THROW_MESSAGE(FunctionNotImplementedException, msg);*/
          substituter subs(sp, sps);
          substitute(subs);
          return *this;
        }
  
        /** @brief Substitute all transitions \a old_pair with 
            \a new_pair. */
        HfstTransitionGraph &substitute
          (const StringPair &old_pair, 
           const StringPair &new_pair) 
        {
	  if (old_pair.first == "" || new_pair.first == "" ||
	      old_pair.second == "" || new_pair.second == "")
	    HFST_THROW_MESSAGE
	      (EmptyStringException,
	       "HfstTransitionGraph::substitute");

          substitute_(old_pair, new_pair);
          return *this;
        } 

        /** @brief Substitute all transitions with a set of transitions as
            defined by function \a func. 

            \a func takes as its argument a transition \a sp and inserts
            into the set of transitions \a sps the transitions with which
            the original transition \a sp must be replaced. \a func returns
            a value indicating whether any substitution must be made, i.e.
            whether any transition was inserted into \a sps. */
        HfstTransitionGraph &
          substitute(bool (*func)
                     (const StringPair &sp, StringPairSet &sps) ) 
        { 
	  /*std::string msg("HfstTransitionGraph &substitute"
			            "substitute(bool (*func)"
                     "(const StringPair &sp, StringPairSet &sps) )" );
		     HFST_THROW_MESSAGE(FunctionNotImplementedException, msg);*/

          substituter subs(func);
          substitute(subs);
          return *this;
        }




        /* ----------------------------------------------------           
              Substitute string pair with a transition graph
           ---------------------------------------------------- */

      protected:
        /* Used in function 
           substitute(const StringPair&, HfstTransitionGraph&) */
        struct substitution_data 
        {
          HfstState origin_state;
          HfstState target_state;
          W weight;
          
          substitution_data(HfstState origin, 
                            HfstState target,
                            W weight)
          {
            origin_state=origin;
            target_state=target;
            this->weight=weight;
          }
        };

        /* Used in function substitute(const StringPair&, 
                                       HfstTransitionGraph&)
           Add a copy of \a graph with epsilon transitions between 
           states and with weight as defined in \a sub. */
        void add_substitution(substitution_data &sub, 
                              const HfstTransitionGraph &graph) {

          // Epsilon transition to initial state of \a graph
          HfstState s = add_state();
          HfstTransition <C> epsilon_transition
            (s, C::get_epsilon(), C::get_epsilon(), 
             sub.weight);
          add_transition(sub.origin_state, epsilon_transition);

          /* Offset between state numbers */
          unsigned int offset = s;

          // Copy \a graph
	  HfstState source_state=0;
          for (const_iterator it = graph.begin(); 
               it != graph.end(); it++)
            {
              for (typename HfstTransitions::const_iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();
                  
                  HfstTransition <C> transition
                    (tr_it->get_target_state() + offset, 
                     data.get_input_symbol(),
                     data.get_output_symbol(),
                     data.get_weight());

                  add_transition(source_state + offset, transition);
                }
	      source_state++;
            }

          // Epsilon transitions from final states of \a graph
          for (typename FinalWeightMap::const_iterator it 
                 = graph.final_weight_map.begin();
               it != graph.final_weight_map.end(); it++)
            {
              HfstTransition <C> epsilon_transition
                (sub.target_state, C::get_epsilon(), C::get_epsilon(),
                 it->second);
              add_transition(it->first + offset, epsilon_transition);
            }
        }


      public:

        /** @brief Substitute all transitions \a old_symbol : \a new_symbol
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
		     const HfstTransitionGraph &graph) {

	  if ( not ( C::is_valid_symbol(sp.first) &&		      
		     C::is_valid_symbol(sp.second) ) ) {
	    HFST_THROW_MESSAGE
	      (EmptyStringException, 
	       "HfstTransitionGraph::substitute(const HfstSymbolPair&, "
	       "const HfstTransitionGraph&)");
	      }

          
          // If neither symbol to be substituted is known to the graph,
          // do nothing.
          if (alphabet.find(sp.first) == alphabet.end() && 
              alphabet.find(sp.second) == alphabet.end())
            return *this;

          // Where the substituting copies of \a graph
          // are inserted (source state, target state, weight)
          std::vector<substitution_data> substitutions;

          // Go through all states
	  HfstState source_state=0;
          for (iterator it = begin(); it != end(); it++)
            {

              // The transitions that are substituted, i.e. removed
              std::vector<typename HfstTransitions::iterator> 
                old_transitions;

              // Go through all transitions
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();

                  // Whether there is anything to substitute 
                  // in this transition
                  if (data.get_input_symbol() == sp.first &&
                      data.get_output_symbol() == sp.second) 
                    {
                      // schedule a substitution
                      substitutions.push_back(substitution_data
                                              (source_state, 
                                               tr_it->get_target_state(), 
                                               data.get_weight()));
                      // schedule the old transition to be deleted
                      old_transitions.push_back(tr_it);
                    }
                  // (one transition gone through)
                } 
              // (all transitions in a state gone through)

              // Remove the substituted transitions
              for (typename std::vector<typename 
                     HfstTransitions::iterator>::iterator IT =
                     old_transitions.begin(); 
                   IT != old_transitions.end(); IT++) {
                it->erase(*IT);
              }
	      
	      source_state++;
            }
          // (all states gone trough)

          // Add the substitutions
          for (typename std::vector<substitution_data>::iterator IT 
                 = substitutions.begin();
               IT != substitutions.end(); IT++)
            {
              add_substitution(*IT, graph);
            }
          return *this;
        }


        /** @brief Insert freely any number of \a symbol_pair in 
            the graph with weight \a weight. */
        HfstTransitionGraph &insert_freely
          (const HfstSymbolPair &symbol_pair, W weight) 
          {    
	    if ( not ( C::is_valid_symbol(symbol_pair.first) &&		      
		       C::is_valid_symbol(symbol_pair.second) ) ) {
	      HFST_THROW_MESSAGE
		(EmptyStringException, 
		 "HfstTransitionGraph::insert_freely"
		 "(const HfstSymbolPair&, W)");
	    }

            alphabet.insert(symbol_pair.first);
            alphabet.insert(symbol_pair.second);
            
	    HfstState source_state=0;
            for (iterator it = begin(); it != end(); it++) {
              HfstTransition <C> tr( source_state, symbol_pair.first, 
                                     symbol_pair.second, weight );              
              it->push_back(tr);
	      source_state++;
            }
            return *this;
          }
        
        /** @brief Insert freely any number of \a graph in this
            graph. */
        HfstTransitionGraph &insert_freely
          (const HfstTransitionGraph &graph)
          {
	    typename C::SymbolType marker_this = C::get_marker(alphabet);
	    typename C::SymbolType marker_graph = C::get_marker(alphabet);
	    typename C::SymbolType marker = marker_this;
	    if (marker_graph > marker)
	      marker = marker_graph;

            HfstSymbolPair marker_pair(marker, marker);
            insert_freely(marker_pair, 0);
            substitute(marker_pair, graph);
            //alphabet.erase(marker); TODO: fix
            return *this;
          }


        /* -------------------------------
               Harmonization function
           ------------------------------- */

        /** @brief Harmonize this HfstTransitionGraph and \a another.

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
        HfstTransitionGraph &harmonize(HfstTransitionGraph &another) {

          /* Collect symbols previously unknown to graphs this and another. */
          HfstTransitionGraphAlphabet unknown_this;
          HfstTransitionGraphAlphabet unknown_another;

          for (typename HfstTransitionGraphAlphabet::const_iterator it
                 = another.alphabet.begin();
               it != another.alphabet.end(); it++)
            {
              if (alphabet.find(*it) == alphabet.end())
                unknown_this.insert(*it);
            }
          for (typename HfstTransitionGraphAlphabet::const_iterator it
                 = alphabet.begin();
               it != alphabet.end(); it++)
            {
              if (another.alphabet.find(*it) == another.alphabet.end())
                unknown_another.insert(*it);
            }
          
          /* No need to harmonize. */
          if (unknown_this.size() == 0 &&
              unknown_another.size() == 0) {
            return *this;
          }

          /* Expand the unknowns. */
          substituter subs_this(unknown_this);
          substitute(subs_this);

          substituter subs_another(unknown_another);
          another.substitute(subs_another);

          return *this;
        }
        

        /* -------------------------------
                Disjunction functions
           ------------------------------- */
      
      protected:
        /* Disjunct the transition of path \a spv pointed by \a it
           to state \a s. If the transition does not exist in the graph,
           it is created as well as its target state.

           @return The final state of path \a spv, when \a it is at end. */
        HfstState disjunct(const StringPairVector &spv, 
                           StringPairVector::const_iterator &it,
                           HfstState s)
        {
          // Path inserted, return the final state on this path
          if (it == spv.end()) {
            return s;
          }

          HfstTransitions tr = state_vector[s];
          bool transition_found=false;
          /* The target state of the transition followed or added */
          HfstState next_state; 

          // Find the transition
          // (Searching is slow?)
          for (typename HfstTransitions::iterator tr_it = tr.begin();
               tr_it != tr.end(); tr_it++)
            {
              C data = tr_it->get_transition_data();
              if (data.get_input_symbol().compare(it->first) == 0 &&
                  data.get_output_symbol().compare(it->second) == 0)
                {
                  transition_found=true;
                  next_state = tr_it->get_target_state();
                  break;
                }
            }

          // If not found, create the transition
          if (not transition_found)
            {
              next_state = add_state();
              HfstTransition <C> transition(next_state, it->first,
                                             it->second, 0);
              add_transition(s, transition);
            }

          // Advance to the next transition on path
          it++;
          return disjunct(spv, it, next_state);
        }
        
      public:
        
        /** @brief Disjunct this graph with a one-path graph 
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
          (const StringPairVector &spv, W weight) 
        {
          StringPairVector::const_iterator it = spv.begin();
          HfstState initial_state = 0;
          HfstState final_state = disjunct(spv, it, initial_state);

          // Set the weight of final state
          if (is_final_state(final_state)) 
            {
              float old_weight = get_final_weight(final_state);
              if (old_weight < weight) 
                return *this; /* The same path with smaller weight remains */
            }
          set_final_weight(final_state, weight);
          return *this;
        }

/* 	/\** @brief Determine whether this graph has input-epsilon cycles. */
/* 	 *\/ */
/* 	bool has_input_epsilon_cycles(void) */
/* 	{ */
/* 	            typedef std::map<HfstState,  */
/*           std::set<HfstTransition<C> > > */
/*           HfstStates; */
/*         HfstStates state_map; */

/* 	    std::set<HfstState> total_seen; */
/* 	    for (state_vector::iterator it = state_vector.begin(); */
/* 		 it != state_vector.end(); ++it) { */
/* 		if (total_seen.count(*it) != 0) { */
/* 		    continue; */
/* 		} */
		
/* 	    } */
/* 	} */
        
        friend class ConversionFunctions;
      };

    /** @brief An HfstTransitionGraph with transitions of type 
        HfstTropicalTransducerTransitionData and weight type float.
        
        This is probably the most useful kind of HfstTransitionGraph. */
    typedef HfstTransitionGraph <HfstTropicalTransducerTransitionData, float> 
      HfstBasicTransducer;

    typedef HfstTransitionGraph <HfstFastTransitionData, float> 
      HfstFastTransducer;

    template <class C, class W> class HfstSubstituteTransducer: 
    public HfstTransitionGraph<C, W> { };
    
  }
   
}

#endif // #ifndef _HFST_TRANSITION_GRAPH_H_
