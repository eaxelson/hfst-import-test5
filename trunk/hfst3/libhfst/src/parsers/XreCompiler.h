//! @file XreCompiler.h
//!
//! @brief A class that encapsulates compilation of Xerox compatible regular
//!        expressions into HFST automata.
//!
//!        Xerox compatible regular expressions are a dialect of regular
//!        expressions commonly used for two-level finite state morphologies.
//!        The details can be found in Finite state morphology (2004) by
//!        Beesley and Karttunen.
//!
//!        This class is merely a wrapper around lex and yacc functions handling
//!        the parsing.

//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GUARD_XreCompiler_h
#define GUARD_XreCompiler_h

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
#include "../HfstDataTypes.h"

namespace hfst { 
//! @brief hfst::xre namespace is used for all functions related to Xerox 
//! Regular Expresisions (XRE) parsing.
namespace xre {
//! @brief A compiler holding information needed to compile XREs.
class XreCompiler
{
  public:
  //! @brief Construct compiler for unknown format transducers.
  XreCompiler();
  //! @brief Create compiler for @a impl format transducers
  XreCompiler(hfst::ImplementationType impl);

  //! @brief Add a definition macro.
  //!        Compilers will replace arcs labeled @a name, with the transducer
  //!        defined by @a xre in later phases of compilation.
  void define(const std::string& name, const std::string& xre);

  //! @brief Compile a transducer defined by @a xre.
  //!        May return a pointer to @e empty transducer on non-fatal error.
  //!        A null pointer is returned on fatal error, if abort is not called.
  HfstTransducer* compile(const std::string& xre);

  void set_expand_definitions(bool expand);

  private:
  std::map<std::string,hfst::HfstTransducer*> definitions_;
  hfst::ImplementationType format_;

}
;
}}
// vim:set ft=cpp.doxygen:
#endif


