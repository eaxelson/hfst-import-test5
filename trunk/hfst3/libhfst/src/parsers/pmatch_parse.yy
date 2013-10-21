%{

#define YYDEBUG 1 

#include <stdio.h>
#include <assert.h>
#include <iostream>

#include "HfstTransducer.h"
#include "HfstInputStream.h"
#include "HfstXeroxRules.h"

    using namespace hfst;
    using hfst::HfstTransducer;
    using namespace hfst::xeroxRules;
    using namespace hfst::implementations;


#include "pmatch_utils.h"

    extern void pmatcherror(const char * text);
    extern int pmatchlex();




    %}

%name-prefix="pmatch"
     %error-verbose
     %debug

           



     %union {




         int value;
         int* values;
         double weight;
         char* label;
         hfst::HfstTransducer* transducer;
         hfst::HfstTransducerPair* transducerPair;
         hfst::HfstTransducerPairVector* transducerPairVector;
         std::pair<std::string, hfst::HfstTransducer*>* transducerDefinition;
         std::map<std::string, hfst::HfstTransducer*>* transducerDefinitions;
    
         std::pair<hfst::xeroxRules::ReplaceArrow, std::vector<hfst::xeroxRules::Rule> >* replaceRuleVectorWithArrow;
         std::pair< hfst::xeroxRules::ReplaceArrow, hfst::xeroxRules::Rule>* replaceRuleWithArrow;   
         std::pair< hfst::xeroxRules::ReplaceArrow, hfst::HfstTransducerPairVector>* mappingVectorWithArrow;
         std::pair< hfst::xeroxRules::ReplaceArrow, hfst::HfstTransducerPair>* mappingWithArrow;
    
   
         std::pair<hfst::xeroxRules::ReplaceType, hfst::HfstTransducerPairVector>* contextWithMark;
   
         hfst::xeroxRules::ReplaceType replType;
         hfst::xeroxRules::ReplaceArrow replaceArrow; 
    
     
     }

/* some parts have been ripped from foma’s parser now */

%type <replaceArrow> REPLACE_ARROW

%type <transducerDefinitions> PMATCH
%type <transducerDefinition> DEFINITION
%type <transducer> REGEXP1 REGEXP2 REGEXP4 REGEXP5 REGEXP6 REGEXP7
REGEXP8 REGEXP9 REGEXP10 REGEXP11 REGEXP12 LABEL
REPLACE REGEXP3
%type <replaceRuleVectorWithArrow> PARALLEL_RULES
%type <replaceRuleWithArrow>  RULE
%type <mappingVectorWithArrow> MAPPINGPAIR_VECTOR
%type <mappingWithArrow> MAPPINGPAIR

%type <contextWithMark> CONTEXTS_WITH_MARK
%type <transducerPairVector> CONTEXTS_VECTOR
%type <transducerPair> CONTEXT
%type <replType>  CONTEXT_MARK

%type <transducer> OPTCAP TOLOWER TOUPPER INSERT RIGHT_CONTEXT LEFT_CONTEXT
NEGATIVE_RIGHT_CONTEXT NEGATIVE_LEFT_CONTEXT

%nonassoc <weight> WEIGHT END_OF_WEIGHTED_EXPRESSION
%nonassoc <label> QUOTED_LITERAL SYMBOL

%left  CROSS_PRODUCT COMPOSITION LENIENT_COMPOSITION INTERSECTION
%left  CENTER_MARKER MARKUP_MARKER
%left  SHUFFLE
%right LEFT_RESTRICTION LEFT_ARROW RIGHT_ARROW LEFT_RIGHT_ARROW
%left  REPLACE_RIGHT REPLACE_LEFT
OPTIONAL_REPLACE_RIGHT OPTIONAL_REPLACE_LEFT
REPLACE_LEFT_RIGHT OPTIONAL_REPLACE_LEFT_RIGHT
RTL_LONGEST_MATCH RTL_SHORTEST_MATCH
LTR_LONGEST_MATCH LTR_SHORTEST_MATCH 
      
%right REPLACE_CONTEXT_UU REPLACE_CONTEXT_LU 
REPLACE_CONTEXT_UL REPLACE_CONTEXT_LL

%left  UNION MINUS UPPER_MINUS LOWER_MINUS UPPER_PRIORITY_UNION
LOWER_PRIORITY_UNION

%left  IGNORING IGNORE_INTERNALLY LEFT_QUOTIENT


%left  COMMACOMMA

 
          
%left  COMMA 
       
%left  BEFORE AFTER


%nonassoc SUBSTITUTE_LEFT TERM_COMPLEMENT
%nonassoc COMPLEMENT CONTAINMENT CONTAINMENT_ONCE CONTAINMENT_OPT
%nonassoc REVERSE INVERT UPPER LOWER STAR PLUS
%nonassoc <values> CATENATE_N_TO_K
%nonassoc <value> CATENATE_N CATENATE_N_PLUS CATENATE_N_MINUS

%nonassoc <label> READ_BIN READ_TEXT READ_SPACED READ_PROLOG READ_RE READ_LEXC
%token LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS
LEFT_CURLY RIGHT_CURLY LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED
PAIR_SEPARATOR_WO_RIGHT PAIR_SEPARATOR_WO_LEFT
%token EPSILON_TOKEN ANY_TOKEN BOUNDARY_MARKER
%token LEXER_ERROR
%token END_OF_EXPRESSION
%token PAIR_SEPARATOR PAIR_SEPARATOR_SOLE 
PAIR_SEPARATOR_WO_RIGHT PAIR_SEPARATOR_WO_LEFT
%token EPSILON_TOKEN ANY_TOKEN BOUNDARY_MARKER
%token LEXER_ERROR

%nonassoc DEFINE ALPHA LOWERALPHA UPPERALPHA NUM PUNCT WHITESPACE OPTCAP_LEFT
TOLOWER_LEFT TOUPPER_LEFT INS_LEFT ENDTAG_LEFT LC_LEFT RC_LEFT NLC_LEFT NRC_LEFT
%%




PMATCH: DEFINITION {
    hfst::pmatch::definitions.insert(*$1);
//    $$ = &hfst::pmatch::definitions;
    delete $1;
 } |
 PMATCH DEFINITION {
     hfst::pmatch::definitions.insert(*$2);
//     $$ = &hfst::pmatch::definitions;
     delete $2;
 }
;

DEFINITION: DEFINE SYMBOL REGEXP1 {
    $3->set_name($2);
    $3->minimize();
    $$ = new std::pair<std::string, hfst::HfstTransducer*>($2, $3);
    if (hfst::pmatch::verbose) {
        std::cerr << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        double duration = (clock() - hfst::pmatch::timer) /
            (double) CLOCKS_PER_SEC;
        hfst::pmatch::timer = clock();
        std::cerr << "compiled " << $2 << " in " << duration << " seconds\n";
        hfst::pmatch::print_size_info($3);
        std::cerr << std::endl;
    }
 }
;

REGEXP1: REGEXP2 END_OF_EXPRESSION { }
| REGEXP2 END_OF_WEIGHTED_EXPRESSION {
    $1->set_final_weights($2);
    $$ = $1;
 }
//| REGEXP2 {
   
//        std::cerr << "regexp1:regexp2\n"<< std::endl; 
//    hfst::pmatch::last_compiled = hfst::pmatch::add_pmatch_delimiters($1);
//    $$ = hfst::pmatch::last_compiled;
//  }
;

REGEXP2: REPLACE
{ 
//          std::cerr << "regexp2:replace \n"<< std::endl; 
}
| REGEXP2 ENDTAG_LEFT SYMBOL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = hfst::pmatch::add_pmatch_delimiters($1);
 }
| REGEXP2 ENDTAG_LEFT QUOTED_LITERAL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = hfst::pmatch::add_pmatch_delimiters($1);
 }
| REGEXP2 COMPOSITION REPLACE {
       
    $$ = & $1->compose(*$3);
    delete $3;
 }
| REGEXP2 CROSS_PRODUCT REPLACE {
    $$ = & $1->cross_product(*$3);
    // pmatcherror("No crossproduct");
    // $$ = $1;
    delete $3;
 }
| REGEXP2 LENIENT_COMPOSITION REPLACE {
    pmatcherror("No lenient composition");
    $$ = $1;
    delete $3;
 }
| REGEXP2 RIGHT_CONTEXT {
    $$ = & $1->concatenate(*$2);
    delete $2;
 }
| REGEXP2 LEFT_CONTEXT {
    $$ = & $2->concatenate(*$1);
    delete $1;
 }
| REGEXP2 NEGATIVE_RIGHT_CONTEXT {
    $$ = & $1->concatenate(*$2);
    delete $2;
 }
| REGEXP2 NEGATIVE_LEFT_CONTEXT {
    $$ = & $2->concatenate(*$1);
    delete $1;
 }
// Bodyless contexts

| LEFT_CONTEXT ENDTAG_LEFT SYMBOL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| LEFT_CONTEXT ENDTAG_LEFT QUOTED_LITERAL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| RIGHT_CONTEXT ENDTAG_LEFT SYMBOL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| RIGHT_CONTEXT ENDTAG_LEFT QUOTED_LITERAL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| NEGATIVE_LEFT_CONTEXT ENDTAG_LEFT SYMBOL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| NEGATIVE_LEFT_CONTEXT ENDTAG_LEFT QUOTED_LITERAL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| NEGATIVE_RIGHT_CONTEXT ENDTAG_LEFT SYMBOL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
| NEGATIVE_RIGHT_CONTEXT ENDTAG_LEFT QUOTED_LITERAL RIGHT_PARENTHESIS {
    hfst::pmatch::add_end_tag($1, $3);
    $$ = $1;
}
;

////////////////////////////
// Replace operators
///////////////////////////

REPLACE : REGEXP3 {}
|  PARALLEL_RULES
{
    // std::cerr << "replace:parallel_rules"<< std::endl;        
    switch ( $1->first )
    {
    case E_REPLACE_RIGHT:
        $$ = new HfstTransducer( replace( $1->second, false ) );
        break;
    case E_OPTIONAL_REPLACE_RIGHT:
        $$ = new HfstTransducer( replace( $1->second, true ) );
        break;
    case E_REPLACE_LEFT:
        $$ = new HfstTransducer( replace_left( $1->second, false ) );
        break;
    case E_OPTIONAL_REPLACE_LEFT:
        $$ = new HfstTransducer( replace_left( $1->second, true ) );
        break;
    case E_RTL_LONGEST_MATCH:
        $$ = new HfstTransducer( replace_rightmost_longest_match( $1->second ) );
        break;
    case E_RTL_SHORTEST_MATCH:
        $$ = new HfstTransducer( replace_rightmost_shortest_match($1->second) );
        break;
    case E_LTR_LONGEST_MATCH:
        $$ = new HfstTransducer( replace_leftmost_longest_match( $1->second ) );
        break;
    case E_LTR_SHORTEST_MATCH:
        $$ = new HfstTransducer( replace_leftmost_shortest_match( $1->second ) );
        break;
    }
       
    delete $1;
}
;

PARALLEL_RULES: RULE
{
    //   std::cerr << "parallel_rules:rule"<< std::endl;        
    std::vector<Rule> * ruleVector = new std::vector<Rule>();
    ruleVector->push_back($1->second);
            
    $$ =  new std::pair< ReplaceArrow, std::vector<Rule> > ($1->first, *ruleVector);
    delete $1;
}
| PARALLEL_RULES COMMACOMMA RULE
{
    // std::cerr << "parallel_rules: parallel_rules ,, rule"<< std::endl;      
    Rule tmpRule($3->second);
    $1->second.push_back(tmpRule);
    $$ =  new std::pair< ReplaceArrow, std::vector<Rule> > ($3->first, $1->second);
    delete $3;
}
;

RULE: MAPPINGPAIR_VECTOR
  {
     // std::cerr << "rule: mapping_vector"<< std::endl;      
    // HfstTransducer allMappingsDisjuncted = disjunctVectorMembers($1->second);
     
     Rule rule( $1->second );;
     $$ =  new std::pair< ReplaceArrow, Rule> ($1->first, rule);
     delete $1;
  }
  | MAPPINGPAIR_VECTOR CONTEXTS_WITH_MARK
  {
    // std::cerr << "rule: mapping_vector contextsWM"<< std::endl;      
 //   HfstTransducer allMappingsDisjuncted = disjunctVectorMembers($1->second);
    
    Rule rule( $1->second, $2->second, $2->first );
    $$ =  new std::pair< ReplaceArrow, Rule> ($1->first, rule);
    delete $1, $2;
  }
  ;
;
      
