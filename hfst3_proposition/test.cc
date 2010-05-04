/*
 * test.cc: A test file for hfst3
 */

#include "HfstTransducer.h"
#include <cstdio>

using namespace hfst;

void print(HfstMutableTransducer &t)
{
  HfstStateIterator it(t);
  while (not it.done()) {
    HfstState s = it.value();
    HfstTransitionIterator IT(t,s);
    while (not IT.done()) {
      //fprintf(stderr, "loop starts\n");
      HfstTransition tr = IT.value();
      //fprintf(stderr, "transition\n");
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

int main(int argc, char **argv) {

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
      out2 << T2;
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

	//tr1.print();
	//fprintf(stderr, "--\n");
	//tr2.print();
	//fprintf(stderr, "\n");

	HfstTransducer t = tr1.compose(tr2);
        fprintf(stderr, "  composed\n");
	t = (HfstTransducer(tr1)).intersect(tr2);
	fprintf(stderr, "  intersected\n");
	t = tr1.disjunct(tr2);
	fprintf(stderr, "  disjuncted\n");
	t = tr1.concatenate(tr2);
	fprintf(stderr, "  concatenated\n");
	t = (HfstTransducer(tr1)).subtract(tr2);
	fprintf(stderr, "  subtracted\n");

      }
      remove("test1.hfst");
      remove("test2.hfst");

    }

  return 0;
}

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
    HfstTransducer &substitute(const StringSymbolPair &old_symbol_pair,
			       const StringSymbolPair &new_symbol_pair);
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

