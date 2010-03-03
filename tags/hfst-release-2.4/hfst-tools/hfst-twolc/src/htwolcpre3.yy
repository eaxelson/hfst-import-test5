%{
  
#include <iostream>
#include <map>
#include <set>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include "../config.h"
#include <hfst2/hfst.h>
#include "CommandLineParser.h"

  extern CommandLineParser command_line_parser;

#include "RuleStorer.h"
#include "InputReader.h"
  
  extern int yylineno;
  extern char * yytext;
  InputReader input_reader(yylineno);

#include "GrammarCompiler.h"
  GrammarCompiler<Unweighted> unweighted_grammar_compiler;
  GrammarCompiler<Weighted> weighted_grammar_compiler;

#define YYDEBUG 1

#define YYERROR_VERBOSE 1
 
  bool verbose = false;
  bool weighted = false;
  void yyerror(const char * text );
  void warn(const char * warning );
  int yylex();
  int yyparse();

%}



%union {
  int symbol_number;
  unsigned int operator_number;
  int exponent;
  char * name;
  char * value;
  char * test_form;
  unsigned int operator_type;
  unsigned int matcher_type;
  HFST::KeyPair * key_pair;
  HFST::KeyPairSet * key_pair_set;
  TransducerType * transducer_type;
};


 /* 
    All unary operators have stronger precedence than binary ones.
 */

 /* Binary operators ordered by precedence from lowest to highest. */
%left  <symbol_number> FREELY_INSERT
%left  <symbol_number> DIFFERENCE
%left  <symbol_number> INTERSECTION
%left  <symbol_number> UNION

 /* Unary operators ordered by precedence from lowest to highest. */
%right <symbol_number> STAR PLUS
%left  <symbol_number> CONTAINMENT CONTAINMENT_ONCE TERM_COMPLEMENT COMPLEMENT 
%right <symbol_number> POWER

 /* Range delimiters. */
%right <symbol_number> RIGHT_SQUARE_BRACKET RIGHT_BRACKET LEFT_PAIR_BRACKET
%left  <symbol_number> LEFT_SQUARE_BRACKET LEFT_BRACKET RIGHT_PAIR_BRACKET

 /* The arrow-tokens. */
%token <symbol_number> LEFT_RESTRICTION_ARROW LEFT_ARROW 
%token <symbol_number> RIGHT_ARROW LEFT_RIGHT_ARROW

 /* Basic tokens. */
%token <symbol_number>   NEWLINE PAIR_SEPARATOR PAIR_SEPARATOR_LEFT 
%token <symbol_number>   PAIR_SEPARATOR_RIGHT PAIR_SEPARATOR_BOTH
%token <symbol_number>   WHERE MATCHED MIXED FREELY IN AND
%token <symbol_number>   EOL EQUALS CENTER_MARKER
%token <symbol_number>   ALPHABET_DECLARATION DIACRITICS_DECLARATION 
%token <symbol_number>   SETS_DECLARATION DEFINITION_DECLARATION
%token <symbol_number>   RULES_DECLARATION VARIABLE_DECLARATION 
%token <name>            RULE_NAME 
%token <value>           SYMBOL NUMBER

%type<value> CHAR HEADER
%type<operator_number> OPERATOR
%type<key_pair> DIACRITIC_PAIR LEFT_SIDE_OF_RULE CENTER_PAIR PLAIN_PAIR
%type<transducer_type> PAIR RE REG REGULAR_EXPRESSION PARENTHESIS RANGE
%type<transducer_type> CONTEXT RIGHT_SIDE_OF_RULE  
%type<key_pair_set> FREELY_INSERT_RANGE RANGE_OF_PAIRS LEFT_SIDES_OF_RULE PAIR_RANGE
%type<key_pair_set> SIMPLE_PAIR_RANGE
%%

ALL: GRAMMAR {}
;

GRAMMAR: ALPHABET GRAMMAR_RULES {}
;

GRAMMAR_RULES: RULES_DECLARATION RULES {}
;

/* Define unknown pair */
ALPHABET: ALPHABET_DECLARATION ALPHABET_LINES 
{ Rule::alphabet.define_pair(string_copy("@?@"),string_copy("@?@")); };

ALPHABET_LINES: ALPHABET_LINES NON_DELIMITER_RANGE EOL 
{}
|
NON_DELIMITER_RANGE EOL
{};

