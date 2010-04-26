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

  ImplementationType types[] = {FOMA_TYPE, SFST_TYPE, TROPICAL_OFST_TYPE, LOG_OFST_TYPE};
  for (int i=0; i<4; i++) 
    {      
      // open an output stream to a file
      HfstOutputStream out("tr.hfst", types[i]);
      out.open();

      // convert both transducers and write them to the stream
      HfstTransducer T1(t1);
      T1 = T1.convert(types[i]);
      HfstTransducer T2(t2);
      T2 = T2.convert(types[i]);
      out << T1;
      out << T2;
      out.close();
      fprintf(stderr, "test.cc: wrote two transducers of type %i\n", types[i]);
      
      // open an input stream to the file
      HfstInputStream in("tr.hfst");
      in.open();
      while (not in.is_eof()) {
	HfstTransducer t(in);
	fprintf(stderr, "test.cc: read a transducer of type %i\n", types[i]);
      }
      remove("tr.hfst");
    }

  return 0;


#ifdef foo

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
}
