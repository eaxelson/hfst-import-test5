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

#include "HfstTransducer.h"
#include "HfstGrammar.h"

#ifndef DEBUG_MAIN
namespace hfst
{
#if HAVE_OPENFST
  HfstGrammar::HfstGrammar(HfstTransducerVector &rule_vector):
  first_rule(*(rule_vector.begin()))
  {
    for (HfstTransducerVector::iterator it = rule_vector.begin();
	 it != rule_vector.end();
	 ++it)
      {
	if (it != rule_vector.begin())
	  {
	    it->harmonize(first_rule);
	  }
	it->convert(TROPICAL_OFST_TYPE);
      }

    for (HfstTransducerVector::iterator it = rule_vector.begin();
	 it != rule_vector.end();
	 ++it)
      {
	transducer_vector.push_back(it->implementation.tropical_ofst);
      }

    grammar = new hfst::implementations::Grammar(transducer_vector);
  }

  HfstGrammar::HfstGrammar(HfstTransducer &rule):
    first_rule(rule)
  {
    rule.convert(TROPICAL_OFST_TYPE);
    transducer_vector.push_back(rule.implementation.tropical_ofst);
    grammar = new hfst::implementations::Grammar(transducer_vector);
  }
    
  HfstGrammar::~HfstGrammar(void) {
    delete grammar; }

  HfstTransducer HfstGrammar::get_first_rule(void) {
    return first_rule; }
#endif 
}
#else
#include <iostream>
#include "HfstDataTypes.h"

using namespace hfst;

int
main(int argc, char** argv)
{
  std::cout << "Unit tests for " __FILE__ ":";
  hfst::HfstTransducerVector rules;
#if HAVE_OPENFST
  HfstTransducer rule(TROPICAL_OFST_TYPE);
  for (int i = 0; i <= 10; i++)
    {
      char* n = static_cast<char*>(malloc(sizeof(char)*3));
      if (sprintf(n, "%d", i) < 1)
        {
          std::cerr << "sprintf fail" << std::endl;
          return EXIT_FAILURE;
        }
      HfstTransducer numberedRule(n, TROPICAL_OFST_TYPE);
      rules.push_back(numberedRule);
    }
  std::cout << std::endl << "constructors:";
  std::cout << " (vector)...";
  HfstGrammar vecRules(rules);
  std::cout << " (transducer)...";
  HfstGrammar aRule(rule);
  std::cout << std::endl << " destructor:";
  delete new HfstGrammar(rules);
  std::cout << std::endl << "get first rule:";
  HfstTransducer firstVec = vecRules.get_first_rule();
  HfstTransducer firstRule = aRule.get_first_rule();
  std::cout << "ok" << std::endl;
#endif
  return EXIT_SUCCESS;
}
#endif
