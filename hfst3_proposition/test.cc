#include "HfstTransducer.h"
#include <cstdio>

using namespace hfst;

int main(int argc, char **argv) {
  HfstMutableTransducer t;
  HfstState second_state = t.add_state();
  t.set_final_weight(second_state, 0.5);
  t.add_transition(0, "foo", "bar", 0.3, second_state);
  HfstTransducer T(t);
  HfstTransducer TR = T.convert(LOG_OFST_TYPE);
  HfstOutputStream os(LOG_OFST_TYPE);
  os << TR;
  cout << TR;
  fprintf(stderr, "\n\n");
  HfstTransducer fb1 = define_transducer("foo", "bar");
  return 0;
}
