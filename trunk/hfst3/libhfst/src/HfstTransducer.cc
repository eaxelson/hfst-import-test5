#include "HfstTransducer.h"

//hfst::symbols::GlobalSymbolTable hfst::symbols::KeyTable::global_symbol_table;

namespace hfst
{
  hfst::implementations::SfstTransducer HfstTransducer::sfst_interface;
  hfst::implementations::TropicalWeightTransducer 
  HfstTransducer::tropical_ofst_interface;
  hfst::implementations::LogWeightTransducer
  HfstTransducer::log_ofst_interface;
  hfst::implementations::FomaTransducer HfstTransducer::foma_interface;


  /*
  HfstState::HfstState(MutableHfstTransducer &t) 
  {
    this->tropical_ofst = t.tropical_ofst;
  }

  HfstState::HfstState(const HfstState &s) { }
  HfstWeight HfstState::get_final_weight(void) const  { return false; }
  bool HfstState::operator< (const HfstState &another) const  { return false; }
  bool HfstState::operator== (const HfstState &another) const  { return false; }
  bool HfstState::operator!= (const HfstState &another) const  { return false; }
  const_iterator HfstState::begin(void) const  { return HfstTransitionIterator(HfstTransducer(UNSPECIFIED_TYPE)); }
  const_iterator HfstState::end(void) const  { return HfstTransitionIterator(HfstTransducer(UNSPECIFIED_TYPE)); }
  */


#ifdef foo
  /* The set of symbols in the symbol table of the transducer. */
  StringSymbolSet HfstTransducer::get_string_symbol_set(void) 
  {
    switch (this->type)
      {
      case (SFST_TYPE):
	return sfst_interface.get_string_symbol_set(implementation.sfst);
	break;
      case (TROPICAL_OFST_TYPE):
	return tropical_ofst_interface.get_string_symbol_set(implementation.tropical_ofst);
	break;
      default:
	throw hfst::exceptions::FunctionNotImplementedException();	
      }
  }

  /* 
     Add symbols from \a unknown_another to the symbol table of transducer \a another.
     Then calculate how numbers in this transducer should be mapped if this transducer
     follows the symbol table of transducer \a another. 
     Delete the symbol table of this transducer and set as its value
     a copy of the symbol table of transducer another.
     Finally, return the calculated number mappings in a KeyMap. 
     This function is inteded to be used with function 'harmonize' in the following way: 

       KeyMap km = transducer.create_mapping(another_transducer, unknown_another); 
       transducer = transducer.harmonize(km);  
  */

  KeyMap HfstTransducer::create_mapping(HfstTransducer &another, StringSymbolSet &unknown_another)
  {
    if (this->type != another.type) 
      {
      throw hfst::exceptions::ErrorException();
      }

    switch (this->type)
      {
      case (SFST_TYPE):
	return sfst_interface.create_mapping(this->implementation.sfst,
					     another.implementation.sfst,
					     unknown_another);
	break;
      case (TROPICAL_OFST_TYPE):
	return tropical_ofst_interface.create_mapping(this->implementation.tropical_ofst,
						      another.implementation.tropical_ofst,
						      unknown_another);
	break;
      default:
	throw hfst::exceptions::FunctionNotImplementedException();	
      }
  }
#endif
  
