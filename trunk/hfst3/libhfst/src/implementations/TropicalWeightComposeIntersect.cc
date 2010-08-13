#include "TropicalWeightComposeIntersect.h"

namespace hfst
{
  namespace implementations
  {
    bool CompositionResultTransducer::StateVectorCmp::operator() 
      (const StateVector &v1,
       const StateVector &v2) const
    { return v1 < v2; }

    void CompositionResultTransducer::set_finality
    (StateId s,StateVector &lexicon_and_rule_states)
    {
      assert(lexicon_and_rule_states.size() == lexicon_and_rule_vector.size());
      TropicalWeight w = TropicalWeight::One();
      for(size_t i = 0; i < lexicon_and_rule_states.size(); ++i)
	{
	  w = 
	    Times
	    (lexicon_and_rule_vector[i]->Final
	     (lexicon_and_rule_states[i]),w);
	}
      result_transducer->SetFinal(s,w);
    }

    void CompositionResultTransducer::add_state
    (StateId s,StateVector &lexicon_and_rule_states)
    {
      state_mapper[lexicon_and_rule_states] = s;
      set_finality(s,lexicon_and_rule_states);
    }

    float CompositionResultTransducer::get_finality
    (StateVector &lexicon_and_rule_states)
    {
      assert(combination_exists(lexicon_and_rule_states));
      return result_transducer->Final
	(state_mapper[lexicon_and_rule_states]).Value();
    }

    CompositionResultTransducer::CompositionResultTransducer
    (StateVector &rule_start_states,TransducerVector &lexicon_and_rule_vector):
      lexicon_and_rule_vector(lexicon_and_rule_vector),
      result_transducer(new StdVectorFst)
    {
      result_transducer->SetStart(result_transducer->AddState());
      add_state(result_transducer->Start(),rule_start_states);
    }

    CompositionResultTransducer::~CompositionResultTransducer(void)
    {
      delete result_transducer;
    }

    bool CompositionResultTransducer::combination_exists
    (StateVector &rule_states)
    {
      return 
	state_mapper.find(rule_states) !=
	state_mapper.end();
    }

    StateId CompositionResultTransducer::operator[] 
    (StateVector &lexicon_and_rule_states)
    {
      if (not combination_exists(lexicon_and_rule_states))
	{
	  StateId new_state = result_transducer->AddState();
	  add_state(new_state,lexicon_and_rule_states);
	}
      return state_mapper[lexicon_and_rule_states];
    }

    StdVectorFst * CompositionResultTransducer::get_result_transducer(void)
    {
      StdVectorFst * temp = result_transducer;
      result_transducer = NULL;
      return temp;
    }

    void CompositionResultTransducer::add_arc(StateId initial_state,
					      Label input_label,
					      Label output_label,
					      TropicalWeight weight,
					      StateId target_state)
    {
      StdArc arc(input_label,output_label,weight,target_state);
      result_transducer->AddArc(initial_state,arc);
    }
    
    RuleTransducer::RuleTransducer(StdVectorFst * rule_transducer):
      rule_transducer(rule_transducer),
      rule_arc_iterator(NULL)
    { 
      assert(rule_transducer->Start() != kNoStateId); 
      init();
    }

    RuleTransducer::RuleTransducer(const RuleTransducer &another):
      rule_transducer(another.rule_transducer),
      rule_arc_iterator(NULL),
      current_state(another.current_state)
    {
      set_state(current_state);
    }
      
    RuleTransducer::~RuleTransducer(void)
    { delete rule_arc_iterator; }

    void RuleTransducer::init(void)
    {
      current_state = rule_transducer->Start();
      set_state(current_state);
    }
    
    void RuleTransducer::set_state(StateId s)
    {
      delete rule_arc_iterator;
      rule_arc_iterator = 
	new StdArcIterator(*rule_transducer,s);
      current_state = s;
    }

    inline int greater_than(Label x, Label y)
    { return (int)x - (int)y; }

    bool RuleTransducer::iterate_find(Label l)
    {
      size_t position = Position();
      size_t counter = 0;
      for (Seek(0); not Done(); Next())
	{
	  ++counter;
	  if (get_input_label() == l)
	    {
	      Seek(position);
	      return true;
	    }
	}
      std::cerr << counter << std::endl;
      Seek(position);
      return false;
    }

