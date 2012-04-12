
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
     CURVED_OPEN = 258,
     CURVED_CLOSE = 259,
     BRACE_OPEN = 260,
     BRACE_CLOSE = 261,
     COMMA = 262,
     STAR = 263,
     SEMICOLON = 264,
     DOT = 265,
     STRUCT = 266,
     NUMBER = 267,
     IDENTIFIER = 268,
     CHARACTER = 269,
     STRING = 270,
     ASSIGN_EQUAL = 271,
     ASSIGN_ADD = 272,
     ASSIGN_SUBTRACT = 273,
     ASSIGN_MULTIPLY = 274,
     ASSIGN_DIVIDE = 275,
     COMPARE_EQUAL = 276,
     COMPARE_NOT_EQUAL = 277,
     COMPARE_LESS_THAN = 278,
     COMPARE_LESS_THAN_EQUAL = 279,
     COMPARE_GREATER_THAN = 280,
     COMPARE_GREATER_THAN_EQUAL = 281,
     NEGATE = 282,
     BITWISE_NEGATE = 283,
     INCREMENT = 284,
     DECREMENT = 285,
     ADD = 286,
     SUBTRACT = 287,
     SLASH = 288,
     PERCENT = 289,
     BOOLEAN_AND = 290,
     BOOLEAN_OR = 291,
     BINARY_AND = 292,
     BINARY_OR = 293,
     BINARY_XOR = 294,
     BINARY_LEFT_SHIFT = 295,
     BINARY_RIGHT_SHIFT = 296,
     IREF = 297,
     IDEREF = 298,
     IADDROF = 299,
     IUNARYPLUS = 300,
     IUNARYMINUS = 301,
     IPREINC = 302,
     IPREDEC = 303,
     IPOSTINC = 304,
     IPOSTDEC = 305,
     TRUE = 306,
     FALSE = 307,
     RETURN = 308,
     IF = 309,
     ELSE = 310,
     WHILE = 311,
     FOR = 312,
     DEBUG = 313,
     SIZEOF = 314,
     TYPE_VOID = 315,
     TYPE_CHAR = 316,
     TYPE_BYTE = 317,
     TYPE_INT = 318,
     TYPE_LONG = 319,
     TYPE_INT8_T = 320,
     TYPE_INT16_T = 321,
     TYPE_INT32_T = 322,
     TYPE_INT64_T = 323,
     TYPE_UINT8_T = 324,
     TYPE_UINT16_T = 325,
     TYPE_UINT32_T = 326,
     TYPE_UINT64_T = 327,
     DEREFDOT = 328
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 41 "parser.y"

	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NInteger* numeric;
	NType* type;
	NDeclarations* decls;
	NFunctionDeclaration* function;
	NStructureDeclaration* structure;
	NVariableDeclaration *variable;
	std::vector<NExpression*> *exprvec;
	std::vector<NDeclaration*> *declvec;
	std::vector<NVariableDeclaration*> *varvec;
	std::string* string;
	const char* data;
	long number;
	int token;



/* Line 1676 of yacc.c  */
#line 147 "parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


