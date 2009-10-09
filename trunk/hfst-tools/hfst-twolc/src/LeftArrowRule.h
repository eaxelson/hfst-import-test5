#ifndef __LEFT_ARROW_RULE_H_
#define __LEFT_ARROW_RULE_H_
#include "../config.h"
#include "CompileUtilities.h"
#include "RuleCompiler.h"
#include <map>
#include <set>
#include <vector>

template<class T> class LeftArrowRule : public Rule
{ 
 protected:
  T context;
  T take_away_context;

 public:
  LeftArrowRule(HFST::Key input_key, 
		HFST::Key output_key, 
		char * rule_name, 
		T context_language):
  Rule(input_key,output_key,rule_name), context(context_language),
  take_away_context(T::empty_language()) {};
  const char * get_input_string(HFST::Key input)
  {
    if (input == HFST::Epsilon)
      {
	return zero_representation;
      }
    return alphabet.get_name(input);
  }
  void conflict_message(LeftArrowRule &another_rule)
  {
    if (silent)
      {
	return;
      }
    std::cerr 
      << "There is a <=-conflict w.r.t. " 
      << "input symbol " << get_input_string(input)
      << " between rules" << std::endl
      << "\t" << name << std::endl 
      << "\t" << another_rule.name << std::endl;
  }		
  void resolve_message(void)
  {
    if (silent)
      {
	return;
      }
    std::cerr
      << "Resolving the conflict by restricting the context of"
      << std::endl 
      << "\t" << name << std::endl 
      << std::endl;
  }

  void unresolvable_conflict_message(void)
  {
    if (silent)
      {
	return;
      }
    const char * input_name;
    (input == 0 ? input_name = "0" : input_name = alphabet.get_name(input));
    std::cerr
      << "The conflict is unresolvable." << std::endl
      << "Warning! The rules will block each other." << std::endl
      << std::endl;
 
  }
  bool conflicts(LeftArrowRule &another_rule)
  {
    assert(input == another_rule.input);
    if (output == another_rule.output)
      {
	return false;
      }
    bool conflict_exists =
      context.is_intersecting(another_rule.context);
    return conflict_exists;
  }
  
  bool unresolvable_conflict(LeftArrowRule &another_rule)
  {
    bool is_unresolvable = 
      (not context.is_less_than(another_rule.context)) 
    and
      (not another_rule.context.is_less_than(context)); 
    return is_unresolvable;
  }
  bool modify_this_rule(LeftArrowRule &another_rule)
  {
    return another_rule.context.is_less_than(context); 
  }
  void resolve(LeftArrowRule &another_rule)
  {
    take_away_context = take_away_context.disjunct(another_rule.context.copy());
  }
  T compile (T center_language)
  {

    context = context.subtract(take_away_context);
    T compiled_rule = RuleCompiler<T>
      (center_language.intersect(context),       
       T::empty_language(),
       alphabet)();
    return compiled_rule;
  }
  T operator() (void)
  {
    if (verbose)
      {
	std::cerr << "Compiling <=-rule " 
		  << name << std::endl 
		  << std::endl;
      }
    return 
      compile(LeftArrowRuleCenterCondition<T>(input,output,alphabet)());
  }
};

template<class T> class LeftArrowEpenthesisRule : 
public LeftArrowRule<T>
{
 public:
  LeftArrowEpenthesisRule(HFST::Key output_key, 
			  char * rule_name, 
			  T context_language):
  LeftArrowRule<T>(HFST::Epsilon,output_key,rule_name,context_language) {};
  T operator() (void)
  {
    if (LeftArrowRule<T>::verbose)
      {
	std::cerr << "Compiling <=-rule " 
		  << LeftArrowRule<T>::name << std::endl 
		  << std::endl;
      }
    return 
      compile(LeftArrowEpenthesisRuleCenterCondition<T>
	      (LeftArrowRule<T>::output,LeftArrowRule<T>::alphabet)());
  }  
};

template<class T>
struct LeftArrowRuleSet
{
  typedef std::set<LeftArrowRule<T>*> Type;
};