    bool RuleTransducer::find(Label l,int lower_position,int upper_position)
    {
      if (lower_position > upper_position)
	{ 
	  size_t position = Position();
	  while ((position != 0) and (get_input_label() > l))
	  { Seek(--position); }
	  return false; 
	}
      size_t middle = (lower_position + upper_position) / 2;
      Seek(middle);
      if (get_input_label() < l)
	{ return find(l,middle+1,upper_position); }
      if (get_input_label() > l)
	{ return find(l,lower_position,middle - 1); }
      
      return true;
    }

    inline bool x_less_than_y(Label x1, Label x2, Label y1, Label y2)
    { return (x1 < y1) or ((x1 == y1) and (x2 < y2)); }
 
    bool RuleTransducer::find(Label l1, Label l2,
			      int lower_position,int upper_position)
    {

      if (lower_position > upper_position)
	{ 
	  size_t position = Position();	  
	  while ((position != 0) and 
		 x_less_than_y(l1,l2,get_input_label(),get_output_label()))
	    { Seek(--position); }	
	  return false; 
	}
      size_t middle = (lower_position + upper_position) / 2;
      Seek(middle);
      if (x_less_than_y(get_input_label(),get_output_label(),l1,l2))
	{ return find(l1,l2,middle+1,upper_position); }
      if (x_less_than_y(l1,l2,get_input_label(),get_output_label()))
	{ return find(l1,l2,lower_position,middle - 1); }
      
      return true;
    }

    bool RuleTransducer::find_input_label(Label l)
    {
      if (Done())
	{ return false; }
      if (get_input_label() == l)
	{
	  if (get_next_input_label() == l)
	    { 
	      Next();
	      return true;
	    }
	  return false;
	}
      if (find(l,Position(),NumArcs()-1))
	{ 
	  assert(get_input_label() == l);
	  return true;
	}
      return false;
      
      /*
      while(not Done() and (get_input_label() < l))
	{ Next(); }
      
      if (Done())
	{ return false; }

      if (get_input_label() == l)
	{ return true; }

	return false;
      */
    }

    int greater_than(Label x1, Label x2, Label y1, Label y2)
    {
      if (x1 == y1)
	{
	  return (int)y1 - (int)y2;
	}
      return (int)x1 - (int)x2;
    }

    bool RuleTransducer::find_label_pair(Label l1,Label l2)
    { 
      if (Done())
	{ return false; }
      if ((get_input_label() == l1) and (get_output_label() == l2))
	{
	  if ((get_next_input_label() == l1) and (get_next_output_label() == l2))
	    { 
	      Next();
	      return true; 
	    }
	}
      if (find(l1,l2,Position(),NumArcs()-1))
	{ 
	  assert(get_input_label() == l1);
	  assert(get_output_label() == l2);
	  return true;
	}
      return false;
      
      /*
      while((not Done())
	    and
	    (get_input_label() < l1))
	{ Next(); }
      
      if (Done())
	{ return false; }

      if (get_input_label() != l1)
	{ return false; }

      while ((not Done())
	     and
	     (get_input_label() == l1)
	     and
	     (get_output_label() < l2))
	{ Next(); }

      if (Done())
	{ return false; }

      if (get_input_label() == l1)
	{ 
	  if (get_output_label() == l2)
	    { return true; }
	}
	return false;
      */
    }

    void RuleTransducer::reset_pair_transitions(void)
    {
      if (rule_arc_iterator->Done())
	{ assert(Position() > 0); }
      Seek(Position() - 1);
      size_t position = Position();
      Label input_label = get_input_label();
      Label output_label = get_output_label();

      while (position > 0)
	{
	  Seek(position - 1);
	  if ((get_input_label() == input_label)
	      and
	      (get_output_label() == output_label))
	    { --position; }	  
	  else
	    { break; }
	}
      Seek(position);
    }

    void RuleTransducer::reset_input_label_transitions(void)
    {
      if (rule_arc_iterator->Done())
	{ assert(Position() > 0); }
      Seek(Position() - 1);
      size_t position = Position();
      Label input_label = get_input_label();

      while (position > 0)
	{
	  Seek(position - 1);
	  if (get_input_label() == input_label)
	    { --position; }	  
	  else
	    { break; }
	}
      Seek(position);
    }

    StateId RuleTransducer::get_target_state(void)
    {
      return rule_arc_iterator->Value().nextstate;
    }

    StateId RuleTransducer::get_current_state(void)
    {
      return current_state;
    }

    StateId RuleTransducer::Start(void)
    {
      return rule_transducer->Start();
    }

    TropicalWeight RuleTransducer::get_arc_weight(void)
    {
      return rule_arc_iterator->Value().weight;
    }

    void RuleTransducer::Next(void)
    {
      assert(not Done());
      rule_arc_iterator->Next();
    }

    bool RuleTransducer::Done(void)
    {
      return rule_arc_iterator->Done();
    }

    bool RuleTransducer::Done(Label l)
    {
      if (not Done())
	{
	  return get_input_label() != l;
	}
      return true;
    }

    bool RuleTransducer::Done(Label l1, Label l2)
    {
      if (not Done())
	{
	  return 
	    (get_input_label() != l1)
	    or
	    (get_output_label() != l2);
	}
      return true;
    }

    size_t RuleTransducer::Seek(size_t pos)
    {
      rule_arc_iterator->Seek(pos);
      return pos;
    }
    
    size_t RuleTransducer::Position(void)
    {
      return rule_arc_iterator->Position();
    }

    size_t RuleTransducer::NumArcs(void)
    {
      return rule_transducer->NumArcs(current_state);
    }

    Label RuleTransducer::get_input_label(void)
    {
      return rule_arc_iterator->Value().ilabel;
    }
    Label RuleTransducer::get_output_label(void)
    {
      return rule_arc_iterator->Value().olabel;
    }

    Label RuleTransducer::get_next_input_label(void)
    {
      assert(not Done());
      Next();
      if (Done())
	{ 
	  Seek(Position() - 1);
	  return -1; 
	}
      Label l = get_input_label();
      Seek(Position() - 1);
      return l;
    }

    Label RuleTransducer::get_next_output_label(void)
    {
      assert(not Done());
      Next();
      if (Done())
	{ 
	  Seek(Position() - 1);
	  return -1; 
	}
      Label l = get_output_label();
      Seek(Position() - 1);
      return l;
    }

    StdVectorFst * RuleTransducer::get_transducer(void)
    { return rule_transducer; }

    bool arcs_sorted(StdVectorFst*rule)
    {
      for (StateId s = 0; s < rule->NumStates(); ++s)
	{
	  Label l = 0;
	  for (StdArcIterator aiter(*rule,s); not aiter.Done(); aiter.Next())
	    {
	      Label l1 = aiter.Value().ilabel;
	      if (l1 < l)
		{ return false; }
	    }
	}
      return true;
    }
    
    /*
      Create a grammar with the rules in the vector rules.
      There should be at least one rule (otherwise the operation is
      kind of pointless).
    */
    Grammar::Grammar(TransducerVector &rules)
    {
      assert(rules.size() != 0);
      for (TransducerVector::iterator it = rules.begin();
	   it != rules.end();
	   ++it)
	{
	  StdVectorFst * rule = *it;
	  assert(rule->Start() != kNoStateId);
	  ArcSort<StdArc,StdMyILabelCompare>(rule,StdMyILabelCompare());
	  rule_vector.push_back(rule);
	  rule_states.push_back(rule->Start());
	}
    }

