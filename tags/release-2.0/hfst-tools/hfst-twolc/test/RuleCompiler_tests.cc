#include "RuleCompiler_tests.h"

bool test(void)
{
  Rule::alphabet.define_pair(string_copy("a"),string_copy("a"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("b"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("c"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("d"));
  Rule::alphabet.define_pair(string_copy("b"),string_copy("a"));
  Rule::alphabet.define_pair(string_copy("b"),string_copy("b"));
  Rule::alphabet.define_pair(string_copy("__HFST_TWOLC_EPSILON_SYMBOL"),
			     string_copy("a"));
  Rule::alphabet.define_pair(string_copy("__HFST_TWOLC_EPSILON_SYMBOL"),
			     string_copy("b"));
  Weighted w = Weighted::universal(Rule::alphabet);
  w.destroy_fst();
  Unweighted uw = Unweighted::universal(Rule::alphabet);
  uw.destroy_fst();
  
  // Sigma* marker a:b marker Sigma*
  Unweighted center_condition =
    RightArrowRuleCenterCondition<Unweighted>(Unweighted(string_copy("a"),
							 string_copy("b"),
							 Rule::alphabet),
					      Rule::alphabet)();
  
  // Sigma* marker Sigma marker a:a Sigma
  Unweighted context_condition =
    RuleContextCondition<Unweighted>(Unweighted(),
				     Unweighted(string_copy("a"),
						string_copy("a"),
						Rule::alphabet),
				     Rule::alphabet)();

  // a:b => _ a:a ;
  Unweighted rule = RuleCompiler<Unweighted>(center_condition,
					     context_condition,
					     Rule::alphabet)();
  rule.destroy_fst();
  Rule::verbose = false;
  Rule::silent = true;
  Rule::resolve_conflicts = true;

  context_condition =
    RuleContextCondition<Unweighted>(Unweighted(),
				     Unweighted(string_copy("a"),
						string_copy("a"),
						Rule::alphabet),
				     Rule::alphabet)();
  Unweighted context_condition2 =  
    RuleContextCondition<Unweighted>
    (Unweighted(),
     Unweighted(string_copy("a"),
		string_copy("c"),
		Rule::alphabet).concatenate(Unweighted(string_copy("a"),
						       string_copy("c"),
						       Rule::alphabet)),
     Rule::alphabet)();
  
  RightArrowRules<Unweighted> right_rules;
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("b")),
		 string_copy("Rule 1"),
		 context_condition);
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("c")),
		 string_copy("Rule 1.1"),
		 context_condition.copy());
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("b")),
		 string_copy("Rule 2"),
		 context_condition2);
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("c")),
		 string_copy("Rule 2.1"),
		 context_condition2.copy());
  CompiledRule<Unweighted>::Vector * v = right_rules();
  for (CompiledRule<Unweighted>::Vector::iterator it = v->begin();
       it != v->end();
       ++it)
    {
      CompiledRule<Unweighted>::Pair * p = *it;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  delete v;

  Unweighted context_condition3 =  
    RuleContextCondition<Unweighted>
    (Unweighted(),
     Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet).concatenate(Unweighted(string_copy("b"),
						       string_copy("a"),
						       Rule::alphabet)),
     Rule::alphabet)();

  Unweighted context_condition4 =  
    RuleContextCondition<Unweighted>
    (Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Rule::alphabet)();

  Unweighted context_condition5 =  
    RuleContextCondition<Unweighted>
    (Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet).concatenate(Unweighted(string_copy("b"),
						       string_copy("b"),
						       Rule::alphabet)),
     Rule::alphabet)();

  LeftArrowRules<Unweighted> left_rules;
  left_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		      Rule::alphabet.get_key(string_copy("b")),
		      string_copy("Left Rule 1"),
		      context_condition3);
  left_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		      Rule::alphabet.get_key(string_copy("d")),
		      string_copy("Left Rule 2"),
		      context_condition4);
  left_rules.add_rule(0,
		      Rule::alphabet.get_key(string_copy("a")),
		      string_copy("Left Rule 3"),
		      context_condition4.copy());
  left_rules.add_rule(0,
		      Rule::alphabet.get_key(string_copy("b")),
		      string_copy("Left Rule 4"),
		      context_condition5);
  CompiledRule<Unweighted>::Vector * lv = left_rules();
  for (CompiledRule<Unweighted>::Vector::iterator it = lv->begin();
       it != lv->end();
       ++it)
    {
      CompiledRule<Unweighted>::Pair * p = *it;
      //std::cout << p->first << std::endl;
      //p->second.display(Rule::alphabet);
      //std::cout << std::endl;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  delete lv;

  OtherRules<Unweighted> other_rules;
  Unweighted context_condition_other =  
    RuleContextCondition<Unweighted>
    (Unweighted(),
     Unweighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Rule::alphabet)();
  other_rules.add_restriction_rule(Rule::alphabet.get_key(string_copy("a")),
				   Rule::alphabet.get_key(string_copy("b")),
				   string_copy("Restriction Rule"),
				   context_condition_other.copy());
  other_rules.add_restriction_rule
    (Rule::alphabet.get_key(string_copy
			    ("__HFST_TWOLC_EPSILON_SYMBOL")),
     Rule::alphabet.get_key(string_copy("b")),
     string_copy("Restriction Rule 2"),
     context_condition_other.copy());
  
  HFST::KeyPairSet * key_pair_set1 = HFST::create_empty_keypair_set();
  key_pair_set1->insert(HFST::define_keypair
		       (Rule::alphabet.get_key(string_copy("a")),
			Rule::alphabet.get_key(string_copy("b"))));
  key_pair_set1->insert(HFST::define_keypair
		       (Rule::alphabet.get_key(string_copy("a")),
			Rule::alphabet.get_key(string_copy("a"))));
  other_rules.add_right_arrow_list_rule
    (key_pair_set1,
     string_copy("Right Arrow List Rule"),
     context_condition_other.copy());

  other_rules.add_left_arrow_list_rule
    (key_pair_set1,
     string_copy("Left Arrow List Rule"),
     context_condition_other.copy());

  other_rules.add_restriction_list_rule
    (key_pair_set1,
     string_copy("Restricition List Rule"),
     context_condition_other);

  CompiledRule<Unweighted>::Vector * ov = other_rules();
  for (CompiledRule<Unweighted>::Vector::iterator it = ov->begin();
       it != ov->end();
       ++it)
    {
      CompiledRule<Unweighted>::Pair * p = *it;
      //std::cout << p->first << std::endl;
      //p->second.display(Rule::alphabet);
      //std::cout << std::endl;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  TwolCAlphabet::destroy(key_pair_set1);
  delete ov;


  return true;

}

bool test2(void)
{
  Rule::alphabet.define_pair(string_copy("a"),string_copy("a"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("b"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("c"));
  Rule::alphabet.define_pair(string_copy("a"),string_copy("d"));
  Rule::alphabet.define_pair(string_copy("b"),string_copy("a"));
  Rule::alphabet.define_pair(string_copy("b"),string_copy("b"));
  Rule::alphabet.define_pair(string_copy("__HWFST_TWOLC_EPSILON_SYMBOL"),
			     string_copy("a"));
  Rule::alphabet.define_pair(string_copy("__HWFST_TWOLC_EPSILON_SYMBOL"),
			     string_copy("b"));
  Weighted w = Weighted::universal(Rule::alphabet);
  w.destroy_fst();
  Weighted uw = Weighted::universal(Rule::alphabet);
  uw.destroy_fst();
  
  // Sigma* marker a:b marker Sigma*
  Weighted center_condition =
    RightArrowRuleCenterCondition<Weighted>(Weighted(string_copy("a"),
							 string_copy("b"),
							 Rule::alphabet),
					      Rule::alphabet)();
  
  // Sigma* marker Sigma marker a:a Sigma
  Weighted context_condition =
    RuleContextCondition<Weighted>(Weighted(),
				     Weighted(string_copy("a"),
						string_copy("a"),
						Rule::alphabet),
				     Rule::alphabet)();

  // a:b => _ a:a ;
  Weighted rule = RuleCompiler<Weighted>(center_condition,
					     context_condition,
					     Rule::alphabet)();
  rule.destroy_fst();
  Rule::verbose = false;
  Rule::silent = true;
  Rule::resolve_conflicts = true;

  context_condition =
    RuleContextCondition<Weighted>(Weighted(),
				     Weighted(string_copy("a"),
						string_copy("a"),
						Rule::alphabet),
				     Rule::alphabet)();
  Weighted context_condition2 =  
    RuleContextCondition<Weighted>
    (Weighted(),
     Weighted(string_copy("a"),
		string_copy("c"),
		Rule::alphabet).concatenate(Weighted(string_copy("a"),
						       string_copy("c"),
						       Rule::alphabet)),
     Rule::alphabet)();
  
  RightArrowRules<Weighted> right_rules;
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("b")),
		 string_copy("Rule 1"),
		 context_condition);
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("c")),
		 string_copy("Rule 1.1"),
		 context_condition.copy());
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("b")),
		 string_copy("Rule 2"),
		 context_condition2);
  right_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		 Rule::alphabet.get_key(string_copy("c")),
		 string_copy("Rule 2.1"),
		 context_condition2.copy());
  CompiledRule<Weighted>::Vector * v = right_rules();
  for (CompiledRule<Weighted>::Vector::iterator it = v->begin();
       it != v->end();
       ++it)
    {
      CompiledRule<Weighted>::Pair * p = *it;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  delete v;

  Weighted context_condition3 =  
    RuleContextCondition<Weighted>
    (Weighted(),
     Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet).concatenate(Weighted(string_copy("b"),
						       string_copy("a"),
						       Rule::alphabet)),
     Rule::alphabet)();

  Weighted context_condition4 =  
    RuleContextCondition<Weighted>
    (Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Rule::alphabet)();

  Weighted context_condition5 =  
    RuleContextCondition<Weighted>
    (Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet).concatenate(Weighted(string_copy("b"),
						       string_copy("b"),
						       Rule::alphabet)),
     Rule::alphabet)();

  LeftArrowRules<Weighted> left_rules;
  left_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		      Rule::alphabet.get_key(string_copy("b")),
		      string_copy("Left Rule 1"),
		      context_condition3);
  left_rules.add_rule(Rule::alphabet.get_key(string_copy("a")),
		      Rule::alphabet.get_key(string_copy("d")),
		      string_copy("Left Rule 2"),
		      context_condition4);
  left_rules.add_rule(0,
		      Rule::alphabet.get_key(string_copy("a")),
		      string_copy("Left Rule 3"),
		      context_condition4.copy());
  left_rules.add_rule(0,
		      Rule::alphabet.get_key(string_copy("b")),
		      string_copy("Left Rule 4"),
		      context_condition5);
  CompiledRule<Weighted>::Vector * lv = left_rules();
  for (CompiledRule<Weighted>::Vector::iterator it = lv->begin();
       it != lv->end();
       ++it)
    {
      CompiledRule<Weighted>::Pair * p = *it;
      //std::cout << p->first << std::endl;
      //p->second.display(Rule::alphabet);
      //std::cout << std::endl;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  delete lv;

  OtherRules<Weighted> other_rules;
  Weighted context_condition_other =  
    RuleContextCondition<Weighted>
    (Weighted(),
     Weighted(string_copy("b"),
		string_copy("a"),
		Rule::alphabet),
     Rule::alphabet)();
  other_rules.add_restriction_rule(Rule::alphabet.get_key(string_copy("a")),
				   Rule::alphabet.get_key(string_copy("b")),
				   string_copy("Restriction Rule"),
				   context_condition_other.copy());
  other_rules.add_restriction_rule
    (Rule::alphabet.get_key(string_copy
			    ("__HWFST_TWOLC_EPSILON_SYMBOL")),
     Rule::alphabet.get_key(string_copy("b")),
     string_copy("Restriction Rule 2"),
     context_condition_other.copy());
  
  HWFST::KeyPairSet * key_pair_set1 = HWFST::create_empty_keypair_set();
  key_pair_set1->insert(HWFST::define_keypair
		       (Rule::alphabet.get_key(string_copy("a")),
			Rule::alphabet.get_key(string_copy("b"))));
  key_pair_set1->insert(HWFST::define_keypair
		       (Rule::alphabet.get_key(string_copy("a")),
			Rule::alphabet.get_key(string_copy("a"))));
  other_rules.add_right_arrow_list_rule
    (key_pair_set1,
     string_copy("Right Arrow List Rule"),
     context_condition_other.copy());

  other_rules.add_left_arrow_list_rule
    (key_pair_set1,
     string_copy("Left Arrow List Rule"),
     context_condition_other.copy());

  other_rules.add_restriction_list_rule
    (key_pair_set1,
     string_copy("Restricition List Rule"),
     context_condition_other);

  CompiledRule<Weighted>::Vector * ov = other_rules();
  for (CompiledRule<Weighted>::Vector::iterator it = ov->begin();
       it != ov->end();
       ++it)
    {
      CompiledRule<Weighted>::Pair * p = *it;
      //std::cout << p->first << std::endl;
      //p->second.display(Rule::alphabet);
      //std::cout << std::endl;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  TwolCAlphabet::destroy(key_pair_set1);
  delete ov;


  return true;

}

int main(void)
{
  if (not (test() and test2()))
    {
      exit(1);
    }
  exit(0);
};
