%option 8Bit batch yylineno noyywrap

/* the "incl" state is used to pick up the name of an include file */
%x incl

%{
/*******************************************************************/
/*                                                                 */
/*  FILE     hfst-scanner.ll                                      */
/*  MODULE   h(w)fst-calculate                                     */
/*  PROGRAM  HFST version 2.0                                      */
/*  AUTHOR   Erik Axelson, University of Helsinki                  */
/*  (based on Helmut Schmid's code for SFST)                       */
/*                                                                 */
/*******************************************************************/

#include <string.h>

#include <hfst2/hfst.h>

namespace HFST { void error2(char *message, char *input); }
namespace HWFST { void error2(const char *message, const char *input); }


#ifdef WEIGHTED

//using HWFST::TheAlphabet;
//using HWFST::Character;
using HWFST::ContextsHandle;
// using HFST::MakeCompactTransducer;
using HWFST::Range;
using HWFST::Ranges;
using HWFST::Repl_Type;
using HWFST::TransducerHandle;
using HWFST::Twol_Type;
//using HWFST::Verbose;

using HWFST::error2;
using HWFST::repl_up;
using HWFST::repl_down;
using HWFST::repl_left;
using HWFST::repl_right;
using HWFST::twol_both;
using HWFST::twol_left;
using HWFST::twol_right;

#else

//using HFST::TheAlphabet;
//using HFST::Character;
using HFST::ContextsHandle;
// using HFST::MakeCompactTransducer;
using HFST::Range;
using HFST::Ranges;
using HFST::Repl_Type;
using HFST::TransducerHandle;
using HFST::Twol_Type;
//using HFST::Verbose;

using HFST::error2;
using HFST::repl_up;
using HFST::repl_down;
using HFST::repl_left;
using HFST::repl_right;
using HFST::twol_both;
using HFST::twol_left;
using HFST::twol_right;

#endif

#include "hfst-compiler.h"

#define MAX_INCLUDE_DEPTH 10
  
int Include_Stack_Ptr = 0;
YY_BUFFER_STATE Include_Stack[MAX_INCLUDE_DEPTH];
char *Name_Stack[MAX_INCLUDE_DEPTH];
int  Lineno_Stack[MAX_INCLUDE_DEPTH];

extern char *FileName;

bool UTF8=true;
bool Verbose=true;

extern bool weighted;

static char *unquote(char *string) {
  char *s, *result=string;

  for( s=string++; *string; s++, string++) {
    if (*string == '\\')
      string++;
    *s = *string;
  }
  *(s-1) = '\0';

  return strdup(result);
}

static void print_lineno() {
  if (!Verbose)
    return;
  fputc('\r',stderr);
  for( int i=0; i<Include_Stack_Ptr; i++ )
    fputs("  ", stderr);
  fprintf(stderr,"%s: %d", FileName, yylineno);
}

extern void yyerror(char *text);

%}

