
#ifndef __GRAMMAR_COMPILER_H_
#define __GRAMMAR_COMPILER_H_
#include "../config.h"
#include <utility>
#include "CompileUtilities.h"
#include "RuleCompiler.h"
#include "LeftArrowRule.h"
#include "RightArrowRule.h"
#include "OtherRules.h"

enum Operator
  { RIGHT_RULE, LEFT_RULE, RESTRICTION_RULE };

template<class T> class GrammarCompiler
{
 protected:
  RightArrowRules<T> right_arrow_rules;
  LeftArrowRules<T> left_arrow_rules;
  OtherRules<T> other_rules;
 public:
  bool silent;
  bool verbose;
  bool resolve_conflicts;

  GrammarCompiler(void):
   silent(false),verbose(false),resolve_conflicts(false) {};
  void define_alphabet_pair(char * input,
			    char * output)
  {
    Rule::alphabet.define_pair(input,
			       output);
  }
  void set_booleans(bool silent_b,
		    bool verbose_b,
		    bool resolve_conflicts_b)
  {
    silent = silent_b;
    verbose = verbose_b;
    resolve_conflicts = resolve_conflicts_b;
    Rule::resolve_conflicts = resolve_conflicts_b;
    Rule::verbose = verbose;
    Rule::silent = silent;
  }

  void add_rule(char * name,HFST::KeyPair * center_pair,
		unsigned int op,
		T context)
  {
    assert(op <= RESTRICTION_RULE);
    HFST::Key input = HFST::get_input_key(center_pair);
    HFST::Key output = HFST::get_output_key(center_pair);
    delete center_pair;
    switch ((Operator)(op))
      {
      case RIGHT_RULE:
	right_arrow_rules.add_rule(input,
				   output,
				   name,
				   context);
	break;
      case LEFT_RULE:
	left_arrow_rules.add_rule(input,
				  output,
				  name,
				  context);
	break;
      case RESTRICTION_RULE:
	other_rules.add_restriction_rule(input,
					 output,
					 name,
					 context);
	break;
      default:
	assert(false);
      }
  }
  void add_rule(char * name,HFST::KeyPairSet * center_pair_set,
		size_t op,
		T context)
  {
    assert(op <= RESTRICTION_RULE);
    switch ((Operator)(op))
      {
      case RIGHT_RULE:
	other_rules.add_right_arrow_list_rule(center_pair_set,
					      name,
					      context);
	break;
      case LEFT_RULE:
	other_rules.add_left_arrow_list_rule(center_pair_set,
					     name,
					     context);
	break;
      case RESTRICTION_RULE:
	other_rules.add_restriction_list_rule(center_pair_set,
					      name,
					      context);
	break;
      default:
	assert(false);
      }
    TwolCAlphabet::destroy(center_pair_set);
  }
  typename CompiledRule<T>::Vector * operator() (void)
    {
      typename CompiledRule<T>::Vector * compiled_rules = 
	new typename CompiledRule<T>::Vector;
      typename CompiledRule<T>::Vector * compiled_right_arrow_rules =
	right_arrow_rules();
      typename CompiledRule<T>::Vector * compiled_left_arrow_rules =
	left_arrow_rules();
      typename CompiledRule<T>::Vector * compiled_other_rules =
	other_rules();

      compiled_rules->insert(compiled_rules->end(),
			     compiled_right_arrow_rules->begin(),
			     compiled_right_arrow_rules->end());
      delete compiled_right_arrow_rules;
      compiled_rules->insert(compiled_rules->end(),
			     compiled_left_arrow_rules->begin(),
			     compiled_left_arrow_rules->end());
      delete compiled_left_arrow_rules;
      compiled_rules->insert(compiled_rules->end(),
			     compiled_other_rules->begin(),
			     compiled_other_rules->end());
      delete compiled_other_rules;      
      return compiled_rules;
    }
};

union TransducerType
{
  Weighted * w;
  Unweighted * u;
};

#endif
