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

  /** @brief A namespace for all code that forms a bridge between
      backend libraries and HFST or is not else directly accessible
      through the HFST API. */
  namespace implementations {

    /** @brief The number of a state in an HfstNet. */
    typedef unsigned int HfstState;

    // Needed by class TransitionData
    struct string_comparison {
      bool operator() (const std::string &str1, const std::string &str2) const {
	return (str1.compare(str2) < 0);
      }
    };

    /** @brief One implementation of template class C in 
	HfstTransition. 

	A TransitionData has an input symbol and an output symbol of type
	SymbolType (string) and a weight of type WeightType (float).

	\internal Actually a TransitionData has an input and an output number
	of type unsigned int, but this implementation is hidden from the user.
	The class has two static maps and functions that take care of conversion
	between strings and internal numbers.

	@see HfstTransition HfstArc */
    class TransitionData {
    public:
      /** @brief The input and output symbol type. */
      typedef std::string SymbolType;
      /** @brief The weight type. */
      typedef float WeightType;

      typedef std::map<unsigned int, SymbolType> Number2SymbolMap;
      typedef std::map<SymbolType, unsigned int, string_comparison> Symbol2NumberMap;

    public: /* Fix this */
      /* Maps that contain information of the mappings between strings and numbers */
      static Number2SymbolMap number2symbol_map;
      static Symbol2NumberMap symbol2number_map;
      /* Next free number */
      static unsigned int max_number;

    protected:
      /* Get the symbol that is mapped as number */
      static std::string get_symbol(unsigned int number) {
	Number2SymbolMap::iterator it = number2symbol_map.find(number);
	if (it == number2symbol_map.end()) {
	  fprintf(stderr, "ERROR: TransitionData::get_symbol(unsigned int number) "
		          "number is not mapped to any symbol\n");
	  throw hfst::exceptions::HfstInterfaceException();
	}
	return it->second;
      }

      /* Get the number that is used to represent the symbol */
      static unsigned int get_number(const std::string &symbol) {
	Symbol2NumberMap::iterator it = symbol2number_map.find(symbol);
	if (it == symbol2number_map.end()) {
	  max_number++;
	  symbol2number_map[symbol] = max_number;
	  number2symbol_map[max_number] = symbol;
	  return max_number;
	}
	return it->second;
      }

    private:
      /* The actual transition data */
      unsigned int input_number;
      unsigned int output_number;
      WeightType weight;

    public:

      /** @brief Create a TransitionData with epsilon input and output
	  strings and weight zero. */
    TransitionData(): input_number(0), output_number(0), weight(0) {}

      /** @brief Create a deep copy of TransitionData \a data. */
      TransitionData(const TransitionData &data) {
	input_number = data.input_number;
	output_number = data.output_number;
	weight = data.weight;
      }

      /** @brief Create a TransitionData with input symbol \a 
	  isymbol, output symbol \a osymbol and weight \a weight. */
      TransitionData(SymbolType isymbol,
		     SymbolType osymbol,
		     WeightType weight) {
	input_number = get_number(isymbol);
	output_number = get_number(osymbol);
	this->weight = weight;
      }

      SymbolType get_input_symbol() {
	return get_symbol(input_number);
      }

      SymbolType get_output_symbol() {
	return get_symbol(output_number);
      }

      WeightType get_weight() {
	return weight;
      }

      /* Are these needed? */
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
	  \a another. 

	  /internal is it too slow if strin comparison is used instead?
      */
      bool operator<(const TransitionData &another) const {
	if (input_number < another.input_number )
	  return true;
	if (input_number > another.input_number)
	  return false;
	if (output_number < another.output_number)
	  return true;
	if (output_number > another.output_number)
	  return false;
	return (weight < another.weight);
      }

      friend class Number2SymbolMapInitializer;
      friend class Symbol2NumberMapInitializer;
    };

    // Initialization of static members in class TransitionData..
    class Number2SymbolMapInitializer {
    public:
      Number2SymbolMapInitializer(TransitionData::Number2SymbolMap &map) {
	map[0] = std::string("@_EPSILON_SYMBOL_@");
	map[1] = std::string("@_UNKNOWN_SYMBOL_@");
	map[2] = std::string("@_IDENTITY_SYMBOL_@");
      }
    };

    class Symbol2NumberMapInitializer {
    public:
      Symbol2NumberMapInitializer(TransitionData::Symbol2NumberMap &map) {
	map["@_EPSILON_SYMBOL_@"] = 0;
	map["@_UNKNOWN_SYMBOL_@"] = 1;
	map["@_IDENTITY_SYMBOL_@"] = 2;
      }
    };

    TransitionData::Number2SymbolMap TransitionData::number2symbol_map;
    Number2SymbolMapInitializer dummy1(TransitionData::number2symbol_map);

    TransitionData::Symbol2NumberMap TransitionData::symbol2number_map;
    Symbol2NumberMapInitializer dummy2(TransitionData::symbol2number_map);

    unsigned int TransitionData::max_number=2;
    // ..initialization done


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
	target_state(another.target_state), transition_data(another.transition_data) 
	  {}

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
	unsigned int max_state;

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
        HfstNet(void): max_state(0) {
	  initialize_alphabet(alphabet);
	  state_map[0]=std::set<HfstTransition_ <C> >();
	}

	/** @brief Create a deep copy of HfstNet \a net. */
        HfstNet(const HfstNet &net): max_state(net.max_state) {
	  state_map = net.state_map;
	  final_weight_map = net.final_weight_map;
	  alphabet = alphabet;
	}

	/** @brief Create an HfstNet equivalent to HfstTransducer \a transducer. */
	HfstNet(const HfstTransducer &transducer) {
	  initialize_alphabet(alphabet);
	  throw hfst::exceptions::FunctionNotImplementedException
	    ("HfstNet(const HfstTransducer &transducer)");
	}

	void initialize_alphabet(HfstNetAlphabet &alpha) {
	  alpha.insert("@_EPSILON_SYMBOL_@");
	  alpha.insert("@_UNKNOWN_SYMBOL_@");
	  alpha.insert("@_IDENTITY_SYMBOL_@");
	}

	/** @brief Add a symbol to the alphabet of the transducer. */
	void add_symbol_to_alphabet(const std::string &symbol) {
	  alphabet.insert(symbol);
	}

	/** @brief Remove all symbols that do not occur in transitions of
	    the transducer from its alphabet. */
	void prune_alphabet() {

	  // Which symbols occur in the transducer
	  HfstNetAlphabet symbols_found;
	  initialize_alphabet(symbols_found); /* special symbols are always known */

	  for (iterator it = begin(); it != end(); it++)
	    {
	      for (typename HfstTransitionSet::iterator tr_it
		     = it->second.begin();
		   tr_it != it->second.end(); tr_it++)
		{
		  C data = tr_it->get_transition_data();
		  
		  symbols_found.insert(data.get_input_symbol());
		  symbols_found.insert(data.get_output_symbol());
		}
	    }
	  
	  // Which symbols in the transducer's alphabet did not occur in 
	  // the transducer
	  HfstNetAlphabet symbols_not_found;

	  for (typename HfstNetAlphabet::iterator it = alphabet.begin();
	       it != alphabet.end(); it++) 
	    {
	      if (symbols_found.find(*it) == symbols_found.end())
		symbols_not_found.insert(*it);
	    }

	  // Remove the symbols that did not occur in the transducer
	  // from its alphabet
	  for (typename HfstNetAlphabet::iterator it = symbols_not_found.begin();
	       it != symbols_not_found.end(); it++)
	    {
	      alphabet.erase(*it);
	    }
	}

	/** @brief Add a state \a s to this net.
 
	    If the state already exists, it is not added again. */
	void add_state(HfstState s) {
	  if (state_map.find(s) == state_map.end())
	    state_map[s]=std::set<HfstTransition_ <C> >();
	  if (max_state < s)
	    max_state=s;
	}

	/** @brief Add a transition \a transition to state \a s. 

	    If state \a s does not exist, it is created. */
	void add_transition(HfstState s, HfstTransition_<C> transition) {

	  C data = transition.get_transition_data();
	  add_state(s);
	  add_state(transition.get_target_state());
	  alphabet.insert(data.get_input_symbol());
	  alphabet.insert(data.get_output_symbol());
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
	  if (s > max_state)
	    max_state=s;
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
	  if (s > max_state)
	    max_state=s;
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
		     <<	 data.get_input_symbol().c_str() << "\t"
		     <<	 data.get_output_symbol().c_str();
		  if (write_weights)
		    os <<  "\t" << data.get_weight(); 
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
	    char a1 [100]; char a2 [100]; char a3 [100]; 
	    char a4 [100]; char a5 [100];
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
		  C data = tr_it->get_transition_data();

		  // Whether there is anything to substitute 
		  // in this transition
		  bool substitution_made=false;
		  std::string new_input_symbol;
		  std::string new_output_symbol;

		  if (input_side && 
		      data.get_input_symbol().compare(old_symbol) == 0) {
		    new_input_symbol = new_symbol;
		    substitution_made=true;
		  }
		  else
		    new_input_symbol=data.get_input_symbol();

		  if (output_side && 
		      data.get_output_symbol().compare(old_symbol) == 0) {
		    new_output_symbol = new_symbol;
		    substitution_made=true;
		  }
		  else
		    new_output_symbol=data.get_output_symbol();

		  // If there is something to substitute,
		  if (substitution_made) {
		    HfstTransition_ <C> new_transition
		      (tr_it->get_target_state(),
		       new_input_symbol,
		       new_output_symbol,
		       data.get_weight());

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

      protected:
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

	/* Add a copy of \a transducer with epsilon transitions between 
	   states and with weight as defined in \a sub. */
	void add_substitution(substitution_data &sub, HfstNet &transducer) {

	  // Epsilon transition to initial state of \a transducer
	  max_state++;
	  HfstTransition_ <C> epsilon_transition
	    (max_state, "@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@", 
	     sub.weight);
	  add_transition(sub.origin_state, epsilon_transition);

	  /* Offset between state numbers */
	  unsigned int offset = max_state;

	  // Copy \a transducer
	  for (iterator it = transducer.begin(); 
	       it != transducer.end(); it++)
	    {
	      for (typename HfstTransitionSet::iterator tr_it
		     = it->second.begin();
		   tr_it != it->second.end(); tr_it++)
		{
		  C data = tr_it->get_transition_data();
		  
		  HfstTransition_ <C> transition
		    (tr_it->get_target_state() + offset, 
		     data.get_input_symbol(),
		     data.get_output_symbol(),
		     data.get_weight());

		  add_transition(it->first + offset, transition);
		}
	    }

	  // Epsilon transitions from final states of \a transducer
	  for (typename FinalWeightMap::iterator it 
		 = transducer.final_weight_map.begin();
	       it != transducer.final_weight_map.end(); it++)
	    {
	      HfstTransition_ <C> epsilon_transition
		(sub.target_state, "@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@",
		 it->second);
	      add_transition(it->first + offset, epsilon_transition);
	    }
	}


      public:

	/** @brief Substitute all transitions \a old_symbol : \a new_symbol
	    with a copy of \a transducer.

	    Copies of \a transducer are attached to this transducer with
	    epsilon transitions. 
	    
	    The weights of the transitions to be substituted are copied
	    to epsilon transitions leaving from the source state of
	    the transitions to be substituted to the initial state
	    of a copy of \a transducer.

	    The final weights in \a 
	    transducer are copied to epsilon transitions leading from
	    the final states (after substitution non-final states)
	    of \a transducer to target states of transitions
	    \a old_symbol : \a new_symbol (that are substituted)
	    in this transducer.
	    
	    @pre This transducer and \a transducer are harmonized.
	*/
	void substitute(const StringPair &sp, HfstNet &transducer) {
	  
	  // If neither symbol to be substituted is known to the transducer,
	  // do nothing.
	  if (alphabet.find(sp.first) == alphabet.end() && 
	      alphabet.find(sp.second) == alphabet.end())
	    return;

	  // Where the substituting copies of \a transducer
	  // are inserted (source state, target state, weight)
	  std::vector<substitution_data> substitutions;

	  // Go through all states
	  for (iterator it = begin(); it != end(); it++)
	    {

	      // The transitions that are substituted, i.e. removed
	      std::vector<typename HfstTransitionSet::iterator> 
		old_transitions;

	      // Go through all transitions
	      for (typename HfstTransitionSet::iterator tr_it
		     = it->second.begin();
		   tr_it != it->second.end(); tr_it++)
		{
		  C data = tr_it->get_transition_data();

		  // Whether there is anything to substitute 
		  // in this transition
		  if (data.get_input_symbol().compare(sp.first) == 0 &&
		      data.get_output_symbol().compare(sp.second) == 0) 
		    {
		      // schedule a substitution
		      substitutions.push_back(substitution_data
					      (it->first, 
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
		     HfstTransitionSet::iterator>::iterator IT =
		     old_transitions.begin(); 
		   IT != old_transitions.end(); IT++) {
		it->second.erase(*IT);
	      }

	    }
	  // (all states gone trough)

	  // Add the substitutions
	  for (typename std::vector<substitution_data>::iterator IT 
		 = substitutions.begin();
	       IT != substitutions.end(); IT++)
	    {
	      add_substitution(*IT, transducer);
	    }
	}


	// TODO:
	void substitute(const StringPair &sp, const StringPairSet &sps) {}
	void substitute(void (*func)(std::string &isymbol, std::string &osymbol) ) { }  
	void substitute(const StringPair &old_pair, const StringPair &new_pair) {} 

	/** @brief Insert freely any number of \a symbol_pair in 
	    the transducer with weight \a weight. */
	void insert_freely(const StringPair &symbol_pair, W weight) 
	{	  
	  alphabet.insert(symbol_pair.first);
	  alphabet.insert(symbol_pair.second);

	  for (iterator it = begin(); it != end(); it++) {
	      HfstTransition_ <C> tr( it->first, symbol_pair.first, 
				      symbol_pair.second, weight );	      
	      it->second.insert(tr);
	    }
	}
      
      protected:
	/* Disjunct the transition of path \a spv pointed by \a it
	   to state \a s. If the transition does not exist in the transducer,
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

	  HfstTransitionSet tr = state_map[s];
	  bool transition_found=false;
	  /* The target state of the transition followed or added */
	  HfstState next_state; 

	  // Find the transition
	  // (Searching is slow..)
	  for (typename HfstTransitionSet::iterator tr_it = tr.begin();
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
	      max_state++;
	      next_state = max_state;
	      HfstTransition_ <C> transition(next_state, it->first,
					     it-> second, 0);
	      add_transition(s, transition);
	    }

	  // Advance to the next transition on path
	  it++;
	  return disjunct(spv, it, next_state);
	}
	
      public:
	
	/** @brief Disjunct this transducer with a one-path transducer 
	    defined by string pair vector \a spv with weight \a weight. 
	    
	    @pre This transducer must be a trie where all weights are in
	    final states, i.e. all transitions have a zero weight. */
	void disjunct(const StringPairVector &spv, W weight) 
	{
	  StringPairVector::const_iterator it = spv.begin();
	  HfstState initial_state = 0;
	  HfstState final_state = disjunct(spv, it, initial_state);

	  // Set weight of final state
	  if (is_final_state(final_state)) 
	    {
	      float old_weight = get_final_weight(final_state);
	      if (old_weight < weight) 
		return; /* The same path with smaller weight remains */
	    }
	  set_final_weight(final_state, weight);
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


