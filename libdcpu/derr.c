/**

	File:		aerr.c

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Implements error handling infrastructure
		    so that the toolchain can safely handle
		    errors as they occur.

**/

#include <stdlib.h>
#include <setjmp.h>
#include <ppexpr.h>
#include <argtable2.h>
#include <assert.h>
#include "debug.h"
#include "derr.h"

// Error definition
jmp_buf __derrjmp;

// Utility method for throwing errors.
void dhalt(int errid, const char* errdata)
{
	struct errinfo* err = malloc(sizeof(struct errinfo));
	err->errid = errid;
	err->errdata = errdata;
	longjmp(__derrjmp, (long)err);
    assert(false);
}

void dwarn(int errid, const char* errdata)
{
	const char* prepend = "warning: ";
	int msglen;
	char* msg;

	if (dwarnpolicy[errid - _WARN_OFFSET].errid == errid &&
		dwarnpolicy[errid - _WARN_OFFSET].treat_as_error)
		dhalt(errid, errdata);
	if (dwarnpolicy[errid - _WARN_OFFSET].errid == errid &&
		!dwarnpolicy[errid - _WARN_OFFSET].silence)
	{
		// FIXME: Use bstrings here.
		msglen = strlen(derrstr[errid]) + strlen(prepend) + 1;
		msg = malloc(msglen);
		memset(msg, '\0', msglen);
		strcat(msg, prepend);
		strcat(msg, derrstr[errid]);
		printd(LEVEL_WARNING, msg, errdata);
	}
}

void dsetwarnpolicy(struct arg_str* warning_policies)
{
    int i, w;

	// Set up warning policies.
	for (i = 0; i < warning_policies->count; i++)
	{
		if (strcmp(warning_policies->sval[i], "all") == 0)
			for (w = 0; w < _WARN_COUNT; w++)
				dwarnpolicy[w].silence = false;
		else if (strcmp(warning_policies->sval[i], "error") == 0)
			for (w = 0; w < _WARN_COUNT; w++)
		dwarnpolicy[w].treat_as_error = true;
		else
		{
			// Loop through enabling any that match.
			for (w = 0; w < _WARN_COUNT; w++)
			{
				if (strcmp(dwarnpolicy[w].name, warning_policies->sval[i]) == 0)
					dwarnpolicy[w].silence = false;
			}

			// Loop through disabling any that match.
			if (strlen(warning_policies->sval[i]) > 3 && warning_policies->sval[i][0] == 'n' &&
				warning_policies->sval[i][1] == 'o' && warning_policies->sval[i][2] == '-')
			{
				for (w = 0; w < _WARN_COUNT; w++)
				{
					if (strcmp(dwarnpolicy[w].name, warning_policies->sval[i] + 3) == 0)
						dwarnpolicy[w].silence = true;
				}
			}

	    // Loop through any prefixed with error.
			if (strlen(warning_policies->sval[i]) > 6 && warning_policies->sval[i][0] == 'e' &&
				warning_policies->sval[i][1] == 'r' && warning_policies->sval[i][2] == 'r' &&
		warning_policies->sval[i][3] == 'o' && warning_policies->sval[i][4] == 'r' &&
		warning_policies->sval[i][5] == '=')
			{
				for (w = 0; w < _WARN_COUNT; w++)
				{
					if (strcmp(dwarnpolicy[w].name, warning_policies->sval[i] + 6) == 0)
			dwarnpolicy[w].treat_as_error = true;
				}
			}
	    
	    // Loop through any prefixed with no-error.
			if (strlen(warning_policies->sval[i]) > 9 && warning_policies->sval[i][0] == 'n' &&
				warning_policies->sval[i][1] == 'o' && warning_policies->sval[i][2] == '-' &&
		warning_policies->sval[i][3] == 'e' && warning_policies->sval[i][4] == 'r' &&
		warning_policies->sval[i][5] == 'r' && warning_policies->sval[i][6] == 'o' &&
				warning_policies->sval[i][7] == 'r' && warning_policies->sval[i][8] == '=')
			{
				for (w = 0; w < _WARN_COUNT; w++)
				{
					if (strcmp(dwarnpolicy[w].name, warning_policies->sval[i] + 9) == 0)
			dwarnpolicy[w].treat_as_error = true;
				}
			}
		}
	}
}

uint16_t dhalt_label_resolution_not_permitted(bstring name)
{
	dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, name->data);
	return 0;
}

void dhalt_expression_exit_handler(int code, void* data)
{
	switch (code)
	{
		case EXPR_EXIT_LABEL_NOT_FOUND:
			dhalt(ERR_LABEL_NOT_FOUND, ((bstring)data)->data);
		case EXPR_EXIT_DIVIDE_BY_ZERO:
			dhalt(ERR_EXPRESSION_DIVIDE_BY_ZERO, ((bstring)data)->data);
		default:
			dhalt(ERR_GENERIC, NULL);
	}
}
