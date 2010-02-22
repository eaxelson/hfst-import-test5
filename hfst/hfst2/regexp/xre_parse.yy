%{

#include <stdio.h>
#include <assert.h>

#include "../hfst.h"

#include "xre_lex.h"
#include "xre.h"

extern void xreerror(char * text);

#ifndef YYDEBUG
#define YYDEBUG 1
#endif


%}

%error-verbose

%union {
	int value;
	int* values;
    double weight;
	char* name;
	Key symbol;
	HFST::TransducerHandle transducer;
}


%type	<transducer>	ONE_REGEXP

%left	<transducer>	DIFFERENCE
%left	<transducer>	INTERSECTION
%left	<transducer>	UNION
%left	<transducer>	IGNORING IGNORE_INTERNALLY
%left	<transducer>	COMPOSITION CROSS_PRODUCT

%left	<transducer>	CONCATENATE

%token	<transducer>	CONTAINMENT CONTAINMENT_ONCE CONTAINMENT_OPT TERM_COMPLEMENT COMPLEMENT
%token	<value>			CATENATE_N CATENATE_N_PLUS CATENATE_N_MINUS
%token	<values>		CATENATE_N_TO_K
%token	<transducer>	STAR PLUS
%token	<transducer>	PAIR_SEPARATOR PAIR_SEPARATOR_SOLE PAIR_SEPARATOR_WO_LEFT PAIR_SEPARATOR_WO_RIGHT
%token	<transducer> ANY EPSILON_TOKEN
%token	<name>		READ_BIN READ_TEXT READ_SPACED READ_PROLOG
%token	<name>		SYMBOL
%token	<transducer>	REVERSE INVERT UPPER LOWER

%token	<transducer>	BEFORE AFTER SHUFFLE

%type	<transducer>	IMPLICIT_PAIR HALF_PAIR PAIR RE REG REGULAR_EXPRESSION
%type	<transducer>	PRECEDENCE_BRACKETS
%type	<transducer>	OPTIONALITY_BRACKETS
%token	<name>			QUOTED_LITERAL 
%type	<symbol>		CHAR
%token  <weight>    END_OF_WEIGHTED_EXPRESSION

%token  <transducer>    LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_CURLY RIGHT_CURLY END_OF_EXPRESSION
%token  <transducer>    LEFT_RESTRICTION LEFT_RIGHT_ARROW LEFT_ARROW RIGHT_ARROW REPLACE_ARROW OPTIONAL_REPLACE LTR_LONGEST_REPLACE LTR_SHORTEST_REPLACE RTL_LONGEST_REPLACE RTL_SHORTEST_REPLACE REPLACE_CONTEXT_UU REPLACE_CONTEXT_LU REPLACE_CONTEXT_UL REPLACE_CONTEXT_LL CENTER_MARKER BOUNDARY_MARKER ERROR
%%

ONE_REGEXP: REGULAR_EXPRESSION
	{
		$$ = _xre_transducer_ = HFST::minimize($1);
	}
	| REGULAR_EXPRESSION END_OF_EXPRESSION
	{
		$$ = _xre_transducer_ = HFST::minimize($1);
	}
    | REGULAR_EXPRESSION END_OF_WEIGHTED_EXPRESSION
    {
        $$ = _xre_transducer_ = HFST::add_weight(HFST::minimize($1), $2);
    }
	;

REGULAR_EXPRESSION: REGULAR_EXPRESSION UNION REG
	{
		$$ = HFST::disjunct($1, $3);
	}
	| REGULAR_EXPRESSION INTERSECTION REG 
	{
		$$ = HFST::intersect($1, $3); 
	}
	| REGULAR_EXPRESSION DIFFERENCE REG
	{
		$$ = HFST::subtract($1, $3); 
	}
	| REGULAR_EXPRESSION COMPOSITION REG
	{
		$$ = HFST::compose($1, $3);
	}
	| REGULAR_EXPRESSION SHUFFLE REG
	{
		$$ = HFST::shuffle($1, $3);
	}
	| REGULAR_EXPRESSION CROSS_PRODUCT REG
	{
	  $$ = 
	    HFST::compose
	    ($1,HFST::compose
	     (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	      $3));
	}
	| REGULAR_EXPRESSION IGNORING REG
	{
	  HFST::KeyPair * non_existent_pair = 
	    HFST::define_keypair(HFST::get_unused_key(_xre_key_table));
	  $$ = HFST::insert_freely($1,non_existent_pair);
	  $$ = HFST::substitute_with_transducer($$,non_existent_pair,$3); 
	  delete non_existent_pair;
	}
	| REGULAR_EXPRESSION IGNORE_INTERNALLY REG
	{
	  HFST::KeyPair * non_existent_pair = 
	    HFST::define_keypair(HFST::get_unused_key(_xre_key_table));
	  HFST::TransducerHandle freely_insert_universal =
	    HFST::insert_freely
	    (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	     non_existent_pair);

	  $$ = HFST::insert_freely($1,non_existent_pair);
	  $$ = HFST::subtract
	    ($$,
	     HFST::concatenate
	     (HFST::define_transducer(non_existent_pair),
	      HFST::copy(freely_insert_universal)));
	  $$ = HFST::subtract
	    ($$,
	     HFST::concatenate
	     (freely_insert_universal,
	      HFST::define_transducer(non_existent_pair)));

	  $$ = HFST::substitute_with_transducer($$,non_existent_pair,$3); 
	  delete non_existent_pair;
	}
	| REG { $$ = $1; }
	;

