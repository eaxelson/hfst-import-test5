/*
   Test file for HfstTransducer constructors, destructor, operator=
   and member functions set_name, get_name and get_type.
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;
using hfst::implementations::HfstBasicTransducer;
using hfst::implementations::HfstBasicTransition;


int main(int argc, char **argv) 
{

  const unsigned int TYPES_SIZE=3;
  const ImplementationType types [] = {SFST_TYPE, 
				       TROPICAL_OPENFST_TYPE, 
				       /*LOG_OPENFST_TYPE,*/ 
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
      /*FILE * f = fopen("foobar.att", "wb");
      fprintf(f, 
	      "0 1 @0@ @0@ 0.0\n"
	      "1 2 foo bar 0\n"
	      "2 3 @0@ @0@\n"
	      "3\n");
	      fclose(f);*/
      char * srcdir = getenv("srcdir");
      std::string srcdir_(".");
      if (srcdir != NULL)
	srcdir_ = std::string(srcdir);
      const char * file_to_open = (srcdir_ + 
				   std::string("/foobar.att")).c_str();
      FILE * file = fopen(file_to_open, "rb");
      HfstTransducer foobar_att(file, types[i], "@0@");
      fclose(file);
      foobar_att.minimize();
      assert(foobar.compare(foobar_att));

      /* From HfstInputStream. 
	 Tests also functions get_type, set_name and get_name. */
      verbose_print("Construction from HfstInputStream", types[i]);
      HfstOutputStream out("testfile.hfst", foobar.get_type());
      foobar.set_name("foobar");
      out << foobar;
      out.close();
      HfstInputStream in("testfile.hfst");
      HfstTransducer foobar_stream(in);
      in.close();
      assert(foobar.compare(foobar_stream));
      assert(foobar_stream.get_name().compare("foobar") == 0);
      assert(foobar_stream.get_type() == types[i]);

      /* Destructor. */
      verbose_print("Destructor", types[i]);
      HfstTransducer * nu = new HfstTransducer("new", types[i]);
      delete nu;

      /* Operator=. */
      verbose_print("Operator=", types[i]);
      HfstTransducer foobar2("baz", types[i]);
      foobar2 = foobar;
      assert(foobar.compare(foobar2));

    }

}