RULES: RULE_DEFINITIONS {}

RULE_DEFINITIONS:
RULE_DEFINITIONS RULE 
{}
| RULE                  
{};

RULE: HEADER LEFT_SIDE_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE  
{
  if (weighted)
    {
      weighted_grammar_compiler.add_rule($1,$2,$3,*($4->w));
      delete $4->w;
    }
  else
    {
      unweighted_grammar_compiler.add_rule($1,$2,$3,*($4->u));
      delete $4->u;
    }
  delete $4;
}
| HEADER LEFT_SIDES_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE 
{
  if (weighted)
    {
      weighted_grammar_compiler.add_rule($1,$2,$3,*($4->w));
      delete $4->w;
    }
  else
    {
      unweighted_grammar_compiler.add_rule($1,$2,$3,*($4->u));
      delete $4->u;
    }
  delete $4;
}
;

RIGHT_SIDE_OF_RULE: 
RIGHT_SIDE_OF_RULE CONTEXT EOL 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->disjunct(*($2->w)));
      delete $1->w;
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->disjunct(*($2->u)));
      delete $1->u;
      delete $2->u;
    }
  delete $1;
  delete $2;
}   
| RIGHT_SIDE_OF_RULE CENTER_MARKER EOL 
{
  $$ = new TransducerType;
  if (weighted)
    {
      Weighted
	w(RuleContextCondition<Weighted>(Weighted(),
					 Weighted(),
					 Rule::alphabet)());      
      $$->w = new Weighted($1->w->disjunct(w));
      delete $1->w;
    }
  else
    {
      Unweighted 
	u(RuleContextCondition<Unweighted>(Unweighted(),
					   Unweighted(),
					   Rule::alphabet)());
      $$->u = new Unweighted($1->u->disjunct(u));
      delete $1->u;
    }
  delete $1;
}
| CONTEXT EOL 
{
  $$ = $1;
}
| CENTER_MARKER EOL 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted
	(RuleContextCondition<Weighted>(Weighted(),
					Weighted(),
					Rule::alphabet)());
    }
  else
    {
      $$->u = new Unweighted
	(RuleContextCondition<Unweighted>(Unweighted(),
					  Unweighted(),
					  Rule::alphabet)());
    }
}
;

CONTEXT: REGULAR_EXPRESSION CENTER_MARKER REGULAR_EXPRESSION          
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted
	(RuleContextCondition<Weighted>(*($1->w),
					*($3->w),
					Rule::alphabet)());
      delete $1->w;
      delete $3->w;
    }
  else
    {
      $$->u = new Unweighted
	(RuleContextCondition<Unweighted>(*($1->u),
					  *($3->u),
					  Rule::alphabet)());
      delete $1->u;
      delete $3->u;
    }
  delete $1;
  delete $3;
}
| CENTER_MARKER REGULAR_EXPRESSION 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted
	(RuleContextCondition<Weighted>(Weighted(),
					*($2->w),
					Rule::alphabet)());
    }
  else
    {
      $$->u = new Unweighted
	(RuleContextCondition<Unweighted>(Unweighted(),
					  *($2->u),
					  Rule::alphabet)());
    }

}
| REGULAR_EXPRESSION CENTER_MARKER 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted
	(RuleContextCondition<Weighted>(*($1->w),
					Weighted(),
					Rule::alphabet)());
    }
  else
    {
      $$->u = new Unweighted
	(RuleContextCondition<Unweighted>(*($1->u),
					  Unweighted(),
					  Rule::alphabet)());
    }
};


OPERATOR: LEFT_RESTRICTION_ARROW 
{
  $$ = RESTRICTION_RULE;
}
| LEFT_ARROW 
{
  $$ = LEFT_RULE;
}
| RIGHT_ARROW 
{
  $$ = RIGHT_RULE;
}
| LEFT_RIGHT_ARROW 
{
  std::cerr << "This shouldn't happen. It's a bug." << std::endl
	    << "please report it to hfst-bugs@helsinki.fi" << std::endl;
  assert(false);
}
;

HEADER: RULE_NAME 
{ 
  if (verbose)
    {
      std::cerr << "Precompiling rule " << $1 << std::endl;
    }
  $$ = $1; 
};

LEFT_SIDE_OF_RULE: CENTER_PAIR 
{ $$ = $1; };

