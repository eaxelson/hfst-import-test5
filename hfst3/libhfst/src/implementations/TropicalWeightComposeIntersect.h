#ifndef _TROPICAL_WEIGHT_COMPOSE_INTERSECT_H_
#define _TROPICAL_WEIGHT_COMPOSE_INTERSECT_H_
#include <fst/fstlib.h>
#include <set>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <iostream>
#include <climits>
#include <string>

/** @file TropicalWeightComposeIntersect.h
    \brief Composing intersect algorithm implemented for OpenFst's transducers 
    with tropical weights.

    Currently this is the only implementation of this algorithm. */

namespace hfst
{
  namespace implementations
  {


    using namespace fst;
    typedef StdArc::StateId StateId;
    typedef StdArc::Label Label;
    typedef std::vector<Label> LabelVector;
    typedef std::set<Label> LabelSet;

    typedef std::vector<StdVectorFst*> TransducerVector;
    typedef std::vector<StateId> StateVector;
    typedef std::pair<StateId,StateVector> StateConfiguration;
    typedef std::queue<StateConfiguration> Agenda;
    typedef ArcIterator<StdVectorFst> StdArcIterator;

    class RuleTransducer;
    typedef  std::vector<RuleTransducer> RuleTransducerVector;

    const Label NO_LABEL = INT_MAX;
    const StateId NO_STATE_ID = INT_MAX;

    template<class A> class MyOLabelCompare {
    public:
      bool operator() (const A &arc1, const A &arc2) const 
      {
	if (arc1.olabel == arc2.olabel)
	  {
	    return arc1.ilabel < arc2.ilabel;
	  }
	return arc1.olabel < arc2.olabel;
      }
      
      uint64 Properties(uint64 props) const {
	return (props & kArcSortProperties) | kOLabelSorted;
      }
    };
    
    template<class A> class MyILabelCompare {
    public:
      bool operator() (const A &arc1, const A &arc2) const 
      {
	if (arc1.ilabel == arc2.ilabel)
	  {
	    return arc1.olabel < arc2.olabel;
	  }
	return arc1.ilabel < arc2.ilabel;
      }
      
      uint64 Properties(uint64 props) const {
	return (props & kArcSortProperties) | kILabelSorted;
      }
    };

    typedef MyILabelCompare<StdArc> StdMyILabelCompare;
    typedef MyOLabelCompare<StdArc> StdMyOLabelCompare;

    template<class A,class AC,class B, class BC> class ParallelIterator
    {
      typename A::iterator a_it;
      typename B::iterator b_it;
      typename A::iterator a_end_it;
      typename B::iterator b_end_it;
    public:
    ParallelIterator(A &a, B &b):
      a_it(a.begin()),
      b_it(b.begin()),
      a_end_it(a.end()),
      b_end_it(b.end())
	{};
     bool Done(void)
     {
       return (a_it == a_end_it) or (b_it == b_end_it);
     }
     void Next(void)
     {
       ++a_it;
       ++b_it;
     }
     AC &dereference_first(void)
     {
       return *a_it;
     }

     BC &dereference_second(void)
     {
       return *b_it;
     }

     void set_first(AC &a)
     {
       a_it = a;
     }

     void set_second(BC &b)
     {
       b_it = b;
     }
    };

    template<class A,class AC,class B, class BC> class ConstParallelIterator
    {
      typename A::const_iterator a_it;
      typename B::const_iterator b_it;
      typename A::const_iterator a_end_it;
      typename B::const_iterator b_end_it;
    public:
    ConstParallelIterator(A &a, B &b):
      a_it(a.begin()),
      b_it(b.begin()),
      a_end_it(a.end()),
      b_end_it(b.end())
	{};
     bool Done(void)
     {
       return (a_it == a_end_it) or (b_it == b_end_it);
     }
     void Next(void)
     {
       ++a_it;
       ++b_it;
     }
     const AC &dereference_first(void)
     {
       return *a_it;
     }

     const BC &dereference_second(void)
     {
       return *b_it;
     }
    };

    typedef 
      ParallelIterator<RuleTransducerVector,RuleTransducer,StateVector,StateId>
      RuleAndStateIterator;

    class CompositionResultTransducer
    {
    protected:
      struct StateVectorCmp
      {
	bool operator() (const StateVector &v1,
			 const StateVector &v2) const;
      };
      typedef map<StateVector,StateId> StateMapper;
      void set_finality(StateId s,StateVector &lexicon_and_rule_states);
      void add_state(StateId s,StateVector &lexicon_and_rule_states);
      StateMapper state_mapper;
      TransducerVector lexicon_and_rule_vector;
      StdVectorFst * result_transducer;
    public:
      CompositionResultTransducer(StateVector &lexicon_and_rule_start_states,
				  TransducerVector &lexicon_and_rule_vector);
      ~CompositionResultTransducer(void);
      bool combination_exists(StateVector &rule_states);
      StateId operator[] (StateVector &lexicon_and_rule_states);
      float get_finality(StateVector &lexicon_and_rule_states);
      StdVectorFst * get_result_transducer(void);
      void add_arc(StateId initial_state,
		   Label input_label,
		   Label output_label,
		   TropicalWeight weight,
		   StateId target_state);
    };

