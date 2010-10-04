%{
/*******************************************************************/
/*                                                                 */
/*  FILE     hfst-compiler.yy                                       */
/*  MODULE   hfst-compiler                                          */
/*  PROGRAM  HFST                                                  */     
/*                                                                 */
/*******************************************************************/

#include <stdio.h>

#include "HfstCompiler.h"

extern char* FileName;
extern bool Verbose;

using std::cerr;
using namespace hfst;

extern int  yylineno;
extern char *yytext;

void yyerror(char *text);
void warn(char *text);
void warn2(const char *text, char *text2);
int yylex( void );
int yyparse( void );

static int Switch=0;
HfstCompiler * compiler;
HfstTransducer * Result;

ImplementationType type;
bool DEBUG=false;


%}

/* Slight Hfst addition SFST::... */
%union {
  int        number;
  hfst::Twol_Type  type;
  hfst::Repl_Type  rtype;
  char       *name;
  char       *value;
  unsigned char uchar;
  unsigned int  longchar;
  hfst::Character  character;
  hfst::HfstTransducer   *expression;
  hfst::Range      *range;
  hfst::Ranges     *ranges;
  hfst::Contexts   *contexts;
}

%token <number> NEWLINE ALPHA COMPOSE PRINT POS INSERT SUBSTITUTE SWITCH
%token <type>   ARROW
%token <rtype>  REPLACE
%token <name>   SYMBOL VAR SVAR RVAR RSVAR
%token <value>  STRING STRING2 UTF8CHAR
%token <uchar>  CHARACTER

%type  <uchar>      SCHAR
%type  <longchar>   LCHAR
%type  <character>  CODE
%type  <expression> RE
%type  <range>      RANGE VALUE VALUES
%type  <ranges>     RANGES
%type  <contexts>   CONTEXT CONTEXT2 CONTEXTS CONTEXTS2

%left PRINT INSERT SUBSTITUTE
%left ARROW REPLACE
%left COMPOSE
%left '|'
%left '-'
%left '&'
%left SEQ
%left '!' '^' '_'
%left '*' '+'
%%

ALL:        ASSIGNMENTS RE NEWLINES { Result=compiler->result($2, Switch); }
          ;

ASSIGNMENTS: ASSIGNMENTS ASSIGNMENT {}
          | ASSIGNMENTS NEWLINE     {}
          | /* nothing */           {}
          ;

ASSIGNMENT: VAR '=' RE              { if (DEBUG) { printf("defining transducer variable \"%s\"..\n", $1); }; if (compiler->def_var($1,$3)) warn2("assignment of empty transducer to",$1); if(DEBUG) printf("done\n"); }
          | RVAR '=' RE             { if (DEBUG) { printf("defining agreement transducer variable \"%s\"..\n", $1); }; if (compiler->def_rvar($1,$3)) warn2("assignment of empty transducer to",$1); }
          | SVAR '=' VALUES         { if (DEBUG) { printf("defining range variable \"%s\"..\n", $1); }; if (compiler->def_svar($1,$3)) warn2("assignment of empty symbol range to",$1); }
          | RSVAR '=' VALUES        { if (DEBUG) { printf("defining agreement range variable \"%s\"..\n", $1); }; if (compiler->def_svar($1,$3)) warn2("assignment of empty symbol range to",$1); }
          | RE PRINT STRING         { compiler->write_to_file($1, $3); }
          | ALPHA RE                { if (DEBUG) { printf("defining alphabet..\n"); }; compiler->def_alphabet($2); delete $2; }
          ;

