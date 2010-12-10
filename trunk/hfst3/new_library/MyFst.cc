#include "MyFst.h"
#include <stdio.h>

namespace mfstl {

  MyFst::MyFst() :
    istring(""), ostring("") {
  }

  MyFst::MyFst(std::string str) :
    istring(str), ostring(str) {
  }

  MyFst::MyFst(std::string istr, std::string ostr) :
    istring(istr), ostring(ostr) {
  }

  MyFst::~MyFst() {
  }
  
  void MyFst::invert() {
    istring = ostring;
    ostring = istring;
  }

  MyFst * MyFst::copy() {
    return new MyFst(this->istring, this->ostring);
  }
  
}

using namespace mfstl;

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  MyFst * test = new MyFst("foo", "bar");
  delete test;

  fprintf(stderr, "test: OK\n");

  return 0;
}

