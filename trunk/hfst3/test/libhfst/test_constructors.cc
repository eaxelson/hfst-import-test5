/*
   Test file for HfstTransducer constructors. 
*/

#include "HfstTransducer.h"
using namespace hfst;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;

int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=4;
  ImplementationType types [] = {SFST_TYPE, 
				 TROPICAL_OFST_TYPE, 
				 LOG_OFST_TYPE, 
				 FOMA_TYPE};

  for (unsigned int i=0; i<TYPES_SIZE; i++)
    {

      /* The empty transducer. */
      HfstTransducer empty(types[i]);

      /* The epsilon transducer. */
      HfstTransducer epsilon("@_EPSILON_SYMBOL_@", types[i]);

      /* One-transition transducer. */
      HfstTransducer foo("foo", types[i]);
      HfstTransducer foobar("foo", "bar", types[i]);

      /* The copy constructor. */
      HfstTransducer foobar_copy(foobar);

      assert(foobar.compare(foobar_copy));

      /* Conversion from HfstBasicTransducer. */
      HfstBasicTransducer basic;
      basic.add_state(1);
      basic.add_transition(0, HfstBasicTransition(1, "foo", "bar", 0));
      basic.set_final_weight(1, 0);
      HfstTransducer foobar_basic(basic, types[i]);

      assert(foobar.compare(foobar_basic));

      /* By tokenizing. */
      HfstTokenizer tok;
      tok.add_skip_symbol("baz");
      tok.add_multichar_symbol("foo");
      tok.add_multichar_symbol("bar");
      HfstTransducer foo_tok("bazfoobaz", tok, types[i]);
      HfstTransducer foobar_tok("bazfoo", "barbaz", tok, types[i]);

      assert(foo.compare(foo_tok));
      assert(foobar.compare(foobar_tok));

      /* From AT&T format. */

      /* From HfstInputStream. */

    }

}
