%{

#include <stdio.h>
#include <assert.h>

#include "../HfstTransducer.h"
#include "../HfstInputStream.h"

using hfst::HfstTransducer;


#include "xre_utils.h"

extern void xreerror(char * text);
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
}

%type <transducer> ONE_REGEXP EXP
%type <label> CHAR

%token <value> CATENATE_N CATENATE_N_PLUS CATENATE_N_MINUS
%token <values> CATENATE_N_TO_K
%token <weight> WEIGHT END_OF_WEIGHTED_EXPRESSION
%token <label> QUOTED_LITERAL SYMBOL 
               READ_TEXT READ_BIN READ_PROLOG READ_OTHER READ_SPACED
               READ_RE
%token BOUNDARY_MARKER CENTER_MARKER MARKUP_MARKER
       LEFT_RESTRICTION LEFT_RIGHT_ARROW LEFT_ARROW RIGHT_ARROW
       REPLACE_ARROW
       OPTIONAL_REPLACE LTR_LONGEST_REPLACE LTR_SHORTEST_REPLACE
       RTL_LONGEST_REPLACE RTL_SHORTEST_REPLACE
       REPLACE_CONTEXT_UU REPLACE_CONTEXT_LL
       REPLACE_CONTEXT_LU REPLACE_CONTEXT_UL
       LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_BRACKET RIGHT_BRACKET 
       LEFT_CURLY RIGHT_CURLY LEFT_BRACKET_DOTTED RIGHT_BRACKET_DOTTED
       COMPOSITION CROSS_PRODUCT
       UPPER_PRIORITY_UNION LOWER_PRIORITY_UNION
       UPPER_MINUS LOWER_MINUS
       SUBSTITUTE_LEFT
       INTERSECTION UNION MINUS
       BEFORE AFTER SHUFFLE
       IGNORING IGNORE_INTERNALLY
       PLUS STAR
       LOWER UPPER INVERT REVERSE
       CONTAINMENT CONTAINMENT_ONCE CONTAINMENT_OPT
       COMPLEMENT TERM_COMPLEMENT
       PAIR_SEPARATOR PAIR_SEPARATOR_SOLE PAIR_SEPARATOR_WO_LEFT
       PAIR_SEPARATOR_WO_RIGHT
       EPSILON_TOKEN ANY_TOKEN
       END_OF_EXPRESSION
       LEXER_ERROR
       ARROWSTUFF CONTEXTSTUFF COMMA

%%

ONE_REGEXP: EXP {
        hfst::xre::last_compiled = & $1->minimize();
        $$ = hfst::xre::last_compiled;
    }
    | EXP END_OF_EXPRESSION {
        hfst::xre::last_compiled = & $1->minimize();
        $$ = hfst::xre::last_compiled;
    }
    | EXP END_OF_WEIGHTED_EXPRESSION {
        hfst::xre::last_compiled = & $1->minimize().set_final_weights($2);
        $$ = hfst::xre::last_compiled;
    }
    ;

EXP: EXP CROSS_PRODUCT EXP {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented crossproduct\n");
     }
   | EXP COMPOSITION EXP {
        $$ = & $1->compose(*$3);
        delete $3;
     }
   | EXP ARROWSTUFF CONTEXTSTUFF {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented arrows\n");
     }
   | EXP UNION EXP {
        $$ = & $1->disjunct(*$3);
        delete $3;
     }
   | EXP INTERSECTION EXP {
        $$ = & $1->intersect(*$3);
        delete $3;
     }
   | EXP MINUS EXP {
        $$ = & $1->subtract(*$3);
        delete $3;
     }
   | EXP SHUFFLE EXP {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented shuffle\n");
     }
   | EXP BEFORE EXP {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented before\n");
     }
   | EXP AFTER EXP {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented after\n");
     }
    | EXP EXP {
        $$ = & $1->concatenate(*$2);
        delete $2;
    }
    | EXP IGNORING EXP
    {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented ignoring\n");
    }
    | EXP IGNORE_INTERNALLY EXP
    {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented ignoring internally\n");
    }
    | EXP STAR
    {
        $$ = & $1->repeat_star();
    }
    | EXP PLUS
    {
        $$ = & $1->repeat_plus();
    }
    | EXP CATENATE_N
    {
        $$ = & $1->repeat_n($2);
    }
    | EXP CATENATE_N_PLUS
    {
        $$ = & $1->repeat_n_plus($2);
    }
    | EXP CATENATE_N_MINUS
    {
        $$ = & $1->repeat_n_minus($2);
    }
    | EXP CATENATE_N_TO_K
    {
        $$ = & $1->repeat_n_to_k($2[0], $2[1]);
        free($2);
    }
    | EXP REVERSE
    {
        $$ = & $1->reverse();
    }
    | EXP INVERT
    {
        $$ = & $1->invert();
    }
    | EXP UPPER
    {
        $$ = & $1->input_project();
    }
    | EXP LOWER
    {
        $$ = & $1->output_project();
    }
    | CONTAINMENT EXP
    { 
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented contains\n");
    }
    | CONTAINMENT_ONCE EXP
    { 
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented contains once\n");
    }
    | CONTAINMENT_OPT EXP
    { 
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented contains optionally\n");
    }
    | TERM_COMPLEMENT EXP
    {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented term complement\n");
    }
    | COMPLEMENT EXP
    {
        $$ = new HfstTransducer(hfst::xre::format);
        fprintf(stderr, "*** XRE unimplemented complement\n");
    }  
    | LEFT_BRACKET EXP RIGHT_BRACKET 
    {
        $$ = $2; 
    }
    | LEFT_CURLY EXP RIGHT_CURLY
    {
        /* Xerox defines curly may only bracket disjunctions,
         * but, meh, I’ll synonymise it to square brackets.
         */
        $$ = $2;
    }
    | LEFT_PARENTHESIS EXP RIGHT_PARENTHESIS
    {
        $$ = & $2->optionalize();
    }
    | LEFT_BRACKET RIGHT_BRACKET 
    {
        $$ = new HfstTransducer(hfst::xre::format); 
    }
    | LEFT_CURLY RIGHT_CURLY
    {
        $$ = new HfstTransducer(hfst::xre::format);
    }
    | LEFT_PARENTHESIS RIGHT_PARENTHESIS {
        $$ = new HfstTransducer("@0@", hfst::xre::format); 
    }
    | READ_PROLOG {
        fprintf(stderr, "*** XRE: @pl unimplemented\n");
        free($1);
        $$ = new HfstTransducer(hfst::xre::format);
      }
    | READ_BIN
    {
        hfst::HfstInputStream instream($1);
        //instream.open();
        $$ = new HfstTransducer(instream);
        free($1);
    }
    | READ_TEXT
    {
        fprintf(stderr, "*** XRE: @txt unimplemented");
        free($1);
    }
    | CHAR PAIR_SEPARATOR CHAR {
        $$ = new HfstTransducer($1, $3, hfst::xre::format);
        free($1);
        free($3);
      }
    | CHAR PAIR_SEPARATOR CHAR WEIGHT {
        HfstTransducer* t = new HfstTransducer($1, $3, hfst::xre::format);
        t->set_final_weights($4);
        $$ = t;
        free($1);
        free($3);
      }
    | CHAR PAIR_SEPARATOR_WO_RIGHT     {
        $$ = new HfstTransducer($1, "@?@", hfst::xre::format);
        free($1);
      }
    | PAIR_SEPARATOR_WO_LEFT CHAR   {
        $$ = new HfstTransducer("@?@", $2, hfst::xre::format);
        free($2);
      }
    | CHAR {
        if (hfst::xre::definitions.find($1) != hfst::xre::definitions.end())
          {
            // label alone may be sneaky definition macro
            HfstTransducer* def = hfst::xre::definitions[$1];
            $$ = new HfstTransducer(*def);
          }
        else
          {
            $$ = new HfstTransducer($1, hfst::xre::format);
          }
        free($1);
      }
    | CHAR WEIGHT {
        HfstTransducer* wc = new HfstTransducer($1, hfst::xre::format);
        wc->set_final_weights($2);
        $$ = wc;
        free($1);
      }
    | PAIR_SEPARATOR_SOLE {
        $$ = new HfstTransducer("@?@", hfst::xre::format);
      }
    ;

CHAR: SYMBOL { $$ = $1; }
    | QUOTED_LITERAL { $$ = $1; }
    | ANY_TOKEN {
        $$ = strdup("@?@");
        }
    | EPSILON_TOKEN {
        $$ = strdup("@0@");
        }
    ;


%%

