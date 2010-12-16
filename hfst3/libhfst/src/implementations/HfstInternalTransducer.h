#ifndef _INTERNAL_TRANSDUCER_H_
#define _INTERNAL_TRANSDUCER_H_

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

/** @file HfstInternalTransducer.h
    \brief Declaration of classes needed by HFST's 
    internal transducer format. */

namespace hfst {
  namespace implementations {

    /** @brief The number of a state in an HfstInternalTransducer. */
    typedef unsigned int HfstState;

    class HfstTrieState {
    protected:
      typedef std::set< std::pair < std::pair<unsigned int, unsigned int>,
	HfstTrieState * > > HfstTrieStateTransitions;

      bool final;
      float weight;
      unsigned int state_number;
      HfstTrieStateTransitions transitions;

      HfstTrieState(unsigned int);
      ~HfstTrieState();
      void print(const HfstAlphabet * alphabet);
      void add_transition(unsigned int inumber, unsigned int onumber, 
			  HfstTrieState * target_state);
      HfstTrieState * find(unsigned int, unsigned int);      

      friend class HfstTrie;
      friend class HfstInternalTransducer;
    };

    /** @brief A transducer format for fast disjunction of
	one-path transducers.

	This format supports only disjunction of one-path
	transducers. For a more extensive set of operations,
	an HfstTrie can be converted to an HfstInternalTransducer
	and again to an HfstTransducer.
 */
    class HfstTrie {
    protected:
      typedef std::vector<HfstTrieState *> HfstTrieStateVector;
      HfstTrieState * initial_state;
      HfstAlphabet * alphabet;
      unsigned int max_state_number;
      HfstTrieStateVector states;

    public:
      /** @brief Create an empty trie.

	  The trie has one state that is not final. */
      HfstTrie();
      /** @brief Delete the trie. */
      ~HfstTrie();
      /** @brief Add a one-path transducer as defined with \a spv 
	  with weight \a weight to the trie. */
      void add_path(const StringPairVector &spv, float weight=0);

      void print();

      friend class HfstInternalTransducer;
    };

    class InternalTransducerLine {
    public:
      bool final_line;
      HfstState origin;
      HfstState target;
      unsigned int isymbol;
      unsigned int osymbol;
      float weight;
      
    InternalTransducerLine():
      final_line(false), origin(0), target(0), 
	isymbol(0), osymbol(0),
	weight((float)0) 
	  {};

      bool operator<(const InternalTransducerLine &another) const;
    };
    
    class HfstInternalTransducer;

    /** @brief A transition in an HfstInternalTransducer. */
    struct HfstTransition {
    public:
      /** @brief The source state of the transition. */
      HfstState source;
      /** @brief The target state of the transition. */
      HfstState target;
      /** @brief The input symbol of the transition. */
      std::string isymbol;
      /** @brief The output symbol of the transition. */
      std::string osymbol;
      /** @brief The weight of the transition. */
      float weight;
      
      /** @brief Create an uninitialized HfstTransition. */
      HfstTransition() {};

      /** @brief Create an HfstTransition from state \a source
	  to state \a target with input string \a isymbol,
	  output string \a osymbol and weight \a weight. */
      HfstTransition(HfstState source,
		     HfstState target,
		     std::string &isymbol,
		     std::string &osymbol,
		     float weight) {
	this->source = source;
	this->target = target;
	this->isymbol = isymbol;
	this->osymbol = osymbol;
	this->weight = weight;
      }

      /* A less-than operator used for storing Transitions in a set. */
      bool operator<(const HfstTransition &another) const {
	if (this->source < another.source) return true;
	if (this->source > another.source) return false;
	
	if (this->target < another.target) return true;
	if (this->target > another.target) return false;
	
	if (this->isymbol.compare(another.isymbol) < 0) return true;
	if (this->isymbol.compare(another.isymbol) > 0) return false;
	
	if (this->osymbol.compare(another.osymbol) < 0) return true;
	if (this->osymbol.compare(another.osymbol) > 0) return false;
	
	if ( this->weight < another.weight ) return true;
	if ( this->weight > another.weight ) return false;
	
	return false;
      }

    };


    // A new version of HfstInternalTransducer

    template <class W> struct HfstInternalState
      {
	HfstState state_number;
	W final_weight;
    };

    struct TransitionData {
      std::string input_symbol;
      std::string output_symbol;
      float weight;
    };

