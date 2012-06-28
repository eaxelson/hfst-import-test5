%option 8Bit batch nounput noyywrap prefix="hxfst"

%{
//! @file xfst-lexer.ll
//!
//! @brief a lexer for xfst
//!
//! @author Tommi A. Pirinen


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include "xfst-parser.h"
#include "xfst-utils.h"

#include <assert.h>

extern void hxfsterror(const char *text);

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
/* special meaning in xfst */
A7RESTRICTED [ <>()\[\]%!;:""]
/* non-restricted ASCII */
A7UNRESTRICTED [\x21-\x7e]{-}[ <>%()\[\]!;:""]
/* special meaning in xre */
XRERESTRICTED [- <>%!:;@0~\\&?$+*/_{}\]\[-]
/* non-restricted ASCII in XRE */
XREUNRESTRICTED [\x21-\x7e]{-}[- <>%!:;@0~\\&?$+*/_{}\]\[-]
/* keywords */
KEYWORDS "read"|"regex"|"save"|"write"|"stack"

/* RegExp. stuff */
XREALPHA {XREUNRESTRICTED}|{U8H}|{EC}
XREOPERATOR [~\\&-/ |+*$_\]\[{}()0?:""]|"@\""|"$."|"$?"|"./."|"<>"|".#."|"."[riul]
XRECHAR {XREALPHA}+|{XREOPERATOR}
XRETOKEN {XRECHAR}+

/* String entry part:
 * - any unrestricted ASCII-7,
 * - any high unicode codepoint (past U+007F) UTF-8-encoded
 * - any %-escaped UTF-8
 */
NAMECHAR {A7UNRESTRICTED}|{U8H}|{EC}
NAMETOKEN [A-Za-z]{NAMECHAR}*
PROTOTYPE {NAMETOKEN}"("[a-zA-Z ,]*")"
RANGE {NAMECHAR}"-"{NAMECHAR}
NUMBER [1-9][0-9]*|[0-9]

/* White space */
WSP [\t ]
LWSP [\t ]*

%x REGEX_STATE
%%

"add properties"|"add" {
    return ADD_PROPS;
}

"apply up"|"up" {
    return APPLY_UP;
}

"apply down"|"down" {
    return APPLY_DOWN;
}

"apply med"|"med" {
    return APPLY_MED;
}

"ambiguous upper"|"ambiguous" {
    return AMBIGUOUS;
}

"alias" {
    return DEFINE_ALIAS;
}

"apropos"{WSP}+.* {
    hxfstlval.text = hfst::xfst::strstrip(hxfsttext + strlen("apropos "));
    return APROPOS;
}

"apropos"{WSP}* {
    hxfstlval.text = strdup("");
    return APROPOS;
}

"cleanup net"|"cleanup" {
    return CLEANUP;
}

"clear"|"clear stack" {
    return CLEAR;
}

"collect epsilon-loops"|"epsilon-loops" {
    return COLLECT_EPSILON_LOOPS;
}

"compact sigma" {
    return COMPACT_SIGMA;
}

"compile-replace lower"|"com-rep lower" {
    return COMPILE_REPLACE_LOWER;
}

"compile-replace upper"|"com-rep upper" {
    return COMPILE_REPLACE_UPPER;
}

"complete net"|"complete" {
    return COMPLETE;
}

"compose net"|"compose" {
    return COMPOSE;
}

"concatenate net"|"concatenate" {
    return CONCATENATE;
}

"crossproduct net"|"crossproduct" {
    return CROSSPRODUCT;
}

"define"{WSP}+{NAMETOKEN}{PROTOTYPE} {
    BEGIN(REGEX_STATE);
    hxfstlval.name = hfst::xfst::strstrip(yytext+strlen("define "));
    return DEFINE_FUNCTION;
}


"define"{WSP}+{NAMETOKEN} {
    BEGIN(REGEX_STATE);
    hxfstlval.name = hfst::xfst::strstrip(yytext+strlen("define "));
    return DEFINE_NAME;
}

"determinize net"|"determinize"|"determinise net"|"determinise" {
    return DETERMINIZE;
}

"echo"{WSP}+.*$ {
    hxfstlval.text = hfst::xfst::strstrip(hxfsttext + strlen("echo "));
    return ECHO;
}

"echo"{WSP}* {
    hxfstlval.text = strdup("");
    return ECHO;
}

"edit properties"|"edit" {
    return EDIT_PROPS;
}

"eliminate flag" {
    return ELIMINATE_FLAG;
}

"eliminate flags" {
    return ELIMINATE_ALL;
}

"epsilon-remove net"|"epsilon-remove" {
    return EPSILON_REMOVE;
}

"extract ambiguous" {
    return EXTRACT_AMBIGUOUS;
}

"extract unambiguous" {
    return EXTRACT_UNAMBIGUOUS;
}


("help"|"describe") {
    return DESCRIBE;
}

"hfst"{WSP}*.*$ {
    hxfstlval.text = strdup(hxfsttext);
    return HFST;
}

"ignore net"|"ignore" {
    return IGNORE;
}

"intersect net"|"intersect"|"conjunct" {
    return INTERSECT;
}

"inspect"|"inspect net" {
    return INSPECT;
}

"invert net"|"invert" {
    return INVERT;
}

"label net" {
    return LABEL;
}

"list" {
    return LIST;
}

"load defined"|"loadd" {
    return LOADD;
}

"load"|"load stack" {
    return LOADS;
}

"lower-side net"|"lower-side" {
    return LOWER_SIDE;
}

"minimize net"|"minimize"|"minimise" {
    return MINIMIZE;
}

"minus net"|"minus"|"subtract" {
    return MINUS;
}

"name net"|"name" {
    return NAME;
}

"negate net"|"negate" {
    return NEGATE;
}

"one-plus net"|"one-plus" {
    return ONE_PLUS;
}

"pop"|"pop stack" {
    return POP;
}

"print aliases"|"aliases" {
    return PRINT_ALIASES;
}
"print arc-tally"|"arc-tally" {
    return PRINT_ARCCOUNT;
}
"print defined"|"pdefined" {
    return PRINT_DEFINED;
}
"write definition"|"wdef" {
    return SAVE_DEFINITION;
}

"write definitions"|"wdefs" {
    return SAVE_DEFINITIONS;
}

"print directory"|"directory" {
    return PRINT_DIR;
}

"write dot"|"wdot"|"dot" {
    return SAVE_DOT;
}

"print file-info"|"file-info" {
    return PRINT_FILE_INFO;
}
"print flags"|"flags" {
    return PRINT_FLAGS;
}
"print labels"|"labels"$ {
    return PRINT_LABELS;
}
"print label-maps"|"label-maps" {
    return PRINT_LABELMAPS;
}
"print label-tally"|"label-tally" {
    return PRINT_LABEL_COUNT;
}
"print list" {
    return PRINT_LIST;
}
"print lists" {
    return PRINT_LISTS;
}
"print longest-string"|"longest-string"|"pls" {
    return PRINT_LONGEST_STRING;
}
"print longest-string-size"|"longest-string-size"|"plz" {
    return PRINT_LONGEST_STRING_SIZE;
}
"print lower-words"|"lower-words" {
    return PRINT_LOWER_WORDS;
}
"print name"|"pname" {
    return PRINT_NAME;
}
"print net" {
    return PRINT;
}
("print"|"write")" properties"|"props" {
    return PRINT_PROPS;
}
"print random-lower"|"random-lower" {
    return PRINT_RANDOM_LOWER;
}
"print random-upper"|"random-upper" {
    return PRINT_RANDOM_UPPER;
}
"print random-words"|"random-words" {
    return PRINT_RANDOM_WORDS;
}
"print shortest-string-"("size"|"length")|"shortest-string-size"|"psz" {
    return PRINT_SHORTEST_STRING_SIZE;
}
"print shortest-string"|"shortest-string"|"pss" {
    return PRINT_SHORTEST_STRING;
}
"print sigma"|"sigma" {
    return PRINT_SIGMA;
}
"print sigma-tally"|"sigma-tally"|"sitally" {
    return PRINT_SIGMA_COUNT;
}
"print sigma-word-tally" {
    return PRINT_SIGMA_WORD_COUNT;
}
"print size"|"size" {
    return PRINT_SIZE;
}
"print stack"|"stack" {
    return PRINT_STACK;
}
"print upper-words"|"upper-words" {
    return PRINT_UPPER_WORDS;
}

"print words"|"words" {
    return PRINT_WORDS;
}
"prune net"|"prune" {
    return PRUNE;
}


"push"|"push defined" {
    return PUSH_DEFINED;
}

"quit"|"exit"|"bye"|"stop"|"hyvästi"|"au revoir"|"näkemiin"|"viszlát"|"auf wiedersehen"|"hasta la vista"|"arrivederci"|"dodongo" {
    hxfstlval.name = strdup(yytext);
    return QUIT;
}

"lexc"|"read lexc" {
    return READ_LEXC;
}

"read properties"|"rprops" {
    return READ_PROPS;
}
"read prolog"|"rpl" {
    return READ_PROLOG;
}
"regex"|"read regex" {
    BEGIN(REGEX_STATE);
    return READ_REGEX;
}
"rs"|"read spaced-text" {
    return READ_SPACED;
}
"rt"|"read text" {
    return READ_TEXT;
}

"reverse net"|"reverse" {
    return REVERSE;
}

"rotate"|"rotate stack" {
    return ROTATE;
}

"save defined"|"saved" {
    return SAVE_DEFINITIONS;
}

"save stack"|"save"|"ss" {
    return SAVE_STACK;
}

"set" {
    return SET;
}

"show variables" {
    return SHOW_ALL;
}

"show variable"|"show" {
    return SHOW;
}

"shuffle net"|"shuffle" {
    return SHUFFLE;
}

"sigma net" {
    return SIGMA;
}

"sort net"|"sort" {
    return SORT;
}

"source" {
    return SOURCE;
}

"substitute defined" {
    return SUBSTITUTE_NAMED;
}

"for" {
    return FOR;
}

"substitute label" {
    return SUBSTITUTE_LABEL;
}

"substitute symbol" {
    return SUBSTITUTE_SYMBOL;
}

"substring net"|"substring" {
    return SUBSTRING;
}

"system"{WSP}+.*$ {
    hxfstlval.text = strdup(yytext+7);
    return SYSTEM;
}

"test equivalent"|"equivalent"|"te" {
    return TEST_EQ;
}
"test functional"|"functional"|"tf" {
    return TEST_FUNCT;
}
"test identity"|"identity"|"ti" {
    return TEST_ID;
}
"test lower-bounded"|"lower-bounded"|"tlb" {
    return TEST_LOWER_BOUNDED;
}
"test lower-universal"|"lower-universal"|"tlu" {
    return TEST_LOWER_UNI;
}
"test non-null"|"tnn" {
    return TEST_NONNULL;
}
"test null"|"tnu" {
    return TEST_NULL;
}
"test overlap"|"overlap"|"to" {
    return TEST_OVERLAP;
}
"test sublanguage"|"sublanguage"|"ts" {
    return TEST_SUBLANGUAGE;
}
"test upper-bounded"|"upper-bounded"|"tub" {
    return TEST_UPPER_BOUNDED;
}
"test upper-universal"|"upper-universal"|"tuu" {
    return TEST_UPPER_UNI;
}
"test unambiguous" {
    return TEST_UNAMBIGUOUS;
}

"turn"|"turn stack" {
    return TURN;
}

"twosided flag-diacritics"|"tfd" {
    return TWOSIDED_FLAGS;
}

"undefine" {
    return UNDEFINE;
}

"unlist" {
    return UNLIST;
}

"union net"|"union"|"disjunct" {
    return UNION;
}

"upper-side net"|"upper-side" {
    return UPPER_SIDE;
}

"view net" {
    return VIEW;
}

"wpl"|"write prolog" {
    return SAVE_PROLOG;
}
"wspaced-text"|"write spaced-text" {
    return SAVE_SPACED;
}
"wt"|"write text" {
    return SAVE_TEXT;
}

"zero-plus net"|"zero-plus" {
    return ZERO_PLUS;
}

<REGEX_STATE>[^;]*";" {
    BEGIN(0);
    hxfstlval.name = hfst::xfst::strstrip(hxfsttext);
    return REGEX;
}

<REGEX_STATE>. {
    hxfsterror("Unexpected character when parsing REGEX\n");
    fprintf(stderr, "%s", hxfsttext);
    return ERROR;
}

">"{WSP}*{NAMETOKEN} {
    hxfstlval.file = hfst::xfst::strstrip(hxfsttext+1);
    return REDIRECT_OUT;
}
"<"{WSP}*{NAMETOKEN} {
    hxfstlval.file = hfst::xfst::strstrip(hxfsttext+1);
    return REDIRECT_IN;
}

{RANGE} {
    char* range = hfst::xfst::strstrip(hxfsttext);
    char* s = range;
    hxfstlval.list = static_cast<char**>(malloc(sizeof(char*)*2));
    char* p = hxfstlval.list[0];
    while (*s != '-')
    {
        *p = *s;
        p++;
        s++;
    }
    *p = '\0';
    p = hxfstlval.list[1];
    s++;
    while (*s != '\0')
    {
        *p = *s;
        p++;
        s++;
    }
    *p = '\0';
    free(range);
    return RANGE;
}

"("[a-zA-Z_ ,]*")" {
    hxfstlval.name = strdup(hxfsttext);
    return PROTOTYPE;
}

"(" {
    return LEFT_PAREN;
}
";" {
    return SEMICOLON;
}
":" {
    return COLON;
}
"END;"|"END" {
    return END_SUB;
}
{NAMETOKEN} {
    hxfstlval.name = strdup(hxfsttext);
    return NAMETOKEN;
}
{NUMBER} {
    hxfstlval.number = strtoul(hxfsttext, 0, 10);
    return NUMBER;
}

[\x04] {
    return CTRLD;
}

[\n\r] { return NEWLINE; }

[\t ]* { /* skip whitespace */ }

"!".*$ { /* skip comments */ }

[\x80-\xff] {
    hxfsterror("Illegal 8-bit sequence (cannot form valid UTF-8)");
    return ERROR;
}

. {
    hxfsterror("Syntax error in lexer (no valid token found at the point)");
    return ERROR;
}

