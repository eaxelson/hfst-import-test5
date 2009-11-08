#ifndef __RIGHT_ARROW_RULE_H_
#define __RIGHT_ARROW_RULE_H_
#include "../config.h"
#include "CompileUtilities.h"
#include "RuleCompiler.h"
#include <map>
#include <vector>

template<class T> class RightArrowRule;

template<class T> 
struct RightArrowRuleVector
{
  typedef std::vector<RightArrowRule<T>*> Type;
};
 
template<class T> class RightArrowRule : public Rule
{
 protected:
  T context;
 public:
  RightArrowRule(HFST::Key input_key, HFST::Key output_key, 
		 char * rule_name, T context_language):
  Rule(input_key,output_key,rule_name), context(context_language){};
  T get_context(void)
    {
      return context;
    }
  void conflict_message(typename RightArrowRuleVector<T>::Type &other_rules)
  {
    std::cerr << "The right arrow rules" << std::endl
                << "\t" << name << std::endl;
    for (typename RightArrowRuleVector<T>::Type::iterator it = 
	   other_rules.begin();
	 it != other_rules.end();
	 ++it)
      {
	    std::cerr << "\t" << (*it)->name << std::endl;
      }
    const char * input_name; 
    (input == 0 ? input_name = "0" : input_name = alphabet.get_name(input));  
    const char * output_name;  
    (output == 0 ? output_name = "0" : output_name = alphabet.get_name(output));  
    std::cerr << "have the same center " << input_name 
	      << ":" << output_name << ". " 
	      << "Joining rule contexts." << std::endl << std::endl;
      }
  void resolve(typename RightArrowRuleVector<T>::Type other_rules)
  {
    if (other_rules.empty())
      {
	return;
      }
    if (not silent)
      {
	conflict_message(other_rules);
      }
    for (typename RightArrowRuleVector<T>::Type::iterator it = 
	   other_rules.begin();
	 it != other_rules.end();
	 ++it)
      {
	T other_context = (*it)->get_context();
	context = context.disjunct(other_context);
	delete *it;
      }
  }
  T operator() (void)
  {
    if (verbose)
      {
	std::cerr << "Compiling =>-rule " << name << std::endl 
		  << std::endl;
      }
    return 
      RuleCompiler<T>(RightArrowRuleCenterCondition<T>(T(input,
							 output),alphabet)(),
		      context,
		      alphabet)();
  }
};

template<class T> class RightArrowRuleConflictResolver
{
 private:
  HFST::Key input;
  HFST::Key output;
  char * resolver_name;
  RightArrowRule<T> first_rule;
  typename RightArrowRuleVector<T>::Type subsequent_rules;
 public:
  RightArrowRuleConflictResolver(HFST::Key input_key,
				 HFST::Key output_key,
				 char * name,
				 T context):
  input(input_key), output(output_key), resolver_name(string_copy(name)),
  first_rule(input_key,output_key,name,context) {};
  ~RightArrowRuleConflictResolver(void)
    {
      free(resolver_name);
    }
  void add_rule(char * name, T context)
  {
    subsequent_rules.push_back(new RightArrowRule<T>(input,
						     output,
						     name,
						     context));
  }
  char * get_name(void)
  {
    return string_copy(resolver_name);
  }
  T operator() (void)
  {
    first_rule.resolve(subsequent_rules);
    return first_rule();
  }
};

typedef std::pair<HFST::Key,HFST::Key> CenterPair;

template<class T> 
struct RightArrowRuleMap
{
  struct PairCompare
  {
    bool operator() (const CenterPair &p1,
		     const CenterPair &p2) const
    {
      if (p1.first == p2.first)
	{
	  return p1.second < p2.second;
	}
      return p1.first < p2.first;
    }
  };

  typedef std::map<CenterPair,RightArrowRuleConflictResolver<T>*,PairCompare>
    Type;
};

template<class T> class RightArrowRules
{
 private:
  typename RightArrowRuleMap<T>::Type rule_sets;
 public:
  RightArrowRules(void) {};
  void add_rule(HFST::Key input, HFST::Key output, char * name, T context)
  {
    CenterPair c(input,output);
    if (rule_sets.find(c) == rule_sets.end())
      {
	rule_sets[c] = 
	  new RightArrowRuleConflictResolver<T>(input,output,name,context);
      }
    else
      {
	rule_sets[c]->add_rule(name,context);
      }
  }
  
  typename CompiledRule<T>::Vector * operator() (void)
  {
    typename CompiledRule<T>::Vector * rule_vector =
      new typename CompiledRule<T>::Vector;
    for (typename RightArrowRuleMap<T>::Type::iterator it =
	   rule_sets.begin();
	 it != rule_sets.end();
	 ++it)

      {
	T compiled_rule = it->second->operator()();
	char * rule_name = it->second->get_name();
	typename CompiledRule<T>::Pair * rule_pair =
	  new typename CompiledRule<T>::Pair(rule_name,
					     compiled_rule);
	rule_vector->push_back(rule_pair);
	delete it->second;
      }
    return rule_vector;
  }
};

#endif