    /* 
       Find the next matching transition with input label input_label.
    */
    bool Grammar::find_transitions(Label input_label)
    {
      transitions_exist = false;
      Label old_output_label = NO_LABEL;
      RuleTransducer &first_rule = rule_vector[0];

      /* 
	 Examine transitions with input label input_label in the first rule
	 and try to find transitions, in the rest of the, rules whose output 
	 labels match the output label of the transition in the first rule.
      */
      for (first_rule.find_input_label(input_label);
	   not first_rule.Done(input_label);
	   first_rule.Next())
	{ 
	  /* 
	     A potential transition has been found. At least the first
	     rule has a matching transitions.
	  */
	  transitions_exist = true;
	  
	  /* 
	     All of the rules need to have the same output label 
	     as the first rule, so we need to store the output label. 
	  */
	  Label output_label = first_rule.get_output_label();

	  /* 
	     If we've already examined transitions with the current output 
	     label, move on to the next output label, since we know that there
	     can't be any matching transitions.
	  */
	  if (output_label == old_output_label)
	    { continue; }

	  /*
	    Check the rest of the rules for transitions with input label
	    input_label and output label output_label.
	   */
	  for (RuleTransducerVector::iterator it = rule_vector.begin() + 1;
	       it != rule_vector.end();
	       ++it)
	    {
	      transitions_exist = 
		it->find_label_pair(input_label,output_label);

	      /* 
		 If no suitable transitions exist, in this rule, there can't 
		 be transitions matching in all of the rules, so give up.
	      */
	      if (not transitions_exist)
		{ break; }
	    }
	  
	  /* 
	     If we found a suitable transition, this is the first one, just
	     break.
	  */
	  if (transitions_exist)
	    { break; }

	  old_output_label = output_label;
	}
      return transitions_exist;
    }

    /* 
       Either a transitions has been found, which means that we're not done,
       or the transitions with input label input_label have been exhausted in 
       the first rule.
     */
    bool Grammar::Done(Label input_label)
    {
      return rule_vector[0].Done(input_label);
    }
    
    /*
      Find the next transition with input label input_label, if there is one.
     */
    void Grammar::Next(Label input_label)
    {
      /* First look for transitions with the same output label as the 
	 previous transition. */
      Label output_label = rule_vector[0].get_output_label();

      /* 
	 We find the next suitable tansition in alphabetical order
	 of the transition indices of the rules, if there is one.
       */
      RuleTransducerVector::reverse_iterator first_changed_rule =
	rule_vector.rbegin();
      for ( ; 
	   first_changed_rule != rule_vector.rend();
	   ++first_changed_rule)
	{
	  if (first_changed_rule->Done())
	    { continue; }
	  
	  first_changed_rule->Next();
	  if (not first_changed_rule->Done(input_label,output_label))
	    {
	      break;
	    }
	}

      /* 
	 If first_changed_rule == rule_vector.rend(), there were no more
	 transitions with input input_label and output output_label. In 
	 this case, look for other transitions with input input_label.

	 If first_changed_rule != rule_vector.rend(), there were transitions
	 with input input_label and output output_label. Choose the first one
	 which is larger in alphabetical order than the previous transition.
       */
      if (first_changed_rule == rule_vector.rend())
	{
	  find_transitions(input_label);
	}
      else
	{
	  for (RuleTransducerVector::reverse_iterator 
		 rev_it = rule_vector.rbegin();
	       rev_it != first_changed_rule;
	       ++rev_it)
	    {
	      rev_it->reset_pair_transitions();
	    }
	}
    }

    void Grammar::reset_to_start_states(void)
    {
      StateVector start_states;
      for (RuleTransducerVector::iterator it = rule_vector.begin();
	   it != rule_vector.end();
	   ++it)
	{
	  start_states.push_back(it->Start());
	}
      set_states(start_states);
    }

    /* Return to the first transitions with input label input_label. */
    void Grammar::reset_transitions(void)
    {
      for (RuleTransducerVector::iterator it = rule_vector.begin();
	   it != rule_vector.end();
	   ++it)
	{
	  it->reset_input_label_transitions();
	}
    }

    /* 
       If a transition has bee found, resturn its input label,
       otherwise return NO_LABEL.
    */
    Label Grammar::get_input_label(void)
    {
      if (not transitions_exist)
	{
	  return NO_LABEL;
	}
      RuleTransducer &first_rule = rule_vector[0];
      return first_rule.get_input_label();
    }

    /* 
       If a transition has bee found, resturn its output label,
       otherwise return NO_LABEL.
    */
    Label Grammar::get_output_label(void)
    {
      if (not transitions_exist)
	{
	  return NO_LABEL;
	}
      RuleTransducer &first_rule = rule_vector[0];
      return first_rule.get_output_label();
    }

