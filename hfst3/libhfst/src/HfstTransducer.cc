#include "HfstTransducer.h"

namespace hfst
{
  hfst::implementations::SfstTransducer HfstTransducer::sfst_interface;
  hfst::implementations::TropicalWeightTransducer 
  HfstTransducer::tropical_ofst_interface;
  hfst::implementations::LogWeightTransducer
  HfstTransducer::log_ofst_interface;
  hfst::implementations::FomaTransducer HfstTransducer::foma_interface;

  
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
    tropical_ofst_iterator(hfst::implementations::TropicalWeightStateIterator(t.transducer.implementation.tropical_ofst)) {}

  
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

  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    if (this->type != another.type)
      {
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    if (this->anonymous && another.anonymous) {
      printf("no need to harmonize\n");
      return;
    }

    switch(this->type)
      {
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
      case (FOMA_TYPE):
	// no need to harmonize as foma's functions take care of harmonizing
	break;
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
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.create_empty_transducer();
	break;
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
      case FOMA_TYPE:
	implementation.foma = foma_interface.create_empty_transducer();
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
    StringPairVector * spv = 
      multichar_symbol_tokenizer.tokenize(utf8_str);
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*spv);
	break;
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
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
  }

  HfstTransducer::HfstTransducer(const StringPairSet & sps, ImplementationType type):
    type(type),anonymous(false),is_trie(false)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(sps);
	this->type = SFST_TYPE;
	break;
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
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(sps);
	this->type = FOMA_TYPE;
	break;
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
    StringPairVector * spv = 
      multichar_symbol_tokenizer.tokenize
      (upper_utf8_str,lower_utf8_str); //,key_table);
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*spv);
	break;
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
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
  }


  HfstTransducer::HfstTransducer(HfstInputStream &in):
    type(in.type), anonymous(false),is_trie(false)
  { in.read_transducer(*this); }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),is_trie(another.is_trie)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.copy(another.implementation.sfst);
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst =
	  tropical_ofst_interface.copy(another.implementation.tropical_ofst);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst =
	  log_ofst_interface.copy(another.implementation.log_ofst);
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.copy(another.implementation.foma);
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  HfstTransducer::HfstTransducer(const HfstMutableTransducer &another):
    type(TROPICAL_OFST_TYPE), anonymous(another.transducer.anonymous), 
    is_trie(another.transducer.is_trie)
  {
    implementation.tropical_ofst =
      tropical_ofst_interface.copy(another.transducer.implementation.tropical_ofst);
  }
  


  HfstTransducer::~HfstTransducer(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	delete implementation.sfst;
	break;
      case TROPICAL_OFST_TYPE:
	delete implementation.tropical_ofst;
	break;
      case LOG_OFST_TYPE:
	delete implementation.log_ofst;
	break;
      case FOMA_TYPE:
	foma_interface.delete_foma(implementation.foma);
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
    switch (this->type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(number);
	break;
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(number);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(number);
	break;
      case FOMA_TYPE:
	{
	  char buf [255];
	  sprintf(buf, "\\%i", number);
	  implementation.foma = foma_interface.define_transducer(buf);
	  break;
	}
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }

HfstTransducer::HfstTransducer(unsigned int inumber, unsigned int onumber, ImplementationType type):
type(type),anonymous(true),is_trie(false)
  {
    switch (this->type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(inumber, onumber);
	break;
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(inumber, onumber);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(inumber, onumber);
	break;
      case FOMA_TYPE:
	{
	  char ibuf [255];
	  sprintf(ibuf, "\\%i", inumber);
	  char obuf [255];
	  sprintf(obuf, "\\%i", onumber);
	  implementation.foma = foma_interface.define_transducer( ibuf, obuf );
	  break;
	}
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }


HfstTransducer::HfstTransducer(const std::string &symbol, ImplementationType type): 
type(type),anonymous(false),is_trie(false)
  {
    switch (this->type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(symbol.c_str());
	break;
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(symbol);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(symbol);
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer(strdup(symbol.c_str()));
	// should the char* be deleted?
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }

HfstTransducer::HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type):
type(type),anonymous(false),is_trie(false)
  {
    switch (this->type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(isymbol.c_str(), osymbol.c_str());
	break;
      case TROPICAL_OFST_TYPE:
      case UNSPECIFIED_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(isymbol, osymbol);
	this->type = TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(isymbol, osymbol);
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer( strdup(isymbol.c_str()), strdup(osymbol.c_str()) );
	// should the char*:s be deleted?
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }


  ImplementationType HfstTransducer::get_type(void) const {
    switch (this->type)
      {
      case SFST_TYPE:
	return SFST_TYPE;
	break;
      case TROPICAL_OFST_TYPE:
	return TROPICAL_OFST_TYPE;
	break;
      case LOG_OFST_TYPE:
	return LOG_OFST_TYPE;
	break;
      case FOMA_TYPE:
	return FOMA_TYPE;
	break;
      case UNSPECIFIED_TYPE:
	return UNSPECIFIED_TYPE;
	break;
      case ERROR_TYPE:
	return ERROR_TYPE;
	break;
      default:
	throw hfst::exceptions::TransducerHasUnknownTypeException();
	break;
      }
  }

void HfstTransducer::test_minimize()
{
  this->tropical_ofst_interface.test_minimize();
}

  bool HfstTransducer::are_equivalent(const HfstTransducer &one, const HfstTransducer &another) 
  {
    if (one.type != another.type)
      throw hfst::exceptions::TransducerTypeMismatchException ();
    
    HfstTransducer one_copy(one);
    HfstTransducer another_copy(another);
    one_copy.convert(another_copy.type);
    one_copy.harmonize(another_copy);
    one_copy.minimize();
    another_copy.minimize();

    //std::cerr << one_copy << "--\n" << another_copy;

    switch (one_copy.type)
      {
	// Does not work if both transducers are empty?
	/*case SFST_TYPE:
	return one_copy.sfst_interface.are_equivalent(one_copy.implementation.sfst, another_copy.implementation.sfst);
	break;*/
      case SFST_TYPE:
	one_copy.convert(TROPICAL_OFST_TYPE);
	another_copy.convert(TROPICAL_OFST_TYPE);
	return one_copy.tropical_ofst_interface.are_equivalent(one_copy.implementation.tropical_ofst, another_copy.implementation.tropical_ofst);
	break;
      case TROPICAL_OFST_TYPE:
	return one_copy.tropical_ofst_interface.are_equivalent(one_copy.implementation.tropical_ofst, another_copy.implementation.tropical_ofst);
	break;
      case LOG_OFST_TYPE:
	return one_copy.log_ofst_interface.are_equivalent(one_copy.implementation.log_ofst, another_copy.implementation.log_ofst);
	break;
      case FOMA_TYPE:
	return one_copy.foma_interface.are_equivalent(one_copy.implementation.foma, another_copy.implementation.foma);
	break;
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
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

  void HfstTransducer::extract_strings(WeightedPaths<float>::Set &results)
  {
    switch (this->type)
      {
      case LOG_OFST_TYPE:
	hfst::implementations::LogWeightTransducer::extract_strings
	  (implementation.log_ofst,results);
	break;
      case TROPICAL_OFST_TYPE:
	hfst::implementations::TropicalWeightTransducer::extract_strings
	  (implementation.tropical_ofst,results);
	break;
      case SFST_TYPE:
	hfst::implementations::SfstTransducer::extract_strings(implementation.sfst, results);
	break;
      case FOMA_TYPE:
	{
	  HfstTransducer tc = convert(*this, FOMA_TYPE);
	  hfst::implementations::TropicalWeightTransducer::extract_strings
	    (tc.implementation.tropical_ofst,results);
	  break;
	}
      default:
	throw hfst::exceptions::FunctionNotImplementedException(); 
	break;
      }
  }

  HfstTransducer &HfstTransducer::insert_freely(const StringPair &symbol_pair)
  {
    ImplementationType original_type = this->type;
    if (original_type == FOMA_TYPE)
      this->convert(TROPICAL_OFST_TYPE);

    switch (this->type)    
      {
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
      case SFST_TYPE:
	{
	  hfst::implementations::Transducer * temp =
	    hfst::implementations::SfstTransducer::insert_freely
	    (implementation.sfst,symbol_pair);
	  delete implementation.sfst;
	  implementation.sfst = temp;
	  break;
	}
      default:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;       
      }
    this->convert(original_type);
    return *this;
  }



  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol)
  {
    ImplementationType original_type = this->type;
    this->convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol,new_symbol);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    this->convert(original_type);
    return *this;
  }


  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPair &new_symbol_pair)
  { 
    ImplementationType original_type = this->type;
    this->convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol_pair,new_symbol_pair);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    this->convert(original_type);
    return *this;
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPairSet &new_symbol_pair_set)
  { 
    ImplementationType original_type = this->type;
    this->convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol_pair,new_symbol_pair_set);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    this->convert(original_type);
    return *this;
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &symbol_pair,
   HfstTransducer &transducer)
  { 
    if (this->type != transducer.type)
      throw hfst::exceptions::TransducerTypeMismatchException ();
   
    ImplementationType original_type = this->type;
    this->convert(TROPICAL_OFST_TYPE);
    transducer.convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,symbol_pair,transducer.implementation.tropical_ofst);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    this->convert(original_type);
    transducer.convert(original_type);
    return *this;
  }

  HfstTransducer &HfstTransducer::set_final_weights(float weight)
  {
    if (this->type == TROPICAL_OFST_TYPE)
      implementation.tropical_ofst  =
	this->tropical_ofst_interface.set_final_weights(this->implementation.tropical_ofst, weight);
    if (this->type == LOG_OFST_TYPE)
      implementation.log_ofst  =
	this->log_ofst_interface.set_final_weights(this->implementation.log_ofst, weight);
    return *this;
  }

  HfstTransducer &HfstTransducer::transform_weights(float (*func)(float))
  {
    if (this->type == TROPICAL_OFST_TYPE)
      implementation.tropical_ofst  =
	this->tropical_ofst_interface.transform_weights(this->implementation.tropical_ofst, func);
    if (this->type == LOG_OFST_TYPE)
      implementation.log_ofst  =
	this->log_ofst_interface.transform_weights(this->implementation.log_ofst, func);
    return *this;
  }

  HfstTransducer &HfstTransducer::compose
  (const HfstTransducer &another)
  { is_trie = false;
    return apply(&hfst::implementations::SfstTransducer::compose,
		 &hfst::implementations::TropicalWeightTransducer::compose,
		 &hfst::implementations::LogWeightTransducer::compose,
		 &hfst::implementations::FomaTransducer::compose,
		 const_cast<HfstTransducer&>(another)); }

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
      case LOG_OFST_TYPE:
	hfst::implementations::LogWeightTransducer::disjunct_as_tries
	  (*implementation.log_ofst,another.implementation.log_ofst);
	break;
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
    ImplementationType original_type = this->type;
    if (original_type == SFST_TYPE || original_type == FOMA_TYPE)
      this->convert(TROPICAL_OFST_TYPE);

    switch (this->type)
      {
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::n_best
	    (implementation.tropical_ofst,(int)n);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  return *this;
	  break;
      }
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::n_best
	    (implementation.log_ofst,(int)n);
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  return *this;
	  break;
	}
      default:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;       
      }
    this->convert(original_type);
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
	  case FOMA_TYPE:
	    internal =
	      hfst::implementations::foma_to_internal_format(t.implementation.foma);
	    break;
	  case SFST_TYPE:
	    internal = 
	      hfst::implementations::sfst_to_internal_format(t.implementation.sfst);
	    break;
	  case TROPICAL_OFST_TYPE:
	    internal = t.implementation.tropical_ofst;  // just copy the pointer
	    break;
	  case LOG_OFST_TYPE:
	  internal =
	      hfst::implementations::log_ofst_to_internal_format(t.implementation.log_ofst);
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
	  case SFST_TYPE:
	    delete retval->implementation.sfst;  // delete the empty transducer
	    retval->implementation.sfst = 
	      hfst::implementations::internal_format_to_sfst(internal);
	    if (t.type != TROPICAL_OFST_TYPE)  // if a new intermediate transducer was created,
	      delete internal;                  // delete it
	    break;
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
	  case FOMA_TYPE:
	    retval->foma_interface.delete_foma(retval->implementation.foma);
	    retval->implementation.foma =
	      hfst::implementations::internal_format_to_foma(internal);
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

  HfstTransducer &HfstTransducer::convert(ImplementationType type)
  {
    if (type == UNSPECIFIED_TYPE)
      { throw hfst::implementations::SpecifiedTypeRequiredException(); }
    if (type == this->type)
      { return *this; }

    try 
      {
	hfst::implementations::InternalTransducer * internal;
	switch (this->type)
	  {
	  case FOMA_TYPE:
	    internal =
	      hfst::implementations::foma_to_internal_format(implementation.foma);
	    foma_interface.delete_foma(implementation.foma);
	    break;
	  case SFST_TYPE:
	    internal = 
	      hfst::implementations::sfst_to_internal_format(implementation.sfst);
	    delete implementation.sfst;
	    break;
	  case TROPICAL_OFST_TYPE:
	    internal = implementation.tropical_ofst;
	    break;
	  case LOG_OFST_TYPE:
	  internal =
	      hfst::implementations::log_ofst_to_internal_format(implementation.log_ofst);
	  delete implementation.log_ofst;
	    break;
	case ERROR_TYPE:
	case UNSPECIFIED_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }

	this->type = type;
	switch (this->type)
	  {
	  case SFST_TYPE:
	    implementation.sfst = 
	      hfst::implementations::internal_format_to_sfst(internal);
	    delete internal;
	    break;
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
	  case FOMA_TYPE:
	    implementation.foma =
	      hfst::implementations::internal_format_to_foma(internal);
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

void HfstTransducer::write_in_att_format(const char * filename)
{
  FILE * ofile = fopen(filename, "wb");
  if (ofile == NULL)
    throw hfst::exceptions::FileNotReadableException();
  HfstTransducer conv = convert(*this, TROPICAL_OFST_TYPE);
  if (this->anonymous)
    this->tropical_ofst_interface.write_in_att_format_number
      (conv.implementation.tropical_ofst, ofile);
  else
    this->tropical_ofst_interface.write_in_att_format
      (conv.implementation.tropical_ofst, ofile);
  fclose(ofile);
}

void HfstTransducer::write_in_att_format(FILE * ofile)
{
  HfstTransducer conv = convert(*this, TROPICAL_OFST_TYPE);
  if (this->anonymous)
    this->tropical_ofst_interface.write_in_att_format_number
      (conv.implementation.tropical_ofst, ofile);
  else
    this->tropical_ofst_interface.write_in_att_format
      (conv.implementation.tropical_ofst, ofile);
}

HfstTransducer &HfstTransducer::read_in_att_format(const char * filename, ImplementationType type)
{
  if (type == ERROR_TYPE) {
    throw hfst::exceptions::TransducerHasWrongTypeException();
  }
  HfstTransducer * retval = new HfstTransducer(TROPICAL_OFST_TYPE);
  FILE * ifile = fopen(filename, "rb");
  if (ifile == NULL)
    throw hfst::exceptions::FileNotReadableException();
  retval->implementation.tropical_ofst = TropicalWeightTransducer::read_in_att_format(ifile);
  fclose(ifile);
  retval->convert(type);
  return *retval;
}

HfstTransducer &HfstTransducer::read_in_att_format(FILE * ifile, ImplementationType type)
{
  HfstTransducer * retval = new HfstTransducer(TROPICAL_OFST_TYPE);
  retval->implementation.tropical_ofst = TropicalWeightTransducer::read_in_att_format(ifile);
  retval->convert(type);
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

std::ostream &operator<<(std::ostream &out,HfstTransducer &t)
  {
    HfstTransducer tc(t);
    tc.convert(TROPICAL_OFST_TYPE);
    if (t.anonymous)
      tc.tropical_ofst_interface.write_in_att_format_number(tc.implementation.tropical_ofst, out);
    else
      tc.tropical_ofst_interface.write_in_att_format(tc.implementation.tropical_ofst, out);
    return out;
  }


  HfstMutableTransducer::HfstMutableTransducer(void):
    transducer(HfstTransducer(TROPICAL_OFST_TYPE))
  {
  }

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
