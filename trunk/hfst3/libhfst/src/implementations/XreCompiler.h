//! @file XreCompiler.h
//!
//! @brief Functions for building transducers from Xerox regexps

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
#include "../HfstTransducer.h"

namespace hfst { namespace xre {
//! @brief A compiler holding information contained in lexc style lexicons.
class XreCompiler
{
  public:
  //! @brief create compiler 
  XreCompiler();
  XreCompiler(hfst::ImplementationType impl);

  //! @brief add @a alphabet to sigma set.
  void define(const std::string& name, const std::string& xre);

  //! @brief compile an expression
  HfstTransducer* compile(const std::string& xre);

  private:
  std::map<std::string,hfst::HfstTransducer> definitions_;
  hfst::ImplementationType format_;

}
;
}}
// vim:set ft=cpp.doxygen:
#endif


