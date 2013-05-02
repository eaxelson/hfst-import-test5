%option 8Bit batch noyylineno noyywrap nounput prefix="xre"

%{


#include <string.h>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstXeroxRules.h"

#include "xre_parse.h"
#include "xre_utils.h"


#undef YY_INPUT
#define YY_INPUT(buf, retval, maxlen)   (retval = hfst::xre::getinput(buf, maxlen))

namespace hfst { namespace xre {
extern unsigned int cr;
extern std::set<unsigned int> positions;
extern char * position_symbol;
} }

extern
void xreerror(char *text);

%}

/* c.f. Unicode Standard 5.1 D92 Table 3-7 */
U1 [\x20-\x7e]
U2 [\xc2-\xdf][\x80-\xbf]
U31 \xe0[\xa0-\xbf][\x80-\xbf]
U32 [\xe1-\xec][\x80-\xbf][\x80-\xbf]
U33 \xed[\x80-\x9f][\x80-\xbf]
U34 [\xee-\xef][\x80-\xbf][\x80-\xbf]
U41 \xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]
U42 [\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]
U43 \xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf]
/* non US-ASCII */
U8H {U43}|{U42}|{U41}|{U34}|{U33}|{U32}|{U31}|{U2}
/* any UTF-8 */
U8C {U8H}|{U1}
/* Escaped */
EC "%"{U8C}

/* any ASCII */
A7 [\x00-\x7e]
/* special meaning in xre */
A7RESTRICTED [- |<>%!,^:;@0~\\&?$+*/_(){}\]\[-]
/* non-restricted ASCII */
A7UNRESTRICTED [\x21-\x7e]{-}[- |<>%!,^:;@0~\\&?$+*/_(){}\]\[-]

WEIGHT [0-9]+(\.[0-9]+)?

/* token character */
NAME_CH {A7UNRESTRICTED}|{U8H}|{EC}
UINTEGER [1-9][0-9]*
INTEGER -?[1-9][0-9]*
WSP [\t ]
LWSP [\t\r\n ]

/* curly brackets */
BRACED      [{]([^}]|[\300-\337].|[\340-\357]..|[\360-\367]...)+[}]
%%




{BRACED} {
  hfst::xre::cr += (unsigned int)strlen(xretext);
  xrelval.label = hfst::xre::strip_curly(xretext);
  return CURLY_BRACKETS;
}


"~"   { hfst::xre::cr += (unsigned int)strlen(xretext); return COMPLEMENT; }
"\\"  { hfst::xre::cr += (unsigned int)strlen(xretext); return TERM_COMPLEMENT; }
"&"   { hfst::xre::cr += (unsigned int)strlen(xretext); return INTERSECTION; }
"-"   { hfst::xre::cr += (unsigned int)strlen(xretext); return MINUS; }

"$."  { hfst::xre::cr += (unsigned int)strlen(xretext); return CONTAINMENT_ONCE; }
"$?"  { hfst::xre::cr += (unsigned int)strlen(xretext); return CONTAINMENT_OPT; }
"$"   { hfst::xre::cr += (unsigned int)strlen(xretext); return CONTAINMENT; }

"+"   { hfst::xre::cr += (unsigned int)strlen(xretext); return PLUS; }
"*"   { hfst::xre::cr += (unsigned int)strlen(xretext); return STAR; }

"./." { hfst::xre::cr += (unsigned int)strlen(xretext); return IGNORE_INTERNALLY; }
"/"   { hfst::xre::cr += (unsigned int)strlen(xretext); return IGNORING; }

"|"   { hfst::xre::cr += (unsigned int)strlen(xretext); return UNION; }

"<>"  { hfst::xre::cr += (unsigned int)strlen(xretext); return SHUFFLE; }
"<"   { hfst::xre::cr += (unsigned int)strlen(xretext); return BEFORE; }
">"   { hfst::xre::cr += (unsigned int)strlen(xretext); return AFTER; }