// Mappings: ( ie. a -> b , c -> d , ... , g -> d)
MAPPINGPAIR_VECTOR: MAPPINGPAIR_VECTOR COMMA MAPPINGPAIR
{
    // std::cerr << "mapping_vector : mapping_vector comma mapping"<< std::endl;      
    // check if new Arrow is the same as the first one

    if ($1->first != $3->first)
    {
        pmatcherror("Replace arrows should be the same. Calculated as if all replacements had the fist arrow.");
        //exit(1);
    }

	$1->second.push_back($3->second);
    $$ =  new std::pair< ReplaceArrow, HfstTransducerPairVector> ($1->first, $1->second);
    delete $3;
            
}
      
| MAPPINGPAIR
{
    // std::cerr << "mapping_vector : mapping"<< std::endl;      
	 HfstTransducerPairVector * mappingPairVector = new HfstTransducerPairVector();
	 mappingPairVector->push_back( $1->second );
	 $$ =  new std::pair< ReplaceArrow, HfstTransducerPairVector> ($1->first, * mappingPairVector);
	 delete $1; 
}
     
;

    
MAPPINGPAIR: REPLACE REPLACE_ARROW REPLACE
{
        // std::cerr << "mapping : r2 arrow r2"<< std::endl;      
  
          HfstTransducerPair mappingPair(*$1, *$3);
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);

          delete $1, $3;
      }
  	| REPLACE REPLACE_ARROW REPLACE MARKUP_MARKER REPLACE
      {
      
          HfstTransducerPair marks(*$3, *$5);
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::pmatch::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
          
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $3, $5;
      }
      | REPLACE REPLACE_ARROW REPLACE MARKUP_MARKER
      {
   
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::pmatch::format);
          HfstTransducerPair marks(*$3, epsilon);
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::pmatch::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
                   
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $3;
      }
      | REPLACE REPLACE_ARROW MARKUP_MARKER REPLACE
      {
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::pmatch::format);
          HfstTransducerPair marks(epsilon, *$4);
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::pmatch::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
          
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $4;
      }     
      
| LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED REPLACE_ARROW REPLACE
  {
      HfstTransducer epsilon(hfst::internal_epsilon, hfst::pmatch::format);
      //HfstTransducer mappingTr(epsilon);
      //mappingTr.cross_product(*$4);
      HfstTransducerPair mappingPair(epsilon, *$4);
      
      $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($3, mappingPair);
      delete $4;
  }
  | LEFT_BRACKET_DOTTED REPLACE RIGHT_BRACKET_DOTTED REPLACE_ARROW REPLACE
  {
    //  HfstTransducer mappingTr(*$2);
    //  mappingTr.cross_product(*$5);
	  HfstTransducerPair mappingPair(*$2, *$5);
      $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($4, mappingPair);
      delete $2, $5;
  }
;    
  
   
      

        
// Contexts: ( ie. || k _ f , ... , f _ s )
CONTEXTS_WITH_MARK:  CONTEXT_MARK CONTEXTS_VECTOR
{
       
    //std::cerr << "context w mark: conMark conVect"<< std::endl;      
         
    $$ =  new std::pair< ReplaceType, HfstTransducerPairVector> ($1, *$2);
    //$$ = $2;
    //std::cerr << "Context Mark: \n" << $1  << std::endl;
   
}  
;
CONTEXTS_VECTOR: CONTEXT
{
    HfstTransducerPairVector * ContextVector = new HfstTransducerPairVector();
    ContextVector->push_back(*$1);
    $$ = ContextVector;
    delete $1; 
}

| CONTEXTS_VECTOR COMMA CONTEXT
{
    $1->push_back(*$3);
    $$ = $1;
    delete $3; 
}
      
;
CONTEXT: REPLACE CENTER_MARKER REPLACE 
{
    $$ = new HfstTransducerPair(*$1, *$3);
    delete $1, $3; 
}
| REPLACE CENTER_MARKER
{
    // std::cerr << "Mapping: \n" << *$1  << std::endl;
            
    HfstTransducer epsilon(hfst::internal_epsilon, hfst::pmatch::format);
            
    // std::cerr << "Epsilon: \n" << epsilon  << std::endl;
    $$ = new HfstTransducerPair(*$1, epsilon);
    delete $1; 
}
| CENTER_MARKER REPLACE
{
    HfstTransducer epsilon(hfst::internal_epsilon, hfst::pmatch::format);
    $$ = new HfstTransducerPair(epsilon, *$2);
    delete $2; 
}
;
      