    /* Requires rule_transducer to be non-empty. */   
    class RuleTransducer
    {
    protected:
      StdVectorFst * rule_transducer;
      StdArcIterator * rule_arc_iterator;
      StateId current_state;

      bool find(Label l,int lower_position,int upper_position);
      bool find(Label l1, Label l2, int lower_position,int upper_position);

      Label get_next_input_label(void);
      Label get_next_output_label(void);

    public:
      RuleTransducer(StdVectorFst * rule_transducer);
      RuleTransducer(const RuleTransducer &another);
      ~RuleTransducer(void);
      void init(void);

      void set_state(StateId s);
      bool iterate_find(Label l);
      bool find_input_label(Label l);
      bool find_label_pair(Label l1,Label l2);
      void reset_pair_transitions(void);
      void reset_input_label_transitions(void);
      StateId get_target_state(void);
      StateId get_current_state(void);
      StateId Start(void);
      TropicalWeight get_arc_weight(void);
      void Next(void);
      bool Done(void);
      bool Done(Label l);
      bool Done(Label l1, Label l2);
      size_t Position(void);
      size_t Seek(size_t pos);
      size_t NumArcs(void);
      Label get_input_label(void);
      Label get_output_label(void);
      StdVectorFst * get_transducer(void);
    };

    class StateSequenceTransitionIterator;

    class Grammar
    {
    protected:
      RuleTransducerVector rule_vector;
      StateVector rule_states;
      StateVector rule_target_states;
      bool transitions_exist;

    public:
      Grammar(TransducerVector &rules);
      bool find_transitions(Label input_label);
      void Next(Label input_label);
      bool Done(Label input_label);
      void reset_transitions(void);
      void reset_to_start_states(void);
      Label get_input_label(void);
      Label get_output_label(void);
      void get_target_states(StateVector &target_state_vector);
      void get_current_states(StateVector &target_state_vector);
      void get_transducers(TransducerVector &transducer_vector);
      StdVectorFst * get_first_rule(void);
      TropicalWeight get_arc_weight(void);
      void set_states(StateVector &new_rule_states);
      friend class StateSequenceTransitionIterator;
    };

    class StateSequenceTransitionIterator
    {
    protected:
      TransducerVector transducers;
      StateVector transducer_states;
      StdArcIterator * arc_iterator;
      size_t transducer_index;
      bool set_arc_iterator(void);
    public:
      StateSequenceTransitionIterator(Grammar &grammar);
      ~StateSequenceTransitionIterator(void);
      size_t get_transducer_index(void);
      StateId get_target_state(void);
      Label get_input_symbol(void);
      Label get_output_symbol(void);
      TropicalWeight get_weight(void);
      bool Done(void);
      void Next(void);
    };

    class ComposeIntersectImpl
    {
    protected:
      Agenda agenda;
      LabelVector epsilon_like_symbols;
      LabelSet epsilon_like_symbol_set;
      StdVectorFst * lexicon_transducer;
      Grammar &grammar;
      CompositionResultTransducer composition_result;

      bool use_treshhold; // Whether we should discard paths
      float treshhold;    // with greater weight than a given treshhold.
      float path_weight;

      StateVector target_state_vector;

      StateId get_lexicon_state(StateConfiguration &s);
      void evaluate_result(void);
      void evaluate_state_configuration(StateConfiguration &configuration);
      void set_grammar_states(StateConfiguration &configuration);
      void handle_lexicon_epsilon_like_transitions(StateId lexicon_state,
						   StateId composition_state);
      void handle_grammar_input_epsilon_like_transitions
	(StateId lexicon_state,StateId composition_state);
      void handle_grammar_input_and_output_epsilon_like_transitions
	(StateId lexicon_state,StateId composition_state);
      void handle_grammar_epsilon_like_transitions(StateId lexicon_state,
						   StateId composition_state);

      bool compose_lexicon_arc_with_rules
	(const StdArc &lexicon_arc, StateId composition_state,StateId lexicon_state);

      StateId get_composition_state(StateVector &target_state_vector);
      bool is_epsilon_like_symbol(Label l);

    public:
      ComposeIntersectImpl(StdVectorFst * lexicon,
			   Grammar &grammar,
			   StateVector &lexicon_and_rule_start_states,
			   TransducerVector &lexicon_and_rule_transducers);
      ComposeIntersectImpl(StdVectorFst * lexicon,
			   Grammar &grammar,
			   StateVector &lexicon_and_rule_start_states,
			   TransducerVector &lexicon_and_rule_transducers,
			   float treshhold);
      StdVectorFst * get_result(void);
      void define_epsilon_like_symbol(Label l);
    };

    class ComposeIntersectFst
    {
    protected:
      ComposeIntersectImpl * implementation;
      Grammar * grammar;

    public:
      ComposeIntersectFst(StdVectorFst * lexicon, 
			  TransducerVector &rules);
      ComposeIntersectFst(StdVectorFst * lexicon, 
			  Grammar &grammar);
      ComposeIntersectFst(StdVectorFst * lexicon, 
			  Grammar &grammar,
			  float treshhold);

      ~ComposeIntersectFst(void);
      void add_epsilon_like_symbol(Label l);
      StdVectorFst * operator() (void);

    };


  }
}
#endif