    /* 
       If a valid transition has been found, write the target states of the
       transition in target_state_vector. Otherwise fill the vector with
       NO_STATE_IDs.
     */
    void Grammar::get_target_states(StateVector &target_state_vector)
    {
      if (not transitions_exist)
	{
	  target_state_vector.assign(rule_vector.size(),NO_STATE_ID);
	}
      else
	{
	  target_state_vector.clear();
	  for (RuleTransducerVector::iterator it = rule_vector.begin();
	       it != rule_vector.end();
	       ++it)
	    {
	      target_state_vector.push_back(it->get_target_state());
	    }
	}
    }

    void Grammar::get_current_states(StateVector &target_state_vector)
    {
      target_state_vector.clear();
      for (RuleTransducerVector::iterator it = rule_vector.begin();
	   it != rule_vector.end();
	   ++it)
	{
	  target_state_vector.push_back(it->get_current_state());
	}
    }

    void Grammar::get_transducers(TransducerVector &transducer_vector)
    {
      transducer_vector.clear();
      for (RuleTransducerVector::iterator it = rule_vector.begin();
	   it != rule_vector.end();
	   ++it)
	{
	  transducer_vector.push_back(it->get_transducer());
	}
    }

    /*
      If a valid transition has been found, return the product of the weights
      of the component transitions, otherwise return infinite weight
      (TropicalWeight::Zero()). 
     */
    TropicalWeight Grammar::get_arc_weight(void)
    {
      if (not transitions_exist)
	{
	  return TropicalWeight::Zero();
	}
      TropicalWeight arc_weight = TropicalWeight::One();
      for (RuleTransducerVector::iterator it = rule_vector.begin();
	   it != rule_vector.end();
	   ++it)
	{
	  arc_weight = Times(arc_weight,it->get_arc_weight());
	}
      return arc_weight;
    }

    void Grammar::set_states(StateVector &new_rule_states)
    {
      rule_states.assign(new_rule_states.begin(),new_rule_states.end());
      for (RuleAndStateIterator it(rule_vector,new_rule_states);
	   not it.Done();
	   it.Next())
	{
	  it.dereference_first().set_state(it.dereference_second());
	}
    }

    StdVectorFst * Grammar::get_first_rule(void)
    { return rule_vector[0].get_transducer(); }

    bool StateSequenceTransitionIterator::set_arc_iterator(void)
    {
      delete arc_iterator;
      if ((transducers.size() - 1) < transducer_index)
	{ 
	  arc_iterator = NULL;
	  return false;
	}
      arc_iterator = new StdArcIterator(*transducers[transducer_index],
					transducer_states[transducer_index]);
      return true;
    }

    StateSequenceTransitionIterator::StateSequenceTransitionIterator
    (Grammar &grammar):
      transducer_states(grammar.rule_states),
      arc_iterator(NULL),
      transducer_index(0)
    {
      assert(grammar.rule_vector.size() > 0);
      for (RuleTransducerVector::iterator it = grammar.rule_vector.begin();
	   it != grammar.rule_vector.end();
	   ++it)
	{
	  transducers.push_back(it->get_transducer());
	}
      set_arc_iterator();
    }

    StateSequenceTransitionIterator::~StateSequenceTransitionIterator(void)
    { delete arc_iterator; }

    size_t StateSequenceTransitionIterator::get_transducer_index(void)
    { return transducer_index; }
    
    StateId StateSequenceTransitionIterator::get_target_state(void)
    { return arc_iterator->Value().nextstate; }

    Label StateSequenceTransitionIterator::get_input_symbol(void)
    { return arc_iterator->Value().ilabel; }

    Label StateSequenceTransitionIterator::get_output_symbol(void)
    { return arc_iterator->Value().olabel; }

    TropicalWeight StateSequenceTransitionIterator::get_weight(void)
    { return arc_iterator->Value().weight; }

    bool StateSequenceTransitionIterator::Done(void)
    {
      while (arc_iterator->Done())
	{
	  ++transducer_index;
	  if (not set_arc_iterator())
	    { return true; }
	}
      return false;
    }

    void StateSequenceTransitionIterator::Next(void)
    { arc_iterator->Next(); }

    StateId ComposeIntersectImpl::get_lexicon_state(StateConfiguration &s)
    { return s.second[0]; }

