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

namespace hfst
{
#if HAVE_SFST
  hfst::implementations::SfstTransducer HfstTransducer::sfst_interface;
#endif
#if HAVE_OPENFST
  hfst::implementations::TropicalWeightTransducer 
  HfstTransducer::tropical_ofst_interface;
  hfst::implementations::LogWeightTransducer
  HfstTransducer::log_ofst_interface;
#endif
#if HAVE_FOMA
  hfst::implementations::FomaTransducer HfstTransducer::foma_interface;
#endif


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
    
  HfstGrammar::~HfstGrammar(void)
  {
    delete grammar;
  }

  HfstTransducer &HfstGrammar::get_first_rule(void)
  {
    return first_rule;
  }
#endif  

#if HAVE_MUTABLE
  HfstTransitionIterator::HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s):
    tropical_ofst_iterator(hfst::implementations::TropicalWeightTransitionIterator(t.transducer.implementation.tropical_ofst, s)) {}

  HfstTransitionIterator::~HfstTransitionIterator(void) {}

  bool HfstTransitionIterator::done() { return tropical_ofst_iterator.done(); }
  
  HfstTransition HfstTransitionIterator::value()
  {
    hfst::implementations::TropicalWeightTransition twt = tropical_ofst_iterator.value();
    float weight = twt.get_weight().Value();
    HfstState target_state = twt.get_target_state();
    return HfstTransition( twt.get_input_symbol(),
			   twt.get_output_symbol(),
			   weight,
			   target_state );
  }
  
  void HfstTransitionIterator::next() { tropical_ofst_iterator.next(); }

  HfstStateIterator::HfstStateIterator(const HfstMutableTransducer &t):
    {
     
    }

  
  HfstStateIterator::~HfstStateIterator(void) {}
  
  bool HfstStateIterator::done() { return tropical_ofst_iterator.done(); }

  HfstState HfstStateIterator::value() { return tropical_ofst_iterator.value(); }

  void HfstStateIterator::next() { tropical_ofst_iterator.next(); }

  HfstTransition::HfstTransition(std::string isymbol, std::string osymbol, float weight, HfstState target_state):
    isymbol(isymbol), osymbol(osymbol), weight(weight), target_state(target_state) {}

  HfstTransition::~HfstTransition(void) {}

  std::string HfstTransition::get_input_symbol(void) { return isymbol; }

  std::string HfstTransition::get_output_symbol(void) { return osymbol; }
  
  float HfstTransition::get_weight(void) { return weight; }
  
  HfstState HfstTransition::get_target_state(void) { return target_state; }
#endif

  void collect_unknown_sets(StringSet &s1, StringSet &unknown1,
			    StringSet &s2, StringSet &unknown2)
  {
    for (StringSet::const_iterator it1 = s1.begin(); it1 != s1.end(); it1++) {
      String sym1 = *it1;
      if ( s2.find(sym1) == s2.end() )
	unknown2.insert(sym1);
    }
    for (StringSet::const_iterator it2 = s2.begin(); it2 != s2.end(); it2++) {
      String sym2 = *it2;
      if ( s1.find(sym2) == s1.end() )
	unknown1.insert(sym2);
    }
  }

  /*  Harmonize symbol-to-number encodings and expand unknown and identity symbols. 

      In the case of foma transducers, does nothing because foma's own functions
      take care of harmonizing. If harmonization is needed, FomaTransducer::harmonize
      can be used instead. */
  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    if (this->type != another.type)
      {
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    if (this->anonymous && another.anonymous) {
      return;
    }

    switch(this->type)
      {
#if HAVE_SFST
      case (SFST_TYPE):
	{
	  std::pair <SFST::Transducer*, SFST::Transducer*> result;
	  result =
	    sfst_interface.harmonize(this->implementation.sfst,
				     another.implementation.sfst);
	  this->implementation.sfst = result.first;
	  another.implementation.sfst = result.second;
	  break;
	}
#endif
#if HAVE_FOMA
      case (FOMA_TYPE):
	// no need to harmonize as foma's functions take care of harmonizing
	break;
#endif
#if HAVE_OPENFST
      case (TROPICAL_OFST_TYPE):
	{
	  std::pair <fst::StdVectorFst*, fst::StdVectorFst*> result;
	  result =
	    tropical_ofst_interface.harmonize(this->implementation.tropical_ofst,
					      another.implementation.tropical_ofst);
	  this->implementation.tropical_ofst = result.first;
	  another.implementation.tropical_ofst = result.second;
	  break;
	}
      case (LOG_OFST_TYPE):
	{
	  // this could be done with templates...
	  hfst::implementations::InternalTransducer *internal_this =
	    hfst::implementations::log_ofst_to_internal_format(this->implementation.log_ofst);
	  hfst::implementations::InternalTransducer *internal_another =
	    hfst::implementations::log_ofst_to_internal_format(another.implementation.log_ofst);
	  std::pair <fst::StdVectorFst*, fst::StdVectorFst*> result =
	    tropical_ofst_interface.harmonize(internal_this, internal_another);
	  this->implementation.log_ofst = hfst::implementations::internal_format_to_log_ofst(result.first);
	  another.implementation.log_ofst = hfst::implementations::internal_format_to_log_ofst(result.second);
	  break;
	}
#endif
      case (UNSPECIFIED_TYPE):
      case (ERROR_TYPE):
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  HfstTransducer::HfstTransducer():
    type(UNSPECIFIED_TYPE),anonymous(false),is_trie(true)
  {}

  HfstTransducer::HfstTransducer(ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.create_empty_transducer();
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.create_empty_transducer();
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.create_empty_transducer();
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = foma_interface.create_empty_transducer();
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	implementation.hfst_ol = hfst_ol_interface.create_empty_transducer(type==HFST_OLW_TYPE?true:false);
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }


  HfstTransducer::HfstTransducer(const std::string& utf8_str, 
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    StringPairVector * spv = 
      multichar_symbol_tokenizer.tokenize(utf8_str);
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*spv);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(*spv);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(*spv);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
  }

  HfstTransducer::HfstTransducer(const StringPairSet & sps, ImplementationType type):
    type(type),anonymous(false),is_trie(false)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(sps);
	this->type = SFST_TYPE;
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(sps);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(sps);
	this->type = LOG_OFST_TYPE;
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(sps);
	this->type = FOMA_TYPE;
	break;
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  HfstTransducer::HfstTransducer(const std::string& upper_utf8_str,
				 const std::string& lower_utf8_str,
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    StringPairVector * spv = 
      multichar_symbol_tokenizer.tokenize
      (upper_utf8_str,lower_utf8_str); //,key_table);
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*spv);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(*spv);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(*spv);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
  }


  HfstTransducer::HfstTransducer(HfstInputStream &in):
    type(in.type), anonymous(false),is_trie(false)
  { 
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();
    in.read_transducer(*this); 
  }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),is_trie(another.is_trie)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.copy(another.implementation.sfst);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst =
	  tropical_ofst_interface.copy(another.implementation.tropical_ofst);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst =
	  log_ofst_interface.copy(another.implementation.log_ofst);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = foma_interface.copy(another.implementation.foma);
	break;
