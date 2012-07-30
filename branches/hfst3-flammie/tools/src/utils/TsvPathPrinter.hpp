//! @file TsvPathPrinter.h
//! @brief Definitions for printing path automata in TSV format
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

#ifndef _HFST_TSV_PATH_PRINTER_H_
#define _HFST_TSV_PATH_PRINTER_H_

#include "PathPrinter.hpp"

/**
 * Class for the most straight-forward, yet slightly configurable, printing of
 * paths in TSV-like formats.
 */
class TsvPathPrinter: public PathPrinter
{

  public:
      /**
       * @brief construct TSV printer for path with settings.
       */
      TsvPathPrinter(std::ostream*, const std::string sep = "\t", 
                     const std::string quot = "", 
                     const std::string ender = "\n");
      /**
       * @brief Destruct the printer.
       */
      ~TsvPathPrinter();

      /**
       * @brief Print TSV headers, if any. By default captions for first two
       *        fields.
       */
      void print_begin_file() const;

      /**
        * @brief Prints record start if any, and input as-is to first field.
        */
      void print_begin_cohort(const hfst::HfstOneLevelPath& input) const;

      /**
       * @brief Prints each result, introduced by field separator.
       */
      void print_lookup_set(const hfst::HfstTwoLevelPaths& lookups) const;

      /**
       * @brief Prints record end, by default a newline.
       */
      void print_end_cohort() const;

      /**
       * @brief Prints TSV footers, if any.
       */
      void print_end_file() const;

      /**
       * @brief Print result side, introduced by field separator.
       */
      void print_path_now(const hfst::HfstTwoLevelPath& path) const;

      /**
       * @brief Sets field separator. Suggested values are of course
       *    tab, or comma.
       */
      void set_field_separator(const std::string& sep);

      /**
       * @brief Sets quotation marker. Suggested values are empty string,
       *    single or double quotation marks.
       */
      void set_quotation_mark(const std::string& sep);

  private:
      std::string separator_;
      std::string quotation_;
      std::string record_end_;
};



#endif
// vim: set ft=cpp.doxygen:

