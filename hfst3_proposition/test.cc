#include "HfstTransducer.h"
#include <cstdio>

using namespace hfst;

int main(int argc, char **argv) {
  HfstMutableTransducer t;
  HfstState initial_state = t.add_state();
  HfstState end_state = t.add_state();
  t.set_final_weight(end_state, 0.5);
  t.add_transition(initial_state, "foo", "bar", 0.3, end_state);
  HfstTransducer T(t);
  HfstTransducer TR = T.convert(LOG_OFST_TYPE);
  HfstOutputStream os(LOG_OFST_TYPE);
  os << TR;
  return 0;
}
