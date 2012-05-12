%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-vm-dbg

	Authors:        James Rhodes

	Description:    Defines parser for the debugger.

**/

#define YYERROR_VERBOSE

#include <lexfix.h>
#include <bstrlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dbgaux.h"

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
%token <token> ID_ATTACH ID_INSPECT ID_HARDWARE ID_CPU ID_DETACH ID_LIST ID_MEMORY ID_HELP
%token <token> ID_DISASSEMBLE ID_SYMBOLS COLON
%token <string> PARAM PATH CHARACTER STRING
%token <number> ADDRESS

// Define our parser types.
%type <string> param

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
		memory_command |
		help_command ;

param:
		PATH |
		PARAM ;
	
general_command:
		ID_LOAD ID_SYMBOLS PATH
		{
			// Path is in $3.
			ddbg_load_symbols($3);
		} |
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
			ddbg_continue_vm();
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
		} |
		ID_INSPECT ID_SYMBOLS
		{
			// Show a list of all of the loaded symbols.
			ddbg_inspect_symbols();
		} ;

breakpoint_command:
		breakpoint_add_command |
		breakpoint_list_command |
		breakpoint_delete_command ;

breakpoint_add_command:
		ID_BREAKPOINT ADDRESS
		{
			// Add a breakpoint in memory
			// at the specified RAM address.
			ddbg_add_breakpoint(bfromcstr("memory"), $2);
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
		ID_BREAKPOINT ID_DELETE ADDRESS
		{
			ddbg_delete_breakpoint(bfromcstr("memory"), $3);
		} |
		ID_BREAKPOINT ID_DELETE PATH COLON ADDRESS
		{
			// Delete a breakpoint in the specified file ($3)
			// at the specified line number ($5).
			ddbg_delete_breakpoint($3, $5);
		} |
		ID_DELETE ID_BREAKPOINT PATH COLON ADDRESS
		{
			// Delete a breakpoint in the specified file ($3)
			// at the specified line number ($5).
			ddbg_delete_breakpoint($3, $5);
		} ;

hardware_command:
		hardware_attach_command |
		hardware_detach_command |
		hardware_inspect_command ;

hardware_attach_command:
		ID_HARDWARE ID_ATTACH param
		{
			// Attach a specified piece of hardware to
			// the DCPU.
			ddbg_attach($3);
		} |
		ID_ATTACH ID_HARDWARE param
		{
			// Attach a specified piece of hardware to
			// the DCPU.
			ddbg_attach($3);
		} ;

hardware_detach_command:
		ID_HARDWARE ID_DETACH param
		{
			// Detach a specified piece of hardware from
			// the DCPU.
			printf("Not implemented.");
		} |
		ID_DETACH ID_HARDWARE param
		{
			// Detach a specified piece of hardware from
			// the DCPU.
			printf("Not implemented.");
		} ;

hardware_inspect_command:
		ID_HARDWARE ID_INSPECT param
		{
			// Inspect a specified piece of hardware on
			// the DCPU.
			printf("Not implemented.");
		} |
		ID_INSPECT ID_HARDWARE param
		{
			// Inspect a specified piece of hardware on
			// the DCPU.
			printf("Not implemented.");
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
		memory_inspect_command |
		memory_disassemble_command ;

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

memory_disassemble_command:
		ID_DISASSEMBLE
		{
			// Disassemble memory.
			ddbg_disassemble(0, 0);
		} |
		ID_DISASSEMBLE ADDRESS
		{
			// Disassemble memory.
			ddbg_disassemble($2, 0);
		} |
		ID_DISASSEMBLE ADDRESS ADDRESS
		{
			// Disassemble memory.
			ddbg_disassemble($2, $3);
		} ;

help_command:
		ID_HELP
		{
			ddbg_help(bfromcstr("general"));
		} | 
		ID_HELP ID_BREAKPOINT
		{
			ddbg_help(bfromcstr("break"));
		} |
		ID_HELP ID_LOAD
		{
			ddbg_help(bfromcstr("load"));
		} |
		ID_HELP ID_RUN
		{
			ddbg_help(bfromcstr("run"));
		} |
		ID_HELP ID_CONTINUE
		{
			ddbg_help(bfromcstr("continue"));
		} |
		ID_HELP ID_INSPECT
		{
			ddbg_help(bfromcstr("inspect"));
		} |
		ID_HELP ID_QUIT
		{
			ddbg_help(bfromcstr("quit"));
		} ;
		
%%

#include "lexer.h"

void yyerror(void* scanner, const char *str)
{
    fprintf(stderr,"Unable to parse command: %s\n", str);
}
