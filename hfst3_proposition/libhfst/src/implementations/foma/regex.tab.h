
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
#line 152 "regex.y"

     char *string;
     struct fsm *net;
     int  type;



/* Line 1676 of yacc.c  */
#line 134 "regex.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




