
/** \defgroup CoreExtensionLayer Core Extension Layer
 *
 * Datatypes and functions related to manipulation of transducer states and transitions.
 * 
 */
/*@{*/



/** @name Datatypes */
//@{


/** \brief A state in a transducer. */
typedef State State;

/** \brief A transition between two states. */
typedef Transition Transition;

/** \brief A transition iterator over all transitions leaving from a state. */
typedef TransitionIterator TransitionIterator;


//@}




/** @name Defining states */
//@{


/** \brief Create a state in transducer \a t. */
State create_state( TransducerHandle t );

/** \brief Set the state \a s in transducer \a t as final with weight \a w. */
State set_final_state( State s, TransducerHandle t, float w=0 );


//@}




/** @name Defining transitions */
//@{


/** \brief Define a transition leaving from state \a orig in transducer \a t with key pair \a p, target state \a target and weight \a w. */
void define_transition( TransducerHandle t, State orig, KeyPair *p, State target, float w=0 );

//@}





/** @name Using states */
//@{


/** \brief Get initial state from transducer \a t. */
State get_initial_state( TransducerHandle t );

/** \brief Whether state \a s in transducer \a t is final. */
bool is_final_state( State s, TransducerHandle t );

/** \brief Get the final weight of state \a s in transducer \a t. 

@pre \a s is a final state. */
float get_final_weight( State s, TransducerHandle t );


//@}




/** @name Using transitions */
//@{


/** \brief Get the key pair of transition \a tr. */
KeyPair *get_transition_keypair( Transition tr );

/** \brief Get the goal state of transition \a tr. */
State get_transition_to( Transition tr );

/** \brief Get the weight of transition \a tr. */
float get_transition_weight( Transition tr );


//@}





/** @name Iterators over transitions */
//@{


/** \brief A transition iterator over transitions leaving from state \a s in transducer \a t. */
TransitionIterator begin_ti( TransducerHandle t, State s );

/** \brief Next transition in \a ti. */
void next_ti( TransitionIterator &ti );

/** \brief Whether transition iterator \a ti has passed the last transition. */
bool is_end_ti( TransitionIterator ti );

/** \brief Get the transition represented by transition iterator \a ti. */
Transition get_ti_transition( TransitionIterator ti );



/** \brief Set the state \a s in transducer \a t as marked. */
State set_marked_state( State s, TransducerHandle t );


/** \brief Unset the state \a s in transducer \a t as marked. */ 	 
State unset_marked_state( State s, TransducerHandle t );


/** \brief Whether state \a s in transducer \a t has been marked. */ 	
bool is_marked( State s, TransducerHandle t );


//@}

// \brief Size of the transition iterator ti. */
//size_t size_ti( TransitionIterator ti );



/*@}*/

