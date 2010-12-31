//! @file XreCompiler.cc
//!
//! @brief Functions for building transducers from Xerox regexps

#include "XreCompiler.h"
#include "xre_utils.h"

#ifndef DEBUG_MAIN

namespace hfst { namespace xre {
XreCompiler::XreCompiler() : 
    definitions_(),
    format_(hfst::TROPICAL_OFST_TYPE)
{}

XreCompiler::XreCompiler(hfst::ImplementationType impl) :
    definitions_(),
    format_(impl)
{}

void
XreCompiler::define(const std::string& name, const std::string& xre)
{
  HfstTransducer* compiled = compile(xre);
  definitions_[name] = compiled;
}

HfstTransducer*
XreCompiler::compile(const std::string& xre)
{
  return hfst::xre::compile(xre, definitions_, format_);
}

}}

#else
#include <cstdlib>
#include <cassert>

using namespace hfst;
using namespace hfst::xre;

int
main(int argc, char** argv)
  {
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "constructors: ";
    std::cout << " (default)...";
    XreCompiler defaultXre();
#if HAVE_SFST
    std::cout << " (SFST)...";
    XreCompiler sfstXre = XreCompiler(SFST_TYPE);
#endif
#if HAVE_OPENFST
    std::cout << " (OpenFst...";
    XreCompiler ofstXre = XreCompiler(TROPICAL_OFST_TYPE);
#endif
#if HAVE_FOMA
    std::cout << " (Foma)...";
    XreCompiler fomaXre = XreCompiler(FOMA_TYPE);
#endif
    std::cout << std::endl << "compilation: ";
#if HAVE_SFST
    std::cout << "sfst compile(c a t)...";
    assert(sfstXre.compile("c a t") != 0);
    std::cout << "(f i:o 0:u g h t)...";
    assert(sfstXre.compile("f i:o 0:u g h t") != 0);
    std::cout << "(c a t | dog)...";
    assert(sfstXre.compile("c a t | dog") != 0);
#endif
#if HAVE_OPENFST
    std::cout << "ofst compile(c a t)...";
    assert(ofstXre.compile("c a t") != 0);
    std::cout << "(f i:o 0:u g h t)...";
    assert(ofstXre.compile("f i:o 0:u g h t") != 0);
    std::cout << "(c a t | dog)...";
    assert(ofstXre.compile("c a t | dog") != 0);
#endif
#if HAVE_SFST
    std::cout << "foma compile(c a t)...";
    assert(fomaXre.compile("c a t") != 0);
    std::cout << "(f i:o 0:u g h t)...";
    assert(fomaXre.compile("f i:o 0:u g h t") != 0);
    std::cout << "(c a t | dog)...";
    assert(fomaXre.compile("c a t | dog") != 0);
#endif
    std::cout << std::endl << "define:";
#if HAVE_SFST
    std::cout << "sfst define(vowels, a | e | i | o | u | y)...";
    sfstXre.define("vowels", "a | e | i | o | u | y");
#endif
#if HAVE_OPENFST
    std::cout << "sfst define(vowels, a | e | i | o | u | y)...";
    ofstXre.define("vowels", "a | e | i | o | u | y");
#endif
#if HAVE_FOMA
    std::cout << "sfst define(vowels, a | e | i | o | u | y)...";
    fomaXre.define("vowels", "a | e | i | o | u | y");
#endif
    return EXIT_SUCCESS;
  }
#endif