CONTEXT_MARK: REPLACE_CONTEXT_UU
{
    $$ = REPL_UP;
}
| REPLACE_CONTEXT_LU 
{
    $$ = REPL_RIGHT;
}
| REPLACE_CONTEXT_UL
{
    $$ = REPL_LEFT;
}
| REPLACE_CONTEXT_LL
{
    $$ = REPL_DOWN;
}
;
     
     
     
     


REPLACE_ARROW: REPLACE_RIGHT
{
    $$ = E_REPLACE_RIGHT;
}
| OPTIONAL_REPLACE_RIGHT
{
    $$ = E_OPTIONAL_REPLACE_RIGHT;
}
| RTL_LONGEST_MATCH
{
    $$ = E_RTL_LONGEST_MATCH;
}
| RTL_SHORTEST_MATCH
{
    $$ = E_RTL_SHORTEST_MATCH;
}
| LTR_LONGEST_MATCH
{
    $$ = E_LTR_LONGEST_MATCH;
}
| LTR_SHORTEST_MATCH
{
    $$ = E_LTR_SHORTEST_MATCH;
}
;








////////////////



REGEXP3: REGEXP4 { }
| REGEXP3 SHUFFLE REGEXP4 {
    pmatcherror("No shuffle");
    $$ = $1;
    delete $3;
 }
| REGEXP3 BEFORE REGEXP4 {
    pmatcherror("No before");
    $$ = $1;
    delete $3;
 }
| REGEXP3 AFTER REGEXP4 {
    pmatcherror("No after");
    $$ = $1;
    delete $3;
 }

;

REGEXP4: REGEXP5 { }
| REGEXP4 LEFT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
    pmatcherror("No Arrows");
    $$ = $1;
    delete $3;
    delete $5;
 }
| REGEXP4 RIGHT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
    pmatcherror("No Arrows");
    $$ = $1;
    delete $3;
    delete $5;
 }
| REGEXP4 LEFT_RIGHT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
    pmatcherror("No Arrows");
    $$ = $1;
    delete $3;
    delete $5;
 }
;

REGEXP5: REGEXP6 { }
| REGEXP5 UNION REGEXP6 {
    $$ = & $1->disjunct(*$3);
    delete $3;
 }
| REGEXP5 INTERSECTION REGEXP6 {
    $$ = & $1->intersect(*$3);
    delete $3;
 }
| REGEXP5 MINUS REGEXP6 {
    $$ = & $1->subtract(*$3);
    delete $3;
 }
| REGEXP5 UPPER_MINUS REGEXP6 {
    pmatcherror("No upper minus");
    $$ = $1;
    delete $3;
 }
| REGEXP5 LOWER_MINUS REGEXP6 {
    pmatcherror("No lower minus");
    $$ = $1;
    delete $3;
 }
| REGEXP5 UPPER_PRIORITY_UNION REGEXP6 {
    pmatcherror("No upper priority union");
    $$ = $1;
    delete $3;
 }
| REGEXP5 LOWER_PRIORITY_UNION REGEXP6 {
    pmatcherror("No lower priority union");
    $$ = $1;
    delete $3;
 }
;

REGEXP6: REGEXP7 { }
| REGEXP6 REGEXP7 { 
    $$ = & $1->concatenate(*$2);
    delete $2;
 }
;

REGEXP7: REGEXP8 { }
| REGEXP7 IGNORING REGEXP8 {
    pmatcherror("No ignoring");
    $$ = $1;
    delete $3;
 }
| REGEXP7 IGNORE_INTERNALLY REGEXP8 {
    pmatcherror("No ignoring internally");
    $$ = $1;
    delete $3;
 }
| REGEXP7 LEFT_QUOTIENT REGEXP8 {
    pmatcherror("No left quotient");
    $$ = $1;
    delete $3;
 }

;

REGEXP8: REGEXP9 { }
| COMPLEMENT REGEXP8 {
    pmatcherror("No complement");
    $$ = $2;
 }
