/*
 * A test file for functions in namespace hfst::rules.
 */

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

#include <cstdio>
#include <assert.h>
#include <fstream>

using namespace hfst;
;


void compare_and_delete(HfstTransducer * rule_transducers1[],
			HfstTransducer * rule_transducers2[],
			HfstTransducer * rule_transducers3[])
{
  rule_transducers1[0]->convert(TROPICAL_OPENFST_TYPE);
  rule_transducers1[2]->convert(TROPICAL_OPENFST_TYPE);

  rule_transducers2[0]->convert(TROPICAL_OPENFST_TYPE);
  rule_transducers2[2]->convert(TROPICAL_OPENFST_TYPE);

  rule_transducers3[0]->convert(TROPICAL_OPENFST_TYPE);
  rule_transducers3[2]->convert(TROPICAL_OPENFST_TYPE);

  assert(rule_transducers1[0]->compare(*rule_transducers1[1]) );
  assert(rule_transducers1[0]->compare(*rule_transducers1[2]) );
  assert(rule_transducers2[0]->compare(*rule_transducers2[1]) );
  assert(rule_transducers2[0]->compare(*rule_transducers2[2]) );
  assert(rule_transducers3[0]->compare(*rule_transducers3[1]) );
  assert(rule_transducers3[0]->compare(*rule_transducers3[2]) );

  for (int i=0; i<3; i++) {
    delete rule_transducers1[i];
    delete rule_transducers2[i];
    delete rule_transducers3[i];
  }
}

int main(int argc, char **argv) {

 
  ImplementationType types [] = {SFST_TYPE, TROPICAL_OPENFST_TYPE, FOMA_TYPE};
  HfstTransducer * rule_transducers1 [3]; 
  HfstTransducer * rule_transducers2 [3]; 
  HfstTransducer * rule_transducers3 [3]; 


  /* HfstTransducer two_level_if(
     HfstTransducerPair &context, 
     StringPairSet &mappings, 
     StringPairSet &alphabet, 
     ImplementationType type); */

  verbose_print("HfstTransducer two_level_if("
		"HfstTransducerPair &context," 
		"StringPairSet &mappings," 
		"StringPairSet &alphabet," 
		"ImplementationType type");

  {
    for (int i=0; i<3; i++) {
      HfstTransducer leftc("c", types[i]);
      HfstTransducer rightc("c", types[i]);
      HfstTransducerPair context(leftc, rightc);
      StringPair mapping("a", "b");
      StringPairSet mappings;
      mappings.insert(mapping);
      StringPairSet alphabet;
      alphabet.insert(StringPair("a", "a"));
      alphabet.insert(StringPair("a", "b"));
      alphabet.insert(StringPair("b", "b"));
      alphabet.insert(StringPair("c", "c"));
      
      HfstTransducer rule_transducer1 
	= rules::two_level_if(context, mappings, alphabet);
      HfstTransducer rule_transducer2 
	= rules::two_level_only_if(context, mappings, alphabet);
      HfstTransducer rule_transducer3 
	= rules::two_level_if_and_only_if(context, mappings, alphabet);
      rule_transducers1[i] = new HfstTransducer(rule_transducer1);
      rule_transducers2[i] = new HfstTransducer(rule_transducer2);
      rule_transducers3[i] = new HfstTransducer(rule_transducer3);
    }
  }

  compare_and_delete(rule_transducers1,
		     rule_transducers2,
		     rule_transducers3);

  // replace_down
  // FIXME: temporarily omitted since the replace_down function copied
  // from SFST has a bug..

  /*
  {
    for (int i=0; i<3; i++) {
      HfstTokenizer TOK;
      HfstTransducer mapping("ab", "x", TOK, types[i]);
      HfstTransducer left_context("ab", "ab", TOK, types[i]);
      HfstTransducer right_context("a", types[i]);
      HfstTransducerPair context(left_context, right_context);
      StringPairSet alphabet;
      alphabet.insert(StringPair("a", "a"));
      alphabet.insert(StringPair("b", "b"));
      alphabet.insert(StringPair("x", "x"));
      bool optional = false;
      
      HfstTransducer replace_down_transducer
	= rules::replace_down(context, mapping, optional, alphabet);
      
      HfstTransducer test_abababa("abababa", TOK, types[i]);
      test_abababa.compose(replace_down_transducer);
      HfstTransducer abxaba("abxaba", TOK, types[i]);
      HfstTransducer ababxa("ababxa", TOK, types[i]);
      HfstTransducer expected_result(types[i]);
      expected_result.disjunct(abxaba);
      expected_result.disjunct(ababxa);
      assert(expected_result.compare(test_abababa));
    }
    } */

#ifdef FOO
  /* HfstTransducer &replace_in_context(
     HfstTransducerPair &context, 
     ReplaceType repl_type, 
     HfstTransducer &t, 
     bool optional, 
     StringPairSet &alphabet) */
  
  verbose_print("HfstTransducer &replace_in_context("
		"HfstTransducerPair &context," 
		"ReplaceType repl_type," 
		"HfstTransducer &t," 
		"bool optional," 
		"StringPairSet &alphabet");

  for (int i=0; i<3; i++) {
    HfstTransducer left("c",types[i]);
    HfstTransducer right("c",types[i]);
    HfstTransducerPair context(left,right);
    HfstTransducer mapping("a","b",types[i]);
    HfstTransducer mapping2("b","c",types[i]);
    mapping.disjunct(mapping2);
    StringPairSet alphabet;
    alphabet.insert(StringPair("a","a"));
    alphabet.insert(StringPair("b","b"));
    alphabet.insert(StringPair("c","c"));
    HfstTransducer rule = rules::replace_up(context, mapping, false, alphabet);
  }
#endif
    
}



