#include "HfstTransducer.h"
#include <cstdio>

using namespace hfst;

int main(int argc, char **argv) {
  HfstMutableTransducer t;
  HfstState second_state = t.add_state();
  t.set_final_weight(second_state, 0.5);
  t.add_transition(0, "foo", "bar", 0.3, second_state);
  HfstStateIterator it(t);
  while (not it.done()) {
    HfstState s = it.value();
    cout << s << "\n";
    it.next();
  }
  HfstTransducer T(t);

  HfstTransducer TR = T.convert(LOG_OFST_TYPE);
  HfstOutputStream os(LOG_OFST_TYPE);
  os << TR;
  fprintf(stderr, "\n\n");
  cout << TR;
  fprintf(stderr, "\n");
  return 0;
}
