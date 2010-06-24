/*
 * test.cc: A test file for hfst3
 */

#include "HfstTransducer.h"
#include <cstdio>
#include <assert.h>
#include <fstream>

using namespace hfst;
using namespace hfst::exceptions;

void print(HfstMutableTransducer &t)
{
  HfstStateIterator it(t);
  while (not it.done()) {
    HfstState s = it.value();
    HfstTransitionIterator IT(t,s);
    while (not IT.done()) {
      HfstTransition tr = IT.value();
      cout << s << "\t" << tr.get_target_state() << "\t"
	   << tr.get_input_symbol() << "\t" << tr.get_output_symbol()
	   << "\t" << tr.get_weight();
      cout << "\n";
      IT.next();
    }
    if ( t.is_final(s) )
      cout << s << "\t" << t.get_final_weight(s) << "\n";
    it.next();
  }
  return;
}

float func(float w) {
  return 2*w + 0.5;
}

void test_function( HfstTransducer& (HfstTransducer::*pt_function) (ImplementationType), 
		    HfstTransducer &tr0,
		    HfstTransducer &tr1,
		    HfstTransducer &tr2,
		    HfstTransducer &tr3 ) 
{
  HfstTransducer test0 = (HfstTransducer(tr0).*pt_function)(tr0.get_type()); 
  HfstTransducer test1 = (HfstTransducer(tr1).*pt_function)(tr1.get_type());
  HfstTransducer test2 = (HfstTransducer(tr2).*pt_function)(tr2.get_type());
  HfstTransducer test3 = (HfstTransducer(tr3).*pt_function)(tr3.get_type());
  
  /*assert ( test0.get_type() !=
	   test1.get_type() !=
	   test2.get_type() !=
	   test3.get_type() );*/

  assert (HfstTransducer::are_equivalent( test0, test1 ) );
  assert (HfstTransducer::are_equivalent( test0, test2 ) );
  assert (HfstTransducer::are_equivalent( test0, test3 ) );
}

void test_function( HfstTransducer& (HfstTransducer::*pt_function) (HfstTransducer&, ImplementationType), 
		    HfstTransducer &tr0,
		    HfstTransducer &tr1,
		    HfstTransducer &tr2,
		    HfstTransducer &tr3 ) 
{

  HfstTransducer test0s = HfstTransducer(tr0);
  HfstTransducer test1s = HfstTransducer(tr1);
  HfstTransducer test2s = HfstTransducer(tr2);
  HfstTransducer test3s = HfstTransducer(tr3);

  HfstTransducer test0 = (HfstTransducer(tr0).*pt_function)(test0s, tr0.get_type()); 
  HfstTransducer test1 = (HfstTransducer(tr1).*pt_function)(test1s, tr1.get_type());
  HfstTransducer test2 = (HfstTransducer(tr2).*pt_function)(test2s, tr2.get_type());
  HfstTransducer test3 = (HfstTransducer(tr3).*pt_function)(test3s, tr3.get_type());

  /*assert ( test0.get_type() !=
	   test1.get_type() !=
	   test2.get_type() !=
	   test3.get_type() );*/
 
  assert (HfstTransducer::are_equivalent( test0, test1 ) );
  assert (HfstTransducer::are_equivalent( test0, test2 ) );
  assert (HfstTransducer::are_equivalent( test0, test3 ) );
}

void test_extract_strings( HfstTransducer &t )
{
  try {
    WeightedPaths<float>::Set results;
    t.extract_strings(results);
  }
  catch (TransducerIsCyclicException e) {}
  
  HfstTokenizer tok;
  HfstTransducer tr1("aa", "aa", tok, t.get_type());
  HfstTransducer tr2("acb", "acb", tok, t.get_type());
  HfstTransducer tr3("bb", "bb", tok, t.get_type());
  HfstTransducer disj = tr1.disjunct(tr2);
  disj = disj.disjunct(tr3);
  disj = disj.minimize();

  try {
    WeightedPaths<float>::Set results;
    disj.extract_strings(results);
    assert(3 == (int)results.size());
    HfstTransducer all_paths(t.get_type());
    for (WeightedPaths<float>::Set::iterator it = results.begin();
	 it != results.end(); it++)
      {
	WeightedPath<float> wp = *it;
	HfstTransducer one_path(wp.istring, wp.ostring, tok, t.get_type());
	one_path.set_final_weights(wp.weight);
	all_paths.disjunct(one_path);
      }

    assert( HfstTransducer::are_equivalent(all_paths, disj) );
  }
  catch (HfstInterfaceException e) { 
    assert(false); }
}

