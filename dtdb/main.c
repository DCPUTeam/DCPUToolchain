/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Debugger

	Authors:	José Manuel Díez

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

#include <bstrlib.h>

#include <dbgaux.h>
#include <lexfix.h>
#include <parser.h>
#include <lexer.h>
#include <dcpuhook.h>
#include <version.h>

#include "sdp.h"

#define MAX_ARGUMENTS 10
#define MAX_ARGUMENT_LENGTH 50

bstring path;
vm_t* vm;
#ifdef FEATURE_SDP
pthread_t sdp_thread;
#endif

extern int dbg_yyparse(void* scanner);

void ddbg_sigint(int signal)
{
	vm->halted = true;
	printf("(interrupt)\n");
}

void get_command(char* command_buffer, int max)
{
	printf("> ");
	fgets(command_buffer, max, stdin);
}

int main(int argc, char** argv)
{
	char* buf;
	yyscan_t scanner;

	// Set global path variable.
	osutil_setarg0(bautofree(bfromcstr(argv[0])));

	// Register signal handler.
	signal(SIGINT, ddbg_sigint);

	// Create VM.
	ddbg_create_vm();

	// Create SDP thread if supported.
#ifdef FEATURE_SDP
	pthread_create(&sdp_thread, NULL, (void*)ddbg_sdp_thread, vm);
#endif
	version_print(bautofree(bfromcstr("Debugger")));

	for (;;)
	{
		buf = readline("> ");
		add_history(buf);

		dbg_yylex_init(&scanner);
		dbg_yy_scan_string(buf, scanner);
		dbg_yyparse(scanner);
		dbg_yylex_destroy(scanner);
	}

	free(buf);

	return 0;
}