CC	[\x80-\xbf]
C1	[A-Za-z0-9._/\-]
C2	[A-Za-z0-9._/\-&()+,=?\^|~]
C3	[A-Za-z0-9._/\-&()+,=?\^|~#<>]
C4	[A-Za-z0-9._/\-&()+,=?\^|~$<>]
C5	[A-Za-z0-9._/\-&()+,=?\^|~$#]
FN	[A-Za-z0-9._/\-*+]

%%

#include           BEGIN(incl);
<incl>[ \t]*       /* eat the whitespace */
<incl>{FN}+        { error2((char*)"Missing quotes",yytext); }
<incl>\"{FN}+\"  { /* got the include file name */
                     FILE *file;
                     char *name=unquote(yytext);
                     if ( Include_Stack_Ptr >= MAX_INCLUDE_DEPTH )
		       {
			 fprintf( stderr, "Includes nested too deeply" );
			 exit( 1 );
		       }
		     if (Verbose) fputc('\n', stderr);
		     file = fopen( name, "rt" );
		     if (!file)
                       error2((char*)"Can't open include file",name);
                     else
                       {
                         Name_Stack[Include_Stack_Ptr] = FileName;
                         FileName = name;
                         Lineno_Stack[Include_Stack_Ptr] = yylineno;
			 yylineno = 1;
		         Include_Stack[Include_Stack_Ptr++]=YY_CURRENT_BUFFER;
		         yy_switch_to_buffer(
                              yy_create_buffer(yyin, YY_BUF_SIZE));
                         yyin = file;
			 print_lineno();
		         BEGIN(INITIAL);
                       }
                  }
<<EOF>>           {
                     if (Verbose)
		       fputc('\n', stderr);
                     if ( --Include_Stack_Ptr < 0 )
		       yyterminate();
		     else
		       {
                         free(FileName);
                         FileName = Name_Stack[Include_Stack_Ptr];
                         yylineno = Lineno_Stack[Include_Stack_Ptr];
			 yy_delete_buffer( YY_CURRENT_BUFFER );
			 yy_switch_to_buffer(Include_Stack[Include_Stack_Ptr]);
                       }
                  }


^[ \t]*\%.*\r?\n  { print_lineno();  /* ignore comments */ }

\%.*\\[ \t]*\r?\n { print_lineno();  /* ignore comments */ }

\%.*              { /* ignore comments */ }


^[ \t]*ALPHABET[ \t]*= { return ALPHA; }
^[ \t]*EXIT[ \t]*= { return EXIT; }

\|\|              { return COMPOSE; }
\|\|_             { return L_COMPOSE; }
"<=>"             { yylval.type = twol_both; return ARROW; }
"=>"              { yylval.type = twol_right; return ARROW; }
"<="              { yylval.type = twol_left; return ARROW; }
"^->"             { yylval.rtype = repl_up;   return REPLACE; }
"_->"             { yylval.rtype = repl_down; return REPLACE; }
"/->"             { yylval.rtype = repl_right;return REPLACE; }
"\\->"            { yylval.rtype = repl_left; return REPLACE; }
">>"              { return PRINT_BIN; }
">bin>"           { return PRINT_BIN; }
">text>"          { return PRINT_TEXT; }
">num>"           { return PRINT_NUM; }
">bin>"[ \t]*cout  { return PRINT_BIN_COUT; }
">text>"[ \t]*cout { return PRINT_TEXT_COUT; }
">bin>"[ \t]*cerr  { return PRINT_BIN_CERR; }
">text>"[ \t]*cerr { return PRINT_TEXT_CERR; }
">num>"[ \t]*cerr { return PRINT_NUM_CERR; }
">num>"[ \t]*cout { return PRINT_NUM_COUT; }
"<<"              { return INSERT; }
"__"              { return POS; }
"=="              { return EQUAL; }
"!="              { return NOT_EQUAL; }


[.,{}\[\]()&!?|*+:=_\^\-] { return yytext[0]; }

\$=({C3}|(\\.))+\$ { yylval.name = strdup(yytext); return RVAR; }

\$({C3}|(\\.))+\$ { yylval.name = strdup(yytext); return VAR; }

#=({C4}|(\\.))+# { yylval.name = strdup(yytext); return RSVAR; }

#({C4}|(\\.))+# { yylval.name = strdup(yytext); return SVAR; }

\<{C5}*\>        { yylval.name = strdup(yytext); return SYMBOL; }

\">({C5}|(\\.))*<\" { 
                    yylval.value = unquote(yytext)+1;
		    yylval.value[strlen(yylval.value)-1] = 0;
                    return STRING3;
                  }

\"<({C5}|(\\.))*>\" { 
                    yylval.value = unquote(yytext)+1;
		    yylval.value[strlen(yylval.value)-1] = 0;
                    return STRING2;
                  }

\"({FN}|(\\.))+\" { 
                    yylval.value = unquote(yytext);
                    return STRING;
                  }

[ \t]             { /* ignored */ }
\\[ \t]*([ \t]\%.*)?\r?\n { print_lineno(); /* ignored */ }
\r?\n             { print_lineno(); return NEWLINE; }

\\[0-9]+          { long l=atol(yytext+1); 
		    if (l <= 1114112) { yylval.uchar=l; return CHARACTER; }
		    yyerror((char*)"invalid expression");
                  }


\\.                { yylval.value=strdup(yytext+1); return UTF8CHAR; }
[\x00-\x7f]        { yylval.value=strdup(yytext); return UTF8CHAR; }
[\xc0-\xdf]{CC}    { yylval.value=strdup(yytext); return UTF8CHAR; }
[\xe0-\xef]{CC}{2} { yylval.value=strdup(yytext); return UTF8CHAR; }
[\xf0-\xff]{CC}{3} { yylval.value=strdup(yytext); return UTF8CHAR; }

%%