    void ComposeIntersectImpl::define_epsilon_like_symbol(Label l)
    {
      if (epsilon_like_symbols.size() < ((unsigned int)l + 1))
	{
	  epsilon_like_symbols.insert(epsilon_like_symbols.end(),
				      l + 1 - epsilon_like_symbols.size(),
				      0);
	}
      epsilon_like_symbols[l] = 1;
      epsilon_like_symbol_set.insert(l);
    }

    bool ComposeIntersectImpl::is_epsilon_like_symbol(Label l)
    {
      if ((unsigned int)l < epsilon_like_symbols.size())
	{ return 1 == (unsigned int)epsilon_like_symbols[l]; }
      return false;
    }

    /*
    ComposeIntersectImpl::ComposeIntersectImpl(StdVectorFst * lexicon,
     TransducerVector &rules,
     StateVector &lexicon_and_rule_start_states,
     TransducerVector &lexicon_and_rule_transducers):
      lexicon_transducer(lexicon),
      grammar(rules),
      composition_result
      (lexicon_and_rule_start_states,lexicon_and_rule_transducers) 
    {
    / 
	 The state configuration consisting of the start states of the lexicon
	 and grammar rules is the first state combination in the agenda.
      /
      agenda.push
	(StateConfiguration
	 (composition_result[lexicon_and_rule_start_states],
	  lexicon_and_rule_start_states));
      
      / Define epsilon to be an epsilon like symbol. /
      define_epsilon_like_symbol(0);
    }
    */
    ComposeIntersectImpl::ComposeIntersectImpl
    (StdVectorFst * lexicon,
     Grammar &grammar,					       
     StateVector &lexicon_and_rule_start_states,
     TransducerVector &lexicon_and_rule_transducers):
      lexicon_transducer(lexicon),
      grammar(grammar),
      composition_result
      (lexicon_and_rule_start_states,lexicon_and_rule_transducers),
      use_treshhold(false)
    {
      /* 
	 The state configuration consisting of the start states of the lexicon
	 and grammar rules is the first state combination in the agenda.
      */
      agenda.push
	(StateConfiguration
	 (composition_result[lexicon_and_rule_start_states],
	  lexicon_and_rule_start_states));
      
      /* Define epsilon to be an epsilon like symbol. */
      define_epsilon_like_symbol(0);
    }

    ComposeIntersectImpl::ComposeIntersectImpl
    (StdVectorFst * lexicon,
     Grammar &grammar,					       
     StateVector &lexicon_and_rule_start_states,
     TransducerVector &lexicon_and_rule_transducers,
     float treshhold):
      lexicon_transducer(lexicon),
      grammar(grammar),
      composition_result
      (lexicon_and_rule_start_states,lexicon_and_rule_transducers),
      use_treshhold(true),
      treshhold(treshhold)
    {
      /* 
	 The state configuration consisting of the start states of the lexicon
	 and grammar rules is the first state combination in the agenda.
      */
      agenda.push
	(StateConfiguration
	 (composition_result[lexicon_and_rule_start_states],
	  lexicon_and_rule_start_states));
      
      /* Define epsilon to be an epsilon like symbol. */
      define_epsilon_like_symbol(0);
    }

    void ComposeIntersectImpl::evaluate_result(void)
    {
      while (not agenda.empty())
	{
	  evaluate_state_configuration(agenda.front());
	  agenda.pop();
	}
    }

    void ComposeIntersectImpl::set_grammar_states
    (StateConfiguration &configuration)
    {
      StateVector grammar_states(configuration.second.begin() + 1,
				 configuration.second.end());
      grammar.set_states(grammar_states);
    }

    StateId ComposeIntersectImpl::get_composition_state
    (StateVector &target_state_vector)
    {
      bool combination_existed =
	composition_result.combination_exists(target_state_vector);
      StateId composition_target_state =
	composition_result[target_state_vector];
      
      if (not combination_existed)
	{
	  StateConfiguration target_state_configuration
	    (composition_target_state,target_state_vector);
	  agenda.push(target_state_configuration);
	}
      return composition_target_state;
    }

