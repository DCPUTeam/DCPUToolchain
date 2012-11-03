/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Linker

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <stdio.h>
#include <bstring.h>
#include <parser.h>

extern int yyparse();
extern FILE* yyin, *yyout;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("usage: dtpolicy <file>\n");
        return 1;
    }
    
    // parse
    yyin = fopen(argv[1], "r");
    yyout = NULL;
    yyparse();
    fclose(yyin);
    
    return 0;
}

