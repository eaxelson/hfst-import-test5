%option 8Bit batch yylineno noyywrap

%{
#include <stdio.h>

static
void
skip_spaces(char **s)
{
    while ((**s != '\0') && (is_space(**s)))
      {
        *s++;
      }
}
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

/* linear whitespace */
LWSP [\t ]
/* whitespace */
WSP [\n\t ]
/* newlines */
NL \n|\r|\r\n

/* reserced from regular strings */
RESERVED [*/# \t\n\r]
/* number giving count of things */
NUMBER [0-9]|[1-9][0-9]*
/* characters used for continuation classes */
CONTCHAR [^\t\n\r /]
/* non-reserved characters used for unicode strings */
UNINR {U8H}|[\x21-\x22\x24-\x29\x2b-\x2e\x30-\x7e]
/* any unicode char except space */
UNI {U8H}|[\x21-\x7e]

%%

"#".* {
    /* comment, ignore */
}

^"SET"{WSP}+{UNINR}+$ {
    const char* s = yytext + 3;
    skip_spaces(&s);
    yylval.string = strdup(s);
    return SET_LINE;
}

^"TRY"{WSP}+{UNINR}+$ {
    const char* s = yytext + 3;
    skip_spaces(&s);
    yylval.string = strdup(s);
    return TRY_LINE;
}

^"REP"{WSP}+{NUMBER}$ {
    const char* s = yytext + 3;
    skip_spaces(&s);
    char* end = yytext;
    yylval.number = strtoul(s, end, 10);
    assert(*end == '\0');
    return REP_COUNT;
}

^"REP" {
    return REP_LEADER;
}

^"KEY"{WSP}+{UNINR}+$ {
    const char* s = yytext + 3;
    skip_spaces(&s);
    yylval.string = strdup(s);
    return KEY_LINE;
}

^"PFX" {
    return PREFIX_LEADER;
}


^"SFX" {
    return SUFFIX_LEADER;
}

^"FLAG"{WSP}+{UNI}+$ {
    const char* s = yytext + 3;
    skip_spaces(&s);
    yylval.string = strdup(s);
    return FLAG_LINE;
}

^"AF"{WSP}+{NUMBER}$ {
    const char* s = yytext + 2;
    skip_spaces(&s);
    char* end = yytext;
    yylval.number = strtoul(s, &end, 10);
    assert(*end == '\0');
    return AF_COUNT;
}

^"AF" {
    return AF_LEADER;
}

^"AM"{WSP}+{NUMBER}$ {
    const char* s = yytext + 2;
    skip_spaces(&s);
    char* end = yytext;
    yylval.number = strtoul(s, &end, 10);
    assert(*end == '\0');
    return AM_COUNT;
}

^"AM" {
    return AM_LEADER;
}

^"COMPOUNDMIN" {
    return COMPOUNMIN_LEADER;
}

^"COMPOUNDFLAG" {
    return COMPOUNDFLAG_LEADER;
}

^"COMPOUNDBEGIN" {
    return COMPOUNDBEGIN_LEADER;
}

^"COMPOUNDLAST" {
    return COMPOUNDLAST_LEADER;
}

^"COMPOUNDMIDDLE" {
    return COMPOUNDMIDDLE_LEADER;
}

^"CIRCUMFIX" {
    return CIRCUMFIX_LEADER;
}

^[A-Z]+{WSP}*.*$ {
    fprintf(stderr, "Skipped unknown line %s\n", yytext);
    yylval.string = strdup(yytext);
    return UNKNOWN_LINE;
}

"/"{NUMBER} {
    char* end = yytext;
    yylvalue.number = strtoul(yytext+1, end, 10);
    return CONTNUMBER;
}

"/"{UNINR}+ {
    yylvalue.string = strdup(yytext+1);
    return CONTSTRING;
}

{NUMBER} {
    yylvalue.number = strtoul(yytext);
    return NUMBER;
}

"*"{UNINR}+ {
    yylvalue.string = strdup(yytext+1);
    return UTF8_ERRSTRING;
}

{UNINR}+ {
    yylvalue.string = strdup(yytext);
    return UTF8_STRING;
}

{LWSP}* {
    /* skip */
}

. {
    if (*yytext < 0)
      {
        fprintf(stderr, "Lexer error at %s [%c] on line %lu"
                "(Possibly unexpected broken UTF-8)\n",
                yytext, yytext[0], yylineno);
      }
    else
      {
        fprintf(stderr, "Lexer error at %s [%c] on line %lu\n", 
                yytext, yytext[0], yylineno);
      }
    return ERROR;
}

%%

