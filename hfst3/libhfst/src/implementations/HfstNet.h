#ifndef _HFST_NET_H_
#define _HFST_NET_H_

#include <string>
#include <set>
#include "../HfstAlphabet.h"
#include "../HfstSymbolDefs.h"
#include "../HfstExceptions.h"
#include <cassert>
#include <iostream>

namespace hfst {
  class HfstTransducer;
}

/** @file HfstNet.h
    @brief Declaration of classes needed by HFST's 
    own transducer format. */

namespace hfst {
  namespace implementations {

    /** @brief The number of a state in an HfstNet. */
    typedef unsigned int HfstState;


    /** @brief One implementation of template class C in 
	HfstTransition. 

	@see HfstTransition HfstArc */
    struct TransitionData {
      /** @brief The input and output symbol type. */
      typedef std::string SymbolType;
      /** @brief The weight type. */
      typedef float WeightType;
      /** @brief The input symbol. */
      SymbolType input_symbol;
      /** @brief The output symbol. */
      SymbolType output_symbol;
      /** @brief The transition weight. */
      WeightType weight;

      /** @brief Create a TransitionData with empty input and output
	  strings and weight zero. */
      TransitionData() {
	input_symbol="";
	output_symbol="";
	weight=0;
      }

      /** @brief Create a deep copy of TransitionData \a data. */
      TransitionData(const TransitionData &data) {
	input_symbol = data.input_symbol;
	output_symbol = data.output_symbol;
	weight = data.weight;
      }

      /** @brief Create a TransitionData with input symbol \a 
	  isymbol, output symbol \a osymbol and weight \a weight. */
      TransitionData(SymbolType isymbol,
		     SymbolType osymbol,
		     WeightType weight) {
	input_symbol=isymbol;
	output_symbol=osymbol;
	this->weight=weight;
      }

      static bool is_epsilon(const SymbolType &symbol) {
	return (symbol.compare("@_EPSILON_SYMBOL_@") == 0);
      }
      static bool is_unknown(const SymbolType &symbol) {
	return (symbol.compare("@_UNKNOWN_SYMBOL_@") == 0);
      }
      static bool is_identity(const SymbolType &symbol) {
	return (symbol.compare("@_IDENTITY_SYMBOL_@") == 0);
      }

      /** @brief Whether this transition is less than transition 
	  \a another. */
      bool operator<(const TransitionData &another) const {
	if (input_symbol.compare(another.input_symbol) < 0)
	  return true;
	if (input_symbol.compare(another.input_symbol) > 0)
	  return false;
	if (output_symbol.compare(another.output_symbol) < 0)
	  return true;
	if (output_symbol.compare(another.output_symbol) > 0)
	  return false;
	return (weight < another.weight);
      }
    };

    /** @brief A transition that consists of a target state and 
	transition data represented by class C. 

       HfstTransition is not parametrized with class W, it is the user's 
       responsibility to use the same weight type in
       transitions and final states.

       The easiest way to use this template is to choose the 
       ready-templated HfstArc.

       @see HfstArc
    */
    template <class C> class HfstTransition_ 
      {
      protected:
	HfstState target_state;
	C transition_data;
      public:

	/** @brief Create an uninitialized transition. */
        HfstTransition_(): target_state(0)
	  {}

	/** @brief Create a transition leading to state \a s with input symbol
	    \a isymbol, output_symbol \a osymbol and weight \a weight. */
	HfstTransition_(HfstState s, 
			typename C::SymbolType isymbol, 
			typename C::SymbolType osymbol, 
			typename C::WeightType weight):
	target_state(s), transition_data(isymbol, osymbol, weight)
	  {}

	/** @brief Create a deep copy of transition \a another. */
      HfstTransition_(const HfstTransition_<C> &another): 
	target_state(another.target_state), transition_data(another.transition_data) {}

	/** @brief Whether this transition is less than transition \a
	    another. Needed for storing transitions in a set. */
	bool operator<(const HfstTransition_<C> &another) const {
	  if (target_state == another.target_state)
	    return (transition_data < another.transition_data);
	  return (target_state < another.target_state);
	}

	/** @brief Assign this transition the same value as transition 
	    \a another. */
	void operator=(const HfstTransition_<C> &another) const {
	  target_state = another.target_state;
	  transition_data = transition_data;
	}

	/** @brief Get the target state of the transition. */
	HfstState get_target_state() const {
	  return target_state;
	}

	/** @brief Get the transition data of the transition. */
	const C & get_transition_data() const {
	  return transition_data;
	}

      };

