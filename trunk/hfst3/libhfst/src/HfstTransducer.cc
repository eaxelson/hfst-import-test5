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

/*  @file HfstTransducer.cc
    \brief Implementations of functions declared in file HfstTransducer.h 

    The implementations call backend implementations that are declared in
    files in the directory implementations. */

#include "HfstTransducer.h"
#include "implementations/compose_intersect/ComposeIntersectLexicon.h"

using hfst::implementations::ConversionFunctions;

namespace hfst
{

  // *** INTERFACES THROUGH WHICH THE BACKEND LIBRARIES ARE CALLED ***

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
  /* Add here the interface between HFST and your transducer library. */
  //#if HAVE_MY_TRANSDUCER_LIBRARY
  //hfst::implementations::MyTransducerLibraryTransducer 
  // HfstTransducer::my_transducer_library_interface;
  //#endif


  // *** TESTING AND OPTIMIZATION FUNCTIONS... ***

  /* The default minimization algorithm if Hopcroft. */
  MinimizationAlgorithm minimization_algorithm=HOPCROFT;
  /* By default, harmonization is not optimized. */
  bool harmonize_smaller=false;
  /* By default, unknown symbols are used. */
  bool unknown_symbols_in_use=true;

  void set_harmonize_smaller(bool value) {
    harmonize_smaller=value; }

  bool get_harmonize_smaller(void) {
    return harmonize_smaller; }

  void set_minimization_algorithm(MinimizationAlgorithm a) {
    minimization_algorithm=a; 
#if HAVE_SFST
    if (minimization_algorithm == HOPCROFT)
      hfst::implementations::sfst_set_hopcroft(true);
    else
      hfst::implementations::sfst_set_hopcroft(false);
#endif
#if HAVE_OPENFST
    if (minimization_algorithm == HOPCROFT)
      hfst::implementations::openfst_tropical_set_hopcroft(true);
    else
      hfst::implementations::openfst_tropical_set_hopcroft(false);
    if (minimization_algorithm == HOPCROFT)
      hfst::implementations::openfst_log_set_hopcroft(true);
    else
      hfst::implementations::openfst_log_set_hopcroft(false);
#endif
    // in foma, Hopcroft is always used
  }

  MinimizationAlgorithm get_minimization_algorithm() {
    return minimization_algorithm; }

  void set_unknown_symbols_in_use(bool value) {
    unknown_symbols_in_use=value; }

  bool get_unknown_symbols_in_use() {
    return unknown_symbols_in_use; }

  /* For profiling. */
  float HfstTransducer::get_profile_seconds(ImplementationType type)
  {
#if HAVE_SFST
    if (type == SFST_TYPE)
      return sfst_interface.get_profile_seconds();
#endif
#if HAVE_OPENFST
    if (type == TROPICAL_OPENFST_TYPE)
      return tropical_ofst_interface.get_profile_seconds();
#endif
    return 0;
  }

  // *** ...TESTING AND OPTIMIZATION FUNCTIONS ENDS ***


  // used only for SFST_TYPE
  StringPairSet HfstTransducer::get_symbol_pairs()
  {
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      return sfst_interface.get_symbol_pairs(this->implementation.sfst);
    else
#endif
      HFST_THROW_MESSAGE(FunctionNotImplementedException, "get_symbol_pairs");
  }