LEFT_SIDES_OF_RULE: RANGE_OF_PAIRS 
{
  $$ = $1;
};

CENTER_PAIR: SYMBOL PAIR_SEPARATOR SYMBOL 
{
  $$ = HFST::define_keypair(Rule::alphabet.get_key($1),
			    Rule::alphabet.get_key($3));
};

RANGE_OF_PAIRS: RANGE_OF_PAIRS UNION CENTER_PAIR 
{
  $1->insert($3);
  $$ = $1;
}
| RANGE_OF_PAIRS UNION PAIR_RANGE
{
  $$ = Rule::alphabet.join($1,$3);
} 
| PAIR_RANGE
{
  $$ = $1;
}
| CENTER_PAIR 
{
  $$ = new KeyPairSet;
  $$->insert($1);
};

REGULAR_EXPRESSION: REGULAR_EXPRESSION UNION REG 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->disjunct(*($3->w)));
      delete $1->w;
      delete $3->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->disjunct(*($3->u)));
      delete $1->u;
      delete $3->u;
    }
  delete $1;
  delete $3;
}
| REGULAR_EXPRESSION INTERSECTION REG 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->intersect(*($3->w)));
      delete $1->w;
      delete $3->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->intersect(*($3->u)));
      delete $1->u;
      delete $3->u;
    }
  delete $1;
  delete $3;
}
| REGULAR_EXPRESSION DIFFERENCE REG 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->subtract(*($3->w)));
      delete $1->w;
      delete $3->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->subtract(*($3->u)));
      delete $1->u;
      delete $3->u;
    }
  delete $1;
  delete $3;
}
| REGULAR_EXPRESSION FREELY_INSERT LEFT_SQUARE_BRACKET
FREELY_INSERT_RANGE RIGHT_SQUARE_BRACKET 
{
  $$ = $1;
  if (weighted)
    {
      $$->w->insert($4);
    }
  else
    {
      $$->u->insert($4);
    }
}
| REG 
{
  $$ = $1;
};

REG: REG RE 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->concatenate(*($2->w)));
      delete $1->w;
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->concatenate(*($2->u)));
      delete $1->u;
      delete $2->u;
    }
  delete $1;
  delete $2;
}
| RE 
{
  $$ = $1;
};

RE: CONTAINMENT RE 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($2->w->containment(Rule::alphabet));
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted($2->u->containment(Rule::alphabet));
      delete $2->u;
    }
  delete $2;
}
| CONTAINMENT_ONCE RE 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($2->w->containment_once(Rule::alphabet));
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted($2->u->containment_once(Rule::alphabet));
      delete $2->u;
    }
  delete $2;
}
| TERM_COMPLEMENT RE 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted(Weighted::any_pair(Rule::alphabet).
			   subtract(*($2->w)));
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted(Unweighted::any_pair(Rule::alphabet).
			     subtract(*($2->u)));
      delete $2->u;
    }
  delete $2;
}
| COMPLEMENT RE 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted(Weighted::universal(Rule::alphabet).
			   subtract(*($2->w)));
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted(Unweighted::universal(Rule::alphabet).
			     subtract(*($2->u)));
      delete $2->u;
    }
  delete $2;
}
| RE STAR 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->repeat_star());
      delete $1->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->repeat_star());
      delete $1->u;
    }
  delete $1;
}
| RE PLUS 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($1->w->repeat_plus());
      delete $1->w;
    }
  else
    {
      $$->u = new Unweighted($1->u->repeat_plus());
      delete $1->u;
    }
  delete $1;
}
| RE POWER NUMBER 
{
  assert(false);
}
| RANGE 
{
  $$ = $1;
}
| PARENTHESIS 
{
  $$ = $1;
}
| PAIR_RANGE
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted(Weighted::to_transducer($1));
    }
  else
    {
      $$->u = new Unweighted(Unweighted::to_transducer($1));
    }
  Rule::alphabet.destroy($1);
}
| PAIR 
{
  $$ = $1;
};

PAIR_RANGE: LEFT_PAIR_BRACKET SIMPLE_PAIR_RANGE RIGHT_PAIR_BRACKET
{ $$ = $2; }
;

SIMPLE_PAIR_RANGE: SIMPLE_PAIR_RANGE PLAIN_PAIR
{ 
  $$ = $1;
  $$->insert($2);
}
| PLAIN_PAIR
{
  $$ = new HFST::KeyPairSet;
  $$->insert($1);
}
;