  HfstTransitionIterator::HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s):
    tropical_ofst_iterator(hfst::implementations::TropicalWeightTransitionIterator(t.transducer.implementation.tropical_ofst, s))
  {}

  HfstTransitionIterator::~HfstTransitionIterator(void)  
  {}

  bool HfstTransitionIterator::done()
  {
    return tropical_ofst_iterator.done();
  }
  
  HfstTransition HfstTransitionIterator::value()
  {
    hfst::implementations::TropicalWeightTransition twt = tropical_ofst_iterator.value();
    HfstWeight weight = twt.get_weight().Value();
    HfstState target_state = twt.get_target_state();
    return HfstTransition( twt.get_input_symbol(),
			   twt.get_output_symbol(),
			   weight,
			   target_state );
  }
  
  void HfstTransitionIterator::next()
  {
    tropical_ofst_iterator.next();
  }

  
  HfstStateIterator::HfstStateIterator(const HfstMutableTransducer &t):
    tropical_ofst_iterator(hfst::implementations::TropicalWeightStateIterator(t.transducer.implementation.tropical_ofst))
  {}

  //HfstStateIterator::HfstStateIterator(void)  {}
  
  HfstStateIterator::~HfstStateIterator(void)  
  { 
  }
  
  /*
  void HfstStateIterator::operator= (const HfstStateIterator &another)  {}
  
  bool HfstStateIterator::operator== (const HfstStateIterator &another) const  { return false; }
  
  bool HfstStateIterator::operator!= (const HfstStateIterator &another) const  { return false; }
  
  const HfstState HfstStateIterator::operator* (void)  { return HfstState(HfstTransducer(UNSPECIFIED_TYPE)); }
  
  void HfstStateIterator::operator++ (void)  {}
  
  void HfstStateIterator::operator++ (int)  {}
  
  HfstWeight get_final_weight(HfstState s) {}*/

  bool HfstStateIterator::done()
  {
    return tropical_ofst_iterator.done();
  }

  HfstState HfstStateIterator::value()
  {
    return tropical_ofst_iterator.value();
  }

  void HfstStateIterator::next()
  {
    tropical_ofst_iterator.next();
  }



  HfstTransition::HfstTransition(std::string isymbol, std::string osymbol, HfstWeight weight, HfstState target_state):
    isymbol(isymbol), osymbol(osymbol), weight(weight), target_state(target_state)
  {}

  HfstTransition::~HfstTransition(void)
  {}

  std::string HfstTransition::get_input_symbol(void)
  {
    return isymbol;
  }

  std::string HfstTransition::get_output_symbol(void)
  {
    return osymbol;
  }
  
  HfstWeight HfstTransition::get_weight(void)
  {
    return weight;
  }
  
  HfstState HfstTransition::get_target_state(void)
  {
    return target_state;
  }


  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    if (this->type != another.type)
      {
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }

    //fprintf(stderr, "harmonize(1): this type is %i\n", this->type);
    //fprintf(stderr, "harmonize(1): another type is %i\n", another.type);

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

#ifdef foo
  {
    if (this->anonymous or another.anonymous)
      { return; }

    // 1. Calculate the set of unknown symbols for transducers this and another.

    StringSymbolSet unknown_this;    // symbols known to another but not this
    StringSymbolSet unknown_another; // and vice versa
    StringSymbolSet this_symbols = this->get_string_symbol_set();
    StringSymbolSet another_symbols = another.get_string_symbol_set();
    KeyTable::collect_unknown_sets(this_symbols, unknown_this,
				   another_symbols, unknown_another);

    // 2. Add new symbols from transducer this to the symbol table of transducer
    //    another and recode the symbol numbers of transducer this so that
    //    it follows the symbol table of transducer another.

    KeyMap km = this->create_mapping(another, unknown_another);
    this->recode_symbol_numbers(km);

    // 3. Calculate the set of symbol pairs to which a non-identity "?:?"
    //    transition is expanded for both transducers.
    
    return;
  }


    /*
    hfst::symbols::SymbolPairSet non_identity_this = 
      KeyTable::non_identity_cross_product(unknown_this, us);
    hfst::symbols::SymbolPairSet non_identity_another = 
      KeyTable::non_identity_cross_product(unknown_another, us);


    hfst::symbols::KeyMap key_map;
    this->key_table.harmonize(key_map,another.key_table);
    key_table = another.key_table;
    switch (type)
      {
      case SFST_TYPE:
      {*/
	  /* SFST deletes here by default, so we don't need to delete. */
    /* this->implementation.sfst = 
	    sfst_interface.harmonize(implementation.sfst,key_map);
	  break;
	}
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    tropical_ofst_interface.harmonize
	    (implementation.tropical_ofst,key_map);
	  delete this->implementation.tropical_ofst;
	  this->implementation.tropical_ofst = temp;*/
	  // ADDED
	  /*tropical_ofst_interface.expand_unknown(this->implementation.tropical_ofst,
						 unknown_this, non_identity_this);
	  tropical_ofst_interface.expand_unknown(another.implementation.tropical_ofst,
	  unknown_another, non_identity_another);*/
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    log_ofst_interface.harmonize
	    (implementation.log_ofst,key_map);
	  delete this->implementation.log_ofst;
	  this->implementation.log_ofst  = temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
#endif



  HfstTransducer::HfstTransducer(ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.create_empty_transducer();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.create_empty_transducer();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.create_empty_transducer();
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.create_empty_transducer();
	break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

/*
  HfstTransducer::HfstTransducer
  (const KeyTable &key_table,ImplementationType type):
    type(type),anonymous(false),key_table(key_table),is_trie(true)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.create_empty_transducer();
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.create_empty_transducer();
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.create_empty_transducer();
	break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }
*/

  HfstTransducer::HfstTransducer(const std::string& utf8_str, 
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    StringPairVector * spv = 
      multichar_symbol_tokenizer.tokenize(utf8_str); //,key_table);
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*spv);
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(*spv);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(*spv);
	break;
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
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
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(*spv);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(*spv);
	break;
      case FOMA_TYPE:
	implementation.foma =
	  foma_interface.define_transducer(*spv);
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete spv;
  }

/*  HfstTransducer::HfstTransducer(KeyPairVector * kpv,
				 ImplementationType type):
    type(type),anonymous(true),is_trie(true)
  {
    switch (type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(*kpv);
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = 
	  tropical_ofst_interface.define_transducer(*kpv);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = 
	  log_ofst_interface.define_transducer(*kpv);
	break;
      default:
	assert(false);
      }
      }*/

  HfstTransducer::HfstTransducer(HfstInputStream &in):
    type(in.type), anonymous(false),is_trie(false)
  { in.read_transducer(*this); }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),is_trie(another.is_trie)
    //, key_table(another.key_table)
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
    is_trie(another.transducer.is_trie) // key_table(another.transducer.key_table) 
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

  // for testing
