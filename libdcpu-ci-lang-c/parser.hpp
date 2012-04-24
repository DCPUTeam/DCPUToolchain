
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
     ERROR = 258,
     CURVED_OPEN = 259,
     CURVED_CLOSE = 260,
     BRACE_OPEN = 261,
     BRACE_CLOSE = 262,
     COMMA = 263,
     STAR = 264,
     SEMICOLON = 265,
     DOT = 266,
     STRUCT = 267,
     NUMBER = 268,
     IDENTIFIER = 269,
     CHARACTER = 270,
     STRING = 271,
     ASSEMBLY = 272,
     ASSIGN_EQUAL = 273,
     ASSIGN_ADD = 274,
     ASSIGN_SUBTRACT = 275,
     ASSIGN_MULTIPLY = 276,
     ASSIGN_DIVIDE = 277,
     COMPARE_EQUAL = 278,
     COMPARE_NOT_EQUAL = 279,
     COMPARE_LESS_THAN = 280,
     COMPARE_LESS_THAN_EQUAL = 281,
     COMPARE_GREATER_THAN = 282,
     COMPARE_GREATER_THAN_EQUAL = 283,
     NEGATE = 284,
     BITWISE_NEGATE = 285,
     INCREMENT = 286,
     DECREMENT = 287,
     ADD = 288,
     SUBTRACT = 289,
     SLASH = 290,
     PERCENT = 291,
     BOOLEAN_AND = 292,
     BOOLEAN_OR = 293,
     BINARY_AND = 294,
     BINARY_OR = 295,
     BINARY_XOR = 296,
     BINARY_LEFT_SHIFT = 297,
     BINARY_RIGHT_SHIFT = 298,
     IREF = 299,
     IDEREF = 300,
     IADDROF = 301,
     IUNARYPLUS = 302,
     IUNARYMINUS = 303,
     IPREINC = 304,
     IPREDEC = 305,
     IPOSTINC = 306,
     IPOSTDEC = 307,
     TRUE = 308,
     FALSE = 309,
     RETURN = 310,
     IF = 311,
     ELSE = 312,
     WHILE = 313,
     FOR = 314,
     DEBUG = 315,
     SIZEOF = 316,
     TYPE_VOID = 317,
     TYPE_CHAR = 318,
     TYPE_BYTE = 319,
     TYPE_INT = 320,
     TYPE_LONG = 321,
     TYPE_INT8_T = 322,
     TYPE_INT16_T = 323,
     TYPE_INT32_T = 324,
     TYPE_INT64_T = 325,
     TYPE_UINT8_T = 326,
     TYPE_UINT16_T = 327,
     TYPE_UINT32_T = 328,
     TYPE_UINT64_T = 329,
     DEREFDOT = 330
   };
#endif
/* Tokens.  */
#define ERROR 258
#define CURVED_OPEN 259
#define CURVED_CLOSE 260
#define BRACE_OPEN 261
#define BRACE_CLOSE 262
#define COMMA 263
#define STAR 264
#define SEMICOLON 265
#define DOT 266
#define STRUCT 267
#define NUMBER 268
#define IDENTIFIER 269
#define CHARACTER 270
#define STRING 271
#define ASSEMBLY 272
#define ASSIGN_EQUAL 273
#define ASSIGN_ADD 274
#define ASSIGN_SUBTRACT 275
#define ASSIGN_MULTIPLY 276
#define ASSIGN_DIVIDE 277
#define COMPARE_EQUAL 278
#define COMPARE_NOT_EQUAL 279
#define COMPARE_LESS_THAN 280
#define COMPARE_LESS_THAN_EQUAL 281
#define COMPARE_GREATER_THAN 282
#define COMPARE_GREATER_THAN_EQUAL 283
#define NEGATE 284
#define BITWISE_NEGATE 285
#define INCREMENT 286
#define DECREMENT 287
#define ADD 288
#define SUBTRACT 289
#define SLASH 290
#define PERCENT 291
#define BOOLEAN_AND 292
#define BOOLEAN_OR 293
#define BINARY_AND 294
#define BINARY_OR 295
#define BINARY_XOR 296
#define BINARY_LEFT_SHIFT 297
#define BINARY_RIGHT_SHIFT 298
#define IREF 299
#define IDEREF 300
#define IADDROF 301
#define IUNARYPLUS 302
#define IUNARYMINUS 303
#define IPREINC 304
#define IPREDEC 305
#define IPOSTINC 306
#define IPOSTDEC 307
#define TRUE 308
#define FALSE 309
#define RETURN 310
#define IF 311
#define ELSE 312
#define WHILE 313
#define FOR 314
#define DEBUG 315
#define SIZEOF 316
#define TYPE_VOID 317
#define TYPE_CHAR 318
#define TYPE_BYTE 319
#define TYPE_INT 320
#define TYPE_LONG 321
#define TYPE_INT8_T 322
#define TYPE_INT16_T 323
#define TYPE_INT32_T 324
#define TYPE_INT64_T 325
#define TYPE_UINT8_T 326
#define TYPE_UINT16_T 327
#define TYPE_UINT32_T 328
#define TYPE_UINT64_T 329
#define DEREFDOT 330




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 62 "parser.y"

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
#line 224 "parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


