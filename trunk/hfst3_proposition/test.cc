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
      HfstTransition tr = IT.value();
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

  HfstTransducer T_FOMA = T.convert(FOMA_TYPE);
  //HfstOutputStream os(LOG_OFST_TYPE);
  /*os << TR;
    fprintf(stderr, "\n\n");*/
  //cout << TR;
  //fprintf(stderr, "\n");
  return 0;
}