int main(int argc, char **argv) {

  {
    HfstTransducer anon1(3,4,TROPICAL_OFST_TYPE);
    HfstTransducer anon2(4,1,TROPICAL_OFST_TYPE);
    anon1.disjunct(anon2);
    anon1.minimize();
    std::cerr << anon1 << "\n";
  }

  {
    HfstTransducer anon1(3,4,LOG_OFST_TYPE);
    HfstTransducer anon2(4,1,LOG_OFST_TYPE);
    anon1.disjunct(anon2);
    anon1.minimize();
    std::cerr << anon1 << "\n";
  }

  {
    HfstTransducer anon1(3,4,SFST_TYPE);
    HfstTransducer anon2(4,1,SFST_TYPE);
    anon1.disjunct(anon2);
    anon1.minimize();
    std::cerr << anon1 << "\n";
  }

  {
    HfstTransducer anon1(3,4,FOMA_TYPE);
    HfstTransducer anon2(4,1,FOMA_TYPE);
    anon1.disjunct(anon2);
    anon1.minimize();
    std::cerr << anon1 << "\n";
  }

  // HfstTransducer two_level_if(HfstTransducerPair &context, StringPairSet &mappings, StringPairSet &alphabet, ImplementationType type);

  ImplementationType types [] = {SFST_TYPE, TROPICAL_OFST_TYPE, FOMA_TYPE};
  HfstTransducer * rule_transducers1 [3]; 
  HfstTransducer * rule_transducers2 [3]; 
  HfstTransducer * rule_transducers3 [3]; 

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

    HfstTransducer rule_transducer1 = rules::two_level_if(context, mappings, alphabet);
    HfstTransducer rule_transducer2 = rules::two_level_only_if(context, mappings, alphabet);
    HfstTransducer rule_transducer3 = rules::two_level_if_and_only_if(context, mappings, alphabet);
    rule_transducers1[i] = new HfstTransducer(rule_transducer1);
    rule_transducers2[i] = new HfstTransducer(rule_transducer2);
    rule_transducers3[i] = new HfstTransducer(rule_transducer3);

    //rule_transducer1.minimize();    
    //std::cerr << rule_transducer1 << "\n"; 

    //rule_transducer2.minimize();    
    //std::cerr << rule_transducer2 << "\n"; 

    //rule_transducer3.minimize();    
    //std::cerr << rule_transducer3 << "\n"; 
  }

  rule_transducers1[0]->convert(TROPICAL_OFST_TYPE);
  rule_transducers1[2]->convert(TROPICAL_OFST_TYPE);

  rule_transducers2[0]->convert(TROPICAL_OFST_TYPE);
  rule_transducers2[2]->convert(TROPICAL_OFST_TYPE);

  rule_transducers3[0]->convert(TROPICAL_OFST_TYPE);
  rule_transducers3[2]->convert(TROPICAL_OFST_TYPE);

  assert( HfstTransducer::are_equivalent(*rule_transducers1[0], *rule_transducers1[1]) );
  assert( HfstTransducer::are_equivalent(*rule_transducers1[0], *rule_transducers1[2]) );

  assert( HfstTransducer::are_equivalent(*rule_transducers2[0], *rule_transducers2[1]) );
  assert( HfstTransducer::are_equivalent(*rule_transducers2[0], *rule_transducers2[2]) );

  assert( HfstTransducer::are_equivalent(*rule_transducers3[0], *rule_transducers3[1]) );
  assert( HfstTransducer::are_equivalent(*rule_transducers3[0], *rule_transducers3[2]) );

  // HfstTransducer & replace_in_context(HfstTransducerPair &context, ReplaceType repl_type, HfstTransducer &t, bool optional, StringPairSet &alphabet)

  {
    ImplementationType type = TROPICAL_OFST_TYPE;
    HfstTransducer left("c",type);
    HfstTransducer right("c",type);
    HfstTransducerPair context(left,right);
    HfstTransducer mapping("a","b",type);
    HfstTransducer mapping2("b","c",type);
    mapping.disjunct(mapping2);
    StringPairSet alphabet;
    alphabet.insert(StringPair("a","a"));
    alphabet.insert(StringPair("b","b"));
    alphabet.insert(StringPair("c","c"));
    HfstTransducer rule = rules::replace_up(context, mapping, false, alphabet);
    rule.minimize();
    std::cerr << rule << "\n";
  }

  exit(0);

