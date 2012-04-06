//
// DCPU Compiler by James Rhodes
//
// Main entry point.
//

#include <cstdlib>
#include <cstdio>
#include "Node.h"
extern "C"
{
	#include "pp.h"
}
extern int yyparse();
extern FILE *yyin, *yyout;
extern NDeclarations* program;

int main(int argc, char* argv[])
{
	// Parse C.
	yyin = pp_do(argv[1]);
	if (yyin == NULL)
	{
		pp_cleanup();
		return 1;
	}
	yyparse();
	fclose(yyin);
	
	pp_cleanup();
	return 0;
}