  void HfstTransducer::insert_to_alphabet(const std::string &symbol) 
  {
    HfstTokenizer::check_utf8_correctness(symbol);

    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        sfst_interface.insert_to_alphabet(implementation.sfst, symbol);
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        tropical_ofst_interface.insert_to_alphabet
          (implementation.tropical_ofst, symbol);
      case LOG_OPENFST_TYPE:
        log_ofst_interface.insert_to_alphabet
          (implementation.log_ofst, symbol);
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        foma_interface.insert_to_alphabet(implementation.foma, symbol);
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      default:
        HFST_THROW_MESSAGE(FunctionNotImplementedException,
                           "insert_to_alphabet");
    }    
  }

  StringSet HfstTransducer::get_alphabet() const
  {
    switch(type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        return sfst_interface.get_alphabet(implementation.sfst);
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        return tropical_ofst_interface.get_alphabet
          (implementation.tropical_ofst);
      case LOG_OPENFST_TYPE:
        return log_ofst_interface.get_alphabet(implementation.log_ofst);
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        return foma_interface.get_alphabet(implementation.foma);
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      default:
        HFST_THROW_MESSAGE(FunctionNotImplementedException, "get_alphabet");
    }    
  }


  // *** HARMONIZATION FUNCTIONS... ***

  /* Insert to unknown1 strings that are found in s2 but not in s1 
     and vice versa. */
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

  /*  Harmonize symbol-to-number encodings and expand unknown and 
      identity symbols. 

      In the case of foma transducers, does nothing because foma's own functions
      take care of harmonizing. If harmonization is needed, 
      FomaTransducer::harmonize can be used instead. */
  void HfstTransducer::harmonize(HfstTransducer &another)
  {
    if (this->type != another.type) {
        HFST_THROW(TransducerTypeMismatchException); }

    if (this->anonymous && another.anonymous) {
      return; }

    switch(this->type)
      {
#if HAVE_SFST
      case (SFST_TYPE):
        {
          std::pair <SFST::Transducer*, SFST::Transducer*> result;
          if ( harmonize_smaller && 
               sfst_interface.number_of_states(this->implementation.sfst) >
               sfst_interface.number_of_states(another.implementation.sfst) ) {
            result =
              sfst_interface.harmonize(another.implementation.sfst,
                                       this->implementation.sfst,
                                       unknown_symbols_in_use);
            this->implementation.sfst = result.second;
            another.implementation.sfst = result.first;
          }
          else {
            result =
              sfst_interface.harmonize(this->implementation.sfst,
                                       another.implementation.sfst,
                                       unknown_symbols_in_use);
            this->implementation.sfst = result.first;
            another.implementation.sfst = result.second;
          }
          break;
        }
#endif
#if HAVE_FOMA
      case (FOMA_TYPE):
        // no need to harmonize as foma's functions take care of harmonizing
        break;
#endif
#if HAVE_OPENFST
      case (TROPICAL_OPENFST_TYPE):
        {
          std::pair <fst::StdVectorFst*, fst::StdVectorFst*> result;

          if ( harmonize_smaller && 
               tropical_ofst_interface.number_of_states
               (this->implementation.tropical_ofst) >
               tropical_ofst_interface.number_of_states
               (another.implementation.tropical_ofst) ) {
            result =
              tropical_ofst_interface.harmonize
                (another.implementation.tropical_ofst,
                 this->implementation.tropical_ofst,
                 unknown_symbols_in_use);          
            if (unknown_symbols_in_use) {  // new transducers are created
              delete another.implementation.tropical_ofst;
              delete this->implementation.tropical_ofst; 
            }
            this->implementation.tropical_ofst = result.second;
            another.implementation.tropical_ofst = result.first;
          }
          else {
            result =
              tropical_ofst_interface.harmonize
                (this->implementation.tropical_ofst,
                 another.implementation.tropical_ofst,
                 unknown_symbols_in_use);          
            if (unknown_symbols_in_use) {  // new transducers are created
              delete another.implementation.tropical_ofst;
              delete this->implementation.tropical_ofst; 
            }
            this->implementation.tropical_ofst = result.first;
            another.implementation.tropical_ofst = result.second;
          }
          break;
        }
      case (LOG_OPENFST_TYPE):
        {
          std::pair <hfst::implementations::LogFst*, 
            hfst::implementations::LogFst*> result;

          if ( harmonize_smaller && 
               log_ofst_interface.number_of_states
               (this->implementation.log_ofst) >
               log_ofst_interface.number_of_states
               (another.implementation.log_ofst) ) {
            result =
              log_ofst_interface.harmonize
                (another.implementation.log_ofst,
                 this->implementation.log_ofst,
                 unknown_symbols_in_use);          
            if (unknown_symbols_in_use) {  // new transducers are created
              delete another.implementation.log_ofst;
              delete this->implementation.log_ofst; 
            }
            this->implementation.log_ofst = result.second;
            another.implementation.log_ofst = result.first;
          }
          else {
            result =
              log_ofst_interface.harmonize
                (this->implementation.log_ofst,
                 another.implementation.log_ofst,
                 unknown_symbols_in_use);          
            if (unknown_symbols_in_use) {  // new transducers are created
              delete another.implementation.log_ofst;
              delete this->implementation.log_ofst; 
            }
            this->implementation.log_ofst = result.first;
            another.implementation.log_ofst = result.second;
          }
          break;
        }
#endif
      case (ERROR_TYPE):
      default:
        HFST_THROW(TransducerHasWrongTypeException);
      }
  }

  // *** ...HARMONIZATION FUNCTIONS ENDS ***


  // *** LOOKUP FUNCTIONS... Implemented only for HFST_OL and HFST_OLW *** //

  void HfstTransducer::lookup(HfstOneLevelPaths& results, const StringVector& s,
                              ssize_t limit) const {
    lookup_fd(results, s, limit);
  }

    void HfstTransducer::lookup_fd(HfstOneLevelPaths& results, 
                                   const StringVector& s,
                                   ssize_t limit) const {
        switch(this->type) {

        /* TODO: Convert into HFST_OL(W)_TYPE, if needed? */

        case (HFST_OL_TYPE):
        case (HFST_OLW_TYPE):
            results = this->implementation.hfst_ol->lookup_fd(s);
            return;

        case (ERROR_TYPE):
          HFST_THROW(TransducerHasWrongTypeException);
        default:
            (void)results;
            (void)s;
            (void)limit;
            HFST_THROW(FunctionNotImplementedException);
        }
  }

  void HfstTransducer::lookup(HfstOneLevelPaths& results, 
                              const HfstTokenizer& tok,
                              const std::string &s, 
                              ssize_t limit) const {
    StringVector sv = tok.tokenize_one_level(s);
    lookup(results, sv, limit);
  }

    void HfstTransducer::lookdown(HfstOneLevelPaths& results, 
                                  const StringVector& s,
                                  ssize_t limit) const {
    (void)results;
    (void)s;
    (void)limit;
    HFST_THROW(FunctionNotImplementedException);
  }

    void HfstTransducer::lookdown_fd(HfstOneLevelPaths& results, 
                                     StringVector& s,
                                     ssize_t limit) const {
    (void)results;
    (void)s;
    (void)limit;
    HFST_THROW(FunctionNotImplementedException);
  }

  bool HfstTransducer::is_lookup_infinitely_ambiguous(const StringVector& s)
    const {
      switch(this->type) {
      /* TODO: Convert into HFST_OL(W)_TYPE, if needed. */
      case (HFST_OL_TYPE):
      case (HFST_OLW_TYPE):
          return this->implementation.hfst_ol->is_infinitely_ambiguous();
      default:
          (void)s;
          HFST_THROW(FunctionNotImplementedException);      
      }
  }

  bool HfstTransducer::is_lookdown_infinitely_ambiguous
  (const StringVector& s) const {
    (void)s;
    HFST_THROW(FunctionNotImplementedException);
  }

  // *** ...LOOKUP FUNCTIONS ENDS ***


  // *** Transducer constructors and destructor... *** //

  HfstTransducer::HfstTransducer():
    type(ERROR_TYPE),anonymous(false),is_trie(true), name("")
  {}


  HfstTransducer::HfstTransducer(ImplementationType type):
    type(type),anonymous(false),is_trie(true), name("")
  {
    if (not is_implementation_type_available(type))
      HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.create_empty_transducer();
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.create_empty_transducer();
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.create_empty_transducer();
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma = foma_interface.create_empty_transducer();
        break;
#endif
        /* Add here your implementation. */
        //#if HAVE_MY_TRANSDUCER_LIBRARY
          //case MY_TRANSDUCER_LIBRARY_TYPE:
        //implementation.my_transducer_library 
        //  = my_transducer_library_interface.create_empty_transducer();
        //break;
        //#endif
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
        implementation.hfst_ol = hfst_ol_interface.create_empty_transducer
          (type==HFST_OLW_TYPE?true:false);
        break;
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }


  HfstTransducer::HfstTransducer(const std::string& utf8_str, 
                                 const HfstTokenizer 
                                 &multichar_symbol_tokenizer,
                                 ImplementationType type):
    type(type),anonymous(false),is_trie(true), name("")
  {
    if (not is_implementation_type_available(type))
      HFST_THROW(ImplementationTypeNotAvailableException);

    StringPairVector spv = 
      multichar_symbol_tokenizer.tokenize(utf8_str);
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(spv);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(spv);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.define_transducer(spv);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma =
          foma_interface.define_transducer(spv);
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::HfstTransducer(const StringPairVector & spv, 
                                 ImplementationType type):
    type(type), anonymous(false), is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(spv);
        this->type = SFST_TYPE;
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(spv);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.define_transducer(spv);
        this->type = LOG_OPENFST_TYPE;
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma =
          foma_interface.define_transducer(spv);
        this->type = FOMA_TYPE;
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::HfstTransducer(const StringPairSet & sps, 
                                 ImplementationType type, 
                                 bool cyclic):
    type(type),anonymous(false),is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(sps,cyclic);
        this->type = SFST_TYPE;
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(sps,cyclic);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.define_transducer(sps,cyclic);
        this->type = LOG_OPENFST_TYPE;
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma =
          foma_interface.define_transducer(sps,cyclic);
        this->type = FOMA_TYPE;
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::HfstTransducer(const std::vector<StringPairSet> & spsv,
                                 ImplementationType type):
    type(type),anonymous(false),is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(spsv);
        this->type = SFST_TYPE;
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(spsv);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.define_transducer(spsv);
        this->type = LOG_OPENFST_TYPE;
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma =
          foma_interface.define_transducer(spsv);
        this->type = FOMA_TYPE;
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::HfstTransducer(const std::string& upper_utf8_str,
                                 const std::string& lower_utf8_str,
                                 const HfstTokenizer 
                                 &multichar_symbol_tokenizer,
                                 ImplementationType type):
    type(type),anonymous(false),is_trie(true), name("")
  {
    if (not is_implementation_type_available(type))
      HFST_THROW(ImplementationTypeNotAvailableException);

    StringPairVector spv = 
      multichar_symbol_tokenizer.tokenize
      (upper_utf8_str,lower_utf8_str);
    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(spv);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(spv);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          log_ofst_interface.define_transducer(spv);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma =
          foma_interface.define_transducer(spv);
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(ImplementationTypeNotAvailableException);
      }
  }


  HfstTransducer::HfstTransducer(HfstInputStream &in):
    type(in.type), anonymous(false),is_trie(false), name("")
  {
      if (not is_implementation_type_available(type)) {
        HFST_THROW(ImplementationTypeNotAvailableException);
      }

    in.read_transducer(*this); 
  }

  HfstTransducer::HfstTransducer(const HfstTransducer &another):
    type(another.type),anonymous(another.anonymous),
    is_trie(another.is_trie), name("")
  {
    if (not is_implementation_type_available(type))
      HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.copy(another.implementation.sfst);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst =
          tropical_ofst_interface.copy(another.implementation.tropical_ofst);
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst =
          log_ofst_interface.copy(another.implementation.log_ofst);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma = foma_interface.copy(another.implementation.foma);
        break;
#endif
      case HFST_OL_TYPE:
          implementation.hfst_ol 
            = another.implementation.hfst_ol->copy
            (another.implementation.hfst_ol, false);
          break;
      case HFST_OLW_TYPE:
          implementation.hfst_ol 
            = another.implementation.hfst_ol->copy
            (another.implementation.hfst_ol, true);
          break;
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::HfstTransducer
  ( const hfst::implementations::HfstBasicTransducer &net,
    ImplementationType type):
    type(type),anonymous(false),is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(&net);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(&net);
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_log_ofst(&net);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(&net);
        break;
#endif
      case HFST_OL_TYPE:
        implementation.hfst_ol =
          ConversionFunctions::hfst_basic_transducer_to_hfst_ol(&net, false);
        break;
      case HFST_OLW_TYPE:
        implementation.hfst_ol =
          ConversionFunctions::hfst_basic_transducer_to_hfst_ol(&net, true);
        break;
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  HfstTransducer::~HfstTransducer(void)
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        delete implementation.sfst;
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        delete implementation.tropical_ofst;
        break;
      case LOG_OPENFST_TYPE:
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
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }


HfstTransducer::HfstTransducer(const std::string &symbol, 
                               ImplementationType type): 
  type(type),anonymous(false),is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    HfstTokenizer::check_utf8_correctness(symbol);

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = sfst_interface.define_transducer(symbol);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst = 
          tropical_ofst_interface.define_transducer(symbol);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst = log_ofst_interface.define_transducer(symbol);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma = foma_interface.define_transducer(symbol);
        // should the char* be deleted?
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