#ifdef FOO

  //HfstTransducer t(TROPICAL_OFST_TYPE);
  //t.test_minimize();
  //exit(0);

  ImplementationType types[] = {SFST_TYPE, TROPICAL_OFST_TYPE, LOG_OFST_TYPE, FOMA_TYPE};
  for (int i=0; i<4; i++) {
    {
      // Test the empty transducer constructors for all implementation types.
      HfstTransducer tr(types[i]);
      assert (tr.get_type() == types[i]);
      for (int j=0; j<4; j++) {
	// Test the conversions.
	HfstTransducer trconv = tr.convert(types[j]);
	assert (tr.get_type() == types[j]);
	std::cerr << tr << "--\n" << trconv;
	assert (HfstTransducer::are_equivalent(tr, trconv));
	HfstTransducer tranother(types[j]);
	assert (HfstTransducer::are_equivalent(tr, tranother));
      }
      HfstMutableTransducer mut(tr);
      HfstTransducer foo(mut);
      assert (HfstTransducer::are_equivalent(tr, foo));
    }
    //printf("Empty constructors tested on transducers of type %i.\n", types[i]);
    {
      // Test the one-transition transducer constructors for all implementation types.
      HfstTransducer tr("foo", types[i]);
      assert (tr.get_type() == types[i]);
      for (int j=0; j<4; j++) {
	// Test the conversions.
	HfstTransducer trconv = tr.convert(types[j]);
	assert (tr.get_type() == types[j]);
	assert (HfstTransducer::are_equivalent(tr, trconv));
	HfstTransducer tranother("foo", types[j]);
	assert (HfstTransducer::are_equivalent(tr, tranother));
      }
      HfstMutableTransducer mut(tr);
      HfstTransducer foo(mut);
      assert (HfstTransducer::are_equivalent(tr, foo));
    }
    //printf("One-transition constructors tested on transducers of type %i.\n", types[i]);
    {
      // Test the two-transition transducer constructors for all implementation types.
      HfstTransducer tr("foo", "bar", types[i]);
      assert (tr.get_type() == types[i]);
      for (int j=0; j<4; j++) {
	// Test the conversions.
	HfstTransducer trconv = tr.convert(types[j]);
	assert (tr.get_type() == types[j]);
	assert (HfstTransducer::are_equivalent(tr, trconv));
	HfstTransducer tranother("foo", "bar", types[j]);
	assert (HfstTransducer::are_equivalent(tr, tranother));
	// test the att format
	tr.write_in_att_format("testfile");
	HfstTransducer foo = HfstTransducer::read_in_att_format("testfile");  // WrongTypeException
	assert (HfstTransducer::are_equivalent(tr, foo));
      }
      HfstMutableTransducer mut(tr);
      HfstTransducer foo(mut);
      assert (HfstTransducer::are_equivalent(tr, foo));
    }
    //printf("Two-transition constructors tested on transducers of type %i.\n", types[i]);
    {
      // Test the one-string transducer constructors for all implementation types.
      HfstTokenizer tok;
      tok.add_multichar_symbol("foo");
      tok.add_multichar_symbol("bar");
      tok.add_skip_symbol("fo");
      tok.add_skip_symbol("a");
      HfstTransducer tr("foobara", tok, types[i]);
      //std::cerr << "tokenized:\n" << tr << "\n\n";
      assert (tr.get_type() == types[i]);
      for (int j=0; j<4; j++) {
	// Test the conversions.
	HfstTransducer trconv = tr.convert(types[j]);
	assert (tr.get_type() == types[j]);
	assert (HfstTransducer::are_equivalent(tr, trconv));
	HfstTransducer tranother("foobar", tok, types[j]);
	assert (HfstTransducer::are_equivalent(tr, tranother));
      }
      HfstMutableTransducer mut(tr);
      HfstTransducer foo(mut);
      assert (HfstTransducer::are_equivalent(tr, foo));
    }
    //printf("One-string constructors tested on transducers of type %i.\n", types[i]);
    {
      // Test the two-string transducer constructors for all implementation types.
      HfstTokenizer tok;
      tok.add_multichar_symbol("foo");
      tok.add_multichar_symbol("bar");
      HfstTransducer tr("fofoo", "barbarba", tok, types[i]);
      assert (tr.get_type() == types[i]);
      for (int j=0; j<4; j++) {
	// Test the conversions.
	HfstTransducer trconv = tr.convert(types[j]);
	assert (tr.get_type() == types[j]);
	assert (HfstTransducer::are_equivalent(tr, trconv));
	HfstTransducer tranother("fofoo", "barbarba", tok, types[j]);
	assert (HfstTransducer::are_equivalent(tr, tranother));
      }
      HfstMutableTransducer mut(tr);
      HfstTransducer foo(mut);
      assert (HfstTransducer::are_equivalent(tr, foo));
    }
    //printf("Two-string constructors tested on transducers of type %i.\n", types[i]);
    {
      HfstTransducer *pt = new HfstTransducer(types[i]);
      delete pt;
    }
    printf("Constructors and delete tested on transducers of type %i.\n", types[i]);
  }

  {
      HfstTransducer foo = HfstTransducer::read_in_att_format("test_transducer.att");
      HfstTransducer test0 = HfstTransducer(foo);
      HfstTransducer test1 = HfstTransducer(foo);
      HfstTransducer test2 = HfstTransducer(foo);
      HfstTransducer test3 = HfstTransducer(foo);
      test0.convert(types[0]);
      test1.convert(types[1]);
      test2.convert(types[2]);
      test3.convert(types[3]);

      assert ( test0.get_type() !=
	       test1.get_type() !=
	       test2.get_type() !=
	       test3.get_type() );
      

      // ----- repeat_star -----
      test_function(&HfstTransducer::repeat_star, test0, test1, test2, test3);
      printf("repeat_star tested\n");


      // ----- repeat_plus -----
      test_function(&HfstTransducer::repeat_plus, test0, test1, test2, test3);
      printf("repeat_plus tested\n");


      // ----- repeat_n -----
      {
	const int MAX_REPEAT_N = 5; 
	for (int n=0; n <= MAX_REPEAT_N; n++) {

	  HfstTransducer test0_n = HfstTransducer(test0).repeat_n(n); 
	  HfstTransducer test1_n = HfstTransducer(test1).repeat_n(n);
	  HfstTransducer test2_n = HfstTransducer(test2).repeat_n(n);
	  HfstTransducer test3_n = HfstTransducer(test3).repeat_n(n);
	
	assert ( test0_n.get_type() !=
		 test1_n.get_type() !=
		 test2_n.get_type() !=
		 test3_n.get_type() );

	//	std::cerr << test0_n << "--\n" << test3_n << "\n";
	  assert (HfstTransducer::are_equivalent( test0_n, test1_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test2_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test3_n ) );
	}
      }
      printf("repeat_n tested\n");


      // ----- repeat_n_minus -----
      {
	const int MAX_REPEAT_N = 5; 
	for (int n=0; n <= MAX_REPEAT_N; n++) {
	  HfstTransducer test0_n = HfstTransducer(test0).repeat_n_minus(n); 
	  HfstTransducer test1_n = HfstTransducer(test1).repeat_n_minus(n);
	  HfstTransducer test2_n = HfstTransducer(test2).repeat_n_minus(n);
	  HfstTransducer test3_n = HfstTransducer(test3).repeat_n_minus(n);
	
	  assert ( test0_n.get_type() !=
		   test1_n.get_type() !=
		   test2_n.get_type() !=
		   test3_n.get_type() );

	  assert (HfstTransducer::are_equivalent( test0_n, test1_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test2_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test3_n ) );
	}
      }
      printf("repeat_n_minus tested\n");


      // ----- repeat_n_plus -----
      {
	const int MAX_REPEAT_N = 1; 
	for (int n=0; n <= MAX_REPEAT_N; n++) {
	  HfstTransducer test0_n(test0);
	  test0_n.repeat_n_plus(n);

	  HfstTransducer test1_n(test1);
	  test1_n.repeat_n_plus(n);

	  HfstTransducer test2_n(test2);
	  test2_n.repeat_n_plus(n);

	  HfstTransducer test3_n(test3);
	  test3_n.repeat_n_plus(n);
	  //HfstTransducer test0_n = HfstTransducer(test0).repeat_n_plus(n); 
	  //HfstTransducer test1_n = HfstTransducer(test1).repeat_n_plus(n);
	  //HfstTransducer test2_n = HfstTransducer(test2).repeat_n_plus(n);
	  //HfstTransducer test3_n = HfstTransducer(test3).repeat_n_plus(n);
	
	  assert ( test0_n.get_type() !=
		   test1_n.get_type() !=
		   test2_n.get_type() !=
		   test3_n.get_type() );

	  assert (HfstTransducer::are_equivalent( test0_n, test1_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test2_n ) );
	  assert (HfstTransducer::are_equivalent( test0_n, test3_n ) );	  
	}
      }
      printf("repeat_n_plus tested\n");


      // ----- repeat_n_to_k -----
      {
	const int REPEAT_N_MAX = 2; 
	const int REPEAT_K_MAX = 3;
	for (int n=0; n <= REPEAT_N_MAX; n++) {
	  for (int k=n; k <= REPEAT_K_MAX; k++) {

	    HfstTransducer test0_n(test0);
	    test0_n.repeat_n_to_k(n,k); 
	    HfstTransducer test1_n(test1);
	    test1_n.repeat_n_to_k(n,k); 
	    HfstTransducer test2_n(test2);
	    test2_n.repeat_n_to_k(n,k); 
	    HfstTransducer test3_n(test3);
	    test3_n.repeat_n_to_k(n,k); 

	    //HfstTransducer test0_n = HfstTransducer(test0).repeat_n_to_k(n,k); 
	    //HfstTransducer test1_n = HfstTransducer(test1).repeat_n_to_k(n,k);
	    //HfstTransducer test2_n = HfstTransducer(test2).repeat_n_to_k(n,k);
	    //HfstTransducer test3_n = HfstTransducer(test3).repeat_n_to_k(n,k);
	    
	    assert ( test0_n.get_type() !=
		     test1_n.get_type() !=
		     test2_n.get_type() !=
		     test3_n.get_type() );

	    assert (HfstTransducer::are_equivalent( test0_n, test1_n ) );
	    assert (HfstTransducer::are_equivalent( test0_n, test2_n ) );
	    assert (HfstTransducer::are_equivalent( test0_n, test3_n ) );
	  }
	}
      }
      printf("repeat_n_to_k tested\n");

      // ----- optionalize -----
      test_function(&HfstTransducer::optionalize, test0, test1, test2, test3);
      printf("optionalize tested\n");

      // ----- invert -----
      test_function(&HfstTransducer::invert, test0, test1, test2, test3);
      printf("invert tested\n");

      // ----- reverse -----
      test_function(&HfstTransducer::reverse, test0, test1, test2, test3);
      printf("reverse tested\n");

      // ----- input_project -----
      test_function(&HfstTransducer::input_project, test0, test1, test2, test3);
      printf("input_project tested\n");

      // ----- output_project -----
      test_function(&HfstTransducer::output_project, test0, test1, test2, test3);
      printf("output_project tested\n");

      // ----- substitute string -----
      {
	HfstTransducer test0s = HfstTransducer(test0).substitute("a", "e", test0.get_type());
	HfstTransducer test1s = HfstTransducer(test1).substitute("a", "e", test1.get_type());
	HfstTransducer test2s = HfstTransducer(test2).substitute("a", "e", test2.get_type());
	HfstTransducer test3s = HfstTransducer(test3).substitute("a", "e", test3.get_type());
	
	assert (HfstTransducer::are_equivalent( test0s, test1s ) );
	assert (HfstTransducer::are_equivalent( test0s, test2s ) );
	assert (HfstTransducer::are_equivalent( test0s, test3s ) );
      }
      printf("substitute(string) tested\n");

      // ----- substitute string pair -----
      {
	HfstTransducer test0s = HfstTransducer(test0).substitute(StringPair("a", "b"), StringPair("e", "f"));
	HfstTransducer test1s = HfstTransducer(test1).substitute(StringPair("a", "b"), StringPair("e", "f"));
	HfstTransducer test2s = HfstTransducer(test2).substitute(StringPair("a", "b"), StringPair("e", "f"));
	HfstTransducer test3s = HfstTransducer(test3).substitute(StringPair("a", "b"), StringPair("e", "f"));
	
	assert (HfstTransducer::are_equivalent( test0s, test1s ) );
	assert (HfstTransducer::are_equivalent( test0s, test2s ) );
	assert (HfstTransducer::are_equivalent( test0s, test3s ) );
      }
      printf("substitute(StringPair) tested\n");

      // ----- substitute string pair with transducer -----
      {
	StringPair ssp("a", "b");
	HfstTransducer r0(test0);
	HfstTransducer r1(test1);
	HfstTransducer r2(test2);
	HfstTransducer r3(test3);
	HfstTransducer test0s = HfstTransducer(test0).substitute(ssp, r3);
	HfstTransducer test1s = HfstTransducer(test1).substitute(ssp, r2);
	HfstTransducer test2s = HfstTransducer(test2).substitute(ssp, r1);
	HfstTransducer test3s = HfstTransducer(test3).substitute(ssp, r0);
	
	assert (HfstTransducer::are_equivalent( test0s, test1s ) );
	assert (HfstTransducer::are_equivalent( test0s, test2s ) );
	assert (HfstTransducer::are_equivalent( test0s, test3s ) );
      }
      printf("substitute(StringPair, HfstTransducer) tested\n");

      // ----- set_final_weights -----
      { 
	float weight=0.5;

	HfstTransducer test0s = HfstTransducer(test0).set_final_weights(weight);
	HfstTransducer test1s = HfstTransducer(test1).set_final_weights(weight);
	HfstTransducer test2s = HfstTransducer(test2).set_final_weights(weight);
	HfstTransducer test3s = HfstTransducer(test3).set_final_weights(weight);

	assert (HfstTransducer::are_equivalent( test0s, test3s ) );
	assert (HfstTransducer::are_equivalent( test1s, test2s ) );
      }
      printf("set_final_weights tested\n");

      // ----- transform_weights -----
      { 
	HfstTransducer test0s = HfstTransducer(test0).transform_weights(&func);
	HfstTransducer test1s = HfstTransducer(test1).transform_weights(&func);
	HfstTransducer test2s = HfstTransducer(test2).transform_weights(&func);
	HfstTransducer test3s = HfstTransducer(test3).transform_weights(&func);

	assert (HfstTransducer::are_equivalent( test0s, test3s ) );
	assert (HfstTransducer::are_equivalent( test1s, test2s ) );
      }
      printf("transform_weights tested\n");

      // ----- extract_strings -----
      {
	test_extract_strings(test0);
	test_extract_strings(test1);
	test_extract_strings(test2);
	test_extract_strings(test3);
      }
      printf("extract_strings tested\n");

      // ----- compose -----
      test_function(&HfstTransducer::compose, test0, test1, test2, test3);
      printf("compose tested\n");

      // ----- concatenate -----
      test_function(&HfstTransducer::concatenate, test0, test1, test2, test3);
      printf("concatenate tested\n");

      // ----- disjunct -----
      test_function(&HfstTransducer::disjunct, test0, test1, test2, test3);
      printf("disjunct tested\n");

      // ----- intersect -----
      test_function(&HfstTransducer::intersect, test0, test1, test2, test3);
      printf("intersect tested\n");

      // ----- subtract -----
      test_function(&HfstTransducer::subtract, test0, test1, test2, test3);
      printf("subtract tested\n");

      // ----- remove_epsilons -----
      test_function(&HfstTransducer::remove_epsilons, test0, test1, test2, test3);
      printf("remove_epsilons tested\n");

      // ----- determinize -----
      test_function(&HfstTransducer::determinize, test0, test1, test2, test3);
      printf("determinize tested\n");

      // ----- minimize -----
      test_function(&HfstTransducer::minimize, test0, test1, test2, test3);
      printf("minimize tested\n");

      // ----- write_in_att_format and read_in_att_format -----
      {
	FILE * ofile = fopen("test_transducer2.att", "wb");
	if (ofile == NULL) {
	  printf("File could not be opened for writing.\n");
	  exit(1);
	}
	HfstTransducer(test0).write_in_att_format(ofile);
	fprintf(ofile, "--\n");
	HfstTransducer(test1).write_in_att_format(ofile);
	fprintf(ofile, "--\n");
	HfstTransducer(test2).write_in_att_format(ofile);
	fprintf(ofile, "--\n");
	HfstTransducer(test3).write_in_att_format(ofile);
	fclose(ofile);
	
	FILE * ifile = fopen("test_transducer2.att", "rb");
	if (ifile == NULL) {
	  printf("File could not be opened for reading.\n");
	  exit(1);
	}
	HfstTransducer read_transducers [4] = { HfstTransducer(TROPICAL_OFST_TYPE),
						HfstTransducer(TROPICAL_OFST_TYPE),
						HfstTransducer(TROPICAL_OFST_TYPE),
						HfstTransducer(TROPICAL_OFST_TYPE) };
	int n=0;
	while (not feof(ifile)) {
	  read_transducers[n] = HfstTransducer::read_in_att_format(ifile);
	  n++;
	}
	assert( n == 4 );
	assert ( HfstTransducer::are_equivalent( read_transducers[0], test0 ) );
	assert ( HfstTransducer::are_equivalent( read_transducers[1], test1 ) );
	assert ( HfstTransducer::are_equivalent( read_transducers[2], test2 ) );
	assert ( HfstTransducer::are_equivalent( read_transducers[3], test3 ) );
      }
      printf("write_in_att_format and read_in_att_format tested\n");

      // this test is too sensitive for differences in the number of tabs/spaces
      // basically this function is tested in write_in_att_format and read_in_att_format 
      /* ----- operator<< -----
      {
	std::filebuf fb;
	fb.open ("foo",std::ios::out);
	std::ostream os(&fb);
	os << test0 << "--\n" << test1 << "--\n" << test2 << "--\n" << test3;	
	fb.close();
	// note: the return value of 'system(char*)' is system-dependent
	assert ( 0 == system("diff foo operator_ltlt_test.att") );
      }
      printf("operator<< tested\n"); */

      // ----- -----
      //test_function(&HfstTransducer::*, test0, test1, test2, test3);
      //printf("* tested\n");

  }

  return 0;