PLAIN_PAIR: CHAR PAIR_SEPARATOR CHAR
{
  $$ = HFST::define_keypair(Rule::alphabet.get_key($1),
			    Rule::alphabet.get_key($3));
};

RANGE: LEFT_SQUARE_BRACKET REGULAR_EXPRESSION RIGHT_SQUARE_BRACKET 
{
  $$ = $2;
}
| LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET 
{  
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted();
    }
  else
    {
      $$->u = new Unweighted();
    }
};

PARENTHESIS: LEFT_BRACKET REGULAR_EXPRESSION RIGHT_BRACKET 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted($2->w->optionalize());
      delete $2->w;
    }
  else
    {
      $$->u = new Unweighted($2->u->optionalize());
      delete $2->u;
    }
  delete $2;
}
| LEFT_BRACKET RIGHT_BRACKET 
{
  $$ = new TransducerType;
  if (weighted)
    {
      $$->w = new Weighted();
    }
  else
    {
      $$->u = new Unweighted();
    }
};

NON_DELIMITER_RANGE: NON_DELIMITER_RANGE SIMPLE_PAIR {}
| /* nothing */ {};

FREELY_INSERT_RANGE: FREELY_INSERT_RANGE DIACRITIC_PAIR 
{ 
  $$ = $1;
  $$->insert($2);
}
| DIACRITIC_PAIR 
{
  $$ = new KeyPairSet;
  $$->insert($1);
};

DIACRITIC_PAIR: SYMBOL PAIR_SEPARATOR SYMBOL 
{ 
  $$ = HFST::define_keypair(Rule::alphabet.get_key($1),
			    Rule::alphabet.get_key($3));
};

SIMPLE_PAIR: CHAR PAIR_SEPARATOR CHAR 
{
  Rule::alphabet.define_pair($1,$3);
}

PAIR: CHAR PAIR_SEPARATOR CHAR 
{
  TransducerType * return_fst = new TransducerType;
  if (weighted)
    {
      return_fst->w = new Weighted($1,
				   $3,
				   Rule::alphabet);
    }
  else
    {
      return_fst->u = new Unweighted($1,
				     $3,
				     Rule::alphabet);
    }
  $$ = return_fst;
}
;

CHAR: SYMBOL { $$ = $1; }
| NUMBER { assert(false); };

%%

void warn(const char * warning) { }

void yyerror(const char * text) {
  input_reader.error(text);
  exit(1);
}

int main(int argc,char * argv[])
{
  CommandLineParser command_line_parser(argc,argv,false,false);
  if (command_line_parser.help or
      command_line_parser.usage or
      command_line_parser.version)
    {
      exit(0);
    }
  weighted = command_line_parser.weighted;
  verbose = command_line_parser.verbose;
  if (command_line_parser.verbose)
    {
      std::cerr << "Resolving rule conflicts and compiling rules."
		<< std::endl;
    }
  input_reader.set_input(&std::cin);
  //yydebug = 1;
  unweighted_grammar_compiler.set_booleans
    (command_line_parser.silent,
     command_line_parser.verbose,
     command_line_parser.resolve_conflicts);
  weighted_grammar_compiler.set_booleans
    (command_line_parser.silent,
     command_line_parser.verbose,
     command_line_parser.resolve_conflicts);
  if (not command_line_parser.weighted)
    {
      delete weighted_grammar_compiler();
      int exit_code = yyparse();
      if (exit_code != 0)
	{
	  exit(exit_code);
	}
      RuleStorer<Unweighted>
	(unweighted_grammar_compiler(),
	 Rule::alphabet.get_key_table(),
	 command_line_parser.output_file_defined,
	 command_line_parser.output_file_name,
	 command_line_parser.store_names,
	 command_line_parser.names_file_name,
	 command_line_parser.verbose)();
    }
  else
    {
      delete unweighted_grammar_compiler();
      int exit_code = yyparse();
      if (exit_code != 0)
	{
	  exit(exit_code);
	}
      RuleStorer<Weighted>
	(weighted_grammar_compiler(),
	 Rule::alphabet.get_key_table(),
	 command_line_parser.output_file_defined,
	 command_line_parser.output_file_name,
	 command_line_parser.store_names,
	 command_line_parser.names_file_name,
	 command_line_parser.verbose)();
    }
}


