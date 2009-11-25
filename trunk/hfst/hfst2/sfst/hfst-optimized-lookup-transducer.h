#include <hfst.h>
#include "optimized_lookup_defs.h"
#include "fst.h"
#include <climits>
#include <cassert>
#include <cstring>
#include <map>
#include <set>
#include <vector>
#include <iostream>

#ifndef HFST_RUNTIME_CONVERT_HFST
#define HFST_RUNTIME_CONVERT_HFST

struct compare_hfst_labels 
{
  bool operator() ( const Label &l1,
		    const Label &l2) const
  {
    if (l1.lower_char() == l2.lower_char())
      {
	return l1.upper_char() < l2.upper_char();
      }
    return l1.lower_char() < l2.lower_char();
  }
};

typedef std::set<Label,compare_hfst_labels> HfstLabelSet;

/*
  A class which can translate between Node *, StateIdNumbers and source and
  target transition table indices.
*/
class HfstIdNumberMap
{
private:
  typedef 
  std::map<StateIdNumber,const Node*> IdNumbersToNodes;

  typedef 
  std::map<const Node*,StateIdNumber> NodesToIdNumbers;

  typedef 
  std::map<StateIdNumber,TransitionTableIndex> IdNumbersToIndices;

  IdNumbersToNodes id_to_node;
  NodesToIdNumbers node_to_id;
  IdNumbersToIndices id_to_target_index;
  
  StateIdNumber node_counter;
  ArcNumber arc_counter;

  void add_node(const Node * n);
  /*
    Assign every node n in t a unique id number i. Assign the start node 
    t->root_node() id number 0. Set node_to_id[n] = i and
    id_to_node[i] = n.
  */
  void set_node_maps(Transducer * t);

public:
  HfstIdNumberMap(Transducer * t):
    node_counter(0),
    arc_counter(0)
  { set_node_maps(t); }
  
  StateIdNumber get_number_of_nodes(void)
  { return node_counter; }
  
  ArcNumber get_number_of_arcs(void)
  { return arc_counter; }

  StateIdNumber get_node_id(const Node * n);

  const Node * get_id_node(StateIdNumber i);
  
  void set_target_transitions(StateIdNumber i,
			      TransitionTableIndex target_index);

  TransitionTableIndex
  get_id_place(StateIdNumber id)
  { return id_to_target_index[id]; }
};


/* Each transition has an input symbol, 
   an output symbol, a source state and
   a target state.

   Every transition is located at some 
   index in the transition table. This 
   index is transition_place.
*/
class HfstTransition
{
private:
  static HFST::KeyTable * keys;
  static HfstIdNumberMap * nodes_to_id_numbers;

  TransitionTableIndex transition_place;

  SymbolNumber input_symbol;
  SymbolNumber output_symbol;
  StateIdNumber source_state_id;
  StateIdNumber target_state_id;
  
  static const char * get_symbol_string(SymbolNumber num);

  /* Do the SFST stuff necessary to extract the input and output symbols
     of a transition label and the target node. */
  static SymbolNumber get_input_symbol(const Arc &a);
  static SymbolNumber get_output_symbol(const Arc &a);
  static const Node * get_target_node(const Arc &a);


public:
  /* 
     Set the symbol numbers and set the indices of the states according
     to HfstIdNumberMap nodes_to_id_numbers.
  */
  HfstTransition(const Node * source_node, 
	     const Arc &a):
    transition_place(NO_TABLE_INDEX)
  {
#ifdef DEBUG
    assert(nodes_to_id_numbers->get_node_id(source_node) !=
	   NO_ID_NUMBER);
    assert(nodes_to_id_numbers->get_node_id(get_target_node(a)) !=
	   NO_ID_NUMBER);
#endif
    input_symbol = get_input_symbol(a);
    output_symbol = get_output_symbol(a);
    source_state_id = 
      nodes_to_id_numbers->get_node_id(source_node);
    target_state_id = 
      nodes_to_id_numbers->get_node_id(get_target_node(a));
  }
  
  SymbolNumber return_input_symbol(void)
  { return input_symbol; }

  static void set_key_table(HFST::KeyTable * kt);
  static void set_id_numbers(HfstIdNumberMap * ids);

  void set_transition_place(TransitionTableIndex p)
  { transition_place = p; }
  
