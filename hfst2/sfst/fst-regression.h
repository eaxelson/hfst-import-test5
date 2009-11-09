#include "fst.h"
#include "utf8.h"

#include <vector>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

/** \file fst-regression.h
 *  Functions for regression-testing of two-level grammars. 
 * 
 */


vector<char*> * read_string( char * str );
void erase(const int BUFFERSIZE, char * byte_string);
void process_line(const char * file_name, const int file_row_number, char * str, 
		  vector<vector<char*>*> * str_codes, const int BUFFERSIZE);
void read_utf8file( FILE * utf8file, const char * file_name,vector<vector<char*>*> * analyses,
		    vector<vector<vector<char*>*>*> * surface_forms);
vector<Character> * recode(const char * file_name, const char * lexicon_name, const  vector<char*> * tokens, const Alphabet * sigma);
void print_form(ostream &out, const vector<Character> * form, const Alphabet &sigma);

int trace_rule( const vector<Label> * pair_string, const Transducer * rule, const Alphabet &sigma, bool &fails );
void add_path_to_trie(const vector<Label> * path, Transducer * t);
bool trace(const vector<Character> * analysis, const vector<Character> * surface, const vector<Transducer*> * all_analyses_by_rules,
           const vector<Transducer*> * Rules, vector<char*> * rule_names, const Alphabet &sigma, Transducer * all_allowed_forms);
vector<Transducer*> * make_analyses( const vector<Character> * analysis, const vector<Transducer*> * Rules, const Alphabet &sigma );
vector<vector<Character>*> * get_surface_forms1( Node * n , NodeNumbering &node_numbers, int visited[] );
vector<vector<Character>*> * get_surface_forms( Transducer * t );
void derivations(const vector<Character> * analysis, const vector<vector<Character>*> * surface_forms, const vector<Transducer*> * Rules, 
		 vector<char*> * rule_names, const Alphabet &sigma);