HfstTransducer::HfstTransducer(const std::string &symbol, ImplementationType type): 
type(type),anonymous(false),is_trie(false)
  {
    switch (this->type)
      {
      case SFST_TYPE:
	implementation.sfst = sfst_interface.define_transducer(symbol.c_str());
	break;
      case TROPICAL_OFST_TYPE:
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(symbol);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(symbol);
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer(strdup(symbol.c_str()));
	// should the char* be deleted?
	break;
      case UNSPECIFIED_TYPE:
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
	implementation.tropical_ofst = tropical_ofst_interface.define_transducer(isymbol, osymbol);
	break;
      case LOG_OFST_TYPE:
	implementation.log_ofst = log_ofst_interface.define_transducer(isymbol, osymbol);
	break;
      case FOMA_TYPE:
	implementation.foma = foma_interface.define_transducer( strdup(isymbol.c_str()), strdup(osymbol.c_str()) );
	// should the char*:s be deleted?
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
	break;
      }
  }


  ImplementationType HfstTransducer::get_type(void) {
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

  bool HfstTransducer::test_equivalence(HfstTransducer &one, HfstTransducer &another) 
  {
    HfstTransducer onecopy(one);
    HfstTransducer anothercopy(another);
    onecopy.convert(TROPICAL_OFST_TYPE);
    anothercopy.convert(TROPICAL_OFST_TYPE);
    onecopy.harmonize(anothercopy);
    //std::cerr << onecopy << "--\n" << anothercopy << "\n\n";
    return tropical_ofst_interface.test_equivalence(
	     onecopy.implementation.tropical_ofst,
	     anothercopy.implementation.tropical_ofst);
  }

  HfstTransducer &HfstTransducer::remove_epsilons(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::remove_epsilons,
       &hfst::implementations::TropicalWeightTransducer::remove_epsilons,
       &hfst::implementations::LogWeightTransducer::remove_epsilons,
       &hfst::implementations::FomaTransducer::remove_epsilons,
       type); }

  HfstTransducer &HfstTransducer::determinize(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::determinize,
       &hfst::implementations::TropicalWeightTransducer::determinize,
       &hfst::implementations::LogWeightTransducer::determinize,
       &hfst::implementations::FomaTransducer::determinize,
       type); }

  HfstTransducer &HfstTransducer::minimize(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::minimize,
       &hfst::implementations::TropicalWeightTransducer::minimize,
       &hfst::implementations::LogWeightTransducer::minimize,
       &hfst::implementations::FomaTransducer::minimize,
       type); }

  HfstTransducer &HfstTransducer::repeat_star(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_star,
       &hfst::implementations::TropicalWeightTransducer::repeat_star,
       &hfst::implementations::LogWeightTransducer::repeat_star,
       &hfst::implementations::FomaTransducer::repeat_star,
       type); }

  HfstTransducer &HfstTransducer::repeat_plus(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_plus,
       &hfst::implementations::TropicalWeightTransducer::repeat_plus,
       &hfst::implementations::LogWeightTransducer::repeat_plus,
       &hfst::implementations::FomaTransducer::repeat_plus,
       type); }

  HfstTransducer &HfstTransducer::repeat_n(unsigned int n, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_n,
       &hfst::implementations::LogWeightTransducer::repeat_n,
       &hfst::implementations::FomaTransducer::repeat_n,
       n,type); }

  HfstTransducer &HfstTransducer::repeat_n_plus(unsigned int n, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer a(*this);
    return (a.repeat_n(n,type)).concatenate(this->repeat_star(type));  // FIX: memory leaks?
  }

  HfstTransducer &HfstTransducer::repeat_n_minus(unsigned int n, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_le_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_le_n,
       &hfst::implementations::LogWeightTransducer::repeat_le_n,
       &hfst::implementations::FomaTransducer::repeat_le_n,
       n,type); }

  HfstTransducer &HfstTransducer::repeat_n_to_k(unsigned int n, unsigned int k, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer a(*this);
    return (a.repeat_n(n,type).concatenate(this->repeat_n_minus(k)));   // FIX: memory leaks? 
  }

  HfstTransducer &HfstTransducer::optionalize(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::optionalize,
       &hfst::implementations::TropicalWeightTransducer::optionalize,
       &hfst::implementations::LogWeightTransducer::optionalize,
       &hfst::implementations::FomaTransducer::optionalize,
       type); }

  HfstTransducer &HfstTransducer::invert(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::invert,
       &hfst::implementations::TropicalWeightTransducer::invert,
       &hfst::implementations::LogWeightTransducer::invert,
       &hfst::implementations::FomaTransducer::invert,
       type); }

  HfstTransducer &HfstTransducer::reverse(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::reverse,
       &hfst::implementations::TropicalWeightTransducer::reverse,
       &hfst::implementations::LogWeightTransducer::reverse,
       &hfst::implementations::FomaTransducer::reverse,
       type); }

  HfstTransducer &HfstTransducer::input_project(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_input_language,
       &hfst::implementations::TropicalWeightTransducer::extract_input_language,
       &hfst::implementations::LogWeightTransducer::extract_input_language,
       &hfst::implementations::FomaTransducer::extract_input_language,
       type); }

  HfstTransducer &HfstTransducer::output_project(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_output_language,
       &hfst::implementations::TropicalWeightTransducer::extract_output_language,
       &hfst::implementations::LogWeightTransducer::extract_output_language,
       &hfst::implementations::FomaTransducer::extract_output_language,
       type); }

void HfstTransducer::extract_strings(WeightedPaths<float>::Set &results)
  {
    switch (type)
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
      default:
	throw hfst::exceptions::FunctionNotImplementedException(); 
      }
  }

