/*
   Test file for HfstTransducer constructors. 
*/

#include "HfstTransducer.h"
using namespace hfst;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;

const bool VERBOSE=true;

void verbose_print(const char *msg, ImplementationType type) {
  if (VERBOSE) {
    fprintf(stderr, "Testing:\t%s", msg);
    fprintf(stderr, " for type ");
    switch (type) 
      {
      case SFST_TYPE:
	fprintf(stderr, "SFST_TYPE");
	break;
      case TROPICAL_OFST_TYPE:
	fprintf(stderr, "TROPICAL_OFST_TYPE");
	break;
      case LOG_OFST_TYPE:
	fprintf(stderr, "LOG_OFST_TYPE");
	break;
      case FOMA_TYPE:
	fprintf(stderr, "FOMA_TYPE");
	break;
      default:
	fprintf(stderr, "(type not known)");
	break;
      }
    fprintf(stderr, "...\n");
  }
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

      /* The empty transducer. */
      verbose_print("The empty transducer", types[i]);
      HfstTransducer empty(types[i]);

      /* The epsilon transducer. */
      verbose_print("The epsilon transducer", types[i]);
      HfstTransducer epsilon("@_EPSILON_SYMBOL_@", types[i]);

      /* One-transition transducer. */
      verbose_print("One-transition transducer", types[i]);
      HfstTransducer foo("foo", types[i]);
      HfstTransducer foobar("foo", "bar", types[i]);

      /* The copy constructor. */
      verbose_print("The copy constructor", types[i]);
      HfstTransducer foobar_copy(foobar);
      assert(foobar.compare(foobar_copy));

      /* Conversion from HfstBasicTransducer. */
      verbose_print("Conversion from HfstBasicTransducer", types[i]);
      HfstBasicTransducer basic;
      basic.add_state(1);
      basic.add_transition(0, HfstBasicTransition(1, "foo", "bar", 0));
      basic.set_final_weight(1, 0);
      HfstTransducer foobar_basic(basic, types[i]);
      assert(foobar.compare(foobar_basic));

      /* By tokenizing. */
      verbose_print("Construction by tokenization", types[i]);
      HfstTokenizer tok;
      tok.add_skip_symbol("baz");
      tok.add_multichar_symbol("foo");
      tok.add_multichar_symbol("bar");
      HfstTransducer foo_tok("bazfoobaz", tok, types[i]);
      HfstTransducer foobar_tok("bazfoo", "barbaz", tok, types[i]);
      assert(foo.compare(foo_tok));
      assert(foobar.compare(foobar_tok));

      /* From AT&T format. */
      verbose_print("Construction from AT&T format", types[i]);
      FILE * file = fopen("foobar.att", "rb");
      HfstTransducer foobar_att(file, types[i], "@0@");
      fclose(file);
      foobar_att.minimize();
      assert(foobar.compare(foobar_att));

      /* From HfstInputStream. */
      verbose_print("Construction from HfstInputStream", types[i]);
      HfstOutputStream out("testfile.hfst", types[i]);
      out << foobar;
      out.close();
      HfstInputStream in("testfile.hfst");
      HfstTransducer foobar_stream(in);
      in.close();
      assert(foobar.compare(foobar_stream));

    }

}
