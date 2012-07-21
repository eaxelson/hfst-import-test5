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

#include <string>
#include <map>

using std::string;
using std::map;

#include "HfstTransducer.h"
#include "HfstFlagDiacritics.h"
#include "HfstExceptionDefs.h"
#include "implementations/compose_intersect/ComposeIntersectLexicon.h"

using hfst::implementations::ConversionFunctions;


#ifndef MAIN_TEST

namespace hfst
{


// -----------------------------------------------------------------------
//
//     Interfaces through which the backend implementations are called   
//
// -----------------------------------------------------------------------

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
  hfst::implementations::HfstOlTransducer HfstTransducer::hfst_ol_interface;
/* Add here the interface between HFST and your transducer library. */
//#if HAVE_MY_TRANSDUCER_LIBRARY
//hfst::implementations::MyTransducerLibraryTransducer 
// HfstTransducer::my_transducer_library_interface;
//#endif



// -----------------------------------------------------------------------
//
//                   Testing and optimization functions   
//
// -----------------------------------------------------------------------

/* The default minimization algorithm if Hopcroft. */
MinimizationAlgorithm minimization_algorithm=HOPCROFT;
/* By default, harmonization is not optimized. */
bool harmonize_smaller=true;
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


// -----------------------------------------------------------------------
//
//                   Alphabet and harmonization   
//
// -----------------------------------------------------------------------

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

    if (symbol == "")
      HFST_THROW_MESSAGE(EmptyStringException, "insert_to_alphabet");

    hfst::implementations::HfstBasicTransducer * net 
      = convert_to_basic_transducer();
    net->add_symbol_to_alphabet(symbol);
    convert_to_hfst_transducer(net);
}


void HfstTransducer::remove_from_alphabet(const std::string &symbol) 
{

  HfstTokenizer::check_utf8_correctness(symbol);

  if (symbol == "")
    HFST_THROW_MESSAGE(EmptyStringException, "remove_from_alphabet");

    hfst::implementations::HfstBasicTransducer * net 
      = convert_to_basic_transducer();
    net->remove_symbol_from_alphabet(symbol);
    convert_to_hfst_transducer(net);
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

unsigned int HfstTransducer::get_symbol_number(const std::string &symbol) 
{
  switch(type)
    {
#if HAVE_SFST
    case SFST_TYPE:
      return sfst_interface.get_symbol_number(implementation.sfst, symbol);
#endif
#if HAVE_OPENFST
    case TROPICAL_OPENFST_TYPE:
      return tropical_ofst_interface.get_symbol_number
    (implementation.tropical_ofst, symbol);
    case LOG_OPENFST_TYPE:
      return log_ofst_interface.get_symbol_number(implementation.log_ofst,
                          symbol);
#endif
#if HAVE_FOMA
    case FOMA_TYPE:
      return foma_interface.get_symbol_number(implementation.foma,
                          symbol);
#endif
    case ERROR_TYPE:
      HFST_THROW(TransducerHasWrongTypeException);
    case HFST_OL_TYPE:
    case HFST_OLW_TYPE:
    default:
      HFST_THROW_MESSAGE(FunctionNotImplementedException, 
               "get_symbol_number");
    }    
}

HfstTransducer * HfstTransducer::harmonize_(const HfstTransducer &another)
{
  using namespace implementations;
    if (this->type != another.type) {
        HFST_THROW(TransducerTypeMismatchException); }

    if (this->anonymous && another.anonymous) {
      HFST_THROW_MESSAGE
    (HfstFatalException, "harmonize_ with anonymous transducers"); }

    switch(this->type)
    {
#if HAVE_FOMA
    case (FOMA_TYPE):
      // no need to harmonize as foma's functions take care of harmonizing
      return NULL;
      break;
#endif // HAVE_FOMA
#if HAVE_SFST || HAVE_OPENFST
    case (SFST_TYPE):
    case (TROPICAL_OPENFST_TYPE):
    case (LOG_OPENFST_TYPE):
      {
    HfstBasicTransducer * another_basic = another.get_basic_transducer();
    HfstBasicTransducer * this_basic = this->convert_to_basic_transducer();

    this_basic->harmonize(*another_basic);

    this->convert_to_hfst_transducer(this_basic);
    HfstTransducer * another_harmonized 
      = new HfstTransducer(*another_basic, this->type);
    delete another_basic;

    return another_harmonized;
    break;
      }
#endif
    case (ERROR_TYPE):
    default:
        HFST_THROW(TransducerHasWrongTypeException);
    }

}

/*  Harmonize symbol-to-number encodings and expand unknown and 
    identity symbols. 

    In the case of foma transducers, does nothing because foma's own functions
    take care of harmonizing. If harmonization is needed, 
    FomaTransducer::harmonize can be used instead. */
void HfstTransducer::harmonize(HfstTransducer &another)
{
  using namespace implementations;
    if (this->type != another.type) {
        HFST_THROW(TransducerTypeMismatchException); }

    if (this->anonymous && another.anonymous) {
    return; }

    switch(this->type)
    {
#if HAVE_FOMA
    case (FOMA_TYPE):
        // no need to harmonize as foma's functions take care of harmonizing
        break;
#endif // HAVE_FOMA
#if HAVE_SFST || HAVE_OPENFST
    case (SFST_TYPE):
    case (TROPICAL_OPENFST_TYPE):
    case (LOG_OPENFST_TYPE):
      {
    HfstBasicTransducer * this_basic = this->convert_to_basic_transducer();
    HfstBasicTransducer * another_basic = 
      another.convert_to_basic_transducer();

    this_basic->harmonize(*another_basic);

    this->convert_to_hfst_transducer(this_basic);
    another.convert_to_hfst_transducer(another_basic);
    break;
      }
#endif
    case (ERROR_TYPE):
    default:
        HFST_THROW(TransducerHasWrongTypeException);
    }

    if (false)
      {
    this->write_in_att_format(stderr);
    another.write_in_att_format(stderr);
      }
}

// test function
void HfstTransducer::print_alphabet()
{
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      this->sfst_interface.print_alphabet(this->implementation.sfst);
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      this->tropical_ofst_interface.print_alphabet
    (this->implementation.tropical_ofst);
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
    HfstBasicTransducer net(*this);
    net.print_alphabet();
      }
#endif

    return;
}



// -----------------------------------------------------------------------
//
//                        Lookup functions   
//
// -----------------------------------------------------------------------

HfstOneLevelPaths * HfstTransducer::lookup(const StringVector& s,
                ssize_t limit) const
{
    return lookup_fd(s, limit);
}

HfstOneLevelPaths * HfstTransducer::lookup(const std::string & s,
                       ssize_t limit) const
{
    return lookup_fd(s, limit);
}

HfstOneLevelPaths * HfstTransducer::lookup_fd(const StringVector& s,
                          ssize_t limit) const
{
    switch(this->type) {

        /* TODO: Convert into HFST_OL(W)_TYPE, if needed? */

    case (HFST_OL_TYPE):
    case (HFST_OLW_TYPE):
    return this->implementation.hfst_ol->lookup_fd(s);

    case (ERROR_TYPE):
    HFST_THROW(TransducerHasWrongTypeException);
    default:
    (void)s;
    (void)limit;
    HFST_THROW(FunctionNotImplementedException);
    }
}

HfstOneLevelPaths * HfstTransducer::lookup_fd(const std::string & s,
                         ssize_t limit) const
{
    switch(this->type) {

        /* TODO: Convert into HFST_OL(W)_TYPE, if needed? */

    case (HFST_OL_TYPE):
    case (HFST_OLW_TYPE):
    return this->implementation.hfst_ol->lookup_fd(s);

    case (ERROR_TYPE):
    HFST_THROW(TransducerHasWrongTypeException);
    default:
    (void)s;
    (void)limit;
    HFST_THROW(FunctionNotImplementedException);
    }
}

HfstOneLevelPaths * HfstTransducer::lookup(const HfstTokenizer& tok,
                       const std::string &s, 
                       ssize_t limit) const
{
    StringVector sv = tok.tokenize_one_level(s);
    return lookup(sv, limit);
}

