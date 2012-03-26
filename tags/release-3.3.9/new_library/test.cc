#include "MyFst.h"
#include <stdio.h>

using namespace mfstl;

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  MyFst * test = new MyFst("foo", "bar");
  delete test;

  fprintf(stderr, "test: OK\n");

  return 0;
}
