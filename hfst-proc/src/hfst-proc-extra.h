#ifndef _HFST_PROC_EXTRA_H_
#define _HFST_PROC_EXTRA_H_

#include "hfst-optimized-lookup.h"

inline bool indexes_transition_table(const TransitionTableIndex i)
{
  return i >= TRANSITION_TARGET_TABLE_START;
}
inline bool indexes_transition_index_table(const TransitionTableIndex i)
{
  return i < TRANSITION_TARGET_TABLE_START;
}

/**
 * Represents a (possibly partial) path through a transducer. This is used
 * during lookup for storing lookup paths that are continued as input symbols
 * are provided.
 */
class LookupPath
{
 protected:
  /**
   * Points to the state in the transition index table or the transition table
   * where the path ends. This follows the normal semantics whereby values less
   * than TRANSITION_TARGET_TABLE_START reference the transition index table
   * while values greater than or equal to TRANSITION_TARGET_TABLE_START
   * reference the transition table
   */
  TransitionTableIndex index;
  
  /**
   * Whether the path ends at a final state
   */
  bool final;
  
  /**
   * The output symbols of the transitions this path has followed
   */
  SymbolNumberVector output_symbols;
  
 public:
  LookupPath(const TransitionTableIndex initial): index(initial) {}
  
  LookupPath(const LookupPath& o):
    index(o.index), output_symbols(o.output_symbols) {}
  
  virtual ~LookupPath() {}
  
  virtual LookupPath* clone() const {return new LookupPath(*this);}
  
  /**
   * Follow the transition index, modifying our index
   * @param index the index to follow
   */
  virtual void follow(const TransitionIndex& index);
  
  /**
   * Follow the transition, appending the output symbol
   * @param transition the transition to follow
   * @true if following the transition succeeded. This can fail because of
   *       e.g. flag diacritics in some subclasses
   */
  virtual bool follow(const Transition& transition);
  
  TransitionTableIndex get_index() const {return index;}
  bool at_final() const {return final;}
  SymbolNumberVector get_output_symbols() const {return output_symbols;}
};

/**
 * A lookup path which additionally stores the state of the flag diacritics on
 * the path
 */
class LookupPathFd : public LookupPath
{
 protected:
  /**
   * The values of the flag diacritic features at the end of the path
   */
  FlagDiacriticState fd_state;
  
  /**
   * A pointer to the list flag diacritic operations in the transducer where
   * the lookup is being done
   */
  const OperationVector* fd_operations;
  
  /**
   * Evaluates the given flag diacritic operation, possibly modifying fd_state,
   * and returning whether the operation is allowed or not
   */
  bool evaluate_flag_diacritic(FlagDiacriticOperation op);
  
 public:
  LookupPathFd(const TransitionTableIndex initial, const OperationVector& op):
    LookupPath(initial), fd_operations(&op) {}
  LookupPathFd(const LookupPathFd& o): LookupPath(o), fd_state(o.fd_state), 
                                       fd_operations(o.fd_operations) {}
  
  virtual LookupPath* clone() const {return new LookupPathFd(*this);}
  
  /**
   * As in the superclass, except also modifies the flag diacritic state if
   * appropriate, and checks whether or not the follow action is allowed
   */
  virtual bool follow(const Transition& transition);
  
  FlagDiacriticState get_fd_state() {return fd_state;}
};

/**
 * A lookup path which additionally stores the summed weight of the path
 */
class LookupPathW : public LookupPath
{
 protected:
  /**
   * The summed weight of the transitions this path has followed
   */
  Weight weight;
  
  /**
   * An extra weight that is added to the sum when the path is at a final state
   */
  Weight final_weight;
  
  void follow_weight(const TransitionIndex& index);
  void follow_weight(const Transition& transition);
 public:
  LookupPathW(const LookupPathW& o): LookupPath(o), weight(o.weight),
    final_weight(o.final_weight) {}
  
  virtual LookupPath* clone() const {return new LookupPathW(*this);}

  /**
   * As in the superclass, except also modifying weight
   */
  virtual void follow(const TransitionIndex& index);
  /**
   * As in the superclass, except also modifying weight
   **/  
  virtual bool follow(const Transition& transition);
  
  Weight get_weight() const {return at_final() ? weight+final_weight : weight;}
};

/**
 * A lookup path with weight and flag diacritics
 */
class LookupPathWFd : public LookupPathFd
{
 protected:
  /**
   * The summed weight of the transitions this path has followed
   */
  Weight weight;
  
  /**
   * An extra weight that is added to the sum when the path is at a final state
   */
  Weight final_weight;
  
  void follow_weight(const TransitionIndex& index);
  void follow_weight(const Transition& transition);
 public:
  LookupPathWFd(const LookupPathWFd& o): LookupPathFd(o), weight(o.weight),
    final_weight(o.final_weight) {}
  
