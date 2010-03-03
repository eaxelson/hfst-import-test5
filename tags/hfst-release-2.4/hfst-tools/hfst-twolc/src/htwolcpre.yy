%{

  #include <iostream>
  #include <map>
  #include <set>
  #include <string.h>
  #include <fstream>
  #include <cstdlib>

  #include "CommandLineParser.h"

  #include "InputReader.h"
  extern int yylineno;
  extern char * yytext;
  InputReader input_reader(yylineno);

  #include "RuleModifier.h"
  RuleModifier rule_modifier;

  #include "GrammarDisplayer.h"
  GrammarDisplayer grammar_displayer;

  #include "grammar_defs.h"
  #include "rule_parts.h"

  //  bool UTF8 = true;

  extern int yylineno;
  extern char * yytext;

  bool test_fst;
#ifdef FST_DEBUG
 test_fst = 1;
#endif

#define YYDEBUG 1

#define YYERROR_VERBOSE 1
 
 bool verbose = false;
 
  void yyerror(const char * text );
  void warn(const char * warning );
  int yylex();
  int yyparse();

%}



%union {
  int symbol_number;
  int exponent;
  char * name;
  char * value;
  char * test_form;
  NonDelimiterRange * NonDelimiterRange_p;
  NonDelimiterSymbolRange * NonDelimiterSymbolRange_p;
  RuleStrings * RuleStrings_p;
  Pair * Pair_p;
  Set * Set_p;
  unsigned int operator_type;
  unsigned int matcher_type;
  RuleString * RuleString_p;
  RuleRepresentationVector * RuleRepresentationVector_p;
  CenterPairVector * CenterPairVector_p;
}


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
%right <symbol_number> RIGHT_SQUARE_BRACKET RIGHT_BRACKET 
%left  <symbol_number> LEFT_SQUARE_BRACKET LEFT_BRACKET

 /* The arrow-tokens. */
%token <symbol_number> LEFT_RESTRICTION_ARROW LEFT_ARROW 
%token <symbol_number> RIGHT_ARROW LEFT_RIGHT_ARROW

 /* Basic tokens. */
%token <symbol_number>   NEWLINE ANY  PAIR_SEPARATOR PAIR_SEPARATOR_LEFT 
%token <symbol_number>   PAIR_SEPARATOR_RIGHT PAIR_SEPARATOR_BOTH
%token <symbol_number>   WHERE MATCHED MIXED FREELY IN AND
%token <symbol_number>   EOL EQUALS CENTER_MARKER EPSILON LONELY_EPSILON
%token <symbol_number>   ALPHABET_DECLARATION DIACRITICS_DECLARATION 
%token <symbol_number>   SETS_DECLARATION DEFINITION_DECLARATION
%token <symbol_number>   RULES_DECLARATION VARIABLE_DECLARATION LONELY_ANY
%token <name>            RULE_NAME 
%token <value>           SYMBOL
%token <value>           NUMBER

%type <Pair_p>                    SIMPLE_PAIR LEFT_SIDE_OF_RULE CENTER_PAIR
%type <NonDelimiterRange_p>       NON_DELIMITER_RANGE
%type <NonDelimiterSymbolRange_p> NON_DELIMITER_SYMBOL_RANGE VARIABLE_SEQUENCE
%type <value>                     CHAR 
%type <Set_p>                     SET
%type <matcher_type>              MATCHER
%type <RuleString_p>              PAIR RE REG REGULAR_EXPRESSION RANGE 
%type <RuleString_p>              PARENTHESIS CONTEXT FREELY_INSERT_RANGE
%type <RuleString_p>              DIACRITIC_PAIR
%type <RuleRepresentationVector_p> RULE
%type <CenterPairVector_p>        RANGE_OF_PAIRS
%%

ALL: GRAMMAR {}
;

GRAMMAR: ALPHABET GRAMMAR1 {}
;

GRAMMAR1: DIACRITICS GRAMMAR2 {}
| GRAMMAR2 {}
;

GRAMMAR2: VARIABLES GRAMMAR3 {}
| GRAMMAR3           {}
;

GRAMMAR3: SETS GRAMMAR4 {}
| GRAMMAR4      {}
;

GRAMMAR4: DEFINITIONS GRAMMAR5 {}
| GRAMMAR5             {}
;