REG: REG CONCATENATE RE 
	{ 
		$$ = HFST::concatenate( $1, $3 );
	}
	| REG RE 
	{
		$$ = HFST::concatenate( $1, $2 );
	}
	| RE
	{
		$$ = $1;
	}
	;

RE: CONTAINMENT RE
	{ 
	  $$ =
	    HFST::concatenate
	    (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	     HFST::concatenate
	     ($2,
	      HFST::repeat_star(HFST::define_transducer(_xre_creation_pi()))));
	  
	}
	| CONTAINMENT_ONCE RE
	{ 
	  HFST::print_transducer_number($2);
	  HFST::TransducerHandle containment =
	    HFST::concatenate
	    (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	     HFST::concatenate
	     ($2,
	      HFST::repeat_star(HFST::define_transducer(_xre_creation_pi()))));
	  HFST::TransducerHandle containment_twice =
	    HFST::concatenate(HFST::copy(containment),HFST::copy(containment));
	  $$ = HFST::subtract(containment,containment_twice);
	}
	| CONTAINMENT_OPT RE	
	{ 
	  HFST::TransducerHandle containment =
	    HFST::concatenate
	    (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	     HFST::concatenate
	     ($2,
	      HFST::repeat_star(HFST::define_transducer(_xre_creation_pi()))));
	  HFST::TransducerHandle containment_twice =
	    HFST::concatenate(HFST::copy(containment),containment);
	  $$ = 
	    HFST::subtract
	    (HFST::repeat_star(HFST::define_transducer(_xre_creation_pi())),
	     containment_twice);
	}
	| TERM_COMPLEMENT RE
	{
		HFST::KeyPairSet* pi = _xre_negation_pi();
		$$ = HFST::negate($2, pi);
		delete pi;
	}
	| COMPLEMENT RE
	{
		HFST::KeyPairSet* pi = _xre_negation_pi();
		$$ = HFST::negate($2, pi);
		delete pi;
	} 
	| RE STAR
	{
		$$ = HFST::repeat_star($1);
	}
	| RE PLUS
	{
		$$ = HFST::repeat_plus($1);
	}
	| RE CATENATE_N
	{
		$$ = HFST::repeat_n($1, $2);
	}
	| RE CATENATE_N_PLUS
	{
		HFST::TransducerHandle n_times = HFST::repeat_n(HFST::copy($1), $2);
		HFST::TransducerHandle more_times = HFST::repeat_star($1);
		$$ = HFST::concatenate(n_times, more_times);
	}
	| RE CATENATE_N_MINUS
	{
		$$ = HFST::repeat_le_n($1, $2);
	}
	| RE CATENATE_N_TO_K
	{
		HFST::TransducerHandle n_times = HFST::repeat_n(HFST::copy($1), $2[0]);
		HFST::TransducerHandle to_k_times = HFST::repeat_le_n($1, $2[1] - $2[0]);
		$$ = HFST::concatenate(n_times, to_k_times);
		free($2);
	}
	| RE REVERSE
	{
		$$ = HFST::reverse($1);
	}
	| RE INVERT
	{
		$$ = HFST::invert($1);
	}
	| RE UPPER
	{
		$$ = HFST::extract_input_language($1);
	}
	| RE LOWER
	{
		$$ = HFST::extract_output_language($1);
	}
	| PRECEDENCE_BRACKETS
	{
		$$ = $1;
	}
	| OPTIONALITY_BRACKETS 
	{
		// optionality is handled on lower level already
		$$ = $1;
	}
	| PAIR
	{
		$$ = $1;
	}
	| HALF_PAIR
	{
		$$ = $1;
	}
	| IMPLICIT_PAIR
	{
		$$ = $1;
	}
	| READ_BIN
	{
		char* fn = strdup($1);
		$$ = HFST::read_transducer(fn, _xre_key_table);
		_xre_new_transducer($$);
		free($1);
		free(fn);
	}
	| READ_TEXT
	{
		fprintf(stderr, "*** XRE: read_text unimplemented");
		free($1);
		assert(false);
	}