| CONTAINMENT REGEXP8 {
    HfstTransducer* left = new HfstTransducer(hfst::internal_unknown,
                                              hfst::internal_unknown,
                                              hfst::pmatch::format);
    HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                               hfst::internal_unknown,
                                               hfst::pmatch::format);
    right->repeat_star();
    left->repeat_star();
    HfstTransducer* contain_once = 
        & ((right->concatenate(*$2).concatenate(*left)));
    $$ = & (contain_once->repeat_star());
    delete $2;
    delete left;
 }
| CONTAINMENT_ONCE REGEXP8 {
    HfstTransducer* left = new HfstTransducer(hfst::internal_unknown,
                                              hfst::internal_unknown,
                                              hfst::pmatch::format);
    HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                               hfst::internal_unknown,
                                               hfst::pmatch::format);
    right->repeat_star();
    left->repeat_star();
    HfstTransducer* contain_once = 
        & ((right->concatenate(*$2).concatenate(*left)));
    $$ = contain_once;
    delete $2;
    delete left;
 }
| CONTAINMENT_OPT REGEXP8 {
    HfstTransducer* left = new HfstTransducer(hfst::internal_unknown,
                                              hfst::internal_unknown,
                                              hfst::pmatch::format);
    HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                               hfst::internal_unknown,
                                               hfst::pmatch::format);
    right->repeat_star();
    left->repeat_star();
    HfstTransducer* contain_once = 
        & ((right->concatenate(*$2).concatenate(*left)));
    $$ = & (contain_once->optionalize());
    delete $2;
    delete left;
 }
;

REGEXP9: REGEXP10 { }
| REGEXP9 STAR {
    $$ = & $1->repeat_star();
 }
| REGEXP9 PLUS {
    $$ = & $1->repeat_plus();
 }
| REGEXP9 REVERSE {
    $$ = & $1->reverse();
 }
| REGEXP9 INVERT {
    $$ = & $1->invert();
 }
| REGEXP9 UPPER {
    $$ = & $1->input_project();
 }
| REGEXP9 LOWER {
    $$ = & $1->output_project();
 }
| REGEXP9 CATENATE_N {
    $$ = & $1->repeat_n($2);
 }
| REGEXP9 CATENATE_N_PLUS {
    $$ = & $1->repeat_n_plus($2 + 1);
 }
| REGEXP9 CATENATE_N_MINUS {
    $$ = & $1->repeat_n_minus($2 - 1);
 }
| REGEXP9 CATENATE_N_TO_K {
    $$ = & $1->repeat_n_to_k($2[0], $2[1]);
    free($2);
 }
| INSERT { }
| OPTCAP { }
| TOUPPER { }
| TOLOWER { }
;

REGEXP10: REGEXP11 { }
| TERM_COMPLEMENT REGEXP10 {
    HfstTransducer* any = new HfstTransducer(hfst::internal_identity,
                                             hfst::internal_identity,
                                             hfst::pmatch::format);
    $$ = & ( any->subtract(*$2));
    delete $2;
 }
| SUBSTITUTE_LEFT REGEXP10 COMMA REGEXP10 COMMA REGEXP10 RIGHT_BRACKET {
    pmatcherror("no substitute");
    $$ = $2;
 }
;

REGEXP11: REGEXP12 { }
| LEFT_BRACKET REGEXP2 RIGHT_BRACKET {
    $$ = & $2->minimize();
 }
| LEFT_PARENTHESIS REGEXP2 RIGHT_PARENTHESIS {
    $$ = & $2->optionalize();
 }
| LEFT_CURLY SYMBOL RIGHT_CURLY {
    HfstTokenizer tok;
    $$ = new HfstTransducer($2, tok, hfst::pmatch::format);
 }
| ALPHA {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_alpha_acceptor);
 }
| LOWERALPHA {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_lowercase_acceptor);
 }
| UPPERALPHA {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_uppercase_acceptor);
 }
| NUM {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_numeral_acceptor);
 }
| PUNCT {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_punct_acceptor);
 }
| WHITESPACE {
    $$ = new HfstTransducer(*hfst::pmatch::get_utils()->latin1_whitespace_acceptor);
 }
;

OPTCAP: OPTCAP_LEFT REGEXP11 RIGHT_PARENTHESIS {
    $$ = hfst::pmatch::get_utils()->optcap(* $2);
}
;

TOLOWER: TOLOWER_LEFT REGEXP11 RIGHT_PARENTHESIS {
    $$ = hfst::pmatch::get_utils()->tolower(* $2);
}
;

TOUPPER: TOUPPER_LEFT REGEXP11 RIGHT_PARENTHESIS {
    $$ = hfst::pmatch::get_utils()->toupper(* $2);
}
;

REGEXP12: LABEL { }
| LABEL WEIGHT { 
    $$ = & $1->set_final_weights($2);
 }
| READ_BIN {
    hfst::HfstInputStream instream($1);
    $$ = new HfstTransducer(instream);
    instream.close();
    free($1);
  }
| READ_TEXT {
  $$ = hfst::pmatch::read_text($1);
  free($1);
  }
| READ_SPACED {
    pmatcherror("no read spaced");
  }
| READ_PROLOG {
    pmatcherror("no read prolog");
  }
| READ_RE {
    pmatcherror("Definitely no read regex");
  }
| READ_LEXC {
    $$ = hfst::HfstTransducer::read_lexc_ptr($1, hfst::TROPICAL_OPENFST_TYPE);
    free($1);
  }
;

LABEL: SYMBOL PAIR_SEPARATOR SYMBOL {
    $$ = new HfstTransducer($1, $3, hfst::pmatch::format);
    free($1);
    free($3);
 }
| SYMBOL PAIR_SEPARATOR EPSILON_TOKEN {
    $$ = new HfstTransducer($1, hfst::internal_epsilon, hfst::pmatch::format);
    free($1);
 }
| SYMBOL PAIR_SEPARATOR ANY_TOKEN {
    $$ = new HfstTransducer($1, hfst::internal_unknown, hfst::pmatch::format);
    free($1);
 }
| EPSILON_TOKEN PAIR_SEPARATOR EPSILON_TOKEN {
    $$ = new HfstTransducer(hfst::internal_epsilon, 
                            hfst::internal_epsilon, hfst::pmatch::format);
 }
| EPSILON_TOKEN PAIR_SEPARATOR SYMBOL {
    $$ = new HfstTransducer(hfst::internal_epsilon, $3, hfst::pmatch::format);
    free($3);
 }
| EPSILON_TOKEN PAIR_SEPARATOR ANY_TOKEN {
    $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_unknown,
                            hfst::pmatch::format);
 }
| ANY_TOKEN PAIR_SEPARATOR ANY_TOKEN {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                            hfst::pmatch::format);
 }
| ANY_TOKEN PAIR_SEPARATOR SYMBOL {
    $$ = new HfstTransducer(hfst::internal_unknown, $3, hfst::pmatch::format);
    free($3);
 }
| ANY_TOKEN PAIR_SEPARATOR EPSILON_TOKEN {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_epsilon,
                            hfst::pmatch::format);
 }
| SYMBOL PAIR_SEPARATOR_WO_RIGHT {
    $$ = new HfstTransducer($1, hfst::internal_unknown, hfst::pmatch::format);
    free($1);
 }
| EPSILON_TOKEN PAIR_SEPARATOR_WO_RIGHT {
    $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_unknown,
                            hfst::pmatch::format);
 }
| ANY_TOKEN PAIR_SEPARATOR_WO_RIGHT {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                            hfst::pmatch::format);
 }
| PAIR_SEPARATOR_WO_LEFT SYMBOL {
    $$ = new HfstTransducer(hfst::internal_unknown, $2, hfst::pmatch::format);
    free($2);
 }
| PAIR_SEPARATOR_WO_LEFT ANY_TOKEN {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                            hfst::pmatch::format);
 }
| PAIR_SEPARATOR_WO_LEFT EPSILON_TOKEN {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_epsilon,
                            hfst::pmatch::format);
 }
| SYMBOL {
    if (hfst::pmatch::definitions.count($1) != 0) {
        if (hfst::pmatch::verbose) {
            std::cerr << "including " <<
                hfst::pmatch::definitions[$1]->get_name() << " with ";
            hfst::pmatch::print_size_info(hfst::pmatch::definitions[$1]);
        }
        $$ = new HfstTransducer(* hfst::pmatch::definitions[$1]);
    } else {
        if (strlen($1) == 0) {
            $$ = new HfstTransducer(hfst::pmatch::format);
        } else {
            $$ = new HfstTransducer($1, $1, hfst::pmatch::format);
        }
    }
    free($1);
 }
