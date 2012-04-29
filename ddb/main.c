/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

	Description:	Main entry point.

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <libgen.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dcpu.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

#include <signal.h>

#include <parser.h>
#include <lexer.h>
#include <dcpuhook.h>


#include "sdp.h"

#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 50


char* path;
extern vm_t* vm;
pthread_t sdp_thread;

extern int dbg_yyparse(void* scanner);

#ifdef _WIN32
char* dirname(char* fixpath)
{
	// FIXME: This assumes the resulting path will always
	// be shorter than the original (which it should be
	// given that we're only returning a component of it, right?)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(fixpath, drive, dir, NULL, NULL);
	strcpy(fixpath, drive);
	strcpy(fixpath + strlen(fixpath), dir);
	return fixpath;
}
#endif

void ddbg_sigint(int signal) {
	pthread_kill(sdp_thread, SIGTERM);
	exit(0);
}


void get_command(char* command_buffer, int max) {
	printf("> ");
	fgets(command_buffer, max, stdin);
}	


int main(int argc, char** argv) {
	char* buf;
	char** args;
	char* tmp;
	unsigned int num_args = 0;
	unsigned int i = 0;
	int child = 0;
	yyscan_t scanner;
	
	args = malloc(sizeof(char*) * MAX_ARGUMENTS);
	for(i = 0; i < MAX_ARGUMENTS; i++) args[i] = malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
	
	// Set global path variable.
	path = strdup(argv[0]);
	path = (char*) dirname(path);
	
	signal(SIGINT, ddbg_sigint);
	
	//ddbg_quickstart("qt.o");
	//ddbg_run_vm();
	//ddbg_dump_state();
	
	pthread_create(&sdp_thread, NULL, (void*)ddbg_sdp_thread, vm);
	printf("Welcome to the DCPU Toolchain Debugger, the best debugger in the multiverse.\n");
	
	for(;;) {
		buf = readline("> ");
		add_history (buf);
		
		dbg_yylex_init(&scanner);
		dbg_yy_scan_string(buf, scanner);
		dbg_yyparse(scanner);
		dbg_yylex_destroy(scanner);
		/*yylex_init(&scanner);
		buffer = yy_scan_buffer(buf, strlen(buf)+2, scanner);
		yy_switch_to_buffer(buffer, scanner);
		yyparse(scanner);
		/*yy_delete_buffer(buffer, scanner);
		yylex_destroy(&scanner);*/
	}
	
    free(buf);
	
	return 0;
}
