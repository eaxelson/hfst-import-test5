/*
   Test file for HfstTransducer member functions:
   - compare
   - compose
   - convert
   - extract_strings and extract_strings_fd
   - insert_freely
   - is_cyclic
   - is_lookup_infinitely_ambiguous, lookup and lookup_fd
   - n_best
   - push_weights
   - set_final_weights
   - substitute
   - transform_weights
   
   Functions:
      concatenate 
      determinize 
      disjunct 
      input_project 
      intersect 
      invert 
      minimize 
      optionalize 
      output_project 
      remove_epsilons 
      repeat_n 
      repeat_n_minus 
      repeat_n_plus 
      repeat_n_to_k 
      repeat_plus 
      repeat_star 
      reverse 
      subtract 
   are tested in command line tool tests

   TODO: What about 'get_alphabet etc'?

*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

using hfst::implementations::HfstBasicTransition;
using hfst::implementations::HfstBasicTransducer;

typedef std::vector<std::string> StringVector;

bool compare_string_vectors(const StringVector &v1, const StringVector &v2)
{
  if (v1.size() != v2.size())
    return false;
  for (unsigned int i=0; i<v1.size(); i++)
    {
      if (v1[i].compare(v2[i]) != 0)
	return false;
    }
  return true;
}

bool do_hfst_lookup_paths_contain(const HfstLookupPaths &results,
				  const HfstLookupPath &expected_path,
				  float path_weight=0,
				  bool test_path_weight=false)
{
  bool found=false;
  float weight=0;
  for (HfstLookupPaths::const_iterator it = results.begin();
       it != results.end(); it++)
    {
      if (compare_string_vectors(it->first, expected_path.first)) 
	{
	  found = true;
	  weight = it->second;
	}
    }
  if (found == false)
    return false;
  if (not test_path_weight)
    return true;
  
  if (weight > (path_weight - 0.01) && 
      weight < (path_weight + 0.01))
    return true;
  return false;  
}

float modify_weights(float f)
{
  return f/2;
}

bool modify_transitions(const StringPair &sp, StringPairSet &sps)
{
  if (sp.first.compare(sp.second) == 0) {
    sps.insert(StringPair("<IDENTITY>", "<IDENTITY>"));
    return true;
  }
  return false;
}

int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=4;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OFST_TYPE, 
				       LOG_OFST_TYPE, 
				       FOMA_TYPE};

  for (unsigned int i=0; i<TYPES_SIZE; i++)
    {

      /* Function compare. */
      {
	verbose_print("function compare", types[i]);

	HfstTransducer t1("foo", "bar", types[i]);
	HfstTransducer t2("foo", "@_EPSILON_SYMBOL_@", types[i]);
	HfstTransducer t3("@_EPSILON_SYMBOL_@", "bar", types[i]);
	t2.concatenate(t3);
	t2.minimize();
	/* Alignments must be the same. */
	assert(not t1.compare(t2));

	HfstTransducer t4("foo", "bar", types[i]);
	HfstTransducer t5("@_EPSILON_SYMBOL_@", types[i]);
	t4.concatenate(t5);
	/* One transducer is minimal, another is not. */
	assert(t1.compare(t4));

	/* Weights. */
	if (types[i] == TROPICAL_OFST_TYPE ||
	    types[i] == LOG_OFST_TYPE)
	  {
	    HfstTransducer t6("foo", "bar", types[i]);
	    t6.set_final_weights(0.3);
	    HfstTransducer t7("foo", "bar", types[i]);
	    t7.set_final_weights(0.1);

	    /* Weights differ. */
	    assert(not t6.compare(t7));

	    HfstTransducer t8("@_EPSILON_SYMBOL_@", types[i]);
	    t8.set_final_weights(0.2);
	    t7.concatenate(t8);
	    /* Weights are the same on each path. */
	    assert(t6.compare(t7));
	  }
      }


      /* Function compose. */
      {
	verbose_print("function compose", types[i]);

	HfstTransducer t1("foo", "bar", types[i]);
	t1.set_final_weights(2);
	HfstTransducer t2("bar", "baz", types[i]);
	t2.set_final_weights(3);
	HfstTransducer t3("foo", "baz", types[i]);
	t3.set_final_weights(5);
	t1.compose(t2);
	assert(t1.compare(t3));
      }


      /* Function convert. */
      {
	verbose_print("function convert", types[i]);

	HfstTransducer t1("foo", "bar", types[i]);
	HfstTransducer t2("foo", "bar", types[i]);
	for (unsigned int j=0; j<=TYPES_SIZE; j++)
	  {
	    t1.convert(types[(i+j)%TYPES_SIZE]);
	  }
	assert(t1.compare(t2));
      }


      /* Functions extract_strings and extract_strings_fd. */
      {
	verbose_print("function extract_strings(_fd)", types[i]);

	using hfst::StringPair;
	using hfst::StringPairSet;
	using hfst::HfstTokenizer;
	using hfst::WeightedPaths;

	HfstTokenizer tok;
	HfstTransducer cat("cat", "cats", tok, types[i]);
	cat.set_final_weights(3);
	HfstTransducer dog("dog", "dogs", tok, types[i]);
	dog.set_final_weights(2.5);
	HfstTransducer mouse("mouse", "mice",  tok, types[i]);
	mouse.set_final_weights(1.7);
	HfstTransducer animals(types[i]);
	animals.disjunct(cat);
	animals.disjunct(dog);
	animals.disjunct(mouse);
	animals.minimize();

	StringPairSet expected_results;
	expected_results.insert(StringPair("cat","cats"));
	expected_results.insert(StringPair("dog","dogs"));
	expected_results.insert(StringPair("mouse","mice"));

	WeightedPaths<float>::Set results;
	animals.extract_strings(results, 
				3, /* max_num */ 
				0  /* cycles */ );

	/* Test that results are as expected. */
	assert(results.size() == 3);
	for (WeightedPaths<float>::Set::const_iterator it = results.begin();
	     it != results.end(); it++)
	  {
	    StringPair sp(it->istring, it->ostring);
	    assert(expected_results.find(sp) != expected_results.end());
	    /* Test weights. */
	    if (types[i] == TROPICAL_OFST_TYPE ||
		types[i] == LOG_OFST_TYPE)
	      {
		/* Rounding can affect precision. */
		if (it->istring.compare("cat") == 0)
		  assert(it->weight > 2.99 && it->weight < 3.01);
		else if (it->istring.compare("dog") == 0)
		  assert(it->weight > 2.49 && it->weight < 2.51);
		else if (it->istring.compare("mouse") == 0)
		  assert(it->weight > 1.69 && it->weight < 1.71);
		else
		  assert(false);
	      }
	  }
	
	/* More tests... */

	
	/* Functions is_lookup_infinitely_ambiguous, lookup and lookup_fd. */
	verbose_print("functions is_lookup_infinitely_ambiguous "
		      "and lookup(_fd)", types[i]);
	
	/* add an animal with two possible plural forms */
	// if type is LOG_OFST_TYPE:
	// FATAL: EncodeMapper: Weight-encoded arc has non-trivial weight
	if (types[i] != LOG_OFST_TYPE)
	  {
	    HfstTransducer hippopotamus1("hippopotamus", "hippopotami", 
					 tok, types[i]);
	    hippopotamus1.set_final_weights(1.2);
	    HfstTransducer hippopotamus2("hippopotamus", "hippopotamuses", 
					 tok, types[i]);
	    hippopotamus2.set_final_weights(1.4);
	    animals.disjunct(hippopotamus1);
	    animals.disjunct(hippopotamus2);
	    animals.minimize();
	  }

	/* convert to optimized lookup format */
	HfstTransducer animals_ol(animals);
	if (types[i] == TROPICAL_OFST_TYPE ||
	    types[i] == LOG_OFST_TYPE) {
	  animals_ol.convert(HFST_OLW_TYPE); }
	else {
	  animals_ol.convert(HFST_OL_TYPE); }

	/* no limit to the number of lookup results */
	ssize_t limit=-1;

	/* strings to lookup */
	HfstLookupPath lookup_cat = tok.lookup_tokenize("cat");
	HfstLookupPath lookup_dog = tok.lookup_tokenize("dog");
	HfstLookupPath lookup_mouse = tok.lookup_tokenize("mouse");
	HfstLookupPath lookup_hippopotamus 
	  = tok.lookup_tokenize("hippopotamus");

	/* where results of lookup are stored */
	HfstLookupPaths results_cat;
	HfstLookupPaths results_dog;
	HfstLookupPaths results_mouse;
	HfstLookupPaths results_hippopotamus;

	/* check that lookups are not infinitely ambiguous */
	assert(not animals_ol.is_lookup_infinitely_ambiguous(lookup_cat));
	assert(not animals_ol.is_lookup_infinitely_ambiguous(lookup_dog));
	assert(not animals_ol.is_lookup_infinitely_ambiguous(lookup_mouse));
	assert(not animals_ol.is_lookup_infinitely_ambiguous
	       (lookup_hippopotamus));

	// todo: more is_lookup_infinitely_ambiguous tests...

	/* perform lookups */
	animals_ol.lookup(results_cat, lookup_cat, limit);
	animals_ol.lookup(results_dog, lookup_dog, limit);
	animals_ol.lookup(results_mouse, lookup_mouse, limit);
	animals_ol.lookup(results_hippopotamus, lookup_hippopotamus, limit);

	/* check that the number of results is correct */
	assert(results_cat.size() == 1);
	assert(results_dog.size() == 1);
	assert(results_mouse.size() == 1);
	if (types[i] != LOG_OFST_TYPE)
	  assert(results_hippopotamus.size() == 2);

	bool test_weight=false;
	if (types[i] == TROPICAL_OFST_TYPE ||
	    types[i] == LOG_OFST_TYPE) {
	  test_weight=true; }

	tok.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	/* check that the results are correct */
	HfstLookupPath expected_path = tok.lookup_tokenize("cats");
	assert(do_hfst_lookup_paths_contain
	       (results_cat, expected_path, 3, test_weight));

	expected_path = tok.lookup_tokenize("dogs");
	assert(do_hfst_lookup_paths_contain
	       (results_dog, expected_path, 2.5, test_weight));

	expected_path = tok.lookup_tokenize("mice@_EPSILON_SYMBOL_@");
	assert(do_hfst_lookup_paths_contain
		(results_mouse, expected_path, 1.7, test_weight));

	expected_path = tok.lookup_tokenize("hippopotami@_EPSILON_SYMBOL_@");
	if (types[i] != LOG_OFST_TYPE)
	  assert(do_hfst_lookup_paths_contain
		 (results_hippopotamus, expected_path, 1.2, test_weight));
	
	expected_path = tok.lookup_tokenize("hippopotamuses");
	if (types[i] != LOG_OFST_TYPE)
	  assert(do_hfst_lookup_paths_contain
		 (results_hippopotamus, expected_path, 1.4, test_weight));


	// if type is LOG_OFST_TYPE:
	// FATAL: SingleShortestPath: Weight needs to have the path property
	// and be right distributive: log
	if (types[i] != LOG_OFST_TYPE)
	  {	    

	    /* Function n_best. */
	    verbose_print("function n_best", types[i]);
	    
	    HfstTransducer animals1(animals);
	    animals1.n_best(1);
	    WeightedPaths<float>::Set results1;
	    animals1.extract_strings(results1);
	    assert(results1.size() == 1);
	    
	    HfstTransducer animals2(animals);
	    animals2.n_best(2);
	    WeightedPaths<float>::Set results2;
	    animals2.extract_strings(results2);
	    assert(results2.size() == 2);
	    
	    HfstTransducer animals3(animals);
	    animals3.n_best(3);
	    WeightedPaths<float>::Set results3;
	    animals3.extract_strings(results3);
	    assert(results3.size() == 3);
	    
	    HfstTransducer animals4(animals);
	    animals4.n_best(4);
	    WeightedPaths<float>::Set results4;
	    animals4.extract_strings(results4);
	    assert(results4.size() == 4);
	    
	    HfstTransducer animals5(animals);
	    animals5.n_best(5);
	    WeightedPaths<float>::Set results5;
	    animals5.extract_strings(results5);
	    assert(results5.size() == 5);
	  }
	
      }


      /* Functions insert_freely. */
      {
	verbose_print("functions insert_freely", types[i]);

	HfstTransducer t1("a", "b", types[i]);
	t1.insert_freely(StringPair("c", "d"));

	HfstTransducer t2("a", "b", types[i]);
	HfstTransducer tr("c", "d", types[i]);
	t2.insert_freely(tr);

	assert(t1.compare(t2));
      }


      /* Function is_cyclic. */
      {
	verbose_print("function is_cyclic", types[i]);

	HfstTransducer t1("a", "b", types[i]);
	assert(not t1.is_cyclic());
	t1.repeat_star();
	assert(t1.is_cyclic());
      }


      /* Function push_weights. */
      {
	if (types[i] == TROPICAL_OFST_TYPE)
	  {
	    verbose_print("function push_weights", types[i]);

	    /* Create an HFST basic transducer [a:b] with transition 
	       weight 0.3 and final weight 0.5. */
	    HfstBasicTransducer t;
	    t.add_state(1);
	    t.add_transition(0, HfstBasicTransition(1, "a", "b", 0.3));
	    t.set_final_weight(1, 0.5);
	    
	    /* Convert to tropical OpenFst format and push weights 
	       toward final and initial states. */
	    HfstTransducer T_final(t, TROPICAL_OFST_TYPE);
	    T_final.push_weights(TO_FINAL_STATE);
	    HfstTransducer T_initial(t, TROPICAL_OFST_TYPE);
	    T_initial.push_weights(TO_INITIAL_STATE);
	    
	    /* Convert back to HFST basic transducer. */
	    HfstBasicTransducer t_final(T_final);
	    HfstBasicTransducer t_initial(T_initial);
	    
	    /* Test the final weight. */
	    try {
	      /* Rounding can affect the precision. */  
	      assert(0.79 < t_final.get_final_weight(1) &&
		     t_final.get_final_weight(1) < 0.81);
	    } 
	    /* If the state does not exist or is not final */
	    catch (hfst::exceptions::HfstArgumentException e) {
	      assert(false);
	    }

	    /* Test the transition weight. */
	    try {
	      HfstBasicTransducer::HfstTransitionSet transitions = t_initial[0];
	      assert(transitions.size() == 1);
	      float weight = transitions.begin()->get_weight();
	      /* Rounding can affect the precision. */  
	      assert(0.79 < weight &&
		     weight < 0.81);
	    }
	    /* If the state does not exist or is not final */
	    catch (hfst::exceptions::HfstArgumentException e) {
	      assert(false);
	    }
	  }
	
      }


      /* Functions set_final_weights and transform_weights. */
      {
	if (types[i] == TROPICAL_OFST_TYPE ||
	    types[i] == LOG_OFST_TYPE)
	  {
	    verbose_print("functions set_final_weights and "
			  "transform_weights", types[i]);
	    
	    /* Create an HFST basic transducer [a:b] with transition 
	       weight 0.3 and final weight 0.5. */
	    HfstBasicTransducer t;
	    t.add_state(1);
	    t.add_transition(0, HfstBasicTransition(1, "a", "b", 0.3));
	    t.set_final_weight(1, 0.5);

	    /* Modify weights. */
	    HfstTransducer T(t, types[i]);
	    T.set_final_weights(0.2);
	    T.transform_weights(&modify_weights);
	    T.push_weights(TO_FINAL_STATE);

	    /* Convert back to HFST basic transducer and test the weight. */
	    HfstBasicTransducer tc(T);	    
	    try {	    
	      assert(0.24 < tc.get_final_weight(1) &&
		     tc.get_final_weight(1) < 0.26);
	    }
	    /* If the state does not exist or is not final */
	    catch (hfst::exceptions::HfstArgumentException e) {
	      assert(false);
	    }

	  }
      }


      /* Functions substitute. */
      {
	if (types[i] != TROPICAL_OFST_TYPE &&
	    types[i] != LOG_OFST_TYPE) {
	verbose_print("functions substitute", types[i]);

	HfstTokenizer tok;
	tok.add_multichar_symbol("<eps>");
	HfstTransducer t("cat", "cats", tok, types[i]);

	/* String with String */
	HfstTransducer t1(t);
	t1.substitute("c", "C", true, false);
	t1.substitute("t", "T", false, true);
	t1.substitute("@_EPSILON_SYMBOL_@", "<eps>");
	t1.substitute("a", "A");
	HfstTransducer t1_("CAt<eps>", "cATs", tok, types[i]);
	assert(t1.compare(t1_));

	/* StringPair with StringPair */
	HfstTransducer t2(t);
	t2.substitute(StringPair("c","c"), StringPair("C","c"));
	t2.substitute(StringPair("C","c"), StringPair("h","H"));
	HfstTransducer t2_("hat", "Hats", tok, types[i]);

	/* StringPair with StringPairSet */
	HfstTransducer t3(t);
	StringPairSet sps;
	sps.insert(StringPair("c","c"));
	sps.insert(StringPair("C","C"));
	sps.insert(StringPair("h","h"));
	sps.insert(StringPair("H","H"));
	t3.substitute(StringPair("c","c"), sps); // TROPICAL_OFST_TYPE: SEGFAULT
	HfstTransducer t3_("cat", "cats", tok, types[i]);
	HfstTransducer t3_1("Cat", "Cats", tok, types[i]);
	HfstTransducer t3_2("hat", "hats", tok, types[i]);
	HfstTransducer t3_3("Hat", "Hats", tok, types[i]);
	t3_.disjunct(t3_1);
	t3_.disjunct(t3_2);
	t3_.disjunct(t3_3);
	t3_.minimize();

	assert(t3.compare(t3_));

	/* StringPair with HfstTransducer */

	/* Substitute with function */
	}
      }


    }
}

