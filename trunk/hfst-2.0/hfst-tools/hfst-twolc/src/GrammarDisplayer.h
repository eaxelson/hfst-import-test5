#ifndef __GRAMMAR_DISPLAYER_H
#define __GRAMMAR_DISPLAYER_H_
#include "../config.h"
#include <iostream>
#include "string_manipulation.h"
#include "grammar_defs.h"
#include "rule_parts.h"

class GrammarDisplayer
{
 public:
  void display_alphabet_declaration(void);
  void display_sets_declaration(void);
  void display_definitions_declaration(void);
  void display_diacritics_declaration(void);
  void display_rules_declaration(void);
  void display_pair(Pair * pair);
  void display_end_of_line(void);
  void display_alphabet(NonDelimiterRange * alphabet_range);
  void display_set_definition(Set * set);
  char * get_epsilon_symbol(void);
  char * get_any_symbol(void);
  char * get_union_operator(void);
  char * get_intersection_operator(void);
  char * get_difference_operator(void);
  char * get_containment_operator(void);
  char * get_containment_once_operator(void);
  char * get_term_complement_operator(void);
  char * get_complement_operator(void);
  char * get_freely_insert_operator(void);
  char * get_star_operator(void);
  char * get_plus_operator(void);
  char * get_power_operator(void);

  void display(RuleStrings &rule_representation);
  void display(RuleString * rule_representation);
  void display(RuleCenter &rule_representation);
  void display(Operator rule_operator);
  void display(RuleRepresentation * rule_representation);
  void display(RuleRepresentationVector * rule_representations);
  void display_regexp(char * regexp_name, RuleString * regexp);
};
#endif