    /** @brief An HfstTransition with transition data of type
	TransitionData. 

	@see TransitionData */
    typedef HfstTransition_<TransitionData> HfstArc;

    /** @brief A simple transducer format.

       An HfstNet contains a map, where each state is mapped
       to a set of that state's transitions (class C), and a map, where
       each final state is mapped to its final weight (class W). 
       Class C must use the weight type W. 

       Probably the easiest way to use this template is to choose
       the ready-templated HfstFsm. HfstFsm is the implementation that is
       used as an example in this documentation.

       An example of creating a HfstFsm [foo:bar baz:baz] with weight 0.4
       from scratch:
\verbatim
  // Create an empty net
  // The net has initially one start state (number zero) that is not final
  HfstFsm net;
  // Add two states to the net
  net.add_state(1);
  net.add_state(2);
  // Create a transition [foo:bar] leading to state 1 with weight 0.1 ...
  HfstArc arc(1, "foo", "bar", 0.1);
  // ... and add it to state zero
  net.add_transition(0, arc);
  // Add a transition [baz:baz] with weight 0 from state 1 to state 2 
  net.add_transition(1, HfstArc(2, "baz", "baz", 0.0));
  // Set state 2 as final with weight 0.3
  net.set_final_weight(2, 0.3);
\endverbatim

       An example of iterating through a HfstFsm's states and transitions
       when printing it in AT&T format to stderr:
\verbatim
  // Go through all states
  for (HfstFsm::iterator it = net.begin();
       it != net.end(); it++)
    {
      // Go through the set of transitions in each state
      for (HfstFsm::HfstTransitionSet::iterator tr_it = it->second.begin();
	   tr_it != it->second.end(); tr_it++)
	{
	  TransitionData data = tr_it->get_transition_data();

	  fprintf(stderr, "%i\t%i\t%s\t%s\t%f\n",
		  it->first,
		  tr_it->get_target_state(),
		  data.input_symbol.c_str(),
		  data.output_symbol.c_str(),
		  data.weight
		  );
	}
      if (net.is_final_state(it->first))
	{
	  fprintf(stderr, "%i\t%f\n",
		  it->first,
		  net.get_final_weight(it->first));
	}
    }
\endverbatim

       @see #HfstFsm TransitionData */
    template <class C, class W> class HfstNet 
      {
      protected:
	typedef std::map<HfstState, 
	  std::set<HfstTransition_<C> > >
	  HfstStateMap_;
	HfstStateMap_ state_map;
	typedef std::map<HfstState,W> FinalWeightMap;
	FinalWeightMap final_weight_map;
	typedef std::set<typename C::SymbolType> HfstNetAlphabet;
	HfstNetAlphabet alphabet;

      public:
	/** @brief A set of transitions of a state in an HfstNet. */
	typedef std::set<HfstTransition_<C> > HfstTransitionSet;
	/** @brief An iterator type that points to the map of states in the net. 

	    The value pointed by the iterator is of type 
	    std::pair<HfstState, HfstTransitionSet >. */
	typedef typename HfstStateMap_::iterator iterator;
	/** @brief A const iterator type that points to the map of states in the net.

	    The value pointed by the iterator is of type 
	    std::pair<HfstState, HfstTransitionSet >. */
	typedef typename HfstStateMap_::const_iterator const_iterator;

	/** @brief Create a transducer with one initial state that has state
	    number zero and is not a final state, i.e. an empty transducer. */
	HfstNet(void) {
	  state_map[0]=std::set<HfstTransition_ <C> >();
	}

	/** @brief Create a deep copy of HfstNet \a net. */
	HfstNet(const HfstNet &net) {
	  state_map = net.state_map;
	  final_weight_map = net.final_weight_map;
	  alphabet = alphabet;
	}

	/** @brief Create an HfstNet equivalent to HfstTransducer \a transducer. */
	HfstNet(const HfstTransducer &transducer) {
	  throw hfst::exceptions::FunctionNotImplementedException
	    ("HfstNet(const HfstTransducer &transducer)");
	}

	/** @brief Add a state \a s to this net.
 
	    If the state already exists, it is not added again. */
	void add_state(HfstState s) {
	  if (state_map.find(s) == state_map.end())
	    state_map[s]=std::set<HfstTransition_ <C> >();
	}

	/** @brief Add a transition \a transition to state \a s. 

	    If state \a s does not exist, it is created. */
	void add_transition(HfstState s, HfstTransition_<C> transition) {

	  const C data = transition.get_transition_data();
	  add_state(s);
	  add_state(transition.get_target_state());
	  alphabet.insert(data.input_symbol);
	  alphabet.insert(data.output_symbol);
	  state_map[s].insert(transition);
	}

	/** @brief Whether state \a s is final. */
	bool is_final_state(HfstState s) const {
	  return (final_weight_map.find(s) != final_weight_map.end());
	}

	/** Get the final weight of state \a s in this net. */
	W get_final_weight(HfstState s) {
	  if (final_weight_map.find(s) != final_weight_map.end())
	    return final_weight_map[s];
	}

	/** @brief Set the final weight of state \a s in this net to \a weight. 

	    If the state does not exist, it is created. */
	void set_final_weight(HfstState s, const W & weight) {
	  final_weight_map[s] = weight;
	}

	/** @brief Get an iterator to the beginning of the map of states in 
	    the net. 

	    For an example, see #HfstNet */
	iterator begin() { return state_map.begin(); }

	/** @brief Get a const iterator to the beginning of the map of states in 
	    the net. */
	const_iterator begin() const { return state_map.begin(); }

	/** @brief Get an iterator to the end of the map of states in
	    the net. */
	iterator end() { return state_map.end(); }

	/** @brief Get a const iterator to the end of the map of states in
	    the net. */
	const_iterator end() const { return state_map.end(); }

	/** @brief Get the set of transitions of state \a s in this net. 

	    If the state does not exist, it is created. The created
	    state has an empty set of transitions. */
	std::set<HfstTransition_<C> > & operator[](HfstState s) {
	  return state_map[s];
	}	

	/** @brief Write the net in AT&T format to ostream \a os.
	    \a write_weights defines whether weights are printed. */
	void write_in_att_format(std::ostream &os, bool write_weights=true) 
	{
	  for (iterator it = begin(); it != end(); it++)
	    {
	      for (typename HfstTransitionSet::iterator tr_it
		     = it->second.begin();
		   tr_it != it->second.end(); tr_it++)
		{
		  C data = tr_it->get_transition_data();
		  
		  os <<  it->first << "\t" 
		     <<  tr_it->get_target_state() << "\t"
		     <<	 data.input_symbol.c_str() << "\t"
		     <<	 data.output_symbol.c_str();
		  if (write_weights)
		    os <<  "\t" << data.weight; 
		  os << "\n";
		}
	      if (is_final_state(it->first))
		{
		  os <<  it->first;
		  if (write_weights)
		    os << "\t" <<  get_final_weight(it->first);
		  os << "\n";
		}
	    }	  
	}

	/** @brief Create an HfstNet as defined in AT&T format in istream \a is.
	    @pre \a is not at end, otherwise an exception is thrown. 
	    @note Multiple AT&T transducer definitions are separated with 
	    the line "--". */
	static HfstNet read_in_att_format(std::istream &is, 
					  std::string epsilon_symbol=
					  std::string("@_EPSILON_SYMBOL_@")) {

	  HfstNet retval;
	  char line [255];
	  while(not is.getline(line,255).eof()) {

	    if (*line == '-') // transducer separator line is "--"
	      return retval;

	    // scan one line that can have a maximum of five fields
	    char a1 [100]; char a2 [100]; char a3 [100]; char a4 [100]; char a5 [100];
	    // how many fields could be parsed
	    int n = sscanf(line, "%s\t%s\t%s\t%s\t%s", a1, a2, a3, a4, a5);
	    
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
	      if (epsilon_symbol.compare(input_symbol) == 0)
		input_symbol="@_EPSILON_SYMBOL_@";
	      if (epsilon_symbol.compare(output_symbol) == 0)
		output_symbol="@_EPSILON_SYMBOL_@";
	      
	      HfstTransition_ <C> tr( atoi(a2), input_symbol, 
				      output_symbol, weight );
	      retval.add_transition( atoi(a1), tr );
	    }
	    
	    else  // line could not be parsed
	      throw hfst::exceptions::NotValidAttFormatException();       
	  }
	  return retval;
	}