  TransitionTableIndex get_place(void)
  { return transition_place; }
#ifdef DEBUG
  /* 
     Display a transition in format:
     input_symbol   output_symbol   source_state_id    target_state_id
  */
  void display(void);
#endif
  static void write_empty_transition(FILE * f,bool final);
#ifdef DEBUG
  static void display_empty_transition(bool final);
#endif
  void write(FILE * f);

  bool operator<(const HfstTransition &another_index) const;
};

/*
  HfstTransition indices are like transitions, but one transition index
  represents all transitions in a state with a particular input symbol.
  
  input_symbol is the input symbol of the transitions which HfstTransitionIndex
  represents and first_transition_index is the index of the first transition
  in the transition vector with input symbol input_symbol of the parent state 
  of this transition index.
*/
class HfstTransitionIndex
{
private:
  static HFST::KeyTable * keys;

  TransitionTableIndex first_transition_place;

  SymbolNumber input_symbol;
  SymbolNumber first_transition_index;
  
  static const char * get_symbol_string(SymbolNumber num);  
public:
  HfstTransitionIndex(SymbolNumber input,
		  SymbolNumber first_transition):
    first_transition_place(NO_TABLE_INDEX),
    input_symbol(input),
    first_transition_index(first_transition)
  {}
  
  static void set_key_table(HFST::KeyTable * kt);

#ifdef DEBUG
  void display(void);
#endif
  
  SymbolNumber get_index(void)
  { return first_transition_index; }

  SymbolNumber get_input_symbol(void)
  { return input_symbol; }

  void set_place(TransitionTableIndex p)
  { first_transition_place = p; }

  
  void write(IndexVector * transition_index_table, 
	     TransitionTableIndex start_index);

  bool operator<(const HfstTransitionIndex &another_index) const;
};

struct HfstTransitionCompare
{
  bool operator() (const HfstTransition * t1,
		   const HfstTransition * t2) const 
  { 
    return t1->operator<(*t2); 
  }
};

struct HfstTransitionIndexCompare
{
  bool operator() (const HfstTransitionIndex * i1,
		   const HfstTransitionIndex * i2) const 
  { 
    return i1->operator<(*i2); 
  }
};

typedef std::set<HfstTransition*,HfstTransitionCompare> HfstTransitionSet;
typedef std::set<HfstTransitionIndex*,HfstTransitionIndexCompare> HfstTransitionIndexSet;
typedef std::set<SymbolNumber> SymbolNumberSet;

class HfstFstState
{
private:
  HfstTransitionSet transitions;
  HfstTransitionIndexSet transition_indices;

  TransitionTableIndex first_transition_index;

  bool final;
  StateIdNumber id;

  static KeyTable * keys;
  static HfstIdNumberMap * state_id_numbers;

  void set_transitions(const Node * n);
  void set_transition_indices(void);

  friend class fst_state_compare;

public:
  HfstFstState(const Node * n):
    first_transition_index(0),
    final(n->is_final()),
    id(state_id_numbers->get_node_id(n))
  {
    set_transitions(n);
    set_transition_indices();
  };

  TransitionTableIndex
  set_transition_table_indices(TransitionTableIndex place);

  TransitionTableIndex
  get_first_transition_index(void)
  { return first_transition_index; }

  static void set_key_table(HFST::KeyTable * kt);
  static void set_id_numbers(HfstIdNumberMap * ids);

  StateIdNumber get_id(void)
  { return id; }

  SymbolNumberSet * get_input_symbols(void);

  SymbolNumber number_of_input_symbols(void)
  { return transition_indices.size(); }

  SymbolNumber number_of_transitions(void)
  { return transitions.size(); }
  bool is_final(void)
  { return final; }
  
  bool is_big_state(void)
  { return (transition_indices.size() > BIG_STATE_LIMIT); }
  
  bool is_start_state(void)
  { return id == 0; }

  void add_state_indices(IndexVector * transition_index_table);

  TransitionTableIndex
  write_transitions(FILE * f,
		    TransitionTableIndex place);

#ifdef DEBUG
  void display(void);
#endif
};


class HfstTransitionTableIndices 
{
private:
  PlaceHolderVector indices;
  PlaceHolderVector::size_type lower_bound;
  unsigned int lower_bound_test_count;
  SymbolNumber number_of_input_symbols;
  
