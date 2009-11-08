%{
/*******************************************************************/
/*                                                                 */
/*  FILE     hfst-compiler.yy                                      */
/*  MODULE   h(w)fst-calculate                                     */
/*  PROGRAM  HFST version 2.0                                      */
/*  AUTHOR   Erik Axelson, University of Helsinki                  */
/*  (based on Helmut Schmid's code for SFST)                       */
/*                                                                 */
/*******************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <hfst2/hfst.h>

#include "hfst-program-options.h"


#ifdef WEIGHTED

namespace HWFST {
#include "calculate_functions.h"
}

using HWFST::Key;
using HWFST::KeyPairSet;
using HWFST::ContextsHandle;
using HWFST::Range;
using HWFST::Ranges;
using HWFST::Repl_Type;
using HWFST::TransducerHandle;
using HWFST::Twol_Type;

using HWFST::append_context;
using HWFST::add_range;
using HWFST::add_value;
using HWFST::add_values;
using HWFST::add_var_values;
using HWFST::concatenate;
using HWFST::character_code;
using HWFST::complement_range;
using HWFST::compose;
using HWFST::intersect;
using HWFST::define_keypair_set;
using HWFST::define_transducer_agreement_variable;
using HWFST::define_range_variable;
using HWFST::define_transducer_variable;
using HWFST::disjunct;
using HWFST::explode;
using HWFST::insert_freely;
using HWFST::extract_input_language;
using HWFST::make_context;
using HWFST::make_mapping;
using HWFST::make_rule;
using HWFST::minimize;
using HWFST::negate;
using HWFST::new_transducer;
using HWFST::optionalize;
using HWFST::read_transducer_and_harmonize;
using HWFST::read_words;
using HWFST::repeat_star;
using HWFST::repeat_plus;
using HWFST::make_replace;
using HWFST::make_replace_in_context;
using HWFST::make_restriction;
using HWFST::result;
using HWFST::copy_range_agreement_variable_value;
using HWFST::copy_transducer_agreement_variable_value;
using HWFST::subtract;
using HWFST::copy_range_variable_value;
using HWFST::invert;
using HWFST::symbol_code;
using HWFST::extract_output_language;
using HWFST::utf8_to_int;
using HWFST::copy_transducer_variable_value;
using HWFST::write_transducer;
using HWFST::is_empty;

using HWFST::are_equivalent;
using HWFST::add_epsilon_to_alphabet;
using HWFST::read_symbol_table_text;
using HWFST::set_alphabet_defined;
using HWFST::set_ofst_symbol_table;

using HWFST::print_transducer;
using HWFST::print_transducer_number;
using HWFST::read_transducer_text;
using HWFST::write_symbol_table;
using HWFST::delete_transducer;

#else


namespace HFST {
#include "calculate_functions.h"
void store_as_compact(TransducerHandle t, FILE *file);
}

//using HFST::TheAlphabet;
using HFST::Key;
using HFST::KeyPairSet;
using HFST::ContextsHandle;
using HFST::store_as_compact;
using HFST::Range;
using HFST::Ranges;
using HFST::Repl_Type;
using HFST::TransducerHandle;
using HFST::Twol_Type;

using HFST::append_context;
using HFST::add_range;
using HFST::add_value;
using HFST::add_values;
using HFST::add_var_values;
using HFST::concatenate;
using HFST::character_code;
using HFST::complement_range;
using HFST::compose;
using HFST::intersect;
using HFST::define_keypair_set;
using HFST::define_transducer_agreement_variable;
using HFST::define_range_variable;
using HFST::define_transducer_variable;
using HFST::disjunct;
using HFST::explode;
using HFST::insert_freely;
using HFST::extract_input_language;
using HFST::make_context;
using HFST::make_mapping;
using HFST::make_rule;
using HFST::minimize;
using HFST::negate;
using HFST::new_transducer;
using HFST::optionalize;
using HFST::read_transducer_and_harmonize;
using HFST::read_words;
using HFST::repeat_star;
using HFST::repeat_plus;
using HFST::make_replace;
using HFST::make_replace_in_context;
using HFST::make_restriction;
using HFST::result;
using HFST::subtract;
using HFST::invert;
using HFST::symbol_code;
using HFST::extract_output_language;
using HFST::utf8_to_int;
using HFST::copy_transducer_variable_value;
using HFST::copy_range_variable_value;
using HFST::copy_range_agreement_variable_value;
using HFST::copy_transducer_agreement_variable_value;
using HFST::write_transducer;
using HFST::is_empty;

using HFST::are_equivalent;
using HFST::add_epsilon_to_alphabet;
using HFST::read_symbol_table_text;
using HFST::set_alphabet_defined;
using HFST::explode_and_minimise;

using HFST::print_transducer;
using HFST::print_transducer_number;
using HFST::read_transducer_text;
using HFST::write_symbol_table;
using HFST::delete_transducer;

#endif



extern bool UTF8;
extern int  yylineno;
extern char *yytext;

char *FileName;

void yyerror(char *text);
void warn(char *text, char *FileName);
void warn2(char *text, char *text2, char *FileName);
void delete_keypair_set(KeyPairSet *kps);
int yylex( void );
int yyparse( void );

static int Switch=0;
TransducerHandle Result;
KeyPairSet *Pi;


%}

%union {
  int        number;
  Twol_Type  type;
  Repl_Type  rtype;
  char       *name;
  char       *value;
  unsigned char uchar;
  unsigned int  longchar;
  Key  character;
  TransducerHandle  expression;
  Range      *range;
  Ranges     *ranges;
  ContextsHandle   contexts;
}

%token <number> NEWLINE ALPHA EXIT COMPOSE L_COMPOSE PRINT_BIN POS INSERT EQUAL NOT_EQUAL PRINT_TEXT PRINT_NUM PRINT_BIN_COUT PRINT_BIN_CERR PRINT_TEXT_COUT PRINT_TEXT_CERR PRINT_NUM_CERR PRINT_NUM_COUT
%token <type>   ARROW
%token <rtype>  REPLACE
%token <name>   SYMBOL VAR SVAR RVAR RSVAR
%token <value>  STRING STRING2 STRING3 UTF8CHAR
%token <uchar>  CHARACTER

%type  <uchar>      SCHAR
%type  <longchar>   LCHAR
%type  <character>  CODE
%type  <expression> RE
%type  <range>      RANGE VALUES
%type  <ranges>     RANGES
%type  <contexts>   CONTEXT CONTEXT2 CONTEXTS CONTEXTS2

%left PRINT_BIN INSERT EQUAL NOT_EQUAL PRINT_TEXT PRINT_BIN_COUT PRINT_BIN_CERR PRINT_TEXT_COUT PRINT_TEXT_CERR PRINT_NUM PRINT_NUM_CERR PRINT_NUM_COUT
%left ARROW REPLACE
%left COMPOSE L_COMPOSE
%left '|'
%left '-'
%left '&'
%left SEQ
%left '!' '^' '_'
%left '*' '+'
%%

ALL:        ASSIGNMENTS RE NEWLINES { Result=result($2, Switch); }
          ;

ASSIGNMENTS: ASSIGNMENTS ASSIGNMENT {}
          | ASSIGNMENTS NEWLINE     {}
          | /* nothing */           {}
          ;

ASSIGNMENT: VAR '=' RE              { if (define_transducer_variable($1,$3)) warn2((char*)"assignment of empty transducer to",$1,FileName); }
          | RVAR '=' RE             { if (define_transducer_agreement_variable($1,$3)) warn2((char*)"assignment of empty transducer to",$1,FileName); }
          | SVAR '=' VALUES         { if (define_range_variable($1,$3)) warn2((char*)"assignment of empty symbol range to",$1,FileName); }
          | RSVAR '=' VALUES        { if (define_range_variable($1,$3)) warn2((char*)"assignment of empty symbol range to",$1,FileName); }
          | RE PRINT_BIN STRING     { write_transducer($1, $3, NULL, false); delete_transducer($1); }
          | ALPHA RE                { delete_keypair_set(Pi); Pi = define_keypair_set($2); delete_transducer($2); set_alphabet_defined(1);  }  // elements of Pi not deleted!
	  | ALPHA STRING2	    { delete_keypair_set(Pi); TransducerHandle tr = read_transducer_and_harmonize($2); Pi = define_keypair_set(tr); delete_transducer(tr); set_alphabet_defined(1); }
	  | RE EQUAL RE             { if (!are_equivalent($1,$3)) warn((char*)"non-equivalent expressions",FileName); delete_transducer($1); delete_transducer($3); }
	  | RE NOT_EQUAL RE         { if (are_equivalent($1,$3)) warn((char*)"equivalent expressions",FileName); delete_transducer($1); delete_transducer($3); }
	  | RE PRINT_BIN_COUT       { write_transducer($1,cout); }
	  | RE PRINT_BIN_CERR       { write_transducer($1,cerr); }
	  | RE PRINT_TEXT_COUT      { fprintf(stdout,"\n"); print_transducer($1,NULL,true,cout); }
	  | RE PRINT_TEXT_CERR      { fprintf(stderr,"\n"); print_transducer($1,NULL,true,cerr); }
	  | RE PRINT_NUM_CERR      { fprintf(stderr,"\n"); print_transducer_number($1,true,cerr); }
	  | RE PRINT_NUM_COUT      { fprintf(stderr,"\n"); print_transducer_number($1,true,cout); }
	  | EXIT RE                { Result=result($2, Switch); YYACCEPT; }
          ;

RE:         RE ARROW CONTEXTS2      { $$ = make_restriction($1,$2,$3,0,Pi); }
	  | RE '^' ARROW CONTEXTS2  { $$ = make_restriction($1,$3,$4,1,Pi); }
	  | RE '_' ARROW CONTEXTS2  { $$ = make_restriction($1,$3,$4,-1,Pi); }
          | RE REPLACE CONTEXT2     { $$ = make_replace_in_context(minimize(explode($1)),$2,$3,false,Pi); }
          | RE REPLACE '(' ')'      { $$ = make_replace($1, $2, false, Pi); }
          | RE REPLACE '?' CONTEXT2 { $$ = make_replace_in_context(minimize(explode($1)),$2,$4,true,Pi);}
          | RE REPLACE '?' '(' ')'  { $$ = make_replace($1, $2, true, Pi); }
          | RE RANGE ARROW RANGE RE { $$ = make_rule($1,$2,$3,$4,$5,Pi); }
          | RE RANGE ARROW RANGE    { $$ = make_rule($1,$2,$3,$4,NULL,Pi); }
          | RANGE ARROW RANGE RE    { $$ = make_rule(NULL,$1,$2,$3,$4,Pi); }
          | RANGE ARROW RANGE       { $$ = make_rule(NULL,$1,$2,$3,NULL,Pi); }
          | RE COMPOSE RE           { $$ = compose($1, $3); }
//          | RE L_COMPOSE RE       { $$ = light_composition($1, $3); }
          | '{' RANGES '}' ':' '{' RANGES '}' { $$ = make_mapping($2,$6); }
          | RANGE ':' '{' RANGES '}' { $$ = make_mapping(add_range($1,NULL),$4); }
          | '{' RANGES '}' ':' RANGE { $$ = make_mapping($2,add_range($5,NULL)); }
          | RE INSERT CODE ':' CODE  { $$ = insert_freely($1, $3, $5); }
          | RE INSERT CODE           { $$ = insert_freely($1, $3, $3); }
          | RANGE ':' RANGE  { $$ = new_transducer($1,$3,Pi); }
          | RANGE            { $$ = new_transducer($1,$1,Pi); }
          | VAR              { $$ = copy_transducer_variable_value($1); }
          | RVAR             { $$ = copy_transducer_agreement_variable_value($1); }
          | RE '*'           { $$ = repeat_star($1); }
          | RE '+'           { $$ = repeat_plus($1); }
          | RE '?'           { $$ = optionalize($1); }
          | RE RE %prec SEQ  { $$ = concatenate($1, $2); }
          | '!' RE           { $$ = ::negate($2,Pi); }
          | '^' '_' RE       { $$ = invert($3); }
          | '^' RE           { $$ = extract_output_language($2); }
          | '_' RE           { $$ = extract_input_language($2); }
          | RE '&' RE        { $$ = intersect($1, $3); }
          | RE '-' RE        { $$ = subtract($1, $3); }
          | RE '|' RE        { $$ = disjunct($1, $3); }
          | '(' RE ')'       { $$ = $2; }
          | STRING           { $$ = read_words($1); }
          | STRING2          { $$ = read_transducer_and_harmonize($1); }
          | STRING3          {} //{ if (strcmp($1,"") == 0) { $$ = read_transducer_text(stdin,NULL,false); } else { $$ = read_transducer_text($1,NULL,false); } }
          ;

RANGES:     RANGE RANGES     { $$ = add_range($1,$2); }
          |                  { $$ = NULL; }
          ;

RANGE:      '[' VALUES ']'   { $$=$2; }
          | '[' '^' VALUES ']' { $$=complement_range($3); }
          | '[' RSVAR ']'    { $$=copy_range_agreement_variable_value($2); }
          | '.'              { $$=NULL; }
          | CODE             { $$=add_value($1,NULL); }
          ;

CONTEXTS2:  CONTEXTS               { $$ = $1; }
          | '(' CONTEXTS ')'       { $$ = $2; }
          ;

CONTEXTS:   CONTEXT ',' CONTEXTS   { $$ = append_context($1,$3); }
          | CONTEXT                { $$ = $1; }
          ;

CONTEXT2:   CONTEXT                { $$ = $1; }
          | '(' CONTEXT ')'        { $$ = $2; }
          ;

CONTEXT :   RE POS RE              { $$ = make_context($1, $3); }
          |    POS RE              { $$ = make_context(NULL, $2); }
          | RE POS                 { $$ = make_context($1, NULL); }
          ;

VALUES:     LCHAR '-' LCHAR VALUES	{ $$=add_values($1,$3,$4); }
          | LCHAR '-' LCHAR		{ $$=add_values($1,$3,NULL); }
          | SVAR  VALUES		{ $$=add_var_values($1,$2); }
          | SVAR                        { $$=copy_range_variable_value($1); }
          | CODE VALUES			{ $$=add_value($1,$2); }
          | CODE			{ $$=add_value($1,NULL); }
          | SCHAR VALUES		{ $$=add_value($1,$2); }
	  | SCHAR			{ $$=add_value($1,NULL); }
          ;

LCHAR:      CHARACTER	{ $$=$1; }
	  | SCHAR       { $$=$1; }
          | UTF8CHAR	{ $$=utf8_to_int($1); } // ?
          ;

CODE:       CHARACTER	{ $$=character_code($1); }
          | UTF8CHAR	{ $$=symbol_code(strdup($1)); }
          | SYMBOL	{ $$=symbol_code($1); }
          ;

SCHAR:      '.'		{ $$=character_code('.'); }
          | '!'		{ $$=character_code('!'); }
          | '?'		{ $$=character_code('?'); }
          | '{'		{ $$=character_code('{'); }
          | '}'		{ $$=character_code('}'); }
          | ')'		{ $$=character_code(')'); }
          | '('		{ $$=character_code('('); }
          | '&'		{ $$=character_code('&'); }
          | '|'		{ $$=character_code('|'); }
          | '*'		{ $$=character_code('*'); }
          | '+'		{ $$=character_code('+'); }
          | ':'		{ $$=character_code(':'); }
          | ','		{ $$=character_code(','); }
          | '='		{ $$=character_code('='); }
          | '_'		{ $$=character_code('_'); }
          | '^'		{ $$=character_code('^'); }
          | '-'		{ $$=character_code('-'); }
          ;

NEWLINES:   NEWLINE NEWLINES     {}
          | /* nothing */        {}
          ;

%%

extern FILE *yyin;
static int Compact=0;
static int LowMem=0;
bool Verb=false;
char *input_filename=NULL;
char *output_filename=NULL;
char *store_filename=NULL;  // where symbols table is stored
char *read_filename=NULL;   // where symbol table is read
bool store_symbols=true;
bool backwards_compatible=false;

void print_usage(const char *program_name);
void print_version(const char* program_name);



void yyerror(char *text) {
  printf("\nInputfile: %i: %s at: %s \naborted.\n", yylineno, text, yytext);
  exit(1);
}

void warn(char *text, char *FileName) {
  fprintf(stderr, "\n%s: %i: warning: %s!\n", FileName, yylineno, text);
}

void warn2(char *text, char *text2, char *FileName) {
  fprintf(stderr, "\n%s: %i: warning: %s!: %s\n", FileName, yylineno, text, text2);
}

void delete_keypair_set(KeyPairSet *kps) {
    if (kps != NULL) { 
      for (KeyPairSet::iterator it=kps->begin(); it!=kps->end(); it++)
        delete(*it);      
      kps->clear(); 
      delete kps;
    }
}  


/*FA****************************************************************/
/*                                                                 */
/*  get_flags                                                      */
/*                                                                 */
/*FE****************************************************************/

void get_flags( int *argc, char **argv )

{
  for( int i=1; i<*argc; i++ ) {
    if (strcmp(argv[i],"-h") == 0 ||
	strcmp(argv[i],"--help") == 0 ) {
      print_usage(argv[0]);
      exit(0);
    }
    if (strcmp(argv[i],"-q") == 0 ||
	strcmp(argv[i],"--quiet") == 0 ||
	strcmp(argv[i],"-s") == 0 ||
	strcmp(argv[i],"--silent") == 0 ) {
      Verb=false;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-v") == 0 ||
	     strcmp(argv[i],"--verbose") == 0 ) {
      Verb=true;
    }
    else if (strcmp(argv[i],"-V") == 0 ||
	     strcmp(argv[i],"--version") == 0 ) {
      print_version(argv[0]);
      exit(0);
    }
    else if (strcmp(argv[i],"--compact") == 0 ||
	     strcmp(argv[i],"-c") == 0 ) {
      Compact = 1;
      Switch = !Switch;  // Look-up is done from output to input
		         // in compact transducers (Added by Erik Axelson)
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-l") == 0) {  // not implemented
      LowMem = 1;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--invert") == 0 ||
	     strcmp(argv[i],"-I") == 0 ) {
      Switch = !Switch;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--do-not-write-symbols") == 0 ||
	     strcmp(argv[i],"-D") == 0 ) {
      store_symbols=false;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--compatible") == 0 ||
	     strcmp(argv[i],"-C") == 0 ) {
      backwards_compatible=true;
      argv[i] = NULL;
    }
    else if (strncmp(argv[i],"--input=", (size_t)8) == 0) {
        input_filename = &(argv[i][8]);
	//fprintf(stderr, "\ninput_file: %s\n", input_filename);
    }
    else if (strncmp(argv[i],"--write-symbols-to=", (size_t)19) == 0) {
        store_filename = &(argv[i][19]);
	//fprintf(stderr, "\nstore alpha_file: %s\n", store_filename);
    }
    else if (strncmp(argv[i],"--read-symbols=", (size_t)15) == 0 ) {
        read_filename = &(argv[i][15]);
	//fprintf(stderr, "\nread alpha_file: %s\n", read_alphabet_filename);
    }
    else if (strncmp(argv[i],"--output=", (size_t)9) == 0) {
        output_filename = &(argv[i][9]);
	//fprintf(stderr, "\noutput_file: %s\n", output_filename); 
    }
    else if (strcmp(argv[i],"-i") == 0 || 
    strcmp(argv[i],"--input") == 0) {
      if (i+1 < *argc) {
        input_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-W") == 0 || 
    strcmp(argv[i],"--write-symbols-to") == 0) {
      if (i+1 < *argc) {
        store_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-R") == 0 ||
    strcmp(argv[i],"--read-symbols") == 0 ) {
      if (i+1 < *argc) {
        read_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-o") == 0 ||
    strcmp(argv[i],"--output") == 0 ) {
      if (i+1 < *argc) {
        output_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
        exit(0);
      }
    }
    else
      input_filename = argv[i];
  }
  // remove flags from the argument list
  int k;
  for( int i=k=1; i<*argc; i++)
    if (argv[i] != NULL)
      argv[k++] = argv[i];
  *argc = k;
}





void print_usage(const char *program_name) {

	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(stdout, "Usage: %s [OPTIONS...] [INFILE]\n"
#ifdef WEIGHTED
		   "Create a weighted transducer as defined in SFST calculus\n"
#else
		   "Create an unweighted transducer as defined in SFST calculus\n"
#endif
		   "\n", program_name);
print_common_program_options(stdout);
print_common_unary_program_options(stdout);


#               if DEBUG
	fprintf(stdout,	"%-35s%s", "  -d, --debug", "Print debugging messages and results\n");
#               endif
#ifndef WEIGHTED
        fprintf(stdout,	"%-35s%s", "  -C, --compatible", "Write the result in SFST compatible format.\n");
#else
	fprintf(stdout,	"%-35s%s", "  -C, --compatible", "Write the result in OpenFst compatible format.\n");
#endif 
       fprintf(stdout, "%-35s%s", "  -I, --invert", "Invert the result before writing.\n");
#ifndef WEIGHTED
       fprintf(stdout, "%-35s%s", "  -c, --compact", "Write the result in SFST compact format.\n");
       fprintf(stdout, "%-35s%s", "",               "(Not supported by HFST tools)\n"
);
#endif
		   fprintf(stdout, "\n"
		   "If OUTFILE or INFILE is missing or -,"
		   "standard streams will be used.\n"
		   "If no symbol table file is given, the program automatically creates one as it encounters new symbols.\n"
		   );
		   fprintf(stdout, "\n");
		   print_more_info(stdout, "Calculate");
		   fprintf(stdout, "\n");
		   print_report_bugs(stdout);
}


void print_version(const char* program_name) {

	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(stdout, "%s 0.1 (" PACKAGE_STRING ")\n"
		   "copyright (C) 2008 University of Helsinki,\n"
		   "License GPLv3: GNU GPL version 3 "
		   "<http://gnu.org/licenses/gpl.html>\n"
		   "This is free software: you are free to change and redistribute it.\n"
		   "There is NO WARRANTY, to the extent permitted by law.\n",
		program_name);
}



/*FA****************************************************************/
/*                                                                 */
/*  main                                                           */
/*                                                                 */
/*FE****************************************************************/

int main( int argc, char *argv[] )

{

  get_flags(&argc, argv);


#ifdef WEIGHTED
  if (backwards_compatible && store_symbols) {
    store_symbols=false;
  }
#endif

  FILE *input_file=NULL;

  if (input_filename == NULL || strcmp(input_filename, "-") == 0 ) {
    FileName = (char*)"stdin";
    yyin = stdin;	
  }
  else {
    input_file = fopen(input_filename,"rt");
    if (input_file == NULL) {
      fprintf(stderr,"\nError: Cannot open grammar file \"%s\"\n\n", input_filename);
      exit(1);
    }
    else {
      FileName = input_filename;
      yyin = input_file;
    }
  }

  FILE *output_file=NULL;
  if (output_filename != NULL) {
    output_file = fopen(output_filename,"w");
    if (output_file == NULL) {
      fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", output_filename);
      exit(1);
    }
    fclose(output_file);
  }
  Result = NULL;

  add_epsilon_to_alphabet();
  if (read_filename != NULL) {
    ifstream is(read_filename);
    if (Verb)
      fprintf(stderr, "reading symbol table from \"%s\"\n", read_filename);
    read_symbol_table_text(is);	
    is.close();	
  }
  set_alphabet_defined(0);	

  try {
    yyparse();

    if (Verb && is_empty(Result)) {
      warn((char*)"resulting transducer is empty",FileName);
    } 

#ifndef WEIGHTED 
    if (Compact) {
      if (output_filename != NULL) {
        output_file = fopen(output_filename,"w");
        if (output_file == NULL) {
          fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", output_filename);
          exit(1);
        }
        store_as_compact(Result, output_file);
        fclose(output_file);
      } 
      else
        store_as_compact(Result, stdout);
      exit(0);	     
    }
    //Result = explode_and_minimise(Result);
#endif

    if (output_filename != NULL ) {
      if (store_symbols)
        write_transducer(Result, output_filename, NULL, backwards_compatible);  // a NULL KeyTable -> TheAlphabet is written
      else
        write_transducer(Result, output_filename, backwards_compatible);
    }
    else {
      if (store_symbols) 
        write_transducer(Result, NULL, cout, backwards_compatible);
      else
        write_transducer(Result, cout, backwards_compatible);
    }
    delete_transducer(Result);
    if (store_filename != NULL) {
      ofstream os(store_filename);
      write_symbol_table(NULL, os);
      os.close();
    }
    delete_keypair_set(Pi);
  }
  catch(const char* p) {
      fprintf(stderr, "\n%s\n\n", p);
      exit(1);
  }
}

