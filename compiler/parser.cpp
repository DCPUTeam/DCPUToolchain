/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines parser for the compiler.

**/

class NAssignmentExpression;
class NInteger;

#include <vector>
#include "asmgen.h"
#include "node.h"

// Turn on verbose error messages.
#define YYERROR_VERBOSE

// Defines the root node.
NDeclarations* program;

// YY-stuff.
extern int yylex();
extern int yycolumn;

void yyerror(const char* msg)
{
	fflush(stdout);
	fprintf(stderr, "\n%*s\n%*s\n", yycolumn, "^", yycolumn, msg);
}



/* Line 189 of yacc.c  */
#line 113 "parser.cpp"

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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 321 "parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 333 "parser.cpp"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   792

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  113
/* YYNRULES -- Number of states.  */
#define YYNSTATES  187

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   330

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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    15,    18,    22,
      29,    36,    37,    39,    43,    51,    57,    58,    60,    64,
      67,    72,    81,    92,    94,    96,    99,   102,   106,   109,
     113,   116,   118,   121,   124,   126,   128,   130,   132,   134,
     136,   139,   141,   143,   151,   157,   161,   165,   171,   181,
     183,   187,   191,   194,   197,   202,   206,   210,   212,   214,
     219,   221,   223,   225,   227,   231,   234,   238,   240,   244,
     247,   249,   251,   253,   258,   260,   262,   263,   265,   269,
     271,   273,   275,   277,   279,   281,   283,   285,   287,   289,
     291,   293,   295,   297,   299,   301,   303,   305,   307,   309,
     311,   313,   315,   317,   319,   321,   323,   325,   327,   329,
     331,   333,   335,   337
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      77,     0,    -1,    78,    -1,    79,    -1,    81,    -1,    83,
      10,    -1,    78,    79,    -1,    78,    81,    -1,    78,    83,
      10,    -1,    85,    84,     4,    80,     5,    86,    -1,    85,
      84,     4,    80,     5,    10,    -1,    -1,    83,    -1,    80,
       8,    83,    -1,    12,    84,     6,    82,    10,     7,    10,
      -1,    12,    84,     6,     7,    10,    -1,    -1,    83,    -1,
      82,    10,    83,    -1,    85,    84,    -1,    85,    84,    18,
      98,    -1,    85,     4,     9,    84,     5,     4,    80,     5,
      -1,    85,     4,     9,    84,     5,     4,    80,     5,    18,
      98,    -1,    14,    -1,   106,    -1,    12,    84,    -1,   106,
       9,    -1,    12,    84,     9,    -1,    85,     9,    -1,     6,
      87,     7,    -1,     6,     7,    -1,    88,    -1,    87,    88,
      -1,    83,    10,    -1,    90,    -1,    93,    -1,    94,    -1,
      91,    -1,    92,    -1,    95,    -1,    98,    10,    -1,    88,
      -1,    86,    -1,    56,     4,    98,     5,    89,    57,    89,
      -1,    56,     4,    98,     5,    89,    -1,    55,    98,    10,
      -1,    60,    98,    10,    -1,    58,     4,    98,     5,    89,
      -1,    59,     4,    98,    10,    98,    10,    98,     5,    89,
      -1,    17,    -1,    98,    11,    84,    -1,    96,   104,    98,
      -1,     9,    84,    -1,     9,   100,    -1,     9,     4,    98,
       5,    -1,    97,   104,    98,    -1,    84,   104,    98,    -1,
      97,    -1,    96,    -1,    84,     4,   103,     5,    -1,    84,
      -1,   100,    -1,   101,    -1,   102,    -1,    98,   105,    98,
      -1,    39,    99,    -1,     4,    98,     5,    -1,    84,    -1,
      98,    11,    84,    -1,     9,    98,    -1,    13,    -1,    53,
      -1,    54,    -1,    61,     4,    85,     5,    -1,    15,    -1,
      16,    -1,    -1,    98,    -1,   103,     8,    98,    -1,    18,
      -1,    19,    -1,    20,    -1,    21,    -1,    22,    -1,    37,
      -1,    38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,
      -1,    43,    -1,    23,    -1,    24,    -1,    25,    -1,    26,
      -1,    27,    -1,    28,    -1,    33,    -1,    34,    -1,     9,
      -1,    35,    -1,    62,    -1,    63,    -1,    64,    -1,    65,
      -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   126,   126,   133,   138,   143,   148,   152,   156,   162,
     167,   175,   178,   183,   189,   194,   202,   205,   210,   216,
     220,   224,   228,   234,   241,   246,   251,   256,   261,   268,
     272,   278,   283,   289,   293,   294,   295,   296,   297,   298,
     299,   305,   310,   316,   320,   326,   332,   338,   344,   350,
     357,   361,   370,   374,   378,   382,   391,   395,   399,   403,
     408,   412,   416,   420,   428,   432,   436,   442,   446,   450,
     456,   460,   464,   468,   474,   481,   489,   492,   497,   503,
     504,   505,   506,   507,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   529,   533,   537,   541,   545,   549,   553,   557,   561,
     565,   569,   573,   577
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR", "CURVED_OPEN", "CURVED_CLOSE",
  "BRACE_OPEN", "BRACE_CLOSE", "COMMA", "STAR", "SEMICOLON", "DOT",
  "STRUCT", "NUMBER", "IDENTIFIER", "CHARACTER", "STRING", "ASSEMBLY",
  "ASSIGN_EQUAL", "ASSIGN_ADD", "ASSIGN_SUBTRACT", "ASSIGN_MULTIPLY",
  "ASSIGN_DIVIDE", "COMPARE_EQUAL", "COMPARE_NOT_EQUAL",
  "COMPARE_LESS_THAN", "COMPARE_LESS_THAN_EQUAL", "COMPARE_GREATER_THAN",
  "COMPARE_GREATER_THAN_EQUAL", "NEGATE", "BITWISE_NEGATE", "INCREMENT",
  "DECREMENT", "ADD", "SUBTRACT", "SLASH", "PERCENT", "BOOLEAN_AND",
  "BOOLEAN_OR", "BINARY_AND", "BINARY_OR", "BINARY_XOR",
  "BINARY_LEFT_SHIFT", "BINARY_RIGHT_SHIFT", "IREF", "IDEREF", "IADDROF",
  "IUNARYPLUS", "IUNARYMINUS", "IPREINC", "IPREDEC", "IPOSTINC",
  "IPOSTDEC", "TRUE", "FALSE", "RETURN", "IF", "ELSE", "WHILE", "FOR",
  "DEBUG", "SIZEOF", "TYPE_VOID", "TYPE_CHAR", "TYPE_BYTE", "TYPE_INT",
  "TYPE_LONG", "TYPE_INT8_T", "TYPE_INT16_T", "TYPE_INT32_T",
  "TYPE_INT64_T", "TYPE_UINT8_T", "TYPE_UINT16_T", "TYPE_UINT32_T",
  "TYPE_UINT64_T", "DEREFDOT", "$accept", "program", "prog_decl",
  "func_decl", "func_decl_args", "struct_decl", "struct_decl_args",
  "var_decl", "ident", "type", "block", "stmts", "stmt", "block_or_stmt",
  "stmt_if", "stmt_return", "stmt_debug", "stmt_while", "stmt_for",
  "stmt_asm", "fldref", "deref", "expr", "addressable", "numeric",
  "character", "string", "call_args", "assignop", "binaryop", "type_base", 0
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
     325,   326,   327,   328,   329,   330
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    78,    78,    78,    78,    78,    79,
      79,    80,    80,    80,    81,    81,    82,    82,    82,    83,
      83,    83,    83,    84,    85,    85,    85,    85,    85,    86,
      86,    87,    87,    88,    88,    88,    88,    88,    88,    88,
      88,    89,    89,    90,    90,    91,    92,    93,    94,    95,
      96,    96,    97,    97,    97,    97,    98,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    99,    99,    99,
     100,   100,   100,   100,   101,   102,   103,   103,   103,   104,
     104,   104,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     2,     2,     3,     6,
       6,     0,     1,     3,     7,     5,     0,     1,     3,     2,
       4,     8,    10,     1,     1,     2,     2,     3,     2,     3,
       2,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     7,     5,     3,     3,     5,     9,     1,
       3,     3,     2,     2,     4,     3,     3,     1,     1,     4,
       1,     1,     1,     1,     3,     2,     3,     1,     3,     2,
       1,     1,     1,     4,     1,     1,     0,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,     0,     2,     3,     4,     0,
       0,    24,    23,    25,     1,     6,     7,     0,     5,     0,
      28,    19,    26,    16,    27,     8,     0,    11,     0,     0,
       0,     0,    17,     0,     0,     0,    12,     0,     0,    70,
      74,    75,     0,    71,    72,     0,    60,    58,    57,    20,
      61,    62,    63,    15,    25,     0,    19,     0,     0,     0,
       0,     0,    52,    53,     0,    60,     0,    65,     0,    76,
      79,    80,    81,    82,    83,     0,     0,     0,    99,     0,
      91,    92,    93,    94,    95,    96,    97,    98,   100,    84,
      85,    86,    87,    88,    89,    90,     0,     0,    18,    11,
       0,    10,     9,    13,    66,     0,     0,    52,    69,    53,
       0,     0,    77,     0,    56,    51,    55,    50,    64,    14,
       0,    30,    49,     0,     0,     0,     0,     0,     0,     0,
      31,    34,    37,    38,    35,    36,    39,     0,    54,     0,
      50,    73,    59,     0,    21,     0,     0,     0,     0,     0,
      33,    29,    32,    40,    54,    78,     0,    45,     0,     0,
       0,    46,    22,     0,     0,     0,    42,    41,    44,    47,
       0,     0,     0,    43,     0,     0,    48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    15,    16,    17,    45,    18,    41,   138,    56,    43,
     176,   139,   177,   178,   141,   142,   143,   144,   145,   146,
      57,    58,   147,    77,    60,    61,    62,   123,    85,   106,
      21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -170
