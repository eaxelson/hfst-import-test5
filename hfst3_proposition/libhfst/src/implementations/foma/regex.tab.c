
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 18 "regex.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "foma.h"
#define MAX_F_RECURSION 100
extern int yyerror();
extern int yylex();
extern int my_yyparse(char *my_string);
struct fsm *current_parse;
int rewrite, rule_direction;
int substituting = 0;
static char *subval1, *subval2;
struct fsmcontexts *contexts;
struct fsmrules *rules;
struct rewrite_set *rewrite_rules;
static struct fsm *fargs[100][MAX_F_RECURSION];  /* Function arguments [number][frec] */
static int frec = -1;                            /* Current depth of function recursion */
static char *fname[MAX_F_RECURSION];             /* Function names */
static int fargptr[MAX_F_RECURSION];             /* Current argument no. */
/* Variable to produce internal symbols */
unsigned int g_internal_sym = 23482342;

void add_function_argument(struct fsm *net) {
    fargs[fargptr[frec]][frec] = net;
    fargptr[frec]++;
}

void declare_function_name(char *s) {
    if (frec > MAX_F_RECURSION) {
        printf("Function stack depth exceeded. Aborting.\n");
        exit(1);
    }
    fname[frec] = xxstrdup(s);
}

struct fsm *function_apply(void) {
    int i, mygsym, myfargptr;
    char *regex;
    char repstr[13], oldstr[13];
    if ((regex = find_defined_function(fname[frec],fargptr[frec])) == NULL) {
        printf("***Error: function %s@%i) not defined!\n",fname[frec], fargptr[frec]);
        return NULL;
    }

    regex = xxstrdup(regex);
    mygsym = g_internal_sym;
    myfargptr = fargptr[frec];
    /* Create new regular expression from function def. */
    /* and parse that */
    for (i = 0; i < fargptr[frec]; i++) {
        sprintf(repstr,"%012X",g_internal_sym);
        sprintf(oldstr, "@ARGUMENT%02i@", (i+1));       
        streqrep(regex, oldstr, repstr);
        /* We temporarily define a network and save argument there */
        /* The name is a running counter g_internal_sym */
        add_defined(fargs[i][frec], repstr);
        g_internal_sym++;
    }

    my_yyparse(regex);
    for (i = 0; i < myfargptr; i++) {
        sprintf(repstr,"%012X",mygsym);
        /* Remove the temporarily defined network */
        remove_defined(repstr);
        mygsym++;
    }
    frec--;
    return(current_parse);
}

void add_context_pair(struct fsm *L, struct fsm *R) {
    struct fsmcontexts *newcontext;
    newcontext = xxmalloc(sizeof(struct fsmcontexts));
    newcontext->left = L;
    newcontext->right = R;
    newcontext->next = contexts;
    contexts = newcontext;
}

void add_rewrite_rule() {
    struct rewrite_set *new_rewrite_rule;
    if (rules != NULL) {
        new_rewrite_rule = xxmalloc(sizeof(struct rewrite_set));
        new_rewrite_rule->rewrite_rules = rules;
        new_rewrite_rule->rewrite_contexts = contexts;
        new_rewrite_rule->next = rewrite_rules;
        new_rewrite_rule->rule_direction = rule_direction;
        rewrite_rules = new_rewrite_rule;
        rules = NULL;
        contexts = NULL;
        rule_direction = 0;
    }
}

void add_rule(struct fsm *L, struct fsm *R, struct fsm *R2, int type) {
    struct fsm *test;
    struct fsmrules *newrule;
    rewrite = 1;
    newrule = xxmalloc(sizeof(struct fsmrules));

    if ((type & ARROW_DOTTED) != 0) {
        newrule->left = fsm_minus(fsm_copy(L), fsm_empty_string());       
    } else {
        newrule->left = L;
    }
    newrule->right = R;
    newrule->right2 = R2;
    newrule->next = rules;
    newrule->arrow_type = type;
    if ((type & ARROW_DOTTED) != 0) {
        newrule->arrow_type = type - ARROW_DOTTED;
    }

    rules = newrule;

    if ((type & ARROW_DOTTED) != 0) {
        /* Add empty [..] -> B for dotted rules (only if LHS contains the empty string) */
        test = fsm_intersect(fsm_copy(L),fsm_empty_string());
        if (!fsm_isempty(test)) {
            newrule = xxmalloc(sizeof(struct fsmrules));
            newrule->left = test;
            newrule->right = fsm_copy(R);
            newrule->right2 = fsm_copy(R2);
            newrule->next = rules;
            newrule->arrow_type = type;
            rules = newrule;
        }
    }
}




/* Line 189 of yacc.c  */
#line 208 "regex.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NET = 258,
     END = 259,
     LBRACKET = 260,
     RBRACKET = 261,
     LPAREN = 262,
     RPAREN = 263,
     ENDM = 264,
     ENDD = 265,
     CRESTRICT = 266,
     CONTAINS = 267,
     CONTAINS_OPT_ONE = 268,
     CONTAINS_ONE = 269,
     XUPPER = 270,
     XLOWER = 271,
     FLAG_ELIMINATE = 272,
     IGNORE_ALL = 273,
     IGNORE_INTERNAL = 274,
     CONTEXT = 275,
     NCONCAT = 276,
     MNCONCAT = 277,
     MORENCONCAT = 278,
     LESSNCONCAT = 279,
     DOUBLE_COMMA = 280,
     COMMA = 281,
     SHUFFLE = 282,
     PRECEDES = 283,
     FOLLOWS = 284,
     RIGHT_QUOTIENT = 285,
     LEFT_QUOTIENT = 286,
     INTERLEAVE_QUOTIENT = 287,
     UQUANT = 288,
     EQUANT = 289,
     VAR = 290,
     IN = 291,
     IMPLIES = 292,
     EQUALS = 293,
     NEQ = 294,
     SUBSTITUTE = 295,
     SUCCESSOR_OF = 296,
     PRIORITY_UNION_U = 297,
     PRIORITY_UNION_L = 298,
     LENIENT_COMPOSE = 299,
     TRIPLE_DOT = 300,
     LDOT = 301,
     RDOT = 302,
     FUNCTION = 303,
     SUBVAL = 304,
     ISUNAMBIGUOUS = 305,
     ISIDENTITY = 306,
     ISFUNCTIONAL = 307,
     NOTID = 308,
     LOWERUNIQ = 309,
     LOWERUNIQEPS = 310,
     ALLFINAL = 311,
     UNAMBIGUOUSPART = 312,
     AMBIGUOUSPART = 313,
     AMBIGUOUSDOMAIN = 314,
     EQSUBSTRINGS = 315,
     ARROW = 316,
     DIRECTION = 317,
     HIGH_CROSS_PRODUCT = 318,
     CROSS_PRODUCT = 319,
     COMPOSE = 320,
     MINUS = 321,
     INTERSECT = 322,
     UNION = 323,
     COMPLEMENT = 324,
     INVERSE = 325,
     REVERSE = 326,
     KLEENE_PLUS = 327,
     KLEENE_STAR = 328,
     TERM_NEGATION = 329
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 152 "regex.y"

     char *string;
     struct fsm *net;
     int  type;



