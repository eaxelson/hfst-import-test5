#ifndef _HFST_TRANSDUCER_H_
#define _HFST_TRANSDUCER_H_
#include "implementations/SymbolDefs.h"
#include "implementations/GlobalSymbolTable.h"
#include "implementations/SfstTransducer.h"
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include <string>
#include <cassert>
#include <iostream>

namespace HFST3
{

  using HFST_SYMBOLS::KeyTable;
  using HFST_SYMBOLS::Key;
  using HFST_SYMBOLS::KeyPair;
  using HFST_SYMBOLS::StringSymbolPair;
  using HFST_SYMBOLS::KeyPairVector;
  
  using HFST3_INTERFACE::SfstTransducer;
  using HFST3_INTERFACE::SfstState;
  using HFST3_INTERFACE::SfstTransition;
  using HFST3_INTERFACE::SfstStateIndexer;
  using HFST3_INTERFACE::TropicalWeightTransducer;
  using HFST3_INTERFACE::TropicalWeightState;
  using HFST3_INTERFACE::TropicalWeightTransition;
  using HFST3_INTERFACE::TropicalWeightStateIndexer;

  enum ImplementationType
  {
    SFST_TYPE,
    TROPICAL_OFST_TYPE,
    UNSPECIFIED_TYPE,
    ERROR_TYPE
  };

  enum WeightType
  {
    BOOL,
    FLOAT
  };

  class HfstTransducer;

  class HfstInputStream
  {
  protected:

    union StreamImplementation
    {
      HFST3_INTERFACE::SfstInputStream * sfst;
      HFST3_INTERFACE::TropicalWeightInputStream * tropical_ofst;
    };

    ImplementationType type;
    StreamImplementation implementation;
    void read_transducer(HfstTransducer &t);
    ImplementationType stream_fst_type(std::istream &in);

  public:

    HfstInputStream(void);
    HfstInputStream(const char * filename);
    ~HfstInputStream(void);
    void open(void);
    void close(void);
    bool is_open(void);
    bool is_eof(void);
    bool is_bad(void);
    bool is_good(void);
    bool is_fst(void);
    friend class HfstTransducer;
  };

  class HfstTransducer
  {
  protected:
    union TransducerImplementation
    {
      HFST3_INTERFACE::Transducer * sfst;
      HFST3_INTERFACE::StdVectorFst * tropical_ofst;
      HFST3_INTERFACE::StdVectorFst * internal; 
    };
    
    static HFST3_INTERFACE::SfstTransducer sfst_interface;
    static HFST3_INTERFACE::TropicalWeightTransducer tropical_ofst_interface;

    ImplementationType type;

    bool anonymous; // this variable doesn't do anything yet, but it is
                    // is supposed to keep trakc of whether the transducer's
                    // KeyTable is maintained or not.

    KeyTable key_table;
    TransducerImplementation implementation; 

    void harmonize(HfstTransducer &another);

#include "apply_schemas.h"

  public:
    HfstTransducer(ImplementationType type);
    HfstTransducer(const KeyTable &key_table,ImplementationType type);
    HfstTransducer(const char * utf8_str, 
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    HfstTransducer(const char * upper_utf8_str,
    		   const char * lower_utf8_str,
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    HfstTransducer(HfstInputStream &in);
    HfstTransducer(const HfstTransducer &another);
    ~HfstTransducer(void);
    HfstTransducer &remove_epsilons(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &determinize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &minimize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_star(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_plus(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n(int n,ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_le_n(int n,
				ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &optionalize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &invert(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &input_project(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &output_project(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &substitute(Key old_key,Key new_key);
    HfstTransducer &substitute(const std::string &old_symbol,
			       const std::string &new_symbol);
    HfstTransducer &substitute(const KeyPair &old_key_pair, 
			       const KeyPair &new_key_pair);
    HfstTransducer &substitute(const StringSymbolPair &old_symbol_pair,
			       const StringSymbolPair &new_symbol_pair);
    HfstTransducer &compose(HfstTransducer &another,
			    ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &concatenate(HfstTransducer &another,
				ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &disjunct(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &intersect(HfstTransducer &another,
			      ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &subtract(HfstTransducer &another,
			     ImplementationType type=UNSPECIFIED_TYPE);
    WeightType get_weight_type(void);

    template<class W> HfstTransducer &set_final_weight(W weight) 
      { (void)weight; 
	throw HFST3_INTERFACE::FunctionNotImplementedException(); }

    template<class T> typename T::const_iterator begin(void)
      { throw HFST3_INTERFACE::FunctionNotImplementedException(); }

    template<class T> typename T::const_iterator end(void)
      { throw HFST3_INTERFACE::FunctionNotImplementedException(); }

    HfstTransducer &anonymize(void);
    KeyTable &get_key_table(void);
    ImplementationType get_type(void);
    HfstTransducer &convert(ImplementationType type);
    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
  };

  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);
}
#endif