;

PRECEDENCE_BRACKETS: LEFT_BRACKET REGULAR_EXPRESSION RIGHT_BRACKET 
	{
		$$ = $2; 
	}
	| LEFT_CURLY REGULAR_EXPRESSION RIGHT_CURLY
	{
		/* Xerox defines curly may only bracket disjunctions,
		 * but, meh, I’ll synonymise it to square brackets.
		 */
		$$ = $2;
	}
	| LEFT_BRACKET RIGHT_BRACKET 
	{
		$$ = HFST::create_empty_transducer(); 
	}
	| LEFT_CURLY RIGHT_CURLY
	{
		$$ = HFST::create_empty_transducer();
	}
	;

OPTIONALITY_BRACKETS: LEFT_PARENTHESIS REGULAR_EXPRESSION RIGHT_PARENTHESIS
	{
		$$ = HFST::optionalize($2);
	}
	| LEFT_PARENTHESIS RIGHT_PARENTHESIS {
		$$ = HFST::create_epsilon_transducer(); 
	}
	;

PAIR: CHAR PAIR_SEPARATOR CHAR 
	{
		$$ = _xre_make_key_pair($1, $3);
	}
	| CHAR PAIR_SEPARATOR ANY {
	$$ = _xre_make_key_pair($1, ANY_KEY);
	}
	| ANY PAIR_SEPARATOR CHAR {
		$$ = _xre_make_key_pair(ANY_KEY, $3); 
	}
	| ANY PAIR_SEPARATOR ANY {
		$$ = _xre_make_key_pair(ANY_KEY, ANY_KEY);  
	}
	| CHAR PAIR_SEPARATOR EPSILON_TOKEN {
		$$ = _xre_make_key_pair($1, 0);
	}
	| EPSILON_TOKEN PAIR_SEPARATOR EPSILON_TOKEN {
		$$ = HFST::create_epsilon_transducer();
	}
	| EPSILON_TOKEN PAIR_SEPARATOR CHAR	 {
		$$ = _xre_make_key_pair(0, $3);
	}
	| EPSILON_TOKEN PAIR_SEPARATOR ANY {
		$$ = _xre_make_key_pair(0, ANY_KEY);
	}
	| ANY PAIR_SEPARATOR EPSILON_TOKEN	 {
		$$ = _xre_make_key_pair(ANY_KEY, 0);
	}
	;

HALF_PAIR: EPSILON_TOKEN PAIR_SEPARATOR_WO_RIGHT	 {
				$$ = _xre_make_key_pair(0, ANY_KEY);
			}
		| PAIR_SEPARATOR_WO_LEFT EPSILON_TOKEN	 {
				$$ = _xre_make_key_pair(ANY_KEY, 0);
			}
		| CHAR PAIR_SEPARATOR_WO_RIGHT { 
				$$ = _xre_make_key_pair($1, ANY_KEY);  
			}
		| PAIR_SEPARATOR_WO_LEFT CHAR {
				$$ = _xre_make_key_pair(ANY_KEY, $2);
			}
		;

IMPLICIT_PAIR: CHAR 
		{
			if (_xre_definitions.find($1) != _xre_definitions.end())
			{
				$$ = HFST::copy(_xre_definitions[$1]);
			}
			else
			{
				$$ = _xre_make_key_pair($1, $1);
			}
		}
		| ANY	
		{
			HFST::KeyPairSet* pi = _xre_creation_pi();
			$$ = HFST::define_transducer(pi);
			delete pi;
		}
		|		
		EPSILON_TOKEN {
			$$ = HFST::create_epsilon_transducer();
		}
		| PAIR_SEPARATOR_SOLE
			{
				HFST::KeyPairSet* pi = _xre_creation_pi();
				$$ = HFST::define_transducer(pi);
				delete pi;
			}
		;

CHAR: SYMBOL 
	{
		$$ = _xre_string_to_key($1);
		_xre_new_key($$);
		free($1);
	}
	| QUOTED_LITERAL 
	{
		$$ = _xre_string_to_key($1);
		_xre_new_key($$);
		free($1);
	}
	;


%%

