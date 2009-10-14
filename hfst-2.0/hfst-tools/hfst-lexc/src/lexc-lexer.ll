%option 8Bit batch yylineno noyywrap

%{
#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include "lexc-parser.h"
#include "flex-utils.h"
#include "string-munging.h"
#include "lexcio.h"

#include <assert.h>

extern void hlexcerror(const char *text);

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
/* special meaning in lexc */
A7RESTRICTED [ <>%!;:""]
/* non-restricted ASCII */
A7UNRESTRICTED [\x21-\x7e]{-}[ <>%!;:""]
/* special meaning in xre */
XRERESTRICTED [- <>%!:;@0~\\&?$+*/_{}\]\[-]
/* non-restricted ASCII in XRE */
XREUNRESTRICTED [\x21-\x7e]{-}[- <>%!:;@0~\\&?$+*/_{}\]\[-]


/* RegExp. stuff */
XREALPHA {XREUNRESTRICTED}|{U8H}|{EC}
XREOPERATOR [~\\&-/ |+*$_\]\[{}()0?:""]|"@\""|"$."|"$?"|"./."|"<>"|".#."|"."[riul]
XRECHAR {XREOPERATOR}|{XREALPHA}
XRETOKEN {XRECHAR}+

/* String entry part:
 * - any unrestricted ASCII-7,
 * - any high unicode codepoint (past U+007F) UTF-8-encoded
 * - any %-escaped UTF-8
 */
STRINGCHAR {A7UNRESTRICTED}|{U8H}|{EC}
STRINGTOKEN {STRINGCHAR}+

/* Lexicon name part */
LEXICONCHAR {A7UNRESTRICTED}|{U8H}|{EC}
LEXICONNAME {LEXICONCHAR}+


/* White space */
WSP [\t ]
LWSP [\r\n\t ]

%x METADATA ALPHABET MULTICHARS DEFINITIONS LEXICONS ENDED
%%


<INITIAL>^{WSP}*"Metadata"{LWSP}+ {
	BEGIN METADATA;
	token_update_positions(hlexctext);
	return METADATA_START;
}

<INITIAL,METADATA>^{WSP}*"Alphabet"{LWSP}+ {
	BEGIN ALPHABET;
	token_update_positions(hlexctext);
	return ALPHABET_START;
}

<INITIAL,METADATA,ALPHABET>^{WSP}*"Multichar_Symbols"{LWSP}+ {
	BEGIN MULTICHARS;
	token_update_positions(hlexctext);
	return MULTICHARS_START;
}

<INITIAL,METADATA,ALPHABET,MULTICHARS>^{WSP}*("Definitions"|"Declarations"){LWSP}+ {
	BEGIN DEFINITIONS;
	token_update_positions(hlexctext);
	return DEFINITIONS_START;
}

<INITIAL,METADATA,ALPHABET,MULTICHARS,DEFINITIONS>^{WSP}*"LEXICON"{WSP}+{LEXICONNAME} {
	BEGIN LEXICONS;
	token_update_positions(hlexctext);
	char* lexicon_start;
	lexicon_start = strstrip(hlexctext);
	hlexclval.name = strdup_nonconst_part(lexicon_start, "LEXICON",
										  NULL, true);
	free(lexicon_start);
	return LEXICON_START;
}

<INITIAL,METADATA,ALPHABET,MULTICHARS,DEFINITIONS>^{WSP}*"END"{LWSP}+ {
	token_update_positions(hlexctext);
	return END_START;
}

<INITIAL>!.* {
	token_update_positions(hlexctext);
}

<INITIAL>{LWSP} {
	token_update_positions(hlexctext);
}


<METADATA>^[^:]* {
	token_update_positions(hlexctext);
	hlexclval.name = strstrip(hlexctext);
	return METADATA_FIELDNAME;
}

<METADATA>":" {
	token_update_positions(hlexctext);
	return ':';
}

<METADATA>[^:]*$ {
	token_update_positions(hlexctext);
	hlexclval.name = strstrip(hlexctext);
	return METADATA_VALUE;
}

<METADATA>[\r\n] { token_update_positions(hlexctext); }

<ALPHABET>{STRINGTOKEN} {
	token_update_positions(hlexctext);
	hlexclval.name = strip_percents(hlexctext, false);
	return ALPHABET_SYMBOL;
}

<ALPHABET>!.* { token_update_positions(hlexctext); }

<ALPHABET>{LWSP} { token_update_positions(hlexctext); }

<MULTICHARS>{STRINGTOKEN} {
	token_update_positions(hlexctext);
	hlexclval.name = strip_percents(hlexctext, false);
	return MULTICHAR_SYMBOL;
}

<MULTICHARS>!.* { token_update_positions(hlexctext); }

<MULTICHARS>{LWSP} { token_update_positions(hlexctext); }

<DEFINITIONS>{STRINGTOKEN}{WSP}*/"=" {
	token_update_positions(hlexctext);
	hlexclval.name = strstrip(hlexctext);
	return DEFINITION_NAME;
}

<DEFINITIONS>"="{XRETOKEN}";" {
	token_update_positions(hlexctext);
	hlexclval.name = strdup_nonconst_part(hlexctext, "=", ";", false);
	return DEFINITION_EXPRESSION;
}

<DEFINITIONS>!.* { token_update_positions(hlexctext); }

<DEFINITIONS>{LWSP} { token_update_positions(hlexctext); }

<LEXICONS>^{WSP}*"LEXICON"{WSP}+{LEXICONNAME} {
	token_update_positions(hlexctext);
	char* lexicon_start;
	lexicon_start = strstrip(hlexctext);
	hlexclval.name = strdup_nonconst_part(lexicon_start, "LEXICON", 0, true);
	free(lexicon_start);
	return LEXICON_START;
}

<LEXICONS>{STRINGTOKEN} {
	token_update_positions(hlexctext);
	hlexclval.name = strip_percents(hlexctext, false);
	return ULSTRING;
}

<LEXICONS>"<"{WSP}*{XRETOKEN}{WSP}*">" {
	token_update_positions(hlexctext);
	hlexclval.name = strdup_nonconst_part(hlexctext, "<", ">", false);
	return XEROX_REGEXP;
}

<LEXICONS>{LEXICONNAME}/{WSP}*("\""|";") {
	token_update_positions(hlexctext);
	hlexclval.name = strdup(hlexctext);
	return LEXICON_NAME;
}

<LEXICONS>"\""[^\n\t""]*"\"" {
	token_update_positions(hlexctext);
	hlexclval.name = strdup(hlexctext);
	return ENTRY_GLOSS;
}

<LEXICONS>^{WSP}*"END"{LWSP}+ {
	BEGIN ENDED;
	token_update_positions(hlexctext);
	return END_START;
}

<LEXICONS>";" {
	token_update_positions(hlexctext);
	return hlexctext[0];
}

<LEXICONS>":" {
	token_update_positions(hlexctext);
	return hlexctext[0];
}

<LEXICONS>!.* {
	token_update_positions(hlexctext);

}
<LEXICONS>{LWSP} {
	token_update_positions(hlexctext);

}

<ENDED>. { 
	token_update_positions(hlexctext);

}
<ENDED>{LWSP} {
	token_update_positions(hlexctext);
}

<*>[\x80-\xff] {
	token_update_positions(hlexctext);
	hlexcerror(_("Illegal 8-bit sequence (cannot form valid UTF-8)"));
	return ERROR;
}

<*>. {
	token_update_positions(hlexctext);
	hlexcerror(_("Syntax error in lexer (no valid token found at the point)"));
	return ERROR;
}

%%