template<class T> class LeftArrowRuleConflictResolver
{
 protected:
  HFST::Key input;
  typename LeftArrowRuleSet<T>::Type rule_set;
 public:
  LeftArrowRuleConflictResolver(void): input(HFST::Epsilon) {};
  LeftArrowRuleConflictResolver(HFST::Key input_key, 
				HFST::Key output_key,
				char * rule_name,
				T context) :
  input(input_key)
    {
      rule_set.insert(new LeftArrowRule<T>(input_key,
					   output_key,
					   rule_name,
					   context));
    };
  void add_rule(HFST::Key input_key, 
		HFST::Key output_key,
		char * rule_name,
		T context)
  {
    assert(input_key == input);
    rule_set.insert(new LeftArrowRule<T>(input_key,
					 output_key,
					 rule_name,
					 context));
  }
  void resolve(void)
  {
    for(typename LeftArrowRuleSet<T>::Type::iterator it = rule_set.begin();
	it != rule_set.end();
	++it)
      {
	typename LeftArrowRuleSet<T>::Type::iterator temp = it;
	++temp;
	for(typename LeftArrowRuleSet<T>::Type::iterator jt = temp;
	    jt != rule_set.end();
	    ++jt)
	  {
	    LeftArrowRule<T> * r1 = *it;
	    LeftArrowRule<T> * r2 = *jt;
	    if (r1->conflicts(*r2))
	      {
		r1->conflict_message(*r2);
		if (not r1->unresolvable_conflict(*r2))
		  {
		    if (not Rule::resolve_conflicts)
		      {
			continue;
		      }
		    r1->resolve_message();
		    if (r1->modify_this_rule(*r2))
		      {
			r1->resolve(*r2);
		      }
		    else
		      {
			r2->resolve(*r1);
		      }
		  }
		else
		  {
		    r1->unresolvable_conflict_message();
		  }
	      }
	  }
      }
  }
  typename CompiledRule<T>::Vector * operator() (void)
  {
    resolve();
    typename CompiledRule<T>::Vector * rules = 
      new typename CompiledRule<T>::Vector; 
    for (typename LeftArrowRuleSet<T>::Type::iterator it = rule_set.begin();
	 it != rule_set.end();
	 ++it)
      {
	char * rule_name = (*it)->get_name_copy();
	T rule = (*it)->operator()();
	delete *it;
	rules->push_back
	  (new typename CompiledRule<T>::Pair(rule_name,rule));
      }
    return rules;
  }
};

template<class T> class LeftArrowEpenthesisRuleConflictResolver :
public LeftArrowRuleConflictResolver<T>
{
 public:
  LeftArrowEpenthesisRuleConflictResolver<T>(HFST::Key output_key,
					     char *rule_name,
					     T context)
    {
      LeftArrowRuleConflictResolver<T>::rule_set.
	insert(new LeftArrowEpenthesisRule<T>(output_key,
					      rule_name,
					      context));
    };
  void add_rule(HFST::Key input_key,
		HFST::Key output_key,		
		char * rule_name,
		T context)
  {
    assert(input_key == HFST::Epsilon);
    LeftArrowRuleConflictResolver<T>::rule_set.
      insert(new LeftArrowEpenthesisRule<T>(output_key,
					    rule_name,
					    context));
  }
  typename CompiledRule<T>::Vector * operator() (void)
  {
    LeftArrowRuleConflictResolver<T>::resolve();
    typename CompiledRule<T>::Vector * rules = 
      new typename CompiledRule<T>::Vector; 
    for (typename LeftArrowRuleSet<T>::Type::iterator it = 
	   LeftArrowRuleConflictResolver<T>::rule_set.begin();
	 it != LeftArrowRuleConflictResolver<T>::rule_set.end();
	 ++it)
      {
	char * rule_name = (*it)->get_name_copy();
	LeftArrowEpenthesisRule<T> * r =
	  static_cast<LeftArrowEpenthesisRule<T>*>(*it);
	T rule = r->operator()();
	delete *it;
	rules->push_back
	  (new typename CompiledRule<T>::Pair(rule_name,rule));
      }
    return rules;
  }
};

template<class T> 
struct LeftArrowRuleMap
{
  typedef std::map<HFST::Key,LeftArrowRuleConflictResolver<T>*> Type;
};

template<class T> class LeftArrowRules
{
 protected:
  typename LeftArrowRuleMap<T>::Type rule_sets;
 public:
  LeftArrowRules<T> (void) {};
  void add_rule(HFST::Key input_key,
		HFST::Key output_key,
		char * rule_name,
		T context)
  {
    if (rule_sets.find(input_key) == rule_sets.end())
      {
	if (input_key == HFST::Epsilon)
	  {
	    rule_sets[HFST::Epsilon] =
	      new LeftArrowEpenthesisRuleConflictResolver<T>
	      (output_key,rule_name,context);
	  }
	else
	  {
	    rule_sets[input_key] =
	      new LeftArrowRuleConflictResolver<T>
	      (input_key,output_key,rule_name,context);	   
	  }
      }
    else
      {
	rule_sets[input_key]->add_rule
	  (input_key,output_key,rule_name,context);
      }
  }
  typename CompiledRule<T>::Vector * operator() (void)
  {
    typename CompiledRule<T>::Vector * rules = 
      new typename CompiledRule<T>::Vector;
    for (typename LeftArrowRuleMap<T>::Type::iterator it = rule_sets.begin();
	 it != rule_sets.end();
	 ++it)
      {
	typename CompiledRule<T>::Vector * these_rules;
	if (it->first == HFST::Epsilon)
	  {
	    LeftArrowEpenthesisRuleConflictResolver<T> * r = 
	      static_cast<LeftArrowEpenthesisRuleConflictResolver<T>*>
	      (it->second);
	    these_rules = r->operator()();
	  }
	else
	  {
	    these_rules = it->second->operator()();
	  }
	delete it->second;
	rules->insert(rules->end(),these_rules->begin(),these_rules->end());
	delete these_rules;
      }
    return rules;
  } 
};
#endif