".o." { hfst::xre::cr += (unsigned int)strlen(xretext); return COMPOSITION; }
".O." { hfst::xre::cr += (unsigned int)strlen(xretext); return LENIENT_COMPOSITION; }
".x." { hfst::xre::cr += (unsigned int)strlen(xretext); return CROSS_PRODUCT; }
".P." { hfst::xre::cr += (unsigned int)strlen(xretext); return UPPER_PRIORITY_UNION; }
".p." { hfst::xre::cr += (unsigned int)strlen(xretext); return LOWER_PRIORITY_UNION; }
".-u." { hfst::xre::cr += (unsigned int)strlen(xretext); return UPPER_MINUS; }
".-l." { hfst::xre::cr += (unsigned int)strlen(xretext); return LOWER_MINUS; }
"`" {   hfst::xre::cr += (unsigned int)strlen(xretext); return SUBSTITUTE_LEFT; }

"\\<=" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_RESTRICTION; }
"<=>" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_RIGHT_ARROW; }
"<=" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_ARROW; }
"=>" { hfst::xre::cr += (unsigned int)strlen(xretext); return RIGHT_ARROW; }

"->" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_RIGHT; }
"(->)" { hfst::xre::cr += (unsigned int)strlen(xretext); return OPTIONAL_REPLACE_RIGHT; }
"<-" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_LEFT; }
"(<-)" { hfst::xre::cr += (unsigned int)strlen(xretext); return OPTIONAL_REPLACE_LEFT; }
"<->" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_LEFT_RIGHT; }
"(<->)" { hfst::xre::cr += (unsigned int)strlen(xretext); return OPTIONAL_REPLACE_LEFT_RIGHT; }
"@->" { hfst::xre::cr += (unsigned int)strlen(xretext); return LTR_LONGEST_MATCH; }
"@>" { hfst::xre::cr += (unsigned int)strlen(xretext); return LTR_SHORTEST_MATCH; }
"->@" { hfst::xre::cr += (unsigned int)strlen(xretext); return RTL_LONGEST_MATCH; }
">@" { hfst::xre::cr += (unsigned int)strlen(xretext); return RTL_SHORTEST_MATCH; }

"||" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_CONTEXT_UU; }
"//" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_CONTEXT_LU; }
"\\\\" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_CONTEXT_UL; }
"\\//" { hfst::xre::cr += (unsigned int)strlen(xretext); return REPLACE_CONTEXT_LL; }
"_"+ { hfst::xre::cr += (unsigned int)strlen(xretext); return CENTER_MARKER; }
"..."+ { hfst::xre::cr += (unsigned int)strlen(xretext); return MARKUP_MARKER; }

"\\\\\\" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_QUOTIENT; }

"^"{UINTEGER}","{UINTEGER} { 
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.values = hfst::xre::get_n_to_k(xretext);
    return CATENATE_N_TO_K;
}

"^{"{UINTEGER}","{UINTEGER}"}" {
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.values = hfst::xre::get_n_to_k(xretext);
    return CATENATE_N_TO_K;
}

"^>"{UINTEGER} { 
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.value = strtol(xretext + 2, 0, 10);
    return CATENATE_N_PLUS; 
}

"^<"{UINTEGER} { 
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.value = strtol(xretext + 2, 0, 10);
    return CATENATE_N_MINUS;
}

"^"{UINTEGER}                  { 
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.value = strtol(xretext + 1, 0, 10);
    return CATENATE_N;
}

".r" { hfst::xre::cr += (unsigned int)strlen(xretext); return REVERSE; }
".i" { hfst::xre::cr += (unsigned int)strlen(xretext); return INVERT; }
".u" { hfst::xre::cr += (unsigned int)strlen(xretext); return UPPER; }
".l" { hfst::xre::cr += (unsigned int)strlen(xretext); return LOWER; }

"@bin\""[^""]+"\""|"@\""[^""]+"\"" { 
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::get_quoted(xretext);
    return READ_BIN;
}