/* Line 214 of yacc.c  */
#line 326 "regex.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 338 "regex.tab.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  83
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   893

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  121
/* YYNRULES -- Number of states.  */
#define YYNSTATES  236

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    13,    17,    21,    25,
      27,    29,    33,    36,    41,    43,    47,    52,    56,    59,
      64,    70,    74,    78,    84,    89,    97,   104,   112,   119,
     125,   131,   135,   139,   144,   149,   155,   163,   170,   177,
     183,   191,   198,   205,   211,   213,   217,   221,   225,   227,
     229,   231,   235,   239,   243,   247,   251,   255,   257,   260,
     264,   268,   272,   276,   280,   282,   286,   290,   294,   298,
     302,   304,   307,   310,   313,   316,   318,   320,   323,   326,
     329,   332,   335,   338,   341,   345,   348,   351,   354,   357,
     359,   362,   364,   366,   371,   374,   378,   382,   388,   394,
     400,   403,   408,   413,   415,   419,   423,   427,   431,   435,
     439,   443,   447,   451,   455,   463,   467,   470,   474,   478,
     482,   486
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      76,     0,    -1,    77,    -1,    77,    76,    -1,    78,     4,
      -1,    79,    -1,    78,    65,    79,    -1,    78,    44,    79,
      -1,    78,    64,    79,    -1,    80,    -1,    81,    -1,    80,
      20,    80,    -1,    80,    20,    -1,    80,    20,    26,    80,
      -1,    20,    -1,    20,    25,    80,    -1,    80,    20,    25,
      80,    -1,    20,    26,    80,    -1,    20,    80,    -1,    20,
      80,    26,    80,    -1,    80,    20,    80,    26,    80,    -1,
      80,    11,    80,    -1,    80,    61,    80,    -1,    46,    80,
      47,    61,    80,    -1,    46,    47,    61,    80,    -1,    46,
      80,    47,    61,    80,    26,    80,    -1,    46,    47,    61,
      80,    26,    80,    -1,    46,    80,    47,    61,    80,    62,
      80,    -1,    46,    47,    61,    80,    62,    80,    -1,    80,
      61,    80,    26,    80,    -1,    80,    61,    80,    62,    80,
      -1,    80,    25,    80,    -1,    80,    61,    45,    -1,    80,
      61,    45,    80,    -1,    80,    61,    80,    45,    -1,    80,
      61,    80,    45,    80,    -1,    80,    61,    80,    45,    80,
      26,    80,    -1,    80,    61,    80,    45,    26,    80,    -1,
      80,    61,    45,    80,    26,    80,    -1,    80,    61,    45,
      26,    80,    -1,    80,    61,    80,    45,    80,    62,    80,
      -1,    80,    61,    45,    80,    62,    80,    -1,    80,    61,
      80,    45,    62,    80,    -1,    80,    61,    45,    62,    80,
      -1,    82,    -1,    81,    27,    82,    -1,    81,    28,    82,
      -1,    81,    29,    82,    -1,    83,    -1,    84,    -1,    85,
      -1,    84,    68,    85,    -1,    84,    42,    85,    -1,    84,
      43,    85,    -1,    84,    67,    85,    -1,    84,    66,    85,
      -1,    84,    37,    85,    -1,    86,    -1,    85,    86,    -1,
      35,    36,    85,    -1,    35,    38,    35,    -1,    35,    39,
      35,    -1,    35,    28,    35,    -1,    35,    29,    35,    -1,
      87,    -1,    86,    18,    87,    -1,    86,    19,    87,    -1,
      86,    30,    87,    -1,    86,    31,    87,    -1,    86,    32,
      87,    -1,    88,    -1,    69,    87,    -1,    12,    87,    -1,
      14,    87,    -1,    13,    87,    -1,    89,    -1,    90,    -1,
      89,    73,    -1,    89,    72,    -1,    89,    71,    -1,    89,
      70,    -1,    89,    15,    -1,    89,    16,    -1,    89,    17,
      -1,    89,    63,    90,    -1,    89,    21,    -1,    89,    23,
      -1,    89,    24,    -1,    89,    22,    -1,    91,    -1,    74,
      90,    -1,     3,    -1,    94,    -1,    33,     7,    78,     8,
      -1,    34,    78,    -1,     7,    78,     8,    -1,     5,    78,
       6,    -1,    41,    35,    26,    35,     8,    -1,    41,    35,
      26,    78,     8,    -1,    41,    78,    26,    35,     8,    -1,
      92,    93,    -1,    40,     5,    78,    26,    -1,    49,    26,
      49,     6,    -1,    97,    -1,    51,    78,     8,    -1,    52,
      78,     8,    -1,    50,    78,     8,    -1,    53,    78,     8,
      -1,    54,    78,     8,    -1,    55,    78,     8,    -1,    56,
      78,     8,    -1,    57,    78,     8,    -1,    58,    78,     8,
      -1,    59,    78,     8,    -1,    60,    78,    26,    78,    26,
      78,     8,    -1,    48,    78,    26,    -1,    48,    78,    -1,
      95,    78,    26,    -1,    96,    78,    26,    -1,    96,    78,
       8,    -1,    95,    78,     8,    -1,    95,     8,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   204,   204,   205,   208,   210,   211,   212,   213,   215,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   231,   232,   233,   234,   235,   236,   237,
     238,   240,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   256,   257,   258,   259,   261,   263,
     265,   266,   267,   268,   269,   270,   271,   273,   274,   277,
     279,   280,   281,   282,   284,   285,   286,   287,   288,   289,
     291,   292,   293,   294,   295,   297,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   309,   310,   311,   312,   314,
     315,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   328,   329,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   344,   346,   349,   350,   352,
     354,   356
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NET", "END", "LBRACKET", "RBRACKET",
  "LPAREN", "RPAREN", "ENDM", "ENDD", "CRESTRICT", "CONTAINS",
  "CONTAINS_OPT_ONE", "CONTAINS_ONE", "XUPPER", "XLOWER", "FLAG_ELIMINATE",
  "IGNORE_ALL", "IGNORE_INTERNAL", "CONTEXT", "NCONCAT", "MNCONCAT",
  "MORENCONCAT", "LESSNCONCAT", "DOUBLE_COMMA", "COMMA", "SHUFFLE",
  "PRECEDES", "FOLLOWS", "RIGHT_QUOTIENT", "LEFT_QUOTIENT",
  "INTERLEAVE_QUOTIENT", "UQUANT", "EQUANT", "VAR", "IN", "IMPLIES",
  "EQUALS", "NEQ", "SUBSTITUTE", "SUCCESSOR_OF", "PRIORITY_UNION_U",
  "PRIORITY_UNION_L", "LENIENT_COMPOSE", "TRIPLE_DOT", "LDOT", "RDOT",
  "FUNCTION", "SUBVAL", "ISUNAMBIGUOUS", "ISIDENTITY", "ISFUNCTIONAL",
  "NOTID", "LOWERUNIQ", "LOWERUNIQEPS", "ALLFINAL", "UNAMBIGUOUSPART",
  "AMBIGUOUSPART", "AMBIGUOUSDOMAIN", "EQSUBSTRINGS", "ARROW", "DIRECTION",
  "HIGH_CROSS_PRODUCT", "CROSS_PRODUCT", "COMPOSE", "MINUS", "INTERSECT",
  "UNION", "COMPLEMENT", "INVERSE", "REVERSE", "KLEENE_PLUS",
  "KLEENE_STAR", "TERM_NEGATION", "$accept", "start", "regex", "network",
  "networkA", "n0", "network1", "network2", "network3", "network4",
  "network5", "network6", "network7", "network8", "network9", "network10",
  "network11", "sub1", "sub2", "network12", "fstart", "fmid", "fend", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    76,    77,    78,    78,    78,    78,    79,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    81,    81,    81,    81,    82,    83,
      84,    84,    84,    84,    84,    84,    84,    85,    85,    85,
      85,    85,    85,    85,    86,    86,    86,    86,    86,    86,
      87,    87,    87,    87,    87,    88,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    90,
      90,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    92,    93,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    95,    95,    96,    96,    97,
      97,    97
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     1,     3,     3,     3,     1,
       1,     3,     2,     4,     1,     3,     4,     3,     2,     4,
       5,     3,     3,     5,     4,     7,     6,     7,     6,     5,
       5,     3,     3,     4,     4,     5,     7,     6,     6,     5,
       7,     6,     6,     5,     1,     3,     3,     3,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     1,     2,     3,
       3,     3,     3,     3,     1,     3,     3,     3,     3,     3,
       1,     2,     2,     2,     2,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     2,     2,     2,     1,
       2,     1,     1,     4,     2,     3,     3,     5,     5,     5,
       2,     4,     4,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     7,     3,     2,     3,     3,     3,
       3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    91,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       0,     5,     9,    10,    44,    48,    49,    50,    57,    64,
      70,    75,    76,    89,     0,    92,     0,     0,   103,     0,
       0,    72,    74,    73,     0,     0,    18,     0,    94,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   116,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    71,    90,     1,     3,     4,     0,     0,     0,     0,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,     0,     0,     0,     0,     0,    81,    82,
      83,    85,    88,    86,    87,     0,    80,    79,    78,    77,
       0,   100,   121,     0,     0,    96,    95,    15,    17,     0,
       0,    62,    63,    59,    60,    61,     0,     0,     0,     0,
       0,   115,   106,   104,   105,   107,   108,   109,   110,   111,
     112,   113,     0,     7,     8,     6,    21,     0,     0,    11,
      31,    32,    22,    45,    46,    47,    56,    52,    53,    55,
      54,    51,    65,    66,    67,    68,    69,    84,     0,   120,
     117,   119,   118,    19,    93,   101,     0,     0,     0,    24,
       0,     0,    16,    13,     0,     0,     0,    33,     0,    34,
       0,     0,    97,    98,    99,     0,     0,    23,     0,    20,
      39,    43,     0,     0,    29,     0,     0,    35,    30,   102,
      26,    28,     0,     0,     0,    38,    41,    37,    42,     0,
       0,    25,    27,   114,    36,    40
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,   121,    45,
      46,    47,    48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -34
static const yytype_int16 yypact[] =
{
     613,   -34,   613,   613,   787,   787,   787,   207,    -2,   613,
     660,     8,   671,   439,   613,   613,   613,   613,   613,   613,
     613,   613,   613,   613,   613,   613,   787,   819,    15,   613,
      -1,   -34,    83,   149,   -34,   -34,   765,   787,   612,   -34,
     -34,    57,   -34,   -34,   -16,   -34,   497,   613,   -34,    -4,
       1,   -34,   -34,   -34,   613,   613,   393,   613,    -6,     2,
       7,   112,     9,    11,   613,   370,   -10,    32,   516,    -8,
      31,    33,    42,   115,   131,   153,   186,   229,   231,   238,
     301,   -34,   -34,   -34,   -34,   -34,   613,   613,   613,   613,
     265,   613,   555,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   612,   787,   787,   787,   787,   787,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   819,   -34,   -34,   -34,   -34,
      44,   -34,   -34,    23,    27,   -34,   -34,    83,    83,   613,
     244,   -34,   -34,   787,   -34,   -34,   303,   729,    41,   613,
      40,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   613,   -34,   -34,   -34,    83,   613,   613,   561,
      83,   323,   400,   -34,   -34,   -34,   787,   787,   787,   787,
     787,   787,   -34,   -34,   -34,   -34,   -34,   -34,    19,   -34,
     -34,   -34,   -34,    83,   -34,   -34,   826,   289,    94,    79,
     613,   326,    83,    83,   613,   613,   613,   445,   613,   381,
     613,   108,   -34,   -34,   -34,   613,   613,   457,   613,    83,
      83,    83,   613,   613,    83,   613,   613,   503,    83,   -34,
      83,    83,   613,   613,   296,    83,    83,    83,    83,   613,
     613,    83,    83,   -34,    83,    83
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -34,    87,   -34,     5,   114,    -7,   -34,   141,   -34,   -34,
     538,   -33,     6,   -34,   -34,   -26,   -34,   -34,   -34,   -34,
     -34,   -34,   -34
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      56,    82,   125,    85,   102,    57,    68,    49,    50,   126,
      51,    52,    53,    64,    58,    83,   138,    66,   141,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   179,    81,   120,    86,   181,    86,   131,    86,   142,
      86,   143,   132,    86,   134,    86,   135,   127,   128,   180,
     144,   123,   124,   182,    87,    88,    87,    88,    87,    88,
      87,    88,   130,    87,    88,    87,    88,    86,   201,   136,
     178,    86,   108,   109,   110,    86,   188,    86,   111,   112,
     113,   114,   156,   159,   160,   162,    86,    87,    88,   177,
      89,    87,    88,   139,    89,    87,    88,    87,    88,    90,
     102,   190,   204,    90,    91,   205,    87,    88,    91,   172,
     173,   174,   175,   176,   219,     1,    84,     2,     0,     3,
     115,     0,   183,   145,     4,     5,     6,   116,   117,   118,
     119,     0,   189,   102,   102,   102,   102,   102,   102,   146,
      92,   206,   187,     0,    92,     8,     9,    10,     0,     0,
     192,   193,    11,    12,   197,     0,     0,   191,     0,    86,
      14,   147,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     0,     0,    86,    93,    94,    95,    87,
      88,    26,     0,   207,     0,     0,    27,   209,   210,   211,
       0,   214,   217,   218,   148,    87,    88,    86,   220,   221,
     153,   154,   155,     0,     0,   225,   226,     0,   227,   228,
       1,     0,     2,   224,     3,   231,   232,    87,    88,     4,
       5,     6,   234,   235,     0,     0,     0,     7,     0,     0,
      86,     0,    54,    55,   163,   164,   165,   149,     0,   150,
       8,     9,    10,     0,     0,     0,   151,    11,    12,     0,
      87,    88,   184,    13,     0,    14,     0,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     1,     0,
       2,     0,     3,    86,     0,    86,    26,     4,     5,     6,
       0,    27,    86,     0,     0,     7,     0,     0,    86,     0,
     157,   158,     0,    87,    88,    87,    88,   203,     8,     9,
      10,     0,    87,    88,   233,    11,    12,     0,    87,    88,
       0,    13,     0,    14,     0,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     1,   152,     2,   185,
       3,     0,     0,    86,    26,     4,     5,     6,     0,    27,
      86,     0,     0,     7,     0,    86,     0,    86,     0,   195,
       0,     0,   208,    87,    88,     0,     8,     9,    10,     0,
      87,    88,     0,    11,    12,    87,    88,    87,    88,    13,
      86,    14,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     1,   196,     2,     0,     3,     0,
      87,    88,    26,     4,     5,     6,   137,    27,    59,    60,
       0,     7,     0,     0,    89,     0,    61,   215,    62,    63,
       0,    89,     0,    90,     8,     9,    10,     0,    91,   129,
      90,    11,    12,     0,     0,    91,   198,    13,     0,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,     1,   216,     2,   199,     3,     0,     0,     0,
      26,     4,     5,     6,    92,    27,    89,     0,     0,     7,
       0,    92,   200,     0,     0,    90,     0,     0,    89,     0,
      91,   212,     8,     9,    10,     0,     0,    90,     0,    11,
      12,     0,    91,   222,     0,    13,    67,    14,     0,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       1,     0,     2,     0,     3,   122,    92,   213,    26,     4,
       5,     6,     0,    27,    89,     0,     0,     7,    92,   223,
       0,     0,     0,    90,     0,     0,     0,    89,    91,   229,
       8,     9,    10,     0,     0,     0,    90,    11,    12,     0,
       0,    91,     0,    13,     0,    14,     0,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     1,     0,
       2,     0,     3,   140,    92,   230,    26,     4,     5,     6,
       0,    27,    89,     0,     0,     7,     0,    92,     0,     0,
       0,    90,     0,     0,     0,     0,    91,   194,     8,     9,
      10,     0,     0,     0,     0,    11,    12,     0,     0,   133,
     161,    13,     0,    14,     0,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     1,     0,     2,     0,
       3,     0,    92,     0,    26,     4,     5,     6,     0,    27,
     103,   104,     0,     7,   166,   167,   168,   169,   170,   171,
       0,     0,   105,   106,   107,     0,     8,     9,    10,     0,
       0,     0,     0,    11,    12,     0,     0,     0,     0,    13,
       0,    14,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     1,     0,     2,     0,     3,     0,
       0,     0,    26,     4,     5,     6,     0,    27,    59,    60,
       0,     7,     0,     0,     0,     0,    61,     0,    62,    63,
       0,     0,     0,     0,     8,     9,    65,     0,     0,     0,
       0,    11,    12,     0,     0,     0,     0,    13,     0,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,     1,     0,     2,     0,     3,     0,     0,     0,
      26,     4,     5,     6,     0,    27,     0,     0,     0,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     8,     9,   186,     0,     0,     0,     0,    11,
      12,     0,     0,     0,     0,    13,     0,    14,     0,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       1,     0,     2,     0,     3,     0,     0,     0,    26,     4,
       5,     6,    96,    27,     0,     0,     0,    97,    98,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,     9,     1,     0,     2,     0,     3,    11,    12,     0,
       0,    99,   100,   101,   202,    14,     0,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     8,     9,    59,    60,    26,     0,     0,    11,
      12,    27,    61,     0,    62,    63,     0,    14,     0,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27
};

