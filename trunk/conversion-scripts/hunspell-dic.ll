%option 8Bit batch yylineno noyywrap

%{
#include <stdio.h>
#include <assert.h>

#include "hunspell-dic.tab.hh"
%}

/* c.f. Unicode Standard 5.1 D92 Table 3-7 */
U1 [\x20-\x7e]
U2 [\xc2-\xdf][\x80-\xbf]
U31 \xe0[\xa0-\xbf][\x80-\xbf]
U32 [\xe1-\xec][\x80-\xbf][\x80-\xbf]
U33 \xed[\x80-\x9f][\x80-\xbf]
U34 [\xee-\xef][\x80-\xbf][\x80-\xbf]
U3 {U34}|{U33}|{U32}|{U31}
U41 \xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]
U42 [\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]
U43 \xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf]
U4 {U43}|{U42}|{U41}
/* non US-ASCII */
U8H {U4}|{U3}|{U2}
/* any UTF-8 */
U8C {U8H}|{U1}

/* printable ASCII */
A7P [\x20-\x7e]
/* non-printable */
A7N [\x00-\x1f]|\x7f
/* any ASCII */
A7 {A7P}|{A7N}

/* number giving count of things */
NUMBER [0-9]|[1-9][0-9]*
/* characters used for continuation classes */
CONTCHAR [^\t\n\r /]
/* non-reserved characters used for unicode strings */
UNINR {U8H}|[\x20-\x2e\x30-\x7e]
/* any unicode char except space */
UNI {U8H}|[\x20-\x7e]
/* any nonreserved char */
NR {UNINR}|[\x7f-\xff]

LWSP [ \t]
WSP [ \t\n\r]

%%

^{LWSP}+.* {
    /* this is apparently a comment (cf. de_DE.dic) */
}

^{NUMBER}$ {
    char* end = yytext;
    yylval.number = strtoul(yytext, &end, 10);
    assert ((end != yytext) && (*end == '\0'));
    return COUNT;
}

^{UNINR}+ {
    yylval.string = strdup(yytext);
    return WORD;
}

"/"{NR}* {
    yylval.string = strdup(yytext+1);
    return CONT_THING;
}

"\t"{NUMBER} {
    char* end = yytext;
    yylval.number = strtoul(yytext, &end, 10);
    assert ((end != yytext) && (*end == '\0'));
    return EXTRA_NUMBER;
}

"\t"{UNINR}+ {
    yylval.string = strdup(yytext);
    return EXTRA_STRING;
}

\n {
    /* skip new lines */
}

. {
    if (*yytext < 0) 
      {
        fprintf(stderr, "Lexer error at %s (byte %d) on line %u "
                "(not UTF-8 data?)\n", yytext, *yytext, yylineno);
      }
    else
      {
        fprintf(stderr, "Lexer error at %s (byte %d) on line %u\n",
                yytext, *yytext, yylineno);
      }
    return ERROR;
}

%%

