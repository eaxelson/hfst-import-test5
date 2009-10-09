/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NEWLINE = 258,
     ALPHA = 259,
     EXIT = 260,
     COMPOSE = 261,
     L_COMPOSE = 262,
     PRINT_BIN = 263,
     POS = 264,
     INSERT = 265,
     EQUAL = 266,
     NOT_EQUAL = 267,
     PRINT_TEXT = 268,
     PRINT_NUM = 269,
     PRINT_BIN_COUT = 270,
     PRINT_BIN_CERR = 271,
     PRINT_TEXT_COUT = 272,
     PRINT_TEXT_CERR = 273,
     PRINT_NUM_CERR = 274,
     PRINT_NUM_COUT = 275,
     ARROW = 276,
     REPLACE = 277,
     SYMBOL = 278,
     VAR = 279,
     SVAR = 280,
     RVAR = 281,
     RSVAR = 282,
     STRING = 283,
     STRING2 = 284,
     STRING3 = 285,
     UTF8CHAR = 286,
     CHARACTER = 287,
     SEQ = 288
   };
#endif
/* Tokens.  */
#define NEWLINE 258
#define ALPHA 259
#define EXIT 260
#define COMPOSE 261
#define L_COMPOSE 262
#define PRINT_BIN 263
#define POS 264
#define INSERT 265
#define EQUAL 266
#define NOT_EQUAL 267
#define PRINT_TEXT 268
#define PRINT_NUM 269
#define PRINT_BIN_COUT 270
#define PRINT_BIN_CERR 271
#define PRINT_TEXT_COUT 272
#define PRINT_TEXT_CERR 273
#define PRINT_NUM_CERR 274
#define PRINT_NUM_COUT 275
#define ARROW 276
#define REPLACE 277
#define SYMBOL 278
#define VAR 279
#define SVAR 280
#define RVAR 281
#define RSVAR 282
#define STRING 283
#define STRING2 284
#define STRING3 285
#define UTF8CHAR 286
#define CHARACTER 287
#define SEQ 288




/* Copy the first part of user declarations.  */
#line 1 "hfst-compiler.yy"