HfstTransducer::HfstTransducer(const std::string &isymbol, 
                               const std::string &osymbol, 
                               ImplementationType type):
  type(type),anonymous(false),is_trie(false), name("")
  {
    if (not is_implementation_type_available(type))
        HFST_THROW(ImplementationTypeNotAvailableException);

    HfstTokenizer::check_utf8_correctness(isymbol);
    HfstTokenizer::check_utf8_correctness(osymbol);

    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst 
          = sfst_interface.define_transducer(isymbol, osymbol);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst 
          = tropical_ofst_interface.define_transducer(isymbol, osymbol);
        this->type = TROPICAL_OPENFST_TYPE;
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst 
          = log_ofst_interface.define_transducer(isymbol, osymbol);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma 
          = foma_interface.define_transducer(isymbol, osymbol);
        // should the char*:s be deleted?
        break;
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }

  // *** ...Transducer constructors and destructor ends *** //


  // *** Set and get transducer attributes... *** //

  ImplementationType HfstTransducer::get_type(void) const {
    return this->type; }
  void HfstTransducer::set_name(const std::string &name) {
    HfstTokenizer::check_utf8_correctness(name);
    this->name = name; }   
  std::string HfstTransducer::get_name() const {
    return this->name; }

  // *** ...Set and get transducer attributes ends *** //


  bool HfstTransducer::compare(const HfstTransducer &another) const
  {
    if (this->type != another.type)
      HFST_THROW_MESSAGE(TransducerTypeMismatchException, 
                         "HfstTransducer::compare");

    HfstTransducer one_copy(*this);
    HfstTransducer another_copy(another);
    one_copy.harmonize(another_copy);
    one_copy.minimize();
    another_copy.minimize();

    switch (one_copy.type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        return one_copy.sfst_interface.are_equivalent(
                 one_copy.implementation.sfst, 
                 another_copy.implementation.sfst);
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        return one_copy.tropical_ofst_interface.are_equivalent(
                 one_copy.implementation.tropical_ofst, 
                 another_copy.implementation.tropical_ofst);
      case LOG_OPENFST_TYPE:
        return one_copy.log_ofst_interface.are_equivalent(
                 one_copy.implementation.log_ofst, 
                 another_copy.implementation.log_ofst);
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        return one_copy.foma_interface.are_equivalent(
                 one_copy.implementation.foma, 
                 another_copy.implementation.foma);
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
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
      case TROPICAL_OPENFST_TYPE:
        return tropical_ofst_interface.is_cyclic(implementation.tropical_ofst);
      case LOG_OPENFST_TYPE:
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
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
    }
  }

  HfstTransducer &HfstTransducer::remove_epsilons()
  { is_trie = false;
    return apply(
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::remove_epsilons,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::remove_epsilons,
       &hfst::implementations::LogWeightTransducer::remove_epsilons,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::remove_epsilons,
#endif
       /* Add here your implementation. */
       //#if HAVE_MY_TRANSDUCER_LIBRARY
       //&hfst::implementations::MyTransducerLibraryTransducer::remove_epsilons,
       //#endif
       false ); }

  HfstTransducer &HfstTransducer::determinize()
  { is_trie = false;
    return apply(
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::determinize,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::determinize,
       &hfst::implementations::LogWeightTransducer::determinize,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::determinize,
#endif
       /* Add here your implementation. */
       false ); } 

  HfstTransducer &HfstTransducer::minimize()
  { is_trie = false;
    return apply( 
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::minimize,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::minimize,
       &hfst::implementations::LogWeightTransducer::minimize,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::minimize,
#endif
       /* Add here your implementation. */
       false ); } 

  HfstTransducer &HfstTransducer::repeat_star()
  { is_trie = false;
    return apply(
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::repeat_star,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::repeat_star,
       &hfst::implementations::LogWeightTransducer::repeat_star,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::repeat_star,
#endif
       /* Add here your implementation. */
       false ); }  

  HfstTransducer &HfstTransducer::repeat_plus()
  { is_trie = false;
    return apply( 
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::repeat_plus,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::repeat_plus,
       &hfst::implementations::LogWeightTransducer::repeat_plus,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::repeat_plus,
#endif
       /* Add here your implementation. */
       false ); }  

  HfstTransducer &HfstTransducer::repeat_n(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::repeat_n,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::repeat_n,
       &hfst::implementations::LogWeightTransducer::repeat_n,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::repeat_n,
#endif
       /* Add here your implementation. */
       //#if HAVE_MY_TRANSDUCER_LIBRARY
       //&hfst::implementations::MyTransducerLibraryTransducer::repeat_n,
       //#endif
       n ); }  

  HfstTransducer &HfstTransducer::repeat_n_plus(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer a(*this);
    return (this->repeat_n(n).concatenate(a.repeat_star()));
  }

  HfstTransducer &HfstTransducer::repeat_n_minus(unsigned int n)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::repeat_le_n,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::repeat_le_n,
       &hfst::implementations::LogWeightTransducer::repeat_le_n,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::repeat_le_n,
#endif
       /* Add here your implementation. */
       n ); }   

  HfstTransducer &HfstTransducer::repeat_n_to_k(unsigned int n, unsigned int k)
  { is_trie = false; // This could be done so that is_trie is preserved
    HfstTransducer a(*this);
    return (this->repeat_n(n).concatenate(a.repeat_n_minus(k-n)));
  }

  HfstTransducer &HfstTransducer::optionalize()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
 #if HAVE_SFST
       &hfst::implementations::SfstTransducer::optionalize,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::optionalize,
       &hfst::implementations::LogWeightTransducer::optionalize,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::optionalize,
#endif
       /* Add here your implementation. */
       false ); }   

  HfstTransducer &HfstTransducer::invert()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
 #if HAVE_SFST
       &hfst::implementations::SfstTransducer::invert,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::invert,
       &hfst::implementations::LogWeightTransducer::invert,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::invert,
#endif
       /* Add here your implementation. */
       false ); }    

  HfstTransducer &HfstTransducer::reverse()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply (
 #if HAVE_SFST
       &hfst::implementations::SfstTransducer::reverse,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::reverse,
       &hfst::implementations::LogWeightTransducer::reverse,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::reverse,
#endif
       /* Add here your implementation. */
       false ); }    

  HfstTransducer &HfstTransducer::input_project()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply (
 #if HAVE_SFST
       &hfst::implementations::SfstTransducer::extract_input_language,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::extract_input_language,
       &hfst::implementations::LogWeightTransducer::extract_input_language,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::extract_input_language,
#endif
       /* Add here your implementation. */
       false ); }    

  HfstTransducer &HfstTransducer::output_project()
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply (
 #if HAVE_SFST
       &hfst::implementations::SfstTransducer::extract_output_language,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::
         extract_output_language,
       &hfst::implementations::LogWeightTransducer::extract_output_language,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::extract_output_language,
#endif
       /* Add here your implementation. */
       false ); }    

  std::vector<HfstTransducer*> HfstTransducer::extract_path_transducers()
  {
    if (this->type != SFST_TYPE)
        HFST_THROW(FunctionNotImplementedException);

    std::vector<HfstTransducer*> hfst_paths;
#if HAVE_SFST
    std::vector<SFST::Transducer*> sfst_paths 
      = sfst_interface.extract_path_transducers(this->implementation.sfst);
    for (std::vector<SFST::Transducer*>::iterator it 
           = sfst_paths.begin(); it != sfst_paths.end(); it++)
      {
        HfstTransducer *tr = new HfstTransducer(SFST_TYPE);
        delete tr->implementation.sfst;
        tr->implementation.sfst = *it;
        hfst_paths.push_back(tr);
      }
#endif
    return hfst_paths;
  }

  void HfstTransducer::extract_paths(ExtractStringsCb& callback, int cycles)
    const
  { 
    switch (this->type)
      {
#if HAVE_OPENFST
      case LOG_OPENFST_TYPE:
        hfst::implementations::LogWeightTransducer::extract_paths
          (implementation.log_ofst,callback,cycles,NULL,false);
        break;
      case TROPICAL_OPENFST_TYPE:
        hfst::implementations::TropicalWeightTransducer::extract_paths
          (implementation.tropical_ofst,callback,cycles,NULL,false);
        break;
#endif
#if HAVE_SFST
      case SFST_TYPE:
        hfst::implementations::SfstTransducer::extract_paths
          (implementation.sfst, callback, cycles,NULL,false);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        hfst::implementations::FomaTransducer::extract_paths
          (implementation.foma, callback, cycles,NULL,false);
        break;
#endif
       /* Add here your implementation. */
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
        hfst::implementations::HfstOlTransducer::extract_paths
          (implementation.hfst_ol, callback, cycles);
        break;
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }
  
  void HfstTransducer::extract_paths_fd(ExtractStringsCb& callback, 
                                          int cycles, bool filter_fd)
    const
  { 
    switch (this->type)
      {
#if HAVE_OPENFST
      case LOG_OPENFST_TYPE:
      {
        FdTable<int64>* t_log_ofst 
          = hfst::implementations::LogWeightTransducer::get_flag_diacritics
          (implementation.log_ofst);
        hfst::implementations::LogWeightTransducer::extract_paths
          (implementation.log_ofst,callback,cycles,t_log_ofst,filter_fd);
        delete t_log_ofst;
      }
        break;
      case TROPICAL_OPENFST_TYPE:
      {
        FdTable<int64>* t_tropical_ofst 
          = hfst::implementations::TropicalWeightTransducer::
            get_flag_diacritics(implementation.tropical_ofst);
        hfst::implementations::TropicalWeightTransducer::extract_paths
          (implementation.tropical_ofst,callback,cycles,
           t_tropical_ofst,filter_fd);
        delete t_tropical_ofst;
      }
        break;
#endif
#if HAVE_SFST
      case SFST_TYPE:
      {
        FdTable<SFST::Character>* t_sfst 
          = hfst::implementations::SfstTransducer::get_flag_diacritics
          (implementation.sfst);
        hfst::implementations::SfstTransducer::extract_paths
          (implementation.sfst, callback, cycles, t_sfst, filter_fd);
        delete t_sfst;
      }
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
      {
          FdTable<int>* t_foma 
            = hfst::implementations::FomaTransducer::get_flag_diacritics
            (implementation.foma);
          hfst::implementations::FomaTransducer::extract_paths
            (implementation.foma, callback, cycles, t_foma, filter_fd);
          delete t_foma;
      }
        break;
#endif
       /* Add here your implementation. */
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      {
        const FdTable<hfst_ol::SymbolNumber>* t_hfst_ol 
          = hfst::implementations::HfstOlTransducer::get_flag_diacritics
          (implementation.hfst_ol);
        hfst::implementations::HfstOlTransducer::extract_paths
          (implementation.hfst_ol,callback,cycles,t_hfst_ol,filter_fd);
        delete t_hfst_ol;
      }
        break;
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }
  }
  
  class ExtractStringsCb_ : public ExtractStringsCb
  {
    public:
    //WeightedPaths<float>::Set& paths;
    HfstTwoLevelPaths& paths;
    int max_num;
      
      ExtractStringsCb_(HfstTwoLevelPaths& p, int max): 
        paths(p), max_num(max) {}
      RetVal operator()(HfstTwoLevelPath& path, bool final)
      {
        if(final)
          paths.insert(path);
        
        return RetVal((max_num < 1) || (int)paths.size() < max_num, true);
      }
  };
  
  void HfstTransducer::extract_paths(HfstTwoLevelPaths &results,
                                       int max_num, int cycles) const
  {
    if(is_cyclic() && max_num < 1 && cycles < 0)
      HFST_THROW_MESSAGE(TransducerIsCyclicException,
                         "HfstTransducer::extract_paths");
    
    ExtractStringsCb_ cb(results, max_num);
    extract_paths(cb, cycles);
  }
  
  void HfstTransducer::extract_paths_fd(HfstTwoLevelPaths &results,
                                          int max_num, int cycles,
                                          bool filter_fd) const
  {
    if(is_cyclic() && max_num < 1 && cycles < 0)
      HFST_THROW_MESSAGE(TransducerIsCyclicException,
                         "HfstTransducer::extract_paths_fd");
    
    ExtractStringsCb_ cb(results, max_num);
    extract_paths_fd(cb, cycles, filter_fd);
  }

  bool HfstTransducer::check_for_missing_flags_in
  (const HfstTransducer &another,
   StringSet &missing_flags,
   bool return_on_first_miss) const
  {
    bool retval=false;
    StringSet this_alphabet = get_alphabet();
    StringSet another_alphabet = another.get_alphabet();

    for (StringSet::const_iterator it = another_alphabet.begin();
         it != another_alphabet.end(); it++)
      {
        if ( FdOperation::is_diacritic(*it) && 
             (this_alphabet.find(*it) == this_alphabet.end()) )
          {
            missing_flags.insert(*it);
            retval = true;
            if (return_on_first_miss)
              return retval;
          }
      }
    return retval;
  }

  void HfstTransducer::insert_freely_missing_flags_from
    (const HfstTransducer &another) 
  {
    StringSet missing_flags;
    if (check_for_missing_flags_in(another, missing_flags,
                                   false /* do not return on first miss */ ))
      {
        for (StringSet::const_iterator it = missing_flags.begin();
             it != missing_flags.end(); it++)
          {
            insert_freely(StringPair(*it, *it));
          }
      }
  }

  bool HfstTransducer::check_for_missing_flags_in
    (const HfstTransducer &another) const
  {
    StringSet foo; /* An obligatory argument that is not used. */
    return check_for_missing_flags_in(another, foo, 
                                      true /* return on first miss */);
  }

  HfstTransducer &HfstTransducer::insert_freely
  (const StringPair &symbol_pair)
  {
    HfstTokenizer::check_utf8_correctness(symbol_pair.first);
    HfstTokenizer::check_utf8_correctness(symbol_pair.second);

    /* Add symbols in symbol_pair to the alphabet of this transducer
       and expand unknown and epsilon symbols accordingly. */
    HfstTransducer tmp(symbol_pair.first, symbol_pair.second, this->type);
    tmp.harmonize(*this);

    switch (this->type)    
      {
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        {
          hfst::implementations::TropicalWeightTransducer::insert_freely
            (implementation.tropical_ofst,symbol_pair);
          break;
        }
      case LOG_OPENFST_TYPE:
        {
          hfst::implementations::LogWeightTransducer::insert_freely
            (implementation.log_ofst,symbol_pair);
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->insert_freely(symbol_pair, 0);
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
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
       /* Add here your implementation. */
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }

    return *this;
  }

  HfstTransducer &HfstTransducer::insert_freely
  (const HfstTransducer &tr)
  {
    if (this->type != tr.type)
      HFST_THROW_MESSAGE(TransducerTypeMismatchException,
                         "HfstTransducer::insert_freely");  

    /* In this function, this transducer must always be harmonized
       according to tr, not the other way round. */
    bool harm = harmonize_smaller;
    harmonize_smaller=false;
    this->harmonize(const_cast<HfstTransducer&>(tr));
    harmonize_smaller=harm;

    switch (this->type)    
      {
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        {
          hfst::implementations::HfstBasicTransducer * net = 
            ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
            (implementation.tropical_ofst);
          delete implementation.tropical_ofst;
          
          hfst::implementations::HfstBasicTransducer * substituting_net = 
            ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
            (tr.implementation.tropical_ofst);
          
          net->insert_freely(*substituting_net);
          delete substituting_net;
          implementation.tropical_ofst = 
            ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(net);
          delete net;
          return *this;
          break;
        }
      case LOG_OPENFST_TYPE:
        {
          hfst::implementations::HfstBasicTransducer * net = 
            ConversionFunctions::log_ofst_to_hfst_basic_transducer
            (implementation.log_ofst);
          delete implementation.log_ofst;
          
          hfst::implementations::HfstBasicTransducer * substituting_net = 
            ConversionFunctions::log_ofst_to_hfst_basic_transducer
            (tr.implementation.log_ofst);
          
          net->insert_freely(*substituting_net);
          delete substituting_net;
          implementation.log_ofst = 
            ConversionFunctions::hfst_basic_transducer_to_log_ofst(net);
          delete net;
          return *this;
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        {
          // HfstTransducer::harmonize does nothing to a foma transducer,
          // because foma's own functions take care of harmonizing.
          // Now we need to harmonize because we are using internal transducers.
          this->foma_interface.harmonize
            (implementation.foma,tr.implementation.foma);

          hfst::implementations::HfstBasicTransducer * net = 
            ConversionFunctions::foma_to_hfst_basic_transducer
            (implementation.foma);
          this->foma_interface.delete_foma(implementation.foma);
          
          hfst::implementations::HfstBasicTransducer * substituting_net = 
            ConversionFunctions::foma_to_hfst_basic_transducer
            (tr.implementation.foma);
          
          net->insert_freely(*substituting_net);
          delete substituting_net;
          implementation.foma = 
            ConversionFunctions::hfst_basic_transducer_to_foma(net);
          delete net;
          return *this;
          break;
        }
#endif
#if HAVE_SFST
      case SFST_TYPE:
        {
          hfst::implementations::HfstBasicTransducer * net = 
            ConversionFunctions::sfst_to_hfst_basic_transducer
            (implementation.sfst);
          delete implementation.sfst;
          
          hfst::implementations::HfstBasicTransducer * substituting_net = 
            ConversionFunctions::sfst_to_hfst_basic_transducer
            (tr.implementation.sfst);
          
          net->insert_freely(*substituting_net);
          delete substituting_net;
          implementation.sfst = 
            ConversionFunctions::hfst_basic_transducer_to_sfst(net);
          delete net;
          return *this;
          break;
        }
#endif
        /* Add here your implementation. */
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }

    return *this;
  }


  HfstTransducer &HfstTransducer::substitute
  (bool (*func)(const StringPair &sp, StringPairSet &sps))
  {
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::sfst_to_hfst_basic_transducer
          (implementation.sfst);
        delete implementation.sfst;
        net->substitute(func);
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->substitute(func);
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
          (implementation.tropical_ofst);
        delete implementation.tropical_ofst;
        net->substitute(func);
        implementation.tropical_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(net);
        delete net;
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::log_ofst_to_hfst_basic_transducer
          (implementation.log_ofst);
        delete implementation.log_ofst;
        net->substitute(func);
        implementation.log_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_log_ofst(net);
        delete net;
        return *this;
      }
#endif
    /* Add here your implementation. */
#if HAVE_MY_TRANSDUCER_LIBRARY
    //if (this->type == MY_TRANSDUCER_LIBRARY_TYPE)
    //  {
    //        hfst::implementations::HfstBasicTransducer * net = 
    //          ConversionFunctions::
    //      my_transducer_library_transducer_to_hfst_basic_transducer
    //              (implementation.my_transducer_library);
    //        delete(implementation.my_transducer_library);
    //        net->substitute(func);
    //        implementation.my_transducer_library = 
    //          ConversionFunctions::
    //      hfst_basic_transducer_to_my_transducer_library_transducer(net);
    //        delete net;
    //        return *this;
#endif
    if (this->type == ERROR_TYPE) {
      HFST_THROW(TransducerHasWrongTypeException);
    }

    HFST_THROW(FunctionNotImplementedException);
  }

  HfstTransducer &HfstTransducer::substitute
  (const std::string &old_symbol, const std::string &new_symbol,
   bool input_side, bool output_side)
  {
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        if (input_side && output_side)
          {
            hfst::implementations::Transducer * tmp =
              this->sfst_interface.substitute
              (implementation.sfst, old_symbol, new_symbol);
            delete implementation.sfst;
            implementation.sfst = tmp;
            return *this;
          }
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::sfst_to_hfst_basic_transducer
          (implementation.sfst);
        delete implementation.sfst;
        net->substitute(old_symbol, new_symbol, input_side, output_side);
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->substitute(old_symbol, new_symbol, input_side, output_side);
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        if (input_side && output_side)
          {
            fst::StdVectorFst * tmp =
              this->tropical_ofst_interface.substitute
              (implementation.tropical_ofst, old_symbol, new_symbol);
            delete implementation.tropical_ofst;
            implementation.tropical_ofst = tmp;
            return *this;
          }
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
          (implementation.tropical_ofst);
        delete implementation.tropical_ofst;
        net->substitute(old_symbol, new_symbol, input_side, output_side);
        implementation.tropical_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(net);
        delete net;
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        if (input_side && output_side)
          {
            hfst::implementations::LogFst * tmp =
              this->log_ofst_interface.substitute
              (implementation.log_ofst, old_symbol, new_symbol);
            delete implementation.log_ofst;
            implementation.log_ofst = tmp;
            return *this;
          }
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::log_ofst_to_hfst_basic_transducer
          (implementation.log_ofst);
        delete implementation.log_ofst;
        net->substitute(old_symbol, new_symbol, input_side, output_side);
        implementation.log_ofst = 
          ConversionFunctions::hfst_basic_transducer_to_log_ofst(net);
        delete net;
        return *this;
      }
#endif
    if (this->type == ERROR_TYPE) {
      HFST_THROW(TransducerHasWrongTypeException);
    }

    HFST_THROW(FunctionNotImplementedException);
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPair &new_symbol_pair)
  { 
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::sfst_to_hfst_basic_transducer
          (implementation.sfst);
        delete implementation.sfst;
        net->substitute(old_symbol_pair, new_symbol_pair);
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->substitute(old_symbol_pair, new_symbol_pair);
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        fst::StdVectorFst * tropical_ofst_temp =
          this->tropical_ofst_interface.substitute
          (implementation.tropical_ofst,old_symbol_pair, new_symbol_pair);
        delete implementation.tropical_ofst;
        implementation.tropical_ofst = tropical_ofst_temp;
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        hfst::implementations::LogFst * log_ofst_temp =
          this->log_ofst_interface.substitute
          (implementation.log_ofst,old_symbol_pair,new_symbol_pair);
        delete implementation.log_ofst;
        implementation.log_ofst = log_ofst_temp;
        return *this;
      }
#endif
    if (this->type == ERROR_TYPE) {
      HFST_THROW(TransducerHasWrongTypeException);
    }

    HFST_THROW(FunctionNotImplementedException);
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &old_symbol_pair, 
   const StringPairSet &new_symbol_pair_set)
  { 
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::sfst_to_hfst_basic_transducer
          (implementation.sfst);
        delete implementation.sfst;
        net->substitute(old_symbol_pair, new_symbol_pair_set);
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->substitute(old_symbol_pair, new_symbol_pair_set);
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        /* The implementation of TropicalWeightTransducer segfaults
           for some reason if the symbol pair is substituted with itself. */
        if (new_symbol_pair_set.find
            (StringPair(old_symbol_pair.first, 
                        old_symbol_pair.second))
            != new_symbol_pair_set.end())
          {
            hfst::implementations::HfstBasicTransducer * net = 
              ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
              (implementation.tropical_ofst);
            delete(implementation.tropical_ofst);
            net->substitute(old_symbol_pair, new_symbol_pair_set);
            implementation.tropical_ofst = 
              ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(net);
            delete net;
            return *this;
          }
        fst::StdVectorFst * tropical_ofst_temp =
          this->tropical_ofst_interface.substitute
          (implementation.tropical_ofst,old_symbol_pair, new_symbol_pair_set);
        delete implementation.tropical_ofst;
        implementation.tropical_ofst = tropical_ofst_temp;
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        /* The implementation of LogWeightTransducer segfaults
           for some reason if the symbol pair is substituted with itself. */
        if (new_symbol_pair_set.find
            (StringPair(old_symbol_pair.first, 
                        old_symbol_pair.second))
            != new_symbol_pair_set.end())
          {
            hfst::implementations::HfstBasicTransducer * net = 
              ConversionFunctions::log_ofst_to_hfst_basic_transducer
              (implementation.log_ofst);
            delete(implementation.log_ofst);
            net->substitute(old_symbol_pair, new_symbol_pair_set);
            implementation.log_ofst = 
              ConversionFunctions::hfst_basic_transducer_to_log_ofst(net);
            delete net;
            return *this;
          }
        hfst::implementations::LogFst * log_ofst_temp =
          this->log_ofst_interface.substitute
          (implementation.log_ofst,old_symbol_pair, new_symbol_pair_set);
        delete implementation.log_ofst;
        implementation.log_ofst = log_ofst_temp;
        return *this;
      }
#endif
    if (this->type == ERROR_TYPE) {
      HFST_THROW(TransducerHasWrongTypeException);
    }

    HFST_THROW(FunctionNotImplementedException);
  }

  HfstTransducer &HfstTransducer::substitute
  (const StringPair &symbol_pair,
   HfstTransducer &transducer)
  { 
    if (this->type != transducer.type) {
      HFST_THROW_MESSAGE(TransducerTypeMismatchException,
                         "HfstTransducer::substitute"); }

    bool harm = harmonize_smaller;
    harmonize_smaller=false;
    this->harmonize(transducer);
    harmonize_smaller=harm;

#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        // HfstTransducer::harmonize does nothing to a foma transducer,
        // because foma's own functions take care of harmonizing.
        // Now we need to harmonize because we are using internal transducers.
        this->foma_interface.harmonize
          (implementation.foma,transducer.implementation.foma);

        hfst::implementations::HfstBasicTransducer * net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);

        hfst::implementations::HfstBasicTransducer * substituting_net = 
          ConversionFunctions::foma_to_hfst_basic_transducer
          (transducer.implementation.foma);

        net->substitute(symbol_pair, *substituting_net);
        delete substituting_net;
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(net);
        delete net;
        return *this;
      }
#endif
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::Transducer * tmp =
          this->sfst_interface.substitute
          (implementation.sfst, symbol_pair, transducer.implementation.sfst);
        delete implementation.sfst;
        implementation.sfst = tmp;
        return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        this->tropical_ofst_interface.substitute
          (implementation.tropical_ofst,
           symbol_pair,transducer.implementation.tropical_ofst);
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        this->log_ofst_interface.substitute
          (implementation.log_ofst,
           symbol_pair,transducer.implementation.log_ofst);
        return *this;
      }
#endif
    if (this->type == ERROR_TYPE) {
      HFST_THROW(TransducerHasWrongTypeException);
    }

    HFST_THROW(FunctionNotImplementedException);
  }

  HfstTransducer &HfstTransducer::set_final_weights(float weight)
  {
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE) {
      implementation.tropical_ofst  =
        this->tropical_ofst_interface.set_final_weights
          (this->implementation.tropical_ofst, weight);
      return *this;
    }
    if (this->type == LOG_OPENFST_TYPE) {
      implementation.log_ofst  =
        this->log_ofst_interface.set_final_weights
        (this->implementation.log_ofst, weight);
      return *this; 
    }