  bool state_fits(SymbolNumberSet * input_symbols,
		  bool final_state,
		  PlaceHolderVector::size_type index);

  void insert_state(SymbolNumberSet * input_symbols,
		    bool final_state,
		    PlaceHolderVector::size_type index);
  void get_more_space(void);
  
public:
  HfstTransitionTableIndices(SymbolNumber input_symbol_count):
    lower_bound(0),
    lower_bound_test_count(0),
    number_of_input_symbols(input_symbol_count)
  {
    get_more_space();
  };
  
  PlaceHolderVector::size_type add_state(HfstFstState * state);
  PlaceHolderVector::size_type size(void)
  { return indices.size(); }

  PlaceHolderVector::size_type last_full_index(void);
};

typedef std::set<Node*> NodeSet; 
typedef vector<HfstFstState*> HfstStateVector;


typedef std::vector<transition_index> IndexVector;

class HfstFstHeader 
{
private:
  SymbolNumber number_of_symbols;
  SymbolNumber number_of_input_symbols;
  TransitionTableIndex size_of_transition_index_table;
  TransitionTableIndex size_of_transition_target_table;


  StateIdNumber number_of_states;
  TransitionNumber number_of_transitions;

  bool weighted;

  bool deterministic;
  bool input_deterministic;
  bool minimized;
  bool cyclic;
  bool has_epsilon_epsilon_transitions;
  bool has_input_epsilon_transitions;
  bool has_input_epsilon_cycles;
  bool has_unweighted_input_epsilon_cycles;

  void inspect_nodes(Node * n, 
		     NodeSet &visited_nodes, 
		     SymbolSet &input_symbols)
  {
    if (visited_nodes.find(n) != visited_nodes.end())
      {
	return;
      }
    visited_nodes.insert(n);

    for (ArcsIter aiter(n->arcs());
	 aiter;
	 aiter++)
      {
	Arc a = *aiter;
	Label l = a.label();
	input_symbols.insert(l.lower_char());
	inspect_nodes(a.target_node(),
		      visited_nodes,
		      input_symbols);
      }
  }
  void find_input_epsilon_cycles(Node * n,Node * t,NodeSet &epsilon_targets)
  {
    for (ArcsIter aiter(n->arcs());
	 aiter;
	 aiter++)
      {
	Arc a = *aiter;
	if (a.label().lower_char() != 0)
	  {
	    continue;
	  }

	Node * target = a.target_node();
	if (t == target )
	  { 
	    has_input_epsilon_cycles = true;
	    return; 
	  }

	if ( epsilon_targets.find(target) != epsilon_targets.end())
	  {
	    epsilon_targets.insert(target);
	    find_input_epsilon_cycles(target,t,epsilon_targets);
	  }

	if (has_input_epsilon_cycles)
	  {
	    return;
	  }

      }
  }

  void find_cycles (Node * n,
		    NodeSet &visited_nodes,
		    NodeSet &checked_nodes)
  {

    if (checked_nodes.find(n) != checked_nodes.end())
      {
	return;
      }

    if ( not has_input_epsilon_cycles )
      {
	NodeSet epsilon_nodes;
	find_input_epsilon_cycles(n,n,epsilon_nodes);
      }
    checked_nodes.insert(n);

    visited_nodes.insert(n);
    
    SymbolSet input_symbols;
    HfstLabelSet all_labels;

    for (ArcsIter aiter(n->arcs());
	 aiter;
	 aiter++)
      {
	Arc a = *aiter;
	Label l = a.label();
	Node * target = a.target_node();
	HFST::Key input_key = l.lower_char();
	HFST::Key output_key = l.upper_char();

	if ( input_key == 0 )
	  { 
	    has_input_epsilon_transitions = true;
	    if ( output_key == 0 )
	      {
		has_epsilon_epsilon_transitions = true;
	      }
	  }
	if ( input_symbols.find(input_key) != input_symbols.end())
	  {
	    input_deterministic = false;
	  }
	else
	  {
	    input_symbols.insert(input_key);
	  }
	if ( all_labels.find(l) != all_labels.end() )
	  {
	    deterministic = false;
	  }
	else
	  {
	    all_labels.insert(l);
	  }

	if ( visited_nodes.find(target) != visited_nodes.end() )
	  { cyclic = true; }
	find_cycles(target,
		    visited_nodes,
		    checked_nodes);
      }
    visited_nodes.erase(n);
  }


