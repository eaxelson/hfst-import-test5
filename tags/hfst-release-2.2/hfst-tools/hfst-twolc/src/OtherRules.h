#ifndef __OTHER_RULES_H_
#define __OTHER_RULES_H_
#include "../config.h"
#include "CompileUtilities.h"
#include "RuleCompiler.h"

template<class T> class RestrictionRule : public Rule
{
 protected:
  T context;
 public:
  RestrictionRule(HFST::Key input_key, HFST::Key output_key, 
		 char * rule_name, T context_language):
  Rule(input_key,output_key,rule_name), context(context_language){};
  T operator() (void)
  {
     if (verbose)
      {
	std::cerr << "Compiling /<=-rule " << name << std::endl 
		  << std::endl;
      }
     RestrictionArrowRuleCenterCondition<T> center_condition
       (input,output,alphabet);
     T center = center_condition().intersect(context);
    return 
      RuleCompiler<T>
      (center,
       T::empty_language(),
       alphabet)();
  }
};

template<class T> class ListRule : public Rule
{
 protected:
  T context;
 public:
  ListRule(char * name, T context_language):
  Rule(0,0,name), context(context_language) 
  {};
};

template<class T> class RightArrowListRule : public ListRule<T>
{
 protected:
  T center;
 public:
 RightArrowListRule(HFST::KeyPairSet * key_pair_set,
		    char * name, T context_language):
  ListRule<T>(name,context_language), 
    center(RightArrowRuleCenterCondition<T>(key_pair_set,Rule::alphabet)())
    {};
  T operator() (void)
  {
    if (Rule::verbose)
      {
	std::cerr << "Compiling =>-rule " << ListRule<T>::name 
		  << std::endl 
		  << std::endl;
      }
    return RuleCompiler<T>(center,
			   ListRule<T>::context,
			   ListRule<T>::alphabet)();
  }
};

template<class T> class LeftArrowListRule : public ListRule<T>
{
 protected:
  T center;
 public:
 LeftArrowListRule(HFST::KeyPairSet * key_pair_set,
		   char * name, T context_language):
  ListRule<T>(name,context_language), 
    center(LeftArrowRuleCenterCondition<T>(key_pair_set,Rule::alphabet)())
    {};
  T operator() (void)
  {
    if (Rule::verbose)
      {
	std::cerr << "Compiling <=-rule " << ListRule<T>::name 
		  << std::endl 
		  << std::endl;
      }
    return RuleCompiler<T>(center.intersect(ListRule<T>::context),
			   T::empty_language(),
			   ListRule<T>::alphabet)();
  }
};

template<class T> class RestrictionListRule : public ListRule<T>
{
 protected:
  T center;
 public:
 RestrictionListRule(HFST::KeyPairSet * key_pair_set,
		     char * name, T context_language):
  ListRule<T>(name,context_language), 
    center(RestrictionArrowRuleCenterCondition<T>(key_pair_set,Rule::alphabet)
	   ())
      {};
  T operator() (void)
  {
    if (Rule::verbose)
      {
	std::cerr << "Compiling /<=-rule " << ListRule<T>::name 
		  << std::endl 
		  << std::endl;
      }
    return RuleCompiler<T>(center.intersect(ListRule<T>::context),
			   T::empty_language(),
			   ListRule<T>::alphabet)();
  }
};

template<class T> class OtherRules
{
 protected:
  typename CompiledRule<T>::Vector * compiled_rules;
 public:
 OtherRules(void) :
  compiled_rules(new typename CompiledRule<T>::Vector) {};
  void add_restriction_rule(HFST::Key input,
			    HFST::Key output,
			    char * rule_name,
			    T context)
  {
    compiled_rules->push_back
      (new typename CompiledRule<T>::Pair
       (rule_name,RestrictionRule<T>(input,output,
				     string_copy(rule_name),context)()));

  }
  void add_right_arrow_list_rule(HFST::KeyPairSet * key_pair_set,
				 char * rule_name,
				 T context)
  {
    compiled_rules->push_back
      (new typename CompiledRule<T>::Pair
       (rule_name,RightArrowListRule<T>(key_pair_set,
					string_copy(rule_name),context)()));
  }
  void add_left_arrow_list_rule(HFST::KeyPairSet * key_pair_set,
				char * rule_name,
				T context)
  {
    compiled_rules->push_back
      (new typename CompiledRule<T>::Pair
       (rule_name,LeftArrowListRule<T>(key_pair_set,
				       string_copy(rule_name),context)()));
  }
  void add_restriction_list_rule(HFST::KeyPairSet * key_pair_set,
				 char * rule_name,
				 T context)
  {
    compiled_rules->push_back
      (new typename CompiledRule<T>::Pair
       (rule_name,RestrictionListRule<T>(key_pair_set,
					 string_copy(rule_name),context)()));
  }
  typename CompiledRule<T>::Vector * operator() (void) 
    {
      return compiled_rules;
    }
};

#endif
