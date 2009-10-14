%option 8Bit batch yylineno noyywrap

%{
  // Autotools stuff
  #include "../config.h"

  // typedefs and constants needed.
  #include "grammar_defs.h"
  #include "rule_parts.h"
  #include "RuleModifier.h"

  #include <cstdlib>
  #include <vector>
  #include <map>
  #include <iostream>
  #include <fstream>

  // The parser. Supplies flex with its symbol tables.
  #include "htwolcpre.h" 

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
ANY_LITERAL                       \?|\_\_HFST\_TWOLC\_ANY\_SYMBOL
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
ZERO                             0|\_\_HFST\_TWOLC\_EPSILON\_SYMBOL
%%

{ALPHABET_DECL}    { grammar_displayer.display_alphabet_declaration();
                     return ALPHABET_DECLARATION; }
{DIACRITICS_DECL}  { return DIACRITICS_DECLARATION; }
{VARIABLE_DECL}    { return VARIABLE_DECLARATION; }
{DEFINITIONS_DECL} { return DEFINITION_DECLARATION; }
{SETS_DECL}        { grammar_displayer.display_sets_declaration();
                     return SETS_DECLARATION; }
{RULES_DECL}       { grammar_displayer.display_rules_declaration();
                     return RULES_DECLARATION; }
{WHERE}            { return WHERE; }
{MATCHED}          { return MATCHED; }
{MIXED}            { return MIXED; }
{FREELY}           { return FREELY; }
{IN}               { return IN; }

{COMMENT_BEGINS}.*      { /* comments: ignore    */ }
{WHITE_SPACE}           { /* spaces and tabs: ignore */ }
\n                      { /* ignore */ }

^[ \t]*\"{RULE_NAME_CH}*\"    { 
                                yylval.name = unquote(yytext); 
                                return RULE_NAME; 
                              }

[a]nd                            { return AND; }
{STAR_LITERAL}                   { return STAR; }
{PLUS_LITERAL}                   { return PLUS; }
{FREELY_INSERT_LITERAL}          { return FREELY_INSERT; }
{COMPLEMENT_LITERAL}             { return COMPLEMENT; }
{TERM_COMPLEMENT_LITERAL}        { return TERM_COMPLEMENT; }
{CONTAINMENT_LITERAL_ONCE}       { return CONTAINMENT_ONCE; }
{CONTAINMENT_LITERAL}            { return CONTAINMENT; }
{ANY_LITERAL}                    { return ANY; }
[\t ]{ANY_LITERAL}[\t ]          { return LONELY_ANY; }
^{ANY_LITERAL}[\t ]              { return LONELY_ANY; }
[\t ]{ANY_LITERAL}$              { return LONELY_ANY; }
^{ANY_LITERAL}$                  { return LONELY_ANY; }
{UNION_LITERAL}[\n \t]*          { return UNION; }
{INTERSECTION_LITERAL}[\n \t]*   { return INTERSECTION; }
{POWER_LITERAL}                  { return POWER; }
{DIFFERENCE_LITERAL}             { return DIFFERENCE; }
{INTEGER}                        { 
                                   yylval.value = string_copy(yytext); 
                                   return NUMBER; 
                                 }
{ZERO}                           { return EPSILON; }
[\t ]{ZERO}[\t ]                 { return EPSILON; }
^{ZERO}[\t ]                     { return EPSILON; }
[\t ]{ZERO}$                     { return EPSILON; }
^{ZERO}$                         { return EPSILON; }

{LEFT_RANGE}             { return LEFT_SQUARE_BRACKET; }
{RIGHT_RANGE}            { return RIGHT_SQUARE_BRACKET; }
{LEFT_PAREN}             { return LEFT_BRACKET; }
{RIGHT_PAREN}            { return RIGHT_BRACKET; }

{LEFT_RESTRICTION_OP}\n* { return LEFT_RESTRICTION_ARROW; }
{LEFT_OP}\n*             { return LEFT_ARROW; }
{RIGHT_OP}\n*            { return RIGHT_ARROW; }
{LEFT_RIGHT_OP}\n*       { return LEFT_RIGHT_ARROW; }

{PAIR_SEPARATOR_LITERAL_RIGHT}/{SPECIAL_CH} { return PAIR_SEPARATOR_BOTH; }
{PAIR_SEPARATOR_LITERAL_RIGHT}              { return PAIR_SEPARATOR_RIGHT; }
{PAIR_SEPARATOR_LITERAL}/{SPECIAL_CH}       { return PAIR_SEPARATOR_LEFT; }
{PAIR_SEPARATOR_LITERAL}                    { return PAIR_SEPARATOR; }

{EOL_LITERAL}           { return EOL; }
{EQ_LITERAL}            { return EQUALS; }
{CENTER_MARKER_LITERAL} { return CENTER_MARKER; }

({NAME_CH}|%{ESCAPABLE_CH}?)+ { yylval.value = remove_white_space(yytext);
                                return SYMBOL; }

%%
