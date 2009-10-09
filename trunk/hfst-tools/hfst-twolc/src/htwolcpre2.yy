%{
  
#include <iostream>
#include <map>
#include <set>
#include <string.h>
#include <fstream>
#include <cstdlib>

#include "CommandLineParser.h"
  
#include "InputReader.h"
  
  extern int yylineno;
  extern char * yytext;
  InputReader input_reader(yylineno);
  
#include "GrammarDisplayer.h"
  GrammarDisplayer grammar_displayer;
  
#include "grammar_defs.h"
#include "rule_parts.h"
  
#include "AlphabetCollector.h"
  AlphabetCollector alphabet_collector;

#include "GrammarCollector.h"
  GrammarCollector grammar_collector(alphabet_collector);

#define YYDEBUG 1

#define YYERROR_VERBOSE 1
 
 bool verbose = false;
 
  void yyerror(const char * text );
  void warn(const char * warning );
  int yylex();
  int yyparse();

%}



%union {
  int symbol_number;
  int exponent;
  char * name;
  char * value;
  char * test_form;
  NonDelimiterRange * NonDelimiterRange_p;
  NonDelimiterSymbolRange * NonDelimiterSymbolRange_p;
  RuleStrings * RuleStrings_p;
  Pair * Pair_p;
  Set * Set_p;
  unsigned int operator_type;
  unsigned int matcher_type;
  RuleString * RuleString_p;
  RuleRepresentationVector * RuleRepresentationVector_p;
  CenterPairVector * CenterPairVector_p;
  SymbolSet * SymbolSet_p; 
}


 /* 
    All unary operators have stronger precedence than binary ones.
 */

 /* Binary operators ordered by precedence from lowest to highest. */
%left  <symbol_number> FREELY_INSERT
%left  <symbol_number> DIFFERENCE
%left  <symbol_number> INTERSECTION
%left  <symbol_number> UNION

 /* Unary operators ordered by precedence from lowest to highest. */
%right <symbol_number> STAR PLUS
%left  <symbol_number> CONTAINMENT CONTAINMENT_ONCE TERM_COMPLEMENT COMPLEMENT 
%right <symbol_number> POWER

 /* Range delimiters. */
%right <symbol_number> RIGHT_SQUARE_BRACKET RIGHT_BRACKET 
%left  <symbol_number> LEFT_SQUARE_BRACKET LEFT_BRACKET

 /* The arrow-tokens. */
%token <symbol_number> LEFT_RESTRICTION_ARROW LEFT_ARROW 
%token <symbol_number> RIGHT_ARROW LEFT_RIGHT_ARROW

 /* Basic tokens. */
%token <symbol_number>   NEWLINE PAIR_SEPARATOR PAIR_SEPARATOR_LEFT 
%token <symbol_number>   PAIR_SEPARATOR_RIGHT PAIR_SEPARATOR_BOTH
%token <symbol_number>   WHERE MATCHED MIXED FREELY IN AND
%token <symbol_number>   EOL EQUALS CENTER_MARKER
%token <symbol_number>   ALPHABET_DECLARATION DIACRITICS_DECLARATION 
%token <symbol_number>   SETS_DECLARATION DEFINITION_DECLARATION
%token <symbol_number>   RULES_DECLARATION VARIABLE_DECLARATION 
%token <name>            RULE_NAME 
%token <value>           SYMBOL NUMBER

%type <Pair_p>                    SIMPLE_PAIR LEFT_SIDE_OF_RULE CENTER_PAIR
%type <NonDelimiterRange_p>       NON_DELIMITER_RANGE
%type <SymbolSet_p>               NON_DELIMITER_SYMBOL_RANGE
%type <value>                     CHAR 
%type <Set_p>                     SET
%type <matcher_type>              MATCHER
%type <RuleString_p>              PAIR RE REG REGULAR_EXPRESSION RANGE 
%type <RuleString_p>              PARENTHESIS CONTEXT FREELY_INSERT_RANGE
%type <RuleString_p>              DIACRITIC_PAIR
%type <RuleRepresentationVector_p> RULE
%type <CenterPairVector_p>        RANGE_OF_PAIRS
%%

ALL: GRAMMAR {}
;

GRAMMAR: ALPHABET GRAMMAR1 {}
;

GRAMMAR1: SETS GRAMMAR_RULES {}
| GRAMMAR_RULES      {}
;

GRAMMAR_RULES: RULES_DECLARATION RULES {}
;

ALPHABET: ALPHABET_DECLARATION ALPHABET_LINES {};

ALPHABET_LINES: ALPHABET_LINES NON_DELIMITER_RANGE EOL 
{}
|
NON_DELIMITER_RANGE EOL
{};

SETS: SETS_DECLARATION SET_DEFINITIONS {};


SET_DEFINITIONS: SET_DEFINITIONS SET {}
| SET {};

SET: SYMBOL EQUALS NON_DELIMITER_SYMBOL_RANGE EOL 
{
  alphabet_collector.define_set($1,$3);
};

RULES: RULE_DEFINITIONS {}

RULE_DEFINITIONS:
RULE_DEFINITIONS RULE 
{}
| RULE                  
{};

RULE: HEADER LEFT_SIDE_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE  {}
| HEADER LEFT_SIDE_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE WHERE 
  VARIABLE_DEFINITIONS EOL {}
| HEADER LEFT_SIDES_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE {}
| HEADER LEFT_SIDES_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE WHERE 
  VARIABLE_DEFINITIONS EOL {};

RIGHT_SIDE_OF_RULE: 
RIGHT_SIDE_OF_RULE CONTEXT EOL {}   
| RIGHT_SIDE_OF_RULE CENTER_MARKER EOL {}
| CONTEXT EOL {}
| CENTER_MARKER EOL {}
;

CONTEXT: REGULAR_EXPRESSION CENTER_MARKER REGULAR_EXPRESSION          
{}
| CENTER_MARKER REGULAR_EXPRESSION {}
| REGULAR_EXPRESSION CENTER_MARKER {};

MATCHER: MATCHED {}
| MIXED {}
| FREELY {}
;

VARIABLE_DEFINITIONS: VARIABLE_BLOCK MATCHER {}
| VARIABLE_DEFINITIONS AND VARIABLE_BLOCK MATCHER {}
| VARIABLE_DEFINITIONS AND VARIABLE_BLOCK {}
| VARIABLE_BLOCK {};

VARIABLE_BLOCK: VARIABLE_BLOCK VARIABLE_DEFINITION {}
| VARIABLE_DEFINITION {};

VARIABLE_DEFINITION: SYMBOL IN VARIABLE_SEQUENCE {}
| SYMBOL IN SYMBOL {};

VARIABLE_SEQUENCE: LEFT_BRACKET NON_DELIMITER_SYMBOL_RANGE RIGHT_BRACKET {};;

OPERATOR: LEFT_RESTRICTION_ARROW {}
| LEFT_ARROW {}
| RIGHT_ARROW {}
| LEFT_RIGHT_ARROW {}
;

HEADER: RULE_NAME {
  grammar_collector.insert_rule_name($1);
};

LEFT_SIDE_OF_RULE: CENTER_PAIR {};

LEFT_SIDES_OF_RULE: RANGE_OF_PAIRS {};

CENTER_PAIR: SYMBOL PAIR_SEPARATOR SYMBOL 
{  
  alphabet_collector.insert_pair($1,$3);
  grammar_collector.insert_pair($1,$3);
}
;

RANGE_OF_PAIRS: RANGE_OF_PAIRS UNION CENTER_PAIR {}
| CENTER_PAIR {};

REGULAR_EXPRESSION: REGULAR_EXPRESSION UNION REG {}
| REGULAR_EXPRESSION INTERSECTION REG {}
| REGULAR_EXPRESSION DIFFERENCE REG {}
| REGULAR_EXPRESSION FREELY_INSERT LEFT_SQUARE_BRACKET
FREELY_INSERT_RANGE RIGHT_SQUARE_BRACKET 
{}
| REG {};

REG: REG RE {}
| RE {};

RE: CONTAINMENT RE {}
| CONTAINMENT_ONCE RE {}
| TERM_COMPLEMENT RE {}
| COMPLEMENT RE {}
| RE STAR {}
| RE PLUS {}
| RE POWER NUMBER {}
| RANGE {}
| PARENTHESIS {}
| PAIR {};

RANGE: LEFT_SQUARE_BRACKET REGULAR_EXPRESSION RIGHT_SQUARE_BRACKET {}
| LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET {};

PARENTHESIS: LEFT_BRACKET REGULAR_EXPRESSION RIGHT_BRACKET {}
| LEFT_BRACKET RIGHT_BRACKET {};

NON_DELIMITER_RANGE: NON_DELIMITER_RANGE SIMPLE_PAIR {}
| /* nothing */ {};

FREELY_INSERT_RANGE: FREELY_INSERT_RANGE DIACRITIC_PAIR 
{
}
| DIACRITIC_PAIR 
{
};

DIACRITIC_PAIR: SYMBOL PAIR_SEPARATOR SYMBOL 
{
  grammar_collector.insert_pair($1,$3);
};

NON_DELIMITER_SYMBOL_RANGE: NON_DELIMITER_SYMBOL_RANGE SYMBOL 
{
  if ($1->find($2) != $1->end())
    {
      free($2);
    }
  else
    {
      $1->insert($2);
    }
  $$ = $1;
}
| SYMBOL 
{
  SymbolSet * set = new SymbolSet;
  set->insert($1);
  $$ = set;
};

SIMPLE_PAIR: CHAR PAIR_SEPARATOR CHAR 
{ alphabet_collector.define_alphabet_pair($1,$3); };

PAIR: CHAR PAIR_SEPARATOR CHAR 
{
  alphabet_collector.insert_pair($1,$3);
  grammar_collector.insert_pair($1,$3);

}
;

CHAR: SYMBOL {}
| NUMBER {};




%%

void warn(const char * warning) { }

void yyerror(const char * text) {
  input_reader.error(text);
  exit(1);
}

int main(int argc, char * argv[])
{
  CommandLineParser command_line_reader(argc, argv,false,false);
  if (command_line_reader.verbose)
    {
      std::cerr << "Collecting alphabet pairs and "
		<< "compiling set constructions." << std::endl;
    }
  input_reader.set_input(&std::cin);
  //yydebug = 1;
  size_t exit_code = yyparse();
  alphabet_collector.compute_set_constructions();
  grammar_collector.display();
  exit(exit_code);
}