RE:         RE ARROW CONTEXTS2      { $$ = compiler->restriction($1,$2,$3,0); }
	  | RE '^' ARROW CONTEXTS2  { $$ = compiler->restriction($1,$3,$4,1); }
	  | RE '_' ARROW CONTEXTS2  { $$ = compiler->restriction($1,$3,$4,-1); }
          | RE REPLACE CONTEXT2     { $1 = compiler->explode($1); $1->minimize(); $$ = compiler->replace_in_context($1, $2, $3, false); }
          | RE REPLACE '?' CONTEXT2 { $1 = compiler->explode($1); $1->minimize(); $$ = compiler->replace_in_context($1, $2, $4, true); }
          | RE REPLACE '(' ')'      { $1 = compiler->explode($1); $1->minimize(); $$ = compiler->replace($1, $2, false); }
          | RE REPLACE '?' '(' ')'  { $1 = compiler->explode($1); $1->minimize(); $$ = compiler->replace($1, $2, true); }
          | RE RANGE ARROW RANGE RE { $$ = compiler->make_rule($1,$2,$3,$4,$5, type); }
          | RE RANGE ARROW RANGE    { $$ = compiler->make_rule($1,$2,$3,$4,NULL, type); }
          | RANGE ARROW RANGE RE    { $$ = compiler->make_rule(NULL,$1,$2,$3,$4, type); }
          | RANGE ARROW RANGE       { $$ = compiler->make_rule(NULL,$1,$2,$3,NULL, type); }
          | RE COMPOSE RE    { $1->compose(*$3); delete $3; $$ = $1; }
          | '{' RANGES '}' ':' '{' RANGES '}' { $$ = compiler->make_mapping($2,$6,type); }
          | RANGE ':' '{' RANGES '}' { $$ = compiler->make_mapping(compiler->add_range($1,NULL),$4,type); }
          | '{' RANGES '}' ':' RANGE { $$ = compiler->make_mapping($2,compiler->add_range($5,NULL),type); }
          | RE INSERT CODE ':' CODE  { $$ = compiler->insert_freely($1,$3,$5); }
          | RE INSERT CODE           { $$ = compiler->insert_freely($1,$3,$3); }
	  | RE SUBSTITUTE CODE ':' CODE  { $$ = compiler->substitute($1,$3,$5); }
	  | RE SUBSTITUTE CODE ':' CODE ':' CODE ':' CODE { $$ = compiler->substitute($1,$3,$5,$7,$9); }
	  | RE SUBSTITUTE CODE ':' CODE '(' RE ')' { $$ = compiler->substitute($1,$3,$5,$7); }
          | RANGE ':' RANGE  { $$ = compiler->new_transducer($1,$3,type); } 
          | RANGE            { $$ = compiler->new_transducer($1,$1,type); }
          | VAR              { if (DEBUG) { printf("calling transducer variable \"%s\"\n", $1); }; $$ = compiler->var_value($1); }
          | RVAR             { if (DEBUG) { printf("calling agreement transducer variable \"%s\"\n", $1); }; $$ = compiler->rvar_value($1,type); }
          | RE '*'           { $1->repeat_star(); $$ = $1; }
          | RE '+'           { $1->repeat_plus(); $$ = $1; }
          | RE '?'           { $1->optionalize(); $$ = $1; }
          | RE RE %prec SEQ  { $1->concatenate(*$2); delete $2; $$ = $1; }
          | '!' RE           { $$ = compiler->negation($2); }
          | SWITCH RE        { $2->invert(); $$ = $2; }
          | '^' RE           { $2->output_project(); $$ = $2; }
          | '_' RE           { $2->input_project(); $$ = $2; }
          | RE '&' RE        { $1->intersect(*$3); delete $3; $$ = $1; }
          | RE '-' RE        { $1->subtract(*$3); delete $3; $$ = $1; }
          | RE '|' RE        { $1->disjunct(*$3); delete $3; $$ = $1; }
          | '(' RE ')'       { $$ = $2; }
          | STRING           { $$ = compiler->read_words($1, type); }
          | STRING2          { try { $$ = compiler->read_transducer($1, type); } catch (hfst::exceptions::HfstInterfaceException e) { printf("\nAn error happened when reading file \"%s\"\n", $1); exit(1); } }
          ;

RANGES:     RANGE RANGES     { $$ = compiler->add_range($1,$2); }
          |                  { $$ = NULL; }
          ;

RANGE:      '[' VALUES ']'   { $$=$2; }
          | '[' '^' VALUES ']' { $$=compiler->complement_range($3); }
          | '[' RSVAR ']'    { if (DEBUG) { printf("calling agreement range variable \"%s\"\n", $2); }; $$=compiler->rsvar_value($2); }
          | '.'              { $$=NULL; }
          | CODE             { $$=compiler->add_value($1,NULL); }
          ;

