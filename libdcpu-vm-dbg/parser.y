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
#include "aux.h"

void yyerror(void* scanner, const char *str);

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
%token <token> ID_ATTACH ID_INSPECT ID_HARDWARE ID_CPU ID_DETACH ID_LIST ID_MEMORY
%token <token> COLON
%token <string> PARAM PATH CHARACTER STRING
%token <number> ADDRESS

// Start at the root node.
%start command

// We need to pass in a scanner object
// so that we can scan via strings.
%pure-parser
%name-prefix="dbg_yy"
%lex-param {void* scanner}
%parse-param {void* scanner}

%%

command:
		general_command |
		breakpoint_command |
		hardware_command |
		cpu_command |
		memory_command ;
		
general_command:
		ID_LOAD PATH
		{
			// Path is in $2.
			ddbg_load($2);
			ddbg_flash_vm();
		} |
		ID_RUN
		{
			// Run currently loaded program from
			// beginning.  Prompt / warn if the program
			// has already partially started.
			ddbg_run_vm();
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
			exit(0);
		} ;

breakpoint_command:
		breakpoint_add_command |
		breakpoint_list_command |
		breakpoint_delete_command ;

breakpoint_add_command:
		ID_BREAKPOINT ID_ADD ID_MEMORY COLON ADDRESS
		{
			// Add a breakpoint in memory
			// at the specified RAM address.
			ddbg_add_breakpoint(bfromcstr("memory"), $5);
		} |
		ID_ADD ID_BREAKPOINT ID_MEMORY COLON ADDRESS
		{
			// Add a breakpoint in memory
			// at the specified RAM address.
			ddbg_add_breakpoint(bfromcstr("memory"), $5);
		} |
		ID_BREAKPOINT ID_ADD PATH COLON ADDRESS
		{
			// Add a breakpoint in the specified file
			// at the specified line number.
			ddbg_add_breakpoint($3, $5);
		} |
		ID_ADD ID_BREAKPOINT PATH COLON ADDRESS
		{
			// Add a breakpoint in the specified file
			// at the specified line number.
			ddbg_add_breakpoint($3, $5);
		} ;

breakpoint_list_command:
		ID_BREAKPOINT ID_LIST
		{
			// List all of the breakpoints.
			ddbg_breakpoints_list();
		} |
		ID_LIST ID_BREAKPOINT
		{
			// List all of the breakpoints.
			ddbg_breakpoints_list();
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
		ID_CPU ID_INSPECT
		{
			// Inspect the state of DCPU.
			ddbg_dump_state();
		} |
		ID_INSPECT ID_CPU
		{
			// Inspect the state of DCPU.
			ddbg_dump_state();
		} ;

memory_command:
		memory_inspect_command ;

memory_inspect_command:
		ID_MEMORY ID_INSPECT ADDRESS
		{
			// Inspect the state of memory.
			ddbg_dump_ram($3, 0);
		} |
		ID_INSPECT ID_MEMORY ADDRESS
		{
			// Inspect the state of memory.
			ddbg_dump_ram($3, 0);
		} |
		ID_MEMORY ID_INSPECT ADDRESS ADDRESS
		{
			// Inspect the state of memory.
			ddbg_dump_ram($3, $4);
		} |
		ID_INSPECT ID_MEMORY ADDRESS ADDRESS
		{
			// Inspect the state of memory.
			ddbg_dump_ram($3, $4);
		} ;

%%

#include "lexer.h"

void yyerror(void* scanner, const char *str)
{
    fprintf(stderr,"Unable to parse command: %s\n", str);
}