/*******************************************************************/
/*                                                                 */
/*  FILE     hfst-compiler.yy                                      */
/*  MODULE   h(w)fst-calculate                                     */
/*  PROGRAM  HFST version 2.0                                      */
/*  AUTHOR   Erik Axelson, University of Helsinki                  */
/*  (based on Helmut Schmid's code for SFST)                       */
/*                                                                 */
/*******************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <hfst2/hfst.h>

#include "hfst-program-options.h"


#ifdef WEIGHTED

namespace HWFST {
#include "calculate_functions.h"
}

using HWFST::Key;
using HWFST::KeyPairSet;
using HWFST::ContextsHandle;
using HWFST::Range;
using HWFST::Ranges;
using HWFST::Repl_Type;
using HWFST::TransducerHandle;
using HWFST::Twol_Type;

using HWFST::append_context;
using HWFST::add_range;
using HWFST::add_value;
using HWFST::add_values;
using HWFST::add_var_values;
using HWFST::concatenate;
using HWFST::character_code;
using HWFST::complement_range;
using HWFST::compose;
using HWFST::intersect;
using HWFST::define_keypair_set;
using HWFST::define_transducer_agreement_variable;
using HWFST::define_range_variable;
using HWFST::define_transducer_variable;
using HWFST::disjunct;
using HWFST::explode;
using HWFST::insert_freely;
using HWFST::extract_input_language;
using HWFST::make_context;
using HWFST::make_mapping;
using HWFST::make_rule;
using HWFST::minimize;
using HWFST::negate;
using HWFST::new_transducer;
using HWFST::optionalize;
using HWFST::read_transducer_and_harmonize;
using HWFST::read_words;
using HWFST::repeat_star;
using HWFST::repeat_plus;
using HWFST::make_replace;
using HWFST::make_replace_in_context;
using HWFST::make_restriction;
using HWFST::result;
using HWFST::copy_range_agreement_variable_value;
using HWFST::copy_transducer_agreement_variable_value;
using HWFST::subtract;
using HWFST::copy_range_variable_value;
using HWFST::invert;
using HWFST::symbol_code;
using HWFST::extract_output_language;
using HWFST::utf8_to_int;
using HWFST::copy_transducer_variable_value;
using HWFST::write_transducer;
using HWFST::is_empty;

using HWFST::are_equivalent;
using HWFST::add_epsilon_to_alphabet;
using HWFST::read_symbol_table_text;
using HWFST::set_alphabet_defined;
using HWFST::set_ofst_symbol_table;

using HWFST::print_transducer;
using HWFST::print_transducer_number;
using HWFST::read_transducer_text;
using HWFST::write_symbol_table;
using HWFST::delete_transducer;

#else


namespace HFST {
#include "calculate_functions.h"
void store_as_compact(TransducerHandle t, FILE *file);
}

//using HFST::TheAlphabet;
using HFST::Key;
using HFST::KeyPairSet;
using HFST::ContextsHandle;
using HFST::store_as_compact;
using HFST::Range;
using HFST::Ranges;
using HFST::Repl_Type;
using HFST::TransducerHandle;
using HFST::Twol_Type;

using HFST::append_context;
using HFST::add_range;
using HFST::add_value;
using HFST::add_values;
using HFST::add_var_values;
using HFST::concatenate;
using HFST::character_code;
using HFST::complement_range;
using HFST::compose;
using HFST::intersect;
using HFST::define_keypair_set;
using HFST::define_transducer_agreement_variable;
using HFST::define_range_variable;
using HFST::define_transducer_variable;
using HFST::disjunct;
using HFST::explode;
using HFST::insert_freely;
using HFST::extract_input_language;
using HFST::make_context;
using HFST::make_mapping;
using HFST::make_rule;
using HFST::minimize;
using HFST::negate;
using HFST::new_transducer;
using HFST::optionalize;
using HFST::read_transducer_and_harmonize;
using HFST::read_words;
using HFST::repeat_star;
using HFST::repeat_plus;
using HFST::make_replace;
using HFST::make_replace_in_context;
using HFST::make_restriction;
using HFST::result;
using HFST::subtract;
using HFST::invert;
using HFST::symbol_code;
using HFST::extract_output_language;
using HFST::utf8_to_int;
using HFST::copy_transducer_variable_value;
using HFST::copy_range_variable_value;
using HFST::copy_range_agreement_variable_value;
using HFST::copy_transducer_agreement_variable_value;
using HFST::write_transducer;
using HFST::is_empty;

using HFST::are_equivalent;
using HFST::add_epsilon_to_alphabet;
using HFST::read_symbol_table_text;
using HFST::set_alphabet_defined;
using HFST::explode_and_minimise;

using HFST::print_transducer;
using HFST::print_transducer_number;
using HFST::read_transducer_text;
using HFST::write_symbol_table;
using HFST::delete_transducer;

#endif



extern bool UTF8;
extern int  yylineno;
extern char *yytext;

char *FileName;

void yyerror(char *text);
void warn(char *text, char *FileName);
void warn2(char *text, char *text2, char *FileName);
void delete_keypair_set(KeyPairSet *kps);
int yylex( void );
int yyparse( void );

static int Switch=0;
TransducerHandle Result;
KeyPairSet *Pi;




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 193 "hfst-compiler.yy"
{
  int        number;
  Twol_Type  type;
  Repl_Type  rtype;
  char       *name;
  char       *value;
  unsigned char uchar;
  unsigned int  longchar;
  Key  character;
  TransducerHandle  expression;
  Range      *range;
  Ranges     *ranges;
  ContextsHandle   contexts;
}
/* Line 187 of yacc.c.  */
#line 369 "hfst-compiler.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 382 "hfst-compiler.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1014

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  106
/* YYNRULES -- Number of states.  */
#define YYNSTATES  171

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    37,     2,     2,     2,     2,    35,     2,
      43,    44,    40,    41,    52,    34,    51,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,     2,
       2,    42,     2,    45,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,    38,    39,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    33,    47,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    36
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,    10,    13,    14,    18,    22,    26,
      30,    34,    37,    40,    44,    48,    51,    54,    57,    60,
      63,    66,    69,    73,    78,    83,    87,    92,    97,   103,
     109,   114,   119,   123,   127,   135,   141,   147,   153,   157,
     161,   163,   165,   167,   170,   173,   176,   179,   182,   186,
     189,   192,   196,   200,   204,   208,   210,   212,   214,   217,
     218,   222,   227,   231,   233,   235,   237,   241,   245,   247,
     249,   253,   257,   260,   263,   268,   272,   275,   277,   280,
     282,   285,   287,   289,   291,   293,   295,   297,   299,   301,
     303,   305,   307,   309,   311,   313,   315,   317,   319,   321,
     323,   325,   327,   329,   331,   333,   336
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    55,    57,    68,    -1,    55,    56,    -1,
      55,     3,    -1,    -1,    24,    42,    57,    -1,    26,    42,
      57,    -1,    25,    42,    64,    -1,    27,    42,    64,    -1,
      57,     8,    28,    -1,     4,    57,    -1,     4,    29,    -1,
      57,    11,    57,    -1,    57,    12,    57,    -1,    57,    15,
      -1,    57,    16,    -1,    57,    17,    -1,    57,    18,    -1,
      57,    19,    -1,    57,    20,    -1,     5,    57,    -1,    57,
      21,    60,    -1,    57,    38,    21,    60,    -1,    57,    39,
      21,    60,    -1,    57,    22,    62,    -1,    57,    22,    43,
      44,    -1,    57,    22,    45,    62,    -1,    57,    22,    45,
      43,    44,    -1,    57,    59,    21,    59,    57,    -1,    57,
      59,    21,    59,    -1,    59,    21,    59,    57,    -1,    59,
      21,    59,    -1,    57,     6,    57,    -1,    46,    58,    47,
      48,    46,    58,    47,    -1,    59,    48,    46,    58,    47,
      -1,    46,    58,    47,    48,    59,    -1,    57,    10,    66,
      48,    66,    -1,    57,    10,    66,    -1,    59,    48,    59,
      -1,    59,    -1,    24,    -1,    26,    -1,    57,    40,    -1,
      57,    41,    -1,    57,    45,    -1,    57,    57,    -1,    37,
      57,    -1,    38,    39,    57,    -1,    38,    57,    -1,    39,
      57,    -1,    57,    35,    57,    -1,    57,    34,    57,    -1,
      57,    33,    57,    -1,    43,    57,    44,    -1,    28,    -1,
      29,    -1,    30,    -1,    59,    58,    -1,    -1,    49,    64,
      50,    -1,    49,    38,    64,    50,    -1,    49,    27,    50,
      -1,    51,    -1,    66,    -1,    61,    -1,    43,    61,    44,
      -1,    63,    52,    61,    -1,    63,    -1,    63,    -1,    43,
      63,    44,    -1,    57,     9,    57,    -1,     9,    57,    -1,
      57,     9,    -1,    65,    34,    65,    64,    -1,    65,    34,
      65,    -1,    25,    64,    -1,    25,    -1,    66,    64,    -1,
      66,    -1,    67,    64,    -1,    67,    -1,    32,    -1,    67,
      -1,    31,    -1,    32,    -1,    31,    -1,    23,    -1,    51,
      -1,    37,    -1,    45,    -1,    46,    -1,    47,    -1,    44,
      -1,    43,    -1,    35,    -1,    33,    -1,    40,    -1,    41,
      -1,    48,    -1,    52,    -1,    42,    -1,    39,    -1,    38,
      -1,    34,    -1,     3,    68,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   234,   234,   237,   238,   239,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   299,   300,
     303,   304,   305,   306,   307,   310,   311,   314,   315,   318,
     319,   322,   323,   324,   327,   328,   329,   330,   331,   332,
     333,   334,   337,   338,   339,   342,   343,   344,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   366,   367
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NEWLINE", "ALPHA", "EXIT", "COMPOSE",
  "L_COMPOSE", "PRINT_BIN", "POS", "INSERT", "EQUAL", "NOT_EQUAL",
  "PRINT_TEXT", "PRINT_NUM", "PRINT_BIN_COUT", "PRINT_BIN_CERR",
  "PRINT_TEXT_COUT", "PRINT_TEXT_CERR", "PRINT_NUM_CERR", "PRINT_NUM_COUT",
  "ARROW", "REPLACE", "SYMBOL", "VAR", "SVAR", "RVAR", "RSVAR", "STRING",
  "STRING2", "STRING3", "UTF8CHAR", "CHARACTER", "'|'", "'-'", "'&'",
  "SEQ", "'!'", "'^'", "'_'", "'*'", "'+'", "'='", "'('", "')'", "'?'",
  "'{'", "'}'", "':'", "'['", "']'", "'.'", "','", "$accept", "ALL",
  "ASSIGNMENTS", "ASSIGNMENT", "RE", "RANGES", "RANGE", "CONTEXTS2",
  "CONTEXTS", "CONTEXT2", "CONTEXT", "VALUES", "LCHAR", "CODE", "SCHAR",
  "NEWLINES", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   124,    45,    38,   288,    33,    94,    95,
      42,    43,    61,    40,    41,    63,   123,   125,    58,    91,
      93,    46,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    55,    55,    55,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    58,    58,
      59,    59,    59,    59,    59,    60,    60,    61,    61,    62,
      62,    63,    63,    63,    64,    64,    64,    64,    64,    64,
      64,    64,    65,    65,    65,    66,    66,    66,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    68,    68
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     2,     2,     0,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     2,     2,     2,     2,     2,
       2,     2,     3,     4,     4,     3,     4,     4,     5,     5,
       4,     4,     3,     3,     7,     5,     5,     5,     3,     3,
       1,     1,     1,     2,     2,     2,     2,     2,     3,     2,
       2,     3,     3,     3,     3,     1,     1,     1,     2,     0,
       3,     4,     3,     1,     1,     1,     3,     3,     1,     1,
       3,     3,     2,     2,     4,     3,     2,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,     1,     4,     0,     0,    87,    41,     0,
      42,     0,    55,    56,    57,    86,    85,     0,     0,     0,
       0,    59,     0,    63,     3,   106,    40,    64,    41,    42,
      12,    11,    21,     0,     0,     0,     0,    47,     0,    49,
      50,     0,     0,    59,    77,     0,    86,    85,    96,   104,
      95,    89,   103,   102,    97,    98,   101,    94,    93,    90,
      91,    92,    99,    88,   100,     0,     0,    79,    81,   106,
       0,     0,     0,     0,     0,    15,    16,    17,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    40,     2,     0,     0,     6,   103,     8,     7,
       9,    48,    54,     0,    58,    76,    62,     0,    60,     0,
      78,    80,   105,    33,    10,    38,    13,    14,     0,     0,
       0,    22,    65,    68,     0,     0,    25,    69,    53,    52,
      51,     0,     0,     0,    32,    59,    39,     0,    61,    84,
      82,    75,    83,     0,    72,     0,     0,    73,     0,    26,
       0,     0,    27,    23,    24,    30,    31,     0,    59,    36,
      74,    37,    66,    71,    67,    70,    28,    29,    35,     0,
      34
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    24,    91,    42,    26,   121,   122,   126,
     123,    65,    66,    27,    68,    93
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -123
static const yytype_int16 yypact[] =
{
    -123,     2,   268,  -123,  -123,   869,   893,  -123,   -29,   -19,
     -18,   -17,  -123,  -123,  -123,  -123,  -123,   893,   917,   893,
     893,   116,   683,  -123,  -123,   219,   -13,  -123,  -123,  -123,
      54,   427,   427,   893,   713,   893,   713,   941,   893,   941,
     941,   345,   -27,   116,   713,   -24,    -7,    -6,  -123,  -123,
    -123,  -123,   743,  -123,  -123,  -123,  -123,  -123,  -123,  -123,
    -123,  -123,  -123,  -123,  -123,    -8,    15,   713,   713,    47,
     893,    23,     9,   893,   893,  -123,  -123,  -123,  -123,  -123,
    -123,   562,   490,   893,   893,   893,   634,   658,  -123,  -123,
    -123,   845,   -10,  -123,   116,    16,   427,  -123,  -123,   427,
    -123,   941,  -123,     6,  -123,  -123,  -123,     5,  -123,   962,
    -123,  -123,  -123,   773,  -123,     8,   427,   427,   893,   586,
     389,  -123,  -123,     7,   514,   610,  -123,  -123,   797,   821,
     845,   562,   562,   116,   893,   116,  -123,    69,  -123,  -123,
    -123,   713,  -123,     9,   427,   303,    14,   893,   586,  -123,
      17,   538,  -123,  -123,  -123,   893,   457,    19,   116,  -123,
    -123,  -123,  -123,   427,  -123,  -123,  -123,   457,  -123,    22,
    -123
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -123,  -123,  -123,  -123,    -2,   -37,    65,  -122,  -114,   -62,
     -81,   -22,   -36,   -15,   -35,    24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -85
static const yytype_int16 yytable[] =
{
      25,   127,     3,    31,    32,   146,   104,    67,    94,   153,
     154,   133,    98,    33,   100,    37,    39,    40,    41,    67,
     103,    67,   105,    34,    35,    36,   106,   -84,   -82,    67,
     107,    96,     7,    99,   164,    95,   101,    67,    95,     7,
      15,    16,   108,   150,   127,   110,   111,    15,    16,   109,
      69,   114,    67,    67,   137,   138,   143,   115,   162,   148,
     -56,   165,   135,   152,   -56,    22,   168,    23,   113,   170,
     150,   116,   117,   141,   142,   -56,   -56,     0,     0,   120,
     120,   128,   129,   130,    39,    40,    43,   -56,   -56,   -56,
      92,     0,     7,   112,   -56,   -56,    92,    92,   157,   -56,
      15,    16,    92,     0,    92,    92,    92,     0,    43,     0,
       0,     0,     0,     0,     0,   158,   144,   145,    22,   160,
      23,   169,   145,   120,     0,     0,    67,     0,   161,   120,
     120,     0,   156,     0,     0,     0,     0,     0,     0,     7,
       0,     0,     0,     0,     0,   163,   120,    15,    16,   145,
       0,     0,     0,   167,     0,     0,    92,     0,     0,   134,
     136,    92,     0,     0,    92,    22,    92,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    92,     0,
       0,    92,    92,     0,     0,    92,     0,     0,     0,     0,
       0,     0,     0,    92,    92,    92,     0,     0,   155,     0,
      43,     0,   159,     0,     0,     0,     0,     0,     0,    92,
      92,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,    69,    43,     0,    70,     0,    71,    92,    72,
      73,    74,    92,     0,    75,    76,    77,    78,    79,    80,
      81,    82,     7,    28,     0,    29,     0,    12,    13,    14,
      15,    16,    83,    84,    85,     0,    17,    86,    87,    88,
      89,     0,    20,     0,    90,    21,     0,     0,    22,     0,
      23,     4,     5,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,     0,     0,    17,    18,    19,     0,    70,
       0,    20,   147,    72,    21,     0,     0,    22,     0,    23,
       0,     0,     0,     0,    81,    82,     7,    28,     0,    29,
       0,    12,    13,    14,    15,    16,    83,    84,    85,     0,
      17,    86,    87,    88,    89,     0,    20,   102,    90,    21,
       0,    70,    22,     0,    23,    72,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    81,    82,     7,    28,
       0,    29,     0,    12,    13,    14,    15,    16,    83,    84,
      85,     0,    17,    86,    87,    88,    89,     0,    20,   102,
      90,    21,     0,     0,    22,    70,    23,     0,   147,    72,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      81,    82,     7,    28,     0,    29,     0,    12,    13,    14,
      15,    16,    83,    84,    85,     0,    17,    86,    87,    88,
      89,     0,    20,    70,    90,    21,     0,    72,    22,     0,
      23,     0,     0,     0,     0,     0,     0,     0,    81,    82,
       7,    28,     0,    29,     0,    12,    13,    14,    15,    16,
      83,    84,    85,    70,    17,    86,    87,    88,    89,     0,
      20,     0,    90,    21,     0,     0,    22,     0,    23,     0,
       7,    28,     0,    29,     0,    12,    13,    14,    15,    16,
      83,    84,    85,     0,    17,    86,    87,    88,    89,   118,
      20,     0,    90,    21,     0,     0,    22,     0,    23,     0,
       0,     0,     0,     7,    28,     0,    29,     0,    12,    13,
      14,    15,    16,   118,     0,     0,     0,    17,    18,    19,
       0,     0,     0,   124,     0,   125,    21,     7,    28,    22,
      29,    23,    12,    13,    14,    15,    16,   118,     0,     0,
       0,    17,    18,    19,     0,     0,     0,    20,   149,     0,
      21,     7,    28,    22,    29,    23,    12,    13,    14,    15,
      16,   118,     0,     0,     0,    17,    18,    19,     0,     0,
       0,    20,   166,     0,    21,     7,    28,    22,    29,    23,
      12,    13,    14,    15,    16,   118,     0,     0,     0,    17,
      18,    19,     0,     0,     0,   119,     0,     0,    21,     7,
      28,    22,    29,    23,    12,    13,    14,    15,    16,   118,
       0,     0,     0,    17,    18,    19,     0,     0,     0,    20,
       0,     0,    21,     7,    28,    22,    29,    23,    12,    13,
      14,    15,    16,     0,     0,     0,     0,    17,    18,    19,
       0,     0,     0,   151,     0,   131,    21,     7,    28,    22,
      29,    23,    12,    13,    14,    15,    16,     0,     0,     0,
       0,    17,    18,    38,     0,     0,     0,    20,     0,   132,
      21,     7,    28,    22,    29,    23,    12,    13,    14,    15,
      16,     0,     0,     0,     0,    17,    18,    19,     0,     0,
       0,    20,     0,     0,    21,     0,     7,    22,    44,    23,
      45,     0,     0,     0,    46,    47,    48,    49,    50,     0,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,     0,     0,    63,    64,     7,     0,    44,     0,
       0,     0,     0,     0,    46,    47,    48,    49,    50,     0,
      51,    97,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,     0,     0,    63,    64,     7,     0,    44,     0,
       0,     0,     0,     0,    46,    47,     0,     0,     0,     0,
       0,     0,     0,    54,    55,    56,    57,    58,    59,    60,
      61,    62,     0,     0,    63,    64,     7,    28,     0,    29,
       0,    12,    13,    14,    15,    16,    83,    84,    85,     0,
      17,    86,    87,    88,    89,     0,    20,     0,    90,    21,
       7,    28,    22,    29,    23,    12,    13,    14,    15,    16,
       0,    84,    85,     0,    17,    86,    87,    88,    89,     0,
      20,     0,    90,    21,     7,    28,    22,    29,    23,    12,
      13,    14,    15,    16,     0,     0,    85,     0,    17,    86,
      87,    88,    89,     0,    20,     0,    90,    21,     7,    28,
      22,    29,    23,    12,    13,    14,    15,    16,     0,     0,
       0,     0,    17,    86,    87,    88,    89,     0,    20,     0,
      90,    21,     7,    28,    22,    29,    23,    12,    30,    14,
      15,    16,     0,     0,     0,     0,    17,    18,    19,     0,
       0,     0,    20,     0,     0,    21,     7,    28,    22,    29,
      23,    12,    13,    14,    15,    16,     0,     0,     0,     0,
      17,    18,    19,     0,     0,     0,    20,     0,     0,    21,
       7,    28,    22,    29,    23,    12,    13,    14,    15,    16,
       0,     0,     0,     0,    17,    18,    38,     0,     0,     0,
      20,     0,     0,    21,     7,    28,    22,    29,    23,    12,
      13,    14,    15,    16,     0,     0,     0,     0,     0,     0,
       0,    88,    89,     0,    20,     0,    90,    21,     0,     0,
      22,     0,    23,   139,   140,    48,    49,    50,     0,    51,
      97,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,     0,     0,    63,    64
};

static const yytype_int16 yycheck[] =
{
       2,    82,     0,     5,     6,   119,    43,    22,    21,   131,
     132,    21,    34,    42,    36,    17,    18,    19,    20,    34,
      47,    36,    44,    42,    42,    42,    50,    34,    34,    44,
      52,    33,    23,    35,   148,    48,    38,    52,    48,    23,
      31,    32,    50,   124,   125,    67,    68,    31,    32,    34,
       3,    28,    67,    68,    48,    50,    48,    72,    44,    52,
       6,    44,    46,   125,    10,    49,    47,    51,    70,    47,
     151,    73,    74,   109,   109,    21,    22,    -1,    -1,    81,
      82,    83,    84,    85,    86,    87,    21,    33,    34,    35,
      25,    -1,    23,    69,    40,    41,    31,    32,   135,    45,
      31,    32,    37,    -1,    39,    40,    41,    -1,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    46,   118,   119,    49,   141,
      51,   158,   124,   125,    -1,    -1,   141,    -1,   143,   131,
     132,    -1,   134,    -1,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,   147,   148,    31,    32,   151,
      -1,    -1,    -1,   155,    -1,    -1,    91,    -1,    -1,    94,
      95,    96,    -1,    -1,    99,    49,   101,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,
      -1,   116,   117,    -1,    -1,   120,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,   129,   130,    -1,    -1,   133,    -1,
     135,    -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,   144,
     145,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   156,     3,   158,    -1,     6,    -1,     8,   163,    10,
      11,    12,   167,    -1,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    38,    39,    40,
      41,    -1,    43,    -1,    45,    46,    -1,    -1,    49,    -1,
      51,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    37,    38,    39,    -1,     6,
      -1,    43,     9,    10,    46,    -1,    -1,    49,    -1,    51,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    -1,    26,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    -1,    43,    44,    45,    46,
      -1,     6,    49,    -1,    51,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,
      -1,    26,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    38,    39,    40,    41,    -1,    43,    44,
      45,    46,    -1,    -1,    49,     6,    51,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      21,    22,    23,    24,    -1,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    38,    39,    40,
      41,    -1,    43,     6,    45,    46,    -1,    10,    49,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    22,
      23,    24,    -1,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,     6,    37,    38,    39,    40,    41,    -1,
      43,    -1,    45,    46,    -1,    -1,    49,    -1,    51,    -1,
      23,    24,    -1,    26,    -1,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    38,    39,    40,    41,     9,
      43,    -1,    45,    46,    -1,    -1,    49,    -1,    51,    -1,
      -1,    -1,    -1,    23,    24,    -1,    26,    -1,    28,    29,
      30,    31,    32,     9,    -1,    -1,    -1,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    45,    46,    23,    24,    49,
      26,    51,    28,    29,    30,    31,    32,     9,    -1,    -1,
      -1,    37,    38,    39,    -1,    -1,    -1,    43,    44,    -1,
      46,    23,    24,    49,    26,    51,    28,    29,    30,    31,
      32,     9,    -1,    -1,    -1,    37,    38,    39,    -1,    -1,
      -1,    43,    44,    -1,    46,    23,    24,    49,    26,    51,
      28,    29,    30,    31,    32,     9,    -1,    -1,    -1,    37,
      38,    39,    -1,    -1,    -1,    43,    -1,    -1,    46,    23,
      24,    49,    26,    51,    28,    29,    30,    31,    32,     9,
      -1,    -1,    -1,    37,    38,    39,    -1,    -1,    -1,    43,
      -1,    -1,    46,    23,    24,    49,    26,    51,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    21,    46,    23,    24,    49,
      26,    51,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    37,    38,    39,    -1,    -1,    -1,    43,    -1,    21,
      46,    23,    24,    49,    26,    51,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    37,    38,    39,    -1,    -1,
      -1,    43,    -1,    -1,    46,    -1,    23,    49,    25,    51,
      27,    -1,    -1,    -1,    31,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    51,    52,    23,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    51,    52,    23,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    31,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    51,    52,    23,    24,    -1,    26,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    -1,    43,    -1,    45,    46,
      23,    24,    49,    26,    51,    28,    29,    30,    31,    32,
      -1,    34,    35,    -1,    37,    38,    39,    40,    41,    -1,
      43,    -1,    45,    46,    23,    24,    49,    26,    51,    28,
      29,    30,    31,    32,    -1,    -1,    35,    -1,    37,    38,
      39,    40,    41,    -1,    43,    -1,    45,    46,    23,    24,
      49,    26,    51,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    37,    38,    39,    40,    41,    -1,    43,    -1,
      45,    46,    23,    24,    49,    26,    51,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    37,    38,    39,    -1,
      -1,    -1,    43,    -1,    -1,    46,    23,    24,    49,    26,
      51,    28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,
      37,    38,    39,    -1,    -1,    -1,    43,    -1,    -1,    46,
      23,    24,    49,    26,    51,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    37,    38,    39,    -1,    -1,    -1,
      43,    -1,    -1,    46,    23,    24,    49,    26,    51,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    43,    -1,    45,    46,    -1,    -1,
      49,    -1,    51,    31,    32,    33,    34,    35,    -1,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    -1,    -1,    51,    52
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,     0,     3,     4,     5,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    37,    38,    39,
      43,    46,    49,    51,    56,    57,    59,    66,    24,    26,
      29,    57,    57,    42,    42,    42,    42,    57,    39,    57,
      57,    57,    58,    59,    25,    27,    31,    32,    33,    34,
      35,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    51,    52,    64,    65,    66,    67,     3,
       6,     8,    10,    11,    12,    15,    16,    17,    18,    19,
      20,    21,    22,    33,    34,    35,    38,    39,    40,    41,
      45,    57,    59,    68,    21,    48,    57,    38,    64,    57,
      64,    57,    44,    47,    58,    64,    50,    64,    50,    34,
      64,    64,    68,    57,    28,    66,    57,    57,     9,    43,
      57,    60,    61,    63,    43,    45,    62,    63,    57,    57,
      57,    21,    21,    21,    59,    46,    59,    48,    50,    31,
      32,    65,    67,    48,    57,    57,    61,     9,    52,    44,
      63,    43,    62,    60,    60,    59,    57,    58,    46,    59,
      64,    66,    44,    57,    61,    44,    44,    57,    47,    58,
      47
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 234 "hfst-compiler.yy"
    { Result=result((yyvsp[(2) - (3)].expression), Switch); }
    break;

  case 3:
#line 237 "hfst-compiler.yy"
    {}
    break;

  case 4:
#line 238 "hfst-compiler.yy"
    {}
    break;

  case 5:
#line 239 "hfst-compiler.yy"
    {}
    break;

  case 6:
#line 242 "hfst-compiler.yy"
    { if (define_transducer_variable((yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].expression))) warn2((char*)"assignment of empty transducer to",(yyvsp[(1) - (3)].name),FileName); }
    break;

  case 7:
#line 243 "hfst-compiler.yy"
    { if (define_transducer_agreement_variable((yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].expression))) warn2((char*)"assignment of empty transducer to",(yyvsp[(1) - (3)].name),FileName); }
    break;

  case 8:
#line 244 "hfst-compiler.yy"
    { if (define_range_variable((yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].range))) warn2((char*)"assignment of empty symbol range to",(yyvsp[(1) - (3)].name),FileName); }
    break;

  case 9:
#line 245 "hfst-compiler.yy"
    { if (define_range_variable((yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].range))) warn2((char*)"assignment of empty symbol range to",(yyvsp[(1) - (3)].name),FileName); }
    break;

  case 10:
#line 246 "hfst-compiler.yy"
    { write_transducer((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].value), NULL, false); delete_transducer((yyvsp[(1) - (3)].expression)); }
    break;

  case 11:
#line 247 "hfst-compiler.yy"
    { delete_keypair_set(Pi); Pi = define_keypair_set((yyvsp[(2) - (2)].expression)); delete_transducer((yyvsp[(2) - (2)].expression)); set_alphabet_defined(1);  }
    break;

  case 12:
#line 248 "hfst-compiler.yy"
    { delete_keypair_set(Pi); TransducerHandle tr = read_transducer_and_harmonize((yyvsp[(2) - (2)].value)); Pi = define_keypair_set(tr); delete_transducer(tr); set_alphabet_defined(1); }
    break;

  case 13:
#line 249 "hfst-compiler.yy"
    { if (!are_equivalent((yyvsp[(1) - (3)].expression),(yyvsp[(3) - (3)].expression))) warn((char*)"non-equivalent expressions",FileName); delete_transducer((yyvsp[(1) - (3)].expression)); delete_transducer((yyvsp[(3) - (3)].expression)); }
    break;

  case 14:
#line 250 "hfst-compiler.yy"
    { if (are_equivalent((yyvsp[(1) - (3)].expression),(yyvsp[(3) - (3)].expression))) warn((char*)"equivalent expressions",FileName); delete_transducer((yyvsp[(1) - (3)].expression)); delete_transducer((yyvsp[(3) - (3)].expression)); }
    break;

  case 15:
#line 251 "hfst-compiler.yy"
    { write_transducer((yyvsp[(1) - (2)].expression),cout); }
    break;

  case 16:
#line 252 "hfst-compiler.yy"
    { write_transducer((yyvsp[(1) - (2)].expression),cerr); }
    break;

  case 17:
#line 253 "hfst-compiler.yy"
    { fprintf(stdout,"\n"); print_transducer((yyvsp[(1) - (2)].expression),NULL,true,cout); }
    break;

  case 18:
#line 254 "hfst-compiler.yy"
    { fprintf(stderr,"\n"); print_transducer((yyvsp[(1) - (2)].expression),NULL,true,cerr); }
    break;

  case 19:
#line 255 "hfst-compiler.yy"
    { fprintf(stderr,"\n"); print_transducer_number((yyvsp[(1) - (2)].expression),true,cerr); }
    break;

  case 20:
#line 256 "hfst-compiler.yy"
    { fprintf(stderr,"\n"); print_transducer_number((yyvsp[(1) - (2)].expression),true,cout); }
    break;

  case 21:
#line 257 "hfst-compiler.yy"
    { Result=result((yyvsp[(2) - (2)].expression), Switch); YYACCEPT; }
    break;

  case 22:
#line 260 "hfst-compiler.yy"
    { (yyval.expression) = make_restriction((yyvsp[(1) - (3)].expression),(yyvsp[(2) - (3)].type),(yyvsp[(3) - (3)].contexts),0,Pi); }
    break;

  case 23:
#line 261 "hfst-compiler.yy"
    { (yyval.expression) = make_restriction((yyvsp[(1) - (4)].expression),(yyvsp[(3) - (4)].type),(yyvsp[(4) - (4)].contexts),1,Pi); }
    break;

  case 24:
#line 262 "hfst-compiler.yy"
    { (yyval.expression) = make_restriction((yyvsp[(1) - (4)].expression),(yyvsp[(3) - (4)].type),(yyvsp[(4) - (4)].contexts),-1,Pi); }
    break;

  case 25:
#line 263 "hfst-compiler.yy"
    { (yyval.expression) = make_replace_in_context(minimize(explode((yyvsp[(1) - (3)].expression))),(yyvsp[(2) - (3)].rtype),(yyvsp[(3) - (3)].contexts),false,Pi); }
    break;

  case 26:
#line 264 "hfst-compiler.yy"
    { (yyval.expression) = make_replace((yyvsp[(1) - (4)].expression), (yyvsp[(2) - (4)].rtype), false, Pi); }
    break;

  case 27:
#line 265 "hfst-compiler.yy"
    { (yyval.expression) = make_replace_in_context(minimize(explode((yyvsp[(1) - (4)].expression))),(yyvsp[(2) - (4)].rtype),(yyvsp[(4) - (4)].contexts),true,Pi);}
    break;

  case 28:
#line 266 "hfst-compiler.yy"
    { (yyval.expression) = make_replace((yyvsp[(1) - (5)].expression), (yyvsp[(2) - (5)].rtype), true, Pi); }
    break;

  case 29:
#line 267 "hfst-compiler.yy"
    { (yyval.expression) = make_rule((yyvsp[(1) - (5)].expression),(yyvsp[(2) - (5)].range),(yyvsp[(3) - (5)].type),(yyvsp[(4) - (5)].range),(yyvsp[(5) - (5)].expression),Pi); }
    break;

  case 30:
#line 268 "hfst-compiler.yy"
    { (yyval.expression) = make_rule((yyvsp[(1) - (4)].expression),(yyvsp[(2) - (4)].range),(yyvsp[(3) - (4)].type),(yyvsp[(4) - (4)].range),NULL,Pi); }
    break;

  case 31:
#line 269 "hfst-compiler.yy"
    { (yyval.expression) = make_rule(NULL,(yyvsp[(1) - (4)].range),(yyvsp[(2) - (4)].type),(yyvsp[(3) - (4)].range),(yyvsp[(4) - (4)].expression),Pi); }
    break;

  case 32:
#line 270 "hfst-compiler.yy"
    { (yyval.expression) = make_rule(NULL,(yyvsp[(1) - (3)].range),(yyvsp[(2) - (3)].type),(yyvsp[(3) - (3)].range),NULL,Pi); }
    break;

  case 33:
#line 271 "hfst-compiler.yy"
    { (yyval.expression) = compose((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); }
    break;

  case 34:
#line 273 "hfst-compiler.yy"
    { (yyval.expression) = make_mapping((yyvsp[(2) - (7)].ranges),(yyvsp[(6) - (7)].ranges)); }
    break;

  case 35:
#line 274 "hfst-compiler.yy"
    { (yyval.expression) = make_mapping(add_range((yyvsp[(1) - (5)].range),NULL),(yyvsp[(4) - (5)].ranges)); }
    break;

  case 36:
#line 275 "hfst-compiler.yy"
    { (yyval.expression) = make_mapping((yyvsp[(2) - (5)].ranges),add_range((yyvsp[(5) - (5)].range),NULL)); }
    break;

  case 37:
#line 276 "hfst-compiler.yy"
    { (yyval.expression) = insert_freely((yyvsp[(1) - (5)].expression), (yyvsp[(3) - (5)].character), (yyvsp[(5) - (5)].character)); }
    break;

  case 38:
#line 277 "hfst-compiler.yy"
    { (yyval.expression) = insert_freely((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].character), (yyvsp[(3) - (3)].character)); }
    break;

  case 39:
#line 278 "hfst-compiler.yy"
    { (yyval.expression) = new_transducer((yyvsp[(1) - (3)].range),(yyvsp[(3) - (3)].range),Pi); }
    break;

  case 40:
#line 279 "hfst-compiler.yy"
    { (yyval.expression) = new_transducer((yyvsp[(1) - (1)].range),(yyvsp[(1) - (1)].range),Pi); }
    break;

  case 41:
#line 280 "hfst-compiler.yy"
    { (yyval.expression) = copy_transducer_variable_value((yyvsp[(1) - (1)].name)); }
    break;

  case 42:
#line 281 "hfst-compiler.yy"
    { (yyval.expression) = copy_transducer_agreement_variable_value((yyvsp[(1) - (1)].name)); }
    break;

  case 43:
#line 282 "hfst-compiler.yy"
    { (yyval.expression) = repeat_star((yyvsp[(1) - (2)].expression)); }
    break;

  case 44:
#line 283 "hfst-compiler.yy"
    { (yyval.expression) = repeat_plus((yyvsp[(1) - (2)].expression)); }
    break;

  case 45:
#line 284 "hfst-compiler.yy"
    { (yyval.expression) = optionalize((yyvsp[(1) - (2)].expression)); }
    break;

  case 46:
#line 285 "hfst-compiler.yy"
    { (yyval.expression) = concatenate((yyvsp[(1) - (2)].expression), (yyvsp[(2) - (2)].expression)); }
    break;

  case 47:
#line 286 "hfst-compiler.yy"
    { (yyval.expression) = ::negate((yyvsp[(2) - (2)].expression),Pi); }
    break;

  case 48:
#line 287 "hfst-compiler.yy"
    { (yyval.expression) = invert((yyvsp[(3) - (3)].expression)); }
    break;

  case 49:
#line 288 "hfst-compiler.yy"
    { (yyval.expression) = extract_output_language((yyvsp[(2) - (2)].expression)); }
    break;

  case 50:
#line 289 "hfst-compiler.yy"
    { (yyval.expression) = extract_input_language((yyvsp[(2) - (2)].expression)); }
    break;

  case 51:
#line 290 "hfst-compiler.yy"
    { (yyval.expression) = intersect((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); }
    break;

  case 52:
#line 291 "hfst-compiler.yy"
    { (yyval.expression) = subtract((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); }
    break;

  case 53:
#line 292 "hfst-compiler.yy"
    { (yyval.expression) = disjunct((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); }
    break;

  case 54:
#line 293 "hfst-compiler.yy"
    { (yyval.expression) = (yyvsp[(2) - (3)].expression); }
    break;

  case 55:
#line 294 "hfst-compiler.yy"
    { (yyval.expression) = read_words((yyvsp[(1) - (1)].value)); }
    break;

  case 56:
#line 295 "hfst-compiler.yy"
    { (yyval.expression) = read_transducer_and_harmonize((yyvsp[(1) - (1)].value)); }
    break;

  case 57:
#line 296 "hfst-compiler.yy"
    {}
    break;

  case 58:
#line 299 "hfst-compiler.yy"
    { (yyval.ranges) = add_range((yyvsp[(1) - (2)].range),(yyvsp[(2) - (2)].ranges)); }
    break;

  case 59:
#line 300 "hfst-compiler.yy"
    { (yyval.ranges) = NULL; }
    break;

  case 60:
#line 303 "hfst-compiler.yy"
    { (yyval.range)=(yyvsp[(2) - (3)].range); }
    break;

  case 61:
#line 304 "hfst-compiler.yy"
    { (yyval.range)=complement_range((yyvsp[(3) - (4)].range)); }
    break;

  case 62:
#line 305 "hfst-compiler.yy"
    { (yyval.range)=copy_range_agreement_variable_value((yyvsp[(2) - (3)].name)); }
    break;

  case 63:
#line 306 "hfst-compiler.yy"
    { (yyval.range)=NULL; }
    break;

  case 64:
#line 307 "hfst-compiler.yy"
    { (yyval.range)=add_value((yyvsp[(1) - (1)].character),NULL); }
    break;

  case 65:
#line 310 "hfst-compiler.yy"
    { (yyval.contexts) = (yyvsp[(1) - (1)].contexts); }
    break;

  case 66:
#line 311 "hfst-compiler.yy"
    { (yyval.contexts) = (yyvsp[(2) - (3)].contexts); }
    break;

  case 67:
#line 314 "hfst-compiler.yy"
    { (yyval.contexts) = append_context((yyvsp[(1) - (3)].contexts),(yyvsp[(3) - (3)].contexts)); }
    break;

  case 68:
#line 315 "hfst-compiler.yy"
    { (yyval.contexts) = (yyvsp[(1) - (1)].contexts); }
    break;

  case 69:
#line 318 "hfst-compiler.yy"
    { (yyval.contexts) = (yyvsp[(1) - (1)].contexts); }
    break;

  case 70:
#line 319 "hfst-compiler.yy"
    { (yyval.contexts) = (yyvsp[(2) - (3)].contexts); }
    break;

  case 71:
#line 322 "hfst-compiler.yy"
    { (yyval.contexts) = make_context((yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); }
    break;

  case 72:
#line 323 "hfst-compiler.yy"
    { (yyval.contexts) = make_context(NULL, (yyvsp[(2) - (2)].expression)); }
    break;

  case 73:
#line 324 "hfst-compiler.yy"
    { (yyval.contexts) = make_context((yyvsp[(1) - (2)].expression), NULL); }
    break;

  case 74:
#line 327 "hfst-compiler.yy"
    { (yyval.range)=add_values((yyvsp[(1) - (4)].longchar),(yyvsp[(3) - (4)].longchar),(yyvsp[(4) - (4)].range)); }
    break;

  case 75:
#line 328 "hfst-compiler.yy"
    { (yyval.range)=add_values((yyvsp[(1) - (3)].longchar),(yyvsp[(3) - (3)].longchar),NULL); }
    break;

  case 76:
#line 329 "hfst-compiler.yy"
    { (yyval.range)=add_var_values((yyvsp[(1) - (2)].name),(yyvsp[(2) - (2)].range)); }
    break;

  case 77:
#line 330 "hfst-compiler.yy"
    { (yyval.range)=copy_range_variable_value((yyvsp[(1) - (1)].name)); }
    break;

  case 78:
#line 331 "hfst-compiler.yy"
    { (yyval.range)=add_value((yyvsp[(1) - (2)].character),(yyvsp[(2) - (2)].range)); }
    break;

  case 79:
#line 332 "hfst-compiler.yy"
    { (yyval.range)=add_value((yyvsp[(1) - (1)].character),NULL); }
    break;

  case 80:
#line 333 "hfst-compiler.yy"
    { (yyval.range)=add_value((yyvsp[(1) - (2)].uchar),(yyvsp[(2) - (2)].range)); }
    break;

  case 81:
#line 334 "hfst-compiler.yy"
    { (yyval.range)=add_value((yyvsp[(1) - (1)].uchar),NULL); }
    break;

  case 82:
#line 337 "hfst-compiler.yy"
    { (yyval.longchar)=(yyvsp[(1) - (1)].uchar); }
    break;

  case 83:
#line 338 "hfst-compiler.yy"
    { (yyval.longchar)=(yyvsp[(1) - (1)].uchar); }
    break;

  case 84:
#line 339 "hfst-compiler.yy"
    { (yyval.longchar)=utf8_to_int((yyvsp[(1) - (1)].value)); }
    break;

  case 85:
#line 342 "hfst-compiler.yy"
    { (yyval.character)=character_code((yyvsp[(1) - (1)].uchar)); }
    break;

  case 86:
#line 343 "hfst-compiler.yy"
    { (yyval.character)=symbol_code(strdup((yyvsp[(1) - (1)].value))); }
    break;

  case 87:
#line 344 "hfst-compiler.yy"
    { (yyval.character)=symbol_code((yyvsp[(1) - (1)].name)); }
    break;

  case 88:
#line 347 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('.'); }
    break;

  case 89:
#line 348 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('!'); }
    break;

  case 90:
#line 349 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('?'); }
    break;

  case 91:
#line 350 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('{'); }
    break;

  case 92:
#line 351 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('}'); }
    break;

  case 93:
#line 352 "hfst-compiler.yy"
    { (yyval.uchar)=character_code(')'); }
    break;

  case 94:
#line 353 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('('); }
    break;

  case 95:
#line 354 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('&'); }
    break;

  case 96:
#line 355 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('|'); }
    break;

  case 97:
#line 356 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('*'); }
    break;

  case 98:
#line 357 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('+'); }
    break;

  case 99:
#line 358 "hfst-compiler.yy"
    { (yyval.uchar)=character_code(':'); }
    break;

  case 100:
#line 359 "hfst-compiler.yy"
    { (yyval.uchar)=character_code(','); }
    break;

  case 101:
#line 360 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('='); }
    break;

  case 102:
#line 361 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('_'); }
    break;

  case 103:
#line 362 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('^'); }
    break;

  case 104:
#line 363 "hfst-compiler.yy"
    { (yyval.uchar)=character_code('-'); }
    break;

  case 105:
#line 366 "hfst-compiler.yy"
    {}
    break;

  case 106:
#line 367 "hfst-compiler.yy"
    {}
    break;


/* Line 1267 of yacc.c.  */
#line 2431 "hfst-compiler.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 370 "hfst-compiler.yy"


extern FILE *yyin;
static int Compact=0;
static int LowMem=0;
bool Verb=false;
char *input_filename=NULL;
char *output_filename=NULL;
char *store_filename=NULL;  // where symbols table is stored
char *read_filename=NULL;   // where symbol table is read
bool store_symbols=true;
bool backwards_compatible=false;

void print_usage(const char *program_name);
void print_version(const char* program_name);



void yyerror(char *text) {
  printf("\nInputfile: %i: %s at: %s \naborted.\n", yylineno, text, yytext);
  exit(1);
}

void warn(char *text, char *FileName) {
  fprintf(stderr, "\n%s: %i: warning: %s!\n", FileName, yylineno, text);
}

void warn2(char *text, char *text2, char *FileName) {
  fprintf(stderr, "\n%s: %i: warning: %s!: %s\n", FileName, yylineno, text, text2);
}

void delete_keypair_set(KeyPairSet *kps) {
    if (kps != NULL) { 
      for (KeyPairSet::iterator it=kps->begin(); it!=kps->end(); it++)
        delete(*it);      
      kps->clear(); 
      delete kps;
    }
}  


/*FA****************************************************************/
/*                                                                 */
/*  get_flags                                                      */
/*                                                                 */
/*FE****************************************************************/

void get_flags( int *argc, char **argv )

{
  for( int i=1; i<*argc; i++ ) {
    if (strcmp(argv[i],"-h") == 0 ||
	strcmp(argv[i],"--help") == 0 ) {
      print_usage(argv[0]);
      exit(0);
    }
    if (strcmp(argv[i],"-q") == 0 ||
	strcmp(argv[i],"--quiet") == 0 ||
	strcmp(argv[i],"-s") == 0 ||
	strcmp(argv[i],"--silent") == 0 ) {
      Verb=false;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-v") == 0 ||
	     strcmp(argv[i],"--verbose") == 0 ) {
      Verb=true;
    }
    else if (strcmp(argv[i],"-V") == 0 ||
	     strcmp(argv[i],"--version") == 0 ) {
      print_version(argv[0]);
      exit(0);
    }
    else if (strcmp(argv[i],"--compact") == 0 ||
	     strcmp(argv[i],"-c") == 0 ) {
      Compact = 1;
      Switch = !Switch;  // Look-up is done from output to input
		         // in compact transducers (Added by Erik Axelson)
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"-l") == 0) {  // not implemented
      LowMem = 1;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--invert") == 0 ||
	     strcmp(argv[i],"-I") == 0 ) {
      Switch = !Switch;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--do-not-write-symbols") == 0 ||
	     strcmp(argv[i],"-D") == 0 ) {
      store_symbols=false;
      argv[i] = NULL;
    }
    else if (strcmp(argv[i],"--compatible") == 0 ||
	     strcmp(argv[i],"-C") == 0 ) {
      backwards_compatible=true;
      argv[i] = NULL;
    }
    else if (strncmp(argv[i],"--input=", (size_t)8) == 0) {
        input_filename = &(argv[i][8]);
	//fprintf(stderr, "\ninput_file: %s\n", input_filename);
    }
    else if (strncmp(argv[i],"--write-symbols-to=", (size_t)19) == 0) {
        store_filename = &(argv[i][19]);
	//fprintf(stderr, "\nstore alpha_file: %s\n", store_filename);
    }
    else if (strncmp(argv[i],"--read-symbols=", (size_t)15) == 0 ) {
        read_filename = &(argv[i][15]);
	//fprintf(stderr, "\nread alpha_file: %s\n", read_alphabet_filename);
    }
    else if (strncmp(argv[i],"--output=", (size_t)9) == 0) {
        output_filename = &(argv[i][9]);
	//fprintf(stderr, "\noutput_file: %s\n", output_filename); 
    }
    else if (strcmp(argv[i],"-i") == 0 || 
    strcmp(argv[i],"--input") == 0) {
      if (i+1 < *argc) {
        input_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-W") == 0 || 
    strcmp(argv[i],"--write-symbols-to") == 0) {
      if (i+1 < *argc) {
        store_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-R") == 0 ||
    strcmp(argv[i],"--read-symbols") == 0 ) {
      if (i+1 < *argc) {
        read_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
 	exit(0);
      }
    }
    else if (strcmp(argv[i],"-o") == 0 ||
    strcmp(argv[i],"--output") == 0 ) {
      if (i+1 < *argc) {
        output_filename = argv[i+1];
	i++;
      }
      else {
        print_usage(argv[0]);
        exit(0);
      }
    }
    else
      input_filename = argv[i];
  }
  // remove flags from the argument list
  int k;
  for( int i=k=1; i<*argc; i++)
    if (argv[i] != NULL)
      argv[k++] = argv[i];
  *argc = k;
}





void print_usage(const char *program_name) {

	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dhelp
	fprintf(stdout, "Usage: %s [OPTIONS...] [INFILE]\n"
#ifdef WEIGHTED
		   "Create a weighted transducer as defined in SFST calculus\n"
#else
		   "Create an unweighted transducer as defined in SFST calculus\n"
#endif
		   "\n", program_name);
print_common_program_options(stdout);
print_common_unary_program_options(stdout);


#               if DEBUG
	fprintf(stdout,	"%-35s%s", "  -d, --debug", "Print debugging messages and results\n");
#               endif
#ifndef WEIGHTED
        fprintf(stdout,	"%-35s%s", "  -C, --compatible", "Write the result in SFST compatible format.\n");
#else
	fprintf(stdout,	"%-35s%s", "  -C, --compatible", "Write the result in OpenFst compatible format.\n");
#endif 
       fprintf(stdout, "%-35s%s", "  -I, --invert", "Invert the result before writing.\n");
#ifndef WEIGHTED
       fprintf(stdout, "%-35s%s", "  -c, --compact", "Write the result in SFST compact format.\n");
       fprintf(stdout, "%-35s%s", "",               "(Not supported by HFST tools)\n"
);
#endif
		   fprintf(stdout, "\n"
		   "If OUTFILE or INFILE is missing or -,"
		   "standard streams will be used.\n"
		   "If no symbol table file is given, the program automatically creates one as it encounters new symbols.\n"
		   );
		   fprintf(stdout, "\n");
		   print_more_info(stdout, "Calculate");
		   fprintf(stdout, "\n");
		   print_report_bugs(stdout);
}


void print_version(const char* program_name) {

	// c.f. http://www.gnu.org/prep/standards/standards.html#g_t_002d_002dversion
	fprintf(stdout, "%s 0.1 (" PACKAGE_STRING ")\n"
		   "copyright (C) 2008 University of Helsinki,\n"
		   "License GPLv3: GNU GPL version 3 "
		   "<http://gnu.org/licenses/gpl.html>\n"
		   "This is free software: you are free to change and redistribute it.\n"
		   "There is NO WARRANTY, to the extent permitted by law.\n",
		program_name);
}



/*FA****************************************************************/
/*                                                                 */
/*  main                                                           */
/*                                                                 */
/*FE****************************************************************/

int main( int argc, char *argv[] )

{

  get_flags(&argc, argv);


#ifdef WEIGHTED
  if (backwards_compatible && store_symbols) {
    store_symbols=false;
  }
#endif

  FILE *input_file=NULL;

  if (input_filename == NULL || strcmp(input_filename, "-") == 0 ) {
    FileName = (char*)"stdin";
    yyin = stdin;	
  }
  else {
    input_file = fopen(input_filename,"rt");
    if (input_file == NULL) {
      fprintf(stderr,"\nError: Cannot open grammar file \"%s\"\n\n", input_filename);
      exit(1);
    }
    else {
      FileName = input_filename;
      yyin = input_file;
    }
  }

  FILE *output_file=NULL;
  if (output_filename != NULL) {
    output_file = fopen(output_filename,"w");
    if (output_file == NULL) {
      fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", output_filename);
      exit(1);
    }
    fclose(output_file);
  }
  Result = NULL;

  add_epsilon_to_alphabet();
  if (read_filename != NULL) {
    ifstream is(read_filename);
    if (Verb)
      fprintf(stderr, "reading symbol table from \"%s\"\n", read_filename);
    read_symbol_table_text(is);	
    is.close();	
  }
  set_alphabet_defined(0);	

  try {
    yyparse();

    if (Verb && is_empty(Result)) {
      warn((char*)"resulting transducer is empty",FileName);
    } 

#ifndef WEIGHTED 
    if (Compact) {
      if (output_filename != NULL) {
        output_file = fopen(output_filename,"w");
        if (output_file == NULL) {
          fprintf(stderr,"\nError: Cannot open output file \"%s\"\n\n", output_filename);
          exit(1);
        }
        store_as_compact(Result, output_file);
        fclose(output_file);
      } 
      else
        store_as_compact(Result, stdout);
      exit(0);	     
    }
    //Result = explode_and_minimise(Result);
#endif

    if (output_filename != NULL ) {
      if (store_symbols)
        write_transducer(Result, output_filename, NULL, backwards_compatible);  // a NULL KeyTable -> TheAlphabet is written
      else
        write_transducer(Result, output_filename, backwards_compatible);
    }
    else {
      if (store_symbols) 
        write_transducer(Result, NULL, cout, backwards_compatible);
      else
        write_transducer(Result, cout, backwards_compatible);
    }
    delete_transducer(Result);
    if (store_filename != NULL) {
      ofstream os(store_filename);
      write_symbol_table(NULL, os);
      os.close();
    }
    delete_keypair_set(Pi);
  }
  catch(const char* p) {
      fprintf(stderr, "\n%s\n\n", p);
      exit(1);
  }
}