	/** @brief Substitute \a old_symbol with \a new_symbol in 
	    all transitions. \a input_side and \a output_side define
	    whether the substitution is made on input and output sides. 

	    @todo Unknown and identity symbols must be handled correctly */
	void substitute(const std::string &old_symbol, 
			const std::string &new_symbol,
			bool input_side=true, 
			bool output_side=true) {

	  // If a symbol is substituted with itself, do nothing.
	  if (old_symbol.compare(new_symbol) == 0)
	    return;
	  // If the old symbol is not known to the transducer, do nothing.
	  if (alphabet.find(old_symbol) == alphabet.end())
	    return;

	  // Whether the substituting symbol is unknown to the transducer
	  bool is_new_symbol_unknown=false;
	  if (alphabet.find(new_symbol) == alphabet.end())
	    is_new_symbol_unknown=true;

	  // Remove the symbol to be substituted from the alphabet
	  // and insert to substituting symbol to the alphabet
	  alphabet.erase(old_symbol);
	  alphabet.insert(new_symbol);

	  // Go through all states
	  for (iterator it = begin(); it != end(); it++)
	    {

	      // The transitions that are substituted, i.e. removed
	      std::vector<typename HfstTransitionSet::iterator> 
		old_transitions;
	      // The substituting transitions that are added
	      HfstTransitionSet new_transitions;

	      // Go through all transitions
	      for (typename HfstTransitionSet::iterator tr_it
		     = it->second.begin();
		   tr_it != it->second.end(); tr_it++)
		{
		  const C data = tr_it->get_transition_data();

		  // Whether there is anything to substitute 
		  // in this transition
		  bool substitution_made=false;
		  std::string new_input_symbol;
		  std::string new_output_symbol;

		  if (input_side && 
		      data.input_symbol.compare(old_symbol) == 0) {
		    new_input_symbol = new_symbol;
		    substitution_made=true;
		  }
		  else
		    new_input_symbol=data.input_symbol;

		  if (output_side && 
		      data.output_symbol.compare(old_symbol) == 0) {
		    new_output_symbol = new_symbol;
		    substitution_made=true;
		  }
		  else
		    new_output_symbol=data.output_symbol;

		  // If there is something to substitute,
		  if (substitution_made) {
		    HfstTransition_ <C> new_transition
		      (tr_it->get_target_state(),
		       new_input_symbol,
		       new_output_symbol,
		       data.weight);

		    // schedule the old transition to be deleted and
		    // the new transition to be added
		    new_transitions.insert(new_transition);
		    old_transitions.push_back(tr_it);
		  }

		  // (one transition gone through)
		} 
	      // (all transitions in a state gone through)

	      // Remove the substituted transitions
	      for (typename std::vector<typename 
		     HfstTransitionSet::iterator>::iterator IT =
		     old_transitions.begin(); 
		   IT != old_transitions.end(); IT++) {
		it->second.erase(*IT);
	      }
	      // and add the substituting transitions
	      for (typename HfstTransitionSet::iterator IT 
		     = new_transitions.begin();
		   IT != new_transitions.end(); IT++) {
		it->second.insert(*IT);
	      }
	      // (all transitions in a state substituted)
	    }
	  // (all states handled)
	}

	// TODO:
	void substitute(const StringPair &sp, const StringPairSet &sps) {}
	void substitute(void (*func)(std::string &isymbol, std::string &osymbol) ) { }  
	void substitute(const StringPair &sp, HfstNet &transducer) {}
	void substitute(const StringPair &old_pair, const StringPair &new_pair) {} 

	/** @brief Insert freely any number of \a symbol_pair in 
	    the transducer. */
	void insert_freely(const StringPair &symbol_pair) {
	  return;
	}
      
	/** @brief Disjunct this transducer with a one-path transducer 
	    defined by string pair vector \a spv. */
	void disjunct(const StringPairVector &spv) {
	  return;
	}	

	friend class hfst::HfstTransducer;
      };

    /** @brief An HfstNet with transitions of type TransitionData and 
	weight type float.

	This is probably the most useful kind of HfstNet. */
    typedef HfstNet <TransitionData, float> HfstFsm;

  }
   
}
#endif // #ifndef _HFST_NET_H_


