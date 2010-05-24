#ifndef _GRAMMAR_COLLECTOR_H
#define _GRAMMAR_COLLECTOR_H
#include "../config.h"
#include "AlphabetCollector.h"
#include "string_manipulation.h"
#include <vector>
#include <iostream>

typedef std::vector<char*> GrammarVector;

class GrammarCollector
{
 private:
  AlphabetCollector &alphabet_collector;
  GrammarVector grammar_vector;
  
  void display(SymbolPairSet &alphabet);
  bool is_first_symbol_in_pair(GrammarVector::iterator it);

 public:
  GrammarCollector(AlphabetCollector &alpha):
    alphabet_collector(alpha)
    {};
  ~GrammarCollector(void)
    {};
  void insert_symbol(char * symbol);
  void insert_pair(char * input, char * output);
  void insert_rule_name(char * name);
  void insert_rule_declaration(void);
  void insert_star(void);
  void insert_plus(void);
  void insert_freely_insert(void);
  void insert_complement(void);
  void insert_term_complement(void);
  void insert_containment_once(void);
  void insert_containment(void);
  void insert_union(void);
  void insert_intersection(void);
  void insert_power(void);
  void insert_difference(void);
  void insert_left_square_bracket(void);
  void insert_right_square_bracket(void);
  void insert_left_bracket(void);
  void insert_right_bracket(void);
  void insert_left_restriction_arrow(void);
  void insert_left_arrow(void);
  void insert_right_arrow(void);
  void insert_left_right_arrow(void);
  void insert_eol(void);
  void insert_equals(void);
  void insert_center_marker(void);
  void display_pairs(SymbolPairSet &symbol_pair_set);
  void display(void);
};

#endif