CONTEXTS2:  CONTEXTS               { $$ = $1; }
          | '(' CONTEXTS ')'       { $$ = $2; }
          ;

CONTEXTS:   CONTEXT ',' CONTEXTS   { $$ = compiler->add_context($1,$3); }
          | CONTEXT                { $$ = $1; }
          ;

CONTEXT2:   CONTEXT                { $$ = $1; }
          | '(' CONTEXT ')'        { $$ = $2; }
          ;

CONTEXT :   RE POS RE              { $$ = compiler->make_context($1, $3); }
          |    POS RE              { $$ = compiler->make_context(NULL, $2); }
          | RE POS                 { $$ = compiler->make_context($1, NULL); }
          ;

VALUES:     VALUE VALUES           { $$=compiler->append_values($1,$2); }
          | VALUE                  { $$ = $1; }
          ;

VALUE:      LCHAR '-' LCHAR	   { $$=compiler->add_values($1,$3,NULL); }
          | SVAR                   { if (DEBUG) { printf("calling range variable \"%s\"", $1); }; $$=compiler->svar_value($1); }
          | LCHAR  	           { $$=compiler->add_value(compiler->character_code($1),NULL); }
          | CODE		   { $$=compiler->add_value($1,NULL); }
	  | SCHAR		   { $$=compiler->add_value($1,NULL); }
          ;

LCHAR:      CHARACTER	{ $$=$1; }
          | UTF8CHAR	{ $$=compiler->utf8toint($1); free($1); }
	  | SCHAR       { $$=$1; }
          ;

CODE:       CHARACTER	{ $$=compiler->character_code($1); }
          | UTF8CHAR	{ $$=compiler->symbol_code($1); }
          | SYMBOL	{ $$=compiler->symbol_code($1); }
          ;

SCHAR:      '.'		{ $$=(unsigned char)compiler->character_code('.'); }
          | '!'		{ $$=(unsigned char)compiler->character_code('!'); }
          | '?'		{ $$=(unsigned char)compiler->character_code('?'); }
          | '{'		{ $$=(unsigned char)compiler->character_code('{'); }
          | '}'		{ $$=(unsigned char)compiler->character_code('}'); }
          | ')'		{ $$=(unsigned char)compiler->character_code(')'); }
          | '('		{ $$=(unsigned char)compiler->character_code('('); }
          | '&'		{ $$=(unsigned char)compiler->character_code('&'); }
          | '|'		{ $$=(unsigned char)compiler->character_code('|'); }
          | '*'		{ $$=(unsigned char)compiler->character_code('*'); }
          | '+'		{ $$=(unsigned char)compiler->character_code('+'); }
          | ':'		{ $$=(unsigned char)compiler->character_code(':'); }
          | ','		{ $$=(unsigned char)compiler->character_code(','); }
          | '='		{ $$=(unsigned char)compiler->character_code('='); }
          | '_'		{ $$=(unsigned char)compiler->character_code('_'); }
          | '^'		{ $$=(unsigned char)compiler->character_code('^'); }
          | '-'		{ $$=(unsigned char)compiler->character_code('-'); }
          ;

NEWLINES:   NEWLINE NEWLINES     {}
          | /* nothing */        {}
          ;

%%

extern FILE  *yyin;
static int Compact=0;
static int LowMem=0;

/*******************************************************************/
/*                                                                 */
/*  yyerror                                                        */
/*                                                                 */
/*******************************************************************/

void yyerror(char *text)

{
  cerr << "\n" << FileName << ":" << yylineno << ": " << text << " at: ";
  cerr << yytext << "\naborted.\n";
  exit(1);
}


/*******************************************************************/
/*                                                                 */
/*  warn                                                           */
/*                                                                 */
/*******************************************************************/

void warn(char *text)

{
  cerr << "\n" << FileName << ":" << yylineno << ": warning: " << text << "!\n";
}


/*******************************************************************/
/*                                                                 */
/*  warn2                                                          */
/*                                                                 */
/*******************************************************************/

