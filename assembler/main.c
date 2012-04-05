//
// DCPU Assembler by James Rhodes
//
// Main entry point.
//

#include <stdio.h>
#include "node.h"

extern int yyparse();
extern FILE *yyin, *yyout;

int main(int argc, char* argv[])
{
	yyin = fopen(argv[1], "r");
	yyparse();

	return 0;
}