  void compute_properties(Transducer * t)
  {
    //SFST transducers are always unweighted.
    weighted = false;

    deterministic = t->is_deterministic();
    minimized = t->is_minimised();

    //These properties are liike this, unless evidence for the
    //contrary is found.
    input_deterministic = true;
    cyclic = false;
    has_epsilon_epsilon_transitions = false;
    has_input_epsilon_transitions = false;
    has_input_epsilon_cycles = false;
    has_unweighted_input_epsilon_cycles = false;

    NodeSet nodes;
    SymbolSet input_symbols;
    input_symbols.insert(0);
    number_of_transitions = 0;
    std::cerr << "Inspecting nodes\n";
    inspect_nodes(t->root_node(),
		  nodes,
		  input_symbols);

        number_of_input_symbols = input_symbols.size();
    number_of_states = nodes.size();

    nodes.clear();
    NodeSet checked_nodes;
    nodes.insert(t->root_node());
    std::cerr << "Finding cycles\n";
    find_cycles(t->root_node(),
		nodes,
		checked_nodes);

    // An unweighted transducer doesn't really have a sensible
    // interpretation for this.
    has_unweighted_input_epsilon_cycles =
      has_input_epsilon_cycles;
  }

public:
  HfstFstHeader(Transducer * t, 
	    SymbolNumber symbol_count,
	    TransitionTableIndex number_of_index_table_entries,
	    TransitionTableIndex number_of_traget_table_entries) :
    number_of_symbols(symbol_count),
    size_of_transition_index_table(number_of_index_table_entries),
    size_of_transition_target_table(number_of_traget_table_entries)
  {
    compute_properties(t);
  }
  
  void display(void)
  {

    std::cerr << "There are " <<  number_of_symbols << " symbols.\n";
    std::cerr << "There are " <<  number_of_input_symbols << " input symbols.\n";
    std::cerr << "There are " << size_of_transition_index_table << " transition indices.\n";
     std::cerr << "There are " << size_of_transition_target_table << " transition entries.\n";

     std::cerr << "There are " << number_of_states << " states.\n";
     std::cerr << "There are " << number_of_transitions << " transitions.\n";

     std::cerr << "The transducer is unweighted.\n";

     if (deterministic)
       {
	 std::cerr << "The transducer is deterministic.\n";
       }
     else
       {
	 std::cerr << "The transducer is non-deterministic.\n";
       }

     if (input_deterministic)
       {
	 std::cerr << "The transducer is input-deterministic.\n";
       }
     else
       {
	 std::cerr << "The transducer is not input-deterministic.\n";
       }

     if (minimized)
       {
	 std::cerr << "The transducer is mnimized.\n";
       }
     else
       {
	 std::cerr << "The transducer is not minimized.\n";
       }

     if (cyclic)
       {
	 std::cerr << "The transducer is cyclic.\n";
       }
     else
       {
	 std::cerr << "The transducer is not cyclic.\n";
       }

     if (has_epsilon_epsilon_transitions)
       {
	 std::cerr << "The transducer is not epsilon-free.\n";
       }
     else
       {
	 std::cerr << "The transducer is epsilon-free.\n";
       }

     if (has_input_epsilon_transitions)
       {
	 std::cerr << "The transducer has input-epsilons.\n";
       }
     else
       {
	 std::cerr << "The transducer has no input-epsilons.\n";
       }

     if (has_input_epsilon_cycles)
       {
	 std::cerr << "The transducer has input-epsilon-cycles.\n";
       }
     else
       {
	 std::cerr << "The transducer has no input-epsilon-cycles.\n";
       }

     if(has_unweighted_input_epsilon_cycles)
       {
	 std::cerr << "The transducer has unweighted input-epsilon-cycles.\n";
       }
     else
       {
	 std::cerr << "The transducer has no unweighted input-epsilon-cycles.\n";
       }
  }

  void write_property(bool value,
		      FILE * f)
  {
    unsigned int property;
    if (value)
      {
	property = 1;
      }
    else
      {
	property = 0;
      }
    unsigned int size;
    size = fwrite(&property,sizeof(unsigned int), 1, f);
  }

