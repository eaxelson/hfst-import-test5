/*
   Test file for HfstBasicTransducer.
   Some of the functions are already tested in other test files,
   because some HfstTransducer functions are implemented only 
   for HfstBasicTransducer.
*/

#include "HfstTransducer.h"
#include "auxiliary_functions.cc"

using namespace hfst;
;

using implementations::HfstState;
using implementations::HfstBasicTransition;
using implementations::HfstBasicTransducer;


int main(int argc, char **argv) 
{

  verbose_print("HfstBasicTransducer construction");

  /* Create an HfstBasicTransducer [a:b c:d] with weight 3.0. */
  HfstBasicTransducer t;
  assert(not t.is_final_state(0));
  HfstState s1 = t.add_state();
  assert(s1 == 1);
  t.add_transition(0, HfstBasicTransition(s1, "a", "b", 1.2));
  assert(not t.is_final_state(s1));
  HfstState s2 = t.add_state();
  assert(s2 = 2);
  t.add_transition(s1, HfstBasicTransition(s2, "c", "d", 0.8));
  assert(not t.is_final_state(s2));
  t.set_final_weight(s2, 1.0);
  assert(t.is_final_state(s2) &&
	 t.get_final_weight(s2) == 1.0);

  /* Take a copy. */
  HfstBasicTransducer tc(t);


  verbose_print("HfstBasicTransducer exceptions");

  /* Asking the weight of a non-final state. */
  for (HfstState s=0; s<5; s++) {
    if (s != s2) {
      try {
	float w = t.get_final_weight(0);
	assert(false);
      }
      //catch (StateIsNotFinalException e) {};
      catch (const HfstException e) {};
    }
  }
  /* Reading a file in non-valid AT&T format. */
  FILE * ofile = fopen("test.att", "wb");
  fprintf(ofile, "0\n0\t1\ta\tb\n1\t2\tb\n2\n");
  fclose(ofile);
  FILE * ifile = fopen("test.att", "rb");
  try {
    HfstBasicTransducer foo 
      = HfstBasicTransducer::read_in_att_format(ifile);
    fclose(ifile);
    remove("test.att");
    assert(false);
  }
  //catch (NotValidAttFormatException e) {
  catch (const HfstException e) {
    fclose(ifile);
    remove("test.att");
  }

  
  verbose_print("HfstBasicTransducer: symbol handling");

  /* */
  t.add_symbol_to_alphabet("foo");
  t.prune_alphabet();
  const std::set<std::string> alphabet = t.get_alphabet();
  assert(alphabet.size() == 7);
  assert(alphabet.find("a") != alphabet.end());
  assert(alphabet.find("b") != alphabet.end());
  assert(alphabet.find("c") != alphabet.end());
  assert(alphabet.find("d") != alphabet.end());
  assert(alphabet.find("foo") == alphabet.end());

  
  verbose_print("HfstBasicTransducer: substitute");

  HfstBasicTransducer tr;
  tr.add_state();
  tr.add_transition(0, HfstBasicTransition(s1, "a", "b", 0));
  tr.add_transition(0, HfstBasicTransition(s1, "a", "b", 0));
  tr.set_final_weight(1, 0);

  StringPairSet sps;
  sps.insert(StringPair("A","B"));
  sps.insert(StringPair("C","D"));
  tr.substitute(StringPair("a","b"), sps);

  verbose_print("HfstBasicTransducer: EmptyStringException");
  
  try {
    HfstBasicTransducer empty_symbol;
    empty_symbol.add_transition(0, HfstBasicTransition(0, "", "", 0));
    assert(false);
  }
  catch (EmptyStringException e) {
    ;
  }
}