#endif
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

#if HAVE_MUTABLE
  HfstTransducer::HfstTransducer(const HfstMutableTransducer &another):
    type(TROPICAL_OFST_TYPE), anonymous(another.transducer.anonymous), 
    is_trie(another.transducer.is_trie)
  {
    implementation.tropical_ofst =
      tropical_ofst_interface.copy(another.transducer.implementation.tropical_ofst);
  }
#endif


  HfstTransducer::~HfstTransducer(void)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	delete implementation.sfst;
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	delete implementation.tropical_ofst;
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	foma_interface.delete_foma(implementation.foma);
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	delete implementation.hfst_ol;
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

HfstTransducer::HfstTransducer(unsigned int number, ImplementationType type): 
type(type),anonymous(true),is_trie(false)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(number);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(number);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(number);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
	  char buf [255];
	  sprintf(buf, "\\%i", number);
	  implementation.foma = foma_interface.define_transducer(buf);
	  break;
	}
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }

HfstTransducer::HfstTransducer(unsigned int inumber, unsigned int onumber, ImplementationType type):
type(type),anonymous(true),is_trie(false)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(inumber, onumber);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(inumber, onumber);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(inumber, onumber);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
	  char ibuf [255];
	  sprintf(ibuf, "\\%i", inumber);
	  char obuf [255];
	  sprintf(obuf, "\\%i", onumber);
	  implementation.foma = foma_interface.define_transducer( ibuf, obuf );
	  break;
	}
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }

HfstTransducer::HfstTransducer(const std::string &symbol, ImplementationType type): 
type(type),anonymous(false),is_trie(false)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(symbol.c_str());
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(symbol);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(symbol);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer(strdup(symbol.c_str()));
	// should the char* be deleted?
	break;
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }

HfstTransducer::HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type):
type(type),anonymous(false),is_trie(false)
  {
    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(isymbol.c_str(), osymbol.c_str());
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(isymbol, osymbol);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(isymbol, osymbol);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer( strdup(isymbol.c_str()), strdup(osymbol.c_str()) );
	// should the char*:s be deleted?
	break;
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }


  ImplementationType HfstTransducer::get_type(void) const {
    return this->type;
  }

  bool HfstTransducer::are_equivalent(const HfstTransducer &one, const HfstTransducer &another) 
  {
    if (one.type != another.type)
      throw hfst::exceptions::TransducerTypeMismatchException();
    
    HfstTransducer one_copy(one);
    HfstTransducer another_copy(another);
    one_copy.harmonize(another_copy);
    one_copy.minimize();
    another_copy.minimize();

    switch (one_copy.type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	return one_copy.sfst_interface.are_equivalent(
                 one_copy.implementation.sfst, another_copy.implementation.sfst);
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	return one_copy.tropical_ofst_interface.are_equivalent(
                 one_copy.implementation.tropical_ofst, another_copy.implementation.tropical_ofst);
      case LOG_OFST_TYPE:
	return one_copy.log_ofst_interface.are_equivalent(
                 one_copy.implementation.log_ofst, another_copy.implementation.log_ofst);
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	return one_copy.foma_interface.are_equivalent(
                 one_copy.implementation.foma, another_copy.implementation.foma);
#endif
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }
  
  bool HfstTransducer::is_cyclic(void) const
  {
    switch(type)
    {
#if HAVE_SFST
      case SFST_TYPE:
        return sfst_interface.is_cyclic(implementation.sfst);
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
        return tropical_ofst_interface.is_cyclic(implementation.tropical_ofst);
      case LOG_OFST_TYPE:
        return log_ofst_interface.is_cyclic(implementation.log_ofst);
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        return foma_interface.is_cyclic(implementation.foma);
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
        return hfst_ol_interface.is_cyclic(implementation.hfst_ol);
      case ERROR_TYPE:
      default:
        throw hfst::exceptions::TransducerHasWrongTypeException();
        return false;
    }
  }

  HfstTransducer &HfstTransducer::remove_epsilons()
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::remove_epsilons,
       &hfst::implementations::TropicalWeightTransducer::remove_epsilons,
       &hfst::implementations::LogWeightTransducer::remove_epsilons,
       &hfst::implementations::FomaTransducer::remove_epsilons); }

  HfstTransducer &HfstTransducer::determinize()
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::determinize,
       &hfst::implementations::TropicalWeightTransducer::determinize,
       &hfst::implementations::LogWeightTransducer::determinize,
       &hfst::implementations::FomaTransducer::determinize); }

  HfstTransducer &HfstTransducer::minimize()
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::minimize,
       &hfst::implementations::TropicalWeightTransducer::minimize,
       &hfst::implementations::LogWeightTransducer::minimize,
       &hfst::implementations::FomaTransducer::minimize); }

  HfstTransducer &HfstTransducer::repeat_star()
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_star,
       &hfst::implementations::TropicalWeightTransducer::repeat_star,
       &hfst::implementations::LogWeightTransducer::repeat_star,
       &hfst::implementations::FomaTransducer::repeat_star); }

  HfstTransducer &HfstTransducer::repeat_plus()
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_plus,
       &hfst::implementations::TropicalWeightTransducer::repeat_plus,
       &hfst::implementations::LogWeightTransducer::repeat_plus,
       &hfst::implementations::FomaTransducer::repeat_plus); }

  HfstTransducer &HfstTransducer::repeat_n(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_n,
       &hfst::implementations::LogWeightTransducer::repeat_n,
       &hfst::implementations::FomaTransducer::repeat_n,
       n); }

  HfstTransducer &HfstTransducer::repeat_n_plus(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer this_repeat_n = HfstTransducer(*this).repeat_n(n);
    HfstTransducer this_repeat_star = HfstTransducer(*this).repeat_star();
    HfstTransducer &retval = (this_repeat_n).concatenate(this_repeat_star);
    return retval;
  }

  HfstTransducer &HfstTransducer::repeat_n_minus(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_le_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_le_n,
       &hfst::implementations::LogWeightTransducer::repeat_le_n,
       &hfst::implementations::FomaTransducer::repeat_le_n,
       n); }

  HfstTransducer &HfstTransducer::repeat_n_to_k(unsigned int n, unsigned int k)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer a(*this);
    return (a.repeat_n(n).concatenate(this->repeat_n_minus(k)));   // FIX: memory leaks? 
  }

  HfstTransducer &HfstTransducer::optionalize()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::optionalize,
       &hfst::implementations::TropicalWeightTransducer::optionalize,
       &hfst::implementations::LogWeightTransducer::optionalize,
       &hfst::implementations::FomaTransducer::optionalize); }

  HfstTransducer &HfstTransducer::invert()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::invert,
       &hfst::implementations::TropicalWeightTransducer::invert,
       &hfst::implementations::LogWeightTransducer::invert,
       &hfst::implementations::FomaTransducer::invert); }

  HfstTransducer &HfstTransducer::reverse()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::reverse,
       &hfst::implementations::TropicalWeightTransducer::reverse,
       &hfst::implementations::LogWeightTransducer::reverse,
       &hfst::implementations::FomaTransducer::reverse); }

  HfstTransducer &HfstTransducer::input_project()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_input_language,
       &hfst::implementations::TropicalWeightTransducer::extract_input_language,
       &hfst::implementations::LogWeightTransducer::extract_input_language,
       &hfst::implementations::FomaTransducer::extract_input_language); }

  HfstTransducer &HfstTransducer::output_project()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_output_language,
       &hfst::implementations::TropicalWeightTransducer::extract_output_language,
       &hfst::implementations::LogWeightTransducer::extract_output_language,
       &hfst::implementations::FomaTransducer::extract_output_language); }

  void HfstTransducer::extract_strings(ExtractStringsCb& callback, int cycles)
  { 
    switch (this->type)
      {
#if HAVE_OPENFST
      case LOG_OFST_TYPE:
	hfst::implementations::LogWeightTransducer::extract_strings
	  (implementation.log_ofst,callback,cycles);
	break;
      case TROPICAL_OFST_TYPE:
	hfst::implementations::TropicalWeightTransducer::extract_strings
	  (implementation.tropical_ofst,callback,cycles);
	break;
#endif
#if HAVE_SFST
      case SFST_TYPE:
	hfst::implementations::SfstTransducer::extract_strings(implementation.sfst, callback, cycles);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	hfst::implementations::FomaTransducer::extract_strings(implementation.foma, callback, cycles);
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	hfst::implementations::HfstOlTransducer::extract_strings(implementation.hfst_ol, callback, cycles);
	break;
      default:
	throw hfst::exceptions::FunctionNotImplementedException(); 
	break;
      }
  }
  
  void HfstTransducer::extract_strings_fd(ExtractStringsCb& callback, int cycles, bool filter_fd)
  { 
    switch (this->type)
      {
#if HAVE_OPENFST
      case LOG_OFST_TYPE:
      {
	FdTable<int64>* t_log_ofst = hfst::implementations::LogWeightTransducer::get_flag_diacritics(implementation.log_ofst);
	hfst::implementations::LogWeightTransducer::extract_strings
	  (implementation.log_ofst,callback,cycles,t_log_ofst,filter_fd);
	delete t_log_ofst;
      }
	break;
      case TROPICAL_OFST_TYPE:
      {
	FdTable<int64>* t_tropical_ofst = hfst::implementations::TropicalWeightTransducer::get_flag_diacritics(implementation.tropical_ofst);
	hfst::implementations::TropicalWeightTransducer::extract_strings
	  (implementation.tropical_ofst,callback,cycles,t_tropical_ofst,filter_fd);
	delete t_tropical_ofst;
      }
	break;
#endif
#if HAVE_SFST
      case SFST_TYPE:
      {
	FdTable<SFST::Character>* t_sfst = hfst::implementations::SfstTransducer::get_flag_diacritics(implementation.sfst);
	hfst::implementations::SfstTransducer::extract_strings(implementation.sfst, callback, cycles, t_sfst, filter_fd);
	delete t_sfst;
      }
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
      {
	  FdTable<int>* t_foma = hfst::implementations::FomaTransducer::get_flag_diacritics(implementation.foma);
	  hfst::implementations::FomaTransducer::extract_strings(implementation.foma, callback, cycles, t_foma, filter_fd);
	  delete t_foma;
      }
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      {
	const FdTable<hfst_ol::SymbolNumber>* t_hfst_ol = hfst::implementations::HfstOlTransducer::get_flag_diacritics(implementation.hfst_ol);
	hfst::implementations::HfstOlTransducer::extract_strings(implementation.hfst_ol,callback,cycles,t_hfst_ol,filter_fd);
	delete t_hfst_ol;
      }
	break;
      default:
	throw hfst::exceptions::FunctionNotImplementedException(); 
	break;
      }
  }
  
  class ExtractStringsCb_ : public ExtractStringsCb
  {
    public:
      WeightedPaths<float>::Set& paths;
      int max_num;
      
      ExtractStringsCb_(WeightedPaths<float>::Set& p, int max): paths(p), max_num(max) {}
      RetVal operator()(WeightedPath<float>& path, bool final)
      {
        if(final)
          paths.insert(path);
        
        return RetVal((max_num < 1) || (int)paths.size() < max_num, true);
      }
  };
  
  void HfstTransducer::extract_strings(WeightedPaths<float>::Set &results, int max_num, int cycles)
  {
    if(is_cyclic() && max_num < 1 && cycles < 0)
      throw hfst::exceptions::TransducerIsCyclicException();
    
    ExtractStringsCb_ cb(results, max_num);
    extract_strings(cb, cycles);
  }
  
  void HfstTransducer::extract_strings_fd(WeightedPaths<float>::Set &results, int max_num, int cycles, bool filter_fd)
  {
    if(is_cyclic() && max_num < 1 && cycles < 0)
      throw hfst::exceptions::TransducerIsCyclicException();
    
    ExtractStringsCb_ cb(results, max_num);
    extract_strings_fd(cb, cycles, filter_fd);
  }

  HfstTransducer &HfstTransducer::insert_freely(const StringPair &symbol_pair)
  {
    switch (this->type)    
      {
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  hfst::implementations::TropicalWeightTransducer::insert_freely
	    (implementation.tropical_ofst,symbol_pair);
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogWeightTransducer::insert_freely
	    (implementation.log_ofst,symbol_pair);
	  break;
	}
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	{
	  hfst::implementations::HfstInternalTransducer * internal_transducer = 
	    hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	  this->foma_interface.delete_foma(implementation.foma);
	  internal_transducer->insert_freely(symbol_pair);
	  implementation.foma = 
	    hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	  delete internal_transducer;
	  break;
	}
#endif
#if HAVE_SFST
      case SFST_TYPE:
	{
	  hfst::implementations::Transducer * temp =
	    hfst::implementations::SfstTransducer::insert_freely
	    (implementation.sfst,symbol_pair);
	  delete implementation.sfst;
	  implementation.sfst = temp;
	  break;
	}
#endif
      default:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;       
      }

    return *this;
  }

  HfstTransducer &HfstTransducer::substitute
  (void (*func)(std::string &isymbol, std::string &osymbol))
  {
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::sfst_to_internal_hfst_format(implementation.sfst);
	delete implementation.sfst;
	internal_transducer->substitute(func);
	implementation.sfst = 
	  hfst::implementations::hfst_internal_format_to_sfst(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	this->foma_interface.delete_foma(implementation.foma);
	internal_transducer->substitute(func);
	implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      {
	fst::StdVectorFst * tropical_ofst_temp =
	  this->tropical_ofst_interface.substitute(implementation.tropical_ofst,func);
	delete implementation.tropical_ofst;
	implementation.tropical_ofst = tropical_ofst_temp;
	return *this;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	hfst::implementations::LogFst * log_ofst_temp =
	  this->log_ofst_interface.substitute(implementation.log_ofst,func);
	delete implementation.log_ofst;
	implementation.log_ofst = log_ofst_temp;
	return *this;
      }
#endif
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol, bool input_side, bool output_side)
  {
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
	if (input_side && output_side)
	  {
	    hfst::implementations::Transducer * tmp =
	      this->sfst_interface.substitute(implementation.sfst, old_symbol, new_symbol);
	    delete implementation.sfst;
	    implementation.sfst = tmp;
	    return *this;
	  }
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::sfst_to_internal_hfst_format(implementation.sfst);
	delete implementation.sfst;
	internal_transducer->substitute(old_symbol, new_symbol, input_side, output_side);
	implementation.sfst = 
	  hfst::implementations::hfst_internal_format_to_sfst(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	this->foma_interface.delete_foma(implementation.foma);
	internal_transducer->substitute(old_symbol, new_symbol, input_side, output_side);
	implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      {
	if (input_side && output_side)
	  {
	    fst::StdVectorFst * tmp =
	      this->tropical_ofst_interface.substitute(implementation.tropical_ofst, old_symbol, new_symbol);
	    delete implementation.tropical_ofst;
	    implementation.tropical_ofst = tmp;
	    return *this;
	  }
	fst::StdVectorFst * tropical_ofst_temp =
	  this->tropical_ofst_interface.substitute(implementation.tropical_ofst, old_symbol, new_symbol);
	delete implementation.tropical_ofst;
	implementation.tropical_ofst = tropical_ofst_temp;
	return *this;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	if (input_side && output_side)
	  {
	    hfst::implementations::LogFst * tmp =
	      this->log_ofst_interface.substitute(implementation.log_ofst, old_symbol, new_symbol);
	    delete implementation.log_ofst;
	    implementation.log_ofst = tmp;
	    return *this;
	  }
	hfst::implementations::LogFst * log_ofst_temp =
	  this->log_ofst_interface.substitute(implementation.log_ofst, old_symbol, new_symbol);
	delete implementation.log_ofst;
	implementation.log_ofst = log_ofst_temp;
	return *this;
      }
#endif
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPair &new_symbol_pair)
  { 
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::sfst_to_internal_hfst_format(implementation.sfst);
	delete implementation.sfst;
	internal_transducer->substitute(old_symbol_pair, new_symbol_pair);
	implementation.sfst = 
	  hfst::implementations::hfst_internal_format_to_sfst(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	this->foma_interface.delete_foma(implementation.foma);
	internal_transducer->substitute(old_symbol_pair, new_symbol_pair);
	implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      {
	fst::StdVectorFst * tropical_ofst_temp =
	  this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol_pair, new_symbol_pair);
	delete implementation.tropical_ofst;
	implementation.tropical_ofst = tropical_ofst_temp;
	return *this;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	hfst::implementations::LogFst * log_ofst_temp =
	  this->log_ofst_interface.substitute(implementation.log_ofst,old_symbol_pair,new_symbol_pair);
	delete implementation.log_ofst;
	implementation.log_ofst = log_ofst_temp;
	return *this;
      }
#endif
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPairSet &new_symbol_pair_set)
  { 
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::sfst_to_internal_hfst_format(implementation.sfst);
	delete implementation.sfst;
	internal_transducer->substitute(old_symbol_pair, new_symbol_pair_set);
	implementation.sfst = 
	  hfst::implementations::hfst_internal_format_to_sfst(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	this->foma_interface.delete_foma(implementation.foma);
	internal_transducer->substitute(old_symbol_pair, new_symbol_pair_set);
	implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      {
	fst::StdVectorFst * tropical_ofst_temp =
	  this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol_pair, new_symbol_pair_set);
	delete implementation.tropical_ofst;
	implementation.tropical_ofst = tropical_ofst_temp;
	return *this;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	hfst::implementations::LogFst * log_ofst_temp =
	  this->log_ofst_interface.substitute(implementation.log_ofst,old_symbol_pair, new_symbol_pair_set);
	delete implementation.log_ofst;
	implementation.log_ofst = log_ofst_temp;
	return *this;
      }
#endif
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &symbol_pair,
   HfstTransducer &transducer)
  { 
    if (this->type != transducer.type)
      throw hfst::exceptions::TransducerTypeMismatchException ();   
    this->harmonize(transducer);

#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
	// HfstTransducer::harmonize does nothing to a foma transducer,
	// because foma's own functions take care of harmonizing.
	// Now we need to harmonize because we are using internal transducers.
	this->foma_interface.harmonize(implementation.foma,transducer.implementation.foma);

	hfst::implementations::HfstInternalTransducer * internal_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(implementation.foma);
	this->foma_interface.delete_foma(implementation.foma);

	hfst::implementations::HfstInternalTransducer * internal_substituting_transducer = 
	  hfst::implementations::foma_to_internal_hfst_format(transducer.implementation.foma);

	internal_transducer->substitute(symbol_pair, *internal_substituting_transducer);
	delete internal_substituting_transducer;
	implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	delete internal_transducer;
	return *this;
      }
#endif
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
	hfst::implementations::Transducer * tmp =
	  this->sfst_interface.substitute(implementation.sfst, symbol_pair, transducer.implementation.sfst);
	delete implementation.sfst;
	implementation.sfst = tmp;
	return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      {
	this->tropical_ofst_interface.substitute(implementation.tropical_ofst,symbol_pair,transducer.implementation.tropical_ofst);
	return *this;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	this->log_ofst_interface.substitute(implementation.log_ofst,symbol_pair,transducer.implementation.log_ofst);
	return *this;
      }
#endif
    throw hfst::exceptions::FunctionNotImplementedException();
  }

  HfstTransducer &HfstTransducer::set_final_weights(float weight)
  {
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      implementation.tropical_ofst  =
	this->tropical_ofst_interface.set_final_weights(this->implementation.tropical_ofst, weight);
    if (this->type == LOG_OFST_TYPE)
      implementation.log_ofst  =
	this->log_ofst_interface.set_final_weights(this->implementation.log_ofst, weight);
#endif
    return *this;
  }

  HfstTransducer &HfstTransducer::push_weights(PushType push_type)
  {
#if HAVE_OPENFST
    bool to_initial_state = (push_type == TO_INITIAL_STATE);
    if (this->type == TROPICAL_OFST_TYPE) 
      {
	hfst::implementations::StdVectorFst * tmp  =
	  this->tropical_ofst_interface.push_weights(this->implementation.tropical_ofst, to_initial_state);
	delete this->implementation.tropical_ofst;
	this->implementation.tropical_ofst = tmp;
      }
    if (this->type == LOG_OFST_TYPE)
      {
	hfst::implementations::LogFst * tmp =
	  this->log_ofst_interface.push_weights(this->implementation.log_ofst, to_initial_state);
	delete this->implementation.log_ofst;
	this->implementation.log_ofst = tmp;
      }
#endif
    return *this;
  }


  HfstTransducer &HfstTransducer::transform_weights(float (*func)(float))
  {
#if HAVE_OPENFST
    if (this->type == TROPICAL_OFST_TYPE)
      implementation.tropical_ofst  =
	this->tropical_ofst_interface.transform_weights(this->implementation.tropical_ofst, func);
    if (this->type == LOG_OFST_TYPE)
      implementation.log_ofst  =
	this->log_ofst_interface.transform_weights(this->implementation.log_ofst, func);
#endif
    return *this;
  }

  void substitute_single_identity_with_unknown(std::string &isymbol, std::string &osymbol) {
    if (isymbol.compare("@_IDENTITY_SYMBOL_@") && (osymbol.compare("@_IDENTITY_SYMBOL_@") == false))
      isymbol = std::string("@_UNKNOWN_SYMBOL_@");
    else if (osymbol.compare("@_IDENTITY_SYMBOL_@") && (isymbol.compare("@_IDENTITY_SYMBOL_@") == false))
      osymbol = std::string("@_UNKNOWN_SYMBOL_@");
    else
      return;
  }

  void substitute_unknown_identity_pairs(std::string &isymbol, std::string &osymbol) {
    if (isymbol.compare("@_UNKNOWN_SYMBOL_@") && isymbol.compare("@_IDENTITY_SYMBOL_@")) {
      isymbol = std::string("@_IDENTITY_SYMBOL_@");
      osymbol = std::string("@_IDENTITY_SYMBOL_@");
    }
  }

  HfstTransducer &HfstTransducer::compose
  (const HfstTransducer &another)
  { is_trie = false;
    
    if (this->type != another.type)
      throw hfst::exceptions::TransducerTypeMismatchException();

    this->harmonize(const_cast<HfstTransducer&>(another));

    if (this->type != FOMA_TYPE) 
      {
	this->substitute("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",false,true);
	(const_cast<HfstTransducer&>(another)).substitute("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",true,false);
      }

    switch (this->type)
      {
      case SFST_TYPE:
        {
	  SFST::Transducer * sfst_temp =
            this->sfst_interface.compose(implementation.sfst,another.implementation.sfst);
          delete implementation.sfst;
          implementation.sfst = sfst_temp;
          break;
        }
      case TROPICAL_OFST_TYPE:
        {
	  fst::StdVectorFst * tropical_ofst_temp =
            this->tropical_ofst_interface.compose(this->implementation.tropical_ofst,
                                another.implementation.tropical_ofst);
          delete implementation.tropical_ofst;
          implementation.tropical_ofst = tropical_ofst_temp;
          break;
        }
      case LOG_OFST_TYPE:
        {
	  hfst::implementations::LogFst * log_ofst_temp =
            this->log_ofst_interface.compose(implementation.log_ofst,
                           another.implementation.log_ofst);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
      case FOMA_TYPE:
        {
          fsm * foma_temp =
            this->foma_interface.compose(implementation.foma,another.implementation.foma);
          this->foma_interface.delete_foma(implementation.foma);
          implementation.foma = foma_temp;
          break;
        }
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    if (this->type != FOMA_TYPE) 
      {
	this->substitute(*substitute_single_identity_with_unknown);
	(const_cast<HfstTransducer&>(another)).substitute(*substitute_unknown_identity_pairs);
      }

    return *this;



    /*    if (this->type != FOMA_TYPE) {
      this->substitute(&substitute_output_identity_with_unknown);
    }
    if (another.type != FOMA_TYPE) {
      (const_cast<HfstTransducer&>(another)).substitute(&substitute_input_identity_with_unknown);
      }*/

    return apply(&hfst::implementations::SfstTransducer::compose,
		 &hfst::implementations::TropicalWeightTransducer::compose,
		 &hfst::implementations::LogWeightTransducer::compose,
		 &hfst::implementations::FomaTransducer::compose,
		 const_cast<HfstTransducer&>(another)); 

    /*
    (const_cast<HfstTransducer&>(another)).substitute(&substitute_unknown_identity_pair);
    return this->substitute(&substitute_single_identitity_with_unknown);
    */
  }

#if HAVE_OPENFST
  HfstTransducer &HfstTransducer::compose_intersect
  (HfstGrammar &grammar)
  {
    HfstTransducer rule_copy(grammar.get_first_rule());
    harmonize(rule_copy);
    switch (type)
      {
      case TROPICAL_OFST_TYPE:
	{
	  fst::ArcSort<fst::StdArc,hfst::implementations::StdMyOLabelCompare>
	    (implementation.tropical_ofst,hfst::implementations::StdMyOLabelCompare());
	  fst::StdVectorFst * temp = implementation.tropical_ofst;
	  implementation.tropical_ofst =
	    hfst::implementations::TropicalWeightTransducer::compose_intersect
	    (implementation.tropical_ofst,grammar.grammar);
	  delete temp;
	  break;
	}
      default:
	throw hfst::exceptions::FunctionNotImplementedException(); 
      }
    return *this;
  }
#endif

  HfstTransducer &HfstTransducer::concatenate
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(&hfst::implementations::SfstTransducer::concatenate,
		 &hfst::implementations::TropicalWeightTransducer::concatenate,
		 &hfst::implementations::LogWeightTransducer::concatenate,
		 &hfst::implementations::FomaTransducer::concatenate,
		 const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::disjunct_as_tries(HfstTransducer &another,
						    ImplementationType type)
  {
    if (type != UNSPECIFIED_TYPE)
      { 
	convert(type);
	if (type != another.type)
	  { another = HfstTransducer(another).convert(type); }
      }
    switch (this->type)
      {
      case SFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case TROPICAL_OFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
#if HAVE_OPENFST
      case LOG_OFST_TYPE:
	hfst::implementations::LogWeightTransducer::disjunct_as_tries
	  (*implementation.log_ofst,another.implementation.log_ofst);
	break;
#endif
      case FOMA_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      default:
	assert(false);
      }
    return *this; 
  }

  HfstTransducer &HfstTransducer::n_best
  (unsigned int n)
  {
    if (not is_implementation_type_available(TROPICAL_OFST_TYPE))
      throw hfst::exceptions::FunctionNotImplementedException();

    ImplementationType original_type = this->type;
#if HAVE_OPENFST
    if (original_type == SFST_TYPE || original_type == FOMA_TYPE) {
      this->convert(TROPICAL_OFST_TYPE);
    }
#endif

    switch (this->type)
      {
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::n_best
	    (implementation.tropical_ofst,(int)n);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  break;
      }
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::n_best
	    (implementation.log_ofst,(int)n);
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  break;
	}
#endif
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      default:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;       
      }
#if HAVE_OPENFST
    this->convert(original_type);
#endif
    return *this;
  }

  HfstTransducer &HfstTransducer::disjunct
  (const HfstTransducer &another)
  {
    is_trie = false;
    return apply(&hfst::implementations::SfstTransducer::disjunct,
		 &hfst::implementations::TropicalWeightTransducer::disjunct,
		 &hfst::implementations::LogWeightTransducer::disjunct,
		 &hfst::implementations::FomaTransducer::disjunct,
		 const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::intersect
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(&hfst::implementations::SfstTransducer::intersect,
		 &hfst::implementations::TropicalWeightTransducer::intersect,
		 &hfst::implementations::LogWeightTransducer::intersect,
		 &hfst::implementations::FomaTransducer::intersect,
		 const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::subtract
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(&hfst::implementations::SfstTransducer::subtract,
		 &hfst::implementations::TropicalWeightTransducer::subtract,
		 &hfst::implementations::LogWeightTransducer::subtract,
		 &hfst::implementations::FomaTransducer::subtract,
		 const_cast<HfstTransducer&>(another)); }


  HfstTransducer &HfstTransducer::convert(const HfstTransducer &t, ImplementationType type)
  {
    if (type == UNSPECIFIED_TYPE)
      { throw hfst::implementations::SpecifiedTypeRequiredException(); }

    // no need to convert, just return a copy
    if (type == t.type)
      { return *(new HfstTransducer(t)); }

    // the return value, contains an empty transducer that is later deleted
    HfstTransducer * retval = new HfstTransducer(type);

    try 
      {
	// make a new intermediate transducer of type TROPICAL_OFST_TYPE 
	hfst::implementations::InternalTransducer * internal;
	switch (t.type)
	  {
#if HAVE_FOMA
	  case FOMA_TYPE:
	    internal =
	      hfst::implementations::foma_to_internal_format(t.implementation.foma);
	    break;
#endif
#if HAVE_SFST
	  case SFST_TYPE:
	    internal = 
	      hfst::implementations::sfst_to_internal_format(t.implementation.sfst);
	    break;
#endif
#if HAVE_OPENFST
	  case TROPICAL_OFST_TYPE:
	    internal = t.implementation.tropical_ofst;  // just copy the pointer
	    break;
	  case LOG_OFST_TYPE:
	  internal =
	      hfst::implementations::log_ofst_to_internal_format(t.implementation.log_ofst);
	    break;
#endif
	  case HFST_OL_TYPE:
	  case HFST_OLW_TYPE:
	    internal =
	      hfst::implementations::hfst_ol_to_internal_format(t.implementation.hfst_ol);
	      break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }

	// transform the intermediate transducer into format defined by 'type'
	// and assign it as a value of retval
	switch (type)
	  {
#if HAVE_SFST
	  case SFST_TYPE:
	    delete retval->implementation.sfst;  // delete the empty transducer
	    retval->implementation.sfst = 
	      hfst::implementations::internal_format_to_sfst(internal);
	    if (t.type != TROPICAL_OFST_TYPE)  // if a new intermediate transducer was created,
	      delete internal;                  // delete it
	    break;
#endif
#if HAVE_OPENFST
	  case TROPICAL_OFST_TYPE:
	  case UNSPECIFIED_TYPE:
	    delete retval->implementation.tropical_ofst;
	    retval->implementation.tropical_ofst = internal;
	    retval->type = TROPICAL_OFST_TYPE;
	    break;                              // no new intermediate transducer was created
	  case LOG_OFST_TYPE:
	    delete retval->implementation.log_ofst;
	    retval->implementation.log_ofst =
	      hfst::implementations::internal_format_to_log_ofst(internal);
	    if (t.type != TROPICAL_OFST_TYPE)
	      delete internal;
	    break;
#endif
#if HAVE_FOMA
	  case FOMA_TYPE:
	    retval->foma_interface.delete_foma(retval->implementation.foma);
	    retval->implementation.foma =
	      hfst::implementations::internal_format_to_foma(internal);
	    if (t.type != TROPICAL_OFST_TYPE)
	      delete internal;
	    break;
#endif
	  case HFST_OL_TYPE:
	  case HFST_OLW_TYPE:
	    delete retval->implementation.hfst_ol;
	    retval->implementation.hfst_ol =
	      hfst::implementations::internal_format_to_hfst_ol(internal, type==HFST_OLW_TYPE?true:false);
	    if (t.type != TROPICAL_OFST_TYPE)
	      delete internal;
	    break;
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }
      }
    catch (hfst::implementations::HfstInterfaceException e)
      { throw e; }
    return *retval;
  }

  /* ERROR_TYPE or UNSPECIFIED_TYPE returns true, so they must be handled separately */
  bool HfstTransducer::is_implementation_type_available(ImplementationType type) {
#if !HAVE_FOMA
    if (type == FOMA_TYPE)
      return false;
#endif
#if !HAVE_SFST
    if (type == SFST_TYPE)
      return false;
#endif
#if !HAVE_OPENFST
    if (type == TROPICAL_OFST_TYPE || type == LOG_OFST_TYPE)
      return false;
#endif
    (void)type; 
    return true;
  }

  HfstTransducer &HfstTransducer::convert(ImplementationType type)
  {
    if (type == UNSPECIFIED_TYPE)
      { throw hfst::implementations::SpecifiedTypeRequiredException(); }
    if (type == this->type)
      { return *this; }

    if (not is_implementation_type_available(type))
      throw hfst::exceptions::ImplementationTypeNotAvailableException();

    try 
      {
	hfst::implementations::InternalTransducer * internal;
	switch (this->type)
	  {
#if HAVE_FOMA
	  case FOMA_TYPE:
	    internal =
	      hfst::implementations::foma_to_internal_format(implementation.foma);
	    foma_interface.delete_foma(implementation.foma);
	    break;
#endif
#if HAVE_SFST
	  case SFST_TYPE:
	    internal = 
	      hfst::implementations::sfst_to_internal_format(implementation.sfst);
	    delete implementation.sfst;
	    break;
#endif
#if HAVE_OPENFST
	  case TROPICAL_OFST_TYPE:
	    internal = implementation.tropical_ofst;
	    break;
	  case LOG_OFST_TYPE:
	  internal =
	      hfst::implementations::log_ofst_to_internal_format(implementation.log_ofst);
	  delete implementation.log_ofst;
	    break;
#endif
	  case HFST_OL_TYPE:
	  case HFST_OLW_TYPE:
	    internal =
	    hfst::implementations::hfst_ol_to_internal_format(implementation.hfst_ol);
	    delete implementation.hfst_ol;
	    break;
	case ERROR_TYPE:
	case UNSPECIFIED_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }

	this->type = type;
	switch (this->type)
	  {
#if HAVE_SFST
	  case SFST_TYPE:
	    implementation.sfst = 
	      hfst::implementations::internal_format_to_sfst(internal);
	    delete internal;
	    break;
#endif
#if HAVE_OPENFST
	  case TROPICAL_OFST_TYPE:
	  case UNSPECIFIED_TYPE:
	    implementation.tropical_ofst = internal;
	    this->type=TROPICAL_OFST_TYPE;
	    break;
	  case LOG_OFST_TYPE:
	    implementation.log_ofst =
	      hfst::implementations::internal_format_to_log_ofst(internal);
	    delete internal;
	    break;
#endif
#if HAVE_FOMA
	  case FOMA_TYPE:
	    implementation.foma =
	      hfst::implementations::internal_format_to_foma(internal);
	    delete internal;
	    break;
#endif
	  case HFST_OL_TYPE:
	  case HFST_OLW_TYPE:
	    implementation.hfst_ol =
	      hfst::implementations::internal_format_to_hfst_ol(internal, this->type==HFST_OLW_TYPE?true:false);
	    delete internal;
	    break;
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }
      }
    catch (hfst::implementations::HfstInterfaceException e)
      { throw e; }
    return *this;
  }

  hfst::implementations::HfstInternalTransducer * 
  HfstTransducer::hfst_transducer_to_internal(
		   HfstTransducer *transducer) {
    hfst::implementations::HfstInternalTransducer * internal_transducer;
    switch(transducer->type)
      {
      case SFST_TYPE:
	internal_transducer = hfst::implementations::sfst_to_internal_hfst_format(transducer->implementation.sfst);
	break;
      case TROPICAL_OFST_TYPE:
	internal_transducer = hfst::implementations::tropical_ofst_to_internal_hfst_format(transducer->implementation.tropical_ofst);
	break;
      case LOG_OFST_TYPE:
	internal_transducer = hfst::implementations::log_ofst_to_internal_hfst_format(transducer->implementation.log_ofst);
	break;
      case FOMA_TYPE:
	internal_transducer = hfst::implementations::foma_to_internal_hfst_format(transducer->implementation.foma);
	break;
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return internal_transducer;
  }

  HfstTransducer * HfstTransducer::internal_to_hfst_transducer(
		     hfst::implementations::HfstInternalTransducer * internal_transducer, 
		     ImplementationType type)
  {
    HfstTransducer * retval = new HfstTransducer(type);
    switch(retval->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
	delete retval->implementation.sfst;
	retval->implementation.sfst = 
	  hfst::implementations::hfst_internal_format_to_sfst(internal_transducer);
	break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OFST_TYPE:
	delete retval->implementation.tropical_ofst;
	retval->implementation.tropical_ofst = 
	  hfst::implementations::hfst_internal_format_to_tropical_ofst(internal_transducer);
	break;
      case LOG_OFST_TYPE:
	delete retval->implementation.log_ofst;
	retval->implementation.log_ofst = 
	  hfst::implementations::hfst_internal_format_to_log_ofst(internal_transducer);
	break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
	retval->foma_interface.delete_foma(retval->implementation.foma);
	retval->implementation.foma = 
	  hfst::implementations::hfst_internal_format_to_foma(internal_transducer);
	break;
#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return retval;
  }

void HfstTransducer::write_in_att_format(const char * filename)
{
  FILE * ofile = fopen(filename, "wb");
  if (ofile == NULL)
    throw hfst::exceptions::FileCannotBeWrittenException();
  write_in_att_format(ofile);
  fclose(ofile);
}

void HfstTransducer::write_in_att_format(FILE * ofile)
{
  hfst::implementations::HfstInternalTransducer * internal_transducer = 
    hfst_transducer_to_internal(this);
  internal_transducer->print_symbol(ofile);
  delete internal_transducer;
}

HfstTransducer &HfstTransducer::read_in_att_format(const char * filename, ImplementationType type)
{
  FILE * ifile = fopen(filename, "rb");
  if (ifile == NULL)
    throw hfst::exceptions::FileNotReadableException();
  HfstTransducer &retval = read_in_att_format(ifile, type);
  fclose(ifile);
  return retval;
}

HfstTransducer &HfstTransducer::read_in_att_format(FILE * ifile, ImplementationType type)
{
  if (not is_implementation_type_available(type))
    throw hfst::exceptions::ImplementationTypeNotAvailableException();
  hfst::implementations::HfstInternalTransducer * internal_transducer = new hfst::implementations::HfstInternalTransducer();
  internal_transducer->alphabet = new hfst::implementations::HfstAlphabet();
  internal_transducer->read_symbol(ifile);
  HfstTransducer *retval = internal_to_hfst_transducer(internal_transducer,type);
  delete internal_transducer;
  return *retval;
}


HfstTransducer &HfstTransducer::operator=(const HfstTransducer &another)
{
  // Check for self-assignment.
  if (&another == this)
    { return *this; }
  
  // set some features and the key table.
  anonymous = another.anonymous;
  is_trie = another.is_trie;
  
  // Delete old transducer.
  switch (this->type)
    {
    case FOMA_TYPE:
      delete implementation.foma;
      break;
    case SFST_TYPE:
      delete implementation.sfst;
      break;
    case TROPICAL_OFST_TYPE:
      delete implementation.tropical_ofst;
      break;
    case LOG_OFST_TYPE:
      delete implementation.log_ofst;
      break;
    case UNSPECIFIED_TYPE:
    case ERROR_TYPE:
    default:
      throw hfst::exceptions::TransducerHasWrongTypeException();
    }
    
  // Set new transducer.
  
  // Sfst doesn't have a const copy constructor, so we need to do a
  // const_cast here. Shouldn't be a problem...
  HfstTransducer &another_1 = const_cast<HfstTransducer&>(another);
  type = another.type;
  switch (type)
    {
    case FOMA_TYPE:
      implementation.foma = 
	foma_interface.copy(another_1.implementation.foma);
      break;
    case SFST_TYPE:
      implementation.sfst = 
	sfst_interface.copy(another_1.implementation.sfst);
      break;
    case TROPICAL_OFST_TYPE:
      implementation.tropical_ofst = 
	tropical_ofst_interface.copy(another_1.implementation.tropical_ofst);
      break;
    case LOG_OFST_TYPE:
      implementation.log_ofst = 
	log_ofst_interface.copy(another_1.implementation.log_ofst);
      break;
    default:
      (void)1;
    }
  return *this;

}

  HfstTokenizer HfstTransducer::create_tokenizer() 
  {
    throw hfst::exceptions::FunctionNotImplementedException();
    /*    HfstTokenizer tok;
    HfstMutableTransducer t(*this);
    HfstStateIterator state_it(t);
    while (not state_it.done()) 
      {
	HfstState s = state_it.value();
	HfstTransitionIterator transition_it(t,s);
	while (not transition_it.done()) 
	  {
	    HfstTransition tr = transition_it.value();
	    if (tr.isymbol.size() > 1) {
	      tok.add_multichar_symbol(tr.isymbol);
	    }
	    if (tr.isymbol.size() > 1) {
	      tok.add_multichar_symbol(tr.osymbol);
	    }
	    // special symbols are added too (is this a problem?)
	    transition_it.next();
	  }
	state_it.next();
      }
      return tok;*/
  }


std::ostream &operator<<(std::ostream &out,HfstTransducer &t)
  {
    hfst::implementations::HfstInternalTransducer * internal_transducer =
      HfstTransducer::hfst_transducer_to_internal(&t);
    internal_transducer->print_symbol(out);
    delete internal_transducer;
    return out;
  }

void HfstTransducer::print_alphabet()
{
  if (this->type == SFST_TYPE)
    this->sfst_interface.print_alphabet(this->implementation.sfst);
  return;
}


#if HAVE_MUTABLE
  // check that OpenFst is available
  HfstMutableTransducer::HfstMutableTransducer(void):
    transducer(HfstTransducer(TROPICAL_OFST_TYPE))
  {
  }

  // check that OpenFst is available
  HfstMutableTransducer::HfstMutableTransducer(const HfstTransducer &t):
    transducer(HfstTransducer(t).convert(TROPICAL_OFST_TYPE))
  {
    // guarantees that the internal representation of an empty transducer has at least one state
    transducer.tropical_ofst_interface.represent_empty_transducer_as_having_one_state(transducer.implementation.tropical_ofst);
  }

  HfstMutableTransducer::~HfstMutableTransducer(void)
  {
  }

  HfstState HfstMutableTransducer::add_state() 
  {
    return this->transducer.tropical_ofst_interface.add_state(
      this->transducer.implementation.tropical_ofst);
  }

  void HfstMutableTransducer::set_final_weight(HfstState s, float w)
  {
    this->transducer.tropical_ofst_interface.set_final_weight(
	    this->transducer.implementation.tropical_ofst, s, w);
  }

  void HfstMutableTransducer::add_transition(HfstState source, std::string isymbol, std::string osymbol, float w, HfstState target)
  {
    return this->transducer.tropical_ofst_interface.add_transition(
             this->transducer.implementation.tropical_ofst,
	     source,
	     isymbol,
	     osymbol,
	     w,
	     target);
  }

  float HfstMutableTransducer::get_final_weight(HfstState s)
  {
    return this->transducer.tropical_ofst_interface.get_final_weight(
	     this->transducer.implementation.tropical_ofst, s);
  }

  bool HfstMutableTransducer::is_final(HfstState s)
  {
    return this->transducer.tropical_ofst_interface.is_final(
	     this->transducer.implementation.tropical_ofst, s);
  }

  HfstState HfstMutableTransducer::get_initial_state()
  {
    return this->transducer.tropical_ofst_interface.get_initial_state(
	     this->transducer.implementation.tropical_ofst);
  }
#endif  /* MUTABLE */

}
#ifdef DEBUG_MAIN
using namespace hfst;
int main(void)
{
  KeyTable key_table;
  key_table.add_symbol("a");
  key_table.add_symbol("b");
  HfstTransducer sfst(SFST_TYPE);
  HfstTransducer sfst_kt(key_table,SFST_TYPE);
  HfstTransducer ofst(TROPICAL_OFST_TYPE);
  HfstTransducer ofst_kt(key_table,TROPICAL_OFST_TYPE);
  HfstTransducer sfst_copy(sfst_kt);
  HfstTransducer foma(FOMA_TYPE);
  std::cout << sfst << std::endl;
  KeyTable key_table_c = sfst_kt.get_key_table();
  assert(key_table_c["a"] == 1);
  assert(key_table_c["b"] == 2);
  HfstTokenizer tokenizer;
  HfstTransducer str_fst_ofst("äläkkä",tokenizer,TROPICAL_OFST_TYPE);
  HfstTransducer str_fst_ofst2("äläkKä","äläkkä",tokenizer,TROPICAL_OFST_TYPE);
  std::cout << str_fst_ofst << std::endl;
  str_fst_ofst.substitute("k","t");
  std::cout << str_fst_ofst << std::endl;
  str_fst_ofst.convert(SFST_TYPE).substitute("l","t");
  std::cout << str_fst_ofst << std::endl;
  str_fst_ofst2.convert(SFST_TYPE);
  assert(str_fst_ofst2.get_type() == SFST_TYPE);
  std::cout << str_fst_ofst2 << std::endl;
  HfstTransducer str_fst_sfst("äläkkä",tokenizer,SFST_TYPE);
  str_fst_sfst.convert(TROPICAL_OFST_TYPE).remove_epsilons();
  str_fst_sfst.determinize().minimize();
  str_fst_sfst.repeat_star().repeat_plus().optionalize().minimize();
  std::cout << str_fst_sfst << std::endl;
  str_fst_sfst.input_project().output_project();
  HfstTransducer str_fst_sfst_test("äläkkä",tokenizer,SFST_TYPE);
  str_fst_sfst_test.disjunct(str_fst_sfst_test);
  str_fst_ofst2.repeat_plus().compose(str_fst_sfst.convert(SFST_TYPE));
  std::cout << str_fst_ofst2.minimize() << std::endl;
  str_fst_ofst2.convert(TROPICAL_OFST_TYPE).set_final_weight<float>(2);
  std::cout << str_fst_ofst2 << std::endl;
  bool failed = false;
  try
    {
      str_fst_ofst2.convert(SFST_TYPE).set_final_weight<bool>(2);
    }
  catch (hfst::implementations::FunctionNotImplementedException e)
    { failed = true; }
  assert(failed);
  failed = false;
  try
    {
      str_fst_ofst2.set_final_weight<float>(2);
    }
  catch (hfst::implementations::WeightTypeMismatchException e)
    { failed = true; }
  assert(failed);
  failed = false;
  HfstTransducer sfst_ducer("kala",tokenizer,SFST_TYPE);
  KeyTable &kt = sfst_ducer.get_key_table();
  for (SfstTransducer::const_iterator it = sfst_ducer.begin<SfstTransducer>();
       it != sfst_ducer.end<SfstTransducer>();
       ++it)
    { 
      SfstState s = *it;
      for (SfstState::const_iterator jt = s.begin();
	   jt != s.end();
	   ++jt)
	{ 
	  SfstTransition tr = *jt;	  
	  std::cerr << "Transition with pair: " 
		    << kt[tr.get_input_key()] << " " 
		    << kt[tr.get_output_key()]
		    << std::endl;
	}
      if (s.get_final_weight())
	{ std::cerr << "Final state" << std::endl; }
    }
  //HfstInputStream in1("fst.sfst");
  //in1.open();
  //HfstTransducer t1(in1);
  //std::cout << t1 << std::endl;
  HfstInputStream in("fst.std.hfst");
  in.open();
  HfstTransducer t(in);
  std::cout << t << std::endl;
  in.close();
  HfstInputStream in2("fst.log.hfst");
  in2.open();
  HfstTransducer t2(in2);
  std::cout << t2 << std::endl;
  in2.close();
  HfstInputStream in3("fst.log.2.hfst");
  in3.open();
  HfstTransducer t3(in3);
  HfstTransducer t4(in3);
  t3.intersect(t4);
  t3.determinize();
  std::cout << t3 << std::endl;
  in3.close();
  HfstInputStream in4("fst.sfst.hfst");
  in4.open();
  HfstTransducer t5(in4);
  std::cout << t5 << std::endl;
  in4.close();

  HfstTransducer ala("ala",tokenizer,TROPICAL_OFST_TYPE);
  ala.concatenate(ala);
  ala.minimize();
  std::cout << ala << std::endl;
}
#endif
