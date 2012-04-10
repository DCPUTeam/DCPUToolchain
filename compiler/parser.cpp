
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

//#define YYDEBUG 1
#define YYERROR_VERBOSE

#include "Node.h"
NDeclarations* program;

extern int yylex();
extern int yycolumn;

void yyerror(const char* msg)
{
	fflush(stdout);
	fprintf(stderr, "\n%*s\n%*s\n", yycolumn, "^", yycolumn, msg);
}



/* Line 189 of yacc.c  */
#line 106 "parser.cpp"

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
     DEBUG = 313
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 33 "parser.y"

	NBlock* block;
	NExpression* expr;
	NAssignmentExpression* exprassign;
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
#line 223 "parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 235 "parser.cpp"

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
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   428

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNRULES -- Number of states.  */
#define YYNSTATES  137

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

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
      55,    56,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    15,    22,    23,
      25,    29,    36,    37,    39,    43,    46,    51,    53,    55,
      58,    61,    65,    68,    70,    73,    76,    78,    80,    82,
      84,    86,    89,    91,    93,   101,   107,   111,   115,   121,
     131,   134,   137,   142,   146,   150,   152,   157,   159,   161,
     163,   165,   169,   172,   176,   178,   181,   183,   185,   187,
     189,   191,   192,   194,   198,   200,   202,   204,   206,   208,
     210,   212,   214,   216,   218,   220,   222,   224,   226,   228,
     230,   232,   234,   236,   238,   240
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      60,     0,    -1,    61,    -1,    62,    -1,    64,    -1,    61,
      62,    -1,    61,    64,    -1,    68,    67,     3,    63,     4,
      69,    -1,    -1,    66,    -1,    63,     7,    66,    -1,    11,
      67,     5,    65,     6,     9,    -1,    -1,    66,    -1,    65,
       9,    66,    -1,    68,    67,    -1,    68,    67,    16,    79,
      -1,    13,    -1,    67,    -1,    67,     8,    -1,    68,     8,
      -1,     5,    70,     6,    -1,     5,     6,    -1,    71,    -1,
      70,    71,    -1,    66,     9,    -1,    73,    -1,    76,    -1,
      77,    -1,    74,    -1,    75,    -1,    79,     9,    -1,    71,
      -1,    69,    -1,    54,     3,    79,     4,    72,    55,    72,
      -1,    54,     3,    79,     4,    72,    -1,    53,    79,     9,
      -1,    58,    79,     9,    -1,    56,     3,    79,     4,    72,
      -1,    57,     3,    79,     9,    79,     9,    79,     4,    72,
      -1,     8,    67,    -1,     8,    81,    -1,     8,     3,    79,
       4,    -1,    78,    85,    79,    -1,    67,    85,    79,    -1,
      78,    -1,    67,     3,    84,     4,    -1,    67,    -1,    81,
      -1,    82,    -1,    83,    -1,    79,    86,    79,    -1,    37,
      80,    -1,     3,    79,     4,    -1,    67,    -1,     8,    79,
      -1,    12,    -1,    51,    -1,    52,    -1,    14,    -1,    15,
      -1,    -1,    79,    -1,    84,     7,    79,    -1,    16,    -1,
      17,    -1,    18,    -1,    19,    -1,    20,    -1,    35,    -1,
      36,    -1,    37,    -1,    38,    -1,    39,    -1,    40,    -1,
      41,    -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,
      25,    -1,    26,    -1,    31,    -1,    32,    -1,     8,    -1,
      33,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   112,   112,   119,   124,   129,   133,   139,   147,   150,
     155,   161,   169,   172,   177,   183,   187,   193,   200,   205,
     210,   217,   221,   227,   232,   238,   242,   243,   244,   245,
     246,   247,   253,   258,   264,   268,   274,   280,   286,   292,
     298,   302,   306,   310,   319,   323,   327,   332,   336,   340,
     344,   348,   352,   356,   362,   366,   372,   376,   380,   386,
     393,   401,   404,   409,   415,   416,   417,   418,   419,   422,
     423,   424,   425,   426,   427,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CURVED_OPEN", "CURVED_CLOSE",
  "BRACE_OPEN", "BRACE_CLOSE", "COMMA", "STAR", "SEMICOLON", "DOT",
  "STRUCT", "NUMBER", "IDENTIFIER", "CHARACTER", "STRING", "ASSIGN_EQUAL",
  "ASSIGN_ADD", "ASSIGN_SUBTRACT", "ASSIGN_MULTIPLY", "ASSIGN_DIVIDE",
  "COMPARE_EQUAL", "COMPARE_NOT_EQUAL", "COMPARE_LESS_THAN",
  "COMPARE_LESS_THAN_EQUAL", "COMPARE_GREATER_THAN",
  "COMPARE_GREATER_THAN_EQUAL", "NEGATE", "BITWISE_NEGATE", "INCREMENT",
  "DECREMENT", "ADD", "SUBTRACT", "SLASH", "PERCENT", "BOOLEAN_AND",
  "BOOLEAN_OR", "BINARY_AND", "BINARY_OR", "BINARY_XOR",
  "BINARY_LEFT_SHIFT", "BINARY_RIGHT_SHIFT", "IREF", "IDEREF", "IADDROF",
  "IUNARYPLUS", "IUNARYMINUS", "IPREINC", "IPREDEC", "IPOSTINC",
  "IPOSTDEC", "TRUE", "FALSE", "RETURN", "IF", "ELSE", "WHILE", "FOR",
  "DEBUG", "$accept", "program", "prog_decl", "func_decl",
  "func_decl_args", "struct_decl", "struct_decl_args", "var_decl", "ident",
  "type", "block", "stmts", "stmt", "block_or_stmt", "stmt_if",
  "stmt_return", "stmt_debug", "stmt_while", "stmt_for", "deref", "expr",
  "expr_assign", "numeric", "character", "string", "call_args", "assignop",
  "binaryop", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,   313
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    59,    60,    61,    61,    61,    61,    62,    63,    63,
      63,    64,    65,    65,    65,    66,    66,    67,    68,    68,
      68,    69,    69,    70,    70,    71,    71,    71,    71,    71,
      71,    71,    72,    72,    73,    73,    74,    75,    76,    77,
      78,    78,    78,    78,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    80,    80,    81,    81,    81,    82,
      83,    84,    84,    84,    85,    85,    85,    85,    85,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     2,     6,     0,     1,
       3,     6,     0,     1,     3,     2,     4,     1,     1,     2,
       2,     3,     2,     1,     2,     2,     1,     1,     1,     1,
       1,     2,     1,     1,     7,     5,     3,     3,     5,     9,
       2,     2,     4,     3,     3,     1,     4,     1,     1,     1,
       1,     3,     2,     3,     1,     2,     1,     1,     1,     1,
       1,     0,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    17,     0,     2,     3,     4,    18,     0,     0,
       1,     5,     6,    19,    20,     0,    12,     8,     0,    13,
       0,     0,     9,     0,     0,    15,     0,     0,    11,    14,
       0,     0,     7,    10,     0,     0,    56,    59,    60,     0,
      57,    58,    47,    45,    16,    48,    49,    50,    22,     0,
       0,     0,     0,     0,     0,    47,     0,    23,    26,    29,
      30,    27,    28,     0,     0,     0,    40,    41,     0,    54,
      52,    61,    64,    65,    66,    67,    68,     0,     0,    84,
      76,    77,    78,    79,    80,    81,    82,    83,    85,    69,
      70,    71,    72,    73,    74,    75,     0,     0,     0,     0,
       0,     0,    25,    21,    24,    31,    53,     0,    55,    62,
       0,    44,    43,    51,    36,     0,     0,     0,    37,    42,
      46,     0,     0,     0,     0,    63,    33,    32,    35,    38,
       0,     0,     0,    34,     0,     0,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,     5,    21,     6,    18,    54,    42,    20,
     126,    56,   127,   128,    58,    59,    60,    61,    62,    43,
      63,    70,    45,    46,    47,   110,    77,    96
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -114
static const yytype_int16 yypact[] =
{
      25,   -10,  -114,    11,    25,  -114,  -114,     4,    15,    14,
    -114,  -114,  -114,  -114,  -114,    27,   -10,   -10,    20,  -114,
      15,    41,  -114,    37,   -10,    35,    53,   -10,  -114,  -114,
     133,    49,  -114,  -114,   133,     2,  -114,  -114,  -114,    34,
    -114,  -114,    57,   155,   387,  -114,  -114,  -114,  -114,   133,
      56,    64,    75,   133,    71,   136,    76,  -114,  -114,  -114,
    -114,  -114,  -114,   282,   157,   133,  -114,  -114,   133,  -114,
    -114,   133,  -114,  -114,  -114,  -114,  -114,   133,   133,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,   133,   303,   133,   133,
     133,   324,  -114,  -114,  -114,  -114,  -114,   183,   387,   387,
      62,   387,   387,   387,  -114,   209,   235,   345,  -114,  -114,
    -114,   133,   106,   106,   133,   387,  -114,  -114,    10,  -114,
     366,   106,   133,  -114,   261,   106,  -114
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -114,  -114,  -114,    77,  -114,    79,  -114,    17,     0,     9,
      59,  -114,   -24,  -113,  -114,  -114,  -114,  -114,  -114,  -114,
     -28,  -114,    52,  -114,  -114,  -114,    51,  -114
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -19
static const yytype_int16 yytable[] =
{
       7,     9,    44,     2,     7,    65,    64,    57,    15,     8,
     129,    10,    13,     8,    36,     2,     7,     7,   133,    16,
      25,    97,   136,    14,     7,   101,    23,     7,     2,    24,
      17,    55,   104,    19,    22,    66,     1,   107,     2,    69,
     108,    29,    68,   109,    33,    26,    28,     2,    27,   111,
     112,    30,    34,    40,    41,    48,    55,    35,    31,    98,
      71,    36,     2,    37,    38,   131,   120,    99,   113,   121,
     115,   116,   117,    72,    73,    74,    75,    76,   100,    34,
     102,    11,   103,    12,    35,    32,    39,    67,    36,     2,
      37,    38,     0,   125,    78,     0,   130,     0,     0,     0,
      40,    41,    49,    50,   134,    51,    52,    53,     0,    34,
       0,    31,     0,    39,    35,     0,     0,     0,    36,     2,
      37,    38,    55,    55,     0,     0,     0,    40,    41,    49,
      50,    55,    51,    52,    53,    55,    34,     0,     0,    71,
       0,    35,     0,    39,    13,    36,     2,    37,    38,   -18,
       0,     0,    72,    73,    74,    75,    76,    40,    41,    49,
      50,   106,    51,    52,    53,    79,     0,     0,     0,     0,
      39,    72,    73,    74,    75,    76,     0,     0,    80,    81,
      82,    83,    84,    85,    40,    41,     0,   119,    86,    87,
      88,    79,    89,    90,    91,    92,    93,    94,    95,     0,
       0,     0,     0,     0,    80,    81,    82,    83,    84,    85,
       0,     0,     0,   122,    86,    87,    88,    79,    89,    90,
      91,    92,    93,    94,    95,     0,     0,     0,     0,     0,
      80,    81,    82,    83,    84,    85,     0,     0,     0,   123,
      86,    87,    88,    79,    89,    90,    91,    92,    93,    94,
      95,     0,     0,     0,     0,     0,    80,    81,    82,    83,
      84,    85,     0,     0,     0,   135,    86,    87,    88,    79,
      89,    90,    91,    92,    93,    94,    95,     0,     0,     0,
       0,     0,    80,    81,    82,    83,    84,    85,     0,     0,
      79,   105,    86,    87,    88,     0,    89,    90,    91,    92,
      93,    94,    95,    80,    81,    82,    83,    84,    85,     0,
       0,    79,   114,    86,    87,    88,     0,    89,    90,    91,
      92,    93,    94,    95,    80,    81,    82,    83,    84,    85,
       0,     0,    79,   118,    86,    87,    88,     0,    89,    90,
      91,    92,    93,    94,    95,    80,    81,    82,    83,    84,
      85,     0,     0,    79,   124,    86,    87,    88,     0,    89,
      90,    91,    92,    93,    94,    95,    80,    81,    82,    83,
      84,    85,     0,     0,    79,   132,    86,    87,    88,     0,
      89,    90,    91,    92,    93,    94,    95,    80,    81,    82,
      83,    84,    85,     0,     0,    79,     0,    86,    87,    88,
       0,    89,    90,    91,    92,    93,    94,    95,    80,    81,
      82,    83,    84,    85,     0,     0,     0,     0,    86,    87,
      88,     0,    89,    90,    91,    92,    93,    94,    95
};

static const yytype_int16 yycheck[] =
{
       0,     1,    30,    13,     4,     3,    34,    31,     8,     0,
     123,     0,     8,     4,    12,    13,    16,    17,   131,     5,
      20,    49,   135,     8,    24,    53,     6,    27,    13,     9,
       3,    31,    56,    16,    17,    35,    11,    65,    13,    39,
      68,    24,     8,    71,    27,     4,     9,    13,     7,    77,
      78,    16,     3,    51,    52,     6,    56,     8,     5,     3,
       3,    12,    13,    14,    15,    55,     4,     3,    96,     7,
      98,    99,   100,    16,    17,    18,    19,    20,     3,     3,
       9,     4,     6,     4,     8,    26,    37,    35,    12,    13,
      14,    15,    -1,   121,    43,    -1,   124,    -1,    -1,    -1,
      51,    52,    53,    54,   132,    56,    57,    58,    -1,     3,
      -1,     5,    -1,    37,     8,    -1,    -1,    -1,    12,    13,
      14,    15,   122,   123,    -1,    -1,    -1,    51,    52,    53,
      54,   131,    56,    57,    58,   135,     3,    -1,    -1,     3,
      -1,     8,    -1,    37,     8,    12,    13,    14,    15,    13,
      -1,    -1,    16,    17,    18,    19,    20,    51,    52,    53,
      54,     4,    56,    57,    58,     8,    -1,    -1,    -1,    -1,
      37,    16,    17,    18,    19,    20,    -1,    -1,    21,    22,
      23,    24,    25,    26,    51,    52,    -1,     4,    31,    32,
      33,     8,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,     4,    31,    32,    33,     8,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,     4,
      31,    32,    33,     8,    35,    36,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    26,    -1,    -1,    -1,     4,    31,    32,    33,     8,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    21,    22,    23,    24,    25,    26,    -1,    -1,
       8,     9,    31,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    41,    21,    22,    23,    24,    25,    26,    -1,
      -1,     8,     9,    31,    32,    33,    -1,    35,    36,    37,
      38,    39,    40,    41,    21,    22,    23,    24,    25,    26,
      -1,    -1,     8,     9,    31,    32,    33,    -1,    35,    36,
      37,    38,    39,    40,    41,    21,    22,    23,    24,    25,
      26,    -1,    -1,     8,     9,    31,    32,    33,    -1,    35,
      36,    37,    38,    39,    40,    41,    21,    22,    23,    24,
      25,    26,    -1,    -1,     8,     9,    31,    32,    33,    -1,
      35,    36,    37,    38,    39,    40,    41,    21,    22,    23,
      24,    25,    26,    -1,    -1,     8,    -1,    31,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,    41,    21,    22,
      23,    24,    25,    26,    -1,    -1,    -1,    -1,    31,    32,
      33,    -1,    35,    36,    37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    13,    60,    61,    62,    64,    67,    68,    67,
       0,    62,    64,     8,     8,    67,     5,     3,    65,    66,
      68,    63,    66,     6,     9,    67,     4,     7,     9,    66,
      16,     5,    69,    66,     3,     8,    12,    14,    15,    37,
      51,    52,    67,    78,    79,    81,    82,    83,     6,    53,
      54,    56,    57,    58,    66,    67,    70,    71,    73,    74,
      75,    76,    77,    79,    79,     3,    67,    81,     8,    67,
      80,     3,    16,    17,    18,    19,    20,    85,    85,     8,
      21,    22,    23,    24,    25,    26,    31,    32,    33,    35,
      36,    37,    38,    39,    40,    41,    86,    79,     3,     3,
       3,    79,     9,     6,    71,     9,     4,    79,    79,    79,
      84,    79,    79,    79,     9,    79,    79,    79,     9,     4,
       4,     7,     4,     4,     9,    79,    69,    71,    72,    72,
      79,    55,     9,    72,    79,     4,    72
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

/* Line 1455 of yacc.c  */
#line 113 "parser.y"
    {
			program = (yyvsp[(1) - (1)].decls);
			(yyval.decls) = (yyvsp[(1) - (1)].decls);
		;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 120 "parser.y"
    {
			(yyval.decls) = new NDeclarations();
			(yyval.decls)->definitions.push_back((yyvsp[(1) - (1)].function));
		;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 125 "parser.y"
    {
			(yyval.decls) = new NDeclarations();
			(yyval.decls)->definitions.push_back((yyvsp[(1) - (1)].structure));
		;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 130 "parser.y"
    {
			(yyvsp[(1) - (2)].decls)->definitions.push_back((yyvsp[(2) - (2)].function));
		;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 134 "parser.y"
    {
			(yyvsp[(1) - (2)].decls)->definitions.push_back((yyvsp[(2) - (2)].structure));
		;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 140 "parser.y"
    {
			(yyval.function) = new NFunctionDeclaration(*(yyvsp[(1) - (6)].type), *(yyvsp[(2) - (6)].ident), *(yyvsp[(4) - (6)].varvec), *(yyvsp[(6) - (6)].block));
			//delete $4;
		;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 147 "parser.y"
    {
			(yyval.varvec) = new VariableList();
		;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 151 "parser.y"
    {
			(yyval.varvec) = new VariableList();
			(yyval.varvec)->push_back((yyvsp[(1) - (1)].variable));
		;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 156 "parser.y"
    {
			(yyvsp[(1) - (3)].varvec)->push_back((yyvsp[(3) - (3)].variable));
		;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 162 "parser.y"
    {
			(yyval.structure) = new NStructureDeclaration(*(yyvsp[(2) - (6)].ident), *(yyvsp[(4) - (6)].varvec));
			//delete $4;
		;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 169 "parser.y"
    {
			(yyval.varvec) = new VariableList();
		;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 173 "parser.y"
    {
			(yyval.varvec) = new VariableList();
			(yyval.varvec)->push_back((yyvsp[(1) - (1)].variable));
		;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 178 "parser.y"
    {
			(yyvsp[(1) - (3)].varvec)->push_back((yyvsp[(3) - (3)].variable));
		;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 184 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(yyvsp[(1) - (2)].type), *(yyvsp[(2) - (2)].ident));
		;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 188 "parser.y"
    {
			(yyval.variable) = new NVariableDeclaration(*(yyvsp[(1) - (4)].type), *(yyvsp[(2) - (4)].ident), (yyvsp[(4) - (4)].expr));
		;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 194 "parser.y"
    {
			(yyval.ident) = new NIdentifier(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 201 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (1)].type)->name, 0);
			delete (yyvsp[(1) - (1)].ident);
		;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 206 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (2)].type)->name, 1);
			delete (yyvsp[(1) - (2)].ident);
		;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 211 "parser.y"
    {
			(yyval.type) = new NType((yyvsp[(1) - (2)].type)->name, (yyvsp[(1) - (2)].type)->pointerCount + 1);
			delete (yyvsp[(1) - (2)].type);
		;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 218 "parser.y"
    {
			(yyval.block) = (yyvsp[(2) - (3)].block);
		;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 222 "parser.y"
    {
			(yyval.block) = new NBlock();
		;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 228 "parser.y"
    {
			(yyval.block) = new NBlock();
			(yyval.block)->statements.push_back((yyvsp[(1) - (1)].stmt));
		;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 233 "parser.y"
    {
			(yyvsp[(1) - (2)].block)->statements.push_back((yyvsp[(2) - (2)].stmt));
		;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 239 "parser.y"
    {
			(yyval.stmt) = (yyvsp[(1) - (2)].variable);
		;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 248 "parser.y"
    {
			(yyval.stmt) = new NExpressionStatement(*(yyvsp[(1) - (2)].expr));
		;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 254 "parser.y"
    {
			(yyval.block) = new NBlock();
			(yyval.block)->statements.push_back((yyvsp[(1) - (1)].stmt));
		;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 259 "parser.y"
    {
			(yyval.block) = (yyvsp[(1) - (1)].block);
		;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 265 "parser.y"
    {
			(yyval.stmt) = new NIfStatement(*(yyvsp[(3) - (7)].expr), *(yyvsp[(5) - (7)].block), (yyvsp[(7) - (7)].block));
		;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 269 "parser.y"
    {
			(yyval.stmt) = new NIfStatement(*(yyvsp[(3) - (5)].expr), *(yyvsp[(5) - (5)].block));
		;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 275 "parser.y"
    {
			(yyval.stmt) = new NReturnStatement(*(yyvsp[(2) - (3)].expr));
		;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 281 "parser.y"
    {
			(yyval.stmt) = new NDebugStatement(*(yyvsp[(2) - (3)].expr));
		;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 287 "parser.y"
    {
			(yyval.stmt) = new NWhileStatement(*(yyvsp[(3) - (5)].expr), *(yyvsp[(5) - (5)].block));
		;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 293 "parser.y"
    {
			(yyval.stmt) = new NForStatement(*(yyvsp[(3) - (9)].expr), *(yyvsp[(5) - (9)].expr), *(yyvsp[(7) - (9)].expr), *(yyvsp[(9) - (9)].block));
		;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 299 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(2) - (2)].ident));
		;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 303 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(2) - (2)].expr));
		;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 307 "parser.y"
    {
			(yyval.expr) = new NDereferenceOperator(*(yyvsp[(3) - (4)].expr));
		;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 311 "parser.y"
    {
			if ((yyvsp[(1) - (3)].expr)->cType == "expression-dereference") // We can't accept NAssignments as the deref in this case.
				(yyval.expr) = new NAssignment(new NAssignmentDereference(((NDereferenceOperator*)(yyvsp[(1) - (3)].expr))->expr), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
			else
				throw new CompilerException("Unable to apply dereferencing assignment operation to non-dereference operator based LHS.");
		;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 320 "parser.y"
    {
			(yyval.expr) = new NAssignment(new NAssignmentIdentifier(*(yyvsp[(1) - (3)].ident)), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
		;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 324 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 328 "parser.y"
    {
			(yyval.expr) = new NMethodCall(*(yyvsp[(1) - (4)].ident), *(yyvsp[(3) - (4)].exprvec));
			delete (yyvsp[(3) - (4)].exprvec);
		;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 333 "parser.y"
    {
			(yyval.ident) = (yyvsp[(1) - (1)].ident);
		;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 337 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 341 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 345 "parser.y"
    {
			(yyval.expr) = (yyvsp[(1) - (1)].expr);
		;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 349 "parser.y"
    {
			(yyval.expr) = new NBinaryOperator(*(yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].token), *(yyvsp[(3) - (3)].expr));
		;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 353 "parser.y"
    {
			(yyval.expr) = new NAddressOfOperator(*(yyvsp[(2) - (2)].exprassign));
		;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 357 "parser.y"
    {
			(yyval.expr) = (yyvsp[(2) - (3)].expr);
		;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 363 "parser.y"
    {
			(yyval.exprassign) = new NAssignmentIdentifier(*(yyvsp[(1) - (1)].ident));
		;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 367 "parser.y"
    {
			(yyval.exprassign) = new NAssignmentDereference(*(yyvsp[(2) - (2)].expr));
		;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 373 "parser.y"
    {
			(yyval.expr) = new NInteger((yyvsp[(1) - (1)].number));
		;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 377 "parser.y"
    {
			(yyval.expr) = new NInteger(1);
		;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 381 "parser.y"
    {
			(yyval.expr) = new NInteger(0);
		;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 387 "parser.y"
    {
			(yyval.expr) = new NCharacter(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 394 "parser.y"
    {
			(yyval.expr) = new NString(*(yyvsp[(1) - (1)].string));
			delete (yyvsp[(1) - (1)].string);
		;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 401 "parser.y"
    {
			(yyval.exprvec) = new ExpressionList();
		;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 405 "parser.y"
    {
			(yyval.exprvec) = new ExpressionList();
			(yyval.exprvec)->push_back((yyvsp[(1) - (1)].expr));
		;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 410 "parser.y"
    {
			(yyvsp[(1) - (3)].exprvec)->push_back((yyvsp[(3) - (3)].expr));
		;}
    break;



/* Line 1455 of yacc.c  */
#line 2155 "parser.cpp"
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



