

/** \defgroup RuleLayer Rule Layer
 *
 * Datatypes and functions for rules and contexts.
 */
/*@{*/



/** @name Datatypes for rule functions
 *
 */
//@{

/** \brief Different types of two-level rules. 
    
Use these with functions #make_rule and #make_restriction. 
Their meaning is explained in both functions.  */

typedef enum {
  twol_left,  /**< Twol_Type 'twol_left' */
  twol_right, /**< Twol_Type 'twol_right' */
  twol_both   /**< Twol_Type 'twol_both' */
} Twol_Type;


/** \brief Different types of replace rules.
    
Use them with functions #make_replace and #make_replace_in_context.
Their meaning is explained in both functions. */

typedef enum {
  repl_left,  /**< Repl_type 'repl_left' */
  repl_right, /**< Repl_type 'repl_right' */
  repl_up,    /**< Repl_type 'repl_up' */
  repl_down   /**< Repl_type 'repl_down' */
} Repl_Type;


struct contexts_t {
  TransducerHandle left, right;
  struct contexts_t *next;
};

typedef contexts_t * ContextsHandle;

/** \brief A list of transducer pairs.

A transducer pair represents the left and right context in a rule. */
typedef ContextsHandle ContextsHandle;

//@}



/** @name Rule functions
 *
 */
//@{


/** \brief ContextsHandle with a context pair of left context \a t1 and right context \a t2. 
    
@param t1 Left context. If NULL, interpreted as an epsilon transducer.
@param t2 Right context. If NULL, interpreted as an epsilon transducer. 

@result A ContextsHandle that points to \a t1 and \a t2.
@post \a t1 and \a t2 are not deleted. */
ContextsHandle make_context (TransducerHandle t1, TransducerHandle t2);


/** \brief Add contexts \a c2 to contexts \a c1 and return the updated \a c1. 
    
@result contexts \a c1 after \a c2 has been added to it.

@post \a c1 and \a c2 are not deleted. */
ContextsHandle append_context (ContextsHandle c1, ContextsHandle c2);


  /** \brief A transducer that performs mappings defined in KeyPairSet \a mappings in context defined by
      transducers \a t1 and \a t2 (\a t1 precedes and \a t2 follows). Twol_Type \a type
      defines the type of mapping (obligatory/optional/not allowed) in context and elsewhere. \a Pi defines
      the alphabet known to the rule.

      @param t1 Left context transducer.
      @param mappings A set of possible key pair mappings that can occur.
<!--  @param r1 Range to be mapped (input side of resulting transducer). -->
      @param type The type of mapping rule.      
      - if \a type is 'twol_left', mapping is obligatory in context. Elsewhere the mapping is optional (if the alphabet allows it).
      - if \a type is 'twol_right', mapping is optional in context. Elsewhere the mapping is not allowed. 
      - if \a type is 'twol_both', mapping is obligatory in context. Elsewhere the mapping is not allowed.
<!--  @param r2 Mapping of Range (output side of resulting transducer). -->
      @param t2 Right context transducer.
      @param Pi The alphabet (set) of key pairs known to the rule
      
      @post \a t1 and \a t2 are deleted.
      
      For example, if the alphabet \a Pi includes symbol pairs a, b, c and a:b, and  the function argument values
      are t1 = <tt>[c]</tt>, mappings = <tt>[a:b]</tt> and t2 = <tt>[c]</tt>, 
      the resulting transducer yields the following outputs with input string 'acacaca' depending on the value of \a type: 
      
\verbatim
      type           input        output(s)
      
      'twol_left'    'acacaca'    'acbcbca', 'bcbcbca', 'acbcbcb', 'bcbcbcb' (note that the alphabet must contain the pair a:b here)
      'twol_right'   'acacaca'    'acacaca', 'acbcaca', 'acacbca', 'acbcbca'
      'twol_both'    'acacaca'    'acbcbca' 
\endverbatim

      More on two-levels rules in Koskenniemi: Two-Level Morphology:
      A General Computational Model fo Word-Form Recognition
      and Production (1983). */
TransducerHandle make_rule (TransducerHandle t1, KeyPairSet *mappings, Twol_Type type, TransducerHandle t2, KeyPairSet *Pi); 



  /** \brief A replace transducer that performs the mapping \a t everywhere.       
      Repl_Type \a type defines whether left and right contexts are matched
      at the output or input level. \a optional defines if the mapping
      is optional.

  The result is equivalent to expression <tt>[~[.*  PROJ(t) .*] PROJ(t)]* ~[.*  PROJ(t) .*]</tt>, where
  <tt>PROJ(t)</tt> is 
  - input level projection of \a t if \a type is 'repl_up'
  - output level projection of \a t if \a type is 'repl_down'

      @pre Repl_Type \a type does not have value
      'repl_left' or 'repl_right'.
      
      @param t A transducer defining the mapping.
      @param type The type of replacement rule, 'repl_up' or 'repl_down'.
      @param optional Whether the replacement becomes optional, i.e. whether
      the resulting transducer is disjuncted with the pi machine <tt>[.*]</tt>.
      @param Pi The alphabet (set) of key pairs known to the rule

      @post \a t is deleted. */