HfstOneLevelPaths * HfstTransducer::lookdown(const StringVector& s,
                         ssize_t limit) const {
    (void)s;
    (void)limit;
    HFST_THROW(FunctionNotImplementedException);
}

HfstOneLevelPaths * HfstTransducer::lookdown_fd(StringVector& s,
                        ssize_t limit) const {
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


// -----------------------------------------------------------------------
//
//              Transducer constructors and destructor   
//
// -----------------------------------------------------------------------

HfstTransducer::HfstTransducer():
    type(UNSPECIFIED_TYPE),anonymous(false),is_trie(true), name("")
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

    if (utf8_str == "")
      HFST_THROW_MESSAGE
    (EmptyStringException, 
     "HfstTransducer(const std::string&, const HfstTokenizer&,"
     " ImplementationType)");

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
    
    for (StringPairVector::const_iterator it = spv.begin();
     it != spv.end(); it++)
      {
    if (it->first == "" || it->second == "")
      HFST_THROW_MESSAGE
        (EmptyStringException, 
         "HfstTransducer(const StringPairVector&, ImplementationType)");
      }

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

    for (StringPairSet::const_iterator it = sps.begin();
     it != sps.end(); it++)
      {
    if (it->first == "" || it->second == "")
      HFST_THROW_MESSAGE
        (EmptyStringException, 
         "HfstTransducer(const StringPairSet&, ImplementationType, bool)");
      }

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

    for (std::vector<StringPairSet>::const_iterator it = spsv.begin();
     it != spsv.end(); it++)
      {
    for (StringPairSet::const_iterator IT = it->begin();
         IT != it->end(); IT++)
      {
        if (IT->first == "" || IT->second == "")
          HFST_THROW_MESSAGE
        (EmptyStringException, 
         "HfstTransducer(const std::vector<StringPairSet>&, "
         " ImplementationType)");
      }
      }

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

    if (upper_utf8_str == "" || 
    lower_utf8_str == "")
      HFST_THROW_MESSAGE
    (EmptyStringException,
     "HfstTransducer(const std::string&, const std::string&, "
     "const HfstTokenizer&, ImplementationType");  

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
    for (map<string,string>::const_iterator prop = another.props.begin();
         prop != another.props.end();
         ++prop)
      {
        if ((prop->first != "type"))
          {
            this->props[prop->first] = prop->second;
          }
      }
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

    if (symbol == "")
      HFST_THROW_MESSAGE
    (EmptyStringException, 
     "HfstTransducer(const std::string&, ImplementationType)");

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

    if (isymbol == "" || osymbol == "")
      HFST_THROW_MESSAGE
    (EmptyStringException, 
     "HfstTransducer(const std::string&, const std::string&, "
     " ImplementationType)");

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


// -----------------------------------------------------------------------
//
//                     Transducer accessors
//
// -----------------------------------------------------------------------

ImplementationType HfstTransducer::get_type(void) const {
    return this->type; }
void HfstTransducer::set_name(const std::string &name) {
    this->set_property("name", name);
}   
std::string HfstTransducer::get_name() const {
    return this->get_property("name"); }

void
HfstTransducer::set_property(const string& property, const string& name)
  {
    HfstTokenizer::check_utf8_correctness(name);
    this->props[property] = name;
    if (property == "name")
      {
        this->name = name;
      }
  }

string HfstTransducer::get_property(const string& property) const
  {
    if (this->props.find(property) != this->props.end())
      {
        return this->props.find(property)->second;
      }
    else
      {
        return "";
      }
  }
const std::map<string,string>&
HfstTransducer::get_properties() const
  {
    return this->props;
  }

// -----------------------------------------------------------------------
//
//                     Properties of a transducer
//
// -----------------------------------------------------------------------

bool HfstTransducer::compare(const HfstTransducer &another) const
{
    if (this->type != another.type)
    HFST_THROW_MESSAGE(TransducerTypeMismatchException, 
               "HfstTransducer::compare");

    HfstTransducer one_copy(*this);
    HfstTransducer another_copy(another);
    one_copy.harmonize(another_copy);
    one_copy.minimize();

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


// -----------------------------------------------------------------------
//
//              Epsilon removal, determinization, minimization
//
// -----------------------------------------------------------------------

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



// -----------------------------------------------------------------------
//
//                        Repeat functions
//
// -----------------------------------------------------------------------

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



// -----------------------------------------------------------------------
//
//                      Unary operators
//
// -----------------------------------------------------------------------

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



// -----------------------------------------------------------------------
//
//                        Extract paths
//
// -----------------------------------------------------------------------

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

void HfstTransducer::extract_random_paths
(HfstTwoLevelPaths &results, int max_num) const
{
  switch (this->type)
    {
#if HAVE_OPENFST
    case TROPICAL_OPENFST_TYPE:
    {
      this->tropical_ofst_interface.extract_random_paths
    (this->implementation.tropical_ofst, results, max_num);
    }
    break;
    case LOG_OPENFST_TYPE:
    {
      this->log_ofst_interface.extract_random_paths
    (this->implementation.log_ofst, results, max_num);
    }
    break;
#endif
#if HAVE_SFST
    case SFST_TYPE:
    {
      this->sfst_interface.extract_random_paths
    (this->implementation.sfst, results, max_num);
    }
    break;
#endif
#if HAVE_FOMA
    case FOMA_TYPE:
    {
      this->foma_interface.extract_random_paths
    (this->implementation.foma, results, max_num);
    }
    break;
#endif
    /* Add here your implementation. */
    case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
    case HFST_OL_TYPE:
    case HFST_OLW_TYPE:
    default:
      HFST_THROW(FunctionNotImplementedException);
    }
}

void HfstTransducer::extract_random_paths_fd
(HfstTwoLevelPaths &results, int max_num, bool filter_fd) const
{
  (void)results;
  (void)max_num;
  (void)filter_fd;
  HFST_THROW(FunctionNotImplementedException);
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




// -----------------------------------------------------------------------
//
//                        Flag diacritics
//
// -----------------------------------------------------------------------

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

bool has_flags(const HfstTransducer &fst)
{
  StringSet alphabet = fst.get_alphabet();
  for (StringSet::const_iterator it = alphabet.begin();
       it != alphabet.end(); it++)
    {
      if (FdOperation::is_diacritic(*it))
	{ return true; }
    }
  return false;	
}

bool HfstTransducer::has_flag_diacritics(void) const
{ return has_flags(*this); }

std::string add_suffix_to_feature_name(const std::string &flag_diacritic,
				       const std::string &suffix)
{
  return 
    "@" +
    FdOperation::get_operator(flag_diacritic) +
    "." +
    FdOperation::get_feature(flag_diacritic) + suffix +

    (FdOperation::has_value(flag_diacritic) ?
     "." +
     FdOperation::get_value(flag_diacritic) :
     "") +

    "@";
}

void rename_flag_diacritics(HfstTransducer &fst,const std::string &suffix)
{
  
  HfstBasicTransducer basic_fst(fst);
  HfstBasicTransducer basic_fst_copy;
  (void)basic_fst_copy.add_state(basic_fst.get_max_state());

  hfst::implementations::HfstState s = 0;

  for (HfstBasicTransducer::const_iterator it = basic_fst.begin();
       it != basic_fst.end();
       ++it)
    {
      for (HfstBasicTransducer::HfstTransitions::const_iterator jt = 
	     it->begin();
	   jt != it->end();
	   ++jt)
	{
	  basic_fst_copy.add_transition
	    (s,
	     HfstBasicTransition
	     (jt->get_target_state(),

	      FdOperation::is_diacritic(jt->get_input_symbol())  ?
	      add_suffix_to_feature_name(jt->get_input_symbol(),suffix) :
	      jt->get_input_symbol(),

	      FdOperation::is_diacritic(jt->get_output_symbol())  ?
	      add_suffix_to_feature_name(jt->get_output_symbol(),suffix) :
	      jt->get_output_symbol(),

	      jt->get_weight()));
	}

      if (basic_fst.is_final_state(s))
	{ basic_fst_copy.set_final_weight(s,basic_fst.get_final_weight(s)); }

      ++s;
    }
  fst = HfstTransducer(basic_fst_copy,fst.get_type());
}

void HfstTransducer::harmonize_flag_diacritics(HfstTransducer &another,
					       bool insert_renamed_flags)
{
  bool this_has_flag_diacritics    = has_flags(*this);
  bool another_has_flag_diacritics = has_flags(another);

  if (this_has_flag_diacritics and another_has_flag_diacritics)
    {
      rename_flag_diacritics(*this,"_1");
      rename_flag_diacritics(another,"_2");
      if (insert_renamed_flags)
	{
	  this->insert_freely_missing_flags_from(another);
	  another.insert_freely_missing_flags_from(*this);
	}
    }
  else if (this_has_flag_diacritics and insert_renamed_flags)
    { another.insert_freely_missing_flags_from(*this); }
  else if (another_has_flag_diacritics and insert_renamed_flags)
    { this->insert_freely_missing_flags_from(another); }
}

bool HfstTransducer::check_for_missing_flags_in
(const HfstTransducer &another) const
{
    StringSet foo; /* An obligatory argument that is not used. */
    return check_for_missing_flags_in(another, foo, 
                                      true /* return on first miss */);
}

// -----------------------------------------------------------------------
//
//                        Insert freely
//
// -----------------------------------------------------------------------

HfstTransducer &HfstTransducer::insert_freely
(const StringPair &symbol_pair)
{
    HfstTokenizer::check_utf8_correctness(symbol_pair.first);
    HfstTokenizer::check_utf8_correctness(symbol_pair.second);

    if (symbol_pair.first == "" || symbol_pair.second == "")
      HFST_THROW_MESSAGE
    (EmptyStringException, 
     "insert_freely(const StringPair&)");

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
      // HfstTransducer::harmonize does nothing to foma transducers, 
      // because foma functions take care of harmonization.
      // However, now we are using HfstBasicTransducer.
      this->foma_interface.harmonize(this->implementation.foma,
                     tmp.implementation.foma);
        hfst::implementations::HfstBasicTransducer * net = 
        ConversionFunctions::foma_to_hfst_basic_transducer
        (implementation.foma);
        this->foma_interface.delete_foma(implementation.foma);
        net->insert_freely
      (StringPair(symbol_pair.first, symbol_pair.second), 0);
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
    HfstTransducer * tr_harmonized = this->harmonize_(tr);
    harmonize_smaller=harm;

    if (tr_harmonized == NULL) { // foma
      tr_harmonized = new HfstTransducer(tr);
    }

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
            (tr_harmonized->implementation.tropical_ofst);
    delete tr_harmonized;
          
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
            (tr_harmonized->implementation.log_ofst);
    delete tr_harmonized;

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
            (implementation.foma,tr_harmonized->implementation.foma);

    hfst::implementations::HfstBasicTransducer * net = 
            ConversionFunctions::foma_to_hfst_basic_transducer
            (implementation.foma);
    this->foma_interface.delete_foma(implementation.foma);
          
    hfst::implementations::HfstBasicTransducer * substituting_net = 
            ConversionFunctions::foma_to_hfst_basic_transducer
            (tr_harmonized->implementation.foma);
    delete tr_harmonized;

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
            (tr_harmonized->implementation.sfst);
    delete tr_harmonized;

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



// -----------------------------------------------------------------------
//
//                        Substitution functions
//
// -----------------------------------------------------------------------

HfstTransducer &HfstTransducer::substitute
(bool (*func)(const StringPair &sp, StringPairSet &sps))
{
  hfst::implementations::HfstBasicTransducer * net 
    = convert_to_basic_transducer();
  net->substitute(func);
  return convert_to_hfst_transducer(net);
}

HfstTransducer &HfstTransducer::substitute
(const std::string &old_symbol, const std::string &new_symbol,
 bool input_side, bool output_side)
{
  // empty strings are not accepted
  if (old_symbol == "" || new_symbol == "")
    HFST_THROW_MESSAGE
      (EmptyStringException, 
       "substitute(const std::string&, const std::string&, bool, bool)");
  
  // if there are implementations available, use them

#if HAVE_SFST
  if (this->type == SFST_TYPE && input_side && output_side)
    {
      hfst::implementations::Transducer * tmp =
    this->sfst_interface.substitute
    (implementation.sfst, old_symbol, new_symbol);
      delete implementation.sfst;
      implementation.sfst = tmp;
      return *this;
    }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE && input_side && output_side)
      {
    fst::StdVectorFst * tmp =
      this->tropical_ofst_interface.substitute
      (implementation.tropical_ofst, old_symbol, new_symbol);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tmp;
    return *this;
      }
    if (this->type == LOG_OPENFST_TYPE && input_side && output_side)
      {
    hfst::implementations::LogFst * tmp =
      this->log_ofst_interface.substitute
      (implementation.log_ofst, old_symbol, new_symbol);
    delete implementation.log_ofst;
    implementation.log_ofst = tmp;
    return *this;
      }
#endif
    
    // use the default HfstBasicTransducer function
    hfst::implementations::HfstBasicTransducer * net =
      convert_to_basic_transducer();
    net->substitute(old_symbol,
		    new_symbol,
		    input_side, output_side);
    return convert_to_hfst_transducer(net);
}


HfstTransducer &HfstTransducer::substitute
(const StringPair &old_symbol_pair, 
 const StringPair &new_symbol_pair)
{ 
  // empty strings are not accepted
  if (old_symbol_pair.first == "" || old_symbol_pair.second == "" ||
      new_symbol_pair.first == "" || new_symbol_pair.second == "")
    HFST_THROW_MESSAGE
      (EmptyStringException, 
       "substitute(const StringPair&, const StringPair&)");
  
  hfst::implementations::HfstBasicTransducer * net 
    = convert_to_basic_transducer();
  net->substitute(old_symbol_pair, new_symbol_pair);
  convert_to_hfst_transducer(net);
  return *this;
}

HfstTransducer &HfstTransducer::substitute
(const StringPair &old_symbol_pair, 
 const StringPairSet &new_symbol_pair_set)
{ 
  if(old_symbol_pair.first == "" || old_symbol_pair.second == "")
    HFST_THROW_MESSAGE
      (EmptyStringException, 
       "substitute(const StringPair&, const StringPairSet&");

  hfst::implementations::HfstBasicTransducer * net 
    = convert_to_basic_transducer();
  net->substitute(old_symbol_pair, new_symbol_pair_set);
  return convert_to_hfst_transducer(net);  

}

HfstTransducer &HfstTransducer::substitute
(const HfstSymbolSubstitutions &substitutions)
{
  hfst::implementations::HfstBasicTransducer * net 
    = convert_to_basic_transducer();

  try  {
    net->substitute(substitutions);
  } 
  catch (const FunctionNotImplementedException & e) {
    for (HfstSymbolSubstitutions::const_iterator it =
	   substitutions.begin(); it != substitutions.end(); it++) {
      net->substitute(it->first, it->second, true, true);
    }
  }
  
  return convert_to_hfst_transducer(net);  
}

HfstTransducer &HfstTransducer::substitute
(const HfstSymbolPairSubstitutions &substitutions)
{ 
  hfst::implementations::HfstBasicTransducer * net 
    = convert_to_basic_transducer();
  net->substitute(substitutions);
  return convert_to_hfst_transducer(net);  
}


HfstTransducer &HfstTransducer::substitute
(const StringPair &symbol_pair,
 HfstTransducer &transducer)
{ 

    if (this->type != transducer.type) {
    HFST_THROW_MESSAGE(TransducerTypeMismatchException,
               "HfstTransducer::substitute"); }

    if (symbol_pair.first == "" || symbol_pair.second == "")
      HFST_THROW_MESSAGE
    (EmptyStringException, 
     "substitute(const StringPair&, HfstTransducer&)");

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


// -----------------------------------------------------------------------
//
//                        Weight handling
//
// -----------------------------------------------------------------------

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




// -----------------------------------------------------------------------
//
//                        Binary operators
//
// -----------------------------------------------------------------------

bool substitute_single_identity_with_the_other_symbol
(const StringPair &sp, StringPairSet &sps)
{
    std::string isymbol = sp.first;
    std::string osymbol = sp.second;
    
    if (isymbol.compare("@_IDENTITY_SYMBOL_@") == 0 && 
        (osymbol.compare("@_IDENTITY_SYMBOL_@") != 0)) {
    isymbol = std::string("@_UNKNOWN_SYMBOL_@");
    sps.insert(StringPair(isymbol, osymbol));
    return true;
    }
    else if (osymbol.compare("@_IDENTITY_SYMBOL_@") == 0 && 
             (isymbol.compare("@_IDENTITY_SYMBOL_@") != 0)) {
    osymbol = std::string("@_UNKNOWN_SYMBOL_@");
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


HfstTransducer &HfstTransducer::compose
(const HfstTransducer &another)
{ is_trie = false;

    if (this->type != another.type)
    HFST_THROW_MESSAGE(HfstTransducerTypeMismatchException,
               "HfstTransducer::compose");

    HfstTransducer * another_harmonized =
      this->harmonize_(const_cast<HfstTransducer&>(another));
    if (another_harmonized == NULL) { // foma
      another_harmonized = new HfstTransducer(another);
    }

    // Handle special symbols here.
    if ( (this->type != FOMA_TYPE) && unknown_symbols_in_use) 
    {
      // comment...
      this->substitute("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",false,true);
      const_cast<HfstTransducer*>(another_harmonized)->substitute
    ("@_IDENTITY_SYMBOL_@","@_UNKNOWN_SYMBOL_@",true,false);
    }
    
    switch (this->type)
    {
#if HAVE_SFST
    case SFST_TYPE:
    {
    SFST::Transducer * sfst_temp =
      this->sfst_interface.compose
      (implementation.sfst,another_harmonized->implementation.sfst);
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
         another_harmonized->implementation.tropical_ofst);
    delete implementation.tropical_ofst;
    implementation.tropical_ofst = tropical_ofst_temp;
    break;
    }
    case LOG_OPENFST_TYPE:
    {
    hfst::implementations::LogFst * log_ofst_temp =
            this->log_ofst_interface.compose
      (implementation.log_ofst,
       another_harmonized->implementation.log_ofst);
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
            (implementation.foma,another_harmonized->implementation.foma);
    this->foma_interface.delete_foma(implementation.foma);
    implementation.foma = foma_temp;
    break;
    }
#endif
    #if HAVE_HFSTOL
    case HFST_OL_TYPE:
    case HFST_OLW_TYPE:
	HFST_THROW(HfstTransducerTypeMismatchException);
	// This is the exception the tool wants to hear
    #endif
    case ERROR_TYPE:
        HFST_THROW(TransducerHasWrongTypeException);
    default:
        HFST_THROW(FunctionNotImplementedException);
    }

    if ( (this->type != FOMA_TYPE) && unknown_symbols_in_use) 
    {
        // comment...
        this->substitute(&substitute_single_identity_with_the_other_symbol);
        (const_cast<HfstTransducer*>(another_harmonized))->
       substitute(&substitute_unknown_identity_pairs);
    }
    delete another_harmonized;

    return *this;
}



HfstTransducer &HfstTransducer::cross_product( const HfstTransducer &another )
{

    if ( this->type != another.type )
    {
        HFST_THROW_MESSAGE(HfstTransducerTypeMismatchException, "HfstTransducer::cross_product");
    }

    HfstTransducer automata1(*this);
    HfstTransducer automata2(another);

	// Check if both input transducers are automata
	HfstTransducer t1_proj(automata1);
	t1_proj.input_project();
	HfstTransducer t2_proj(automata2);
	t2_proj.input_project();
	if ( not t1_proj.compare(automata1) || not t2_proj.compare(automata2) )
	{
		HFST_THROW(ContextTransducersAreNotAutomataException);
	}

	// Put MARK all over lower part of automata1 and upper part of automata2,
	// and then compose them
	// Also, there should be created padding after strings, on both sides

	automata1.insert_to_alphabet("@_MARK_@");
	automata2.insert_to_alphabet("@_MARK_@");

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
	TOK.add_multichar_symbol("@_MARK_@");


	// EpsilonToMark and MarkToEpsilon are paddings (if strings are not the same size)
	HfstTransducer UnknownToMark("@_UNKNOWN_SYMBOL_@", "@_MARK_@", TOK, type);
	HfstTransducer EpsilonToMark("@_EPSILON_SYMBOL_@", "@_MARK_@", TOK, type);

	HfstTransducer MarkToUnknown(UnknownToMark);
	MarkToUnknown.invert();
	HfstTransducer MarkToEpsilon(EpsilonToMark);
	MarkToEpsilon.invert();

	UnknownToMark.repeat_star().minimize();
	EpsilonToMark.repeat_star().minimize();
	MarkToUnknown.repeat_star().minimize();
	MarkToEpsilon.repeat_star().minimize();

	HfstTransducer a1(automata1);
	a1.compose(UnknownToMark).minimize().concatenate(EpsilonToMark).minimize();

	HfstTransducer b1(MarkToUnknown);
	b1.compose(automata2).minimize().concatenate(MarkToEpsilon).minimize();

	HfstTransducer retval(a1);
	retval.compose(b1).minimize();

	retval.remove_from_alphabet("@_MARK_@");

	*this = retval;
	return *this;

}


HfstTransducer &HfstTransducer::priority_union (const HfstTransducer &another)
{
    bool DEBUG = false;

    if ( this->type != another.type )
    {
        HFST_THROW_MESSAGE(HfstTransducerTypeMismatchException, 
                   "HfstTransducer::priority_union");
    }
    HfstTransducer tmp(*this);
    HfstTransducer filter(another);
    HfstTransducer final(*this);

    if ( DEBUG )
    {
        printf("--t1--\n");
        tmp.write_in_att_format(stdout, 1);
        printf("--t2--\n");
        filter.write_in_att_format(stdout, 1);
    }

    /*
     * Invert t1, compose it with t2, invert back.
     * Compose t2 with those to get the pairs which need to be 
     * filtered from t1.
     * Subtract those pairs from t1 and union filtered t1 with t2.
    */

    tmp.invert();

    if ( DEBUG )
      {
        fprintf(stderr, "--tmp--\n");
        tmp.write_in_att_format(stdout, 1);
        fprintf(stderr, "--another--\n");
        another.write_in_att_format(stdout, 1);
      }

    tmp.compose(another);
    tmp.minimize();

    if ( DEBUG )
      {
        fprintf(stderr, "--tmp after composition--\n");
        tmp.write_in_att_format(stdout, 1);
        std::cerr << "\n\n";
      }

    tmp.invert();

    filter.compose(tmp);    // t2 compose changed t1
    filter.minimize();
    if ( DEBUG )
    {
        printf("Filter:\n");
        filter.write_in_att_format(stdout, 1);
    }
    final.subtract(filter); // subtract filter from t1
    final.minimize();
    if ( DEBUG )
    {
        printf("Subtract filter from t1:\n");
        final.write_in_att_format(stdout, 1);
    }
    final.disjunct(another);    // union filtered t1 with t2
    final.minimize();
    if ( DEBUG )
    {
        printf("Final result:\n");
        final.write_in_att_format(stdout, 1);
    }
    *this = final;

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
    (void)remove_word_boundary;
    HfstTokenizer tokenizer;
    tokenizer.add_multichar_symbol("@#@");
    tokenizer.add_multichar_symbol(internal_epsilon);
    HfstTransducer wb(internal_epsilon,"@#@",tokenizer,type);
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
    { 
rules = new implementations::ComposeIntersectRulePair
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
        const_cast<HfstTransducer&>(another));
}



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


// -----------------------------------------------------------------------
//
//                       Conversion functions
//
// -----------------------------------------------------------------------

implementations::HfstBasicTransducer * HfstTransducer::
get_basic_transducer() const
{
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::sfst_to_hfst_basic_transducer
      (implementation.sfst);
    return net;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
	        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
      (implementation.tropical_ofst);
    return net;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::log_ofst_to_hfst_basic_transducer
      (implementation.log_ofst);
    return net;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::foma_to_hfst_basic_transducer
      (implementation.foma);
    return net;
      }
#endif
    if (this->type == ERROR_TYPE) {
    HFST_THROW(TransducerHasWrongTypeException);
    }
    HFST_THROW(FunctionNotImplementedException);
}


implementations::HfstBasicTransducer * HfstTransducer::
convert_to_basic_transducer()
{
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::sfst_to_hfst_basic_transducer
      (implementation.sfst);
        delete implementation.sfst;
    return net;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::tropical_ofst_to_hfst_basic_transducer
      (implementation.tropical_ofst);
        delete implementation.tropical_ofst;
    return net;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::log_ofst_to_hfst_basic_transducer
      (implementation.log_ofst);
        delete implementation.log_ofst;
    return net;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        hfst::implementations::HfstBasicTransducer * net = 
      ConversionFunctions::foma_to_hfst_basic_transducer
      (implementation.foma);
        free(implementation.foma);
    return net;
      }
#endif
    if (this->type == ERROR_TYPE) {
    HFST_THROW(TransducerHasWrongTypeException);
    }
    HFST_THROW(FunctionNotImplementedException);
}


HfstTransducer &HfstTransducer::
convert_to_hfst_transducer(implementations::HfstBasicTransducer *t)
{
#if HAVE_SFST
    if (this->type == SFST_TYPE)
      {
        implementation.sfst = 
        ConversionFunctions::hfst_basic_transducer_to_sfst(t);
        delete t;
    return *this;
      }
#endif
#if HAVE_OPENFST
    if (this->type == TROPICAL_OPENFST_TYPE)
      {
        implementation.tropical_ofst = 
        ConversionFunctions::hfst_basic_transducer_to_tropical_ofst(t);
        delete t;
    return *this;
      }
    if (this->type == LOG_OPENFST_TYPE)
      {
        implementation.log_ofst = 
        ConversionFunctions::hfst_basic_transducer_to_log_ofst(t);
        delete t;
    return *this;
      }
#endif
#if HAVE_FOMA
    if (this->type == FOMA_TYPE)
      {
        implementation.foma = 
        ConversionFunctions::hfst_basic_transducer_to_foma(t);
        delete t;
    return *this;
      }
#endif
    if (this->type == ERROR_TYPE) {
    HFST_THROW(TransducerHasWrongTypeException);
    }
    HFST_THROW(FunctionNotImplementedException);
}

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

HfstTransducer &HfstTransducer::convert(ImplementationType type,
                    std::string options)
{
  if (not is_implementation_type_available(this->type)) {
    HFST_THROW_MESSAGE(HfstFatalException,
		       "HfstTransducer::convert: the original type "
		       "of the transducer is not available!");
  }

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

    hfst::implementations::HfstBasicTransducer * internal=NULL;
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
      assert(internal != NULL);
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
	(internal, this->type==HFST_OLW_TYPE?true:false, options);
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

void HfstTransducer::write_in_att_format_number
(FILE * ofile, bool print_weights) const
{
  hfst::implementations::HfstBasicTransducer net(*this);
  net.write_in_att_format_number(ofile, print_weights);
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
    HfstTropicalTransducerTransitionData>::
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
    HfstTropicalTransducerTransitionData>
        ::read_in_att_format(ifile, std::string(epsilon_symbol));
    HfstTransducer *retval = new HfstTransducer(net,type);
    return *retval;
}



// -----------------------------------------------------------------------
//
//                     Miscellaneous functions   
//
// -----------------------------------------------------------------------


HfstTransducer HfstTransducer::universal_pair( ImplementationType type )
{
	using namespace implementations;
	HfstBasicTransducer bt;
	bt.add_transition(0, HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0) );
	bt.add_transition(0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 0) );
	bt.add_transition(0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "@_EPSILON_SYMBOL_@", 0) );
	bt.add_transition(0, HfstBasicTransition(1, "@_EPSILON_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 0) );
	bt.set_final_weight(1, 0);

	HfstTransducer Retval(bt, type);

    return Retval;
}

HfstTransducer HfstTransducer::identity_pair( ImplementationType type )
{
	using namespace implementations;
	HfstBasicTransducer bt;
	bt.add_transition(0, HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0) );
	bt.set_final_weight(1, 0);

	HfstTransducer Retval(bt, type);

	return Retval;
}

HfstTransducer &HfstTransducer::operator=(const HfstTransducer &another)
{
    // Check for self-assignment.
    if (&another == this)
    { return *this; }
  
    if (this->type != UNSPECIFIED_TYPE && 
    this->type != another.type) {
      HFST_THROW_MESSAGE
    (TransducerTypeMismatchException, "HfstTransducer::operator=");
    }

    // set some features
    anonymous = another.anonymous;
    is_trie = another.is_trie;
    this->set_name(another.get_name());

    // Delete old transducer.
    switch (this->type)
    {
#if HAVE_FOMA
    case FOMA_TYPE:
      free(implementation.foma);
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
    case HFST_OL_TYPE:
    case HFST_OLW_TYPE:
      //HFST_THROW_MESSAGE(FunctionNotImplementedException, 
      //	       "HfstTransducer::operator= for type HFST_OL(W)_TYPE");
      delete implementation.hfst_ol;
      break;
    /* Add here your implementation. */
    //#if HAVE_MY_TRANSDUCER_LIBRARY
    //case MY_TRANSDUCER_LIBRARY_TYPE:
    //delete implementation.my_transducer_library;
    //break;
    //#endif
    case UNSPECIFIED_TYPE:
      break;
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
    case HFST_OL_TYPE:
      implementation.hfst_ol 
	= another_1.implementation.hfst_ol->
	copy(another_1.implementation.hfst_ol, false);
      break;
    case HFST_OLW_TYPE:
      implementation.hfst_ol 
	= another_1.implementation.hfst_ol->
	copy(another_1.implementation.hfst_ol, true);
      break;
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

HfstTransducer * HfstTransducer::read_lexc(const std::string &filename,
                       ImplementationType type)
{
  (void)filename;
  if (not is_implementation_type_available(type))
    HFST_THROW(ImplementationTypeNotAvailableException);
  
  HfstTransducer * retval = new HfstTransducer();
  
  switch (type)
    {
#if HAVE_FOMA
    case FOMA_TYPE:
      retval->implementation.foma = foma_interface.read_lexc(filename);
      retval->type=FOMA_TYPE;
      break;
#endif
#if HAVE_SFST
    case SFST_TYPE:
#endif
#if HAVE_OPENFST
    case TROPICAL_OPENFST_TYPE:
    case LOG_OPENFST_TYPE:
#endif
#if HAVE_SFST || HAVE_OPENFST
      {
    hfst::lexc::LexcCompiler compiler(type);
    compiler.parse(filename.c_str());
    return compiler.compileLexical();
    break;
      }
#endif
    /* Add here your implementation. */
    //#if HAVE_MY_TRANSDUCER_LIBRARY
    //case MY_TRANSDUCER_LIBRARY_TYPE:
    // ...
    //break;
    //#endif
    case ERROR_TYPE:
    default:
      HFST_THROW(TransducerHasWrongTypeException);
    }
  return retval;
}

std::ostream & operator<<
(std::ostream &out, const HfstTransducer & t)
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

}

#else // MAIN_TEST was defined

#include <iostream>
using namespace hfst;
using namespace implementations;

// Cross product unit tests
void cross_product_subtest1( ImplementationType type )
{
	HfstTokenizer TOK;

	HfstTransducer tmp1("dog", TOK, type);
	HfstTransducer tmp2("cat", TOK, type);
	HfstTransducer input1(tmp1);
	input1.disjunct(tmp2).minimize();

	HfstTransducer tmp11("chien", TOK, type);
	HfstTransducer tmp22("chat", TOK, type);
	HfstTransducer input2(tmp11);
	input2.disjunct(tmp22).minimize();


	HfstTransducer cp(input1);
	cp.cross_product(input2);

	HfstTransducer r1("cat", "chien", TOK, type);
	HfstTransducer r2("cat", "chat", TOK, type);
	HfstTransducer r3("dog", "chien", TOK, type);
	HfstTransducer r4("dog", "chat", TOK, type);
	HfstTransducer result(r1);
	result.disjunct(r2).disjunct(r3).disjunct(r4).minimize();

	assert(cp.compare(result));
}

void cross_product_subtest2( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");

	HfstTransducer input1( HfstTransducer::identity_pair(type) );

	HfstTransducer input2("a", TOK, type);

	HfstTransducer cp(input1);
	cp.cross_product(input2);


	HfstTransducer r1("a", TOK, type);
	HfstTransducer r2("@_UNKNOWN_SYMBOL_@", "a", TOK, type);
	HfstTransducer result(r1);
	result.disjunct(r2).minimize();
	assert(cp.compare(result));
}

void cross_product_subtest3( ImplementationType type )
{
	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_UNKNOWN_SYMBOL_@");
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	HfstTransducer input1( HfstTransducer::identity_pair(type) );
	input1.repeat_star().minimize();

	HfstTransducer input2("a", TOK, type);

	HfstTransducer cp(input1);
	cp.cross_product(input2);


	HfstTransducer r1("a", TOK, type);
	HfstTransducer r2("@_UNKNOWN_SYMBOL_@", "a", TOK, type);
	HfstTransducer r3("a", "@_EPSILON_SYMBOL_@", TOK, type);
	HfstTransducer r4("@_UNKNOWN_SYMBOL_@", "@_EPSILON_SYMBOL_@", TOK, type);
	HfstTransducer r5("@_EPSILON_SYMBOL_@", "a", TOK, type);
	r3.disjunct(r4).minimize().repeat_star();
	r1.disjunct(r2).concatenate(r3).minimize();

	HfstTransducer result(r5);
	result.disjunct(r1).minimize();
	assert(cp.compare(result));
}
void cross_product_subtest4( ImplementationType type )
{

	HfstTokenizer TOK;
	TOK.add_multichar_symbol("@_EPSILON_SYMBOL_@");

	HfstTransducer input1("b", TOK, type);
	HfstTransducer input2("a", TOK, type);
	input2.repeat_star().minimize();

	HfstTransducer cp(input1);
	cp.cross_product(input2);

	HfstTransducer r1("b", "a", TOK, type);
	HfstTransducer r2("@_EPSILON_SYMBOL_@", "a", TOK, type);
	r2.repeat_star().minimize();
	r1.concatenate(r2);
	HfstTransducer result("b", "@_EPSILON_SYMBOL_@", TOK, type);
	result.disjunct(r1).minimize();

	assert(cp.compare(result));
}


// Priority union unit tests
void priority_union_test ( ImplementationType type )
{
    HfstBasicTransducer		btEmpty,
    						btEmptyString,
    						epsilon,
    						bt1,
    						bt2,
    						bt3,
    						bt2withoutPriority,
    						btIdentity,
    						btUnknown,
    						btEpsilon,
    						btResult1,
    						btResult2,
    						btResult3,
    						btResult4,
    						btResult5,
    						btResult6,
    						btResult7;

    // Empty string
    btEmptyString.set_final_weight(0, 3);

    // First test transducer
    bt1.add_transition(0, HfstBasicTransition(1, "a", "a", 1) );
    bt1.add_transition(0, HfstBasicTransition(1, "b", "b", 2) );
    bt1.add_transition(1, HfstBasicTransition(2, "@_EPSILON_SYMBOL_@", "1", 3) );
    bt1.set_final_weight(2, 5);

    // Second test transducer
    bt2.add_transition(0, HfstBasicTransition(1, "c", "C", 10) );
    bt2.add_transition(0, HfstBasicTransition(1, "b", "B", 20) );
    bt2.add_transition(1, HfstBasicTransition(2, "@_EPSILON_SYMBOL_@", "1", 30) );
    bt2.set_final_weight(2, 50);

    // Second test transducer without priority string
    bt2withoutPriority.add_transition(0, HfstBasicTransition(1, "c", "C", 10) );
    bt2withoutPriority.add_transition(1, HfstBasicTransition(2, "@_EPSILON_SYMBOL_@", "1", 30) );
    bt2withoutPriority.set_final_weight(2, 50);

    // Third test transducer
    bt3.add_transition(0, HfstBasicTransition(1, "a", "b", 55) );
    bt3.add_transition(0, HfstBasicTransition(1, "b", "b", 65) );
    bt3.set_final_weight(1, 75);

    // Identity to Identity
    btIdentity.add_transition(0, HfstBasicTransition(0, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 100) );
    btIdentity.set_final_weight(0, 100);

    // Unknown to unknown
    btUnknown.add_transition(0, HfstBasicTransition(0, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 200) );
    btUnknown.set_final_weight(0, 200);

    // Epsilon
    btEpsilon.add_transition(0, HfstBasicTransition(0, "@_EPSILON_SYMBOL_@", "@_EPSILON_SYMBOL_@", 300) );
    btEpsilon.set_final_weight(0, 300);

    // Result 1 ... tr1 .p. emptyString
    //			...	emptyString .p. tr1
    btResult1.add_transition(0, HfstBasicTransition(1, "a", "a", 1) );
    btResult1.add_transition(0, HfstBasicTransition(1, "b", "b", 2) );
    btResult1.add_transition(1, HfstBasicTransition(2, "@_EPSILON_SYMBOL_@", "1", 3) );
    btResult1.set_final_weight(2, 5);
    btResult1.set_final_weight(0, 3);

    // Result 2 ... tr1 .p. tr2
      btResult2.add_transition(0, HfstBasicTransition(1, "c", "C", 10) );
    btResult2.add_transition(0, HfstBasicTransition(1, "b", "B", 20) );
    btResult2.add_transition(0, HfstBasicTransition(2, "a", "a", 1) );
    btResult2.add_transition(1, HfstBasicTransition(3, "@_EPSILON_SYMBOL_@", "1", 30) );
    btResult2.add_transition(2, HfstBasicTransition(4, "@_EPSILON_SYMBOL_@", "1", 3) );
    btResult2.set_final_weight(3, 50);
    btResult2.set_final_weight(4, 5);

    // Result 3 ... tr1 .p. tr2 without priority
    btResult3.add_transition(0, HfstBasicTransition(1, "c", "C", 10) );
    btResult3.add_transition(0, HfstBasicTransition(2, "b", "b", 2) );
    btResult3.add_transition(0, HfstBasicTransition(2, "a", "a", 1) );
    btResult3.add_transition(1, HfstBasicTransition(3, "@_EPSILON_SYMBOL_@", "1", 30) );
    btResult3.add_transition(2, HfstBasicTransition(4, "@_EPSILON_SYMBOL_@", "1", 3) );
    btResult3.set_final_weight(3, 50);
    btResult3.set_final_weight(4, 5);

    // Result 4 ... tr1 .p. trIdentity
    btResult4.add_transition(0, HfstBasicTransition(0, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 100) );
    btResult4.add_transition(0, HfstBasicTransition(0, "b", "b", 100) );
    btResult4.add_transition(0, HfstBasicTransition(0, "a", "a", 100) );
    btResult4.add_transition(0, HfstBasicTransition(0, "1", "1", 100) );
    btResult4.set_final_weight(0, 100);

    // Result 5 ... trIdentity .p. tr3
    btResult5.add_transition(0, HfstBasicTransition(1, "a", "b", 130) );
    btResult5.add_transition(0, HfstBasicTransition(2, "b", "b", 140) );
    btResult5.add_transition(0, HfstBasicTransition(3, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 200) );
    btResult5.add_transition(0, HfstBasicTransition(4, "a", "a", 300) );
    btResult5.add_transition(2, HfstBasicTransition(3, "b", "b", 160) );
    btResult5.add_transition(2, HfstBasicTransition(3, "a", "a", 160) );
    btResult5.add_transition(2, HfstBasicTransition(3, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 160) );
    btResult5.add_transition(3, HfstBasicTransition(3, "b", "b", 100) );
    btResult5.add_transition(3, HfstBasicTransition(3, "a", "a", 100) );
    btResult5.add_transition(3, HfstBasicTransition(3, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 100) );
    btResult5.add_transition(4, HfstBasicTransition(3, "b", "b", 0) );
    btResult5.add_transition(4, HfstBasicTransition(3, "a", "a", 0) );
    btResult5.add_transition(4, HfstBasicTransition(3, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0) );
    btResult5.set_final_weight(0, 100);
    btResult5.set_final_weight(1, 0);
    btResult5.set_final_weight(2, 0);
    btResult5.set_final_weight(3, 0);

    // Result 6 ... tr3 .p. trUnknown
    btResult6.add_transition(0, HfstBasicTransition(0, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "@_UNKNOWN_SYMBOL_@", "b", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "b", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "b", "a", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "@_UNKNOWN_SYMBOL_@", "a", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "a", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult6.add_transition(0, HfstBasicTransition(0, "a", "b", 200) );
    btResult6.set_final_weight(0, 200);

    // Result 7 ... trUnknown .p. tr3
    btResult7.add_transition(0, HfstBasicTransition(1, "a", "b", 130) );
    btResult7.add_transition(0, HfstBasicTransition(2, "b", "b", 140) );
    btResult7.add_transition(0, HfstBasicTransition(3, "b", "a", 600) );
    btResult7.add_transition(0, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "a", 400) );
    btResult7.add_transition(0, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "b", 400) );
    btResult7.add_transition(0, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 400) );
    btResult7.add_transition(0, HfstBasicTransition(3, "b", "@_UNKNOWN_SYMBOL_@", 600) );
    btResult7.add_transition(0, HfstBasicTransition(3, "a", "@_UNKNOWN_SYMBOL_@", 600) );
    btResult7.add_transition(1, HfstBasicTransition(4, "a", "b", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "b", "a", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "a", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "b", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "b", "@_UNKNOWN_SYMBOL_@", 470) );
    btResult7.add_transition(1, HfstBasicTransition(4, "a", "@_UNKNOWN_SYMBOL_@", 470) );
    btResult7.add_transition(3, HfstBasicTransition(4, "a", "b", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "b", "a", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "a", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "b", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "b", "@_UNKNOWN_SYMBOL_@", 0) );
    btResult7.add_transition(3, HfstBasicTransition(4, "a", "@_UNKNOWN_SYMBOL_@", 0) );
    btResult7.add_transition(4, HfstBasicTransition(4, "a", "b", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "b", "a", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "a", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "b", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "b", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult7.add_transition(4, HfstBasicTransition(4, "a", "@_UNKNOWN_SYMBOL_@", 200) );
    btResult7.set_final_weight(0, 200);
    btResult7.set_final_weight(1, 0);
    btResult7.set_final_weight(2, 0);
    btResult7.set_final_weight(4, 0);


    // Transforming basic transducers to a TYPE transducer
    HfstTransducer trEmpty(btEmpty, type);
    HfstTransducer trEmptyString(btEmptyString, type);
    HfstTransducer tr1(bt1, type);
    HfstTransducer tr2(bt2, type);
    HfstTransducer tr3(bt3, type);
    HfstTransducer tr2withoutPriority(bt2withoutPriority, type);
    HfstTransducer trIdentity(btIdentity, type);
    HfstTransducer trUnknown(btUnknown, type);
    HfstTransducer trEpsilon(btEpsilon, type);
    HfstTransducer result1(btResult1, type);
    HfstTransducer result2(btResult2, type);
    HfstTransducer result3(btResult3, type);
    HfstTransducer result4(btResult4, type);
    HfstTransducer result5(btResult5, type);
    HfstTransducer result6(btResult6, type);
    HfstTransducer result7(btResult7, type);

    // emptyLang .p. emptyLang
    HfstTransducer testTr = trEmpty;
    assert ( testTr.priority_union( trEmpty ).compare( trEmpty ) );
    // emptyString .p. emptyString
    testTr = trEmptyString;
    assert ( testTr.priority_union( trEmptyString ).compare( trEmptyString ) );
    // transducer .p. emptyString
    testTr = tr1;
    assert ( testTr.priority_union( trEmptyString ).compare( result1 ) );
    // emptyString .p. transducer
    testTr = trEmptyString;
    assert ( testTr.priority_union( tr1 ).compare( result1 ) );
    // normal transducer .p. normal transducer
    testTr = tr1;

    assert ( testTr.priority_union( tr2 ).compare( result2 ) );

    // normal transducer .p. normal transducer without priority string
    testTr = tr1;
    assert (  testTr.priority_union( tr2withoutPriority ).compare( result3 ) );
    // normal transducer .p. universal language
    testTr = tr1;

    if (false) { // DEBUG
      std::cerr << testTr.priority_union( trIdentity ).
    push_weights(TO_FINAL_STATE).minimize()
        << "--\n"
        << result4
        << std::endl;
    }

    // These assertions are omitted until OpenFst minimizes transducers
    // correctly..

    //assert ( testTr.priority_union( trIdentity ).compare( result4 ) ); // FAIL
    // identity .p. normal transducer
    testTr = trIdentity;
    assert (  testTr.priority_union( tr3 ).compare( result5 ) );
    
    // normal .p. unknown
    testTr = tr3;
    
    //assert (  testTr.priority_union( trUnknown ).compare( result6 ) ); FAIL
    // unknown .p. normal
    testTr = trUnknown;
    assert ( testTr.priority_union( tr3 ).compare( result7 ) );
    
}

// Universal pair test function
void universal_pair_test ( ImplementationType type )
{
    HfstBasicTransducer     bt,
                            bt2,
                            bt3,
                            btResult1,
                            btResult2,
                            btResult3,
                            btResult4,
                            btEmpty;

    // Test transducer a:a
    bt.add_transition(0, HfstBasicTransition(1, "a", "a", 0) );
    bt.set_final_weight(1, 0);
    // Test transducer b:b
    bt2.add_transition(0, HfstBasicTransition(1, "a", "b", 0) );
    bt2.set_final_weight(1, 0);
    // Test transducer aa:bb
    bt3.add_transition(0, HfstBasicTransition(1, "a", "b", 0) );
    bt3.add_transition(1, HfstBasicTransition(2, "a", "b", 0) );
    bt3.set_final_weight(2, 0);

    // Result 1 ( a:a .o. universal pair )
    btResult1.add_transition(0, HfstBasicTransition(1, "a", "a", 0) );
    btResult1.add_transition(0, HfstBasicTransition(1, "a", "@_UNKNOWN_SYMBOL_@", 0) );
    btResult1.add_transition(0, HfstBasicTransition(1, "a", "@_EPSILON_SYMBOL_@",  0) );
    btResult1.set_final_weight(1, 0);
    // Result 2 ( universal pair .o. a:a )
    btResult2.add_transition(0, HfstBasicTransition(1, "a", "a", 0) );
    btResult2.add_transition(0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "a", 0) );
    btResult2.add_transition(0, HfstBasicTransition(1, "@_EPSILON_SYMBOL_@", "a", 0) );
    btResult2.set_final_weight(1, 0);
    // Result 3 ( a:b .o. universal pair )
    btResult3.add_transition(0, HfstBasicTransition(1, "a", "b", 0) );
    btResult3.add_transition(0, HfstBasicTransition(1, "a", "a", 0) );
    btResult3.add_transition(0, HfstBasicTransition(1, "a", "@_UNKNOWN_SYMBOL_@", 0) );
    btResult3.add_transition(0, HfstBasicTransition(1, "a", "@_EPSILON_SYMBOL_@", 0) );
    btResult3.set_final_weight(1, 0);
    // Result 4 ( universal pair .o. a:b )
    btResult4.add_transition(0, HfstBasicTransition(1, "a", "b", 0) );
    btResult4.add_transition(0, HfstBasicTransition(1, "b", "b", 0) );
    btResult4.add_transition(0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "b", 0) );
    btResult4.add_transition(0, HfstBasicTransition(1, "@_EPSILON_SYMBOL_@", "b", 0) );
    btResult4.set_final_weight(1, 0);

    HfstTransducer tr1(bt, type);
    HfstTransducer tr2(bt2, type);
    HfstTransducer tr3(bt3, type);
    HfstTransducer result1(btResult1, type);
    HfstTransducer result2(btResult2, type);
    HfstTransducer result3(btResult3, type);
    HfstTransducer result4(btResult4, type);
    HfstTransducer empty(btEmpty, type);

    HfstTransducer un = HfstTransducer::universal_pair(type);

    // Universal pair is tested by composing it with test transducers
    // a:a .o. universal pair
    HfstTransducer tmp = tr1;
    assert ( tmp.compose(un).compare( result1 ) );
    // universal pair .o. a:a
    tmp = un;
    assert ( tmp.compose(tr1).compare( result2 ) );
    // a:b .o. universal pair
    tmp = tr2;
    assert ( tmp.compose(un).compare( result3 ) );
    // universal pair .o. a:b
    tmp = un;
    assert ( tmp.compose(tr2).compare( result4 ) );
    // aa:bb .o. universal pair
    tmp = tr3;
    assert ( tmp.compose(un).compare( empty ) );
    // universal pair .o. aa:bb
    tmp = un;
    assert ( tmp.compose(tr3).compare( empty ) );

}

StringVector remove_flags(const StringVector &v)
{
  StringVector v_wo_flags;
  for (StringVector::const_iterator it = v.begin();
       it != v.end();
       ++it)
    {
      if (not FdOperation::is_diacritic(*it))
	{ v_wo_flags.push_back(*it); }
    }
  return v_wo_flags;
}

int main(int argc, char * argv[])
{
    std::cout << "Unit tests for " __FILE__ ":" << std::endl;
    
    ImplementationType types[] = {SFST_TYPE, 
				  TROPICAL_OPENFST_TYPE,
				  FOMA_TYPE};
    unsigned int NUMBER_OF_TYPES=3;

    for (unsigned int i=0; i < NUMBER_OF_TYPES; i++) 
    {
      if (! HfstTransducer::is_implementation_type_available(types[i]))
	continue;

	// One case that fails with FOMA_TYPE
	HfstTransducer a("a", types[i]);
	a.repeat_n(2);

    	// Test alphabet after substitute

        HfstTransducer t("a", "b", types[i]);
        t.substitute("a", "c");
        StringSet alpha = t.get_alphabet();
        assert(alpha.find("b") != alpha.end());
        assert(alpha.find("c") != alpha.end());
        // TODO: which is correct?
        //assert(alpha.find("a") != alpha.end()); 

        HfstTransducer t1("a", "b", types[i]);
        HfstTransducer t2("a", "c", types[i]);
        t1.substitute(StringPair("a", "b"), t2); 

        alpha = t1.get_alphabet();
        assert(alpha.find("a") != alpha.end());
        assert(alpha.find("c") != alpha.end());
        assert(alpha.find("b") != alpha.end());

        // Test the const arguments are really const
        HfstTransducer ab("a", "b", types[i]);
        HfstTransducer ac("b", "c", types[i]);
        HfstTransducer ab_(ab);
        HfstTransducer ac_(ac);
    
        ab_.compare(ac_);
        assert(ab_.get_alphabet() == ab.get_alphabet());
        assert(ac_.get_alphabet() == ac.get_alphabet());


        HfstTransducer &compose(const HfstTransducer &another);

        HfstTransducer &compose_intersect(const HfstTransducerVector &v);

        HfstTransducer &concatenate(const HfstTransducer &another);

        HfstTransducer &disjunct(const HfstTransducer &another);

        HfstTransducer &disjunct(const StringPairVector &spv);

        HfstTransducer &intersect(const HfstTransducer &another);

        HfstTransducer &subtract(const HfstTransducer &another);

        HfstTransducer &insert_freely(const HfstTransducer &tr);

        HfstTransducer &substitute(const StringPair &symbol_pair,
                       HfstTransducer &transducer);
    
        // priority_union unit tests
        priority_union_test( types[i] );

        // cross_product unit test
        cross_product_subtest1( types[i] );
        cross_product_subtest2( types[i] );
        cross_product_subtest3( types[i] );
        cross_product_subtest4( types[i] );

        // universal pair unit tests
        universal_pair_test( types[i] );

        void insert_freely_missing_flags_from
          (const HfstTransducer &another);

	// Flag diacritic harmonization test
	HfstTokenizer flag_tokenizer;
	flag_tokenizer.add_multichar_symbol("@P.Char.ON@");
	flag_tokenizer.add_multichar_symbol("@R.Char.ON@");	

	HfstTransducer any_a("A",types[i]);
	HfstTransducer any_b("B",types[i]);
	HfstTransducer any_c("C",types[i]);
	any_a.disjunct(any_b).disjunct(any_c).minimize();
	HfstTransducer any_symbol(any_a);
	HfstTransducer any(any_symbol);
	any.repeat_star();

	HfstTransducer a_paths("A" "@P.Char.ON@",
			       flag_tokenizer,
			       types[i]);
	a_paths.concatenate(any);
	HfstTransducer a_end("@R.Char.ON@" "A",
			     flag_tokenizer,
			     types[i]);
	a_paths.concatenate(a_end).minimize();

	HfstTransducer a_paths_copy(a_paths);

	a_paths_copy.convert(HFST_OLW_TYPE);

	HfstOneLevelPaths * results = 
	  a_paths_copy.lookup_fd(flag_tokenizer.tokenize_one_level("ABCBA"));
	assert(results->size() == 1);
	assert(remove_flags(results->begin()->second) ==
	       flag_tokenizer.tokenize_one_level("ABCBA"));
	delete results;

	results = 
	  a_paths_copy.lookup_fd(flag_tokenizer.tokenize_one_level("ABCAA"));
	assert(results->size() == 1);
	assert(remove_flags(results->begin()->second) ==
	       flag_tokenizer.tokenize_one_level("ABCAA"));
	delete results;

	HfstTransducer b_paths(any_symbol);
	HfstTransducer b_paths_("B" 
				"@P.Char.ON@",
				flag_tokenizer,
				types[i]);
	b_paths.concatenate(b_paths_);
	b_paths.concatenate(any);
	HfstTransducer b_end("@R.Char.ON@" "B", 
			     flag_tokenizer,
			     types[i]);
	b_end.concatenate(any_symbol);
	b_paths.concatenate(b_end).minimize();

	HfstTransducer b_paths_copy(b_paths);
	b_paths_copy.convert(HFST_OLW_TYPE);

	results = 
	  b_paths_copy.lookup_fd(flag_tokenizer.tokenize_one_level("ABCBA"));
	assert(results->size() == 1);
	assert(remove_flags(results->begin()->second) ==
	       flag_tokenizer.tokenize_one_level("ABCBA"));
	delete results;

	results = 
	  b_paths_copy.lookup_fd(flag_tokenizer.tokenize_one_level("ABCBB"));
	assert(results->size() == 1);
	assert(remove_flags(results->begin()->second) ==
	       flag_tokenizer.tokenize_one_level("ABCBB"));
	delete results;

	a_paths.harmonize_flag_diacritics(b_paths);

	a_paths.intersect(b_paths).minimize();
	
	a_paths.convert(HFST_OLW_TYPE);
	
	HfstOneLevelPaths * one_result = 
	  a_paths.lookup_fd(flag_tokenizer.tokenize_one_level("ABCBA"));
	assert(one_result->size() == 1);
	       assert(remove_flags(one_result->begin()->second) ==
	       flag_tokenizer.tokenize_one_level("ABCBA"));
	delete one_result;

	HfstOneLevelPaths * no_results = 
	  a_paths.lookup_fd(flag_tokenizer.tokenize_one_level("ABCBB"));
	assert(no_results->size() == 0);
	delete no_results;

	no_results = 
	  a_paths.lookup_fd(flag_tokenizer.tokenize_one_level("ABCAA"));
	assert(no_results->size() == 0);
	delete no_results;

	no_results = 
	  a_paths.lookup_fd(flag_tokenizer.tokenize_one_level("ABCCC"));
	assert(no_results->size() == 0);
	delete no_results;
    }

    
    std::cout << "ok" << std::endl;
    return 0;
}

#endif // MAIN_TEST