"@txt\""[^""]+"\"" {
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::get_quoted(xretext);
    return READ_TEXT;
}

"@stxt\""[^""]+"\"" {
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::get_quoted(xretext);
    return READ_SPACED;
}

"@pl\""[^""]+"\"" {
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::get_quoted(xretext);
    return READ_PROLOG;
}

"@re\""[^""]+"\"" {
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::get_quoted(xretext);
    return READ_RE;
}

"[." { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_BRACKET_DOTTED; }
".]" { hfst::xre::cr += (unsigned int)strlen(xretext); return RIGHT_BRACKET_DOTTED; }
"[" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_BRACKET; }
"]" { hfst::xre::cr += (unsigned int)strlen(xretext); return RIGHT_BRACKET; }
"(" { hfst::xre::cr += (unsigned int)strlen(xretext); return LEFT_PARENTHESIS; }
")" { hfst::xre::cr += (unsigned int)strlen(xretext); return RIGHT_PARENTHESIS; }


{LWSP}":"{LWSP} { hfst::xre::cr += (unsigned int)strlen(xretext); return PAIR_SEPARATOR_SOLE; }
^":"$ { hfst::xre::cr += (unsigned int)strlen(xretext); return PAIR_SEPARATOR_SOLE; }
{LWSP}":" { hfst::xre::cr += (unsigned int)strlen(xretext); return PAIR_SEPARATOR_WO_LEFT; }
":"{LWSP} { hfst::xre::cr += (unsigned int)strlen(xretext); return PAIR_SEPARATOR_WO_RIGHT; }
":" { hfst::xre::cr += (unsigned int)strlen(xretext); return PAIR_SEPARATOR; }

"::"{WEIGHT} {
    hfst::xre::cr += (unsigned int)strlen(xretext); 
    xrelval.weight = hfst::xre::get_weight(xretext + 2);
    return WEIGHT;
}

"\""[^""]+"\"" {
    hfst::xre::cr += (unsigned int)strlen(xretext); 
    xrelval.label = hfst::xre::parse_quoted(xretext); 
    return QUOTED_LITERAL;
}

",," { hfst::xre::cr += (unsigned int)strlen(xretext); return COMMACOMMA; }
"," { hfst::xre::cr += (unsigned int)strlen(xretext); return COMMA; }

"\"\"" { hfst::xre::cr += (unsigned int)strlen(xretext); return EPSILON_TOKEN; }
"0" { hfst::xre::cr += (unsigned int)strlen(xretext); return EPSILON_TOKEN; }
"[]" { hfst::xre::cr += (unsigned int)strlen(xretext); return EPSILON_TOKEN; }
"?" { hfst::xre::cr += (unsigned int)strlen(xretext); return ANY_TOKEN; }

{NAME_CH}+ {
    if (hfst::xre::position_symbol != NULL) {
      if (strcmp(hfst::xre::position_symbol, xretext) == 0) {
        hfst::xre::positions.insert(hfst::xre::cr);
      }
    }
    hfst::xre::cr += (unsigned int)strlen(xretext);
    xrelval.label = hfst::xre::strip_percents(xretext);
    return SYMBOL;
}  

";\t"{WEIGHT} {
    hfst::xre::cr += (unsigned int)strlen(xretext); 
    xrelval.weight = hfst::xre::get_weight(xretext + 2);
    return END_OF_WEIGHTED_EXPRESSION;
}

";" { 
    hfst::xre::cr += (unsigned int)strlen(xretext); 
    return END_OF_EXPRESSION;
}

{LWSP}* { hfst::xre::cr += (unsigned int)strlen(xretext); /* ignorable whitespace */ }

("!"|"#")[^\n]*$ { hfst::xre::cr += (unsigned int)strlen(xretext); /* ignore comments */ }

. { 
    hfst::xre::cr += (unsigned int)strlen(xretext); 
    return LEXER_ERROR;
}

%%
