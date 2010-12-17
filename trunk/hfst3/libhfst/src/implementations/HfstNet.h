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


    /* One implementation of template class C. */
    struct TransitionData {
      typedef std::string SymbolType;
      typedef float WeightType;
      SymbolType input_symbol;
      SymbolType output_symbol;
      WeightType weight;

      TransitionData() {
	input_symbol="";
	output_symbol="";
	weight=0;
      }

      TransitionData(const TransitionData &data) {
	input_symbol = data.input_symbol;
	output_symbol = data.output_symbol;
	weight = data.weight;
      }

      TransitionData(SymbolType isymbol,
		     SymbolType osymbol,
		     WeightType weight) {
	input_symbol=isymbol;
	output_symbol=osymbol;
	this->weight=weight;
      }

      void operator=(const TransitionData &data) {
	input_symbol = data.input_symbol;
	output_symbol = data.output_symbol;
	weight = data.weight;
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

    /* A transition consists of a target state and transition data
       represented by class C. 

       HfstTransition is not parametrized with class W, it is the user's 
       responsibility to use the same weight type in
       transitions and final states.*/
    template <class C> class HfstTransition_ 
      {
      protected:
	HfstState target_state;
	C transition_data;
      public:

        HfstTransition_(): target_state(0)
	  {}

	HfstTransition_(HfstState s, 
			typename C::SymbolType isymbol, 
			typename C::SymbolType osymbol, 
			typename C::WeightType weight):
	target_state(s), transition_data(isymbol, osymbol, weight)
	  {}

      HfstTransition_(const HfstTransition_<C> &another): 
	target_state(another.target_state), transition_data(another.transition_data) {}

	bool operator<(const HfstTransition_<C> &another) const {
	  if (target_state == another.target_state)
	    return (transition_data < another.transition_data);
	  return (target_state < another.target_state);
	}

	void operator=(const HfstTransition_<C> &another) const {
	  target_state = another.target_state;
	  transition_data = transition_data;
	}

	HfstState get_target_state() const {
	  return target_state;
	}

	const C & get_transition_data() const {
	  return transition_data;
	}

      };

    typedef HfstTransition_<TransitionData> HfstArc;

    /* An HfstNet contains a map, where each state is mapped
       to a set of that state's transitions (class C), and a map, where
       each final state is mapped to its final weight (class W). 
       Class W must use the weight type W. */
    template <class C, class W> class HfstNet 
      {
      protected:
	typedef std::map<HfstState, 
	  std::set<HfstTransition_<C> > >
	  HfstStateMap_;
	HfstStateMap_ state_map;
	std::map<HfstState,W> final_weight_map;
	std::set<typename C::SymbolType> alphabet;

      public:
	/** A set of transitions of a state in an HfstNet. */
	typedef std::set<HfstTransition_<C> > HfstTransitionSet;
	/** An iterator type that points to the map of states in the net. 
	    The value pointed by the iterator is of type 
	    std::pair<HfstState, HfstTransitionSet >. */
	typedef typename HfstStateMap_::iterator iterator;
	/** A const iterator type that points to the map of states in the net.
	    The value pointed by the iterator is of type 
	    std::pair<HfstState, HfstTransitionSet >. */
	typedef typename HfstStateMap_::const_iterator const_iterator;

	/** Create a transducer with one initial state that has state
	    number zero and is not a final state, i.e. an empty transducer. */
	HfstNet(void) {
	  state_map[0]=std::set<HfstTransition_ <C> >();
	}

	/** Create a deep copy of HfstNet \a net. */
	HfstNet(const HfstNet &net) {
	  state_map = net.state_map;
	  final_weight_map = net.final_weight_map;
	  alphabet = alphabet;
	}

	/** Create an HfstNet equivalent to HfstTransducer \a transducer. */
	HfstNet(const HfstTransducer &transducer) {
	  throw hfst::exceptions::FunctionNotImplementedException
	    ("HfstNet(const HfstTransducer &transducer)");
	}

	/** @brief Create an HfstNet as defined in AT&T format in istream \a is.
	    @pre \a is not at end, otherwise an exception is thrown. 
	    @note Multiple AT&T transducer definitions are separated with 
	    the line "--". */
	HfstNet(std::istream &is) {
	  throw hfst::exceptions::FunctionNotImplementedException
	    ("HfstNet(std::istream &is)");
	}

	/** Add a state \a s to this net. 
	    If the state already exists, it is not added again. */
	void add_state(HfstState s) {
	  if (state_map.find(s) == state_map.end())
	    state_map[s]=std::set<HfstTransition_ <C> >();
	}

	/** Add a transition \a transition to state \a s. 
	    If state \a s does not exist, it is created. */
	void add_transition(HfstState s, HfstTransition_<C> transition) {

	  C data = transition.get_transition_data();
	  add_state(s);
	  add_state(transition.get_target_state());
	  alphabet.insert(data.input_symbol);
	  alphabet.insert(data.output_symbol);
	  state_map[s].insert(transition);
	}

	/** Whether state \a s is final. */
	bool is_final_state(HfstState s) const {
	  return (final_weight_map.find(s) != final_weight_map.end());
	}

	/** Get the final weight of state \a s in this net. */
	W get_final_weight(HfstState s) {
	  if (final_weight_map.find(s) != final_weight_map.end())
	    return final_weight_map[s];
	}

	/** Set the final weight of state \a s in this net to \a weight. 
	    If the state does not exist, it is created. */
	void set_final_weight(HfstState s, const W & weight) {
	  final_weight_map[s] = weight;
	}

	/** Get an iterator to the beginning of the map of states in 
	    the net. */
	iterator begin() { return state_map.begin(); }

	/** Get a const iterator to the beginning of the map of states in 
	    the net. */
	const_iterator begin() const { return state_map.begin(); }

	/** Get an iterator to the end of the map of states in
	    the net. */
	iterator end() { return state_map.end(); }

	/** Get a const iterator to the end of the map of states in
	    the net. */
	const_iterator end() const { return state_map.end(); }

	/** Get the set of transitions of state \a s in this net. 
	    If the state does not exist, it is created. */
	std::set<HfstTransition_<C> > & operator[](HfstState s) {
	  return state_map[s];
	}	

	/** Write the net in AT&T format to ostream \a os.
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
	
	friend class hfst::HfstTransducer;
      };

    /** An HfstNet with transitions of type TransitionData. */
    typedef HfstNet <TransitionData, float> HfstFsm;
   


#ifdef FOO

      /** @brief Add \a transition to the transducer. */
      void add_transition(HfstTransition &transition);
      /** @brief Set state \a s final with weight \a weight. 	  
	  @todo change name to "set_final_state" */
      void add_final_state(HfstState s, float weight);

      /** @brief Get the number of start state in this
	  transducer. Returns always zero. */
      HfstState get_initial_state();
      /** @brief Whether state \a s is final. */
      bool is_final_state(HfstState s);
      /** @brief Get the final weight of state \a s.
	  @pre \a s is a final state. 
	  @see #get_initial_state */
      float get_final_weight(HfstState s);
      /** @brief get the biggest state number in the transducer.
	  @note The return value is not necessarily the smallest
	  available state number. */
      HfstState max_state_number() const;

      /** @brief Get an iterator to the first state in the transducer. */
      const_iterator begin() const;

      /** @brief Get an iterator that has just passed the last state 
	  in the transducer. */
      const_iterator end() const;

      void add_line(HfstState final_state, float final_weight); 
      void add_line(HfstState origin_state, HfstState target_state,
		    HfstState isymbol, HfstState osymbol,
		    float weight);

    public:
      bool has_no_lines() const;
      const InternalTransducerLineSet *get_lines() const;
      void swap_states(unsigned int s1, unsigned int s2);

    public:
      void read_number(FILE*);
      void read_symbol(FILE*, const std::string &epsilon_symbol);
      void print_number(FILE*, bool print_weights);
      void print_symbol(FILE*, bool print_weights);
      void print_number(std::ostream&, bool print_weights);
      void print_symbol(std::ostream&, bool print_weights);
      void substitute(const StringPair &sp, const StringPairSet &sps);
      void substitute(void (*func)(std::string &isymbol, std::string &osymbol) );   
      void substitute(const StringPair &sp, HfstInternalTransducer &transducer);
      void substitute(const std::string &old_symbol, const std::string &new_symbol,
		      bool input_side=true, bool output_side=true);
      void substitute(const StringPair &old_pair, const StringPair &new_pair); 
      void insert_freely(const StringPair &symbol_pair);
      
      void disjunct(const StringPairVector &spv);

      friend class HfstStateIterator;
      friend class HfstTransitionIterator;

#endif // FOO

  }
}
#endif