#endif
    (void)weight;
    return *this;
  }

  HfstTransducer &HfstTransducer::push_weights(PushType push_type)
  {
#if HAVE_OPENFST
    bool to_initial_state = (push_type == TO_INITIAL_STATE);
    if (this->type == TROPICAL_OPENFST_TYPE) 
      {
        hfst::implementations::StdVectorFst * tmp  =
          this->tropical_ofst_interface.push_weights
          (this->implementation.tropical_ofst, to_initial_state);
        delete this->implementation.tropical_ofst;
        this->implementation.tropical_ofst = tmp;
        return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        hfst::implementations::LogFst * tmp =
          this->log_ofst_interface.push_weights
          (this->implementation.log_ofst, to_initial_state);
        delete this->implementation.log_ofst;
        this->implementation.log_ofst = tmp;
        return *this;
      }
#endif
    (void)push_type;
    return *this;
  }


  HfstTransducer &HfstTransducer::transform_weights(float (*func)(float))
  {
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE) {
      implementation.tropical_ofst  =
        this->tropical_ofst_interface.transform_weights
        (this->implementation.tropical_ofst, func);
      return *this;
    }
    if (this->type == LOG_OPENFST_TYPE) {
      implementation.log_ofst  =
        this->log_ofst_interface.transform_weights
        (this->implementation.log_ofst, func);
      return *this;
    }
