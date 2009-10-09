#ifndef RULEMODIFIER_H
#define RULEMODIFIER_H
#include "../config.h"
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "string_manipulation.h"
#include "grammar_defs.h"
#include "rule_parts.h"

typedef std::vector<char * > VariableValues;
typedef std::set<char*,str_cmp> AlphabetSet;
typedef std::set<char*,lax_str_cmp> LaxAlphabetSet;
typedef std::set<SymbolNumber> UnknownSymbolSet;
typedef std::vector<SymbolNumber> SymbolSetVector; // The order of a symbol 
                                                   // set is influential in
                                                   // rules with variables.
typedef std::map<SymbolNumber,SymbolSetVector*> SetMap;
typedef std::map<char*, RuleString*,str_cmp> DefinitionMap;

class RuleModifier
{
private:
  char * rule_name;
  Operator rule_operator;  
  RuleContexts contexts;
  size_t current_context;
  VariableValueMap variable_values;
  RuleVariableComputer variable_computer;
  UnknownSymbolSet unknown_symbols;  // Variables and possible erroneous
                                     // symbols found in contexts and centers.
  Center center;
  SymbolMap symbols;
  SymbolNumberMap symbol_numbers;
  AlphabetSet twolc_internal_symbols;
  LaxAlphabetSet alphabet_set;
  AlphabetSet diacritics_set;
  AlphabetSet current_rule_diacritics;
  AlphabetSet set_name_set;
  AlphabetSet definition_name_set;
  SetMap symbol_sets;
  DefinitionMap definitions;

  char * HFST_TWOLC_COLON;
  char * HFST_TWOLC_UNION;
  char * HFST_TWOLC_FREELY_INSERT;
  char * HFST_TWOLC_EPSILON;
  char * HFST_TWOLC_LEFT_BRACKET;
  char * HFST_TWOLC_RIGHT_BRACKET;

  /* Append the SymbolNumber of symbol at the end of
     the latest added rule context. */
  void insert_symbol(char * symbol);
  

  /* Add a new rule context. */
  void new_context(void);
  RuleString * stringify(RuleContextVector * symbol_number_vector);
  RuleCenter * stringify(CenterPair * center_pair);
  RuleString * stringify(CenterPairVector * center_pairs);

  static const char * END_OF_CONTEXT;
  static void print(RuleString * rule_contexts);