    bool ComposeIntersectImpl::compose_lexicon_arc_with_rules
    (const StdArc &lexicon_arc, StateId composition_state, 
     StateId lexicon_state)
    {

      (void)lexicon_state;

      bool transitions_found = false;

      for (grammar.find_transitions(lexicon_arc.olabel); 
	   not grammar.Done(lexicon_arc.olabel);
	   grammar.Next(lexicon_arc.olabel))
	{
	  transitions_found = true;

	  TropicalWeight transition_weight = 
	    Times(lexicon_arc.weight, grammar.get_arc_weight());
	  
	  grammar.get_target_states(target_state_vector);
	  target_state_vector.insert
	    (target_state_vector.begin(),lexicon_arc.nextstate);

	  StateId composition_target_state = 
	    get_composition_state(target_state_vector);

	  composition_result.add_arc
	    (composition_state,lexicon_arc.ilabel,grammar.get_output_label(),
	     transition_weight,composition_target_state);
	}
      return transitions_found;
    }

    void ComposeIntersectImpl::handle_lexicon_epsilon_like_transitions
    (StateId lexicon_state, StateId composition_state)
    {
      for(StdArcIterator 
	    it(*lexicon_transducer,lexicon_state);
	  not it.Done();
	  it.Next())
	{
	  const StdArc &lexicon_arc = it.Value();
	  if (is_epsilon_like_symbol(lexicon_arc.olabel))
	    {
	      StateVector current_rule_states;
	      grammar.get_current_states(current_rule_states);
	      
	      current_rule_states.insert(current_rule_states.begin(),
	      				 lexicon_arc.nextstate);
	  
	      StateId composition_target_state = get_composition_state
		(current_rule_states);

	      composition_result.add_arc
		(composition_state,lexicon_arc.ilabel,lexicon_arc.olabel,
		 lexicon_arc.weight,composition_target_state);
	    }
	}
    }

    void 
ComposeIntersectImpl::handle_grammar_input_and_output_epsilon_like_transitions
    (StateId lexicon_state, StateId composition_state)
    {
      StateVector current_rule_states;
      grammar.get_current_states(current_rule_states);
      StateVector current_states(current_rule_states);
      current_states.insert(current_states.begin(),
			    lexicon_state);

      for (StateSequenceTransitionIterator it(grammar);
	   not it.Done();
	   it.Next())
	{
	  if (is_epsilon_like_symbol(it.get_input_symbol()) and
	      is_epsilon_like_symbol(it.get_output_symbol()))
	      {
		size_t transducer_index = it.get_transducer_index();
		current_states[transducer_index + 1] = it.get_target_state();

		StateId composition_target_state =
		  get_composition_state(current_states);
		
		composition_result.add_arc
		  (composition_state,it.get_input_symbol(),
		   it.get_output_symbol(),it.get_weight(),
		   composition_target_state);
		
		current_states[transducer_index + 1] = 
		  current_rule_states[transducer_index];
	      }
	}
    }

    void ComposeIntersectImpl::handle_grammar_input_epsilon_like_transitions
    (StateId lexicon_state, StateId composition_state)
    {
      for (LabelSet::iterator it = epsilon_like_symbol_set.begin();
	   it != epsilon_like_symbol_set.end();
	   ++it)
	{
	  for (grammar.find_transitions(*it); 
	       not grammar.Done(*it); 
	       grammar.Next(*it))
	    {
	      if (is_epsilon_like_symbol(grammar.get_output_label()))
	      	{ continue; }

	      StateVector target_states;
	      grammar.get_target_states(target_states);
	      target_states.insert(target_states.begin(),lexicon_state);
	      StateId composition_target_state = get_composition_state
		(target_states);

	      composition_result.add_arc
		(composition_state,*it,grammar.get_output_label(),
		 grammar.get_arc_weight(),composition_target_state);
	    }
	}
    }

    void ComposeIntersectImpl::handle_grammar_epsilon_like_transitions
    (StateId lexicon_state,StateId composition_state)
    {
      handle_grammar_input_epsilon_like_transitions
	(lexicon_state,composition_state);

      handle_grammar_input_and_output_epsilon_like_transitions
	(lexicon_state,composition_state);
    }

