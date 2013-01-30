/**

    File:       aerr.c

    Project:    DCPU-16 Tools
    Component:  Assembler

    Authors:    James Rhodes

    Description:    Implements error handling infrastructure
            so that the toolchain can safely handle
            errors as they occur.

**/

#define INTERNAL_DERR_REFERENCING

#include <stdlib.h>
#include <setjmp.h>
#include <ppexpr.h>
#include <argtable2.h>
#include <assert.h>
#include "debug.h"
#include "derr.h"

// Error definition
jmp_buf __derrjmp;
bool __derrset = false;
struct errinfo* __derrcurrent = NULL;

struct errinfo* derrinfo()
{
    return __derrcurrent;
}

///
/// @brief Raises an error.
///
void dhalt(int errid, const char* errdata)
{
    if (!__derrset)
    {
        fprintf(stderr, "FATAL: Error handler not set during halting operation!\n");
        fprintf(stderr, "       This indicates a bug in the program using libdcpu.\n");
        fprintf(stderr, "       All applications using libdcpu must use 'dsethalt'\n");
        fprintf(stderr, "       to capture and respond to error messages.\n");
        fprintf(stderr, "       Please report this to the application's maintainer.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "       The original error ID thrown by the toolchain was (%i).\n", errid);
        fprintf(stderr, "\n");
        exit(1);
        return;
    }

    __derrcurrent = malloc(sizeof(struct errinfo));
    __derrcurrent->errid = errid;
    __derrcurrent->errdata = errdata;
    __derrcurrent->has_location = false;
    __derrcurrent->line = 0;
    __derrcurrent->filename = NULL;
    longjmp(__derrjmp, 1);
    assert(false);
}

///
/// @brief Raises an error with location information.
///
void dhaltloc(int errid, const char* errdata, unsigned int line, const char* filename)
{
    if (!__derrset)
    {
        fprintf(stderr, "FATAL: Error handler not set during halting operation!\n");
        fprintf(stderr, "       This indicates a bug in the program using libdcpu.\n");
        fprintf(stderr, "       All applications using libdcpu must use 'dsethalt'\n");
        fprintf(stderr, "       to capture and respond to error messages.\n");
        fprintf(stderr, "       Please report this to the application's maintainer.\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "       The original error ID thrown by the toolchain was (%i).\n", errid);
        fprintf(stderr, "\n");
        exit(1);
        return;
    }

    __derrcurrent = malloc(sizeof(struct errinfo));
    __derrcurrent->errid = errid;
    __derrcurrent->errdata = errdata;
    __derrcurrent->has_location = true;
    __derrcurrent->line = line;
    __derrcurrent->filename = filename;
    longjmp(__derrjmp, 1);
    assert(false);
}

///
/// @brief Raises a warning.
///
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

///
/// @brief Raises a warning with location information.
///
void dwarnloc(int errid, const char* errdata, unsigned int line, const char* filename)
{
    const char* prepend = "warning: ";
    int msglen;
    char* msg;

    if (dwarnpolicy[errid - _WARN_OFFSET].errid == errid &&
            dwarnpolicy[errid - _WARN_OFFSET].treat_as_error)
        dhaltloc(errid, errdata, line, filename);
    if (dwarnpolicy[errid - _WARN_OFFSET].errid == errid &&
            !dwarnpolicy[errid - _WARN_OFFSET].silence)
    {
        // FIXME: Use bstrings here.
        msglen = strlen(derrstr[errid]) + strlen(prepend) + 1;
        msg = malloc(msglen);
        memset(msg, '\0', msglen);
        strcat(msg, prepend);
        strcat(msg, derrstr[errid]);
        printd(LEVEL_WARNING, msg, errdata, line, filename);
    }
}

///
/// @brief Automatically handles formatting and printing an error after dhalt is fired.
///
/// This function automatically handles retrieving the error information and displaying
/// it back to the user after the dhalt() or dhaltloc() functions are called.
///
void dautohandle()
{
    const char* prepend = "error: ";
    struct errinfo* errval = derrinfo();
    bstring msgc = bfromcstr("");
    
    bcatcstr(msgc, prepend);
    if (errval->has_location)
        bformata(msgc, "%s:%u: ", errval->filename, errval->line);
    bformata(msgc, derrstr[errval->errid], errval->errdata);
    printd(LEVEL_ERROR, msgc->data);
    bdestroy(msgc);
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