static const yytype_int16 yypact[] =
{
     255,    -8,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,     8,   255,  -170,  -170,     5,
      76,     4,  -170,    47,  -170,  -170,  -170,    36,  -170,    43,
    -170,     3,  -170,   122,  -170,  -170,    -8,   268,   339,    54,
      -8,    60,  -170,    76,    72,    57,  -170,   339,     6,  -170,
    -170,  -170,   352,  -170,  -170,    77,     7,   120,   120,   728,
    -170,  -170,  -170,  -170,    73,   192,    66,    82,    33,   268,
     407,   339,  -170,  -170,   368,    53,   749,  -170,   268,   339,
    -170,  -170,  -170,  -170,  -170,   339,   339,   339,  -170,    -8,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,  -170,  -170,  -170,  -170,   339,    78,  -170,   268,
      93,  -170,  -170,  -170,  -170,   446,   339,   265,   728,    27,
      -8,    35,   728,    61,   728,   728,   728,  -170,   728,  -170,
      96,  -170,  -170,   339,    99,   107,   108,   339,   103,   164,
    -170,  -170,  -170,  -170,  -170,  -170,  -170,   623,  -170,   485,
      84,  -170,  -170,   339,    98,   644,   339,   339,   339,   665,
    -170,  -170,  -170,  -170,    88,   728,   339,  -170,   524,   563,
     686,  -170,   728,   235,   235,   339,  -170,  -170,    64,  -170,
     707,   235,   339,  -170,   602,   235,  -170
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -170,  -170,  -170,   101,    14,   109,  -170,    18,     2,     1,
      46,  -170,  -108,  -169,  -170,  -170,  -170,  -170,  -170,  -170,
    -170,  -170,   -38,  -170,   -44,  -170,  -170,  -170,   -34,  -170,
    -170
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -69
static const yytype_int16 yytable[] =
{
      59,    20,   140,    23,    73,   179,    22,    37,    24,    70,
      71,    79,   183,    32,    76,    28,   186,    20,    19,    49,
      22,    38,    31,    86,    87,    80,    81,    82,    83,    84,
     119,   162,   -61,   115,    27,   -61,   118,   -61,    44,   110,
     151,   122,    64,   111,    30,    66,    35,   124,   125,   126,
      72,    42,    36,    33,    75,    46,    34,    79,   -67,    53,
      54,   -67,    68,   -67,    63,    69,   152,    55,   128,   153,
      65,    80,    81,    82,    83,    84,   117,    67,   149,   121,
      29,    78,    34,   108,    38,    30,   109,   113,   129,   -68,
      22,   127,   -68,   -66,   -68,   155,   -66,    47,   -66,   159,
     131,   154,    48,   156,    69,    40,    49,    22,    50,    51,
     132,   157,   158,   160,   112,   165,   166,    25,   168,   169,
     170,   181,   150,   130,     0,    26,     0,    46,   172,    39,
       0,     0,    52,     0,    40,     0,     0,   180,    80,    81,
      82,    83,    84,     0,   184,     0,    53,    54,   133,   134,
       0,   135,   136,   137,    55,     2,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    47,     0,
       0,   161,     0,    48,     0,     0,    40,    49,    22,    50,
      51,   132,     0,     0,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,     0,     0,   107,
       0,     0,     0,    52,    40,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    53,    54,   133,
     134,     0,   135,   136,   137,    55,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    47,
       0,   110,     0,     0,    48,     0,     0,    40,    49,    22,
      50,    51,   132,     0,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,     1,     0,    79,
     -60,     0,     0,   -60,    52,   -60,     0,     0,     0,     0,
      40,     0,     0,    80,    81,    82,    83,    84,    53,    54,
     133,   134,     0,   135,   136,   137,    55,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    47,     0,     0,     0,     0,    48,     0,
       0,     0,    49,    22,    50,    51,    47,     0,     0,     0,
       0,    74,     0,     0,     0,    49,    22,    50,    51,     0,
       0,     0,   116,     0,     0,     0,     0,    48,    52,     0,
       0,    49,    22,    50,    51,     0,     0,     0,     0,     0,
       0,    52,    53,    54,     0,     0,     0,     0,     0,     0,
      55,     0,     0,     0,     0,    53,    54,    52,     0,     0,
       0,     0,   114,    55,     0,     0,    88,     0,    89,     0,
       0,    53,    54,     0,     0,     0,     0,     0,     0,    55,
      90,    91,    92,    93,    94,    95,     0,     0,     0,     0,
      96,    97,    98,     0,    99,   100,   101,   102,   103,   104,
     105,   148,     0,     0,     0,    88,     0,    89,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    90,
      91,    92,    93,    94,    95,     0,     0,     0,     0,    96,
      97,    98,     0,    99,   100,   101,   102,   103,   104,   105,
     164,     0,     0,     0,    88,     0,    89,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    90,    91,
      92,    93,    94,    95,     0,     0,     0,     0,    96,    97,
      98,     0,    99,   100,   101,   102,   103,   104,   105,   173,
       0,     0,     0,    88,     0,    89,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,    91,    92,
      93,    94,    95,     0,     0,     0,     0,    96,    97,    98,
       0,    99,   100,   101,   102,   103,   104,   105,   174,     0,
       0,     0,    88,     0,    89,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    90,    91,    92,    93,
      94,    95,     0,     0,     0,     0,    96,    97,    98,     0,
      99,   100,   101,   102,   103,   104,   105,   185,     0,     0,
       0,    88,     0,    89,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    90,    91,    92,    93,    94,
      95,     0,    88,   163,    89,    96,    97,    98,     0,    99,
     100,   101,   102,   103,   104,   105,    90,    91,    92,    93,
      94,    95,     0,    88,   167,    89,    96,    97,    98,     0,
      99,   100,   101,   102,   103,   104,   105,    90,    91,    92,
      93,    94,    95,     0,    88,   171,    89,    96,    97,    98,
       0,    99,   100,   101,   102,   103,   104,   105,    90,    91,
      92,    93,    94,    95,     0,    88,   175,    89,    96,    97,
      98,     0,    99,   100,   101,   102,   103,   104,   105,    90,
      91,    92,    93,    94,    95,     0,    88,   182,    89,    96,
      97,    98,     0,    99,   100,   101,   102,   103,   104,   105,
      90,    91,    92,    93,    94,    95,     0,    88,     0,    89,
      96,    97,    98,     0,    99,   100,   101,   102,   103,   104,
     105,    90,    91,    92,    93,    94,    95,     0,    88,     0,
     120,    96,    97,    98,     0,    99,   100,   101,   102,   103,
     104,   105,    90,    91,    92,    93,    94,    95,     0,     0,
       0,     0,    96,    97,    98,     0,    99,   100,   101,   102,
     103,   104,   105
};

static const yytype_int16 yycheck[] =
{
      38,     0,   110,     1,    48,   174,    14,     4,     0,    47,
       4,     4,   181,     9,    52,    10,   185,    16,     0,    13,
      14,    18,    20,    57,    58,    18,    19,    20,    21,    22,
      74,   139,     5,    71,    16,     8,    74,    10,    36,     6,
       5,    79,    40,    10,     9,    43,    10,    85,    86,    87,
      48,    33,     9,     6,    52,    37,     9,     4,     5,    53,
      54,     8,     5,    10,    10,     8,     5,    61,   106,     8,
      10,    18,    19,    20,    21,    22,    74,     5,   116,    78,
       4,     4,     9,    65,    18,     9,     4,    69,    10,     5,
      14,    89,     8,     5,    10,   133,     8,     4,    10,   137,
       7,     5,     9,     4,     8,    12,    13,    14,    15,    16,
      17,     4,     4,    10,    68,   153,    18,    16,   156,   157,
     158,    57,   120,   109,    -1,    16,    -1,   109,   166,     7,
      -1,    -1,    39,    -1,    12,    -1,    -1,   175,    18,    19,
      20,    21,    22,    -1,   182,    -1,    53,    54,    55,    56,
      -1,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     4,    -1,
      -1,     7,    -1,     9,    -1,    -1,    12,    13,    14,    15,
      16,    17,    -1,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,     7,
      -1,    -1,    -1,    39,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,
      56,    -1,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,     4,
      -1,     6,    -1,    -1,     9,    -1,    -1,    12,    13,    14,
      15,    16,    17,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    12,    -1,     4,
       5,    -1,    -1,     8,    39,    10,    -1,    -1,    -1,    -1,
      12,    -1,    -1,    18,    19,    20,    21,    22,    53,    54,
      55,    56,    -1,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     4,    -1,    -1,    -1,    -1,     9,    -1,
      -1,    -1,    13,    14,    15,    16,     4,    -1,    -1,    -1,
      -1,     9,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,     4,    -1,    -1,    -1,    -1,     9,    39,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    -1,    -1,    53,    54,    39,    -1,    -1,
      -1,    -1,     5,    61,    -1,    -1,     9,    -1,    11,    -1,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      23,    24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,
      33,    34,    35,    -1,    37,    38,    39,    40,    41,    42,
      43,     5,    -1,    -1,    -1,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,
      24,    25,    26,    27,    28,    -1,    -1,    -1,    -1,    33,
      34,    35,    -1,    37,    38,    39,    40,    41,    42,    43,
       5,    -1,    -1,    -1,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    -1,    33,    34,
      35,    -1,    37,    38,    39,    40,    41,    42,    43,     5,
      -1,    -1,    -1,     9,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,    25,
      26,    27,    28,    -1,    -1,    -1,    -1,    33,    34,    35,
      -1,    37,    38,    39,    40,    41,    42,    43,     5,    -1,
      -1,    -1,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    -1,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    42,    43,     5,    -1,    -1,
      -1,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    24,    25,    26,    27,
      28,    -1,     9,    10,    11,    33,    34,    35,    -1,    37,
      38,    39,    40,    41,    42,    43,    23,    24,    25,    26,
      27,    28,    -1,     9,    10,    11,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    42,    43,    23,    24,    25,
      26,    27,    28,    -1,     9,    10,    11,    33,    34,    35,
      -1,    37,    38,    39,    40,    41,    42,    43,    23,    24,
      25,    26,    27,    28,    -1,     9,    10,    11,    33,    34,
      35,    -1,    37,    38,    39,    40,    41,    42,    43,    23,
      24,    25,    26,    27,    28,    -1,     9,    10,    11,    33,
      34,    35,    -1,    37,    38,    39,    40,    41,    42,    43,
      23,    24,    25,    26,    27,    28,    -1,     9,    -1,    11,
      33,    34,    35,    -1,    37,    38,    39,    40,    41,    42,
      43,    23,    24,    25,    26,    27,    28,    -1,     9,    -1,
      11,    33,    34,    35,    -1,    37,    38,    39,    40,    41,
      42,    43,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    -1,    33,    34,    35,    -1,    37,    38,    39,    40,
      41,    42,    43
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    12,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    77,    78,    79,    81,    83,
      85,   106,    14,    84,     0,    79,    81,    83,    10,     4,
       9,    84,     9,     6,     9,    10,     9,     4,    18,     7,
      12,    82,    83,    85,    84,    80,    83,     4,     9,    13,
      15,    16,    39,    53,    54,    61,    84,    96,    97,    98,
     100,   101,   102,    10,    84,    10,    84,     5,     5,     8,
      98,     4,    84,   100,     9,    84,    98,    99,     4,     4,
      18,    19,    20,    21,    22,   104,   104,   104,     9,    11,
      23,    24,    25,    26,    27,    28,    33,    34,    35,    37,
      38,    39,    40,    41,    42,    43,   105,     7,    83,     4,
       6,    10,    86,    83,     5,    98,     4,    84,    98,   100,
      11,    85,    98,   103,    98,    98,    98,    84,    98,    10,
      80,     7,    17,    55,    56,    58,    59,    60,    83,    87,
      88,    90,    91,    92,    93,    94,    95,    98,     5,    98,
      84,     5,     5,     8,     5,    98,     4,     4,     4,    98,
      10,     7,    88,    10,     5,    98,    18,    10,    98,    98,
      98,    10,    98,     5,     5,    10,    86,    88,    89,    89,
      98,    57,    10,    89,    98,     5,    89
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


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
        case 2:

/* Line 1464 of yacc.c  */
#line 127 "parser.y"
    {
			program = (yyvsp[(1) - (1)].decls);
			(yyval.decls) = (yyvsp[(1) - (1)].decls);
		}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 134 "parser.y"
    {
			(yyval.decls) = new NDeclarations();
			(yyval.decls)->definitions.push_back((yyvsp[(1) - (1)].function));
		}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 139 "parser.y"
    {
			(yyval.decls) = new NDeclarations();
			(yyval.decls)->definitions.push_back((yyvsp[(1) - (1)].structure));
		}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 144 "parser.y"
    {
			(yyval.decls) = new NDeclarations();
			(yyval.decls)->definitions.push_back((yyvsp[(1) - (2)].variable));
		}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 149 "parser.y"
    {
			(yyvsp[(1) - (2)].decls)->definitions.push_back((yyvsp[(2) - (2)].function));
		}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 153 "parser.y"
    {
			(yyvsp[(1) - (2)].decls)->definitions.push_back((yyvsp[(2) - (2)].structure));
		}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 157 "parser.y"
    {
			(yyvsp[(1) - (3)].decls)->definitions.push_back((yyvsp[(2) - (3)].variable));
		}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 163 "parser.y"
    {
			(yyval.function) = new NFunctionDeclaration(*(yyvsp[(1) - (6)].type), *(yyvsp[(2) - (6)].ident), *(yyvsp[(4) - (6)].varvec), (yyvsp[(6) - (6)].block));
			//delete $4;
		}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 168 "parser.y"
    {
			(yyval.function) = new NFunctionDeclaration(*(yyvsp[(1) - (6)].type), *(yyvsp[(2) - (6)].ident), *(yyvsp[(4) - (6)].varvec), NULL);
			//delete $4;
		}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 175 "parser.y"
    {
			(yyval.varvec) = new VariableList();
		}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 179 "parser.y"
    {
			(yyval.varvec) = new VariableList();
			(yyval.varvec)->push_back((yyvsp[(1) - (1)].variable));
		}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 184 "parser.y"
    {
			(yyvsp[(1) - (3)].varvec)->push_back((yyvsp[(3) - (3)].variable));
		}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 190 "parser.y"
    {
			(yyval.structure) = new NStructureDeclaration(*(yyvsp[(2) - (7)].ident), *(yyvsp[(4) - (7)].varvec));
			//delete $4;
		}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 195 "parser.y"
    {
			(yyval.structure) = new NStructureDeclaration(*(yyvsp[(2) - (5)].ident), *(new VariableList()));
			//delete $4;
		}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 202 "parser.y"
    {
			(yyval.varvec) = new VariableList();
		}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 206 "parser.y"
    {
			(yyval.varvec) = new VariableList();
			(yyval.varvec)->push_back((yyvsp[(1) - (1)].variable));
		}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 211 "parser.y"
    {
			(yyvsp[(1) - (3)].varvec)->push_back((yyvsp[(3) - (3)].variable));
		}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 217 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(yyvsp[(1) - (2)].type), *(yyvsp[(2) - (2)].ident));
		}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 221 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(yyvsp[(1) - (4)].type), *(yyvsp[(2) - (4)].ident), (yyvsp[(4) - (4)].expr));
		}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 225 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(new NFunctionPointerType(*(yyvsp[(1) - (8)].type), *(yyvsp[(7) - (8)].varvec))) /* TODO: free this memory */, *(yyvsp[(4) - (8)].ident));
		}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 229 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(new NFunctionPointerType(*(yyvsp[(1) - (10)].type), *(yyvsp[(7) - (10)].varvec))) /* TODO: free this memory */, *(yyvsp[(4) - (10)].ident), (yyvsp[(10) - (10)].expr));
		}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 235 "parser.y"
    {
			(yyval.ident) = new NIdentifier(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 242 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (1)].type)->name, 0, false);
			delete (yyvsp[(1) - (1)].ident);
		}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 247 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(2) - (2)].type)->name, 0, true);
			delete (yyvsp[(2) - (2)].ident);
		}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 252 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (2)].type)->name, 1, false);
			delete (yyvsp[(1) - (2)].ident);
		}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 257 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(2) - (3)].type)->name, 1, true);
			delete (yyvsp[(2) - (3)].ident);
		}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 262 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (2)].type)->name, (yyvsp[(1) - (2)].type)->pointerCount + 1, (yyvsp[(1) - (2)].type)->isStruct);
			delete (yyvsp[(1) - (2)].type);
		}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 269 "parser.y"
    {
			(yyval.block) = (yyvsp[(2) - (3)].block);
		}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 273 "parser.y"
    {
			(yyval.block) = new NBlock();
		}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 279 "parser.y"
    {
			(yyval.block) = new NBlock();
			(yyval.block)->statements.push_back((yyvsp[(1) - (1)].stmt));
		}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 284 "parser.y"
    {
			(yyvsp[(1) - (2)].block)->statements.push_back((yyvsp[(2) - (2)].stmt));
		}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 290 "parser.y"
    {
			(yyval.stmt) = (yyvsp[(1) - (2)].variable);
		}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 300 "parser.y"
    {
			(yyval.stmt) = new NExpressionStatement(*(yyvsp[(1) - (2)].expr));
		}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 306 "parser.y"
    {
			(yyval.block) = new NBlock();
			(yyval.block)->statements.push_back((yyvsp[(1) - (1)].stmt));
		}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 311 "parser.y"
    {
			(yyval.block) = (yyvsp[(1) - (1)].block);
		}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 317 "parser.y"
    {
			(yyval.stmt) = new NIfStatement(*(yyvsp[(3) - (7)].expr), *(yyvsp[(5) - (7)].block), (yyvsp[(7) - (7)].block));
		}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 321 "parser.y"
    {
			(yyval.stmt) = new NIfStatement(*(yyvsp[(3) - (5)].expr), *(yyvsp[(5) - (5)].block));
		}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 327 "parser.y"
    {
			(yyval.stmt) = new NReturnStatement(*(yyvsp[(2) - (3)].expr));
		}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 333 "parser.y"
    {
			(yyval.stmt) = new NDebugStatement(*(yyvsp[(2) - (3)].expr));
		}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 339 "parser.y"
    {
			(yyval.stmt) = new NWhileStatement(*(yyvsp[(3) - (5)].expr), *(yyvsp[(5) - (5)].block));
		}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 345 "parser.y"
    {
			(yyval.stmt) = new NForStatement(*(yyvsp[(3) - (9)].expr), *(yyvsp[(5) - (9)].expr), *(yyvsp[(7) - (9)].expr), *(yyvsp[(9) - (9)].block));
		}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 351 "parser.y"
    {
			(yyval.stmt) = new NAssemblyStatement(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 358 "parser.y"
    {
			(yyval.expr) = new NStructureResolutionOperator(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].ident), false);
		}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 362 "parser.y"
    {
			if ((yyvsp[(1) - (3)].expr)->cType == "expression-field") // We can't accept NAssignments as the fldref in this case.
				(yyval.expr) = new NAssignment(*(yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
			else
				throw new CompilerException("Unable to apply field referencing assignment operation to non-field operator based LHS.");
		}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 371 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(2) - (2)].ident));
		}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 375 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(2) - (2)].expr));
		}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 379 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(3) - (4)].expr));
		}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 383 "parser.y"
    {
			if ((yyvsp[(1) - (3)].expr)->cType == "expression-dereference") // We can't accept NAssignments as the deref in this case.
				(yyval.expr) = new NAssignment(*(yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
			else
				throw new CompilerException("Unable to apply dereferencing assignment operation to non-dereference operator based LHS.");
		}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 392 "parser.y"
    {
			(yyval.expr) = new NAssignment(*(yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
		}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 396 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 400 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 404 "parser.y"
    {
			(yyval.expr) = new NMethodCall(*(yyvsp[(1) - (4)].ident), *(yyvsp[(3) - (4)].exprvec));
			delete (yyvsp[(3) - (4)].exprvec);
		}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 409 "parser.y"
    {
			(yyval.ident) = (yyvsp[(1) - (1)].ident);
		}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 413 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 417 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 421 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 429 "parser.y"
    {
			(yyval.expr) = new NBinaryOperator(*(yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
		}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 433 "parser.y"
    {
			(yyval.expr) = new NAddressOfOperator(*(yyvsp[(2) - (2)].expr));
		}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 437 "parser.y"
    {
			(yyval.expr) = (yyvsp[(2) - (3)].expr);
		}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 443 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].ident);
		}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 447 "parser.y"
    {
			(yyval.expr) = new NStructureResolutionOperator(*(yyvsp[(1) - (3)].expr), *(yyvsp[(3) - (3)].ident), false);
		}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 451 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(2) - (2)].expr));
		}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 457 "parser.y"
    {
			(yyval.expr) = new NInteger((yyvsp[(1) - (1)].number));
		}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 461 "parser.y"
    {
			(yyval.expr) = new NInteger(1);
		}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 465 "parser.y"
    {
			(yyval.expr) = new NInteger(0);
		}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 469 "parser.y"
    {
			(yyval.expr) = new NSizeOfOperator(*(yyvsp[(3) - (4)].type));
		}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 475 "parser.y"
    {
			(yyval.expr) = new NCharacter(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 482 "parser.y"
    {
			(yyval.expr) = new NString(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 489 "parser.y"
    {
			(yyval.exprvec) = new ExpressionList();
		}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 493 "parser.y"
    {
			(yyval.exprvec) = new ExpressionList();
			(yyval.exprvec)->push_back((yyvsp[(1) - (1)].expr));
		}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 498 "parser.y"
    {
			(yyvsp[(1) - (3)].exprvec)->push_back((yyvsp[(3) - (3)].expr));
		}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 530 "parser.y"
    {
			(yyval.ident) = new NIdentifier("void");
		}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 534 "parser.y"
    {
			(yyval.ident) = new NIdentifier("char");
		}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 538 "parser.y"
    {
			(yyval.ident) = new NIdentifier("byte");
		}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 542 "parser.y"
    {
			(yyval.ident) = new NIdentifier("int");
		}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 546 "parser.y"
    {
			(yyval.ident) = new NIdentifier("long");
		}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 550 "parser.y"
    {
			(yyval.ident) = new NIdentifier("int8_t");
		}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 554 "parser.y"
    {
			(yyval.ident) = new NIdentifier("int16_t");
		}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 558 "parser.y"
    {
			(yyval.ident) = new NIdentifier("int32_t");
		}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 562 "parser.y"
    {
			(yyval.ident) = new NIdentifier("int64_t");
		}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 566 "parser.y"
    {
			(yyval.ident) = new NIdentifier("uint8_t");
		}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 570 "parser.y"
    {
			(yyval.ident) = new NIdentifier("uint16_t");
		}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 574 "parser.y"
    {
			(yyval.ident) = new NIdentifier("uint32_t");
		}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 578 "parser.y"
    {
			(yyval.ident) = new NIdentifier("uint64_t");
		}
    break;



/* Line 1464 of yacc.c  */
#line 2633 "parser.cpp"
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
		      yytoken, &yylval);
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
		  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



