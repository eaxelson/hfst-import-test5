#include <string>
#include <map>
#include <set>
#include <cassert>
#include <cstdio>
#include <iostream>
#include "../HfstExceptionDefs.h"

namespace hfst {

  namespace implementations {

    struct string_comparison {
      bool operator() (const std::string &str1, const std::string &str2) const {
        return (str1.compare(str2) < 0);
      }
    };

    /** @brief One implementation of template class C in 
        HfstTransition. 

        A HfstTropicalTransducerTransitionData has an input symbol and an 
        output symbol of type SymbolType (string) and a weight of type 
        WeightType (float).

        \internal Actually a HfstTropicalTransducerTransitionData has an 
        input and an output number of type unsigned int, but this 
        implementation is hidden from the user.
        The class has two static maps and functions that take care of conversion
        between strings and internal numbers.

        @see HfstTransition HfstBasicTransition */
    class HfstTropicalTransducerTransitionData {
    public:
      /** @brief The input and output symbol type. */
      typedef std::string SymbolType;
      /** @brief The weight type. */
      typedef float WeightType;
      /** @brief A set of symbols. */
      typedef std::set<SymbolType> SymbolTypeSet;

      typedef std::map<unsigned int, SymbolType> 
        Number2SymbolMap;
      typedef std::map<SymbolType, unsigned int, string_comparison> 
        Symbol2NumberMap;

      static SymbolType get_epsilon()
      {
	return SymbolType("@_EPSILON_SYMBOL_@");
      }

      static SymbolType get_unknown()
      {
	return SymbolType("@_UNKNOWN_SYMBOL_@");
      }

      static SymbolType get_identity()
      {
	return SymbolType("@_IDENTITY_SYMBOL_@");
      }

    public: /* FIXME: Should be private. */
      /* Maps that contain information of the mappings between strings 
         and numbers */
      static Number2SymbolMap number2symbol_map;
      static Symbol2NumberMap symbol2number_map;
      /* Next free number */
      static unsigned int max_number;

    protected:
      /* Get the symbol that is mapped as number */
      static std::string get_symbol(unsigned int number) {

	assert(symbol2number_map.find("") == symbol2number_map.end());
	

        Number2SymbolMap::const_iterator it = number2symbol_map.find(number);

	assert(not(it->second == ""));

        if (it == number2symbol_map.end()) {
          /*fprintf(stderr, "ERROR: "
                  "HfstTropicalTransducerTransitionData::get_symbol"
                  "(unsigned int number) "
                  "number is not mapped to any symbol\n");*/
          std::string message("HfstTropicalTransducerTransitionData: "
                      "number ");
          std::ostringstream oss;
          oss << number;
          message.append(oss.str());
          message.append(" is not mapped to any symbol");
          HFST_THROW_MESSAGE
            (HfstFatalException,
             message);

        }

        return it->second;
      }

      /* Get the number that is used to represent the symbol */
      static unsigned int get_number(const std::string &symbol) {

	if(symbol == "") { // FAIL
	  Symbol2NumberMap::iterator it = symbol2number_map.find(symbol);
	  if (it == symbol2number_map.end()) {
	    std::cerr << "ERROR: No number for the empty symbol\n" 
		      << std::endl;
	  }
	  else {
	    std::cerr << "ERROR: The empty symbol corresdponds to number " 
		      << it->second << std::endl;
	  }
	  assert(false);
	}

        Symbol2NumberMap::iterator it = symbol2number_map.find(symbol);
        if (it == symbol2number_map.end()) {
          max_number++;
          symbol2number_map[symbol] = max_number;
          number2symbol_map[max_number] = symbol;
          return max_number;
        }
        return it->second;
      }

      //private: TEST
    public:
      /* The actual transition data */
      unsigned int input_number;
      unsigned int output_number;
      WeightType weight;

    public:
      void print_transition_data() 
      {
	fprintf(stderr, "%i:%i %f\n", 
		input_number, output_number, weight);
      }

    public:

      /** @brief Create a HfstTropicalTransducerTransitionData with 
          epsilon input and output strings and weight zero. */
    HfstTropicalTransducerTransitionData(): 
      input_number(0), output_number(0), weight(0) {}

      /** @brief Create a deep copy of HfstTropicalTransducerTransitionData 
          \a data. */
      HfstTropicalTransducerTransitionData
        (const HfstTropicalTransducerTransitionData &data) {
        input_number = data.input_number;
        output_number = data.output_number;
        weight = data.weight;
      }

      /** @brief Create a HfstTropicalTransducerTransitionData with 
          input symbol \a isymbol, output symbol \a osymbol 
          and weight \a weight. */
      HfstTropicalTransducerTransitionData(SymbolType isymbol,
					   SymbolType osymbol,
					   WeightType weight) {
	if (isymbol == "" || osymbol == "")
	  HFST_THROW_MESSAGE
	    (EmptyStringException,
	     "HfstTropicalTransducerTransitionData"
	     "(SymbolType, SymbolType, WeightType)");
	
        input_number = get_number(isymbol);
        output_number = get_number(osymbol);
        this->weight = weight;
      }

      /** @brief Get the input symbol. */
      SymbolType get_input_symbol() const {
        return get_symbol(input_number);
      }

      /** @brief Get the output symbol. */
      SymbolType get_output_symbol() const {
        return get_symbol(output_number);
      }

      /** @brief Get the weight. */
      WeightType get_weight() const {
        return weight;
      }

      /* Are these needed? */
      static bool is_epsilon(const SymbolType &symbol) {
        return (symbol.compare("@_EPSILON_SYMBOL_@") == 0);
      }
      static bool is_unknown(const SymbolType &symbol) {
        return (symbol.compare("@_UNKNOWN_SYMBOL_@") == 0);
      }
      static bool is_identity(const SymbolType &symbol) {
        return (symbol.compare("@_IDENTITY_SYMBOL_@") == 0);
      }
      static bool is_valid_symbol(const SymbolType &symbol) {
	if (symbol == "")
	  return false;
	return true;
      }

      static SymbolType get_marker(const SymbolTypeSet &sts) {
	(void)sts;
	return SymbolType("@_MARKER_SYMBOL_@");
      }

      /** @brief Whether this transition is less than transition 
          \a another. 

          /internal is it too slow if string comparison is used instead?
      */
      bool operator<(const HfstTropicalTransducerTransitionData &another) 
        const {
        if (input_number < another.input_number )
          return true;
        if (input_number > another.input_number)
          return false;
        if (output_number < another.output_number)
          return true;
        if (output_number > another.output_number)
          return false;
        return (weight < another.weight);
      }

      void operator=(const HfstTropicalTransducerTransitionData &another)
	{
	  input_number = another.input_number;
	  output_number = another.output_number;
	  weight = another.weight;
	}

      friend class Number2SymbolMapInitializer;
      friend class Symbol2NumberMapInitializer;

      friend class ComposeIntersectFst;
      friend class ComposeIntersectLexicon;
      friend class ComposeIntersectRule;
      friend class ComposeIntersectRulePair;

    };

    // Initialization of static members in class 
    // HfstTropicalTransducerTransitionData..
    class Number2SymbolMapInitializer {
    public:
      Number2SymbolMapInitializer
        (HfstTropicalTransducerTransitionData::Number2SymbolMap &map) {
        map[0] = std::string("@_EPSILON_SYMBOL_@");
        map[1] = std::string("@_UNKNOWN_SYMBOL_@");
        map[2] = std::string("@_IDENTITY_SYMBOL_@");
      }
    };

    class Symbol2NumberMapInitializer {
    public:
      Symbol2NumberMapInitializer
        (HfstTropicalTransducerTransitionData::Symbol2NumberMap &map) {
        map["@_EPSILON_SYMBOL_@"] = 0;
        map["@_UNKNOWN_SYMBOL_@"] = 1;
        map["@_IDENTITY_SYMBOL_@"] = 2;
      }
    };

  } // namespace implementations

} // namespace hfst