    void ComposeIntersectImpl::evaluate_state_configuration
    (StateConfiguration &configuration)
    {

      StateId composition_state = configuration.first;
      set_grammar_states(configuration);

      handle_lexicon_epsilon_like_transitions
	(get_lexicon_state(configuration),composition_state);
      handle_grammar_epsilon_like_transitions
	(get_lexicon_state(configuration),composition_state);
      set_grammar_states(configuration);
      for(StdArcIterator 
	    it(*lexicon_transducer,get_lexicon_state(configuration));
	  not it.Done();
	  it.Next())
	{
	  const StdArc &lexicon_arc = it.Value();
	  if (compose_lexicon_arc_with_rules
	      (lexicon_arc,composition_state,get_lexicon_state(configuration)))
	    {
	      grammar.reset_transitions();
	    }
	}
    }

    StdVectorFst * ComposeIntersectImpl::get_result(void)
    {
      evaluate_result();
      return composition_result.get_result_transducer();
    }

    ComposeIntersectFst::ComposeIntersectFst(StdVectorFst * lexicon, 
					     TransducerVector &rules):
      implementation(NULL),
      grammar(new Grammar(rules))
    {
      StateVector lexicon_and_rule_start_states;
      
      lexicon_and_rule_start_states.push_back(lexicon->Start());
      for (TransducerVector::iterator it = rules.begin();
	   it != rules.end();
	   ++it)
	{ 
	  StdVectorFst * rule = *it;
	  lexicon_and_rule_start_states.push_back(rule->Start()); 
	}

      TransducerVector lexicon_and_rule_transducers;
      lexicon_and_rule_transducers.push_back(lexicon);
      lexicon_and_rule_transducers.insert(lexicon_and_rule_transducers.end(),
					     rules.begin(),
					     rules.end());

      implementation = 
	new ComposeIntersectImpl(lexicon,
				 *grammar,
				 lexicon_and_rule_start_states,
				 lexicon_and_rule_transducers);
    }

    ComposeIntersectFst::ComposeIntersectFst(StdVectorFst * lexicon, 
					     Grammar &grammar):
      implementation(NULL),
      grammar(NULL)
    {
      grammar.reset_to_start_states();
      StateVector lexicon_and_rule_start_states;
      grammar.get_current_states(lexicon_and_rule_start_states);
      lexicon_and_rule_start_states.insert
	(lexicon_and_rule_start_states.begin(),lexicon->Start());

      // If one of the argument transducers is empty, the result will be empty.
      for (StateVector::iterator it = lexicon_and_rule_start_states.begin();
	   it != lexicon_and_rule_start_states.end();
	   ++it)
	{ 
	  if (*it < 0)
	    { return; }
	}

      TransducerVector lexicon_and_rule_transducers;
      grammar.get_transducers(lexicon_and_rule_transducers);
      lexicon_and_rule_transducers.insert
	(lexicon_and_rule_transducers.begin(),lexicon);

      implementation = 
	new ComposeIntersectImpl(lexicon,
				 grammar,
				 lexicon_and_rule_start_states,
				 lexicon_and_rule_transducers);
    }

    ComposeIntersectFst::ComposeIntersectFst(StdVectorFst * lexicon, 
					     Grammar &grammar,
					     float treshhold):
      implementation(NULL),
      grammar(NULL)
    {
      grammar.reset_to_start_states();
      StateVector lexicon_and_rule_start_states;
      grammar.get_current_states(lexicon_and_rule_start_states);
      lexicon_and_rule_start_states.insert
	(lexicon_and_rule_start_states.begin(),lexicon->Start());

      TransducerVector lexicon_and_rule_transducers;
      grammar.get_transducers(lexicon_and_rule_transducers);
      lexicon_and_rule_transducers.insert
	(lexicon_and_rule_transducers.begin(),lexicon);

      implementation = 
	new ComposeIntersectImpl(lexicon,
				 grammar,
				 lexicon_and_rule_start_states,
				 lexicon_and_rule_transducers,
				 treshhold);
    }

    ComposeIntersectFst::~ComposeIntersectFst(void)
    { 
      delete implementation; 
      delete grammar;
    }

    StdVectorFst * ComposeIntersectFst::operator() (void)
    { 
      if (implementation == NULL)
	{ return new StdVectorFst; }
      return implementation->get_result();
    }

    void ComposeIntersectFst::add_epsilon_like_symbol
    (Label l)
    { implementation->define_epsilon_like_symbol(l); }
    
  }
}

#ifdef COMPOSE_INTERSECT_TEST
#include "compose_intersect_test.cc"
#endif
