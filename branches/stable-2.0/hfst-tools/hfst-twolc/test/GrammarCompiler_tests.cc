#include "GrammarCompiler_tests.h"
bool test(void)
{
  GrammarCompiler<Unweighted> c;
  c.set_booleans(true,false,true);
  c.define_alphabet_pair(string_copy("a"),
			 string_copy("a"));
  c.define_alphabet_pair(string_copy("a"),
			 string_copy("b"));
  c.define_alphabet_pair(string_copy("b"),
			 string_copy("b"));
  c.define_alphabet_pair(string_copy("c"),
			 string_copy("c"));

  Unweighted context(string_copy("a"),
                     string_copy("a"),
		     Rule::alphabet);
 
  c.add_rule(string_copy("Left rule"),
	     HFST::define_keypair(Rule::alphabet.get_key(string_copy("a")),
				  Rule::alphabet.get_key(string_copy("b"))),
	     LEFT_RULE,
	     context.copy());

  c.add_rule(string_copy("Left rule"),
  	     HFST::define_keypair(Rule::alphabet.get_key(string_copy("a")),
  				  Rule::alphabet.get_key(string_copy("b"))),
  	     LEFT_RULE,
  	     context.copy());
 
   c.add_rule(string_copy("Restriction rule"),
  	     HFST::define_keypair(Rule::alphabet.get_key(string_copy("a")),
  				  Rule::alphabet.get_key(string_copy("b"))),
  	     RESTRICTION_RULE,
	      context.copy());

  HFST::KeyPairSet * key_pair_set = HFST::create_empty_keypair_set();
  key_pair_set->insert(HFST::define_keypair
  		       (Rule::alphabet.get_key(string_copy("a")),
  			Rule::alphabet.get_key(string_copy("a"))));

    c.add_rule(string_copy("Right List rule"),
  	     key_pair_set,
  	     RIGHT_RULE,
  	     context.copy());

    key_pair_set = HFST::create_empty_keypair_set();
    key_pair_set->insert(HFST::define_keypair
  		       (Rule::alphabet.get_key(string_copy("a")),
  			Rule::alphabet.get_key(string_copy("a"))));
    
    c.add_rule(string_copy("Left List rule"),
	       key_pair_set,
	       LEFT_RULE,
	       context.copy());

    key_pair_set = HFST::create_empty_keypair_set();
    key_pair_set->insert(HFST::define_keypair
			 (Rule::alphabet.get_key(string_copy("a")),
			  Rule::alphabet.get_key(string_copy("a"))));

    c.add_rule(string_copy("Restriction List rule"),
	       key_pair_set,
	       RESTRICTION_RULE,
	       context);

    
  
    CompiledRule<Unweighted>::Vector * result = c();
  for (CompiledRule<Unweighted>::Vector::iterator it = result->begin();
       it != result->end();
       ++it)
    {
      CompiledRule<Unweighted>::Pair * p = *it;
      free(p->first);
      p->second.destroy_fst();
      delete p;
    }
  delete result;
  return true;
}

int main(void)
{
  if (not test())
    {
      exit(1);
    }
  else
    {
      exit(0);
    }
}
