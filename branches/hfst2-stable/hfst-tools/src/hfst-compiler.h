
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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 193 "hfst-compiler.yy"

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



/* Line 1676 of yacc.c  */
#line 135 "hfst-compiler.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


