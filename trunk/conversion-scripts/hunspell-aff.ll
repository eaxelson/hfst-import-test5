%option 8Bit batch yylineno noyywrap

%{
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "hunspell-aff.tab.hh"

static
void
skip_spaces(char **s)
{
    while ((**s != '\0') && (isspace(**s)))
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
NUMBER [1-9][0-9]*
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

^"SET "{UNINR}+ {
    yylval.string = strdup(yytext + 4);
    return SET_LINE;
}

^"TRY "{UNINR}+ {
    yylval.string = strdup(yytext + 4);
    return TRY_LINE;
}

^"REP "{NUMBER}{LWSP}*$ {
    char* s = strdup(yytext + 4);
    char* end = yytext;
    yylval.number = strtoul(s, &end, 10);
    if (*end != '\0')
    {
        fprintf(stderr, "Junk at the end of REP line: %s\n", end);
    }
    return REP_COUNT;
}

^"REP " {
    return REP_LEADER;
}

^"KEY "{UNINR}+ {
    yylval.string = strdup(yytext + 4);
    return KEY_LINE;
}

^"PFX "." "("Y"|"N")" "{NUMBER}{LWSP}*$ {
    char* s = strdup(yytext + 8);
    char* end = yytext;
    yylval.number = strtoul(s, &end, 10);
    if (*end != '\0')
    {
        fprintf(stderr, "Junk at the end of PFX line: %s\n", end);
    }
    return PFX_FIRSTLINE;
}


^"SFX "." "("Y"|"N")" "{NUMBER}{LWSP}*$ {
    char* s = strdup(yytext + 8);
    char* end = yytext;
    yylval.number = strtoul(s, &end, 10);
    if (*end != '\0')
    {
        fprintf(stderr, "Junk at the end of SFX line: %s\n", end);
    }
    return SFX_FIRSTLINE;
}

^"PFX ". {
    yylval.string = strdup(yytext + 4);
    return PFX_LEADER;
}

^"SFX ". {
    yylval.string = strdup(yytext + 4);
    return SFX_LEADER;
}

^"FLAG"{WSP}+{UNI}+$ {
    char* s = strdup(yytext + 3);
    skip_spaces(&s);
    yylval.string = s;
    return FLAG_LINE;
}

^"AF "{NUMBER}$ {
    char* end = yytext;
    yylval.number = strtoul(yytext + 3, &end, 10);
    assert(*end == '\0');
    return AF_COUNT;
}

^"AF "[^# \t\r\n]+ {
    yylval.string = strdup(yytext + 3);
    return AF_LINE;
}

^"AM "{NUMBER}$ {
    char* end = yytext;
    yylval.number = strtoul(yytext + 3, &end, 10);
    assert(*end == '\0');
    return AM_COUNT;
}

^"AM "[^#\t\n\r]+ {
    return AM_LINE;
}

^"COMPOUNDMIN "{NUMBER} {
    char* end = yytext;
    yylval.number = strtoul(yytext + 12, &end, 10);
    assert(*end == '\0');
    return COMPOUNDMIN_LINE;
}

^"COMPOUNDFLAG ". {
    yylval.string = strdup(yytext + 13);
    return COMPOUNDFLAG_LINE;
}

^"COMPOUNDBEGIN ". {
    yylval.string = strdup(yytext + 14);
    return COMPOUNDBEGIN_LINE;
}

^"COMPOUNDEND ". {
    yylval.string = strdup(yytext + 13);
    return COMPOUNDEND_LINE;
}

^"COMPOUNDMIDDLE ". {
    yylval.string = strdup(yytext + 16);
    return COMPOUNDMIDDLE_LINE;
}

^"CIRCUMFIX ". {
    yylval.string = strdup(yytext + 10);
    return CIRCUMFIX_LINE;
}

^("NOSUGGEST "|"ONLYINCOMPOUND "|"COMPOUNDRULE "|"WORDCHARS "|"PHONE "|"NAME "|"LANG "|"HOME "|"VERSION "|"COMPOUNDSYLLABLE "|"SYLLABLENUM "|"KEEPCASE "|"COMPOUNDFORBIDFLAG "|"COMPOUNDPERMITFLAG "|"COMPOUNDPERMITFLAG "|"COMPOUNDFIRST "|"COMPOUNDLAST "|"ONLYROOT "|"HU_KOTOHANGZO "|"NEEDAFFIX "|"ONLYINCOMPOUND "|"COMPOUNDWORDMAX "|"COMPOUNDROOT "|"CHECKCOMPOUND"|"FORBIDDENWORD "|"SUBSTANDARD "|"GENERATE "|"LEMMA_PRESENT "|"MAP "|"BREAK "|"CHECKSHARPS").*$ {
    printf("Skipped (known): %s\n", yytext);
}

"/"{NUMBER} {
    char* end = yytext;
    yylval.number = strtoul(yytext+1, &end, 10);
    return CONTNUMBER;
}

"/"{UNINR}+ {
    yylval.string = strdup(yytext+1);
    return CONTSTRING;
}

"0" {
    return ZERO;
}

{NUMBER} {
    char* end = yytext;
    yylval.number = strtoul(yytext, &end, 10);
    return COUNT;
}

"*"{UNINR}+ {
    yylval.string = strdup(yytext+1);
    return UTF8_ERRSTRING;
}

{UNINR}+ {
    yylval.string = strdup(yytext);
    return UTF8_STRING;
}

{WSP}* {
    /* skip */
}

. {
    if (*yytext < 0)
      {
        fprintf(stderr, "Lexer error at %s [%c] on line %d"
                "(Possibly unexpected broken UTF-8)\n",
                yytext, yytext[0], yylineno);
      }
    else
      {
        fprintf(stderr, "Lexer error at %s [%c] on line %d\n", 
                yytext, yytext[0], yylineno);
      }
    return ERROR;
}

%%