TransducerHandle make_replace (TransducerHandle t, Repl_Type type, bool optional, KeyPairSet *Pi);


  /** \brief A replace transducer that performs mapping \a t in context \a c.
      Repl_Type \a type defines whether left and right contexts are matched
      at the output or input level. \a optional defines if the mapping
      is optional.

      The value of Repl_Type \a type defines where matching takes place in the following way:

      - if 'repl_up', left and right contexts are both matched at the output level
      - if 'repl_down', left and right contexts are both matched at the input level
      - if 'repl_left', left context is matched at the output level and right context at the input level
      - if 'repl_right', left context is matched at the input level and right context at the output level

      For example (from Karttunen 1995) the rule defined by transducer <tt>{ab}:x</tt>,
      left context <tt>[a b]</tt> and right context <tt>[a]</tt>
      ('ab' is mapped to 'x' between 'ab' and 'a') yields the
      following relations when applied to the input string 'abababa' with different Repl_Types:

      \verbatim
  Repl_Type:          repl_up                    repl_down                     repl_left          repl_right
                  
  input level:     a b a b a b a     a b a b a b a   or   a b a b a b a      a b a b a b a       a b a b a b a
  output level:    a b  x   x  a     a b  x  a b a        a b a b  x  a      a b a b  x  a       a b  x  a b a
\endverbatim

      Alphabet definition can affect the way the resulting transducer works. 
      For instance, the rule defined by transducer_ <tt>{aa}:{bb}</tt>, left context <tt>[c]</tt> and right context <tt>[c]</tt>
      ('aa' is mapped to 'bb' between 'c' and 'c') yields the following relations when applied to the input string
      'acaac' with different alphabet definitions:

      \verbatim
      Alphabet            input      output(s) 
      
      a, b and c          'acaac'    'acbbc'
      a, b, c and a:b     'acaac'    'acabc', 'acbac', 'acbbc', 'bcabc', 'bcbac', 'bcbbc'
\endverbatim


      @pre ContextsHandle \a c includes only one transducer pair (i.e. a maximum of one right and one left context).
      Left and right context transducers of \a c are automata, 
      i.e. transducers that map strings onto themselves.
 
      @param t A transducer defining the mapping.
      @param type The type of replacement rule.
      @param c The context where mapping occurs. 
      @param optional Whether the replacement becomes optional, i.e. whether
      the resulting transducer is disjuncted with the pi machine <tt>[.*]</tt>.
      @param Pi The alphabet (set) of key pairs known to the rule
      
      @post \a t and \a c are deleted.

      More on replace operator in Karttunen: The Replace Operator (1995). */
TransducerHandle make_replace_in_context (TransducerHandle t, Repl_Type type, ContextsHandle c, bool optional, KeyPairSet *Pi);


  /** \brief A restriction transducer that performs the mapping \a t in
      all contexts of \a c. Twol_Type \a type defines the type of mapping
      (obligatory/optional/not allowed) in context and elsewhere.
      \a direction defines to which direction mapping can be done.

      @param t Transducer defining the mapping.
      @param type The type of mapping rule.      
      - if 'twol_left', mapping is obligatory in contexts. Elsewhere the mapping is optional (if the alphabet allows it).
      - if 'twol_right', mapping is optional in contexts. Elsewhere the mapping is not allowed. 
      - if 'twol_both', mapping is obligatory in contexts. Elsewhere the mapping is not allowed.
      @param c Contexts where mapping can occur.
      @param direction To which direction mapping can be done.
      - if 0, mapping can be done in both directions
      - if 1, mapping can be done from input to output level 
      - if -1, mapping can be done from output to input level
      @param Pi The alphabet (set) of key pairs known to the rule

      @post \a t and \a c are deleted.

      More on restriction operator in Yli-Jyra and Koskenniemi:
      Compiling Contextual Restrictions on Strings into
      Finite-State Automata (2004). */
TransducerHandle make_restriction (TransducerHandle t, Twol_Type type, ContextsHandle c, int direction, KeyPairSet *Pi);


/** \brief Delete ContextsHandle c.

ContextsHandle \a c and all transducer pairs listed in \a c are deleted. */
void delete_contexts_handle( ContextsHandle c );

//@}


/*@}*/



