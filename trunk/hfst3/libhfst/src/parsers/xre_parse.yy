%{

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



%}

%name-prefix="xre"
%error-verbose

           


%union {
    int value;
    int* values;
    double weight;
    char* label;
    hfst::HfstTransducer* transducer;
    hfst::HfstTransducerPair* transducerPair;
    hfst::HfstTransducerPairVector* transducerPairVector;
    
    pair<hfst::xeroxRules::ReplaceType, hfst::HfstTransducerPairVector>* contextWithMark;
    
    pair<hfst::xeroxRules::ReplaceArrow, hfst::xeroxRules::Rule>* replaceRuleWithArrow;
    pair<hfst::xeroxRules::ReplaceArrow, vector<hfst::xeroxRules::Rule> >* replaceRuleVectorWithArrow;
    pair< hfst::xeroxRules::ReplaceArrow, hfst::HfstTransducer>* mappingWithArrow;
    
    
   // hfst::xeroxRules::Rule* rule;
   // vector<hfst::xeroxRules::Rule>* ruleVector;
   
    hfst::xeroxRules::ReplaceType replType;
    hfst::xeroxRules::ReplaceArrow replaceArrow; 
    
    
}

/* some parts have been ripped from foma’s parser now */

%type <transducer> XRE REGEXP1 REGEXP2 REGEXP3 REGEXP4 REGEXP5 REGEXP6 REGEXP7
                    REGEXP8 REGEXP9 REGEXP10 REGEXP11 REGEXP12 LABEL
                      REPLACE
%type <replaceRuleWithArrow>  RULE
%type <replaceRuleVectorWithArrow> PARALLEL_RULES
%type <replaceArrow>  REPLACE_ARROW
%type <mappingWithArrow>  RULE_MAPPING


%type <replType>  CONTEXT_MARK
%type <contextWithMark> CONTEXTS_WITH_MARK
%type <transducerPairVector> CONTEXTS_VECTOR
%type <transducerPair> CONTEXT
%nonassoc <weight> WEIGHT END_OF_WEIGHTED_EXPRESSION
%nonassoc <label> QUOTED_LITERAL SYMBOL

%left  CROSS_PRODUCT COMPOSITION LENIENT_COMPOSITION
%left  SHUFFLE
%right LEFT_RESTRICTION LEFT_ARROW RIGHT_ARROW LEFT_RIGHT_ARROW
%left  CENTER_MARKER MARKUP_MARKER
%right REPLACE_RIGHT REPLACE_LEFT OPTIONAL_REPLACE_RIGHT OPTIONAL_REPLACE_LEFT
       REPLACE_LEFT_RIGHT OPTIONAL_REPLACE_LEFT_RIGHT
       RTL_LONGEST_MATCH RTL_SHORTEST_MATCH
       LTR_LONGEST_MATCH LTR_SHORTEST_MATCH
       
%left  REPLACE_CONTEXT_UU REPLACE_CONTEXT_LU 
       REPLACE_CONTEXT_UL REPLACE_CONTEXT_LL
       COMMA COMMACOMMA
%left  UNION INTERSECTION MINUS UPPER_MINUS LOWER_MINUS UPPER_PRIORITY_UNION
       LOWER_PRIORITY_UNION
%left  BEFORE AFTER
%left  IGNORING IGNORE_INTERNALLY LEFT_QUOTIENT

%nonassoc SUBSTITUTE_LEFT TERM_COMPLEMENT
%nonassoc COMPLEMENT CONTAINMENT CONTAINMENT_ONCE CONTAINMENT_OPT
%nonassoc REVERSE INVERT UPPER LOWER STAR PLUS
%nonassoc <values> CATENATE_N_TO_K
%nonassoc <value> CATENATE_N CATENATE_N_PLUS CATENATE_N_MINUS

%nonassoc <label> READ_BIN READ_TEXT READ_SPACED READ_PROLOG READ_RE
%token LEFT_BRACKET RIGHT_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS
       LEFT_CURLY RIGHT_CURLY LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED
%token END_OF_EXPRESSION
%token PAIR_SEPARATOR PAIR_SEPARATOR_SOLE 
       PAIR_SEPARATOR_WO_RIGHT PAIR_SEPARATOR_WO_LEFT
%token EPSILON_TOKEN ANY_TOKEN BOUNDARY_MARKER
%token LEXER_ERROR
%%

XRE: REGEXP1 
      {
         std::cerr << "final tr: \n" << *$$ << "\n" << std::endl;
      }
     ;
REGEXP1: REGEXP2 END_OF_EXPRESSION {
       hfst::xre::last_compiled = & $1->minimize();
       $$ = hfst::xre::last_compiled;
   }
   | REGEXP2 END_OF_WEIGHTED_EXPRESSION {
        hfst::xre::last_compiled = & $1->minimize().set_final_weights($2);
        $$ = hfst::xre::last_compiled;
   }
   | REGEXP2 {
        hfst::xre::last_compiled = & $1->minimize();
        $$ = hfst::xre::last_compiled;
   }
  | REPLACE END_OF_EXPRESSION
   {
       //std::cerr << "replace tr before minimization: \n" << *$1 << "\n" << std::endl;  
       hfst::xre::last_compiled = & $1->minimize();
       $$ = hfst::xre::last_compiled;
      // std::cerr << "replace tr after minimization: \n" << *$$ << "\n" << std::endl;  
   }

   ;

REGEXP2: REGEXP3 { }
       | REGEXP2 COMPOSITION REGEXP3 {
            $$ = & $1->compose(*$3);
            delete $3;
        }
       | REGEXP2 CROSS_PRODUCT REGEXP3 {
            xreerror("No crossproduct");
            $$ = $1;
            delete $3;
        }
       | REGEXP2 LENIENT_COMPOSITION REGEXP3 {
            xreerror("No lenient composition");
            $$ = $1;
            delete $3;
        }
       ;


REGEXP3: REGEXP4 { }
       | REGEXP3 SHUFFLE REGEXP4 {
            xreerror("No shuffle");
            $$ = $1;
            delete $3;
        }
       | REGEXP3 BEFORE REGEXP4 {
            xreerror("No before");
            $$ = $1;
            delete $3;
        }
       | REGEXP3 AFTER REGEXP4 {
            xreerror("No after");
            $$ = $1;
            delete $3;
        }
       ;

REGEXP4: REGEXP5 { }
       | REGEXP4 LEFT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
            xreerror("No Arrows");
            $$ = $1;
            delete $3;
            delete $5;
        }
       | REGEXP4 RIGHT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
            xreerror("No Arrows");
            $$ = $1;
            delete $3;
            delete $5;
        }
       | REGEXP4 LEFT_RIGHT_ARROW REGEXP5 CENTER_MARKER REGEXP5 {
            xreerror("No Arrows");
            $$ = $1;
            delete $3;
            delete $5;
        }
       ;
       
       
       
       
       ////////////////////////////




REPLACE : PARALLEL_RULES
      {
      switch ( $1->first )
      {
         case E_REPLACE_RIGHT:
           $$ = new HfstTransducer( replace( $1->second, false ) );
           break;
         case E_OPTIONAL_REPLACE_RIGHT:
           $$ = new HfstTransducer( replace( $1->second, true ) );
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

       
PARALLEL_RULES: /* empty */
      | RULE
      {
         vector<Rule> * ruleVector = new vector<Rule>();
         ruleVector->push_back($1->second);
         
         $$ =  new pair< ReplaceArrow, vector<Rule> > ($1->first, *ruleVector);
         delete $1;
      }
      | PARALLEL_RULES COMMACOMMA RULE
      {
         Rule tmpRule($3->second);
         $1->second.push_back(tmpRule);
         $$ =  new pair< ReplaceArrow, vector<Rule> > ($3->first, $1->second);
         delete $3;
      }
      ;
   
RULE: RULE_MAPPING
      {
      
         //  $$ = new Rule($1->second);;
         
         Rule rule($1->second);;
         $$ =  new pair< ReplaceArrow, Rule> ($1->first, rule);
         delete $1;
      }
      | RULE_MAPPING CONTEXTS_WITH_MARK
      {
        //$$ = new Rule( $1->second, $2->second, $2->first );
        
        Rule rule( $1->second, $2->second, $2->first );
        $$ =  new pair< ReplaceArrow, Rule> ($1->first, rule);
        delete $1, $2;
      }
      ;


RULE_MAPPING: REGEXP2 REPLACE_ARROW REGEXP2
      {
         HfstTransducer tmp(*$1);
         tmp.cross_product(*$3);
         // $$ = & $1->cross_product(*$3);
          $$ =  new pair< ReplaceArrow, HfstTransducer> ($2, tmp);
          std::cerr << "Replace arrow enum \n"<< $2 << std::endl;
          delete $3;
      }
     /* 
      | REGEXP2 REPLACE_ARROW STRING MARKUP_MARKER REGEXP2
      {
      
          HfstTransducer tmp(*$1);
          tmp.cross_product(*$3);
         // $$ = & $1->cross_product(*$3);
          $$ =  new pair< ReplaceArrow, HfstTransducer> (E_REPLACE_RIGHT_MARKUP, tmp);
          std::cerr << "Replace arrow enum \n"<< $2 << std::endl;
          delete $3;
          
          
          HfstTransducer mark_up_replace(const Rule &rule,
                              const HfstTransducerPair &marks,
                              bool optional);
      
      }
      */
      ;    
   
           


// Contexts:
CONTEXTS_WITH_MARK:  
      CONTEXT_MARK CONTEXTS_VECTOR
      {
      $$ =  new pair< ReplaceType, HfstTransducerPairVector> ($1, *$2);
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
CONTEXT: REGEXP2 CENTER_MARKER REGEXP2 
         {
            $$ = new HfstTransducerPair(*$1, *$3);
            delete $1, $3; 
         }
      | REGEXP2 CENTER_MARKER
         {
               
            //hfst::internal_epsilon,
            HfstTokenizer TOK;
            TOK.add_multichar_symbol(hfst::internal_epsilon);
            HfstTransducer *epsilon = new HfstTransducer(hfst::internal_epsilon, TOK, hfst::xre::format);
               
            $$ = new HfstTransducerPair(*$1, *epsilon);
            delete $1, epsilon; 
         }
      | CENTER_MARKER REGEXP2
         {
            //hfst::internal_epsilon,
            HfstTokenizer TOK;
            TOK.add_multichar_symbol(hfst::internal_epsilon);
            HfstTransducer *epsilon = new HfstTransducer(hfst::internal_epsilon, TOK, hfst::xre::format);
         
            $$ = new HfstTransducerPair(*epsilon, *$2);
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
            xreerror("No upper priority union");
            $$ = $1;
            delete $3;
        }
       | REGEXP5 LOWER_PRIORITY_UNION REGEXP6 {
            xreerror("No lower priority union");
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
            xreerror("No ignoring");
            $$ = $1;
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
            xreerror("No complement");
            $$ = $2;
        }
       | CONTAINMENT REGEXP8 {
            xreerror("No containment");
            $$ = $2;
        }
       | CONTAINMENT_ONCE REGEXP8 {
            xreerror("No containment once");
            $$ = $2;
        }
       | CONTAINMENT_OPT REGEXP8 {
            xreerror("No containment optionally");
            $$ = $2;
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
            $$ = & $1->repeat_n(2);
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
            xreerror("no term complement");
            $$ = $2;
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
        | LEFT_CURLY REGEXP2 RIGHT_CURLY {
            $$ = & $2->minimize();
        }
        ;

REGEXP12: LABEL { }
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

LABEL: SYMBOL PAIR_SEPARATOR SYMBOL {
        $$ = new HfstTransducer($1, $3, hfst::xre::format);
        free($1);
        free($3);
     }
     | SYMBOL PAIR_SEPARATOR EPSILON_TOKEN {
        $$ = new HfstTransducer($1, hfst::internal_epsilon, hfst::xre::format);
        free($1);
     }
     | SYMBOL PAIR_SEPARATOR ANY_TOKEN {
        $$ = new HfstTransducer($1, hfst::internal_unknown, hfst::xre::format);
        free($1);
     }
     | EPSILON_TOKEN PAIR_SEPARATOR EPSILON_TOKEN {
        $$ = new HfstTransducer(hfst::internal_epsilon, 
                                hfst::internal_epsilon, hfst::xre::format);
     }
     | EPSILON_TOKEN PAIR_SEPARATOR SYMBOL {
        $$ = new HfstTransducer(hfst::internal_epsilon, $3, hfst::xre::format);
        free($3);
     }
     | EPSILON_TOKEN PAIR_SEPARATOR ANY_TOKEN {
        $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | ANY_TOKEN PAIR_SEPARATOR ANY_TOKEN {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | ANY_TOKEN PAIR_SEPARATOR SYMBOL {
        $$ = new HfstTransducer(hfst::internal_unknown, $3, hfst::xre::format);
        free($3);
     }
     | ANY_TOKEN PAIR_SEPARATOR EPSILON_TOKEN {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_epsilon,
                                hfst::xre::format);
     }
     | SYMBOL PAIR_SEPARATOR_WO_RIGHT {
        $$ = new HfstTransducer($1, hfst::internal_unknown, hfst::xre::format);
        free($1);
     }
     | EPSILON_TOKEN PAIR_SEPARATOR_WO_RIGHT {
        $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | ANY_TOKEN PAIR_SEPARATOR_WO_RIGHT {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | PAIR_SEPARATOR_WO_LEFT SYMBOL {
        $$ = new HfstTransducer(hfst::internal_unknown, $2, hfst::xre::format);
        free($2);
     }
     | PAIR_SEPARATOR_WO_LEFT ANY_TOKEN {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | PAIR_SEPARATOR_WO_LEFT EPSILON_TOKEN {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_epsilon,
                                hfst::xre::format);
     }
     | SYMBOL {
        $$ = new HfstTransducer($1, $1, hfst::xre::format);
        free($1);
     }
     | PAIR_SEPARATOR_SOLE {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | EPSILON_TOKEN {
        $$ = new HfstTransducer(hfst::internal_epsilon, hfst::internal_epsilon,
                                hfst::xre::format);
     }
     | ANY_TOKEN {
        $$ = new HfstTransducer(hfst::internal_unknown, hfst::internal_unknown,
                                hfst::xre::format);
     }
     | QUOTED_LITERAL {
        $$ = new HfstTransducer($1, $1, hfst::xre::format);
        free($1);
     }
     | BOUNDARY_MARKER {
        $$ = new HfstTransducer("@#@", "@#@", hfst::xre::format);
     }
     ;

%%