GRAMMAR5: GRAMMAR_RULES {}
;

GRAMMAR_RULES: RULES_DECLARATION RULES {}
;

ALPHABET: ALPHABET_DECLARATION ALPHABET_LINES {};

ALPHABET_LINES: ALPHABET_LINES NON_DELIMITER_RANGE EOL 
{
  rule_modifier.define_alphabet($2);
  grammar_displayer.display_alphabet($2);
  grammar_displayer.display_end_of_line();
}
| NON_DELIMITER_RANGE EOL
{
  $1->push_back(new Pair(string_copy("@#@"),
			 string_copy("@0@")));
  NonDelimiterSymbolRange * word_boundary_range = new NonDelimiterSymbolRange;
  //word_boundary_range->push_back(string_copy("@#@"));
  rule_modifier.define_diacritics(word_boundary_range);
  rule_modifier.define_alphabet($1);
  grammar_displayer.display_alphabet($1);
  grammar_displayer.display_end_of_line();
};

DIACRITICS: DIACRITICS_DECLARATION NON_DELIMITER_SYMBOL_RANGE EOL 
{
  rule_modifier.define_diacritics($2);
  NonDelimiterRange * diacritics =
    rule_modifier.get_diacritics();
  grammar_displayer.display_alphabet(diacritics);
  grammar_displayer.display_end_of_line();
}
;

VARIABLES: VARIABLE_DECLARATION NON_DELIMITER_SYMBOL_RANGE EOL {}
;

SETS: SETS_DECLARATION SET_DEFINITIONS {}
;


DEFINITIONS: DEFINITION_DECLARATION DEFS {}
;

DEFS: DEFS DEFINITION {}
| DEFINITION {}
;

