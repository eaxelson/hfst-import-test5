%option 8Bit batch yylineno noyywrap

%{
  // Autotools stuff
  #include "../config.h"
  #include <hfst2/hfst.h>
  #include <cstdlib>
  #include <vector>
  #include <map>
  #include <iostream>
  #include <fstream>

  #include "CommandLineParser.h"
  #include "GrammarCompiler.h"

  // The parser. Supplies flex with its symbol tables.
  #include "htwolcpre3.h" 


  // Functions for manipulating strings.
  #include "string_manipulation.h"

  // input_defs.h declares the inputHandler, which is
  // an object that overrides flex' default input reading.
  #include "input_defs.h"

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
LEFT_PAIR_RANGE           :\[:
RIGHT_PAIR_RANGE           :\]:

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
NAME_CH                          [^!|&\"+*\:;\n\t\^<>=\[\]\-$~%\\()= ]
SPECIAL_CH                       [!|&\"+*\:;\n\t\^<>=\[\]\-$~()=\\ ]
ESCAPABLE_CH                     [!|&\"_+*0\:;\t?\^<>=\{\}\[\]\-$~()\%=\\ ]
RULE_NAME_CH                     [^\"\n!]   
WHITE_SPACE                      [ \t]
NON_WHITE_SPACE                  [^ \t]
INTEGER                          -?[1-9][0-9]*
KILL_SYMBOL                      \_\_HFST\_TWOLC\_DIE
%%

{ALPHABET_DECL}    { return ALPHABET_DECLARATION; }
{SETS_DECL}        { return SETS_DECLARATION; }
{RULES_DECL}       { return RULES_DECLARATION; }

{WHITE_SPACE}           { /* spaces and tabs: ignore */ }
\n                      { /* ignore */ }

^[ \t]*\"{RULE_NAME_CH}*\"    { 
                                yylval.name = string_copy(yytext); 
				return RULE_NAME; 
                              }

{STAR_LITERAL}                   { return STAR; }
{PLUS_LITERAL}                   { return PLUS; }
{FREELY_INSERT_LITERAL}          {return FREELY_INSERT; }
{COMPLEMENT_LITERAL}             {return COMPLEMENT; }
{TERM_COMPLEMENT_LITERAL}         {return TERM_COMPLEMENT; }
{CONTAINMENT_LITERAL_ONCE}       {return CONTAINMENT_ONCE; }
{CONTAINMENT_LITERAL}            {return CONTAINMENT; }
{UNION_LITERAL}[\n \t]*          {return UNION; }
{INTERSECTION_LITERAL}[\n \t]*   {return INTERSECTION; }
{POWER_LITERAL}                  {return POWER; }
{DIFFERENCE_LITERAL}             {return DIFFERENCE; }

{LEFT_RANGE}             {return LEFT_SQUARE_BRACKET; }
{RIGHT_RANGE}            {return RIGHT_SQUARE_BRACKET; }
{LEFT_PAREN}             {return LEFT_BRACKET; }
{RIGHT_PAREN}            {return RIGHT_BRACKET; }
{LEFT_PAIR_RANGE}        {return LEFT_PAIR_BRACKET; }   
{RIGHT_PAIR_RANGE}       {return RIGHT_PAIR_BRACKET; }
{LEFT_RESTRICTION_OP}\n* {return LEFT_RESTRICTION_ARROW; }
{LEFT_OP}\n*             {return LEFT_ARROW; }
{RIGHT_OP}\n*            {return RIGHT_ARROW; }
{LEFT_RIGHT_OP}\n*       {return LEFT_RIGHT_ARROW; }

{PAIR_SEPARATOR_LITERAL} {return PAIR_SEPARATOR; }

{EOL_LITERAL}           {return EOL; }
{EQ_LITERAL}            {return EQUALS; }
{CENTER_MARKER_LITERAL} {return CENTER_MARKER; }

{KILL_SYMBOL}           { exit(1); }
({NAME_CH}|%{ESCAPABLE_CH}?)+ {yylval.value = 
                               unescape_and_remove_white_space(yytext);
                               return SYMBOL;}

%%
