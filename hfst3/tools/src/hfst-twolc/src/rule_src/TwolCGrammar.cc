//! @file TwolCGrammar.cc
//!
//! @author Miikka Silfverberg

//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "TwolCGrammar.h"

std::string TwolCGrammar::get_original_name(const std::string &name)
{ return name.substr(0,name.find("SUBCASE:")); }

TwolCGrammar::TwolCGrammar(bool be_quiet,bool be_verbose,bool resolve_conflicts):
  be_quiet(be_quiet),
  be_verbose(be_verbose)
{
  left_arrow_rule_container.set_report_left_arrow_conflicts(not be_quiet);
  left_arrow_rule_container.set_resolve_left_arrow_conflicts
    (resolve_conflicts);
  right_arrow_rule_container.set_report_right_arrow_conflicts
    (be_verbose);
}

void TwolCGrammar::define_diacritics(const SymbolRange &diacritics)
{ 
  this->diacritics = diacritics; 
  OtherSymbolTransducer::define_diacritics(diacritics);
}

void TwolCGrammar::add_rule(const std::string &name,
			    const SymbolPair &center,
			    op::OPERATOR oper,
			    const OtherSymbolTransducerVector contexts)
{
  Rule * rule;
  switch (oper)
    {
    case op::RIGHT:
      rule = new ConflictResolvingRightArrowRule(name,center,contexts);
      right_arrow_rule_container.add_rule_and_display_and_resolve_conflicts
	(static_cast<ConflictResolvingRightArrowRule*>(rule),std::cerr);
      break;
    case op::LEFT:
      rule = new ConflictResolvingLeftArrowRule(name,center,contexts);
      left_arrow_rule_container.add_rule_and_display_and_resolve_conflicts
	(static_cast<ConflictResolvingLeftArrowRule*>(rule),std::cerr); 
      break;
    case op::LEFT_RIGHT:
      rule = new ConflictResolvingRightArrowRule(name,center,contexts);
      right_arrow_rule_container.add_rule_and_display_and_resolve_conflicts
	(static_cast<ConflictResolvingRightArrowRule*>(rule),std::cerr); 
      name_to_rule_subcases[get_original_name(name)].insert(rule);
      rule = new ConflictResolvingLeftArrowRule(name,center,contexts);
      left_arrow_rule_container.add_rule_and_display_and_resolve_conflicts
	(static_cast<ConflictResolvingLeftArrowRule*>(rule),std::cerr); 
      break;
    case op::NOT_LEFT:
      rule = new LeftRestrictionArrowRule(name,center,contexts);
      other_rule_container.add_rule
	(static_cast<LeftRestrictionArrowRule*>(rule)); 
      break;
    }
  name_to_rule_subcases[get_original_name(name)].insert(rule);
}

void TwolCGrammar::add_rule(const std::string &name,
			    const SymbolPairVector &center,
			    op::OPERATOR oper,
			    const OtherSymbolTransducerVector contexts)
{
  OtherSymbolTransducer center_fst = Rule::get_center(center);

  Rule * rule;
  switch (oper)
    {
    case op::RIGHT:
      rule = new RightArrowRule(name,center_fst,contexts);
      other_rule_container.add_rule
	(static_cast<RightArrowRule*>(rule));
      break;
    case op::LEFT:
      rule = new LeftArrowRule(name,center_fst,contexts);
      other_rule_container.add_rule
	(static_cast<LeftArrowRule*>(rule));
      break;
    case op::LEFT_RIGHT:
      rule = new RightArrowRule(name,center_fst,contexts);
      other_rule_container.add_rule
	(static_cast<RightArrowRule*>(rule));

      rule = new LeftArrowRule(name,center_fst,contexts);
      other_rule_container.add_rule
	(static_cast<LeftArrowRule*>(rule));
      break;
    case op::NOT_LEFT:
      rule = new LeftRestrictionArrowRule(name,center_fst,contexts);
      other_rule_container.add_rule
	(static_cast<LeftRestrictionArrowRule*>(rule));
      break;
    }
  name_to_rule_subcases[get_original_name(name)].insert(rule);
}

void TwolCGrammar::compile_and_store(HfstOutputStream &out)
{
  if (not be_quiet)
    { std::cerr << "Compiling rules." << std::endl; }

  left_arrow_rule_container.compile(std::cerr,(not be_quiet) and be_verbose);
  right_arrow_rule_container.compile(std::cerr,(not be_quiet) and be_verbose);
  other_rule_container.compile(std::cerr,(not be_quiet) and be_verbose);

  for (StringRuleSetMap::const_iterator it = name_to_rule_subcases.begin();
       it != name_to_rule_subcases.end();
       ++it)
    { compiled_rule_container.add_rule
	(new Rule(it->first,Rule::RuleVector(it->second.begin(),
					     it->second.end()))); }
  compiled_rule_container.add_missing_symbols_freely(diacritics);

  if (not be_quiet)
    { std::cerr << "Storing rules." << std::endl; }
  compiled_rule_container.store(out,std::cerr,(not be_quiet) and be_verbose);
}

#ifdef TEST_TWOL_C_GRAMMAR
#include <cassert>
int main(void)
{
  TwolCGrammar g(true,true);

  HandySet<SymbolPair> symbols;
  symbols.insert(SymbolPair("a","b"));
  symbols.insert(SymbolPair("a","d"));
  symbols.insert(SymbolPair("b","c"));
  g.set_alphabet(symbols);
 
  OtherSymbolTransducer unknown("__HFST_TWOLC_?","__HFST_TWOLC_?");
  OtherSymbolTransducer diamond("__HFST_TWOLC_DIAMOND");
  OtherSymbolTransducer b_c_pair("b","c");

  OtherSymbolTransducer context = unknown;
  context.    
    apply(&HfstTransducer::concatenate,b_c_pair).
    apply(&HfstTransducer::concatenate,diamond).
    apply(&HfstTransducer::concatenate,unknown).
    apply(&HfstTransducer::concatenate,diamond).
    apply(&HfstTransducer::concatenate,unknown);
  OtherSymbolTransducerVector contexts(1,context);
  g.add_rule("\"test1\"",SymbolPair("a","b"),
	     op::LEFT_RIGHT,contexts);
  g.add_rule("\"test2\"",SymbolPair("a","b"),
	     op::LEFT_RIGHT,contexts);

  OtherSymbolTransducer a_d_pair("a","d");
  OtherSymbolTransducer context1 = unknown;
  context1.    
    //apply(&HfstTransducer::concatenate,a_d_pair).
    apply(&HfstTransducer::concatenate,b_c_pair).
    apply(&HfstTransducer::concatenate,diamond).
    apply(&HfstTransducer::concatenate,unknown).
    apply(&HfstTransducer::concatenate,diamond).
    apply(&HfstTransducer::concatenate,unknown);
  OtherSymbolTransducerVector contexts1(1,context1);
  g.add_rule("\"test3\"",SymbolPair("a","b"),
	     op::LEFT_RIGHT,contexts1);
}
#endif // TEST_TWOL_C_GRAMMAR