/*  HfstTransducer &HfstTransducer::substitute
  (Key old_key,Key new_key)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       &hfst::implementations::FomaTransducer::substitute,
       old_key,new_key,
       this->type); }*/

  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol, ImplementationType type)
  {
    (void)type;
    convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol,new_symbol);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    return *this;
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPair &new_symbol_pair)
  { 
    convert(TROPICAL_OFST_TYPE);
    fst::StdVectorFst * tropical_ofst_temp =
      this->tropical_ofst_interface.substitute(implementation.tropical_ofst,old_symbol_pair,new_symbol_pair);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    return *this;
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &symbol_pair,
   HfstTransducer &transducer)
  {    
    convert(TROPICAL_OFST_TYPE);
    transducer.convert(TROPICAL_OFST_TYPE);
    this->harmonize(transducer);
    implementation.tropical_ofst  =
      this->tropical_ofst_interface.substitute(this->implementation.tropical_ofst,symbol_pair,transducer.implementation.tropical_ofst);
    return *this;
  }

  HfstTransducer &HfstTransducer::set_final_weights(float weight)
  {
    convert(TROPICAL_OFST_TYPE);
    implementation.tropical_ofst  =
      this->tropical_ofst_interface.set_final_weights(this->implementation.tropical_ofst, weight);
    return *this;
  }

  HfstTransducer &HfstTransducer::transform_weights(float (*func)(float))
  {
    convert(TROPICAL_OFST_TYPE);
    implementation.tropical_ofst  =
      this->tropical_ofst_interface.transform_weights(this->implementation.tropical_ofst, func);
    return *this;
  }

  /*  HfstTransducer &HfstTransducer::substitute
  (const KeyPair &old_key_pair,const KeyPair &new_key_pair)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       &hfst::implementations::FomaTransducer::substitute,
       old_key_pair,new_key_pair,
       this->type); }*/

  HfstTransducer &HfstTransducer::compose
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false;
    //harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::compose,
		 &hfst::implementations::TropicalWeightTransducer::compose,
		 &hfst::implementations::LogWeightTransducer::compose,
		 &hfst::implementations::FomaTransducer::compose,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::concatenate
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    //harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::concatenate,
		 &hfst::implementations::TropicalWeightTransducer::concatenate,
		 &hfst::implementations::LogWeightTransducer::concatenate,
		 &hfst::implementations::FomaTransducer::concatenate,
		 another,
		 type); }

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
  (int n,ImplementationType type)
  {
    if (type == SFST_TYPE || type == FOMA_TYPE)
      { convert(TROPICAL_OFST_TYPE); }
    else if (type != UNSPECIFIED_TYPE)
      { convert(type); }
    switch (this->type)
      {
      case SFST_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::n_best
	    (implementation.tropical_ofst,n);
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  return *this;
	  break;
      }
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::n_best
	    (implementation.log_ofst,n);
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  return *this;
	  break;
	}
      case FOMA_TYPE:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;
      default:
	throw hfst::exceptions::FunctionNotImplementedException();
	break;       
      }
  }

  HfstTransducer &HfstTransducer::disjunct
  (HfstTransducer &another,
   ImplementationType type)
  { //harmonize(another);
    /*if (is_trie and another.is_trie)
      {
	try
	  { disjunct_as_tries(another,type); 
	    return *this; }
	catch (hfst::exceptions::FunctionNotImplementedException e) {}
	catch (hfst::exceptions::HfstInterfaceException e) { throw e; }
      }*/
    is_trie = false;
    return apply(&hfst::implementations::SfstTransducer::disjunct,
		 &hfst::implementations::TropicalWeightTransducer::disjunct,
		 &hfst::implementations::LogWeightTransducer::disjunct,
		 &hfst::implementations::FomaTransducer::disjunct,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::intersect
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(&hfst::implementations::SfstTransducer::intersect,
		 &hfst::implementations::TropicalWeightTransducer::intersect,
		 &hfst::implementations::LogWeightTransducer::intersect,
		 &hfst::implementations::FomaTransducer::intersect,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::subtract
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(&hfst::implementations::SfstTransducer::subtract,
		 &hfst::implementations::TropicalWeightTransducer::subtract,
		 &hfst::implementations::LogWeightTransducer::subtract,
		 &hfst::implementations::FomaTransducer::subtract,
		 another,
		 type); }

/*  HfstTransducer &HfstTransducer::anonymize(void)
  { anonymous = true; return *this; }
  
  KeyTable& HfstTransducer::get_key_table(void)
  { return key_table; }

  void HfstTransducer::set_key_table(const KeyTable &kt)
  {
    key_table = kt;
    anonymous = false;
    }*/

/*
  WeightType HfstTransducer::get_weight_type(void)
  {
    switch (type)
      {
      case SFST_TYPE:
	return BOOL;
	break;
      case TROPICAL_OFST_TYPE:
	return FLOAT;
	break;
      case LOG_OFST_TYPE:
	return FLOAT;
	break;
      case FOMA_TYPE:
	return BOOL;
	break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  HfstTransducer &HfstTransducer::set_final_weights(float weight) 
  {
    switch (type)
      {
      case SFST_TYPE:
	return *this;
	break;
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::set_weight
	    (implementation.tropical_ofst,weight); 
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::set_weight
	    (implementation.log_ofst,weight); 
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  break;
	}
      case FOMA_TYPE:
	return *this;
	break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  template<> HfstTransducer &HfstTransducer::set_final_weight<float>
  (float weight)
  { 
    if (get_weight_type() != FLOAT)
      throw hfst::implementations::WeightTypeMismatchException(); 
    switch (type)
      {
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::set_weight
	    (implementation.tropical_ofst,weight); 
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::set_weight
	    (implementation.log_ofst,weight); 
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  break;
	}
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    return *this; 
  }

  HfstTransducer &HfstTransducer::transform_weights(float(*func)(float))
  {
    switch (type)
      {
      case SFST_TYPE:
	return *this;
	break;
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    hfst::implementations::TropicalWeightTransducer::transform_weights
	    (implementation.tropical_ofst,func); 
	  delete implementation.tropical_ofst;
	  implementation.tropical_ofst = temp;
	  break;
	}
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::transform_weights
	    (implementation.log_ofst,func); 
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  break;
	}
      case FOMA_TYPE:
	return *this;
	break;
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  template<> HfstTransducer &HfstTransducer::transform_weights<float>
  (float(*func)(float))
  { 
    if (get_weight_type() != FLOAT)
      { throw hfst::implementations::WeightTypeMismatchException(); }
    switch (type)
      {
      case TROPICAL_OFST_TYPE:
	{ throw hfst::implementations::FunctionNotImplementedException(); }
      case LOG_OFST_TYPE:
	{
	  hfst::implementations::LogFst * temp =
	    hfst::implementations::LogWeightTransducer::transform_weights
	    (implementation.log_ofst,func); 
	  delete implementation.log_ofst;
	  implementation.log_ofst = temp;
	  break;
	}
      default:
	assert(false);
      }
    return *this; 
  }
  
  template<> hfst::implementations::SfstTransducer::const_iterator 

  HfstTransducer::begin<SfstTransducer>(void)
  { 
    if (type != SFST_TYPE) 
      { throw hfst::implementations::TransducerHasWrongTypeException(); }
    return SfstTransducer::begin(implementation.sfst); 
  }

  template<> hfst::implementations::SfstTransducer::const_iterator 
  HfstTransducer::end<SfstTransducer>(void)
  { 
    if (type != SFST_TYPE) 
      { throw hfst::implementations::TransducerHasWrongTypeException(); }
    return SfstTransducer::end(implementation.sfst); 
  }
*/
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
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
	    implementation.tropical_ofst = internal;
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
	  }
      }
    catch (hfst::implementations::HfstInterfaceException e)
      { throw e; }
    return *this;
  }

