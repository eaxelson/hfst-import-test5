#include "HfstTransducer.h"

hfst::symbols::GlobalSymbolTable hfst::symbols::KeyTable::global_symbol_table;

namespace hfst
{
  hfst::implementations::SfstTransducer HfstTransducer::sfst_interface;
  hfst::implementations::TropicalWeightTransducer 
  HfstTransducer::tropical_ofst_interface;
  hfst::implementations::LogWeightTransducer
  HfstTransducer::log_ofst_interface;

  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    hfst::symbols::KeyMap key_map;
    this->key_table.harmonize(key_map,another.key_table);
    key_table = another.key_table;
    switch (type)
      {
      case SFST_TYPE:
	{
	  /* SFST deletes here by default, so we don't need to delete. */
	  this->implementation.sfst = 
	    sfst_interface.harmonize(implementation.sfst,key_map);
	  break;
	}
      case TROPICAL_OFST_TYPE:
	{
	  fst::StdVectorFst * temp =
	    tropical_ofst_interface.harmonize
	    (implementation.tropical_ofst,key_map);
	  delete this->implementation.tropical_ofst;
	  this->implementation.tropical_ofst = temp;
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
  }

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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

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

  HfstTransducer::HfstTransducer(const std::string& utf8_str, 
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    KeyPairVector * kpv = 
      multichar_symbol_tokenizer.tokenize(utf8_str,key_table);
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete kpv;
  }

  HfstTransducer::HfstTransducer(const std::string& upper_utf8_str,
				 const std::string& lower_utf8_str,
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false),is_trie(true)
  {
    KeyPairVector * kpv = 
      multichar_symbol_tokenizer.tokenize
      (upper_utf8_str,lower_utf8_str,key_table);
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
    delete kpv;
  }

  HfstTransducer::HfstTransducer(HfstInputStream &in):
    type(in.type), anonymous(false),is_trie(false)
  { in.read_transducer(*this); }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),
    key_table(another.key_table),is_trie(another.is_trie)
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
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
	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
      }
  }

  HfstTransducer &HfstTransducer::remove_epsilons(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::remove_epsilons,
       &hfst::implementations::TropicalWeightTransducer::remove_epsilons,
       &hfst::implementations::LogWeightTransducer::remove_epsilons,
       type); }

  HfstTransducer &HfstTransducer::determinize(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::determinize,
       &hfst::implementations::TropicalWeightTransducer::determinize,
       &hfst::implementations::LogWeightTransducer::determinize,
       type); }

  HfstTransducer &HfstTransducer::minimize(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::minimize,
       &hfst::implementations::TropicalWeightTransducer::minimize,
       &hfst::implementations::LogWeightTransducer::minimize,
       type); }

  HfstTransducer &HfstTransducer::repeat_star(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_star,
       &hfst::implementations::TropicalWeightTransducer::repeat_star,
       &hfst::implementations::LogWeightTransducer::repeat_star,
       type); }

  HfstTransducer &HfstTransducer::repeat_plus(ImplementationType type)
  { is_trie = false;
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_plus,
       &hfst::implementations::TropicalWeightTransducer::repeat_plus,
       &hfst::implementations::LogWeightTransducer::repeat_plus,
       type); }

  HfstTransducer &HfstTransducer::repeat_n(unsigned int n, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_n,
       &hfst::implementations::LogWeightTransducer::repeat_n,
       n,type); }

  HfstTransducer &HfstTransducer::repeat_n_plus(unsigned int n, ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::repeat_le_n,
       &hfst::implementations::TropicalWeightTransducer::repeat_le_n,
       &hfst::implementations::LogWeightTransducer::repeat_le_n,
       n,type); }

  HfstTransducer &HfstTransducer::optionalize(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::optionalize,
       &hfst::implementations::TropicalWeightTransducer::optionalize,
       &hfst::implementations::LogWeightTransducer::optionalize,
       type); }

  HfstTransducer &HfstTransducer::input_project(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_input_language,
       &hfst::implementations::TropicalWeightTransducer::extract_input_language,
       &hfst::implementations::LogWeightTransducer::extract_input_language,
       type); }

  HfstTransducer &HfstTransducer::output_project(ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply 
      (&hfst::implementations::SfstTransducer::extract_output_language,
       &hfst::implementations::TropicalWeightTransducer::extract_output_language,
       &hfst::implementations::LogWeightTransducer::extract_output_language,
       type); }

  HfstTransducer &HfstTransducer::substitute
  (Key old_key,Key new_key)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       old_key,new_key,
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       key_table.add_symbol(old_symbol),key_table.add_symbol(new_symbol),
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const StringSymbolPair &old_symbol_pair, 
   const StringSymbolPair &new_symbol_pair)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       key_table.get_key_pair(old_symbol_pair),
       key_table.get_key_pair(new_symbol_pair),
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const KeyPair &old_key_pair,const KeyPair &new_key_pair)
  { return apply 
      (&hfst::implementations::SfstTransducer::substitute,
       &hfst::implementations::TropicalWeightTransducer::substitute,
       &hfst::implementations::LogWeightTransducer::substitute,
       old_key_pair,new_key_pair,
       this->type); }

  HfstTransducer &HfstTransducer::compose
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false;
    harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::compose,
		 &hfst::implementations::TropicalWeightTransducer::compose,
		 &hfst::implementations::LogWeightTransducer::compose,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::concatenate
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::concatenate,
		 &hfst::implementations::TropicalWeightTransducer::concatenate,
		 &hfst::implementations::LogWeightTransducer::concatenate,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::disjunct
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::disjunct,
		 &hfst::implementations::TropicalWeightTransducer::disjunct,
		 &hfst::implementations::LogWeightTransducer::disjunct,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::intersect
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::intersect,
		 &hfst::implementations::TropicalWeightTransducer::intersect,
		 &hfst::implementations::LogWeightTransducer::intersect,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::subtract
  (HfstTransducer &another,
   ImplementationType type)
  { is_trie = false; // This could be done so that is_trie is preserved
    harmonize(another);
    return apply(&hfst::implementations::SfstTransducer::subtract,
		 &hfst::implementations::TropicalWeightTransducer::subtract,
		 &hfst::implementations::LogWeightTransducer::subtract,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::anonymize(void)
  { anonymous = true; return *this; }
  
  KeyTable& HfstTransducer::get_key_table(void)
  { return key_table; }

  ImplementationType HfstTransducer::get_type(void)
  { return this->type; }

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
      { throw hfst::implementations::WeightTypeMismatchException(); }
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
	switch (type)
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

  std::ostream &operator<<(std::ostream &out,HfstTransducer &t)
  {
    switch (t.type)
      {
      case SFST_TYPE:
	t.sfst_interface.print(t.implementation.sfst,t.key_table,out);
	break;
      case TROPICAL_OFST_TYPE:
	t.tropical_ofst_interface.print
	  (t.implementation.tropical_ofst,t.key_table,out);
	break;
      case LOG_OFST_TYPE:
	t.log_ofst_interface.print
	  (t.implementation.log_ofst,t.key_table,out);
	break;
 	case UNSPECIFIED_TYPE:
	case ERROR_TYPE:
	default:
	  throw hfst::exceptions::TransducerHasWrongTypeException();
     }
    return out;
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