DEFINITION: SYMBOL EQUALS REGULAR_EXPRESSION EOL 
{
  try 
    {
      rule_modifier.define_definition($1,$3);
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
;

SET_DEFINITIONS: SET_DEFINITIONS SET  
{
  grammar_displayer.display_set_definition($2);
}
| SET                  
{
  grammar_displayer.display_set_definition($1);
}
;

SET: SYMBOL EQUALS NON_DELIMITER_SYMBOL_RANGE EOL 
{
  try 
    {
      rule_modifier.define_set($1);
      rule_modifier.check_symbols($3);
      rule_modifier.add_set_symbols($1,$3);
    }
  catch (const char * w)
    {
      yyerror(w);
    }
  $$ = new Set($1,$3);
};

RULES: RULE_DEFINITIONS {}

RULE_DEFINITIONS:
RULE_DEFINITIONS RULE 
{ 
  grammar_displayer.display($2);
}
| 
RULE                  
{ 
  grammar_displayer.display($1);
}
;

RULE: HEADER LEFT_SIDE_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE  
{
  try 
    {
      RuleRepresentationVector * reduced_rules = 
	rule_modifier.replace_variables();
      rule_modifier.reset();
      $$ = reduced_rules;
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
| HEADER LEFT_SIDE_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE WHERE 
  VARIABLE_DEFINITIONS EOL 
{
  try
    {
      RuleRepresentationVector * reduced_rules = 
	rule_modifier.replace_variables();
      rule_modifier.reset();
      $$ = reduced_rules;
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
| HEADER LEFT_SIDES_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE 
{
  try 
    {
      RuleRepresentationVector * reduced_rules = 
	rule_modifier.replace_variables();
      rule_modifier.reset();
      $$ = reduced_rules;
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
| HEADER LEFT_SIDES_OF_RULE OPERATOR RIGHT_SIDE_OF_RULE WHERE 
  VARIABLE_DEFINITIONS EOL 
{
  try
    {
      RuleRepresentationVector * reduced_rules = 
	rule_modifier.replace_variables();
      rule_modifier.reset();
      $$ = reduced_rules;
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
;

RIGHT_SIDE_OF_RULE: 
RIGHT_SIDE_OF_RULE CONTEXT EOL          
{   
  rule_modifier.add_rule_context($2);
}
| RIGHT_SIDE_OF_RULE CENTER_MARKER EOL          
{   
  RuleString * universal_context_1 = 
    make_pair(grammar_displayer.get_any_symbol());
  universal_context_1 = adjoin(universal_context_1,
			       grammar_displayer.get_star_operator());
  RuleString * universal_context_2 = 
    make_pair(grammar_displayer.get_any_symbol());
  universal_context_2 = adjoin(universal_context_2,
			       grammar_displayer.get_star_operator());
  RuleString * universal = make_context(universal_context_1,
					universal_context_2);
  rule_modifier.add_rule_context(universal);
}
| CONTEXT EOL                             
{ 
  rule_modifier.add_rule_context($1);
}
| CENTER_MARKER EOL                       
{ 
  RuleString * universal_context_1 = 
    make_pair(grammar_displayer.get_any_symbol());
  universal_context_1 = adjoin(universal_context_1,
			       grammar_displayer.get_star_operator());
  RuleString * universal_context_2 = 
    make_pair(grammar_displayer.get_any_symbol());
  universal_context_2 = 
    adjoin(universal_context_2,
	   grammar_displayer.get_star_operator());
  RuleString * universal = 
    make_context(universal_context_1,universal_context_2);
  rule_modifier.add_rule_context(universal);
}
;

CONTEXT: REGULAR_EXPRESSION CENTER_MARKER REGULAR_EXPRESSION          
{ 
  $$ = make_context($1,$3);
}
| CENTER_MARKER REGULAR_EXPRESSION                             
{ 
  RuleString * universal_context =
    make_pair(grammar_displayer.get_any_symbol());
  universal_context = adjoin(universal_context,
			     grammar_displayer.get_star_operator());

  $$ = make_context(universal_context,$2);
}
| REGULAR_EXPRESSION CENTER_MARKER                             
{ 
  RuleString * universal_context = 
    make_pair(grammar_displayer.get_any_symbol());
  universal_context = adjoin(universal_context,
			     grammar_displayer.get_star_operator());

  $$ = make_context($1,universal_context);
}
;

MATCHER: MATCHED       
{ $$ = MATCH; }
| MIXED         
{ $$ = MIX; }
| FREELY        
{ $$ = FREE; }
;

VARIABLE_DEFINITIONS: VARIABLE_BLOCK MATCHER 
{ rule_modifier.set_block_matcher($2);
  rule_modifier.new_block(); }
| VARIABLE_DEFINITIONS AND VARIABLE_BLOCK MATCHER 
{ rule_modifier.set_block_matcher($4);
  rule_modifier.new_block(); }
| VARIABLE_DEFINITIONS AND VARIABLE_BLOCK 
{ rule_modifier.set_block_matcher(FREE);
  rule_modifier.new_block(); }
| VARIABLE_BLOCK
{ rule_modifier.set_block_matcher(FREE);
  rule_modifier.new_block(); }
;

VARIABLE_BLOCK: VARIABLE_BLOCK VARIABLE_DEFINITION {}
| VARIABLE_DEFINITION {}
;

VARIABLE_DEFINITION: SYMBOL IN VARIABLE_SEQUENCE 
{ 
  try 
    {
      rule_modifier.insert_variable($1,$3);
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
| SYMBOL IN SYMBOL            
{
  NonDelimiterSymbolRange * values = new NonDelimiterSymbolRange;
  values->push_back($3);
  try
    {
      rule_modifier.insert_variable($1,values);
    }
  catch (const char * err)
    {
      yyerror(err);
    }
}
;

VARIABLE_SEQUENCE: LEFT_BRACKET NON_DELIMITER_SYMBOL_RANGE RIGHT_BRACKET 
{
  $$ = $2;
}
;

OPERATOR: LEFT_RESTRICTION_ARROW           
{ 
  rule_modifier.set_rule_operator(NOT_LEFT_OPERATOR);
}
| LEFT_ARROW                       
{ 
  rule_modifier.set_rule_operator(LEFT_OPERATOR);
}
| RIGHT_ARROW                      
{ 
  rule_modifier.set_rule_operator(RIGHT_OPERATOR);
}
| LEFT_RIGHT_ARROW                 
{ 
  rule_modifier.set_rule_operator(LEFT_RIGHT_OPERATOR);
}
;

HEADER: RULE_NAME 
{ 
  rule_modifier.set_rule_name($1);
}
;

LEFT_SIDE_OF_RULE: CENTER_PAIR 
{ 
  rule_modifier.set_center_input($1->first);
  rule_modifier.set_center_output($1->second);
  delete $1;
}
;

LEFT_SIDES_OF_RULE: RANGE_OF_PAIRS {}
;

CENTER_PAIR: CHAR PAIR_SEPARATOR CHAR  
{ 
  $$ = new Pair($1,$3);
  
}
| EPSILON PAIR_SEPARATOR CHAR 
{ 
  $$ = new Pair(grammar_displayer.get_epsilon_symbol(),$3);
}
| CHAR PAIR_SEPARATOR EPSILON 
{ 
  $$ = new Pair($1,grammar_displayer.get_epsilon_symbol());
}
| CHAR 
{ 
  $$ = new Pair($1,string_copy($1));
}
| CHAR PAIR_SEPARATOR_LEFT
{
  $$ = new Pair($1,grammar_displayer.get_any_symbol());
}
| PAIR_SEPARATOR_RIGHT CHAR
{
  $$ = new Pair(grammar_displayer.get_any_symbol(),$2);
}
| EPSILON PAIR_SEPARATOR_LEFT
{
  $$ = new Pair(grammar_displayer.get_epsilon_symbol(),
		grammar_displayer.get_any_symbol());
}
| PAIR_SEPARATOR_RIGHT EPSILON
{
  $$ = new Pair(grammar_displayer.get_any_symbol(),
		grammar_displayer.get_epsilon_symbol());
}
| PAIR_SEPARATOR_BOTH
{
  $$ = new Pair(grammar_displayer.get_any_symbol(),
		grammar_displayer.get_any_symbol());
}
| CHAR PAIR_SEPARATOR ANY 
{ 
  $$ = new Pair($1,grammar_displayer.get_any_symbol());
} 
| ANY PAIR_SEPARATOR CHAR 
{ 
  $$ = new Pair(grammar_displayer.get_any_symbol(),$3);
}
| ANY PAIR_SEPARATOR EPSILON 
{ 
  $$ = new Pair(grammar_displayer.get_any_symbol(),
		grammar_displayer.get_epsilon_symbol());
}
| EPSILON PAIR_SEPARATOR ANY 
{ 
  $$ = new Pair(grammar_displayer.get_epsilon_symbol(),
		grammar_displayer.get_any_symbol());
}
;

RANGE_OF_PAIRS: RANGE_OF_PAIRS UNION CENTER_PAIR
{ 
  rule_modifier.add_center_pair($3->first,$3->second);
}
| CENTER_PAIR
{
  rule_modifier.add_center_pair($1->first,$1->second);
}
;

REGULAR_EXPRESSION: REGULAR_EXPRESSION UNION REG 
{ 
  $$ = adjoin($1,
	      grammar_displayer.get_union_operator(),
	      $3);
}
| REGULAR_EXPRESSION INTERSECTION REG 
{ 
  $$ = adjoin($1,
	      grammar_displayer.get_intersection_operator(),
	      $3);
}
| REGULAR_EXPRESSION DIFFERENCE REG   
{ 
  $$ = adjoin($1,
	      grammar_displayer.get_difference_operator(),
	      $3);
}
| REGULAR_EXPRESSION FREELY_INSERT LEFT_SQUARE_BRACKET
  FREELY_INSERT_RANGE RIGHT_SQUARE_BRACKET
{
  $$ = adjoin($1,
	      grammar_displayer.get_freely_insert_operator(),
	      make_range($4));
}
| REG                                 
{ 
  $$ = $1;
}
;

REG: REG RE 
{ 
  $$ = adjoin($1,$2);
}
| RE        
{ 
  $$ = $1;
}
;

RE: CONTAINMENT RE    
{ 
  $$ = adjoin(grammar_displayer.get_containment_operator(),$2);
}
| CONTAINMENT_ONCE RE 
{ 
  $$ = adjoin(grammar_displayer.get_containment_once_operator(),$2);
}
| TERM_COMPLEMENT RE  
{ 
  $$ = adjoin(grammar_displayer.get_term_complement_operator(),$2);
}
| COMPLEMENT RE       
{ 
  $$ = adjoin(grammar_displayer.get_complement_operator(),$2);
}
| RE STAR             
{ 
  $$ = adjoin($1,
	      grammar_displayer.get_star_operator());
}
| RE PLUS             
{ 
  $$ = adjoin($1,
	      grammar_displayer.get_plus_operator());
}
| RE POWER NUMBER     
{ 
  int exp = atoi($3);
  free($3);
  $$ = make_power($1,
	      	  exp); 
}
| RANGE               
{ 
  $$ = $1; 
}
| PARENTHESIS         
{ 
  $$ = $1; 
}
| PAIR                
{ 
  $$ = $1; 
}
;

RANGE: LEFT_SQUARE_BRACKET REGULAR_EXPRESSION RIGHT_SQUARE_BRACKET 
{ 
  $$ = make_range($2);
}
| LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET                         
{ 
  $$ = make_empty_range();
}
;

PARENTHESIS: LEFT_BRACKET REGULAR_EXPRESSION RIGHT_BRACKET 
{ 
  $$ = make_parenthesis($2);
}
| LEFT_BRACKET RIGHT_BRACKET                               
{ 
  $$ = make_empty_range();
}
;

NON_DELIMITER_RANGE: NON_DELIMITER_RANGE SIMPLE_PAIR 
{ 
  $1->push_back($2);
  $$ = $1;
}
| SIMPLE_PAIR                     
{ 
  $$ = new NonDelimiterRange; 
  $$->push_back($1);
}
;

FREELY_INSERT_RANGE: FREELY_INSERT_RANGE DIACRITIC_PAIR
{
  $$ = adjoin($1,$2);
}
| DIACRITIC_PAIR
{
  $$ = $1;
};

DIACRITIC_PAIR: SYMBOL PAIR_SEPARATOR EPSILON
{
  $$ = make_pair($1,
		 grammar_displayer.get_epsilon_symbol());
}

NON_DELIMITER_SYMBOL_RANGE: NON_DELIMITER_SYMBOL_RANGE SYMBOL  
{ 
  $$ = $1;
  $$->push_back($2);
}
| NON_DELIMITER_SYMBOL_RANGE EPSILON 
{ 
  $$ = $1;
  $$->push_back(grammar_displayer.get_epsilon_symbol());
}
| NON_DELIMITER_SYMBOL_RANGE LONELY_EPSILON 
{ 
  $$ = $1;
  $$->push_back(grammar_displayer.get_epsilon_symbol());
}
| CHAR                      
{ 
  $$ = new NonDelimiterSymbolRange;
  $$->push_back($1);
}
| EPSILON                            
{
  $$ = new NonDelimiterSymbolRange;
  $$->push_back(grammar_displayer.get_epsilon_symbol());
}
| LONELY_EPSILON                            
{
  $$ = new NonDelimiterSymbolRange;
  $$->push_back(grammar_displayer.get_epsilon_symbol());
}
;

SIMPLE_PAIR: CHAR PAIR_SEPARATOR CHAR
{ 
  $$ = new Pair($1,$3); 
}
|
CHAR PAIR_SEPARATOR EPSILON
{ 
  $$ = new Pair($1,grammar_displayer.get_epsilon_symbol()); }
|
EPSILON PAIR_SEPARATOR CHAR
{ 
$$ = new Pair(grammar_displayer.get_epsilon_symbol(),$3); }
|
CHAR
{
  $$ = new Pair($1,string_copy($1));
}
;

PAIR: CHAR PAIR_SEPARATOR CHAR 
{ 
  if (rule_modifier.is_definition_name($1))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the left side of a pair!",$1));
    }
  if (rule_modifier.is_definition_name($3))
    {
      yyerror(rule_modifier.generate_warning(
	      "The diacritic %s can't be the right side of a pair!",$3));
    }
  if (rule_modifier.is_diacritic($3))
    {
      yyerror(rule_modifier.generate_warning(
	      "The diacritic %s can't be the right side of a pair!",$3));
    }
  $$ = make_pair($1,$3);
}
| 
CHAR PAIR_SEPARATOR ANY        
{ 

  if (rule_modifier.is_definition_name($1))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the left side of a pair!",$1));
    }
  $$ = make_pair($1,grammar_displayer.get_any_symbol());
}
| EPSILON PAIR_SEPARATOR ANY     
{ 
  $$ = make_pair(grammar_displayer.get_epsilon_symbol(),
		 grammar_displayer.get_any_symbol());
}
| CHAR PAIR_SEPARATOR_LEFT       
{ 
  if (rule_modifier.is_definition_name($1))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the left side of a pair!",$1));
    }
  $$ = make_pair($1,grammar_displayer.get_any_symbol());
}
| PAIR_SEPARATOR_RIGHT CHAR      
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol(),$2);
}
| ANY_LEFT_SIDE_OF_PAIR CHAR     
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol(),$2);
}
| ANY_LEFT_SIDE_OF_PAIR EPSILON  
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol(),
		 grammar_displayer.get_epsilon_symbol());
}
| LONELY_EPSILON                 
{ 
  $$ = make_pair(grammar_displayer.get_epsilon_symbol());
}
| LONELY_ANY
{
  $$ = make_pair(grammar_displayer.get_any_symbol());
}
| ANY_LEFT_SIDE_OF_PAIR ANY      
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol());
}
| CHAR PAIR_SEPARATOR EPSILON    
{ 
  if (rule_modifier.is_definition_name($1))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the left side of a pair!",$1));
    }
  $$ = make_pair($1,grammar_displayer.get_epsilon_symbol());
}
| EPSILON PAIR_SEPARATOR EPSILON 
{ 
  $$ = make_pair(grammar_displayer.get_epsilon_symbol());
}
| EPSILON PAIR_SEPARATOR CHAR    
{ 
  if (rule_modifier.is_definition_name($3))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the right side of a pair!",$3));
    }
  else if (rule_modifier.is_diacritic($3))
    {
      yyerror(rule_modifier.generate_warning(
	      "The diacritic %s can't be the right side of a pair!",$3));
    }
  $$ = make_pair(grammar_displayer.get_epsilon_symbol(),$3);
}
| EPSILON PAIR_SEPARATOR_LEFT    
{ 
  $$ = make_pair(grammar_displayer.get_epsilon_symbol(),
		 grammar_displayer.get_any_symbol());
}
| PAIR_SEPARATOR_RIGHT EPSILON   
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol(),
		 grammar_displayer.get_epsilon_symbol());
}
| PAIR_SEPARATOR EPSILON         
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol(),
		 grammar_displayer.get_epsilon_symbol());
}
| PAIR_SEPARATOR CHAR            
{ 
  if (rule_modifier.is_definition_name($2))
    {
      yyerror(rule_modifier.generate_warning(
	      "The definition %s can't be the right side of a pair!",$2));
    }
  else if (rule_modifier.is_diacritic($2))
    {
      yyerror(rule_modifier.generate_warning(
	      "The diacritic %s can't be the right side of a pair!",$2));
    }
  $$ = make_pair(grammar_displayer.get_any_symbol(),$2);
}
| PAIR_SEPARATOR_BOTH            
{ 
  $$ = make_pair(grammar_displayer.get_any_symbol());
}
| CHAR
{
  if (rule_modifier.is_definition_name($1))
    {
      $$ = make_definition_pair($1);
    }
  else if (rule_modifier.is_diacritic($1))
    {
      $$ = make_pair($1,
		     grammar_displayer.get_epsilon_symbol());
    }
  else
    {
      $$ = make_pair($1,string_copy($1));
    }
}
;

ANY_LEFT_SIDE_OF_PAIR: ANY PAIR_SEPARATOR {};

CHAR: SYMBOL   
{ 
  $$ = $1;
}
| NUMBER
{
  $$ = $1;
}
;




%%

void warn(const char * warning) { }

void yyerror(const char * text) {
  input_reader.error(text);
  print_kill_symbol();
  exit(1);
}

int main(int argc, char * argv[])
{
  //yydebug = 1;
  CommandLineParser command_line_parser(argc,argv,true,true);
  if (command_line_parser.help or
      command_line_parser.usage or
      command_line_parser.version)
    {
      exit(0);
    }
  if (command_line_parser.verbose)
    {
      std::cerr << "Compiling variable rules into ordinary"
		<< " rules and compiling definitions." << std::endl;
    }
  if (command_line_parser.input_file_defined)
    {
      std::ifstream input_stream(command_line_parser.input_file_name,
				 std::ios::in);
      input_reader.set_input(&input_stream);
      return yyparse();
    }
  else
    {
      input_reader.set_input(&std::cin);
      return yyparse();
    }
}