/*  std::ostream &operator<<
(std::ostream &out,HfstTransducer &T)
  {
    HfstMutableTransducer t(T);
    HfstStateIterator it(t);
    while (not it.done()) {
      HfstState s = it.value();
      HfstTransitionIterator IT(t,s);
      while (not IT.done()) {
	HfstTransition tr = IT.value();
	out << s << "\t" << tr.get_target_state() << "\t"
	    << tr.get_input_symbol() << "\t" << tr.get_output_symbol()
	    << "\t" << tr.get_weight();
	out << "\n";
	IT.next();
      }
      if ( t.is_final(s) )
	cout << s << "\t" << t.get_final_weight(s) << "\n";
      it.next();
    }
    return out;
    }*/



void HfstTransducer::write_in_att_format(const char * filename)
{
  FILE * ofile = fopen(filename, "wb");
  if (ofile == NULL)
    throw FileNotReadableException();
  HfstTransducer conv = this->convert(TROPICAL_OFST_TYPE);
  this->tropical_ofst_interface.write_in_att_format
    (conv.implementation.tropical_ofst, ofile);
  fclose(ofile);
}

void HfstTransducer::write_in_att_format(FILE * ofile)
{
  HfstTransducer conv = this->convert(TROPICAL_OFST_TYPE);
  this->tropical_ofst_interface.write_in_att_format
    (conv.implementation.tropical_ofst, ofile);
}

