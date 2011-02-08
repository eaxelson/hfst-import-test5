/*
   Test file for HfstTransducer member functions.
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


      /* Function concatenate. */

      /* Function convert. */
      {
	verbose_print("function convert", types[i]);

	HfstTransducer t1("foo", "bar", types[i]);
	for (unsigned int j=0; j<; j++)
	  {
	    t1.convert(types[(i+j)%TYPES_SIZE]);
	  }
      }


      /* determinize. */
      /* disjunct. */
      /* extract_strings. */
      /* extract_string_fd. */
      /* input_project. */
      /* insert_freely. */
      /* insert_freely. */
      /* intersect. */
      /* invert. */
      /* is_cyclic. */
      /* is_lookup_infinitely_ambiguous. */
      /* lookup. */
      /* lookup_fd. */
      /* minimize. */
      /* n_best. */
      /* optionalize. */
      /* output_project. */
      /* push_weights. */
      /* remove_epsilons. */
      /* repeat_n. */
      /* repeat_n_minus. */
      /* repeat_n_plus. */
      /* repeat_n_to_k. */
      /* repeat_plus. */
      /* repeat_star. */
      /* reverse. */
      /* set_final_weights. */
      /* substitute. */
      /* substitute. */
      /* substitute. */
      /* substitute. */
      /* substitute. */
      /* subtract. */
      /* transform_weights. */
	

    }
}

