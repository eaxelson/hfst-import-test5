#ifndef _HFST_TRANSDUCER_H_
#define _HFST_TRANSDUCER_H_
#include "implementations/SymbolDefs.h"
#include "implementations/GlobalSymbolTable.h"
#include "implementations/SfstTransducer.h"
#include "implementations/TropicalWeightTransducer.h"
#include "implementations/LogWeightTransducer.h"
#include "implementations/FomaTransducer.h"
#include "implementations/HfstTokenizer.h"
#include "implementations/ConvertTransducerFormat.h"
#include <string>
#include <cassert>
#include <iostream>

namespace hfst
{

  using hfst::symbols::KeyTable;
  using hfst::symbols::Key;
  using hfst::symbols::KeyPair;
  using hfst::symbols::KeyMap;
  using hfst::symbols::StringSymbolPair;
  using hfst::symbols::StringSymbolSet;
  using hfst::symbols::KeyPairVector;
  
  using hfst::implementations::SfstTransducer;
  using hfst::implementations::TropicalWeightTransducer;
  using hfst::implementations::TropicalWeightState;
  using hfst::implementations::TropicalWeightStateIterator;
  using hfst::implementations::LogWeightTransducer;
  using hfst::implementations::WeightedStrings;
  using hfst::implementations::WeightedString;
  using hfst::implementations::FomaTransducer;

  enum ImplementationType
  {
    SFST_TYPE,
    TROPICAL_OFST_TYPE,
    LOG_OFST_TYPE,
    FOMA_TYPE,
    UNSPECIFIED_TYPE,
    ERROR_TYPE
  };

  enum WeightType
  {
    BOOL,
    FLOAT
  };

  class HfstTransducer;

  class HfstMutableTransducer;

  class HfstInputStream
  {
  protected:

    union StreamImplementation
    {
      hfst::implementations::SfstInputStream * sfst;
      hfst::implementations::TropicalWeightInputStream * tropical_ofst;
      hfst::implementations::LogWeightInputStream * log_ofst;
      hfst::implementations::FomaInputStream * foma;
    };

    ImplementationType type;
    StreamImplementation implementation;
    void read_transducer(HfstTransducer &t);
    ImplementationType stream_fst_type(std::istream &in);
    int read_library_header(std::istream &in);
    ImplementationType read_version_3_0_fst_type(std::istream &in);
  public:

    HfstInputStream(void);
    HfstInputStream(const char* filename);
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

  class HfstOutputStream
  {
  protected:
    union StreamImplementation
    {
      hfst::implementations::LogWeightOutputStream * log_ofst;
      hfst::implementations::TropicalWeightOutputStream * tropical_ofst;
      hfst::implementations::SfstOutputStream * sfst;
      hfst::implementations::FomaOutputStream * foma;
    };
    ImplementationType type;
    StreamImplementation implementation;

  public:
    HfstOutputStream(ImplementationType type);  // stdout
    HfstOutputStream(const std::string &filename,ImplementationType type);  // file
    ~HfstOutputStream(void);  
    HfstOutputStream &operator<< (HfstTransducer &transducer);  // binary write
    void open(void);
    void close(void);
  };


  typedef hfst::implementations::StateId HfstState;
  typedef float HfstWeight;


  class HfstTransducer
  {
  protected:
    union TransducerImplementation
    {
      hfst::implementations::Transducer * sfst;
      hfst::implementations::StdVectorFst * tropical_ofst;
      hfst::implementations::LogFst * log_ofst;
      fsm * foma;
      hfst::implementations::StdVectorFst * internal; 
    };
    
    static hfst::implementations::SfstTransducer sfst_interface;
    static hfst::implementations::TropicalWeightTransducer tropical_ofst_interface;
    static hfst::implementations::LogWeightTransducer log_ofst_interface;
    static hfst::implementations::FomaTransducer foma_interface;

    ImplementationType type;

    bool anonymous; // this variable doesn't do anything yet, but it is
                    // is supposed to keep track of whether the transducer's
                    // KeyTable is maintained or not.

    KeyTable key_table;

    bool is_trie;

    TransducerImplementation implementation; 

    void harmonize(HfstTransducer &another);
    HfstTransducer &disjunct_as_tries(HfstTransducer &another,
				      ImplementationType type);  

#include "apply_schemas.h"