HfstTransducer &HfstTransducer::read_in_att_format(const char * filename)
{
  HfstTransducer * retval = new HfstTransducer(TROPICAL_OFST_TYPE);
  FILE * ifile = fopen(filename, "rb");
  if (ifile == NULL)
    throw FileNotReadableException();
  retval->implementation.tropical_ofst = TropicalWeightTransducer::read_in_att_format(ifile);
  fclose(ifile);
  return *retval;
}

HfstTransducer &HfstTransducer::read_in_att_format(FILE * ifile)
{
  HfstTransducer * retval = new HfstTransducer(TROPICAL_OFST_TYPE);
  retval->implementation.tropical_ofst = TropicalWeightTransducer::read_in_att_format(ifile);
  return *retval;
}

/*void HfstTransducer::print(void) 
  {
    switch (this->type)
      {
      case FOMA_TYPE:
	this->foma_interface.print_test
	  (this->implementation.foma);
	break;
      case SFST_TYPE:
	this->sfst_interface.print_test
	  (this->implementation.sfst);
	break;
      case TROPICAL_OFST_TYPE:
	this->tropical_ofst_interface.print_test
	  (this->implementation.tropical_ofst);
	break;
      case LOG_OFST_TYPE:
	this->log_ofst_interface.print_test
	  (this->implementation.log_ofst);
	break;
      case UNSPECIFIED_TYPE:
      case ERROR_TYPE:
      default:
	throw hfst::exceptions::TransducerHasWrongTypeException();
     }
     }*/

std::ostream &operator<<(std::ostream &out,HfstTransducer &t)
  {
    HfstTransducer tc(t);
    tc.convert(TROPICAL_OFST_TYPE);
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

  /*HfstMutableTransducer::HfstMutableTransducer(const HfstMutableTransducer &t):
    transducer(HfstTransducer(t).convert(TROPICAL_OFST_TYPE))
  {
  }*/

  HfstMutableTransducer::~HfstMutableTransducer(void)
  {
  }

  HfstState HfstMutableTransducer::add_state() 
  {
    return this->transducer.tropical_ofst_interface.add_state(
      this->transducer.implementation.tropical_ofst);
  }

  void HfstMutableTransducer::set_final_weight(HfstState s, HfstWeight w)
  {
    this->transducer.tropical_ofst_interface.set_final_weight(
	    this->transducer.implementation.tropical_ofst, s, w);
  }

  void HfstMutableTransducer::add_transition(HfstState source, std::string isymbol, std::string osymbol, HfstWeight w, HfstState target)
  {
    return this->transducer.tropical_ofst_interface.add_transition(
             this->transducer.implementation.tropical_ofst,
	     source,
	     isymbol,
	     osymbol,
	     w,
	     target);
  }

  HfstWeight HfstMutableTransducer::get_final_weight(HfstState s)
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