void warn2(const char *text, char *text2)  // HFST: added const

{
  cerr << "\n" << FileName << ":" << yylineno << ": warning: " << text << ": ";
  cerr << text2 << "\n";
}


/* print_usage */

void print_usage(FILE *file) {
    fprintf(file,"\nUsage: %s [options] infile outfile\n", "hfst-calculate");
    fprintf(file,"\nOPTIONS:\n");
    fprintf(file,"-c\tStore the transducer in fst-infl2 format.\n");
    fprintf(file,"-l\tStore the transducer in fst-infl3 format.\n");
    fprintf(file,"-s\tSwitch the upper and lower levels producing a transducer for generation rather than recognition.\n");
    fprintf(file,"-q\tquiet mode\n\n");  
}

/*******************************************************************/
/*                                                                 */
/*  get_flags                                                      */
/*                                                                 */
/*******************************************************************/

void get_flags( int *argc, char **argv )

{
  for( int i=1; i<*argc; i++ ) {
    if (strcmp(argv[i],"-c") == 0) {
      Compact = 1;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-l") == 0) {
      LowMem = 1;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-q") == 0) {
      Verbose = 0;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0) {
      print_usage(stdout);
      exit(0);
    }
    else if (strcmp(argv[i],"-v") == 0 || strcmp(argv[i],"--version") == 0) {
      fprintf(stdout, "hfst-calculate 0.1 (hfst 3.0)\n");
      exit(0);
    }
    else if (strcmp(argv[i],"-s") == 0) {
      Switch = 1;
      argv[i] = NULL;
    }
    // hfst addition
    else if (strcmp(argv[i],"-sfst") == 0) {
      type = SFST_TYPE;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-tropical") == 0) {
      type = TROPICAL_OFST_TYPE;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-log") == 0) {
      type = LOG_OFST_TYPE;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-foma") == 0) {
      type = FOMA_TYPE;
      argv[i] = NULL;
    }
  }
  // remove flags from the argument list
  int k;
  for( int i=k=1; i<*argc; i++)
    if (argv[i] != NULL)
      argv[k++] = argv[i];
  *argc = k;
}


/*******************************************************************/
/*                                                                 */
/*  main                                                           */
/*                                                                 */
/*******************************************************************/

int main( int argc, char *argv[] )

{
  FILE *file;

  get_flags(&argc, argv);
  if (argc < 3) {
    print_usage(stderr);
    exit(1);
  }
  if ((file = fopen(argv[1],"rt")) == NULL) {
    fprintf(stderr,"\nError: Cannot open grammar file \"%s\"\n\n", argv[1]);
    exit(1);
  }
  FileName = argv[1];
  //Result = NULL;
  yyin = file;  

  hfst::set_unknown_symbols_in_use(false);

  compiler = new HfstCompiler(type, Verbose);
  try {
    yyparse();
    //Result->alphabet.utf8 = UTF8;
    //if (Verbose)
    //  cerr << "\n";
    //if (Result->is_empty()) 
    //  warn("resulting transducer is empty"); 
    if ((file = fopen(argv[2],"wb")) == NULL) {
	fprintf(stderr,"\nError: Cannot open output file %s\n\n", argv[2]);
	exit(1);
    }
    //if (Compact) {
    //  MakeCompactTransducer ca(*Result);
    //  delete Result;
    //  ca.store(file);
    //}
    //else if (LowMem)
    //  Result->store_lowmem(file);
    else {
      try {
      bool DEBUG=false;
      if (DEBUG) { printf("writing to file..\n");
         	   //std::cerr << *Result;
		   Result->print_alphabet();
      }
        compiler->write_to_file(Result,argv[2]);
      } catch (hfst::exceptions::HfstInterfaceException e) {
          printf("\nAn error happened when writing to file \"%s\"\n", argv[2]);
      }
	if (DEBUG) printf("..done\n");
    }
    fclose(file);
    //printf("type is: %i\n", Result->get_type());
    delete Result;
    // delete compiler;
  }
  catch(const char* p) {
      cerr << "\n" << p << "\n\n";
      exit(1);
  }
}
