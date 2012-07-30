//! @file TsvFormatter.cc
//! @brief Basic implementations if not redefined.
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

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <hfst.hpp>

#include "TsvPathPrinter.hpp"

using std::ostream;
using hfst::HfstOneLevelPath;
using hfst::HfstTwoLevelPath;
using hfst::HfstTwoLevelPaths;
using hfst::StringVector;
using hfst::StringPairVector;
using hfst::StringPair;

TsvPathPrinter::TsvPathPrinter(ostream* out, string sep, string quot, 
                               string ender) : 
    PathPrinter(out),
    separator_(sep),
    quotation_(quot),
    record_end_(ender)
  {}

TsvPathPrinter::~TsvPathPrinter() {}

void
TsvPathPrinter::print_begin_file() const
  {
    *out_ << quotation_ << "Input" << quotation_ << separator_
        << quotation_ << "Outputs:" << record_end_;
  }


void
TsvPathPrinter::print_begin_cohort(const HfstOneLevelPath& input) const
  {
    *out_ << quotation_;
    for (StringVector::const_iterator s = input.second.begin();
         s != input.second.end();
         ++s)
      {
        *out_ << *s;
      }
    *out_ << quotation_;
  }

void 
TsvPathPrinter::print_lookup_set(const HfstTwoLevelPaths& lookups) const
  {
    for (HfstTwoLevelPaths::const_iterator lookup = lookups.begin();
         lookup != lookups.end();
         ++lookup)
      {
        *out_ << separator_;
        for (StringPairVector::const_iterator sp = lookup->second.begin();
             sp != lookup->second.end();
             ++sp)
          {
            *out_ << sp->second;
          }
        *out_ << separator_;
      }
  }

void
TsvPathPrinter::print_end_cohort() const
  {
    *out_ << record_end_;
  }

void
TsvPathPrinter::print_end_file() const
  {
    // there's like no footer for TSV
  }

void
TsvPathPrinter::print_path_now(const HfstTwoLevelPath& path) const
  {
    *out_ << separator_;
    for (StringPairVector::const_iterator sp = path.second.begin();
         sp != path.second.end();
         ++sp)
      {
        *out_ << sp->second;
      }
    *out_ << separator_;
  }

void
TsvPathPrinter::set_field_separator(const std::string& sep)
  {
    separator_ = sep;
  }

void
TsvPathPrinter::set_quotation_mark(const std::string& quot)
  {
    quotation_ = quot;
  }

#if UNIT_TEST_MAIN

#include <iostream>

int
main(int, char**)
  {
    std::cout << "Unit tests for " << __FILE__ << ":" << std::endl;
    HfstOneLevelPath cat1;
    cat1.first = 0;
    cat1.second.push_back("c");
    cat1.second.push_back("a");
    cat1.second.push_back("t");
    HfstOneLevelPath dog1;
    dog1.first = 0;
    dog1.second.push_back("d");
    dog1.second.push_back("o");
    dog1.second.push_back("g");
    HfstTwoLevelPath cat2;
    cat2.first = 0;
    cat2.second.push_back(StringPair("c", "c"));
    cat2.second.push_back(StringPair("a", "a"));
    cat2.second.push_back(StringPair("t", "t"));
    cat2.second.push_back(StringPair("", "+N"));
    HfstTwoLevelPath dog2;
    dog2.first = 0;
    dog2.second.push_back(StringPair("d", "d"));
    dog2.second.push_back(StringPair("o", "o"));
    dog2.second.push_back(StringPair("g", "g"));
    dog2.second.push_back(StringPair("", "+V"));
    HfstTwoLevelPaths cats2;
    cats2.insert(cat2);
    HfstTwoLevelPaths dogs2;
    dogs2.insert(dog2);
    std::cout << "construct(cin)...";
    TsvPathPrinter tpp(&std::cout);
    std::cout << " ok" << std::endl;
    std::cout << "print long way:" << std::endl;
    tpp.print_begin_file();
    tpp.print_begin_cohort(cat1);
    tpp.print_lookup_set(cats2);
    tpp.print_end_cohort();
    tpp.print_end_file();
    std::cout << std::endl << "ok." << std::endl;
    std::cout << "print only start+result path:" << std::endl;
    tpp.print_begin_cohort(dog1);
    tpp.print_path_now(dog2);
    tpp.print_end_cohort();
    return 0;
  }
#endif
