/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Tokens.  */
#define NET 258
#define END 259
#define LBRACKET 260
#define RBRACKET 261
#define LPAREN 262
#define RPAREN 263
#define ENDM 264
#define ENDD 265
#define CRESTRICT 266
#define CONTAINS 267
#define CONTAINS_OPT_ONE 268
#define CONTAINS_ONE 269
#define XUPPER 270
#define XLOWER 271
#define FLAG_ELIMINATE 272
#define IGNORE_ALL 273
#define IGNORE_INTERNAL 274
#define CONTEXT 275
#define NCONCAT 276
#define MNCONCAT 277
#define MORENCONCAT 278
#define LESSNCONCAT 279
#define DOUBLE_COMMA 280
#define COMMA 281
#define SHUFFLE 282
#define PRECEDES 283
#define FOLLOWS 284
#define RIGHT_QUOTIENT 285
#define LEFT_QUOTIENT 286
#define INTERLEAVE_QUOTIENT 287
#define UQUANT 288
#define EQUANT 289
#define VAR 290
#define IN 291
#define IMPLIES 292
#define EQUALS 293
#define NEQ 294
#define SUBSTITUTE 295
#define SUCCESSOR_OF 296
#define PRIORITY_UNION_U 297
#define PRIORITY_UNION_L 298
#define LENIENT_COMPOSE 299
#define TRIPLE_DOT 300
#define LDOT 301
#define RDOT 302
#define FUNCTION 303
#define SUBVAL 304
#define ISUNAMBIGUOUS 305
#define ISIDENTITY 306
#define ISFUNCTIONAL 307
#define NOTID 308
#define LOWERUNIQ 309
#define LOWERUNIQEPS 310
#define ALLFINAL 311
#define UNAMBIGUOUSPART 312
#define AMBIGUOUSPART 313
#define AMBIGUOUSDOMAIN 314
#define EQSUBSTRINGS 315
#define ARROW 316
#define DIRECTION 317
#define HIGH_CROSS_PRODUCT 318
#define CROSS_PRODUCT 319
#define COMPOSE 320
#define MINUS 321
#define INTERSECT 322
#define UNION 323
#define COMPLEMENT 324
#define INVERSE 325
#define REVERSE 326
#define KLEENE_PLUS 327
#define KLEENE_STAR 328
#define TERM_NEGATION 329




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 152 "regex.y"
{
     char *string;
     struct fsm *net;
     int  type;
}
/* Line 1489 of yacc.c.  */
#line 203 "regex.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



