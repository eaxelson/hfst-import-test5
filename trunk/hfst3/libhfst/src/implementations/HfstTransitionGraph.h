#ifndef _HFST_TRANSITION_GRAPH_H_
#define _HFST_TRANSITION_GRAPH_H_

/** @file HfstTransitionGraph.h
    @brief Class HfstTransitionGraph */

#include <cstdio>
#include <string>
#include <set>
#include <cassert>
#include <iostream>
#include <algorithm>

#include "../HfstSymbolDefs.h"
#include "../HfstExceptionDefs.h"
#include "../HfstDataTypes.h"
#include "../HarmonizeUnknownAndIdentitySymbols.h"
#include "ConvertTransducerFormat.h"
#include "HfstTransition.h"
#include "HfstTropicalTransducerTransitionData.h"
#include "HfstFastTransitionData.h"


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

    /** @brief A simple transition graph format that consists of
        states and transitions between those states.
    
    Probably the easiest way to use this template is to choose
    the implementations #HfstBasicTransducer
    (HfstTransitionGraph<HfstTropicalTransducerTransitionData>)
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
    template <class C> class HfstTransitionGraph 
      {
	
    // --- Datatypes and variables ---

      public:
        /** @brief Datatype for the states of a transition in a graph. */
        typedef std::vector<HfstTransition<C> > HfstTransitions;

    /** @brief Datatype for a symbol in a transition. */
    typedef typename C::SymbolType HfstSymbol;
    /** @brief Datatype for a symbol pair in a transition. */
    typedef std::pair<HfstSymbol, HfstSymbol> 
      HfstSymbolPair; 
    /** @brief A set of symbol pairs. */
    typedef std::set<HfstSymbolPair> HfstSymbolPairSet;
    /** @brief A vector of symbol pairs. */
    typedef std::vector<HfstSymbolPair> HfstSymbolPairVector;
    /** @brief Datatype for the alphabet of a graph. */
        typedef std::set<HfstSymbol> HfstTransitionGraphAlphabet;

      protected:
    /* Datatype for the states of a graph and their transitions.
       Each index of the vector is a state and the transitions 
       on that index are the transitions of that state. */
        typedef std::vector<HfstTransitions> HfstStates;
    /* States of the graph and their transitions. */
        HfstStates state_vector;

    /* Datatype for the final states and their weights in a graph. */
        typedef std::map<HfstState,typename C::WeightType> FinalWeightMap;
    /* The final states and their weights in the graph. */
        FinalWeightMap final_weight_map;

    /* The alphabet of the graph. */
        HfstTransitionGraphAlphabet alphabet;

      protected:
        /* @brief An iterator type that points to a state in a graph. 
        
	   The value pointed by the iterator is of type HfstTransitions. */
        typedef typename HfstStates::iterator iterator;

      public:
        /** @brief A const iterator type that points a state in a graph.
        
            The value pointed by the iterator is of type HfstTransitions. */
        typedef typename HfstStates::const_iterator const_iterator;


    // --------------------------------------------------------
    // --- Construction, assignment, copying and conversion ---
    // --------------------------------------------------------

        /** @brief Create a graph with one initial state that has state number
            zero and is not a final state, i.e. create an empty graph. */
        HfstTransitionGraph(void) {
          initialize_alphabet(alphabet);
      HfstTransitions tr;
          state_vector.push_back(tr);
        }

    /** @brief The assignment operator. */
    HfstTransitionGraph &operator=(const HfstTransitionGraph &graph)
      {
        if (this == &graph)
          return *this;
        state_vector = graph.state_vector;
        final_weight_map = graph.final_weight_map;
        alphabet = graph.alphabet;
        assert(alphabet.count(HfstSymbol()) == 0);
        return *this;
      }

        /** @brief Create a deep copy of HfstTransitionGraph \a graph. */
        HfstTransitionGraph(const HfstTransitionGraph &graph) {
          state_vector = graph.state_vector;
          final_weight_map = graph.final_weight_map;
          alphabet = graph.alphabet;
      assert(alphabet.count(HfstSymbol()) == 0);
        }

        /** @brief Create an HfstTransitionGraph equivalent to HfstTransducer 
            \a transducer. FIXME: move to a separate file */
        HfstTransitionGraph(const hfst::HfstTransducer &transducer) {
          HfstTransitionGraph<HfstTropicalTransducerTransitionData>
            *fsm = ConversionFunctions::
              hfst_transducer_to_hfst_basic_transducer(transducer);
          state_vector = fsm->state_vector;
          final_weight_map = fsm->final_weight_map;
          alphabet = fsm->alphabet;
          delete fsm;
        }


    // --------------------------------------------------
    // --- Initialization, optimization and debugging ---
    // --------------------------------------------------

      protected:
    /* Add epsilon, unknown and identity symbols to the alphabet 
       \a alpha. */
    void initialize_alphabet(HfstTransitionGraphAlphabet &alpha) {
      alpha.insert(C::get_epsilon());
      alpha.insert(C::get_unknown());
      alpha.insert(C::get_identity());
    }

    /* Check that all symbols that occur in the transitions of the graph
       are also in the alphabet. */
    bool check_alphabet() 
    {
          for (iterator it = begin(); it != end(); it++)
            {
              for (typename HfstTransitions::iterator tr_it
                     = it->begin();
                   tr_it != it->end(); tr_it++)
                {
                  C data = tr_it->get_transition_data();
                  
                  if(alphabet.find(data.get_input_symbol()) 
             == alphabet.end()) {
            return false;
          }
                  if(alphabet.find(data.get_output_symbol()) 
             == alphabet.end()) {
            return false;
          }
                }
            }
      return true;
    }

      public:
    /* Print the alphabet of the graph to standard error stream. */
    void print_alphabet() const 
    {
      for (typename HfstTransitionGraphAlphabet::const_iterator it 
         = alphabet.begin(); it != alphabet.end(); it++)
        {
          if (it != alphabet.begin())
        std::cerr << ", ";
          std::cerr << *it;
        }
      std::cerr << std::endl;
    }

      protected:
    /* Get the number of the \a symbol. */
    unsigned int get_symbol_number
      (const HfstSymbol &symbol) const {
      return C::get_number(symbol);
    }

    /* For internal optimization: Reserve space for 
       \a number_of_states states. */
    void initialize_state_vector
      (unsigned int number_of_states)
    {
      state_vector.reserve(number_of_states);
    }

    /* For internal optimization: Reserve space for
       \a number_of_transitions transitions for state number 
       \a state_number. */
    void initialize_transition_vector
      (unsigned int state_number, unsigned int number_of_transitions)
    {
      add_state(state_number);
      state_vector[state_number].reserve(number_of_transitions);
    }


    // -----------------------------------
    // ---------- The alphabet -----------
    // -----------------------------------

      public:
        /** @brief Explicitly add \a symbol to the alphabet of the graph.

            @note Usually the user does not have to take care of the alphabet
            of a graph. This function can be useful in some special cases. */
        void add_symbol_to_alphabet(const HfstSymbol &symbol) {
          alphabet.insert(symbol);
        }

    /** @brief Remove symbol \a symbol from the alphabet of the graph. 

        @note Use with care, removing symbols that occur in the transitions
        of the graph can have unexpected results. */
    void remove_symbol_from_alphabet(const HfstSymbol &symbol) {
      alphabet.erase(symbol);
    }

    /** @brief Same as #add_symbol_to_alphabet for each symbol in
        \a symbols. */
    void add_symbols_to_alphabet(const HfstSymbolPairSet &symbols)
    {
      for (typename HfstSymbolPairSet::const_iterator it = symbols.begin();
           it != symbols.end(); it++)
        {
          alphabet.insert(it->first);
          alphabet.insert(it->second);
        }
    }

    /* Remove all symbols that are given in \a symbols but do not occur 
       in transitions of the graph from its alphabet. */
    void prune_alphabet_after_substitution(const std::set<unsigned int> &symbols)
    {
      if (symbols.size() == 0)
	return;

      std::vector<bool> symbols_found;
      symbols_found.resize
	(C::get_max_number()+1, false);

      // Go through all transitions
      for (iterator it = begin(); it != end(); it++)
	{
	  for (typename HfstTransitions::iterator tr_it
		 = it->begin();
	       tr_it != it->end(); tr_it++)
	    {
	      const C & data = tr_it->get_transition_data();
	      symbols_found.at(data.get_input_number()) = true;
	      symbols_found.at(data.get_output_number()) = true;
	    }
	}

      // Remove symbols in \a symbols from the alphabet if they did not
      // occur in any transitions
      for (std::set<unsigned int>::const_iterator it = symbols.begin();
	   it != symbols.end(); it++)
	{
	  if (! symbols_found.at(*it))
	    alphabet.erase(C::get_symbol(*it));
	}
      
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

        /** @brief Get the set of HfstSymbols in the alphabet 
            of the graph. 
        
            The HfstSymbols do not necessarily occur in any transitions
            of the graph. Epsilon, unknown and identity \link 
            hfst::String symbols\endlink are always included in the alphabet. */
        const HfstTransitionGraphAlphabet &get_alphabet() const {
          return alphabet;
        }



    // ----------------------------------------------------------------
    // --- Adding states and transitions and iterating through them ---
    // ----------------------------------------------------------------

        /** @brief Add a new state to this graph and return its number.
        
            @return The next (smallest) free state number. */
        HfstState add_state(void) {
      HfstTransitions tr;
      state_vector.push_back(tr);
      return state_vector.size()-1;
    }

        /** @brief Add a state \a s to this graph.
 
            If the state already exists, it is not added again.
        All states with state number smaller than \a s are also
        added to the graph if they did not exist before.
            @return \a s*/
        HfstState add_state(HfstState s) {
      while(state_vector.size() <= s) {
        HfstTransitions tr;
        state_vector.push_back(tr);
      }
          return s;
        }

    /** @brief Get the biggest state number in use. */
    HfstState get_max_state() const {
      return state_vector.size()-1;
    }

        /** @brief Add a transition \a transition to state \a s. 

            If state \a s does not exist, it is created. */
    void add_transition(HfstState s, const HfstTransition<C> & transition,
			bool add_symbols_to_alphabet=true) {

          C data = transition.get_transition_data();

          add_state(s);
          add_state(transition.get_target_state());
	  if (add_symbols_to_alphabet) {
	    alphabet.insert(data.get_input_symbol());
	    alphabet.insert(data.get_output_symbol());
	  }
          state_vector[s].push_back(transition);
    }

        /** @brief Whether state \a s is final. 
        FIXME: return positive infinity instead if not final. */
        bool is_final_state(HfstState s) const {
          return (final_weight_map.find(s) != final_weight_map.end());
        }

        /** Get the final weight of state \a s in this graph. */
        typename C::WeightType get_final_weight(HfstState s) const {
          if (final_weight_map.find(s) != final_weight_map.end())
            return final_weight_map.find(s)->second;
          HFST_THROW(StateIsNotFinalException);
        }

        /** @brief Set the final weight of state \a s in this graph 
            to \a weight. 

            If the state does not exist, it is created. */
        void set_final_weight(HfstState s, 
                  const typename C::WeightType & weight) {
      add_state(s);
          final_weight_map[s] = weight;
        }
    
        /** @brief Sort the arcs of this transducer according to input and
            output symbols. */
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

        /** @brief Get an iterator to the beginning of the states in 
            the graph. 

            For an example, see #HfstTransitionGraph */
        iterator begin() { return state_vector.begin(); }

        /** @brief Get a const iterator to the beginning of 
            states in the graph. */
        const_iterator begin() const { return state_vector.begin(); }

        /** @brief Get an iterator to the end of states (last state + 1) 
        in the graph. */
        iterator end() { return state_vector.end(); }

        /** @brief Get a const iterator to the end of states (last state + 1)
        in the graph. */
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


    // --------------------------------------------------
    // -----   Reading and writing in AT&T format   -----
    // --------------------------------------------------

      protected:
        /* Change state numbers s1 to s2 and vice versa. */
        void swap_state_numbers(HfstState s1, HfstState s2) {
	  
	  HfstTransitions s1_copy = state_vector[s1];
	  state_vector[s1] = state_vector[s2];
	  state_vector[s2] = s1_copy;
	  
          // ----- Go through all states -----
          for (iterator it = begin(); it != end(); it++)
            {
	      // Go through all transitions
              for (unsigned int i=0; i < it->size(); i++)
                {
		  HfstTransition<C> &tr_it = it->operator[](i);
		  
		  HfstState new_target=tr_it.get_target_state();
		  if (tr_it.get_target_state() == s1)
		    new_target = s2;
		  if (tr_it.get_target_state() == s2)
		    new_target = s1;
		  
		  if (new_target != tr_it.get_target_state())
		    {
		      HfstTransition<C> tr
			(new_target,
			 tr_it.get_input_symbol(),
			 tr_it.get_output_symbol(),
			 tr_it.get_weight());
		      
		      it->operator[](i) = tr;
		    }
		  
		} // all transitions gone through
	      
	    } // ----- all states gone through -----
	  
	  // Swap final states, if needed
	  typename FinalWeightMap::iterator s1_it = final_weight_map.find(s1);
	  typename FinalWeightMap::iterator s2_it = final_weight_map.find(s2);
	  typename FinalWeightMap::iterator end_it = final_weight_map.end();

	  if (s1_it != end_it && s2_it != end_it) {
	    typename C::WeightType s1_weight = s1_it->second;
	    final_weight_map[s1] = s2_it->second;
	    final_weight_map[s2] = s1_weight;
	  }
	  if (s1_it != end_it) {
	    typename C::WeightType w = s1_it->second;
	    final_weight_map.erase(s1);
	    final_weight_map[s2] = w;
	  }
	  if (s2_it != end_it) {
	    typename C::WeightType w = s2_it->second;
	    final_weight_map.erase(s2);
	    final_weight_map[s1] = w;
	  }

	  return;
	  
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

      public:

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


    // ----------------------------------------------
    // -----       Substitution functions       -----
    // ----------------------------------------------

      protected:

    /* A function that performs in-place-substitution in the graph. */

	void substitute_(const HfstSymbol &old_symbol, 
			 const HfstSymbol &new_symbol,
			 bool input_side=true, 
			 bool output_side=true)
	{
          // ----- Go through all states -----
          for (iterator it = begin(); it != end(); it++)
            {
	      // Go through all transitions
              for (unsigned int i=0; i < it->size(); i++)
                {
		  HfstTransition<C> &tr_it = it->operator[](i);
		  
		  // The substituting input and output symbols for the 
		  // current transition.
		  HfstSymbol substituting_input_symbol
		    = tr_it.get_input_symbol();
		  HfstSymbol substituting_output_symbol
		    = tr_it.get_output_symbol();
		  
		  // Whether a substitution will be performed.
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
		  
		  // If a substitution is to be performed,
		  if (substitution_made) {
		    
		    add_symbol_to_alphabet(new_symbol);
		    
		    // change the current transition accordingly.
		    HfstTransition<C> tr
		      (tr_it.get_target_state(),
		       substituting_input_symbol,
		       substituting_output_symbol,
		       tr_it.get_weight());
		    
		    it->operator[](i) = tr;
		  }
		  
		} // all transitions gone through
	      
	    } // ----- all states gone through -----
	  
	  return;
	}
	
	/* A function that performs in-place-substitution in the graph. */
	void substitute_(const HfstSymbolPair &old_sp, 
			 const HfstSymbolPairSet &new_sps)
	{
	  unsigned int old_input_number = C::get_number(old_sp.first);
	  unsigned int old_output_number = C::get_number(old_sp.second);
	  
	  // Whether any substitution was performed
	  bool substitution_performed=false;
	  
          // ----- Go through all states -----
          for (iterator it = begin(); it != end(); it++)
            {
	      // The transitions to be added to the current state
	      HfstTransitions new_transitions;
	      
	      // Go through all transitions of the current state
              for (unsigned int i=0; i < it->size(); i++)
                {
		  HfstTransition<C> &tr_it = it->operator[](i);
		  
		  // If a match was found, substitute:
		  if (tr_it.get_input_number() == old_input_number &&
		      tr_it.get_output_number() == old_output_number)
		    {
		      substitution_performed=true;
		      
		      // change the current transition so that it is equivalent
		      // to the first substituting transition in new_sps
		      typename HfstSymbolPairSet::const_iterator IT 
			= new_sps.begin();
		      
		      HfstTransition<C> tr
			(tr_it.get_target_state(),
			 C::get_number(IT->first),
			 C::get_number(IT->second),
			 tr_it.get_weight(),
			 true);
		      
		      it->operator[](i) = tr;
		      
		      // and schedule the rest of the substituting transitions
		      // in new_sps to be added to the current state.
		      while (IT != new_sps.end())
			{
			  HfstTransition<C> TR
			    (tr_it.get_target_state(),
			     C::get_number(IT->first),
			     C::get_number(IT->second),
			     tr_it.get_weight(),
			     true);
			  
			  new_transitions.push_back(TR);
			  IT++;
			}
		      
		    } // (substitution and scheduling done)       
		  
		} // (all transitions of a state gone through)
	      
	      // Add the new transitions to the current state
	      for (typename HfstTransitions
		     ::const_iterator NIT = new_transitions.begin();
		   NIT != new_transitions.end(); NIT++)
		{
		  it->push_back(*NIT);
		}
	      
	    } // ( ----- all states in the graph gone through ----- )
	  
	  // If at least one substitution was performed, add all the
	  // symbols in the substituting transitions to the alphabet of
	  // the graph.
	  if (substitution_performed) {
	    add_symbols_to_alphabet(new_sps);
	  }
	  
	  // Remove symbols that were removed because of substitutions
	  // (or didn't occur in the graph in the first place)
	  std::set<unsigned int> syms;
	  for (typename HfstSymbolPairSet::const_iterator it = new_sps.begin();
	       it != new_sps.end(); it++) {
	    syms.insert(C::get_number(it->first));
	    syms.insert(C::get_number(it->second));
	  }
	  prune_alphabet_after_substitution(syms);
	  
	  return;
    }
    
    /* A function that performs in-place-substitution in the graph. */
    void substitute_(bool (*func)
             (const HfstSymbolPair &sp, HfstSymbolPairSet &sps))
    {
          // ----- Go through all states. -----
          for (iterator it = begin(); it != end(); it++)
            {
          // The transitions to be added to the current state.
          HfstTransitions new_transitions;

          // Go through all transitions.
              for (unsigned int i=0; i < it->size(); i++)
                {
          HfstTransition<C> &tr_it = it->operator[](i);

          HfstSymbolPair transition_symbol_pair
            (tr_it.get_input_symbol(),
             tr_it.get_output_symbol());
          HfstSymbolPairSet substituting_transitions;
          
          // If a substitution is to be performed,
          if ((*func)(transition_symbol_pair, 
                  substituting_transitions))
            {          
              // change the transition to the first element
              // in new_sps
              typename HfstSymbolPairSet::const_iterator IT 
            = substituting_transitions.begin();

              if (not C::is_valid_symbol(IT->first) ||
              not C::is_valid_symbol(IT->second) )
            HFST_THROW_MESSAGE
              (EmptyStringException,
               "HfstTransitionGraph::substitute");
              
              HfstTransition<C> tr
            (tr_it.get_target_state(),
             IT->first,
             IT->second,
             tr_it.get_weight());
              
              it->operator[](i) = tr;

              add_symbol_to_alphabet(IT->first);
              add_symbol_to_alphabet(IT->second);

              // and schedule the rest of the elements in new_sps
              // to be added to this state.
              while (IT != substituting_transitions.end())
            {
              
              if (not C::is_valid_symbol(IT->first) ||
                  not C::is_valid_symbol(IT->second) )
                HFST_THROW_MESSAGE
                  (EmptyStringException,
                   "HfstTransitionGraph::substitute");
              
              HfstTransition<C> TR
                (tr_it.get_target_state(),
                 IT->first,
                 IT->second,
                 tr_it.get_weight());
              
              new_transitions.push_back(TR);
              
              add_symbol_to_alphabet(IT->first);
              add_symbol_to_alphabet(IT->second);
              
              IT++;
            }

            } // Substitution and scheduling performed.   

        } // All transitions gone through.

          // Add the new transitions.
          for (typename HfstTransitions
             ::const_iterator NIT = new_transitions.begin();
           NIT != new_transitions.end(); NIT++)
        {
          it->push_back(*NIT);
        }

        } // ----- All states gone through. -----
      
      return;
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
          substitute(const HfstSymbol &old_symbol, 
                     const HfstSymbol  &new_symbol,
                     bool input_side=true, 
                     bool output_side=true) {

      if (not C::is_valid_symbol(old_symbol) || 
          not C::is_valid_symbol(new_symbol) ) {
        HFST_THROW_MESSAGE
          (EmptyStringException,
           "HfstTransitionGraph::substitute"); }

          // If a symbol is substituted with itself, do nothing.
          if (old_symbol == new_symbol)
            return *this;
          // If the old symbol is not known to the graph, do nothing.
          if (alphabet.find(old_symbol) == alphabet.end())
            return *this;

          // Remove the symbol to be substituted from the alphabet
          // if the substitution is made on both sides.
          if (input_side && output_side) {
            /* Special symbols are always included in the alphabet */
            if (not is_epsilon(old_symbol) && 
                not is_unknown(old_symbol) &&
                not is_identity(old_symbol)) {
              alphabet.erase(old_symbol); }
          }
          // Insert the substituting symbol to the alphabet.
          alphabet.insert(new_symbol);

      substitute_(old_symbol, new_symbol, input_side, output_side);

          return *this;
        }

        /** @brief Substitute all transitions \a sp with a set of transitions
            \a sps. */
        HfstTransitionGraph &substitute
          (const HfstSymbolPair &sp, const HfstSymbolPairSet &sps) 
      {
        if (not C::is_valid_symbol(sp.first) || 
        not C::is_valid_symbol(sp.second) ) {
          HFST_THROW_MESSAGE
        (EmptyStringException,
         "HfstTransitionGraph::substitute"); }

        for (typename HfstSymbolPairSet::const_iterator it = sps.begin();
         it != sps.end(); it++)
          {
        if (not C::is_valid_symbol(it->first) || 
            not C::is_valid_symbol(it->second) ) {
          HFST_THROW_MESSAGE
            (EmptyStringException,
             "HfstTransitionGraph::substitute"); }
          }
        
        substitute_(sp, sps);

          return *this;
        }
  
        /** @brief Substitute all transitions \a old_pair with 
            \a new_pair. */
        HfstTransitionGraph &substitute
          (const HfstSymbolPair &old_pair, 
           const HfstSymbolPair &new_pair) 
        {
      if (not C::is_valid_symbol(old_pair.first) || 
          not C::is_valid_symbol(new_pair.first) ||
          not C::is_valid_symbol(old_pair.second) || 
          not C::is_valid_symbol(new_pair.second) ) {
        HFST_THROW_MESSAGE
          (EmptyStringException,
           "HfstTransitionGraph::substitute"); }

      StringPairSet new_pair_set;
      new_pair_set.insert(new_pair);
      substitute_(old_pair, new_pair_set);
    
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
                     (const HfstSymbolPair &sp, HfstSymbolPairSet &sps) ) 
        { 
      substitute_(func);
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
          typename C::WeightType weight;
          
          substitution_data(HfstState origin, 
                            HfstState target,
                            typename C::WeightType weight)
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


        /* ----------------------------
              Insert freely functions
           ---------------------------- */


        /** @brief Insert freely any number of \a symbol_pair in 
            the graph with weight \a weight. */
        HfstTransitionGraph &insert_freely
          (const HfstSymbolPair &symbol_pair, typename C::WeightType weight) 
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
        HfstSymbol marker_this = C::get_marker(alphabet);
        HfstSymbol marker_graph = C::get_marker(alphabet);
        HfstSymbol marker = marker_this;
        if (marker_graph > marker)
          marker = marker_graph;

            HfstSymbolPair marker_pair(marker, marker);
            insert_freely(marker_pair, 0);
            substitute(marker_pair, graph);
            alphabet.erase(marker); // TODO: fix

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
        HfstTransitionGraph &harmonize(HfstTransitionGraph &another) 
      {
        HarmonizeUnknownAndIdentitySymbols foo(*this, another);
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
          (const StringPairVector &spv, typename C::WeightType weight) 
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
        

    // --- Friends ---

        friend class ConversionFunctions;
	friend class hfst::HarmonizeUnknownAndIdentitySymbols;
      };
   
    /** @brief An HfstTransitionGraph with transitions of type 
    HfstTropicalTransducerTransitionData and weight type float.
    
    This is probably the most useful kind of HfstTransitionGraph. */
    typedef HfstTransitionGraph <HfstTropicalTransducerTransitionData> 
      HfstBasicTransducer;
    
    /** @brief A specialization for faster conversion. */
    typedef HfstTransitionGraph <HfstFastTransitionData> 
      HfstFastTransducer;

 
  }
   
}

#endif // #ifndef _HFST_TRANSITION_GRAPH_H_
