//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.
#ifndef _HFST_MUTABLE_TRANSDUCER_H_
#define _HFST_MUTABLE_TRANSDUCER_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif

/** @file HfstMutableTransducer.h
    \brief This file is only for doxygen documentation.

    All functions and datatypes are implemented in files HfstInternalTransducer.h
    and HfstInternalTransducer.cc and typedefed in file HfstTransducer.h. */

namespace hfst
{


#if HAVE_MUTABLE
  /** \brief A handle to a state in a HfstMutableTransducer. **/
  typedef unsigned int HfstState;
#endif

  /** \brief A finite-state synchronous transducer that supports iterating through and adding states and transitions.      


     An example of constructing a transducer from scratch:
\verbatim
HfstMutableTransducer t;       // Create an empty transducer with one start state that is not final.
HfstState initial = 
  t.get_initial_state();       // Get a handle to the start state.
HfstState s = t.add_state();   // Add a second state that is initially not final.
t.set_final_weight(s, 0.5);    // Set the second state final with weight 0.5.
t.add_transition(initial,      // Add a transition leading from the first state
                 "foo",        // with input symbol "foo",
		 "bar",        // output symbol "bar"
		 0.3,          // and weight 0.3
		 s);           // to the second state.

// Now t accepts the string pair "foo":"bar" with weight 0.8.
\endverbatim

     \anchor iterator_example An example of iterating through states and transitions:
\verbatim
// Print a transducer in AT&T format to standard out.
void print(HfstMutableTransducer &t)
{
  HfstStateIterator state_it(t);
  while (not state_it.done()) 
    {
      HfstState s = state_it.value();
      HfstTransitionIterator transition_it(t,s);
      while (not transition_it.done()) 
        {
          HfstTransition tr = transition_it.value();
          cout << s << "\t" 
	       << tr.target_state << "\t"
	       << tr.isymbol << "\t" 
               << tr.osymbol
	       << "\t" << tr.weight << "\n";
          transition_it.next();
        }
      if ( t.is_final(s) )
        cout << s << "\t" 
	     << t.get_final_weight(s) << "\n";
      state_it.next();
    }
  return;
}
\endverbatim

     @see HfstStateIterator HfstTransitionIterator HfstTransition
     */
  /*! \internal This class is basically a wrapper for a TROPICAL_OFST_TYPE HfstTransducer.
     Since creating new states and transitions and modifying them is easiest
     in OpenFst, it is chosen as the only implementation type.
     A separate mutable and iterable transducer class is also safer because
     it does not interact with other operations. */
#if HAVE_MUTABLE
  class HfstMutableTransducer
  {
  protected:
    hfst::implementations::HfstInternalTransducer transducer;
  public:
    /** \brief Create an empty mutable transducer, 
	i.e. a transducer with one non-final initial state. */
    HfstMutableTransducer(void);
    /** \brief Create a mutable transducer equivalent to HfstTransducer \a t. */
    HfstMutableTransducer(const HfstTransducer &t);
    /** \brief Create a deep copy of mutable transducer \a t. */
    HfstMutableTransducer(const HfstMutableTransducer &t);
    /** \brief Delete the mutable transducer. */
    ~HfstMutableTransducer(void);
    /** \brief Set the value of the final weight of state \a s in the mutable transducer to \a w. */
    void set_final_weight(HfstState s, float w);
    /** \brief Whether state \a s in the mutable transducer is final. */
    bool is_final(HfstState s);
    /** \brief Return a handle to the initial state in the mutable transducer. */
    HfstState get_initial_state();
    /** \brief Get the biggest state number in the mutable transducer. */
    HfstState max_state_number() const;    
    /** \brief The final weight of state \a s in the mutable transducer. 
	@pre State \a s must be final. */
    float get_final_weight(HfstState s);
    /** \brief Add transition with input and output symbols \a isymbol and \a osymbol and weight \a w
	between states \a source and \a target in the mutable transducer. 

	@see String */
    void add_transition(HfstState source, std::string isymbol, std::string osymbol, float w, HfstState target);
    /* friend classes */
    friend class HfstTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };
#endif

  /** \brief A state iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstStateIterator
  {
  protected:
    std::set<unsigned int> state_set;
  public:
    /** \brief Create a state iterator for mutable transducer \a t. */
    HfstStateIterator(const HfstMutableTransducer &t);
    ~HfstStateIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current state pointed by the iterator. */
    HfstState value();
    /** \brief Advance the iterator to the next state. */
    void next();
  };
#endif

  /** \brief A transition in an HfstMutableTransducer. 

      Transitions are only returned by an \ref HfstTransitionIterator. 

      For an example of using transitions, see \ref iterator_example "this". 

      @see HfstTransitionIterator HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstTransition
  {
  public:
    /** \brief The input symbol of the transition. */
    std::string isymbol;
    /** \brief The output symbol of the transition. */
    std::string osymbol;
    /** \brief The weight of the transition. */
    float weight;
    /** \brief The target state of the transition. */
    HfstState target_state;
  protected:
    HfstTransition(std::string isymbol, std::string osymbol, float weight, HfstState target_state);
  public:
    ~HfstTransition(void);
    /* \brief The input symbol of the transition. */
    std::string get_input_symbol(void);
    /* \brief The output symbol of the transition. */
    std::string get_output_symbol(void);
    /* \brief The weight of the transition. */
    float get_weight(void);
    /* \brief The target state of the transition. */
    HfstState get_target_state(void);
    friend class HfstTransitionIterator;
  };
#endif

  /** \brief A transition iterator for an HfstMutableTransducer. 

      For an example of iterating through states, see \ref iterator_example "this".

      @see HfstMutableTransducer */
#if HAVE_MUTABLE
  class HfstTransitionIterator
  {
  protected:
    std::set<hfst::implementations::InternalTransducerLine> transition_set;
  public:
    /** \brief Create a transition iterator for state \a s in the mutable transducer \a t. */
    HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s);
    ~HfstTransitionIterator(void);
    /** \brief Whether the iterator is at end. */
    bool done();
    /** \brief The current transition pointed by the iterator. */
    HfstTransition value();
    /** \brief Advance the iterator to the next transition. */
    void next();    
  };
#endif

}



#endif