  void write(FILE * f)
  {

    //std::cerr << "There are " <<  number_of_symbols << " symbols.\n";
    //std::cerr << "There are " <<  number_of_input_symbols << " input symbols.\n";
    unsigned int size;
    size = fwrite(&number_of_input_symbols,sizeof(SymbolNumber),1,f);
    size = fwrite(&number_of_symbols,sizeof(SymbolNumber),1,f);


    //std::cerr << "There are " << size_of_transition_index_table << " transition indices.\n";
    // std::cerr << "There are " << size_of_transition_target_table << " transition entries.\n";

    size = fwrite(&size_of_transition_index_table,sizeof(TransitionTableIndex),1,f);
    size = fwrite(&size_of_transition_target_table,sizeof(TransitionTableIndex),1,f);

    //std::cerr << "There are " << number_of_states << " states.\n";
    // std::cerr << "There are " << number_of_transitions << " transitions.\n";
    size = fwrite(&number_of_states,sizeof(StateIdNumber),1,f);
    size = fwrite(&number_of_transitions,sizeof(TransitionNumber),1,f);

    //std::cerr << "The transducer is unweighted.\n";
    write_property(false,f);

    write_property(deterministic,f);
    write_property(input_deterministic,f);
    write_property(minimized,f);
    write_property(cyclic,f);
    write_property(has_epsilon_epsilon_transitions,f);
    write_property(has_input_epsilon_transitions,f);
    write_property(has_input_epsilon_cycles,f);
    write_property(has_unweighted_input_epsilon_cycles,f);
  }
};

class HfstFst
{
private:
  Transducer * fst;
  HfstIdNumberMap * fst_state_id_numbers;
  HFST::KeyTable * fst_key_table;
  FILE * output_file;
  HfstTransitionTableIndices * fst_indices;
  IndexVector * transition_index_table;

  HfstStateVector states;
  
  void read_nodes(void);
  void set_transition_table_indices(void);
  void set_target_node_places(void);
  
  void add_input_symbols(Node * n,
			 SymbolNumberSet &input_symbols,
			 NodeSet &visited_nodes);

  SymbolNumber number_of_input_symbols(void);
  void build_index_table(TransitionTableIndex index_table_size);
  void write_index_table(void);
  void write_transition_targets();

  TransitionTableIndex count_transitions(void);

#ifdef DEBUG
    void display_symbols(void)
  {
    for (unsigned int i = 0;
	 i < fst_key_table->get_unused_key();
	 ++i)
      {
	HFST::Symbol s = HFST::get_key_symbol(i,fst_key_table);
	const char * symbol_name = HFST::get_symbol_name(s); 
      }
  }
#endif
    void write_symbols(FILE * f)
  {
    for (unsigned int i = 0;
	 i < fst_key_table->get_unused_key();
	 ++i)
      {
	HFST::Symbol s = HFST::get_key_symbol(i,fst_key_table);
	const char * symbol_name = HFST::get_symbol_name(s); 
	//std::cerr << symbol_name << std::endl;
	fputs(symbol_name,f);
	fputc(0,f);
      }
  };

public:
  HfstFst(Transducer * tr,
      HFST::KeyTable * tr_key_table,
      FILE * f):
    fst(tr),
    fst_state_id_numbers(new HfstIdNumberMap(tr)),
    fst_key_table(tr_key_table),
    output_file(f),
    fst_indices(new HfstTransitionTableIndices(number_of_input_symbols())),
    transition_index_table(NULL)
  {
    HfstFstState::set_key_table(fst_key_table);
    HfstFstState::set_id_numbers(fst_state_id_numbers);
#ifdef DEBUG
    std::cerr << "Reading nodes." << std::endl;
#endif
    read_nodes();
#ifdef DEBUG
    std::cerr << "Setting transition indices." << std::endl;
#endif
    set_transition_table_indices();
#ifdef DEBUG
    std::cerr << "Reading target indices." << std::endl;
#endif
    set_target_node_places();
    PlaceHolderVector::size_type index_table_size =
      fst_indices->size();
    delete fst_indices;

    build_index_table(index_table_size);
    HfstFstHeader header(tr,fst_key_table->get_unused_key(),index_table_size,count_transitions());
#ifdef DEBUG
    header.display();
#endif    
    header.write(f);

#ifdef DEBUG
    display_symbols();
#endif
    write_symbols(f);

    write_index_table();
    write_transition_targets();
  }


    
#ifdef DEBUG
  void display(void);
#endif
};

#endif


