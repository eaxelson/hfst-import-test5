%option 8Bit batch yylineno noyywrap prefix="xre"

%{


#include <string.h>

#include "../hfst.h"

#include "xre_parse.hh"
#include "xre.h"

extern char *xre_data;
extern size_t xre_len;


#define YYDEBUG 1
#undef YY_INPUT
#define YY_INPUT(buf, retval, maxlen)	(retval = xre_getinput(buf, maxlen))


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
A7RESTRICTED [- |<>%!:;@0~\\&?$+*/_(){}\]\[-]
/* non-restricted ASCII */
A7UNRESTRICTED [\x21-\x7e]{-}[- |<>%!:;@0~\\&?$+*/_(){}\]\[-]

WEIGHT [0-9]+(\.[0-9]+)?

/* token character */
NAME_CH {A7UNRESTRICTED}|{U8H}|{EC}
UINTEGER [1-9][0-9]*
INTEGER -?[1-9][0-9]*
WSP [\t ]
LWSP [\t\r\n ]
%%

"~"  { return COMPLEMENT; }
"\\" { return TERM_COMPLEMENT; }
{LWSP}*"&"{LWSP}*  { return INTERSECTION; }
{LWSP}*"-"{LWSP}*  { return DIFFERENCE; }

"$." { return CONTAINMENT_ONCE; }
"$?" { return CONTAINMENT_OPT; }
"$"  { return CONTAINMENT; }

"+" { return PLUS; }
"*" { return STAR; }

{LWSP}*"./."{LWSP}* { return IGNORE_INTERNALLY; }
{LWSP}*"/"{LWSP}*   { return IGNORING; }

^{WSP}* {/* ignore initial whitespace */}
{WSP}* {/* ignore final whitespace */}
{LWSP}*{WSP}+{LWSP}* { return CONCATENATE; }
{LWSP}*"|"{LWSP}*  { return UNION; }

{LWSP}*"<>"{LWSP}* { return SHUFFLE; }
{LWSP}*"<"{LWSP}* { return BEFORE; }
{LWSP}*">"{LWSP}* { return AFTER; }

{LWSP}*".o."{LWSP}* { return COMPOSITION; }
{LWSP}*".x."{LWSP}* { return CROSS_PRODUCT; }

"\\<=" { return LEFT_RESTRICTION; }
"<=>" { return LEFT_RIGHT_ARROW; }
"<=" { return LEFT_ARROW; }
"=>" { return RIGHT_ARROW; }
"->" { return REPLACE_ARROW; }
"(->)" { return OPTIONAL_REPLACE; }
"@->" { return LTR_LONGEST_REPLACE; }
"@>" { return LTR_SHORTEST_REPLACE; }
"->@" { return RTL_LONGEST_REPLACE; }
">@" { return RTL_SHORTEST_REPLACE; }
"||" { return REPLACE_CONTEXT_UU; }
"//" { return REPLACE_CONTEXT_LU; }
"\\\\" { return REPLACE_CONTEXT_UL; }
"\\//" { return REPLACE_CONTEXT_LL; }
"_"+ { return CENTER_MARKER; }

".#." { return BOUNDARY_MARKER; }

"^"{UINTEGER}","{UINTEGER} { 
	int* values = (int*)malloc(sizeof(int) * 2);
	char *endptr;
	values[0] = strtol(xretext + 1, &endptr, 10);
	values[1] = strtol(endptr + 1, 0, 10);
	xrelval.values = values;
	return CATENATE_N_TO_K;
}

"^{"{UINTEGER}","{UINTEGER}"}" {
	int* values = (int*)malloc(sizeof(int) * 2);
	char *endptr;
	values[0] = strtol(xretext + 2, &endptr, 10);
	values[1] = strtol(endptr + 1, 0, 10);
	xrelval.values = values;
	return CATENATE_N_TO_K;
}

"^>"{UINTEGER}				   { 
	xrelval.value = strtol(xretext + 2, 0, 10);
	return CATENATE_N_PLUS; 
}

"^<"{UINTEGER}				   { 
	xrelval.value = strtol(xretext + 2, 0, 10);
	return CATENATE_N_MINUS;
}

"^"{UINTEGER}				   { 
	xrelval.value = strtol(xretext + 1, 0, 10);
	return CATENATE_N;
}

".r" { return REVERSE; }
".i" { return INVERT; }
".u" { return UPPER; }
".l" { return LOWER; }

"@bin\""[^""]+"\""|"@\""[^""]+"\"" { 
	xrelval.name = xre_get_quoted(xretext);
	return READ_BIN;
}

"@txt\""[^""]+"\"" {
	xrelval.name = xre_get_quoted(xretext);
	return READ_TEXT;
}

"@stxt\""[^""]+"\"" {
	xrelval.name = xre_get_quoted(xretext);
	return READ_SPACED;
}

"@pl\""[^""]+"\"" {
	xrelval.name = xre_get_quoted(xretext);
	return READ_PROLOG;
}

"["{LWSP}* { return LEFT_BRACKET; }
{LWSP}*"]" { return RIGHT_BRACKET; }
"("{LWSP}* { return LEFT_PARENTHESIS; }
{LWSP}*")" { return RIGHT_PARENTHESIS; }
"{"{LWSP}* { return LEFT_CURLY; }
{LWSP}*"}" { return RIGHT_CURLY; }

{LWSP}":"{LWSP} { return PAIR_SEPARATOR_SOLE; }
{LWSP}":" { return PAIR_SEPARATOR_WO_LEFT; }
":"{LWSP} { return PAIR_SEPARATOR_WO_RIGHT; }
":" {  return PAIR_SEPARATOR; }

[^ \t\r\n]":"$ { return PAIR_SEPARATOR_WO_RIGHT; }
^":"[^ \t\r\n] { return PAIR_SEPARATOR_WO_LEFT; }

"\""[^""]+"\"" {
	xrelval.name = xre_get_quoted(xretext); 
	return QUOTED_LITERAL;
}

"0" { return EPSILON_TOKEN; }
"?" { return ANY; }

{NAME_CH}+ {
	xrelval.name = xre_strip_percents(xretext);
	return SYMBOL;
}  

";\t"{WEIGHT} {
    const char* weightstart = xretext + 2;
    while ((*weightstart != '\0') && 
           ((*weightstart == ' ') || (*weightstart == '\t')))
    {
        weightstart++;
    }
    char* endp;
    xrelval.weight = strtod(weightstart, &endp);
    if (endp == weightstart)
    {
        return ERROR;
    }
    return END_OF_WEIGHTED_EXPRESSION;
}

";" { 
	return END_OF_EXPRESSION;
}

"!"[^\n]*$ { /* ignore comments */ }

. { 
	return ERROR;
}
%%
