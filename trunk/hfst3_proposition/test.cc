#include "HfstTransducer.h"
#include <cstdio>

using namespace hfst;

int main(int argc, char **argv) {
  HfstMutableTransducer t;
  HfstState second_state = t.add_state();
  t.set_final_weight(second_state, 0.5);
  t.add_transition(0, "foo", "bar", 0.3, second_state);
  HfstState initial = t.get_initial_state();
  cout << "initial state: " << initial << "\n";
  HfstStateIterator it(t);
  while (not it.done()) {
    HfstState s = it.value();
    HfstTransitionIterator IT(t,s);
    while (not IT.done()) {
      fprintf(stderr, "loop starts\n");
      HfstTransition tr = IT.value();
      fprintf(stderr, "transition\n");
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
  HfstTransducer T(t);
  T.print_type();

  fprintf(stderr, "main: (1)\n");
  T = T.convert(FOMA_TYPE);   // BUG: type is lost in the conversion
  T.print_type();
  //HfstOutputStream os(LOG_OFST_TYPE);
  /*os << TR;
    fprintf(stderr, "\n\n");*/
  //cout << TR;
  //fprintf(stderr, "\n");
  fprintf(stderr, "main: (2)\n");
  return 0;
}