  virtual LookupPath* clone() const {return new LookupPathWFd(*this);}
  
  /**
   * As in the superclass, except also modifying weight and doing Fd's
   */
  virtual void follow(const TransitionIndex& index);
  /**
   * As in the superclass, except also modifying weight and doing Fd's
   **/  
  virtual bool follow(const Transition& transition);
  
  Weight get_weight() const {return at_final() ? weight+final_weight : weight;}
};

/**
 * A list of references to lookup paths. They are references for the sake of
 * polymorphism
 */
typedef std::vector<LookupPath*> LookupPathVector;



/**
 * Represents the current state of a lookup operation
 */
class LookupState
{
 private:
  /**
   * The transducer in which the lookup is occurring
   */
  const Transducer& transducer;
  
  /**
   * The active paths in a lookup operation. At the start of a lookup this will
   * contain one path. The lookup has failed if it is ever empty.
   */
  LookupPathVector paths;
  
  
  /**
   * Setup the state with a single initial path ending at starting state
   * @param initial a path with which to seed the state
   */
  void init(LookupPath& initial);
  
 public:
  /**
   * Constructor
   * @param t the transducer in which the lookup will occur
   */
  LookupState(const Transducer& t): transducer(t) {}
  
  ~LookupState()
  {
    clear_paths();
  }
  
  /**
   * Determine whether there are any active paths
   * @true if there are any active paths
   */
  bool is_active() const
  {
    return !paths.empty();
  }
  
  void clear_paths();
    
  /**
   * Determine whether any active paths are at a final state
   * @true if an active path is at a final state
   */
  bool is_final() const;
  
  /**
   * Get a list of active paths that are at a final state
   * @return the active paths that are at a final state
   */
  const LookupPathVector get_finals() const;
  
  /**
   * Add a new path to the list of active paths
   * @param path the path to add
   */
  void add_path(LookupPath& path);
  
  /**
   * Get rid of the current paths and replace the list with a new list. Note
   * that the new list is passed by value
   * @param new_paths the new list of paths to store in the lookup state
   */
  void replace_paths(LookupPathVector new_paths);
    
  friend class LookupStepper;
};



/**
 * Class with functions for performing step operations on a LookupState
 */
class LookupStepper
{
 protected:
  /**
   * The transducer in which the lookup is occurring
   */
  const Transducer& transducer;


  /**
   * Find any paths in the state that point to epsilon indices or epsilon
   * transitions, and generate additional paths by following the epsilons.
   * Because the additional paths are appended as they are generated, any
   * new epsilons generated by the epsilons will be properly handled
   * @param s the state to operate on
   */
  void try_epsilons(LookupState& s) const;
  
  /**
   * If the given path points to a place in the index table with an epsilon
   * index, generate an additional path by following it
   * @param s the state to operate on
   * @param path a path pointing to the transition index table
   */
  void try_epsilon_index(LookupState& s, const LookupPath& path) const;
  
  /**
   * If the given path points to one or more epsilon transitions, generate
   * additional paths by following them
   * @param s the state to operate on
   * @param path a path pointing to the beginning of a state in the transition
   *             table or directly to transitions
   */
  virtual void try_epsilon_transitions(LookupState& s, 
                                       const LookupPath& path) const;
  
  
  /**
   * Find any paths in the state that have an index or transitions that match
   * the input symbol and generate a new set of paths by following the index
   * and/or transitions
   * @param s the state to operate on
   * @param input the input symbol to apply
   */
  void apply_input(LookupState& s, const SymbolNumber input) const;
  
  /**
   * If the given path points to a place in the index table with an index for
   * the given input symbol, call try_transitions after having the path follow
   * the index
   * @param new_paths any new paths generated get appended here
   * @param path a path pointing to the transition index table
   * @param input the input symbol to look up in the transition index table
   */
  void try_index(LookupPathVector& new_paths, 
                 const LookupPath& path, const SymbolNumber input) const;
  
  /**
   * If the given path points to one or more transitions whose inputs match
   * the given input symbol, generate new paths by following them
   * @param new_paths any new paths generated get appended here
   * @param path a path pointing to the beginning of a state in the transition
   *             table or directly to transitions
   * @param input the input symbol to look up in the transition table
   */
  void try_transitions(LookupPathVector& new_paths,
                       const LookupPath& path, const SymbolNumber input) const;
 public:
  LookupStepper(const Transducer& t): transducer(t) {}
  
  /**
   * Setup the state with a single initial path ending at starting state and
   * try epsilons at the initial position
   * @param s the state to operate on
   * @param initial an initial path to start the lookup
   */
  void init(LookupState& s, LookupPath& initial) const;
  
  /**
   * Apply a new input symbol to the state's active paths, and then follow any
   * epsilon transitions
   * @param s the state to operate on
   * @param input the new input symbol
   */
  void step(LookupState& s, const SymbolNumber input) const;
};

#endif
