#include "HfstTransducer.h"

HFST_SYMBOLS::GlobalSymbolTable HFST_SYMBOLS::KeyTable::global_symbol_table;

namespace HFST
{
  HFST_IMPLEMENTATIONS::SfstTransducer HfstTransducer::sfst_interface;
  HFST_IMPLEMENTATIONS::TropicalWeightTransducer 
  HfstTransducer::tropical_ofst_interface;

  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    HFST_SYMBOLS::KeyMap key_map;
    this->key_table.harmonize(key_map,another.key_table);
    key_table = another.key_table;
    switch (type)
      {
      case SFST_TYPE:
	this->implementation.sfst = 
	  sfst_interface.harmonize(implementation.sfst,key_map);
	break;
      case TROPICAL_OFST_TYPE:
	this->implementation.tropical_ofst = 
	  tropical_ofst_interface.harmonize
	  (implementation.tropical_ofst,key_map);
	break;
      }
  }

  HfstTransducer::HfstTransducer(ImplementationType type):
  type(type),anonymous(false)
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
      }
  }

  HfstTransducer::HfstTransducer
  (const KeyTable &key_table,ImplementationType type):
    type(type),anonymous(false),key_table(key_table)
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
      }
  }

  HfstTransducer::HfstTransducer(const char * utf8_str, 
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
    type(type),anonymous(false)
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
      }
    delete kpv;
  }

  HfstTransducer::HfstTransducer(const char * upper_utf8_str,
				 const char * lower_utf8_str,
				 const HfstTokenizer 
				 &multichar_symbol_tokenizer,
				 ImplementationType type):
  type(type),anonymous(false)
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
      }
    delete kpv;
  }

  HfstTransducer::HfstTransducer(HfstInputStream &in):
  type(in.type), anonymous(false)
  { in.read_transducer(*this); }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),
    key_table(another.key_table)
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
      }
  }

  HfstTransducer &HfstTransducer::remove_epsilons(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::remove_epsilons,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::remove_epsilons,
       type); }

  HfstTransducer &HfstTransducer::determinize(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::determinize,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::determinize,
       type); }

  HfstTransducer &HfstTransducer::minimize(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::minimize,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::minimize,
       type); }

  HfstTransducer &HfstTransducer::repeat_star(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::repeat_star,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::repeat_star,
       type); }

  HfstTransducer &HfstTransducer::repeat_plus(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::repeat_plus,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::repeat_plus,
       type); }

  HfstTransducer &HfstTransducer::repeat_n(int n, ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::repeat_n,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::repeat_n,
       n,type); }

  HfstTransducer &HfstTransducer::repeat_le_n(int n, ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::repeat_le_n,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::repeat_le_n,
       n,type); }

  HfstTransducer &HfstTransducer::optionalize(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::optionalize,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::optionalize,
       type); }

  HfstTransducer &HfstTransducer::input_project(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::extract_input_language,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::extract_input_language,
       type); }

  HfstTransducer &HfstTransducer::output_project(ImplementationType type)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::extract_output_language,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::extract_output_language,
       type); }

  HfstTransducer &HfstTransducer::substitute
  (Key old_key,Key new_key)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::substitute,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::substitute,
       old_key,new_key,
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::substitute,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::substitute,
       key_table.add_symbol(old_symbol),key_table.add_symbol(new_symbol),
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const StringSymbolPair &old_symbol_pair, 
   const StringSymbolPair &new_symbol_pair)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::substitute,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::substitute,
       key_table.get_key_pair(old_symbol_pair),
       key_table.get_key_pair(new_symbol_pair),
       this->type); }

  HfstTransducer &HfstTransducer::substitute
  (const KeyPair &old_key_pair,const KeyPair &new_key_pair)
  { return apply 
      (&HFST_IMPLEMENTATIONS::SfstTransducer::substitute,
       &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::substitute,
       old_key_pair,new_key_pair,
       this->type); }

  HfstTransducer &HfstTransducer::compose
  (HfstTransducer &another,
   ImplementationType type)
  { harmonize(another);
    return apply(&HFST_IMPLEMENTATIONS::SfstTransducer::compose,
		 &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::compose,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::concatenate
  (HfstTransducer &another,
   ImplementationType type)
  { harmonize(another);
    return apply(&HFST_IMPLEMENTATIONS::SfstTransducer::concatenate,
		 &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::concatenate,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::disjunct
  (HfstTransducer &another,
   ImplementationType type)
  { harmonize(another);
    return apply(&HFST_IMPLEMENTATIONS::SfstTransducer::disjunct,
		 &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::disjunct,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::intersect
  (HfstTransducer &another,
   ImplementationType type)
  { harmonize(another);
    return apply(&HFST_IMPLEMENTATIONS::SfstTransducer::intersect,
		 &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::intersect,
		 another,
		 type); }

  HfstTransducer &HfstTransducer::subtract
  (HfstTransducer &another,
   ImplementationType type)
  { harmonize(another);
    return apply(&HFST_IMPLEMENTATIONS::SfstTransducer::subtract,
		 &HFST_IMPLEMENTATIONS::TropicalWeightTransducer::subtract,
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
      }
  }

  template<> HfstTransducer &HfstTransducer::set_final_weight<float>
  (float weight)
  { 
    if (get_weight_type() != FLOAT)
      { throw HFST_IMPLEMENTATIONS::WeightTypeMismatchException(); }
    fst::StdVectorFst * temp =
      HFST_IMPLEMENTATIONS::TropicalWeightTransducer::set_weight
      (implementation.tropical_ofst,weight); 
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = temp;
    return *this; 
  }

  template<> HFST_IMPLEMENTATIONS::SfstTransducer::const_iterator 
  HfstTransducer::begin<SfstTransducer>(void)
  { 
    if (type != SFST_TYPE) 
      { throw HFST_IMPLEMENTATIONS::TransducerHasWrongTypeException(); }
    return SfstTransducer::begin(implementation.sfst); 
  }

  template<> HFST_IMPLEMENTATIONS::SfstTransducer::const_iterator 
  HfstTransducer::end<SfstTransducer>(void)
  { 
    if (type != SFST_TYPE) 
      { throw HFST_IMPLEMENTATIONS::TransducerHasWrongTypeException(); }
    return SfstTransducer::end(implementation.sfst); 
  }

  HfstTransducer &HfstTransducer::convert(ImplementationType type)
  {
    if (type == UNSPECIFIED_TYPE)
      { throw HFST_IMPLEMENTATIONS::SpecifiedTypeRequiredException(); }
    if (type == this->type)
      { return *this; }

    try 
      {
	HFST_IMPLEMENTATIONS::InternalTransducer * internal;
	switch (this->type)
	  {
	  case SFST_TYPE:
	    internal = 
	      HFST_IMPLEMENTATIONS::sfst_to_internal_format(implementation.sfst);
	    delete implementation.sfst;
	    break;
	  case TROPICAL_OFST_TYPE:
	    internal =
	      implementation.tropical_ofst;
	    break;
	  }
	this->type = type;
	switch (type)
	  {
	  case SFST_TYPE:
	    implementation.sfst = 
	      HFST_IMPLEMENTATIONS::internal_format_to_sfst(internal);
	    delete internal;
	    break;
	  case TROPICAL_OFST_TYPE:
	    implementation.tropical_ofst = internal;
	    break;
	  }
      }
    catch (HFST_IMPLEMENTATIONS::HfstInterfaceException e)
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
      }
    return out;
  }

}
#ifdef DEBUG_MAIN
using namespace HFST;
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
  str_fst_ofst2.repeat_plus().compose(str_fst_sfst.convert(SFST_TYPE));
  std::cout << str_fst_ofst2.minimize() << std::endl;
  str_fst_ofst2.convert(TROPICAL_OFST_TYPE).set_final_weight<float>(2);
  std::cout << str_fst_ofst2 << std::endl;
  bool failed = false;
  try
    {
      str_fst_ofst2.convert(SFST_TYPE).set_final_weight<bool>(2);
    }
  catch (HFST_IMPLEMENTATIONS::FunctionNotImplementedException e)
    { failed = true; }
  assert(failed);
  failed = false;
  try
    {
      str_fst_ofst2.set_final_weight<float>(2);
    }
  catch (HFST_IMPLEMENTATIONS::WeightTypeMismatchException e)
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
  //HfstInputStream in("fst.ofst");
  //in.open();
  //HfstTransducer t(in);
  //std::cout << t << std::endl;
}
#endif
