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

#include "xre_utils.h"

extern void xreerror(const char * text);
extern int xrelex();
extern int yylex();

%}

%name-prefix="xre"
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

%type <transducer> XRE REGEXP1 REGEXP2  REGEXP4 REGEXP5 REGEXP6 REGEXP7
                    REGEXP8 REGEXP9 REGEXP10 REGEXP11 REGEXP12 LABEL
                   REPLACE REGEXP3
%type <replaceRuleVectorWithArrow> PARALLEL_RULES
%type <replaceRuleWithArrow>  RULE
%type <mappingVectorWithArrow> MAPPINGPAIR_VECTOR
%type <mappingWithArrow> MAPPINGPAIR

%type <contextWithMark> CONTEXTS_WITH_MARK
%type <transducerPairVector> CONTEXTS_VECTOR, RESTR_CONTEXTS_VECTOR
%type <transducerPair> CONTEXT, RESTR_CONTEXT
%type <replType>  CONTEXT_MARK
%type <label>     HALFARC

%nonassoc <weight> WEIGHT END_OF_WEIGHTED_EXPRESSION
%nonassoc <label> QUOTED_LITERAL SYMBOL CURLY_BRACKETS

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

%nonassoc <label> READ_BIN READ_TEXT READ_SPACED READ_PROLOG READ_RE
%token LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS
       LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED
       PAIR_SEPARATOR_WO_RIGHT PAIR_SEPARATOR_WO_LEFT
%token EPSILON_TOKEN ANY_TOKEN BOUNDARY_MARKER
%token LEXER_ERROR
%token END_OF_EXPRESSION
%token PAIR_SEPARATOR PAIR_SEPARATOR_SOLE 
       PAIR_SEPARATOR_WO_RIGHT PAIR_SEPARATOR_WO_LEFT
%token EPSILON_TOKEN ANY_TOKEN BOUNDARY_MARKER
%token LEXER_ERROR
%%


XRE: REGEXP1 { }
     ;
REGEXP1: REGEXP2 END_OF_EXPRESSION {
      // std::cerr << "regexp1:regexp2 end of expr \n"<< std::endl; 
       hfst::xre::last_compiled = & $1->minimize();
       $$ = hfst::xre::last_compiled;
   }
   | REGEXP2 END_OF_WEIGHTED_EXPRESSION {
        //std::cerr << "regexp1:regexp2 end of wighted expr \n"<< std::endl; 
        hfst::xre::last_compiled = & $1->minimize().set_final_weights($2);
        $$ = hfst::xre::last_compiled;
   }
   | REGEXP2 {
   
    //    std::cerr << "regexp1:regexp2\n"<< *$1 << std::endl; 
        hfst::xre::last_compiled = & $1->minimize();
        $$ = hfst::xre::last_compiled;
   }
   ;

REGEXP2: REPLACE
         { 
          //  std::cerr << "regexp2:replace \n"<< std::endl; 
         }
       | REGEXP2 COMPOSITION REPLACE {
       
            $$ = & $1->compose(*$3);
            delete $3;
        }
       | REGEXP2 CROSS_PRODUCT REPLACE {
            $$ = & $1->cross_product(*$3);
            // xreerror("No crossproduct");
            // $$ = $1;
            delete $3;
        }
       | REGEXP2 LENIENT_COMPOSITION REPLACE {
            //xreerror("No lenient composition");
            $$ = & $1->lenient_composition(*$3);
            delete $3;
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
               case E_REPLACE_RIGHT_MARKUP:
               default:
                xreerror("Unhandled arrow stuff I suppose");
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
      
// Mappings: ( ie. a -> b , c -> d , ... , g -> d)
MAPPINGPAIR_VECTOR: MAPPINGPAIR_VECTOR COMMA MAPPINGPAIR
      {
        // std::cerr << "mapping_vector : mapping_vector comma mapping"<< std::endl;      
         // check if new Arrow is the same as the first one

         if ($1->first != $3->first)
         {
            xreerror("Replace arrows should be the same. Calculated as if all replacements had the fist arrow.");
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
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::xre::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
          
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $3, $5;
      }
      | REPLACE REPLACE_ARROW REPLACE MARKUP_MARKER
      {
   
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
          HfstTransducerPair marks(*$3, epsilon);
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::xre::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
                   
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $3;
      }
      | REPLACE REPLACE_ARROW MARKUP_MARKER REPLACE
      {
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
          HfstTransducerPair marks(epsilon, *$4);
          HfstTransducerPair tmpMappingPair(*$1, HfstTransducer(hfst::xre::format));
          HfstTransducerPair mappingPair = create_mapping_for_mark_up_replace( tmpMappingPair, marks );
          
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
         delete $1, $4;
      }
       | LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED REPLACE_ARROW REPLACE
      {
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
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
      
       | REPLACE REPLACE_ARROW LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED
      {
          HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
          HfstTransducerPair mappingPair(*$1, epsilon);
          
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
          delete $1;
      }
      | REPLACE REPLACE_ARROW LEFT_BRACKET_DOTTED REPLACE RIGHT_BRACKET_DOTTED
      {
          HfstTransducerPair mappingPair(*$1, *$4);
          $$ =  new std::pair< ReplaceArrow, HfstTransducerPair> ($2, mappingPair);
          delete $1, $4;
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
            
            HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
            
           // std::cerr << "Epsilon: \n" << epsilon  << std::endl;
            $$ = new HfstTransducerPair(*$1, epsilon);
            delete $1; 
         }
      | CENTER_MARKER REPLACE
         {
            HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
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
         |  REPLACE_LEFT
         {
        	 $$ =  E_REPLACE_LEFT;
         }
         |   OPTIONAL_REPLACE_LEFT
         {
        	 $$ = E_OPTIONAL_REPLACE_LEFT;
         }
         ;

////////////////
REGEXP3: REGEXP4 { }
       | REGEXP3 SHUFFLE REGEXP4 {
            xreerror("No shuffle");
            $$ = $1;
            delete $3;
        }
       | REGEXP3 BEFORE REGEXP4 {
            $$ = new HfstTransducer( before (*$1, *$3) );
            delete $1, $3;
        }
       | REGEXP3 AFTER REGEXP4 {
            $$ = new HfstTransducer( after (*$1, *$3) );
            delete $1, $3;
        }

       ;
  
REGEXP4: REGEXP5 { }
        // restriction rule
       | REGEXP4 RIGHT_ARROW RESTR_CONTEXTS_VECTOR {
            //xreerror("No Arrows");
            $$ = new HfstTransducer( restriction(*$1, *$3) ) ;
            delete $1;
            delete $3;
        }
       // doesn't exist in xfst
       | REGEXP4 LEFT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
            xreerror("No Arrows");
            $$ = $1;
            delete $3;
            delete $5;
        }
       // doesn't exist in xfst
       | REGEXP4 LEFT_RIGHT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
            xreerror("No Arrows");
            $$ = $1;
            delete $3;
            delete $5;
        }
       ;

RESTR_CONTEXTS_VECTOR: RESTR_CONTEXT
         {
            HfstTransducerPairVector * ContextVector = new HfstTransducerPairVector();
            ContextVector->push_back(*$1);
            $$ = ContextVector;
            delete $1; 
         }

      | RESTR_CONTEXTS_VECTOR COMMA RESTR_CONTEXT
         {
            $1->push_back(*$3);
            $$ = $1;
            delete $3; 
         }
      
      ;
      
RESTR_CONTEXT: REGEXP4 CENTER_MARKER REGEXP4 
         {
            $$ = new HfstTransducerPair(*$1, *$3);
            delete $1, $3; 
         }
      | REGEXP4 CENTER_MARKER
         {
           // std::cerr << "Mapping: \n" << *$1  << std::endl;
            
            HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
            
           // std::cerr << "Epsilon: \n" << epsilon  << std::endl;
            $$ = new HfstTransducerPair(*$1, epsilon);
            delete $1; 
         }
      | CENTER_MARKER REGEXP4
         {
            HfstTransducer epsilon(hfst::internal_epsilon, hfst::xre::format);
            $$ = new HfstTransducerPair(epsilon, *$2);
            delete $2; 
         }
      | CENTER_MARKER
         {
            HfstTransducer empty(hfst::xre::format);
            $$ = new HfstTransducerPair(empty, empty);
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
            xreerror("No upper minus");
            $$ = $1;
            delete $3;
        }
       | REGEXP5 LOWER_MINUS REGEXP6 {
            xreerror("No lower minus");
            $$ = $1;
            delete $3;
        }
       | REGEXP5 UPPER_PRIORITY_UNION REGEXP6 {
            $$ = & $1->priority_union(*$3);
            delete $3;
        }
       | REGEXP5 LOWER_PRIORITY_UNION REGEXP6 {
            HfstTransducer* left = new HfstTransducer(*$1);
            HfstTransducer* right =  new HfstTransducer(*$3);
            right->invert();
            left->invert();

            $$ = & (left->priority_union(*right).invert());
         //   xreerror("No lower priority union");
         //   $$ = $1;
            delete $1, $3;
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
           // xreerror("No ignoring");
           // $$ = $1;
            $$ = & $1->insert_freely(*$3);
            delete $3;
        }
       | REGEXP7 IGNORE_INTERNALLY REGEXP8 {
            xreerror("No ignoring internally");
            $$ = $1;
            delete $3;
        }
       | REGEXP7 LEFT_QUOTIENT REGEXP8 {
            xreerror("No left quotient");
            $$ = $1;
            delete $3;
        }
       ;

REGEXP8: REGEXP9 { }
       | COMPLEMENT REGEXP8 {
       		// TODO: forbid pair complement (ie ~a:b)
       		HfstTransducer complement = HfstTransducer::identity_pair( hfst::xre::format );
       		complement.repeat_star().minimize();
       		complement.subtract(*$2);
       		$$ = new HfstTransducer(complement);
   			delete $2;
        }
       | CONTAINMENT REGEXP8 {
       
            HfstTransducer* left = new HfstTransducer(hfst::internal_identity,
                                    hfst::internal_identity,
                                    hfst::xre::format);
            HfstTransducer* right = new HfstTransducer(hfst::internal_identity,
                                    hfst::internal_identity,
                                    hfst::xre::format);
            right->repeat_star();
            left->repeat_star();

            $$ = & ((right->concatenate(*$2).concatenate(*left)));
            
       /*
            HfstTransducer* left = new HfstTransducer(hfst::internal_unknown,
                                    hfst::internal_unknown,
                                    hfst::xre::format);
            HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                    hfst::internal_unknown,
                                    hfst::xre::format);
            right->repeat_star();
            left->repeat_star();
            HfstTransducer* contain_once = 
                & ((right->concatenate(*$2).concatenate(*left)));
            $$ = & (contain_once->repeat_star());
            
         */
            delete $2;
            delete left;
        }
       | CONTAINMENT_ONCE REGEXP8 {
                                 
            HfstTransducer* left = new HfstTransducer(hfst::internal_unknown,
                                    hfst::internal_unknown,
                                    hfst::xre::format);
            HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                    hfst::internal_unknown,
                                    hfst::xre::format);
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
                                    hfst::xre::format);
            HfstTransducer* right = new HfstTransducer(hfst::internal_unknown,
                                    hfst::internal_unknown,
                                    hfst::xre::format);
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
            $$ = & $1->repeat_n_plus($2);
        }
       | REGEXP9 CATENATE_N_MINUS {
            $$ = & $1->repeat_n_minus($2);
        }
       | REGEXP9 CATENATE_N_TO_K {
            $$ = & $1->repeat_n_to_k($2[0], $2[1]);
            free($2);
        }
       ;

