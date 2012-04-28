%{
	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		LibDCPU-VM-DBG

	Authors:		James Rhodes

	Description:		Defines parser for the debugger.

**/

#define YYERROR_VERBOSE

#include <bstrlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern unsigned int yylineno;
int yylex();

void yyerror(const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", yylineno, str);
}
 
int yywrap()
{
    return 1;
}

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	bstring string;
	int* token;
}

// Define our lexical token names.
%token <token> ID_LOAD ID_BREAKPOINT ID_RUN ID_CONTINUE ID_STOP ID_QUIT ID_ADD ID_DELETE
%token <token> ID_ATTACH ID_INSPECT ID_HARDWARE ID_CPU ID_DETACH COLON
%token <string> PARAM PATH CHARACTER STRING
%token <number> ADDRESS

// Start at the root node.
%start command

%%

command:
		general_command |
		breakpoint_command |
		hardware_command |
		cpu_command ;

general_command:
		ID_LOAD PATH
		{
			// Path is in $2.
		} |
		ID_RUN
		{
			// Run currently loaded program from
			// beginning.  Prompt / warn if the program
			// has already partially started.
		} |
		ID_CONTINUE
		{
			// Continue execution of paused program.
		} |
		ID_STOP
		{
			// Stop currently running program and reset
			// to beginning.
		} |
		ID_QUIT
		{
			// Stop debugger.
		} ;

breakpoint_command:
		breakpoint_add_command |
		breakpoint_delete_command ;

breakpoint_add_command:
		ID_BREAKPOINT ID_ADD PATH COLON ADDRESS
		{
			// Add a breakpoint in the specified file ($3)
			// at the specified line number ($5).
		} |
		ID_ADD ID_BREAKPOINT PATH COLON ADDRESS
		{
			// Add a breakpoint in the specified file ($3)
			// at the specified line number ($5).
		} ;

breakpoint_delete_command:
		ID_BREAKPOINT ID_DELETE PATH COLON ADDRESS
		{
			// Delete a breakpoint in the specified file ($3)
			// at the specified line number ($5).
		} |
		ID_DELETE ID_BREAKPOINT PATH COLON ADDRESS
		{
			// Delete a breakpoint in the specified file ($3)
			// at the specified line number ($5).
		} ;

hardware_command:
		hardware_attach_command |
		hardware_detach_command |
		hardware_inspect_command ;

hardware_attach_command:
		ID_HARDWARE ID_ATTACH PARAM
		{
			// Attach a specified piece of hardware to
			// the DCPU.
		} |
		ID_ATTACH ID_HARDWARE PARAM
		{
			// Attach a specified piece of hardware to
			// the DCPU.
		} ;

hardware_detach_command:
		ID_HARDWARE ID_DETACH PARAM
		{
			// Detach a specified piece of hardware from
			// the DCPU.
		} |
		ID_DETACH ID_HARDWARE PARAM
		{
			// Detach a specified piece of hardware from
			// the DCPU.
		} ;

hardware_inspect_command:
		ID_HARDWARE ID_INSPECT PARAM
		{
			// Inspect a specified piece of hardware on
			// the DCPU.
		} |
		ID_INSPECT ID_HARDWARE PARAM
		{
			// Inspect a specified piece of hardware on
			// the DCPU.
		} ;

cpu_command:
		cpu_inspect_command ;

cpu_inspect_command:
		ID_CPU ID_INSPECT PARAM
		{
			// Inspect the state of DCPU.
		} |
		ID_DETACH ID_CPU PARAM
		{
			// Inspect the state of DCPU.
		} ;