| PAIR_SEPARATOR_SOLE {
    $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                            hfst::pmatch::format);
  }
| EPSILON_TOKEN {
    $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_epsilon,
                            hfst::pmatch::format);
  }
| ANY_TOKEN {
    $$ = new HfstTransducer(hfst::internal_identity,
                            hfst::pmatch::format);
  }
| QUOTED_LITERAL {
    HfstTokenizer tok;
    $$ = new HfstTransducer($1, tok, hfst::pmatch::format);
    free($1);
  }
| BOUNDARY_MARKER {
    $$ = new HfstTransducer("@BOUNDARY@", "@BOUNDARY@", hfst::pmatch::format);
  }
;

INSERT: INS_LEFT SYMBOL RIGHT_PARENTHESIS {
    if(hfst::pmatch::definitions.count($2) == 0) {
        hfst::pmatch::unsatisfied_insertions.insert($2);
    }
    char * Ins_trans = hfst::pmatch::get_Ins_transition($2);
    $$ = new HfstTransducer(
        Ins_trans, Ins_trans, hfst::pmatch::format);
    $$->set_name($2);
    free(Ins_trans);
    hfst::pmatch::inserted_transducers.insert($2);
}
;

RIGHT_CONTEXT: RC_LEFT REPLACE RIGHT_PARENTHESIS {
    HfstTransducer * rc_entry = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::RC_ENTRY_SYMBOL, hfst::pmatch::format);
    HfstTransducer * rc_exit = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::RC_EXIT_SYMBOL, hfst::pmatch::format);
    rc_entry->concatenate(*$2);
    rc_entry->concatenate(*rc_exit);
    $$ = rc_entry;
    delete $2;
    delete rc_exit;
 }
;

NEGATIVE_RIGHT_CONTEXT: NRC_LEFT REPLACE RIGHT_PARENTHESIS {
    HfstTransducer * nrc_entry = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::NRC_ENTRY_SYMBOL, hfst::pmatch::format);
    HfstTransducer * nrc_exit = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::NRC_EXIT_SYMBOL, hfst::pmatch::format);
    nrc_entry->concatenate(*$2);
    nrc_entry->concatenate(*nrc_exit);
    nrc_entry->disjunct(HfstTransducer("@PMATCH_PASSTHROUGH@",
                                       hfst::internal_epsilon, hfst::pmatch::format));
    $$ = nrc_entry;
    delete $2;
    delete nrc_exit;
 }
;

LEFT_CONTEXT: LC_LEFT REPLACE RIGHT_PARENTHESIS {
    HfstTransducer * lc_entry = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::LC_ENTRY_SYMBOL, hfst::pmatch::format);
    HfstTransducer * lc_exit = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::LC_EXIT_SYMBOL, hfst::pmatch::format);
    lc_entry->concatenate($2->reverse());
    lc_entry->concatenate(*lc_exit);
    lc_entry->substitute("@PMATCH_ENTRY@", "@PMATCH_TMP@");
    lc_entry->substitute("@PMATCH_EXIT@", "@PMATCH_ENTRY@");
    lc_entry->substitute("@PMATCH_TMP@", "@PMATCH_EXIT@");
    $$ = lc_entry;
    delete $2;
    delete lc_exit;
 }
;

NEGATIVE_LEFT_CONTEXT: NLC_LEFT REPLACE RIGHT_PARENTHESIS {
    HfstTransducer * nlc_entry = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::NLC_ENTRY_SYMBOL, hfst::pmatch::format);
    HfstTransducer * nlc_exit = new HfstTransducer(
        hfst::internal_epsilon, hfst::pmatch::NLC_EXIT_SYMBOL, hfst::pmatch::format);
    nlc_entry->concatenate($2->reverse());
    nlc_entry->concatenate(*nlc_exit);
    nlc_entry->substitute("@PMATCH_ENTRY@", "@PMATCH_TMP@");
    nlc_entry->substitute("@PMATCH_EXIT@", "@PMATCH_ENTRY@");
    nlc_entry->substitute("@PMATCH_TMP@", "@PMATCH_EXIT@");
    nlc_entry->disjunct(HfstTransducer("@PMATCH_PASSTHROUGH@",
                                       hfst::internal_epsilon, hfst::pmatch::format));
    $$ = nlc_entry;
    delete $2;
    delete nlc_exit;
 }
;

%%