REGEXP10: REGEXP11 { }
       | TERM_COMPLEMENT REGEXP10 {
            HfstTransducer* any = new HfstTransducer(hfst::internal_identity,
                                        hfst::internal_identity,
                                        hfst::xre::format);
            $$ = & ( any->subtract(*$2));
            delete $2;
        }
       | SUBSTITUTE_LEFT REGEXP10 COMMA REGEXP10 COMMA REGEXP10 RIGHT_BRACKET {
            xreerror("no substitute");
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
            xreerror("no read text");
        }
        | READ_SPACED {
            xreerror("no read spaced");
        }
        | READ_PROLOG {
            xreerror("no read prolog");
        }
        | READ_RE {
            xreerror("Definitely no read regex");
        }
        ;

LABEL: HALFARC {
        if (strcmp($1, hfst::internal_unknown.c_str()) == 0)
          {
            $$ = new HfstTransducer(hfst::internal_identity,
                                    hfst::internal_identity, hfst::xre::format);
          }
        else
          {
            $$ = new HfstTransducer($1, $1, hfst::xre::format);
          }
        free($1);
     }
     |
     HALFARC PAIR_SEPARATOR HALFARC {
        $$ = new HfstTransducer($1, $3, hfst::xre::format);
        free($1);
        free($3);
     }
     | HALFARC PAIR_SEPARATOR_WO_RIGHT {
        $$ = new HfstTransducer($1, hfst::internal_unknown, hfst::xre::format);
        free($1);
     }
     | PAIR_SEPARATOR_WO_LEFT HALFARC {
        $$ = new HfstTransducer(hfst::internal_unknown, $2, hfst::xre::format);
        free($2);
     }
     | PAIR_SEPARATOR_SOLE {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | CURLY_BRACKETS {
        HfstTokenizer TOK;
        $$ = new HfstTransducer($1, TOK, hfst::xre::format);
        free($1);
     }
     ;

HALFARC: SYMBOL
     | EPSILON_TOKEN {
        $$ = strdup(hfst::internal_epsilon.c_str());
     }
     | ANY_TOKEN {
        $$ = strdup(hfst::internal_unknown.c_str());
     }
     | QUOTED_LITERAL 
     | BOUNDARY_MARKER {
        $$ = strdup("@#@");
     }
     ;

%%

