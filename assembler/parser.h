
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
     COMMA = 258,
     BRACKET_OPEN = 259,
     BRACKET_CLOSE = 260,
     COLON = 261,
     SEMICOLON = 262,
     NEWLINE = 263,
     COMMENT = 264,
     ADD = 265,
     KEYWORD = 266,
     BOUNDARY = 267,
     EXTENSION = 268,
     ORIGIN = 269,
     INCLUDE = 270,
     INCBIN = 271,
     EXPORT = 272,
     IMPORT = 273,
     ERROR = 274,
     WORD = 275,
     STRING = 276,
     CHARACTER = 277,
     ADDRESS = 278
   };
#endif
/* Tokens.  */
#define COMMA 258
#define BRACKET_OPEN 259
#define BRACKET_CLOSE 260
#define COLON 261
#define SEMICOLON 262
#define NEWLINE 263
#define COMMENT 264
#define ADD 265
#define KEYWORD 266
#define BOUNDARY 267
#define EXTENSION 268
#define ORIGIN 269
#define INCLUDE 270
#define INCBIN 271
#define EXPORT 272
#define IMPORT 273
#define ERROR 274
#define WORD 275
#define STRING 276
#define CHARACTER 277
#define ADDRESS 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 43 "parser.y"

	int number;
	char* string;
	struct ast_node_address* address;
	struct ast_node_register* registr;
	struct ast_node_parameter* parameter;
	struct ast_node_parameters* parameters;
	struct ast_node_instruction* instruction;
	struct ast_node_label* label;
	struct ast_node_line* line;
	struct ast_node_lines* lines;
	struct ast_node_root* root;
	int* token;



/* Line 1676 of yacc.c  */
#line 115 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


