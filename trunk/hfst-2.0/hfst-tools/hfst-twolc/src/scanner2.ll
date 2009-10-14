%option 8Bit batch yylineno noyywrap

%{
  // Autotools stuff
  #include "../config.h"

  // typedefs and constants needed.
  #include "grammar_defs.h"
  #include "rule_parts.h"
  #include "AlphabetCollector.h"
  #include "GrammarCollector.h"
  extern GrammarCollector grammar_collector;
  
  #include "RuleModifier.h"

  #include <cstdlib>
  #include <vector>
  #include <map>
  #include <iostream>
  #include <fstream>

  // The parser. Supplies flex with its symbol tables.
  #include "htwolcpre2.h" 

  // Functions for manipulating strings.
  #include "string_manipulation.h"

  // input_defs.h declares the inputHandler, which is
  // an object that overrides flex' default input reading.
  #include "input_defs.h"
  #include "GrammarDisplayer.h"

  extern GrammarDisplayer grammar_displayer;

%}

ALPHABET_DECL        ^[\t ]*Alphabet[\t ]*(!.*)?$
DIACRITICS_DECL      ^[\t ]*Diacritics[\t ]*(!.*)?$
VARIABLE_DECL        ^[\t ]*Rule-variables[\t ]*(!.*)?$
SETS_DECL            ^[\t ]*Sets[\t ]*(!.*)?$
RULES_DECL           ^[\t ]*Rules[\t ]*(!.*)?$
DEFINITIONS_DECL     ^[\t ]*Definitions[\t ]*(!.*)?$
WHERE                where
MATCHED              matched
MIXED                mixed
FREELY               freely
IN                   in

COMMENT_BEGINS        !

LEFT_RESTRICTION_OP   \/<=
LEFT_OP               <=
RIGHT_OP              =>
LEFT_RIGHT_OP         <=>

LEFT_RANGE                [\[\{]
RIGHT_RANGE               [\]\}]
LEFT_PAREN                \(
RIGHT_PAREN               \)

EOL_LITERAL                       \;
STAR_LITERAL                      \*
PLUS_LITERAL                      \+
FREELY_INSERT_LITERAL             \_\_HFST\_TWOLC\_FREELY\_INSERT
INTERSECTION_LITERAL              \&
UNION_LITERAL                     \|
POWER_LITERAL                     \^
DIFFERENCE_LITERAL                - 
COMPLEMENT_LITERAL                ~
CONTAINMENT_LITERAL_ONCE          \$\.
CONTAINMENT_LITERAL               \$
TERM_COMPLEMENT_LITERAL           \\
EQ_LITERAL                        =
CENTER_MARKER_LITERAL            \_
PAIR_SEPARATOR_LITERAL           \:
PAIR_SEPARATOR_LITERAL_LEFT      \:[\t ]
PAIR_SEPARATOR_LITERAL_RIGHT     [\t ]\:
NAME_CH                          [^!|&\"+*\:;\n\t?\^<>=\[\]\-$~%\\()= ]
SPECIAL_CH                       [!|&\"+*\:;\n\t\^<>=\[\]\-$~()=\\ ]
ESCAPABLE_CH                     [!|&\"_+*\:;\t?\^<>=\{\}\[\]\-$~()\%=\\ ]
RULE_NAME_CH                     [^\"\n!]   
WHITE_SPACE                      [ \t]
NON_WHITE_SPACE                  [^ \t]
INTEGER                          -?[1-9][0-9]*
KILL_SYMBOL                      \_\_HFST\_TWOLC\_DIE
%%

{ALPHABET_DECL}    { return ALPHABET_DECLARATION; }
{SETS_DECL}        { return SETS_DECLARATION; }
{RULES_DECL}       { grammar_collector.insert_rule_declaration(); 
                     return RULES_DECLARATION; }

{WHITE_SPACE}           { /* spaces and tabs: ignore */ }
\n                      { /* ignore */ }

^[ \t]*\"{RULE_NAME_CH}*\"    { 
                                yylval.name = string_copy(yytext); 
                                return RULE_NAME; 
                              }

{STAR_LITERAL}                   { grammar_collector.insert_star();
                                   return STAR; }
{PLUS_LITERAL}                   { grammar_collector.insert_plus();
                                   return PLUS; }
{FREELY_INSERT_LITERAL}          { grammar_collector.insert_freely_insert();
                                   return FREELY_INSERT; }
{COMPLEMENT_LITERAL}             { grammar_collector.insert_complement();
                                   return COMPLEMENT; }
{TERM_COMPLEMENT_LITERAL}        { grammar_collector.insert_term_complement();
                                   return TERM_COMPLEMENT; }
{CONTAINMENT_LITERAL_ONCE}       { grammar_collector.insert_containment_once();
                                   return CONTAINMENT_ONCE; }
{CONTAINMENT_LITERAL}            { grammar_collector.insert_containment();
                                   return CONTAINMENT; }
{UNION_LITERAL}[\n \t]*          { grammar_collector.insert_union();
                                   return UNION; }
{INTERSECTION_LITERAL}[\n \t]*   { grammar_collector.insert_intersection();
                                   return INTERSECTION; }
{POWER_LITERAL}                  { grammar_collector.insert_power();
                                   return POWER; }
{DIFFERENCE_LITERAL}             { grammar_collector.insert_difference(); 
                                   return DIFFERENCE; }

{LEFT_RANGE}             { grammar_collector.insert_left_square_bracket();
                           return LEFT_SQUARE_BRACKET; }
{RIGHT_RANGE}            { grammar_collector.insert_right_square_bracket();
                           return RIGHT_SQUARE_BRACKET; }
{LEFT_PAREN}             { grammar_collector.insert_left_bracket();
                           return LEFT_BRACKET; }
{RIGHT_PAREN}            { grammar_collector.insert_right_bracket();
                           return RIGHT_BRACKET; }

{LEFT_RESTRICTION_OP}\n* { grammar_collector.insert_left_restriction_arrow();
                           return LEFT_RESTRICTION_ARROW; }
{LEFT_OP}\n*             { grammar_collector.insert_left_arrow();
                           return LEFT_ARROW; }
{RIGHT_OP}\n*            { grammar_collector.insert_right_arrow();
                           return RIGHT_ARROW; }
{LEFT_RIGHT_OP}\n*       { grammar_collector.insert_left_right_arrow();
                           return LEFT_RIGHT_ARROW; }

{PAIR_SEPARATOR_LITERAL}                    { return PAIR_SEPARATOR; }

{EOL_LITERAL}           { grammar_collector.insert_eol();
                          return EOL; }
{EQ_LITERAL}            { return EQUALS; }
{CENTER_MARKER_LITERAL} { grammar_collector.insert_center_marker();
                          return CENTER_MARKER; }

{KILL_SYMBOL}           { print_kill_symbol();
                          exit(1); }
({NAME_CH}|%{ESCAPABLE_CH}?)+ { yylval.value = remove_white_space(yytext);
                                return SYMBOL; }

%%