  public:
    HfstTransducer(ImplementationType type);
    HfstTransducer(const KeyTable &key_table,ImplementationType type);
    HfstTransducer(const std::string& utf8_str, 
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    HfstTransducer(KeyPairVector * kpv, 
		   ImplementationType type);
    HfstTransducer(const std::string& upper_utf8_str,
    		   const std::string& lower_utf8_str,
    		   const HfstTokenizer &multichar_symbol_tokenizer,
		   ImplementationType type);
    HfstTransducer(HfstInputStream &in);
    HfstTransducer(const HfstTransducer &another);
    HfstTransducer(const HfstMutableTransducer &t);
    ~HfstTransducer(void);

    // for testing
    HfstTransducer(const std::string &symbol, ImplementationType type);
    HfstTransducer(const std::string &isymbol, const std::string &osymbol, ImplementationType type);

    void print(void);

    HfstTransducer &remove_epsilons(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &determinize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &minimize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &n_best(int n,ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_star(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_plus(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n_minus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &repeat_n_plus(unsigned int n,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer& repeat_n_to_k(unsigned int n, unsigned int k,
                       ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &optionalize(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &invert(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &input_project(ImplementationType type=UNSPECIFIED_TYPE);
    HfstTransducer &output_project(ImplementationType type=UNSPECIFIED_TYPE);
    void extract_strings(WeightedStrings<float>::Set &results);
    HfstTransducer &substitute(Key old_key, Key new_key);
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
	throw hfst::implementations::FunctionNotImplementedException(); }

    template<class W> HfstTransducer &transform_weights(W (*func)(W weight)) 
      { (void)func; 
	throw hfst::implementations::FunctionNotImplementedException(); }

    template<class T> typename T::const_iterator begin(void)
      { throw hfst::implementations::FunctionNotImplementedException(); }

    template<class T> typename T::const_iterator end(void)
      { throw hfst::implementations::FunctionNotImplementedException(); }

    HfstTransducer &anonymize(void);
    KeyTable &get_key_table(void);
    void set_key_table(const KeyTable &kt);

    //StringSymbolSet get_string_symbol_set(void);
    //KeyMap create_mapping(HfstTransducer &another, StringSymbolSet &unknown_another);

    ImplementationType get_type(void);
    HfstTransducer &convert(ImplementationType type);
    friend std::ostream &operator<<(std::ostream &out, HfstTransducer &t);
    friend class HfstInputStream;
    friend class HfstOutputStream;
    friend class HfstMutableTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };


  /* This class is basically a wrapper for a TROPICAL_OFST_TYPE HfstTransducer.
     Since creating new states and transitions and modifying them is easiest
     in OpenFst, it is chosen as the only implementation type.
     A separate mutable and iterable transducer class is also safer because
     it does not interact with other operations. */
  class HfstMutableTransducer
  {
  protected:
    HfstTransducer transducer;
  public:
    /* Constructors and delete */
    HfstMutableTransducer(void);     /* Returns an empty transducer. */
    HfstMutableTransducer(const HfstTransducer &t);
    HfstMutableTransducer(const HfstMutableTransducer &t);
    ~HfstMutableTransducer(void);
    /* Adding states and transitions */
    HfstState add_state();
    void set_final_weight(HfstState s, HfstWeight w);
    bool is_final(HfstState s);
    HfstState get_initial_state();
    HfstWeight get_final_weight(HfstState s);
    void add_transition(HfstState source, std::string isymbol, std::string osymbol, HfstWeight w, HfstState target);
    /* friend classes */
    friend class HfstTransducer;
    friend class HfstStateIterator;
    friend class HfstTransitionIterator;
  };

  class HfstStateIterator
  {
  protected:
    hfst::implementations::TropicalWeightStateIterator tropical_ofst_iterator;
  public:
    HfstStateIterator(const HfstMutableTransducer &t);
    ~HfstStateIterator(void);
    bool done();
    HfstState value();
    void next();
  };

  class HfstTransition
  {
  protected:
    std::string isymbol;
    std::string osymbol;
    HfstWeight weight;
    HfstState target_state;
    HfstTransition(std::string isymbol, std::string osymbol, HfstWeight weight, HfstState target_state);
  public:
    ~HfstTransition(void);
    std::string get_input_symbol(void);
    std::string get_output_symbol(void);
    HfstWeight get_weight(void);
    HfstState get_target_state(void);
    friend class HfstTransitionIterator;
  };

  class HfstTransitionIterator
  {
  protected:
    hfst::implementations::TropicalWeightTransitionIterator tropical_ofst_iterator;
  public:
    HfstTransitionIterator(const HfstMutableTransducer &t, HfstState s);
    ~HfstTransitionIterator(void);
    bool done();
    HfstTransition value();
    void next();    
  };


  template<> 
    HfstTransducer &HfstTransducer::set_final_weight<float>(float weight);

  template<> 
    HfstTransducer &HfstTransducer::transform_weights<float>(float (*func)(float));

  std::ostream &operator<<(std::ostream &out,HfstTransducer &t);
}



#endif
