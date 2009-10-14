#ifndef RULE_PARTS_H__
#define RULE_PARTS_H__
#include "../config.h"
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <iostream>

#include "string_manipulation.h"
#include "RuleVariableComputer.h"

typedef std::vector<SymbolNumber> RuleContextVector;

typedef std::map<SymbolNumber,char *> SymbolNumberMap;
typedef std::vector<char *> RuleString;
typedef std::vector<RuleString*> RuleStrings;
typedef std::pair<char *, char *> RuleCenter;

enum Operator
  {
    LEFT_OPERATOR,
    LEFT_RIGHT_OPERATOR,
    RIGHT_OPERATOR,
    NOT_LEFT_OPERATOR,
    NOT_DEFINED_OPERATOR // For debugging.
  };


typedef std::pair<SymbolNumber,SymbolNumber> CenterPair;
typedef std::vector<CenterPair> CenterPairVector;
 
class RuleRepresentation {
 public:
  char * rule_name;
  Operator rule_operator;
  RuleCenter center;
  RuleString * center_pairs;
  RuleStrings contexts;
  RuleRepresentation(void):
    rule_name(NULL),
      rule_operator(NOT_DEFINED_OPERATOR),  
    center_pairs(NULL)
      {};

   ~RuleRepresentation(void)
    {
      for (RuleStrings::iterator it = contexts.begin();
	   it != contexts.end();
	   ++it)
	{
	  delete *it;
	}
      delete center_pairs;
      free(rule_name);
    }
};

typedef std::vector<RuleRepresentation *> RuleRepresentationVector;


struct compare_str {
  bool operator() (const char * str1, const char * str2) const
  { return strcmp(str1,str2) < 0; }
};

typedef std::map<char *,SymbolNumber,compare_str> SymbolMap;


/* A class capable of holding one rule context. */
class RuleContextContainer
{

private:
  VariableValueMap &variable_values;
  RuleContextVector context;
  size_t position;

public:
  RuleContextContainer(VariableValueMap &values):
    variable_values(values),
    position(0)
  {};
  
  void insert_SymbolNumber(SymbolNumber c);
  void insert_SymbolNumbers(RuleContextVector * symbol_numbers);
  RuleContextVector * replace(void);
  void reset(void); 
  size_t get_position(void);

};

typedef std::vector<RuleContextContainer*> RuleContexts;


class Center 
{
 private:
  CenterPairVector center_pairs;
  SymbolNumber input_char;
  SymbolNumber output_char;  
  VariableValueMap &variable_values;
  bool input_set;
  bool output_set;
  bool vector_set;

 public:
  Center(VariableValueMap &values):
    variable_values(values),
    input_set(false),
    output_set(false),
    vector_set(false)
    {};
  ~Center(void) {};
  void reset(void);
  CenterPair * replace(void);
  CenterPairVector * replace_variables(void);
  void set_input(SymbolNumber new_input);
  void set_output(SymbolNumber new_output);
  void add_center_pair(SymbolNumber new_input, SymbolNumber new_output);
  bool is_set(void);
  bool vector_is_set(void);
};

RuleString * adjoin(RuleString * str1, RuleString * atr2);
RuleString * adjoin(RuleString * str, char * regex_operator);
RuleString * adjoin(char * regex_operator, RuleString * str);
RuleString * adjoin(RuleString * str1, 
		    char * regex_operator, RuleString * atr2);
RuleString * make_power(RuleString * str, int exponent);
RuleString * make_pair(char * symbol1, char * symbol2);
RuleString * make_pair(char * symbol);
RuleString * make_definition_pair(char * symbol);
RuleString * make_exponent(char *exponent_string);
RuleString * make_context(RuleString * regexp1, RuleString * regexp2);
RuleString * make_empty_parenthesis(void);
RuleString * make_empty_range(void);
RuleString * make_parenthesis(RuleString * contents);
RuleString * make_range(RuleString * contents);
#endif
