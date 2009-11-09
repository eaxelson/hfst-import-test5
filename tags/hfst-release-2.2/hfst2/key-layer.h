
/** \defgroup KeyLayer Transducer Key Layer
 *
 * Datatypes and functions related to input and output labels in transducers.
 */
/*@{*/


/** \name Datatypes
 * 
 */
//@{

/** \brief An unsigned number for the input or output label of a transition between states. */
typedef Key Key;

/** \brief A pair of keys representing a transition in a transducer. */
typedef KeyPair KeyPair;

/** \brief A set of keys aka an alphabet of keys. */
typedef KeySet KeySet;

/** \brief A vector of Keys. */
typedef KeyVector KeyVector;

/** \brief A container for KeyVectors. */
typedef KeyVectorVector KeyVectorVector;

/** \brief A vector of KeyPairs. */
typedef KeyPairVector KeyPairVector;

/** \brief A set of key pairs aka an alphabet of key pairs. */
typedef KeyPairSet KeyPairSet;

/** \brief Iterator over the keys in a KeySet. */
typedef KeyIterator KeyIterator;

/** \brief Iterator over the set of key pairs in a KeyPairSet. */
typedef KeyPairIterator KeyPairIterator;

//@}



/** \name Keys and KeyPairs
 * 
 */
//@{

/** \brief The Key for epsilon is 0. 
    The associated symbol name may be specified by the user. */
const Key Epsilon=0;

/** \brief Define a key pair with input key \a s1 and output key \a s2. */
KeyPair *define_keypair( Key s1, Key s2 );

/** \brief Define an identity key pair with input and output key \a s. */
KeyPair *define_keypair( Key s );

/** \brief Get the input key of KeyPair \a s. */
Key get_input_key( KeyPair *s );

/** \brief Get the output key of KeyPair \a s. */
Key get_output_key( KeyPair *s );

//@}


/** \name Sets aka alphabets of Keys and KeyPairs 
 *
 */
//@{

/** \brief Define an empty set of keys. */
KeySet *create_empty_key_set();

/** \brief Insert key \a s into the set of keys \a Si and return the updated set. */
KeySet *insert_key( Key s, KeySet *Si );

/** \brief Whether key \a s is a member of the set of keys \a Si. */
bool has_key( Key s, KeySet *Si );

/** \brief Define an empty set of key pairs. */
KeyPairSet *create_empty_keypair_set();

/** \brief Insert key pair \a p into the set of key pairs \a Pi and return the updated set. */
KeyPairSet *insert_keypair( KeyPair *p, KeyPairSet *Pi );

/** \brief Whether key pair \a p is a member of the set of key pairs \a Pi. */
bool has_keypair( KeyPair *p, KeyPairSet *Pi );

//@}



/** \name Key and KeyPair Iterators 
 * 
 */
//@{

/** \brief Beginning of the iterator for key set \a Si. */
KeyIterator begin_sigma_key( KeySet *Si );

/** \brief End of the iterator for key set \a Si. */
KeyIterator end_sigma_key( KeySet *Si );

/** \brief Size of the iterator for key set \a Si. */
size_t size_sigma_key( KeySet *Si );

/** \brief Get the key pointed by key iterator \a si. */
Key get_sigma_key( KeyIterator si );

/** \brief Beginning of the iterator for key pair set \a Pi. */
KeyPairIterator begin_pi_key( KeyPairSet *Pi );

/** \brief End of the iterator for key pair set \a Pi. */
KeyPairIterator end_pi_key( KeyPairSet *Pi );

/** \brief Size of the iterator for key pair set \a Pi. */
size_t size_pi_key( KeyPairSet *Pi );

/** \brief Get the key pair pointed by key pair iterator \a pi. */
KeyPair *get_pi_keypair( KeyPairIterator pi );

//@}


/*@}*/ // end of group KeyLayer


char * key_vector_to_pairstring( KeyVector * kv, KeyTable * kt );
char * key_pair_vector_to_pairstring( KeyPairVector * kv, KeyTable * kt );