  void insert_internal_symbol(char * symbol);
  void init_internal_symbols(void);
  RuleString * add_diacritics(RuleString * context_string);

public:
  RuleModifier(void):
    rule_name(NULL),
    rule_operator(NOT_DEFINED_OPERATOR),
    current_context(0),
    variable_computer(variable_values),
    center(variable_values),
    HFST_TWOLC_COLON(string_copy(":")),
    HFST_TWOLC_UNION(string_copy("|")),
    HFST_TWOLC_FREELY_INSERT(string_copy("__HFST_TWOLC_FREELY_INSERT")),
    HFST_TWOLC_EPSILON(string_copy("__HFST_TWOLC_EPSILON_SYMBOL")),
    HFST_TWOLC_LEFT_BRACKET(string_copy("[")),
    HFST_TWOLC_RIGHT_BRACKET(string_copy("]"))
  {
    contexts.push_back(new RuleContextContainer(variable_values));
    init_internal_symbols();
  };
  ~RuleModifier(void)
  {
    reset();
    for(RuleContexts::iterator it = contexts.begin();
	it != contexts.end();
	++it)
      {
	delete *it;
      }

    for (SymbolMap::iterator it = symbols.begin();
	 it != symbols.end();
	 ++it)
      {
	free(it->first);
      }
    for (SymbolNumberMap::iterator it = symbol_numbers.begin();
	 it != symbol_numbers.end();
	 ++it)
      {
	free(it->second);
      }
    for (AlphabetSet::iterator it = twolc_internal_symbols.begin();
	 it != twolc_internal_symbols.end();
	 ++it)
      {
	free(*it);
      }
    for (AlphabetSet::iterator it = diacritics_set.begin();
	 it != diacritics_set.end();
	 ++it)
      {
	free(*it);
      }
    for (AlphabetSet::iterator it = alphabet_set.begin();
	 it != alphabet_set.end();
	 ++it)
      {
	free(*it);
      }
    for (SetMap::iterator it = symbol_sets.begin();
	 it != symbol_sets.end();
	 ++it)
      {
	delete it->second;
      }
    for (AlphabetSet::iterator it = set_name_set.begin();
	 it != set_name_set.end();
	 ++it)
      {
	free(*it);
      }
    for (DefinitionMap::iterator it = definitions.begin();
	 it != definitions.end();
	 ++it)
      {
	free(it->first);
	for (RuleString::iterator jt = it->second->begin();
	     jt != it->second->end();
	     ++jt)
	  {
	    free(*jt);
	  }
	delete it->second;
      }
    for (AlphabetSet::iterator it = definition_name_set.begin();
	 it != definition_name_set.end();
	 ++it)
      {
	free(*it);
      }
    free(HFST_TWOLC_FREELY_INSERT);
    free(HFST_TWOLC_RIGHT_BRACKET);
    free(HFST_TWOLC_LEFT_BRACKET);
    free(HFST_TWOLC_EPSILON);
    free(HFST_TWOLC_COLON);
    free(HFST_TWOLC_UNION);    
  };
  char * generate_warning(const char * warning, const char * symbol);
  char * generate_warning(const char * warning, const char * symbol1,
			  const char * symbol2);

  
  // Set the input and output characters of the
  // center. If char * symbol isn't defined, define
  // it.
  void set_center_input(char * symbol);
  void set_center_output(char * symbol);
  void add_center_pair(char * input,
		       char * output);
  void set_rule_operator(unsigned int op);
  void set_rule_name(char * name);
  /* Define a SymbolNumber without adding it to 
     the latest context.  */
  SymbolNumber define_symbol(char * symbol);

  /* For defining the alphabet of the grammar. */
  void define_alphabet_symbol(char * symbol);
  void define_alphabet(NonDelimiterRange * alphabet_range);
  void define_diacritics(NonDelimiterSymbolRange * diacritics);
  void define_diacritic(char * diacritic);
  bool is_diacritic(char * symbol);
  bool is_internal_symbol(char * symbol);
  bool is_alphabet_symbol(char * symbol);
  bool is_set_name(char * symbol);
  bool is_definition_name(char * symbol);
  bool is_known_symbol(char * symbol);
  void define_set(char * set_name);
  void define_definition(char * definition_name,RuleString * definition);
  void add_set_symbols(char * set_name, NonDelimiterSymbolRange * range);
  void check_symbols(NonDelimiterSymbolRange * symbol_range);

  /* Get the SymbolNumber corresponding to a symbol. It is
     an error to ask for the SymbolNumber of a symbol, which 
     hasn't been declared. */
  SymbolNumber get_symbol_number(char * symbol);

  void insert_definition(char * definition_name);  
  /* Add a symbol to the latest context. If it is the
     END_OF_CONTEXT symbol, begin a new copntext. If
     the symbol hasn't been defined, define it. */
  void add_symbol_to_context(char * symbol);

  void add_rule_context(RuleString * rule_context);
  void add_rule_contexts(RuleStrings * rule_contexts);

  /* Reset all context information (including variable values). The symbol 
     numbering will remain the same. */
  void reset(void);

  /* Define a symbol as a variable and give its values. */
  void insert_variable(SymbolNumber variable,
		       VariableValueVector &values);
  void insert_variable(char * variable,
		       NonDelimiterSymbolRange * values);

  /* Set the matcher for the current block of variables. 
     The matcher is FREE by default. */
  void set_block_matcher(unsigned int matcher);

  /* Close the current block of variables and begin a new one. */
  void new_block(void);

  RuleRepresentationVector * replace_variables(void);
  NonDelimiterRange * get_diacritics(void);
};

#endif