    // Not parametrized with class W, it is the user's 
    // responsibility to use the same weight type in
    // transitions and final states.
    template <class C> class HfstInternalTransition 
      {
      protected:
	HfstState target_state;
	C transition_data;
      public:

        HfstInternalTransition(): target_state(0)
	  {}

      HfstInternalTransition(const HfstInternalTransition<C> &another): 
	target_state(another.target_state), transition_data(transition_data) {}

	bool operator<(const HfstInternalTransition<C> &another) const {
	  if (target_state == another.target_state)
	    return (transition_data < another.transition_data);
	  return (target_state < another.target_state);
	}

	void operator=(const HfstInternalTransition<C> &another) const {
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


    template <class C, class W> class HfstInternalTransducer_ 
      {
      protected:
	typedef std::map<HfstInternalState<W>, 
	  std::set<HfstInternalTransition<C> > >
	  HfstStateMap_;
	HfstStateMap_ state_map;

      public:
	typedef typename HfstStateMap_::iterator iterator;
	typedef typename HfstStateMap_::const_iterator const_iterator;

	iterator begin() {
	  state_map.begin();
	}

	const_iterator begin() const {
	  state_map.begin();
	}

	iterator end() {
	  state_map.end();
	}

	const_iterator end() const {
	  state_map.end();
	}

	friend class hfst::HfstTransducer;
      };

    typedef HfstInternalTransducer_ <TransitionData, float> HfstInternalTransducer__;
   


    /** @brief A simple transducer format that supports adding states
	and transitions and iterating through them. 
	
	This format is used internally for conversion between transducer formats.
	This could also be used for writing binary transducers in an
	implementation-independent format.. (TODO) 

	This format is essentially a text-based representation of a
	transducer. It contains a set of HfstTransitions and final
	states. State number zero is always the start state.

	The operations offered by this class are very limited.
	Convert an HfstInternalTransducer into an HfstTransducer
	if you need more operations.
     */
    class HfstInternalTransducer {
    public:
      typedef std::set<InternalTransducerLine> InternalTransducerLineSet;
      typedef std::set<std::pair<HfstState,float> > FinalStateSet;
      typedef InternalTransducerLineSet::const_iterator const_iterator;

      InternalTransducerLineSet lines;
      FinalStateSet final_states;
      unsigned int disjunct_max_state_number;
      HfstAlphabet * alphabet;

    public:
      /** @brief Create an empty transducer. */
      HfstInternalTransducer();
      /** @brief Delete the transducer. */
      ~HfstInternalTransducer();
      /** @brief Create a deep copy of transducer \a transducer. */
      HfstInternalTransducer(const HfstInternalTransducer &transducer);
      /** @brief Create a new internal transducer equivalent to \a
	  transducer. */
      HfstInternalTransducer(const HfstTransducer &transducer);
      /** @brief Create a new internal transducer equivalent to \a
	  trie. */
      HfstInternalTransducer(const HfstTrie &trie);

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
    };

    /** @brief An iterator to the states of an internal transducer.

	If the an internal transducer is empty, the iterator
	HfstStateIterator it(transducer) is at end just after it has been created:
	it.done() == true
    */
    class HfstStateIterator {
    protected:
      typedef std::set<HfstState> HfstStateSet;
      HfstStateSet state_set;
      HfstStateSet::iterator it;
    public:
      /** @brief Create an iterator to the states of \a transducer. */
      HfstStateIterator(const HfstInternalTransducer &transducer);
      /** @brief Get the value pointed by the iterator. */
      HfstState value();
      /** @brief Advance the iterator to the next state. */
      void next();
      /** @brief Whether the iterator is at end. */
      bool done();
    };

    /** @brief An iterator to the transitions of a given state in an internal transducer. */
    class HfstTransitionIterator {
    protected:
      typedef std::set<HfstTransition> HfstTransitionSet;
      HfstTransitionSet transition_set;
      HfstTransitionSet::iterator it;
    public:
      /** @brief Create an iterator to the transitions leaving from state \a s in 
	  internal transducer \a transducer. */
      HfstTransitionIterator(const HfstInternalTransducer &transducer, HfstState s);
      /** @brief Get the value pointed by the iterator. */
      HfstTransition value();
      /** @brief Advance the iterator to the next transition. */
      void next();
      /** @brief Whether the iterator is at end. */
      bool done();
    };

  }
}
#endif

