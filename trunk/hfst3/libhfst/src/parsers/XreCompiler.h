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

  //! @brief Add a function macro.
  //!        Compilers will replace arcs labeled function_name(arg1, arg2, ... , argN)
  //!        with the transducer defined by @a xre in later phases of compilation.
  //! @param name        The name of the function. It must end with a left bracket '('.
  //! @param arguments   The names of the function arguments in order.
  //! @param xre         The regexp defining the function.
  //!
  //! For example, a call
  //!
  //!   define_function("Foo("), args, "[ [foo|bar] baz+ ]");
  //!
  //! where args is ("foo", "bar", "baz"), defines a function named Foo( that
  //! accepts the disjunction of its first and second arguments concatenated with
  //! one or more its third argument.
  bool define_function(const std::string& name, 
                       const std::vector<std::string>& arguments, 
                       const std::string& xre);

  //! @brief Add a definition macro.
  //!        Compilers will replace arcs labeled @a name, with the transducer
  //!        \a transducer in later phases of compilation.
  void define(const std::string& name, const HfstTransducer & transducer);

  //! @brief Remove a definition macro.
  void undefine(const std::string& name);


  //! @brief Compile a transducer defined by @a xre.
  //!        May return a pointer to @e empty transducer on non-fatal error.
  //!        A null pointer is returned on fatal error, if abort is not called.
  HfstTransducer* compile(const std::string& xre);

  std::set<unsigned int> get_positions_of_symbol_in_xre
    (const std::string & symbol, const std::string & xre);

  //! @brief Whether transducer names defined with function 'define' are expanded.
  //!        Default is false.
  void set_expand_definitions(bool expand);

  //! @brief Whether binary operators harmonize their argument transducers.
  //!        Default is true,
  void set_harmonization(bool harmonize);

  //! @brief Whether warning messages are printed to \a file.
  //!        Default is false, If verbose==false, \a file is ignored.
  void set_verbosity(bool verbose, FILE * file);

  private:
  std::map<std::string,hfst::HfstTransducer*> definitions_;
  std::map<std::string, std::string> function_definitions_;
  std::map<std::string, std::vector<std::string> > function_arguments_;
  hfst::ImplementationType format_;

}
;
}}
// vim:set ft=cpp.doxygen:
#endif