#endif
    (void)func;
    return *this;
  }

  bool substitute_single_identity_with_the_other_symbol
  (const StringPair &sp, StringPairSet &sps)
  {
    std::string isymbol = sp.first;
    std::string osymbol = sp.second;
    
    if (isymbol.compare("@_IDENTITY_SYMBOL_@") == 0 && 
        (osymbol.compare("@_IDENTITY_SYMBOL_@") != 0)) {
      isymbol = osymbol; //std::string("@_UNKNOWN_SYMBOL_@");
      sps.insert(StringPair(isymbol, osymbol));
      return true;
    }
    else if (osymbol.compare("@_IDENTITY_SYMBOL_@") == 0 && 
             (isymbol.compare("@_IDENTITY_SYMBOL_@") != 0)) {
      osymbol = isymbol; //std::string("@_UNKNOWN_SYMBOL_@");
      sps.insert(StringPair(isymbol, osymbol));
      return true;
    }
    else
      return false;
  }

  bool substitute_unknown_identity_pairs
  (const StringPair &sp, StringPairSet &sps)
  {
    std::string isymbol = sp.first;
    std::string osymbol = sp.second;

    if (isymbol.compare("@_UNKNOWN_SYMBOL_@") == 0 && 
        osymbol.compare("@_IDENTITY_SYMBOL_@") == 0) {
      isymbol = std::string("@_IDENTITY_SYMBOL_@");
      osymbol = std::string("@_IDENTITY_SYMBOL_@");
      sps.insert(StringPair(isymbol, osymbol));
      return true;
    }
    return false;
  }

  unsigned int HfstTransducer::number_of_states() const
  {
#if HAVE_OPENFST
    if (type == TROPICAL_OPENFST_TYPE)
      return this->tropical_ofst_interface.number_of_states
        (this->implementation.tropical_ofst);
#endif
#if HAVE_SFST
    if (type == SFST_TYPE)
      return this->sfst_interface.number_of_states
        (this->implementation.sfst);
#endif
    return 0;
  }

  HfstTransducer &HfstTransducer::compose
  (const HfstTransducer &another)
  { is_trie = false;

    if (this->type != another.type)
      HFST_THROW_MESSAGE(HfstTransducerTypeMismatchException,
                         "HfstTransducer::compose");

    bool DEBUG=false;

    /* Substitution requires conversion to HfstBasicTransducer
       which can change the symbol-to-number mappings. 
       That is why it must be done before harmonizing. */
    if ( (this->type != FOMA_TYPE) && unknown_symbols_in_use) 
      {
        if (DEBUG) fprintf(stderr,"substituting for composition..\n");

        // comment...
        this->substitute("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",false,true);
        (const_cast<HfstTransducer&>(another)).substitute
          ("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",true,false);

        if (DEBUG) fprintf(stderr,"..done\n");
      }

    /*fprintf(stderr, "before harmonization:\n");
    this->write_in_att_format(stderr);
    fprintf(stderr, "--\n");
    another.write_in_att_format(stderr);
    fprintf(stderr, "\n");*/

    if (DEBUG) fprintf(stderr, "harmonizing for composition..\n");
    this->harmonize(const_cast<HfstTransducer&>(another));

    /*fprintf(stderr, "before composition:\n");
    this->write_in_att_format(stderr);
    fprintf(stderr, "--\n");
    another.write_in_att_format(stderr);
    fprintf(stderr, "\n");*/

    if (DEBUG) fprintf(stderr,"..done\n");
    
    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        {
          SFST::Transducer * sfst_temp =
            this->sfst_interface.compose
              (implementation.sfst,another.implementation.sfst);
          delete implementation.sfst;
          implementation.sfst = sfst_temp;
          break;
        }
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        {
          fst::StdVectorFst * tropical_ofst_temp =
            this->tropical_ofst_interface.compose
              (this->implementation.tropical_ofst,
               another.implementation.tropical_ofst);
          delete implementation.tropical_ofst;
          implementation.tropical_ofst = tropical_ofst_temp;
          break;
        }
      case LOG_OPENFST_TYPE:
        {
          hfst::implementations::LogFst * log_ofst_temp =
            this->log_ofst_interface.compose(implementation.log_ofst,
                           another.implementation.log_ofst);
          delete implementation.log_ofst;
          implementation.log_ofst = log_ofst_temp;
          break;
        }
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        {
          fsm * foma_temp =
            this->foma_interface.compose
            (implementation.foma,another.implementation.foma);
          this->foma_interface.delete_foma(implementation.foma);
          implementation.foma = foma_temp;
          break;
        }
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      default:
        HFST_THROW(FunctionNotImplementedException);
      }

    /*fprintf(stderr, "after composition:\n");
    this->write_in_att_format(stderr);
    fprintf(stderr, "--\n");
    another.write_in_att_format(stderr);
    fprintf(stderr, "\n");*/

    //#if HAVE_FOMA
    if ( (this->type != FOMA_TYPE) && unknown_symbols_in_use) 
      {
        if (DEBUG) fprintf(stderr,"substituting after composition..\n");

        // comment...
        this->substitute(&substitute_single_identity_with_the_other_symbol);
        (const_cast<HfstTransducer&>(another)).
          substitute(&substitute_unknown_identity_pairs);

        if (DEBUG) fprintf(stderr,"..done\n");
      }
    //#endif

    return *this;
  }

  HfstTransducer &HfstTransducer::compose_intersect
  (const HfstTransducerVector &v)
  {
    if (v.empty())
      { *this = HfstTransducer(type); }
    
    const HfstTransducer &first = *v.begin();

    // If rule transducers contain word boundaries, add word boundaries to 
    // the lexicon unless the lexicon already contains them. 
    std::set<std::string> rule_alphabet = first.get_alphabet();
    bool remove_word_boundary = false;
    if (rule_alphabet.find("@#@") != rule_alphabet.end())
      { 
            std::set<std::string> lexicon_alphabet = get_alphabet();
            remove_word_boundary = 
              (lexicon_alphabet.find("@#@") == lexicon_alphabet.end());
            HfstTokenizer tokenizer;
            tokenizer.add_multichar_symbol("@#@");
            tokenizer.add_multichar_symbol("@_EPSILON_SYMBOL_@");
            HfstTransducer wb("@_EPSILON_SYMBOL_@","@#@",tokenizer,type);
            HfstTransducer wb_copy(wb);
            wb.concatenate(*this).concatenate(wb_copy).minimize();
            *this = wb;
      }

    if (v.size() == 1) 
      {
            // In case there is only onw rule, compose with that.
            implementations::ComposeIntersectRule rule(v.at(0));
            // Create a ComposeIntersectLexicon from *this. 
            implementations::ComposeIntersectLexicon lexicon(*this);
            hfst::implementations::HfstBasicTransducer res = 
              lexicon.compose_with_rules(&rule);
            res.prune_alphabet();
            *this = HfstTransducer(res,type);
      }
    else
      {
	// In case there are many rules, build a ComposeIntersectRulePair 
	// recursively and compose with that.
	std::vector<implementations::ComposeIntersectRule*> rule_vector;
	implementations::ComposeIntersectRule * first_rule = 
	  new implementations::ComposeIntersectRule(*v.begin());
	implementations::ComposeIntersectRule * second_rule = 
	  new implementations::ComposeIntersectRule(*(v.begin() + 1));

        implementations::ComposeIntersectRulePair * rules = 
          new implementations::ComposeIntersectRulePair
          (first_rule,second_rule);
	
            for (HfstTransducerVector::const_iterator it = v.begin() + 2;
                 it != v.end();
                 ++it)
              { rules = new implementations::ComposeIntersectRulePair
                  (new implementations::ComposeIntersectRule(*it),rules); }
            // Create a ComposeIntersectLexicon from *this. 
            implementations::ComposeIntersectLexicon lexicon(*this);
            hfst::implementations::HfstBasicTransducer res = 
              lexicon.compose_with_rules(rules);
            res.prune_alphabet();
            *this = HfstTransducer(res,type);
            delete rules;
      }
    return *this;
  }

  HfstTransducer &HfstTransducer::concatenate
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply
      (
#if HAVE_SFST
       &hfst::implementations::SfstTransducer::concatenate,
#endif
#if HAVE_OPENFST
       &hfst::implementations::TropicalWeightTransducer::concatenate,
       &hfst::implementations::LogWeightTransducer::concatenate,
#endif
#if HAVE_FOMA
       &hfst::implementations::FomaTransducer::concatenate,
#endif
       /* Add here your implementation. */
       //#if HAVE_MY_TRANSDUCER_LIBRARY
       //&hfst::implementations::MyTransducerLibraryTransducer::concatenate,
       //#endif
       const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::disjunct(const StringPairVector &spv)
  {
    switch (this->type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        sfst_interface.disjunct(implementation.sfst, spv);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        tropical_ofst_interface.disjunct(implementation.tropical_ofst, spv);
        break;
      case LOG_OPENFST_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
#endif
      case FOMA_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
       /* Add here your implementation. */
      default:
        assert(false);
      }
    return *this; 
  }

  // TODO...
  HfstTransducer &HfstTransducer::disjunct_as_tries(HfstTransducer &another,
                                                    ImplementationType type)
  {
    convert(type);
    if (type != another.type)
      { another = HfstTransducer(another).convert(type); }

    switch (this->type)
      {
      case SFST_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
      case LOG_OPENFST_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
#endif
      case FOMA_TYPE:
        HFST_THROW(FunctionNotImplementedException);
        break;
      default:
        assert(false);
      }
    return *this; 
  }

  HfstTransducer &HfstTransducer::n_best(unsigned int n) 
  {
    if (not is_implementation_type_available(TROPICAL_OPENFST_TYPE)) {
      (void)n;
      HFST_THROW_MESSAGE(ImplementationTypeNotAvailableException,
                         "HfstTransducer::n_best implemented only for "
                         "TROPICAL_OPENFST_TYPE");
    }

#if HAVE_OPENFST
    ImplementationType original_type = this->type;
    if ((original_type == SFST_TYPE) || (original_type == FOMA_TYPE)) {
      this->convert(TROPICAL_OPENFST_TYPE);
    }
#endif

    switch (this->type)
      {
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        {
          fst::StdVectorFst * temp =
            hfst::implementations::TropicalWeightTransducer::n_best
            (implementation.tropical_ofst,(int)n);
          delete implementation.tropical_ofst;
          implementation.tropical_ofst = temp;
          break;
      }
      case LOG_OPENFST_TYPE:
        {
          hfst::implementations::LogFst * temp =
            hfst::implementations::LogWeightTransducer::n_best
            (implementation.log_ofst,(int)n);
          delete implementation.log_ofst;
          implementation.log_ofst = temp;
          break;
        }
#endif
      case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
      case HFST_OL_TYPE:
      case HFST_OLW_TYPE:
      default:
        HFST_THROW(FunctionNotImplementedException);
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
    return apply(
#if HAVE_SFST
                 &hfst::implementations::SfstTransducer::disjunct,
#endif
#if HAVE_OPENFST
                 &hfst::implementations::TropicalWeightTransducer::disjunct,
                 &hfst::implementations::LogWeightTransducer::disjunct,
#endif
#if HAVE_FOMA
                 &hfst::implementations::FomaTransducer::disjunct,
#endif
                 /* Add here your implementation. */
                 const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::intersect
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
#if HAVE_SFST
                 &hfst::implementations::SfstTransducer::intersect,
#endif
#if HAVE_OPENFST
                 &hfst::implementations::TropicalWeightTransducer::intersect,
                 &hfst::implementations::LogWeightTransducer::intersect,
#endif
#if HAVE_FOMA
                 &hfst::implementations::FomaTransducer::intersect,
#endif
                 /* Add here your implementation. */
                 const_cast<HfstTransducer&>(another)); }

  HfstTransducer &HfstTransducer::subtract
  (const HfstTransducer &another)
  { is_trie = false; // This could be done so that is_trie is preserved
    return apply(
#if HAVE_SFST
                 &hfst::implementations::SfstTransducer::subtract,
#endif
#if HAVE_OPENFST
                 &hfst::implementations::TropicalWeightTransducer::subtract,
                 &hfst::implementations::LogWeightTransducer::subtract,
#endif
#if HAVE_FOMA
                 &hfst::implementations::FomaTransducer::subtract,
#endif
                 /* Add here your implementation. */
                 const_cast<HfstTransducer&>(another)); }




  HfstTransducer &HfstTransducer::convert(const HfstTransducer &t,
                                          ImplementationType type)
  {
    if (type == ERROR_TYPE)
      { 
      HFST_THROW_MESSAGE
        (SpecifiedTypeRequiredException, "HfstTransducer::convert");
      }
    if (type == t.type)
      { return *(new HfstTransducer(t)); }
    if (not is_implementation_type_available(type)) {
      HFST_THROW_MESSAGE
        (ImplementationTypeNotAvailableException, 
         "HfstTransducer::convert");
    }
    hfst::implementations::HfstBasicTransducer net(t);    

    HfstTransducer * retval = new HfstTransducer(net, type);    

    return *retval;
  }


  /* ERROR_TYPE or UNSPECIFIED_TYPE returns true, so they must be handled 
     separately */
  bool HfstTransducer::is_implementation_type_available
  (ImplementationType type) {
#if !HAVE_FOMA
    if (type == FOMA_TYPE)
      return false;
#endif
#if !HAVE_SFST
    if (type == SFST_TYPE)
      return false;
#endif
#if !HAVE_OPENFST
    if (type == TROPICAL_OPENFST_TYPE || type == LOG_OPENFST_TYPE)
      return false;
#endif
       /* Add here your implementation. */
    //#if !HAVE_MY_TRANSDUCER_LIBRARY
    //if (type == MY_TRANSDUCER_LIBRARY_TYPE)
    //  return false;
    //#endif
    (void)type; 
    return true;
  }

  HfstTransducer &HfstTransducer::convert(ImplementationType type)
  {
    if (type == ERROR_TYPE)
      { 
        HFST_THROW_MESSAGE(SpecifiedTypeRequiredException,
                           "HfstTransducer::convert"); }
    if (type == this->type)
      { return *this; }
    if (not is_implementation_type_available(type)) {
      HFST_THROW_MESSAGE(ImplementationTypeNotAvailableException,
                         "HfstTransducer::convert");
    }

    try 
      {
        hfst::implementations::HfstBasicTransducer * internal;
        switch (this->type)
          {
#if HAVE_FOMA
          case FOMA_TYPE:
            internal =
              ConversionFunctions::foma_to_hfst_basic_transducer
              (implementation.foma);
            foma_interface.delete_foma(implementation.foma);
            break;
#endif
       /* Add here your implementation. */
            //#if HAVE_MY_TRANSDUCER_LIBRARY
            //case MY_TRANSDUCER_LIBRARY_TYPE:
            //internal =
            // ConversionFunctions::
            //   my_transducer_library_transducer_to_hfst_basic_transducer
            //     (implementation.my_transducer_library);
            //delete(implementation.my_transducer_library);
            //break;
            //#endif
#if HAVE_SFST
          case SFST_TYPE:
            internal = 
              ConversionFunctions::sfst_to_hfst_basic_transducer
              (implementation.sfst);
            delete implementation.sfst;
            break;
#endif
#if HAVE_OPENFST
          case TROPICAL_OPENFST_TYPE:
            internal =
              ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
              (implementation.tropical_ofst);
            delete implementation.tropical_ofst;
            break;
          case LOG_OPENFST_TYPE:
          internal =
            ConversionFunctions::log_ofst_to_hfst_basic_transducer
            (implementation.log_ofst);
          delete implementation.log_ofst;
            break;
          case HFST_OL_TYPE:
          case HFST_OLW_TYPE:
            internal =
            ConversionFunctions::hfst_ol_to_hfst_basic_transducer
              (implementation.hfst_ol);
            delete implementation.hfst_ol;
            break;
#endif
        case ERROR_TYPE:
        default:
          HFST_THROW(TransducerHasWrongTypeException);
          break;
          }

        this->type = type;
        switch (this->type)
          {
#if HAVE_SFST
          case SFST_TYPE:
            implementation.sfst = 
              ConversionFunctions::hfst_basic_transducer_to_sfst(internal);
            delete internal;
            break;
#endif
       /* Add here your implementation. */
            //#if HAVE_MY_TRANSDUCER_LIBRARY
            //case MY_TRANSDUCER_LIBRARY_TYPE:
            //implementation.my_transducer_library = 
            // ConversionFunctions::
            //   hfst_basic_transducer_to_my_transducer_library_transducer
            //     (internal);
            //delete internal;
            //break;
            //#endif
#if HAVE_OPENFST
          case TROPICAL_OPENFST_TYPE:
            implementation.tropical_ofst =
              ConversionFunctions::hfst_basic_transducer_to_tropical_ofst
              (internal);
            delete internal;
            break;
          case LOG_OPENFST_TYPE:
            implementation.log_ofst =
              ConversionFunctions::hfst_basic_transducer_to_log_ofst(internal);
            delete internal;
            break;
          case HFST_OL_TYPE:
          case HFST_OLW_TYPE:
            implementation.hfst_ol =
              ConversionFunctions::hfst_basic_transducer_to_hfst_ol
              (internal, 
               this->type==HFST_OLW_TYPE?true:false);
            delete internal;
            break;
#endif
#if HAVE_FOMA
          case FOMA_TYPE:
            implementation.foma =
              ConversionFunctions::hfst_basic_transducer_to_foma(internal);
            delete internal;
            break;
#endif
        case ERROR_TYPE:
        default:
          HFST_THROW(TransducerHasWrongTypeException);
          }
      }
    catch (const HfstException e)
      { //throw e; 
        throw e; }
    return *this;
  }


  void HfstTransducer::write_in_att_format
    (const std::string &filename, bool print_weights) const
{
  FILE * ofile = fopen(filename.c_str(), "wb");
  if (ofile == NULL) {
    std::string message(filename);
    HFST_THROW_MESSAGE(StreamCannotBeWrittenException, message);
  }
  write_in_att_format(ofile,print_weights);
  fclose(ofile);
}

void HfstTransducer::write_in_att_format
(FILE * ofile, bool print_weights) const
{
  // Implemented only for internal transducer format.
  hfst::implementations::HfstBasicTransducer net(*this);
  net.write_in_att_format(ofile, print_weights);
}

HfstTransducer::HfstTransducer(FILE * ifile, 
                               ImplementationType type,
                               const std::string &epsilon_symbol):
  type(type),anonymous(false),is_trie(false), name("")
{


  if (not is_implementation_type_available(type))
    HFST_THROW_MESSAGE(ImplementationTypeNotAvailableException,
                       "HfstTransducer::HfstTransducer"
                       "(FILE*, ImplementationType, const std::string&)");

    HfstTokenizer::check_utf8_correctness(epsilon_symbol);

  // Implemented only for internal transducer format.
  hfst::implementations::HfstBasicTransducer net =
    hfst::implementations::HfstTransitionGraph<hfst::implementations::
      HfstTropicalTransducerTransitionData,float>::
        read_in_att_format(ifile, std::string(epsilon_symbol));

  // Conversion is done here.
  switch (type)
      {
#if HAVE_SFST
      case SFST_TYPE:
        implementation.sfst = 
          ConversionFunctions::hfst_basic_transducer_to_sfst(&net);
        break;
#endif
#if HAVE_OPENFST
      case TROPICAL_OPENFST_TYPE:
        implementation.tropical_ofst 
          = ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(&net);
          
        break;
      case LOG_OPENFST_TYPE:
        implementation.log_ofst 
          = ConversionFunctions::hfst_basic_transducer_to_log_ofst(&net);
        break;
#endif
#if HAVE_FOMA
      case FOMA_TYPE:
        implementation.foma = 
          ConversionFunctions::hfst_basic_transducer_to_foma(&net);
        break;
#endif
#if HAVE_HFSTOL
      case HFST_OL_TYPE:
          implementation.hfst_ol 
            = ConversionFunctions::hfst_basic_transducer_to_hfst_ol
            (&net, false);
          break;
      case HFST_OLW_TYPE:
          implementation.hfst_ol 
            = ConversionFunctions::hfst_basic_transducer_to_hfst_ol(&net, true);
          break;
#endif
       /* Add here your implementation. */
        //#if HAVE_MY_TRANSDUCER_LIBRARY
        //case MY_TRANSDUCER_LIBRARY_TYPE:
        //implementation.my_transducer_library = 
        //  ConversionFunctions::
        //    hfst_basic_transducer_to_my_transducer_library_transducer(&net);
        //break;
        //#endif
      case ERROR_TYPE:
        HFST_THROW(SpecifiedTypeRequiredException);
      default:
        HFST_THROW(TransducerHasWrongTypeException);
      }
}

HfstTransducer &HfstTransducer::read_in_att_format
  (const std::string &filename, ImplementationType type, 
   const std::string &epsilon_symbol)
{
  FILE * ifile = fopen(filename.c_str(), "rb");
  if (ifile == NULL) {
    std::string message(filename);
    HFST_THROW_MESSAGE(StreamNotReadableException, message);
  }
  HfstTokenizer::check_utf8_correctness(epsilon_symbol);

  HfstTransducer &retval = read_in_att_format(ifile, type, epsilon_symbol);
  fclose(ifile);
  return retval;
}

HfstTransducer &HfstTransducer::read_in_att_format
  (FILE * ifile, ImplementationType type, const std::string &epsilon_symbol)
{
  if (not is_implementation_type_available(type))
    HFST_THROW_MESSAGE(ImplementationTypeNotAvailableException,
                       "HfstTransducer::read_in_att_format");

  HfstTokenizer::check_utf8_correctness(epsilon_symbol);

  hfst::implementations::HfstBasicTransducer net =
    hfst::implementations::HfstTransitionGraph<hfst::implementations::
      HfstTropicalTransducerTransitionData,float>
        ::read_in_att_format(ifile, std::string(epsilon_symbol));
  HfstTransducer *retval = new HfstTransducer(net,type);
  return *retval;
}


HfstTransducer &HfstTransducer::operator=(const HfstTransducer &another)
{
  // Check for self-assignment.
  if (&another == this)
    { return *this; }
  
  if (this->type != another.type)
    HFST_THROW_MESSAGE
      (TransducerTypeMismatchException, "HfstTransducer::operator=");

  // set some features
  anonymous = another.anonymous;
  is_trie = another.is_trie;
  
  // Delete old transducer.
  switch (this->type)
    {
#if HAVE_FOMA
    case FOMA_TYPE:
      delete implementation.foma;
      break;
#endif
#if HAVE_SFST
    case SFST_TYPE:
      delete implementation.sfst;
      break;
#endif
#if HAVE_OPENFST
    case TROPICAL_OPENFST_TYPE:
      delete implementation.tropical_ofst;
      break;
    case LOG_OPENFST_TYPE:
      delete implementation.log_ofst;
      break;
#endif
       /* Add here your implementation. */
      //#if HAVE_MY_TRANSDUCER_LIBRARY
      //case MY_TRANSDUCER_LIBRARY_TYPE:
      //delete implementation.my_transducer_library;
      //break;
      //#endif
    case ERROR_TYPE:
    default:
      HFST_THROW(TransducerHasWrongTypeException);
    }
    
  // Set new transducer.
  
  // Sfst doesn't have a const copy constructor, so we need to do a
  // const_cast here. Shouldn't be a problem...
  HfstTransducer &another_1 = const_cast<HfstTransducer&>(another);
  type = another.type;
  switch (type)
    {
#if HAVE_FOMA
    case FOMA_TYPE:
      implementation.foma = 
        foma_interface.copy(another_1.implementation.foma);
      break;
#endif
#if HAVE_SFST
    case SFST_TYPE:
      implementation.sfst = 
        sfst_interface.copy(another_1.implementation.sfst);
      break;
#endif
#if HAVE_OPENFST
    case TROPICAL_OPENFST_TYPE:
      implementation.tropical_ofst = 
        tropical_ofst_interface.copy(another_1.implementation.tropical_ofst);
      break;
    case LOG_OPENFST_TYPE:
      implementation.log_ofst = 
        log_ofst_interface.copy(another_1.implementation.log_ofst);
      break;
#endif
      /* Add here your implementation. */
    default:
      (void)1;
    }
  return *this;

}


  HfstTokenizer HfstTransducer::create_tokenizer() 
  {
    HfstTokenizer tok;

    if (this->type == SFST_TYPE) 
      {
        StringPairSet sps = this->get_symbol_pairs();
        for (StringPairSet::const_iterator it = sps.begin();
             it != sps.end(); it++)
          {
            if (it->first.size() > 1)
              tok.add_multichar_symbol(it->first);
            if (it->second.size() > 1)
              tok.add_multichar_symbol(it->second);
          }
      }
    else 
      {
        hfst::implementations::HfstBasicTransducer t(*this);
        t.prune_alphabet();
        StringSet alpha = t.get_alphabet();
        for (StringSet::iterator it = alpha.begin();
             it != alpha.end(); it++)
          {
            if (it->size() > 1)
              tok.add_multichar_symbol(*it);
          }
      }

    return tok;
  }


std::ostream &operator<<(std::ostream &out,const HfstTransducer &t)
  {
    // Implemented only for internal transducer format.
    hfst::implementations::HfstBasicTransducer net(t);
    bool write_weights;
    if (t.type == SFST_TYPE || t.type == FOMA_TYPE)
      write_weights=false;
    else
      write_weights=true;
    net.write_in_att_format(out, write_weights);
    return out;
  }

// test function
void HfstTransducer::print_alphabet()
{
#if HAVE_SFST
  if (this->type == SFST_TYPE)
    this->sfst_interface.print_alphabet(this->implementation.sfst);
#endif
  return;
}


}
