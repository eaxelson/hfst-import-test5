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
     ERROR = 258,
     METADATA_START = 259,
     ALPHABET_START = 260,
     MULTICHARS_START = 261,
     DEFINITIONS_START = 262,
     END_START = 263,
     LEXICON_START = 264,
     LEXICON_NAME = 265,
     ULSTRING = 266,
     ENTRY_GLOSS = 267,
     METADATA_FIELDNAME = 268,
     METADATA_VALUE = 269,
     MULTICHAR_SYMBOL = 270,
     ALPHABET_SYMBOL = 271,
     DEFINITION_NAME = 272,
     DEFINITION_EXPRESSION = 273,
     XEROX_REGEXP = 274
   };
#endif
/* Tokens.  */
#define ERROR 258
#define METADATA_START 259
#define ALPHABET_START 260
#define MULTICHARS_START 261
#define DEFINITIONS_START 262
#define END_START 263
#define LEXICON_START 264
#define LEXICON_NAME 265
#define ULSTRING 266
#define ENTRY_GLOSS 267
#define METADATA_FIELDNAME 268
#define METADATA_VALUE 269
#define MULTICHAR_SYMBOL 270
#define ALPHABET_SYMBOL 271
#define DEFINITION_NAME 272
#define DEFINITION_EXPRESSION 273
#define XEROX_REGEXP 274




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 275 "lexc-parser.yy"
{
	char* name;
	int number;
}
/* Line 1489 of yacc.c.  */
#line 92 "lexc-parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE hlexclval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE hlexclloc;
