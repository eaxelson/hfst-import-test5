%{
//! @file xfst-parser.yy
//!
//! @brief A parser for xfst
//!
//! @author Tommi A. Pirinen


//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3 of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdlib>
#include <cstdio>

#include <string>
using std::string;

#include "XfstCompiler.h"
#include "xfst-utils.h"

// obligatory yacc stuff
extern int hxfstlineno;
void hxfsterror(const char *text);
int hxfstlex(void);



%}

// ouch
%name-prefix="hxfst"
// yup, nice messages
%error-verbose
// just cos I use the llloc struct
%locations

%union 
{
    char* name;
    char* text;
    char** list;
    unsigned int number;
    char* file;
    void* nothing;
}

%token <text> APROPOS DESCRIBE ECHO SYSTEM QUIT REGEX HFST
%token <name> NAMETOKEN GLOB PROTOTYPE LABEL
              DEFINE_NAME DEFINE_FUNCTION
%token <number> NUMBER
%token <list> RANGE
%token <file> REDIRECT_IN REDIRECT_OUT
%token SAVE_PROLOG LOWER FOR REVERSE VIEW LOADD PRINT_LABEL_COUNT
       TEST_OVERLAP TEST_NONNULL CONCATENATE LOADS INVERT PRINT_ALIASES
       PRINT_LABELS OPTIONAL PRINT_SHORTEST_STRING_SIZE READ_PROPS
       TEST_FUNCT PRINT_LABELMAPS SUBSTRING COMPOSE READ_SPACED
       TEST_UPPER_UNI COLLECT_EPSILON_LOOPS ZERO_PLUS INSPECT
       ROTATE PRINT_WORDS POP SAVE_SPACED DEFINE SHOW
       PRINT_LONGEST_STRING_SIZE TEST_EQ SORT SAVE_DEFINITIONS SAVE_DOT
       TEST_UPPER_BOUNDED COMPLETE PRINT_FILE_INFO INTERSECT END_SUB
       TURN PRINT_LIST SUBSTITUTE_SYMBOL APPLY_UP ONE_PLUS UNDEFINE
       EPSILON_REMOVE PRINT_RANDOM_WORDS CTRLD EXTRACT_UNAMBIGUOUS
       SEMICOLON PRINT_LOWER_WORDS READ_PROLOG CLEAR PRINT_SIGMA_COUNT
       SUBSTITUTE_NAMED PRINT_FLAGS SET NEGATE APPLY_DOWN PRINT_STACK SAVE_STACK
       PUSH TEST_LOWER_BOUNDED PRINT_DEFINED APPLY_MED SHOW_ALL PRINT_ARCCOUNT
       PRINT_SIZE TEST_NULL PRINT_RANDOM_UPPER PRINT_LONGEST_STRING UPPER_SIDE
       IGNORE TEST_UNAMBIGUOUS PRINT READ_TEXT UNLIST SUBSTITUTE_LABEL
       SAVE_DEFINITION ELIMINATE_FLAG EDIT_PROPS PRINT_UPPER_WORDS NAME
       EXTRACT_AMBIGUOUS DEFINE_ALIAS PRINT_RANDOM_LOWER CROSSPRODUCT
       COMPACT_SIGMA SOURCE AMBIGUOUS ELIMINATE_ALL PRINT_SIGMA
       PRINT_SHORTEST_STRING LEFT_PAREN PRINT_PROPS READ_REGEX
       DEFINE_LIST TEST_ID PRINT_LISTS TEST_SUBLANGUAGE TEST_LOWER_UNI
       COMPILE_REPLACE_UPPER CLEANUP ADD_PROPS PRINT_SIGMA_WORD_COUNT SHUFFLE
       COLON SAVE_TEXT DETERMINIZE SIGMA COMPILE_REPLACE_LOWER UNION
       PRINT_DIR LIST LOWER_SIDE MINIMIZE MINUS PRINT_NAME PRUNE
       PUSH_DEFINED READ_LEXC TWOSIDED_FLAGS
       ERROR
       NEWLINE
    
%type <text> COMMAND_SEQUENCE NAMETOKEN_LIST LABEL_LIST
%%

XFST_SCRIPT: COMMAND_LIST 
           | COMMAND_LIST CTRLD
           ;

COMMAND_LIST: COMMAND_LIST COMMAND 
            | COMMAND
            ;


COMMAND: ADD_PROPS REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->add_props(hfst::xfst::xfst_fopen($2, "w"));
       }
       | ADD_PROPS NAMETOKEN_LIST CTRLD {
            hfst::xfst::xfst_->add_props($2);
            free($2);
       }
       | EDIT_PROPS END_COMMAND {
            hxfsterror("NETWORK PROPERTY EDITOR unimplemented\n");
            return EXIT_FAILURE;
       }
       // apply
       | APPLY_UP NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->apply_up($2);
            free($2);
       }
       | APPLY_UP REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->apply_up(hfst::xfst::xfst_fopen($2, "r"));
       }
       | APPLY_UP END_COMMAND NAMETOKEN_LIST END_SUB {
            hfst::xfst::xfst_->apply_up($3);
            free($3);
       }
       | APPLY_DOWN NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->apply_down($2);
            free($2);
       }
       | APPLY_DOWN REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->apply_down(hfst::xfst::xfst_fopen($2, "r"));
       }
       | APPLY_DOWN END_COMMAND NAMETOKEN_LIST END_SUB {
            hfst::xfst::xfst_->apply_down($3);
            free($3);
       }
       | APPLY_MED NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->apply_med($2);
            free($2);
       }
       | APPLY_MED REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->apply_med(hfst::xfst::xfst_fopen($2, "r"));
       }
       | APPLY_MED END_COMMAND NAMETOKEN_LIST END_SUB {
            hfst::xfst::xfst_->apply_med($3);
            free($3);
       }
       // ambiguous
       | AMBIGUOUS END_COMMAND {
            hxfsterror("unimplemetend ambiguous\n");
            return EXIT_FAILURE;
       }
       | EXTRACT_AMBIGUOUS END_COMMAND {
            hxfsterror("unimplemetend ambiguous\n");
            return EXIT_FAILURE;
       }
       | EXTRACT_UNAMBIGUOUS END_COMMAND {
            hxfsterror("unimplemetend ambiguous\n");
            return EXIT_FAILURE;
       }
       // define
       | DEFINE_ALIAS NAMETOKEN COMMAND_SEQUENCE END_COMMAND{
            hfst::xfst::xfst_->define_alias($2, $3);
            free($2);
            free($3);
       }
       | DEFINE_ALIAS NAMETOKEN END_COMMAND NAMETOKEN_LIST END_SUB {
            hfst::xfst::xfst_->define_alias($2, $4);
            free($2);
            free($4);
       }
       | DEFINE_LIST NAMETOKEN NAMETOKEN_LIST END_COMMAND {
            hfst::xfst::xfst_->define_list($2, $3);
            free($2);
            free($3);
       }
       | DEFINE_LIST NAMETOKEN RANGE END_COMMAND {
            hfst::xfst::xfst_->define_list($2, $3[0], $3[1]);
            free($2);
            free($3[0]);
            free($3[1]);
            free($3);
       }
       | DEFINE_NAME REGEX {
            hfst::xfst::xfst_->define($1, $2);
            free($1);
            free($2);
       }
       | DEFINE_FUNCTION REGEX {
            hfst::xfst::xfst_->define($1, $1, $2);
            free($1);
            free($2);
       }
       | UNDEFINE NAMETOKEN_LIST END_COMMAND {
            hfst::xfst::xfst_->undefine($2);
            free($2);
       }
       | UNLIST NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->unlist($2);
            free($2);
       }
       | NAME NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->name($2);
            free($2);
       }
       | LOADD NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->load_definitions(hfst::xfst::xfst_fopen($2, "r"));
            free($2);
       }
       // help
       | APROPOS { 
            hfst::xfst::xfst_->apropos($1);
            free($1);
       }
       | DESCRIBE { 
            hfst::xfst::xfst_->describe($1);
       }
       // stack
       | CLEAR END_COMMAND {
            hfst::xfst::xfst_->clear();
       }
       | POP END_COMMAND {
            hfst::xfst::xfst_->pop();
       }
       | PUSH NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->push($2);
            free($2);
       }
       | PUSH END_COMMAND {
            hfst::xfst::xfst_->push();
       }
       | TURN END_COMMAND {
            hfst::xfst::xfst_->turn();
       }
       | ROTATE END_COMMAND {
            hfst::xfst::xfst_->rotate();
       }
       | LOADS REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->load_stack($2);
            free($2);
       }
       // wrobble
       | COLLECT_EPSILON_LOOPS END_COMMAND {
            hfst::xfst::xfst_->collect_epsilon_loops();
       }
       | COMPACT_SIGMA END_COMMAND {
            hfst::xfst::xfst_->compact_sigma();
       }
       // flags
       | ELIMINATE_FLAG NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->eliminate_flag($2);
            free($2);
       }
       | ELIMINATE_ALL END_COMMAND {
            hfst::xfst::xfst_->eliminate_flags();
       }
       // system
       | ECHO { 
            hfst::xfst::xfst_->echo($1);
            free($1);
       }
       | QUIT { 
            hfst::xfst::xfst_->quit($1);
            free($1);
            return EXIT_SUCCESS;
       }
       | HFST {
            hfst::xfst::xfst_->hfst($1);
            free($1);
       }
       | SOURCE NAMETOKEN END_COMMAND {
            hxfsterror("source not implemented yywrap\n");
            return EXIT_FAILURE;
       }
       | SYSTEM {
            hfst::xfst::xfst_->system($1);
            free($1);
       }
       | VIEW END_COMMAND {
            hxfsterror("view not implemented\n");
            return EXIT_FAILURE;
       }
       // vars
       | SET NAMETOKEN NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->set($2, $3);
            free($2);
            free($3);
       }
       | SET NAMETOKEN NUMBER END_COMMAND {
            hfst::xfst::xfst_->set($2, $3);
            free($2);
       }
       | SHOW NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->show($2);
            free($2);
       }
       | SHOW_ALL END_COMMAND {
            hfst::xfst::xfst_->show();
       }
       // tests
       | TEST_EQ END_COMMAND {
            hfst::xfst::xfst_->test_eq();
       }
       | TEST_FUNCT END_COMMAND {
            hfst::xfst::xfst_->test_funct();
       }
       | TEST_ID END_COMMAND {
            hfst::xfst::xfst_->test_id();
       }
       | TEST_LOWER_BOUNDED END_COMMAND {
            hfst::xfst::xfst_->test_lower_bounded();
       }
       | TEST_LOWER_UNI END_COMMAND {
            hfst::xfst::xfst_->test_lower_uni();
       }
       | TEST_UPPER_BOUNDED END_COMMAND {
            hfst::xfst::xfst_->test_upper_bounded();
       }
       | TEST_UPPER_UNI END_COMMAND {
            hfst::xfst::xfst_->test_upper_uni();
       }
       | TEST_NONNULL END_COMMAND {
            hfst::xfst::xfst_->test_nonnull();
       }
       | TEST_NULL END_COMMAND {
            hfst::xfst::xfst_->test_null();
       }
       | TEST_OVERLAP END_COMMAND {
            hfst::xfst::xfst_->test_overlap();
       }
       | TEST_SUBLANGUAGE END_COMMAND {
            hfst::xfst::xfst_->test_sublanguage();
       }
       | TEST_UNAMBIGUOUS END_COMMAND {
            hfst::xfst::xfst_->test_unambiguous();
       }
       // substitutes
       | SUBSTITUTE_NAMED NAMETOKEN FOR LABEL END_COMMAND {
            hfst::xfst::xfst_->substitute($2, $4);
            free($2);
            free($4);
       }
       | SUBSTITUTE_LABEL LABEL_LIST FOR LABEL END_COMMAND {
            hfst::xfst::xfst_->substitute($2, $4);
            free($2);
            free($4);
       }
       | SUBSTITUTE_SYMBOL NAMETOKEN_LIST FOR LABEL END_COMMAND {
            hfst::xfst::xfst_->substitute($2, $4);
            free($2);
            free($4);
       }
       // prints
       | PRINT_ALIASES REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_aliases(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_ALIASES END_COMMAND {
            hfst::xfst::xfst_->print_aliases(stdout);
       }
       | PRINT_ARCCOUNT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_arc_count(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_ARCCOUNT NAMETOKEN END_COMMAND {
            if (strcmp($2, "upper") && strcmp($2, "lower"))
            {
                hxfsterror("should be upper or lower");
                return EXIT_FAILURE;
            }
            hfst::xfst::xfst_->print_arc_count($2, stdout);
            free($2);
       }
       | PRINT_ARCCOUNT END_COMMAND {
            hfst::xfst::xfst_->print_arc_count(stdout);
       }
       | PRINT_DEFINED REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_defined(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_DEFINED END_COMMAND {
            hfst::xfst::xfst_->print_defined(stdout);
       }
       | PRINT_DIR GLOB REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_dir($2, hfst::xfst::xfst_fopen($3, "w"));
            free($2);
       }
       | PRINT_DIR GLOB END_COMMAND {
            hfst::xfst::xfst_->print_dir($2, stdout);
            free($2);
       }
       | PRINT_DIR REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_dir("*", hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_DIR END_COMMAND {
            hfst::xfst::xfst_->print_dir("*", stdout);
       }
       | PRINT_FILE_INFO REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_file_info(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_FILE_INFO END_COMMAND {
            hfst::xfst::xfst_->print_file_info(stdout);
       }
       | PRINT_FLAGS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_flags(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_FLAGS END_COMMAND {
            hfst::xfst::xfst_->print_flags(stdout);
       }
       | PRINT_LABELS NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_labels($2, stdout);
            free($2);
       }
       | PRINT_LABELS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_labels(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_LABELS END_COMMAND {
            hfst::xfst::xfst_->print_labels(stdout);
       }
       | PRINT_LABEL_COUNT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_label_count(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_LABEL_COUNT END_COMMAND {
            hfst::xfst::xfst_->print_label_count(stdout);
       }
       | PRINT_LIST NAMETOKEN REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_list($2, hfst::xfst::xfst_fopen($3, "w"));
            free($2);
       }
       | PRINT_LIST NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_list($2, stdout);
            free($2);
       }
       | PRINT_LISTS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_list(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_LISTS END_COMMAND {
            hfst::xfst::xfst_->print_list(stdout);
       }
       | PRINT_LONGEST_STRING REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_longest_string(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_LONGEST_STRING END_COMMAND {
            hfst::xfst::xfst_->print_longest_string(stdout);
       }
       | PRINT_LONGEST_STRING_SIZE REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_longest_string_size(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_LONGEST_STRING_SIZE END_COMMAND {
            hfst::xfst::xfst_->print_longest_string_size(stdout);
       }
       | PRINT_SHORTEST_STRING REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_shortest_string(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SHORTEST_STRING END_COMMAND {
            hfst::xfst::xfst_->print_shortest_string(stdout);
       }
       | PRINT_SHORTEST_STRING_SIZE REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_shortest_string_size(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SHORTEST_STRING_SIZE END_COMMAND {
            hfst::xfst::xfst_->print_shortest_string_size(stdout);
       }
       | PRINT_LOWER_WORDS NAMETOKEN NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_lower_words($2, $3, stdout);
            free($2);
       }
       | PRINT_LOWER_WORDS NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_lower_words($2, 0, stdout);
            free($2);
       }
       | PRINT_LOWER_WORDS NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_lower_words(0, $2, stdout);
       }
       | PRINT_LOWER_WORDS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_lower_words(0, 0, hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_RANDOM_LOWER NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_random_lower($2, stdout);
       }
       | PRINT_RANDOM_LOWER END_COMMAND {
            hfst::xfst::xfst_->print_random_lower(15, stdout);
       }
       | PRINT_RANDOM_LOWER REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_random_lower(15, hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_UPPER_WORDS NAMETOKEN NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_upper_words($2, $3, stdout);
       }
       | PRINT_UPPER_WORDS NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_upper_words($2, 0, stdout);
            free($2);
       }
       | PRINT_UPPER_WORDS NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_upper_words(0, $2, stdout);
       }
       | PRINT_UPPER_WORDS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_upper_words(0, 0, hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_RANDOM_UPPER NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_random_upper($2, stdout);
       }
       | PRINT_RANDOM_UPPER END_COMMAND {
            hfst::xfst::xfst_->print_random_upper(15, stdout);
       }
       | PRINT_WORDS NAMETOKEN NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_words($2, $3, stdout);
            free($2);
       }
       | PRINT_WORDS NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_words($2, 0, stdout);
            free($2);
       }
       | PRINT_WORDS NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_words(0, $2, stdout);
       }
       | PRINT_WORDS END_COMMAND {
            hfst::xfst::xfst_->print_words(0, 0, stdout);
       }
       | PRINT_RANDOM_WORDS NUMBER END_COMMAND {
            hfst::xfst::xfst_->print_random_words($2, stdout);
       }
       | PRINT_RANDOM_WORDS END_COMMAND {
            hfst::xfst::xfst_->print_random_words(15, stdout);
       }
       | PRINT NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_net($2, stdout);
            free($2);
       }
       | PRINT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_net(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT END_COMMAND {
            hfst::xfst::xfst_->print_net(stdout);
       }
       | PRINT_PROPS NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_properties($2, stdout);
            free($2);
       }
       | PRINT_PROPS END_COMMAND {
            hfst::xfst::xfst_->print_properties(stdout);
       }
       | PRINT_PROPS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_properties(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SIGMA NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_sigma($2, stdout);
            free($2);
       }
       | PRINT_SIGMA REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_sigma(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SIGMA END_COMMAND {
            hfst::xfst::xfst_->print_sigma(stdout);
       }
       | PRINT_SIGMA_COUNT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_sigma_count(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SIGMA_COUNT END_COMMAND {
            hfst::xfst::xfst_->print_sigma_count(stdout);
       }
       | PRINT_SIGMA_WORD_COUNT NAMETOKEN END_COMMAND {
            if (strcmp($2, "upper") && strcmp($2, "lower"))
            {
                hxfsterror("must be upper or lower\n");
                return EXIT_FAILURE;
            }
            hfst::xfst::xfst_->print_sigma_word_count($2, stdout);
            free($2);
       }
       | PRINT_SIGMA_WORD_COUNT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_sigma_word_count(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SIGMA_WORD_COUNT END_COMMAND {
            hfst::xfst::xfst_->print_sigma_word_count(stdout);
       }
       | PRINT_SIZE NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->print_size($2, stdout);
            free($2);
       }
       | PRINT_SIZE REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_size(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_SIZE END_COMMAND {
            hfst::xfst::xfst_->print_size(stdout);
       }
       | PRINT_STACK REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_stack(hfst::xfst::xfst_fopen($2, "w"));
       }
       | PRINT_STACK END_COMMAND {
            hfst::xfst::xfst_->print_stack(stdout);
       }
       | PRINT_LABELMAPS REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->print_labelmaps(hfst::xfst::xfst_fopen($2, "w"));
       }
       // writes
       | SAVE_DOT NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->write_dot($2, stdout);
            free($2);
       }
       | SAVE_DOT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_dot(hfst::xfst::xfst_fopen($2, "w"));
       }
       | SAVE_DOT END_COMMAND {
            hfst::xfst::xfst_->write_dot(stdout);
       }
       | SAVE_DEFINITION NAMETOKEN LEFT_PAREN REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_function($2, $4);
            free($2);
       }
       | SAVE_DEFINITION NAMETOKEN LEFT_PAREN END_COMMAND {
            hfst::xfst::xfst_->write_function($2, 0);
            free($2);
       }
       | SAVE_DEFINITION NAMETOKEN REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_definition($2, $3);
            free($2);
       }
       | SAVE_DEFINITION NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->write_definition($2, 0);
            free($2);
       }
       | SAVE_DEFINITIONS REDIRECT_OUT END_COMMAND {    
            hfst::xfst::xfst_->write_definitions($2);
       }
       | SAVE_DEFINITIONS END_COMMAND {
            hfst::xfst::xfst_->write_definitions(0);
       }
       | SAVE_STACK NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->write_stack($2);
            free($2);
       }
       | SAVE_PROLOG REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_prolog(hfst::xfst::xfst_fopen($2, "w"));
       }
       | SAVE_PROLOG END_COMMAND {
            hfst::xfst::xfst_->write_prolog(stdout);
       }
       | SAVE_SPACED REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_spaced(hfst::xfst::xfst_fopen($2, "w"));
       }
       | SAVE_SPACED END_COMMAND {
            hfst::xfst::xfst_->write_spaced(stdout);
       }
       | SAVE_TEXT REDIRECT_OUT END_COMMAND {
            hfst::xfst::xfst_->write_text(hfst::xfst::xfst_fopen($2, "w"));
       }
       | SAVE_TEXT END_COMMAND {
            hfst::xfst::xfst_->write_text(stdout);
       }
       // reads
       | READ_PROPS REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->read_props(hfst::xfst::xfst_fopen($2, "r"));
       }
       | READ_PROPS END_COMMAND {
            hfst::xfst::xfst_->read_props(stdin);
       }
       | READ_PROLOG NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->read_prolog(hfst::xfst::xfst_fopen($2, "r"));
       }
       | READ_PROLOG END_COMMAND {
            hfst::xfst::xfst_->read_prolog(stdin);
       }
       | READ_REGEX REGEX {
            hfst::xfst::xfst_->read_regex($2);
            free($2);
       }
       | READ_REGEX REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->read_regex(hfst::xfst::xfst_fopen($2, "r"));
       }
       | READ_REGEX NAMETOKEN_LIST SEMICOLON END_COMMAND {
            hfst::xfst::xfst_->read_regex($2);
            free($2);
       }
       | READ_SPACED REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->read_spaced(hfst::xfst::xfst_fopen($2, "r"));
       }
       | READ_SPACED NAMETOKEN_LIST CTRLD {
            hfst::xfst::xfst_->read_spaced($2);
            free($2);
       }
       | READ_TEXT REDIRECT_IN END_COMMAND {
            hfst::xfst::xfst_->read_text(hfst::xfst::xfst_fopen($2, "r"));
       }
       | READ_TEXT NAMETOKEN_LIST CTRLD {
            hfst::xfst::xfst_->read_text($2);
            free($2);
       }
       | READ_LEXC NAMETOKEN END_COMMAND {
            hfst::xfst::xfst_->read_lexc(hfst::xfst::xfst_fopen($2, "r"));
            free($2);
       }
       | READ_LEXC NAMETOKEN_LIST CTRLD {
            hfst::xfst::xfst_->read_lexc(stdin);
       }
       // net ops
       | CLEANUP END_COMMAND {
            hfst::xfst::xfst_->cleanup_net();
       }
       | COMPLETE END_COMMAND {
            hfst::xfst::xfst_->complete_net();
       }
       | COMPOSE END_COMMAND {
            hfst::xfst::xfst_->compose_net();
       }
       | CONCATENATE END_COMMAND {
            hfst::xfst::xfst_->concatenate_net();
       }
       | CROSSPRODUCT END_COMMAND {
            hfst::xfst::xfst_->crossproduct_net();
       }
       | DETERMINIZE END_COMMAND {
            hfst::xfst::xfst_->determinize_net();
       }
       | EPSILON_REMOVE END_COMMAND {
            hfst::xfst::xfst_->epsilon_remove_net();
       }
       | IGNORE END_COMMAND {
            hfst::xfst::xfst_->ignore_net();
       }
       | INTERSECT END_COMMAND {
            hfst::xfst::xfst_->intersect_net();
       }
       | INSPECT END_COMMAND {
            hfst::xfst::xfst_->inspect_net();
       }
       | INVERT END_COMMAND {
            hfst::xfst::xfst_->invert_net();
       }
       | LOWER_SIDE END_COMMAND {
            hfst::xfst::xfst_->lower_side_net();
       }
       | UPPER_SIDE END_COMMAND {
            hfst::xfst::xfst_->upper_side_net();
       }
       | NEGATE END_COMMAND {
            hfst::xfst::xfst_->negate_net();
       }
       | ONE_PLUS END_COMMAND {
            hfst::xfst::xfst_->one_plus_net();
       }
       | ZERO_PLUS END_COMMAND {
            hfst::xfst::xfst_->zero_plus_net();
       }
       | OPTIONAL END_COMMAND {
            hfst::xfst::xfst_->optional_net();
       }
       | REVERSE END_COMMAND {
            hfst::xfst::xfst_->reverse_net();
       }
       | SHUFFLE END_COMMAND {
            hfst::xfst::xfst_->shuffle_net();
       }
       | SIGMA END_COMMAND {
            hfst::xfst::xfst_->sigma_net();
       }
       | SORT END_COMMAND {
            hfst::xfst::xfst_->sort_net();
       }
       | SUBSTRING END_COMMAND {
            hfst::xfst::xfst_->substring_net();
       }
       | UNION END_COMMAND {
            hfst::xfst::xfst_->union_net();
       }
       | LABEL END_COMMAND {
            hfst::xfst::xfst_->label_net();
       }
       | COMPILE_REPLACE_LOWER END_COMMAND {
            hfst::xfst::xfst_->compile_replace_lower_net();
       }
       | COMPILE_REPLACE_UPPER END_COMMAND {
            hfst::xfst::xfst_->compile_replace_upper_net();
       }
       | NAMETOKEN {
            fprintf(stderr, "Command %s is not recognised\n", $1);
       }
       ;

       END_COMMAND: NEWLINE | ;

COMMAND_SEQUENCE: COMMAND_SEQUENCE NAMETOKEN {
                    $$ = static_cast<char*>(malloc(sizeof(char)*strlen($1)+strlen($2)+2));
                    char* r = $$;
                    char* s = $1;
                    while (*s != '\0')
                    {
                        *r = *s;
                        r++;
                        s++;
                    }
                    *r = ' ';
                    r++;
                    s = $2;
                    while (*s != '\0')
                    {
                        *r = *s;
                        r++;
                        s++;
                    }
                    *r = '\0';
                }
                | COMMAND_SEQUENCE SEMICOLON {
                    $$ = $1;
                }
                | NAMETOKEN {
                    $$ = $1;
                }
                ;


LABEL_LIST: LABEL_LIST LABEL {
            $$ = static_cast<char*>(malloc(sizeof(char)*strlen($1) + strlen($2) + 1));
            char* s = $1;
            char* r = $$;
            while (*s != '\0')
            {
                *r = *s;
                r++;
                s++;
            }
            s = $2;
            while (*s != '\0')
            {
                *r = *s;
                r++;
                s++;
            }
            *r = '\0';
          }
          | LABEL {
            $$ = $1;
          }
          ;


NAMETOKEN_LIST: NAMETOKEN_LIST NAMETOKEN {
                $$ = static_cast<char*>(malloc(sizeof(char)*strlen($1)+strlen($2)+2));
                char* s = $1;
                char* r = $$;
                while (*s != '\0')
                {
                    *r = *s;
                    r++;
                    s++;
                }
                *r = ' ';
                r++;
                s = $2;
                while (*s != '\0')
                {
                    *r = *s;
                    r++;
                    s++;
                }
                *r = '\0';
                $$ = $1;
             }
             | NAMETOKEN {
                $$ = $1;
             }
             ;
%%

// oblig. declarations
extern FILE* hxfstin;
int hxfstparse(void);

// gah, bison/flex error mechanism here
void
hxfsterror(const char* text)
{ 
    fprintf(stderr,  "%s\n", text);
}


// vim: set ft=yacc:
