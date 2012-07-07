//! @file PathPrinter.h
//! @brief Definition of abstract interface for classes that implement path
//!        formatting utilities to print paths from automaton.
//! @author James Croom
//! @author Tommi A Pirinen <tommi.pirinen@iki.fi>

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

#ifndef _HFST_PATH_PRINTER_H_
#define _HFST_PATH_PRINTER_H_

#include <hfst.hpp>


/**
 * Abstract base class for handling the outputting of lookup results. Subclasses
 * implement different formatting systems. This class is for immediate printing
 * of results to given output stream. There are three possible paths that
 * path handlers may call printers:
 *
 * @li long, slow lookup: Construct, begin file, 
 *      (begin cohort, print set, end cohort)*, end file
 * @li random path printing: Construct, print path now*
 * @li mixed: Construct, begin file?, (begin cohort, print now* | print set, 
 *      end cohort)*, end file?
 */
class PathPrinter
{

  public:
      /**
       * @brief Create new printer that outputs to @a outstream
       */
      PathPrinter(std::ostream* outstream = &std::cout);
      /**
       * @brief Destruct the printer.
       */
      ~PathPrinter() {}

      /**
       * @brief function for path handler to tell the formatter to start new
       *    file. This is to be called directly after construction and may
       *    simply do nothing if the format does not include a @e preamble.
       */
      void print_begin_file(const std::string filename) const;

      /**
        * @brief function for path handler to tell the formatter to start new
        *    set of output. That is a new word to analyse or such. This may
        *    print nothing if format requires no special things for beginning
        *    of cohort, or it may save the input word if it is printed for
        *    each output.
        */
      void print_begin_cohort(const hfst::HfstOneLevelPath& input) const;

      /**
       * @brief function to print all results in one set. This is one set of
       *    results in one analysis run.
       */
      void print_lookup_set(const hfst::HfstTwoLevelPaths& lookups)const;

      /**
       * @brief function for path handler to tell the formatter to end a set
       *    of output. This is when all possible analyses have been traversed
       *    or so. May print nothing if format does not require an end of set
       *    to be indicated.
       */
      void print_end_cohort() const;

      /**
       * @brief finction for path handler to tell formatter to end whole file
       *    of output. This should be called before destruction or switching
       *    output files.
       */
      void print_end_file() const;

      /**
       * @brief function for path handler to request immediate printing of path.
       *    This may happen while traversing net without knowledge if there will
       *    be further paths in this set. This is useful for end tools that do
       *    not handle paths as sets of lookups.
       */
      void print_path_now(const hfst::HfstTwoLevelPath& path) const;

  protected:
      std::ostream* out_;
};



#endif
// vim: set ft=cpp.doxygen:
