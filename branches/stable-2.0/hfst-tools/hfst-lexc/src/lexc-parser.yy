%{
/*
 * Adapted from fst-compiler.hlexc by Helmut Schmidt.
 * Originally authored by Antoine Trux.
 * with few HFST and misc. changes by Tommi A. Pirinen
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdlib>
#include <cstdio>

// C++
#include <map>
#include <set>
#include <string>
#include <vector>

// Yacc/flex input
#include "flex-utils.h"
// CLI output and such
#include "lexcio.h"
// lexc definitions
#include "lexc.h"

// C++
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

// obligatory yacc stuff
extern int hlexclineno;
extern string& hlexctext;
void hlexcerror(const char *text);
int hlexclex(void);


// Just to check first transducer is properly named (Root)
static bool firstLexicon = true;
static bool warnedInfoStrings = false;
static bool warnedWeightsUnweighted = false;

// Statistics for verbose printing
static unsigned long totalEntries = 0;
static unsigned long currentEntries = 0;
static unsigned long stringEntries = 0;
static unsigned long regexpEntries = 0;

// Metadata
static map<string,string> metadata_;


// The compiler
extern LexcCompiler* lexc;

// Actual functions to handle parsed stuff
static void handle_metadata(const string& name, const string& expression)
{
	metadata_.insert( pair<string,string>(name, expression) );
	lexc_printf(PRINT_DEBUG, 0, _("%s = %s\n"),
			name.c_str(), expression.c_str());
}

static void handle_alphabet(const string& alphabet)
{
	lexc_printf(PRINT_DEBUG, 0, _("ALPHABET: %s\n"),
			alphabet.c_str());
	if (verbosity & PRINT_VERBOSE)
	{
		lexc_list_printf("%s", alphabet.c_str());
	}
	lexc->addAlphabet(alphabet);
	lexc->setClosedSigma(true);
}

static
void
handle_multichar(const string& multichar)
{
	lexc_printf(PRINT_DEBUG, 0, _("MULTICHAR_SYMBOL: %s\n"),
			multichar.c_str());
	if (verbosity & PRINT_VERBOSE)
	{
		lexc_list_printf("%s", multichar.c_str());
	}
	lexc->addAlphabet(multichar);
}

static
void
handle_definition(const string& variable_name, const string& reg_exp)
{
	lexc_printf(PRINT_DEBUG, 0, _("%s = [ %s ]\n"),
			variable_name.c_str(), reg_exp.c_str());
	if (verbosity & PRINT_VERBOSE)
	{
		lexc_list_printf("%s", variable_name.c_str());
	}
	lexc_printf(PRINT_XEROXLIKE, 0,
			_("Defined '%s': ? Kb., ? states, ? arcs, ? paths.\n"),
			variable_name.c_str());
	lexc->addXreDefinition(variable_name, reg_exp);
}

static
void
handle_lexicon_name(const string& lexiconName)
{
	lexc_printf(PRINT_DEBUG, 0, _("LEXICON: %s\n"), lexiconName.c_str());
	if (!firstLexicon)
	{
		lexc_printf(PRINT_XEROXLIKE, 0, _("%d "), currentEntries);
	}
	lexc->setCurrentLexiconName(lexiconName);
	currentEntries = 0;
	if ((firstLexicon) && (lexiconName == "Root"))
	{
		lexc->setInitialLexiconName(lexiconName);
	}
	else if ((firstLexicon) && (lexiconName != "Root"))
	{
		lexc_parser_printf(PRINT_WARNING, 2, 
				_("first lexicon is not named Root\n"));
		lexc_printf(PRINT_VERBOSE, 0, 
				_("  Using %s as initial lexicon\n"), lexiconName.c_str());
		lexc->setInitialLexiconName(lexiconName);
	}
	else if ((!firstLexicon) && (lexiconName == "Root"))
	{
		lexc_parser_printf(PRINT_WARNING, 3,
				_("LEXICON Root is not first\n"));
		lexc_printf(PRINT_VERBOSE, 0, 
				_("  Using Root as initial lexicon again\n"));
		lexc->setInitialLexiconName(lexiconName);
	}
	
	if (verbosity & PRINT_VERBOSE)
	{
		lexc_print_list_end("");
	}
	lexc_printf(PRINT_XEROXLIKE, 0, _("%s..."), lexiconName.c_str());
	lexc_printf(PRINT_VERBOSE, 0, _("\nLexicon %s...\n"), lexiconName.c_str());
	firstLexicon = false;
}

static
void
handle_string_entry_common(const string& cont,
						   const string& gloss,
						   double* weight,
						   bool* is_glossed, bool* is_heavy)
{
	totalEntries++;
	currentEntries++;
	string gloss_handled;
	*is_glossed = false;
	*is_heavy = false;
	if (gloss != "")
	{
		gloss_handled = string(gloss);
		*is_glossed = true;
		size_t wstart = gloss_handled.find("weight:");
		if (wstart != string::npos)
		{
			*is_heavy = true;
			wstart = gloss_handled.find_first_of("-0.123456789", wstart);
			size_t wend = gloss_handled.find_first_not_of("-0.123456789",
														  wstart);
			*weight = strtod(gloss_handled.substr(wstart, wend).c_str(), NULL);
		}
		else
		{
			*is_heavy = false;
		}
	}
	else
	{
		*is_glossed = false;
	}
	lexc_printf(PRINT_VERBOSE, 0, 
				_("Entries so far: %10d; in this lexicon: %10d\r"), 
				totalEntries, currentEntries);
	if ((currentEntries % 10000) == 0)
	{
		lexc_printf(PRINT_XEROXLIKE, 0, _("%d..."), currentEntries);
	}
	if (*is_glossed && !*is_heavy && !warnedInfoStrings)
	{
		lexc_printf(PRINT_VERBOSE, 0, "\n");
		lexc_parser_printf(PRINT_WARNING, 9, _("Ignoring info strings\n"));
		warnedInfoStrings = true;
	}
	if (*is_glossed)
	{
		lexc_printf(PRINT_DEBUG, 0, _("infostring %s\n"), gloss_handled.c_str());
	}
	if (*is_heavy)
	{
		lexc_printf(PRINT_DEBUG, 0, _("weight %f\n"), *weight);
	}
	if (*is_heavy && !weighted && !warnedWeightsUnweighted)
	{
		lexc_printf(PRINT_VERBOSE, 0, "\n");
		lexc_parser_printf(PRINT_WARNING, 11,
			 _("Building unweighted transducer from weighted lexicon\n"));
		warnedWeightsUnweighted = true;
	}
}

static
void
handle_string_entry(const string& data, const string& cont, const string& gloss)
{
	double weight = 0;
	bool is_glossed = false;
	bool is_heavy = false;
	handle_string_entry_common(cont, gloss, &weight, &is_glossed, &is_heavy);
	lexc->addStringEntry(data, cont, weight);
	stringEntries++;
}

static
void
handle_string_pair_entry(const string& upper, const string& lower,
								const string& cont, const string& gloss)
{
	double weight = 0;
	bool is_glossed = false;
	bool is_heavy = false;
	handle_string_entry_common(cont, gloss, &weight, &is_glossed, &is_heavy);
	lexc->addStringPairEntry(upper, lower, cont, weight);
	stringEntries++;
}

static
void
handle_regexp_entry(const string& reg_exp, const string& cont,
						   const string& gloss)
{
	double weight = 0;
	bool is_glossed = false;
	bool is_heavy = false;
	handle_string_entry_common(cont, gloss, &weight, &is_glossed, &is_heavy);
	lexc->addXreEntry(reg_exp, cont, weight);
	regexpEntries++;
}

static
void
handle_eof()
{
	lexc_printf(PRINT_XEROXLIKE, 0, _("%d\n"), currentEntries);
	lexc_printf(PRINT_DEBUG, 0, _("EOF\n"));
	lexc_printf(PRINT_VERBOSE, 0, "\n");
}

static
void
handle_end()
{
	lexc_printf(PRINT_DEBUG, 0, _("END\n"));
	lexc_printf(PRINT_VERBOSE, 0, _("\nExplicit END called, skipping rest\n"));
}

%}

// chee: we want to some lots of data for ddebug, nay?
%error-verbose
%locations

%union 
{
	char* name;
	int number;
}

%token <number>	ERROR METADATA_START ALPHABET_START MULTICHARS_START
	DEFINITIONS_START END_START
%token <name>	LEXICON_START LEXICON_NAME ULSTRING ENTRY_GLOSS
	METADATA_FIELDNAME METADATA_VALUE
	MULTICHAR_SYMBOL ALPHABET_SYMBOL
	DEFINITION_NAME DEFINITION_EXPRESSION
	XEROX_REGEXP

%%

LEXC_FILE: METADATA_PART ALPHABET_PART MULTICHAR_PART
			 DEFINITIONS_PART LEXICON_PART END_PART {
				handle_eof();
			}
			 ;

METADATA_PART: METADATA2 FIELD_VALUE_LIST
				 |
				 ;

METADATA2: METADATA_START {
				lexc_printf(PRINT_VERBOSE, 0, "Reading metadata:");
			}
			;

FIELD_VALUE_LIST: FIELD_VALUE_LIST FIELD_AND_VALUE
				  | FIELD_AND_VALUE
				  ;

FIELD_AND_VALUE: METADATA_FIELDNAME ':' METADATA_VALUE {
					handle_metadata($1, $3);
					free( $1);
					free( $3);
				}
				;

ALPHABET_PART: ALPHABET2 ALPHABET_SYMBOL_LIST
				|
				;

ALPHABET2: ALPHABET_START {
				if (verbosity & PRINT_VERBOSE)
				{
					lexc_print_list_end("");
					lexc_print_list_start(_("Reading alphabet"));
				}
			}
			;

ALPHABET_SYMBOL_LIST: ALPHABET_SYMBOL_LIST ALPHABET_SYMBOL2
					  | ALPHABET_SYMBOL2
					  ;

ALPHABET_SYMBOL2: ALPHABET_SYMBOL {
					handle_alphabet($1);
					free( $1);
				}

MULTICHAR_PART: MULTICHAR_SYMBOLS2 MULTICHAR_SYMBOL_LIST
				 |
				 ;

MULTICHAR_SYMBOLS2: MULTICHARS_START {
					if (verbosity & PRINT_VERBOSE)
					{
						lexc_print_list_end("");
						lexc_print_list_start(_("Reading "
												"multicharacter symbols"));
					}
				}
				;

MULTICHAR_SYMBOL_LIST: MULTICHAR_SYMBOL_LIST MULTICHAR_SYMBOL2
				 | MULTICHAR_SYMBOL2
				 ;

MULTICHAR_SYMBOL2: MULTICHAR_SYMBOL {
			handle_multichar($1);
			free( $1);
		}
		;

DEFINITIONS_PART: DEFINITIONS_START2 DEFINITION_LIST
				  |
				  ;

DEFINITIONS_START2: DEFINITIONS_START {
					if (verbosity & PRINT_VERBOSE)
					{
						lexc_print_list_end("");
						lexc_print_list_start(_("Reading definitions"));
					}
				}
				;

DEFINITION_LIST: DEFINITION_LIST DEFINITION_LINE
				  | DEFINITION_LINE
				  ;

DEFINITION_LINE: DEFINITION_NAME DEFINITION_EXPRESSION {
					handle_definition($1, $2);
					free( $1);
					free( $2);
				}
				;

LEXICON_PART: LEXICONS
		  ;

LEXICONS: LEXICONS LEXICON2 LEXICON_LINES
		  | LEXICON2 LEXICON_LINES
		  ;

LEXICON2: LEXICON_START {
			handle_lexicon_name($1);
			free( $1);
		  }
		  ;

LEXICON_LINES: LEXICON_LINES LEXICON_LINE
		  | LEXICON_LINE
		  ;

LEXICON_LINE: ULSTRING LEXICON_NAME ';' {
				handle_string_entry($1, $2, "");
				free( $1);
				free( $2);
			  }
			  | ULSTRING ':' ULSTRING
				LEXICON_NAME ';' {
				handle_string_pair_entry($1, $3, $4, "");
				free( $1);
				free( $3);
				free( $4);
			  }
			  | LEXICON_NAME ';' {
				handle_string_entry("", $1, "");
				free( $1);
			  }
			  | ULSTRING ':' LEXICON_NAME ';' {
				handle_string_pair_entry($1, "", $3, "");
				free( $1);
				free( $3);
			  }
			  | ':' ULSTRING LEXICON_NAME ';' {
				handle_string_pair_entry("", $2, $3, "");
				free( $2);
				free( $3);
			  }
			  | ':' LEXICON_NAME ';' {
				handle_string_entry("", $2, "");
				free( $2);
			  }
			  | ULSTRING LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_entry($1, $2, $3);
				free( $1);
				free( $2);
				free( $3);
			  }
			  | ULSTRING ':' ULSTRING
				LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_pair_entry($1, $3, $4, $5);
				free( $1);
				free( $3);
				free( $4);
				free( $5);
			  }
			  | LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_entry("", $1, $2);
				free( $1);
				free( $2);
			  }
			  | ULSTRING ':' LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_pair_entry($1, "", $3, $4);
				free( $1);
				free( $3);
				free( $4);
			  }
			  | ':' ULSTRING LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_pair_entry("", $2, $3, $4);
				free( $2);
				free( $3);
				free( $4);
			  }
			  | ':' LEXICON_NAME ENTRY_GLOSS ';' {
				handle_string_entry("", $2, $3);
				free( $2);
				free( $3);
			  }
			  | XEROX_REGEXP LEXICON_NAME ';' {
			  	handle_regexp_entry($1, $2, "");
				free( $1);
				free( $2);
			  }
			  | XEROX_REGEXP LEXICON_NAME ENTRY_GLOSS ';' {
			  	handle_regexp_entry($1, $2, $3);
				free( $1);
				free( $2);
				free( $3);
			  }
			  ;

END_PART: END_START { 
			handle_end();
		}
		|
		;
%%

// oblig. declarations
extern FILE* hlexcin;
int hlexcparse(void);

// gah, bison/flex error mechanism here
void
hlexcerror(const char* text)
{ 
	(void)lexc_parser_printf(PRINT_ERROR, 8, "%s", text);
}


// vim: set ft=yacc:
