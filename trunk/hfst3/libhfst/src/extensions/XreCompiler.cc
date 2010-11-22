//! @file XreCompiler.cc
//!
//! @brief Functions for building transducers from Xerox regexps

#include "XreCompiler.h"
#include "xre_utils.h"

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