#endif

}


#ifdef FOO

  // create transducer t1
  HfstMutableTransducer t1;
  HfstState second_state1 = t1.add_state();
  HfstState third_state1 = t1.add_state();
  t1.set_final_weight(second_state1, 0.5);
  t1.add_transition(0, "foo", "bar", 0.3, second_state1);
  t1.add_transition(second_state1, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 0.2, third_state1);
  t1.add_transition(third_state1, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0.1, second_state1);

  // create transducer t2
  HfstMutableTransducer t2;
  HfstState second_state2 = t2.add_state();
  t2.set_final_weight(second_state2, 0.3);
  t2.add_transition(0, "@_UNKNOWN_SYMBOL_@", "baz", 1.6, second_state2);


  HfstTokenizer TOK;
  TOK.add_multichar_symbol("foo");
  TOK.add_multichar_symbol("bar");
  HfstTransducer TOK_TR("fooofoooa", "barrbabarr", TOK, TROPICAL_OFST_TYPE);
  TOK_TR.print();

  HfstTokenizer TOK2;
  TOK2.add_multichar_symbol("fii");
  TOK2.add_multichar_symbol("baar");
  HfstTransducer TOK_TR2("foofii", "barbaarq", TOK2, TROPICAL_OFST_TYPE);
  TOK_TR2.print();

  HfstTransducer TOK_CAT = TOK_TR.concatenate(TOK_TR2);
  TOK_CAT.print();


  ImplementationType types[] = {TROPICAL_OFST_TYPE, LOG_OFST_TYPE, SFST_TYPE, FOMA_TYPE};
  for (int i=0; i<4; i++) 
    {      
      fprintf(stderr, "testing transducers of type %i\n", types[i]);

      // open two output streams to file
      HfstOutputStream out1("test1.hfst", types[i]);
      out1.open();
      HfstOutputStream out2("test2.hfst", types[i]);
      out2.open();

      // convert both transducers and write them to the streams
      HfstTransducer T1(t1);
      T1 = T1.convert(types[i]);
      HfstTransducer T2(t2);
      T2 = T2.convert(types[i]);
      out1 << T1;
      out1 << T2;
      out2 << T2;
      out2 << T1;
      out1.close();
      out2.close();

      // open two input streams to the files
      HfstInputStream in1("test1.hfst");
      in1.open();
      HfstInputStream in2("test2.hfst");
      in2.open();

      while (not in1.is_eof() && not in2.is_eof()) {
	fprintf(stderr, " while loop\n");
	HfstTransducer tr1(in1);
	HfstTransducer tr2(in2);

	{ HfstTransducer t = tr1.compose(tr2);
	  fprintf(stderr, "  composed\n"); }
	{ HfstTransducer t = tr1.intersect(tr2);
	  fprintf(stderr, "  intersected\n"); }
	{ HfstTransducer t = tr1.disjunct(tr2);
	  fprintf(stderr, "  disjuncted\n"); }
	{ HfstTransducer t = tr1.concatenate(tr2);
	  fprintf(stderr, "  concatenated\n"); }
	{ HfstTransducer t = tr1.subtract(tr2);
	  fprintf(stderr, "  subtracted\n"); }

	{ HfstTransducer t = tr1.remove_epsilons();
	  fprintf(stderr, "  removed epsilons\n"); }
	{ HfstTransducer t = tr1.determinize();
	  fprintf(stderr, "  determinized\n"); }
	{ HfstTransducer t = tr1.minimize();
	  fprintf(stderr, "  minimized\n"); }

	{ HfstTransducer t = tr1.repeat_star();
	  fprintf(stderr, "  repeated star\n"); }
	{ HfstTransducer t = tr1.repeat_plus();
	  fprintf(stderr, "  repeated plus\n"); }
	{ HfstTransducer t = tr1.repeat_n(3);
	  fprintf(stderr, "  repeated n\n"); }
	{ HfstTransducer t = tr1.repeat_n_minus(3);
	  fprintf(stderr, "  repeated n minus\n"); }
	{ HfstTransducer t = tr1.repeat_n_plus(3);
	  fprintf(stderr, "  repeated n plus\n"); }
	{ HfstTransducer t = tr1.repeat_n_to_k(1, 4);
	  fprintf(stderr, "  repeated n to k\n"); }
	{ HfstTransducer t = tr1.optionalize();
	  fprintf(stderr, "  optionalized\n"); }

	{ HfstTransducer t = tr1.invert();
	  fprintf(stderr, "  inverted\n"); }
	{ HfstTransducer t = tr1.input_project();
	  fprintf(stderr, "  input projected\n"); }
	{ HfstTransducer t = tr1.output_project();
	  fprintf(stderr, "  output projected\n"); }
	{ HfstTransducer t = tr1.reverse();
	  fprintf(stderr, "  reversed\n"); }
	{ HfstTransducer t = tr1.substitute(std::string(),
					    std::string();
	  fprintf(stderr, "  substituted string\n"); }
	{ HfstTransducer t = tr1.substitute(const StringPair &old_symbol_pair,
					    const StringPair &new_symbol_pair);
	  fprintf(stderr, "  substituted string pair\n"); }

      }

      remove("test1.hfst");
      remove("test2.hfst");

    }

  return 0;
}