static const yytype_int16 yycheck[] =
{
       7,    27,     6,     4,    37,     7,    13,     2,     3,     8,
       4,     5,     6,     5,     9,     0,    26,    12,    26,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     8,    26,    49,    44,     8,    44,    35,    44,     8,
      44,     8,    35,    44,    35,    44,    35,    54,    55,    26,
       8,    46,    47,    26,    64,    65,    64,    65,    64,    65,
      64,    65,    57,    64,    65,    64,    65,    44,    49,    64,
      26,    44,    15,    16,    17,    44,    35,    44,    21,    22,
      23,    24,    89,    90,    91,    92,    44,    64,    65,   115,
      11,    64,    65,    61,    11,    64,    65,    64,    65,    20,
     133,    61,     8,    20,    25,    26,    64,    65,    25,   103,
     104,   105,   106,   107,     6,     3,    29,     5,    -1,     7,
      63,    -1,   129,     8,    12,    13,    14,    70,    71,    72,
      73,    -1,   139,   166,   167,   168,   169,   170,   171,     8,
      61,    62,   137,    -1,    61,    33,    34,    35,    -1,    -1,
     157,   158,    40,    41,   161,    -1,    -1,   152,    -1,    44,
      48,     8,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    44,    27,    28,    29,    64,
      65,    69,    -1,   190,    -1,    -1,    74,   194,   195,   196,
      -1,   198,   199,   200,     8,    64,    65,    44,   205,   206,
      86,    87,    88,    -1,    -1,   212,   213,    -1,   215,   216,
       3,    -1,     5,   208,     7,   222,   223,    64,    65,    12,
      13,    14,   229,   230,    -1,    -1,    -1,    20,    -1,    -1,
      44,    -1,    25,    26,    93,    94,    95,     8,    -1,     8,
      33,    34,    35,    -1,    -1,    -1,     8,    40,    41,    -1,
      64,    65,     8,    46,    -1,    48,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,     3,    -1,
       5,    -1,     7,    44,    -1,    44,    69,    12,    13,    14,
      -1,    74,    44,    -1,    -1,    20,    -1,    -1,    44,    -1,
      25,    26,    -1,    64,    65,    64,    65,     8,    33,    34,
      35,    -1,    64,    65,     8,    40,    41,    -1,    64,    65,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,     3,    26,     5,    26,
       7,    -1,    -1,    44,    69,    12,    13,    14,    -1,    74,
      44,    -1,    -1,    20,    -1,    44,    -1,    44,    -1,    26,
      -1,    -1,    26,    64,    65,    -1,    33,    34,    35,    -1,
      64,    65,    -1,    40,    41,    64,    65,    64,    65,    46,
      44,    48,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,     3,    62,     5,    -1,     7,    -1,
      64,    65,    69,    12,    13,    14,    26,    74,    28,    29,
      -1,    20,    -1,    -1,    11,    -1,    36,    26,    38,    39,
      -1,    11,    -1,    20,    33,    34,    35,    -1,    25,    26,
      20,    40,    41,    -1,    -1,    25,    26,    46,    -1,    48,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     3,    62,     5,    45,     7,    -1,    -1,    -1,
      69,    12,    13,    14,    61,    74,    11,    -1,    -1,    20,
      -1,    61,    62,    -1,    -1,    20,    -1,    -1,    11,    -1,
      25,    26,    33,    34,    35,    -1,    -1,    20,    -1,    40,
      41,    -1,    25,    26,    -1,    46,    47,    48,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
       3,    -1,     5,    -1,     7,     8,    61,    62,    69,    12,
      13,    14,    -1,    74,    11,    -1,    -1,    20,    61,    62,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    11,    25,    26,
      33,    34,    35,    -1,    -1,    -1,    20,    40,    41,    -1,
      -1,    25,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,     3,    -1,
       5,    -1,     7,    47,    61,    62,    69,    12,    13,    14,
      -1,    74,    11,    -1,    -1,    20,    -1,    61,    -1,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    25,    26,    33,    34,
      35,    -1,    -1,    -1,    -1,    40,    41,    -1,    -1,    61,
      45,    46,    -1,    48,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,     3,    -1,     5,    -1,
       7,    -1,    61,    -1,    69,    12,    13,    14,    -1,    74,
      18,    19,    -1,    20,    96,    97,    98,    99,   100,   101,
      -1,    -1,    30,    31,    32,    -1,    33,    34,    35,    -1,
      -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,     3,    -1,     5,    -1,     7,    -1,
      -1,    -1,    69,    12,    13,    14,    -1,    74,    28,    29,
      -1,    20,    -1,    -1,    -1,    -1,    36,    -1,    38,    39,
      -1,    -1,    -1,    -1,    33,    34,    35,    -1,    -1,    -1,
      -1,    40,    41,    -1,    -1,    -1,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,     3,    -1,     5,    -1,     7,    -1,    -1,    -1,
      69,    12,    13,    14,    -1,    74,    -1,    -1,    -1,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    -1,    -1,    -1,    -1,    40,
      41,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
       3,    -1,     5,    -1,     7,    -1,    -1,    -1,    69,    12,
      13,    14,    37,    74,    -1,    -1,    -1,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,     3,    -1,     5,    -1,     7,    40,    41,    -1,
      -1,    66,    67,    68,     8,    48,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    -1,
      -1,    -1,    33,    34,    28,    29,    69,    -1,    -1,    40,
      41,    74,    36,    -1,    38,    39,    -1,    48,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    74
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     5,     7,    12,    13,    14,    20,    33,    34,
      35,    40,    41,    46,    48,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    69,    74,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    94,    95,    96,    97,    78,
      78,    87,    87,    87,    25,    26,    80,     7,    78,    28,
      29,    36,    38,    39,     5,    35,    78,    47,    80,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    87,    90,     0,    76,     4,    44,    64,    65,    11,
      20,    25,    61,    27,    28,    29,    37,    42,    43,    66,
      67,    68,    86,    18,    19,    30,    31,    32,    15,    16,
      17,    21,    22,    23,    24,    63,    70,    71,    72,    73,
      49,    93,     8,    78,    78,     6,     8,    80,    80,    26,
      78,    35,    35,    85,    35,    35,    78,    26,    26,    61,
      47,    26,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,    26,    79,    79,    79,    80,    25,    26,    80,
      80,    45,    80,    82,    82,    82,    85,    85,    85,    85,
      85,    85,    87,    87,    87,    87,    87,    90,    26,     8,
      26,     8,    26,    80,     8,    26,    35,    78,    35,    80,
      61,    78,    80,    80,    26,    26,    62,    80,    26,    45,
      62,    49,     8,     8,     8,    26,    62,    80,    26,    80,
      80,    80,    26,    62,    80,    26,    62,    80,    80,     6,
      80,    80,    26,    62,    78,    80,    80,    80,    80,    26,
      62,    80,    80,     8,    80,    80
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
      yyerror (scanner, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, scanner)
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
		  Type, Value, scanner); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void     *scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void     *scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (scanner);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void     *scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void     *scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, void     *scanner)
#else
static void
yy_reduce_print (yyvsp, yyrule, scanner)
    YYSTYPE *yyvsp;
    int yyrule;
    void     *scanner;
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, scanner); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void     *scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    void     *scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (scanner);

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
int yyparse (void     *scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
yyparse (void     *scanner)
#else
int
yyparse (scanner)
    void     *scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 163 "regex.y"
{
    clear_quantifiers();
    rewrite = 0;
    contexts = NULL;
    rules = NULL;
    rewrite_rules = NULL;
    rule_direction = 0;
    substituting = 0;
}

/* Line 1242 of yacc.c  */
#line 1733 "regex.tab.c"

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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
        case 4:

/* Line 1455 of yacc.c  */
#line 208 "regex.y"
    { current_parse = (yyvsp[(1) - (2)].net);              ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 210 "regex.y"
    { ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 211 "regex.y"
    { (yyval.net) = fsm_compose((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));         ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 212 "regex.y"
    { (yyval.net) = fsm_lenient_compose((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net)); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 213 "regex.y"
    { (yyval.net) = fsm_cross_product((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));   ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 215 "regex.y"
    { if (rewrite) { add_rewrite_rule(); (yyval.net) = fsm_rewrite(rewrite_rules); } rewrite = 0; contexts = NULL; rules = NULL; rewrite_rules = NULL; ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 217 "regex.y"
    { ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 218 "regex.y"
    { (yyval.net) = NULL; add_context_pair((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 219 "regex.y"
    { add_context_pair((yyvsp[(1) - (2)].net),fsm_empty_string()); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 220 "regex.y"
    { add_context_pair((yyvsp[(1) - (4)].net),fsm_empty_string()); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 221 "regex.y"
    { add_context_pair(fsm_empty_string(),fsm_empty_string());;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 222 "regex.y"
    { add_rewrite_rule(); add_context_pair(fsm_empty_string(),fsm_empty_string());;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 223 "regex.y"
    { add_rewrite_rule(); add_context_pair((yyvsp[(1) - (4)].net),fsm_empty_string());;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 224 "regex.y"
    { add_context_pair(fsm_empty_string(),fsm_empty_string());;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 225 "regex.y"
    { add_context_pair(fsm_empty_string(),(yyvsp[(2) - (2)].net)); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 226 "regex.y"
    { add_context_pair(fsm_empty_string(),(yyvsp[(2) - (4)].net)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 227 "regex.y"
    { add_context_pair((yyvsp[(1) - (5)].net),(yyvsp[(3) - (5)].net)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 228 "regex.y"
    { (yyval.net) = fsm_context_restrict((yyvsp[(1) - (3)].net),contexts);;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 229 "regex.y"
    { add_rule((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net),NULL,(yyvsp[(2) - (3)].type));if ((yyvsp[(3) - (3)].net) == NULL) { YYERROR;};}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 231 "regex.y"
    { add_rule((yyvsp[(2) - (5)].net),(yyvsp[(5) - (5)].net),NULL,(yyvsp[(4) - (5)].type)|ARROW_DOTTED); if ((yyvsp[(5) - (5)].net) == NULL) { YYERROR;};}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 232 "regex.y"
    { add_rule(fsm_empty_string(),(yyvsp[(4) - (4)].net),NULL,(yyvsp[(3) - (4)].type)|ARROW_DOTTED); if ((yyvsp[(4) - (4)].net) == NULL) { YYERROR;};}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 233 "regex.y"
    { add_rule((yyvsp[(2) - (7)].net),(yyvsp[(5) - (7)].net),NULL,(yyvsp[(4) - (7)].type)|ARROW_DOTTED);;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 234 "regex.y"
    { add_rule(fsm_empty_string(),(yyvsp[(4) - (6)].net),NULL,(yyvsp[(3) - (6)].type)|ARROW_DOTTED);;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 235 "regex.y"
    { add_rule((yyvsp[(2) - (7)].net),(yyvsp[(5) - (7)].net),NULL,(yyvsp[(4) - (7)].type)|ARROW_DOTTED); rule_direction = (yyvsp[(6) - (7)].type);;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 236 "regex.y"
    { add_rule(fsm_empty_string(),(yyvsp[(4) - (6)].net),NULL,(yyvsp[(3) - (6)].type)|ARROW_DOTTED); rule_direction = (yyvsp[(5) - (6)].type);;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 237 "regex.y"
    { add_rule((yyvsp[(1) - (5)].net),(yyvsp[(3) - (5)].net),NULL,(yyvsp[(2) - (5)].type));;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 238 "regex.y"
    { add_rule((yyvsp[(1) - (5)].net),(yyvsp[(3) - (5)].net),NULL,(yyvsp[(2) - (5)].type)); rule_direction = (yyvsp[(4) - (5)].type);;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 240 "regex.y"
    { add_rewrite_rule();;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 242 "regex.y"
    { add_rule((yyvsp[(1) - (3)].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[(2) - (3)].type));;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 243 "regex.y"
    { add_rule((yyvsp[(1) - (4)].net),fsm_empty_string(),(yyvsp[(4) - (4)].net),(yyvsp[(2) - (4)].type));;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 244 "regex.y"
    { add_rule((yyvsp[(1) - (4)].net),(yyvsp[(3) - (4)].net),fsm_empty_string(),(yyvsp[(2) - (4)].type));;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 245 "regex.y"
    { add_rule((yyvsp[(1) - (5)].net),(yyvsp[(3) - (5)].net),(yyvsp[(5) - (5)].net),(yyvsp[(2) - (5)].type));;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 246 "regex.y"
    { add_rule((yyvsp[(1) - (7)].net),(yyvsp[(3) - (7)].net),(yyvsp[(5) - (7)].net),(yyvsp[(2) - (7)].type));;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 247 "regex.y"
    { add_rule((yyvsp[(1) - (6)].net),(yyvsp[(3) - (6)].net),fsm_empty_string(),(yyvsp[(2) - (6)].type));;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 248 "regex.y"
    { add_rule((yyvsp[(1) - (6)].net),fsm_empty_string(),(yyvsp[(4) - (6)].net),(yyvsp[(2) - (6)].type));;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 249 "regex.y"
    { add_rule((yyvsp[(1) - (5)].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[(2) - (5)].type));;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 250 "regex.y"
    { add_rule((yyvsp[(1) - (7)].net),(yyvsp[(3) - (7)].net),(yyvsp[(5) - (7)].net),(yyvsp[(2) - (7)].type)); rule_direction = (yyvsp[(6) - (7)].type);;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 251 "regex.y"
    { add_rule((yyvsp[(1) - (6)].net),fsm_empty_string(),(yyvsp[(4) - (6)].net),(yyvsp[(2) - (6)].type)); rule_direction = (yyvsp[(5) - (6)].type);;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 252 "regex.y"
    { add_rule((yyvsp[(1) - (6)].net),(yyvsp[(3) - (6)].net),fsm_empty_string(),(yyvsp[(2) - (6)].type)); rule_direction = (yyvsp[(5) - (6)].type);;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 253 "regex.y"
    { add_rule((yyvsp[(1) - (5)].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[(2) - (5)].type)); rule_direction = (yyvsp[(4) - (5)].type);;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 256 "regex.y"
    { ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 257 "regex.y"
    { (yyval.net) = fsm_shuffle((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));  ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 258 "regex.y"
    { (yyval.net) = fsm_precedes((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 259 "regex.y"
    { (yyval.net) = fsm_follows((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));  ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 261 "regex.y"
    { ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 263 "regex.y"
    { ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 265 "regex.y"
    { ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 266 "regex.y"
    { (yyval.net) = fsm_union((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));                     ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 267 "regex.y"
    { (yyval.net) = fsm_priority_union_upper((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));      ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 268 "regex.y"
    { (yyval.net) = fsm_priority_union_lower((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));      ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 269 "regex.y"
    { (yyval.net) = fsm_intersect((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));                 ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 270 "regex.y"
    { (yyval.net) = fsm_minus((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));                     ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 271 "regex.y"
    { (yyval.net) = fsm_union(fsm_complement((yyvsp[(1) - (3)].net)),(yyvsp[(3) - (3)].net));     ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 273 "regex.y"
    { ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 274 "regex.y"
    { (yyval.net) = fsm_concat((yyvsp[(1) - (2)].net),(yyvsp[(2) - (2)].net)); 
/* printf("Concating: [%s] [%s]\n",yyvsp[(1) - (2)].string, yyvsp[(2) - (2)].string);  */
;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 277 "regex.y"
    { (yyval.net) = fsm_ignore(fsm_contains(fsm_concat(fsm_symbol((yyvsp[(1) - (3)].string)),fsm_concat((yyvsp[(3) - (3)].net),fsm_symbol((yyvsp[(1) - (3)].string))))),union_quantifiers(),OP_IGNORE_ALL); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 279 "regex.y"
    { (yyval.net) = fsm_logical_eq((yyvsp[(1) - (3)].string),(yyvsp[(3) - (3)].string)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 280 "regex.y"
    { (yyval.net) = fsm_complement(fsm_logical_eq((yyvsp[(1) - (3)].string),(yyvsp[(3) - (3)].string))); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 281 "regex.y"
    { (yyval.net) = fsm_logical_precedence((yyvsp[(1) - (3)].string),(yyvsp[(3) - (3)].string)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 282 "regex.y"
    { (yyval.net) = fsm_logical_precedence((yyvsp[(3) - (3)].string),(yyvsp[(1) - (3)].string)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 284 "regex.y"
    { ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 285 "regex.y"
    { (yyval.net) = fsm_ignore((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net), OP_IGNORE_ALL);          ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 286 "regex.y"
    { (yyval.net) = fsm_ignore((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net), OP_IGNORE_INTERNAL);     ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 287 "regex.y"
    { (yyval.net) = fsm_quotient_right((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));                 ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 288 "regex.y"
    { (yyval.net) = fsm_quotient_left((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));                  ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 289 "regex.y"
    { (yyval.net) = fsm_quotient_interleave((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net));            ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 291 "regex.y"
    { ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 292 "regex.y"
    { (yyval.net) = fsm_complement((yyvsp[(2) - (2)].net));       ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 293 "regex.y"
    { (yyval.net) = fsm_contains((yyvsp[(2) - (2)].net));         ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 294 "regex.y"
    { (yyval.net) = fsm_contains_one((yyvsp[(2) - (2)].net));     ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 295 "regex.y"
    { (yyval.net) = fsm_contains_opt_one((yyvsp[(2) - (2)].net)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 297 "regex.y"
    { ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 299 "regex.y"
    { ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 300 "regex.y"
    { (yyval.net) = fsm_kleene_star(fsm_minimize((yyvsp[(1) - (2)].net))); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 301 "regex.y"
    { (yyval.net) = fsm_kleene_plus((yyvsp[(1) - (2)].net)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 302 "regex.y"
    { (yyval.net) = fsm_determinize(fsm_reverse((yyvsp[(1) - (2)].net))); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 303 "regex.y"
    { (yyval.net) = fsm_invert((yyvsp[(1) - (2)].net)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 304 "regex.y"
    { (yyval.net) = fsm_upper((yyvsp[(1) - (2)].net)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 305 "regex.y"
    { (yyval.net) = fsm_lower((yyvsp[(1) - (2)].net)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 306 "regex.y"
    { (yyval.net) = flag_eliminate((yyvsp[(1) - (2)].net), NULL); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 307 "regex.y"
    { (yyval.net) = fsm_cross_product((yyvsp[(1) - (3)].net),(yyvsp[(3) - (3)].net)); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 309 "regex.y"
    { (yyval.net) = fsm_concat_n((yyvsp[(1) - (2)].net),atoi((yyvsp[(2) - (2)].string))); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 310 "regex.y"
    { (yyval.net) = fsm_concat(fsm_concat_n((yyvsp[(1) - (2)].net), atoi((yyvsp[(2) - (2)].string))),fsm_kleene_plus((yyvsp[(1) - (2)].net))); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 311 "regex.y"
    { (yyval.net) = fsm_concat_m_n((yyvsp[(1) - (2)].net),0,atoi((yyvsp[(2) - (2)].string))-1); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 312 "regex.y"
    { (yyval.net) = fsm_concat_m_n((yyvsp[(1) - (2)].net),atoi((yyvsp[(2) - (2)].string)),atoi(strstr((yyvsp[(2) - (2)].string),",")+1)); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 314 "regex.y"
    { ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 315 "regex.y"
    { (yyval.net) = fsm_term_negation((yyvsp[(2) - (2)].net)); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 317 "regex.y"
    { (yyval.net) = (yyvsp[(1) - (1)].net);;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 318 "regex.y"
    { (yyval.net) = (yyvsp[(1) - (1)].net); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 319 "regex.y"
    { (yyval.net) = fsm_complement(fsm_substitute_symbol(fsm_intersect(fsm_quantifier((yyvsp[(1) - (4)].string)),fsm_complement((yyvsp[(3) - (4)].net))),(yyvsp[(1) - (4)].string),"@_EPSILON_SYMBOL_@")); purge_quantifier((yyvsp[(1) - (4)].string)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 320 "regex.y"
    {  (yyval.net) = fsm_substitute_symbol(fsm_intersect(fsm_quantifier((yyvsp[(1) - (2)].string)),(yyvsp[(2) - (2)].net)),(yyvsp[(1) - (2)].string),"@_EPSILON_SYMBOL_@"); purge_quantifier((yyvsp[(1) - (2)].string)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 321 "regex.y"
    { if (count_quantifiers()) (yyval.net) = (yyvsp[(2) - (3)].net); else {(yyval.net) = fsm_optionality((yyvsp[(2) - (3)].net));} ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 322 "regex.y"
    { (yyval.net) = (yyvsp[(2) - (3)].net); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 323 "regex.y"
    {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(2) - (5)].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(2) - (5)].string)),fsm_concat(union_quantifiers(),fsm_concat(fsm_symbol((yyvsp[(4) - (5)].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(4) - (5)].string)),fsm_universal())))))))); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 324 "regex.y"
    {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(2) - (5)].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(2) - (5)].string)),fsm_concat(fsm_ignore((yyvsp[(4) - (5)].net),union_quantifiers(),OP_IGNORE_ALL),fsm_universal()))))); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 325 "regex.y"
    {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_ignore((yyvsp[(2) - (5)].net),union_quantifiers(),OP_IGNORE_ALL),fsm_concat(fsm_symbol((yyvsp[(4) - (5)].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[(4) - (5)].string)),fsm_universal()))))); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 326 "regex.y"
    {(yyval.net) = fsm_substitute_symbol((yyvsp[(1) - (2)].net),subval1,subval2); substituting = 0; xxfree(subval1); xxfree(subval2); subval1 = subval2 = NULL;;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 328 "regex.y"
    { (yyval.net) = (yyvsp[(3) - (4)].net); substituting = 1;                      ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 329 "regex.y"
    { subval1 = xxstrdup((yyvsp[(2) - (4)].string)); subval2 = xxstrdup((yyvsp[(4) - (4)].string)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 331 "regex.y"
    { (yyval.net) = (yyvsp[(1) - (1)].net) ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 332 "regex.y"
    { (yyval.net) = fsm_boolean(fsm_isidentity((yyvsp[(2) - (3)].net)));   ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 333 "regex.y"
    { (yyval.net) = fsm_boolean(fsm_isfunctional((yyvsp[(2) - (3)].net))); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 334 "regex.y"
    { (yyval.net) = fsm_boolean(fsm_isunambiguous((yyvsp[(2) - (3)].net))); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 335 "regex.y"
    { (yyval.net) = fsm_extract_nonidentity(fsm_copy((yyvsp[(2) - (3)].net))); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 336 "regex.y"
    { (yyval.net) = fsm_lowerdet(fsm_copy((yyvsp[(2) - (3)].net))); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 337 "regex.y"
    { (yyval.net) = fsm_lowerdeteps(fsm_copy((yyvsp[(2) - (3)].net))); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 338 "regex.y"
    { (yyval.net) = fsm_markallfinal(fsm_copy((yyvsp[(2) - (3)].net))); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 339 "regex.y"
    { (yyval.net) = fsm_extract_unambiguous(fsm_copy((yyvsp[(2) - (3)].net)));      ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 340 "regex.y"
    { (yyval.net) = fsm_extract_ambiguous(fsm_copy((yyvsp[(2) - (3)].net)));        ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 341 "regex.y"
    { (yyval.net) = fsm_extract_ambiguous_domain(fsm_copy((yyvsp[(2) - (3)].net))); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 342 "regex.y"
    { (yyval.net) = fsm_equal_substrings((yyvsp[(2) - (7)].net),(yyvsp[(4) - (7)].net),(yyvsp[(6) - (7)].net)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 345 "regex.y"
    { frec++; fargptr[frec] = 0 ;declare_function_name((yyvsp[(1) - (3)].string)) ; add_function_argument((yyvsp[(2) - (3)].net)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 347 "regex.y"
    { frec++; fargptr[frec] = 0 ;declare_function_name((yyvsp[(1) - (2)].string)) ; add_function_argument((yyvsp[(2) - (2)].net)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 349 "regex.y"
    { add_function_argument((yyvsp[(2) - (3)].net)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 350 "regex.y"
    { add_function_argument((yyvsp[(2) - (3)].net)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 353 "regex.y"
    { add_function_argument((yyvsp[(2) - (3)].net)); if (((yyval.net) = function_apply()) == NULL) YYERROR; ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 355 "regex.y"
    { add_function_argument((yyvsp[(2) - (3)].net)); if (((yyval.net) = function_apply()) == NULL) YYERROR; ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 357 "regex.y"
    { if (((yyval.net) = function_apply()) == NULL) YYERROR;;}
    break;



/* Line 1455 of yacc.c  */
#line 2742 "regex.tab.c"
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
      yyerror (scanner, YY_("syntax error"));
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
	    yyerror (scanner, yymsg);
	  }
	else
	  {
	    yyerror (scanner, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
		      yytoken, &yylval, scanner);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
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
		  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, scanner);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, scanner);
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



/* Line 1675 of yacc.c  */
#line 359 "regex.y"


