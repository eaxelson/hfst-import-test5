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

*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;

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
	
      }


#ifdef TESTS_IMPLEMENTED
      /* Function insert_freely. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Function is_cyclic. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Functions is_lookup_infinitely_ambiguous, lookup and lookup_fd. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Function n_best. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Function push_weights. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Function set_final_weights. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Functions substitute. */
      {
	verbose_print("function ...", types[i]);
      }


      /* Function transform_weights. */
      {
	verbose_print("function ...", types[i]);
      }
#endif // TESTS_IMPLEMENTED


    }
}