#endif






#ifdef foo

    HfstTransducer &remove_epsilons(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &determinize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &minimize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &n_best(int n,ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_star(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_plus(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n_minus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n_plus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &optionalize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &invert(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &input_project(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &output_project(ImplementationType type=UNSPECIFIED_TYPE);
    void extract_strings(WeightedStrings<float>::Set &results);
    HfstTransducer &substitute(Key old_key, Key new_key);
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol);
    HfstTransducer &substitute(const KeyPair &old_key_pair, 
			       const KeyPair &new_key_pair);
    HfstTransducer &substitute(const StringPair &old_symbol_pair,
			       const StringPair &new_symbol_pair);
    HfstTransducer &compose(HfstTransducer &another,
			    ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &concatenate(HfstTransducer &another,
				ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &disjunct(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &intersect(HfstTransducer &another,
			      ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &subtract(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);



  fprintf(stderr, "disjunction:\n");

  HfstTransducer DISJ = T1.disjunct(T1, SFST_TYPE);

  fprintf(stderr, "disjunction done\n");

  HfstMutableTransducer Disj = HfstMutableTransducer(DISJ);
  fprintf(stderr, "converted to mutable\n");
  print(Disj);


  //HfstTransducer T(t);
  ImplementationType type = T.get_type();
  //fprintf(stderr, "%i\n", type);

  //fprintf(stderr, "main: (1)\n");
  T = T.convert(FOMA_TYPE);
  //fprintf(stderr, "main: (1.5)\n");

  HfstTransducer Tcopy = HfstTransducer(T);
  //fprintf(stderr, "main: (1.51)\n");
  HfstTransducer disj = T.disjunct(Tcopy, FOMA_TYPE);

  //fprintf(stderr, "main: (2)\n");
  T = T.convert(SFST_TYPE);
  //fprintf(stderr, "main: (3)\n");
  T = T.convert(TROPICAL_OFST_TYPE);
  //fprintf(stderr, "main: (4)\n");
  T = T.convert(FOMA_TYPE);
  //fprintf(stderr, "main: (5)\n");
  type = T.get_type();
  //fprintf(stderr, "%i\n", type);
  //HfstOutputStream os(FOMA_TYPE);
  //os << T;
  //fprintf(stderr, "\n\n");
  //cout << T;
  //fprintf(stderr, "\n");
  //fprintf(stderr, "main: (6)\n");
  // FIX: calling ~HfstTransducer causes a glibc with foma
  return 0;
#endif

