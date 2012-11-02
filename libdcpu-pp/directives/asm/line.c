#include "../asm.h"
#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <derr.h>

static void line_handle(state_t* state, match_t* match, bool* reprocess)
{
    list_t* result = ppparam_get(state);

    // Ensure the parameter format is correct.
    if (list_size(result) == 2 &&
            ((parameter_t*)list_get_at(result, 0))->type == NUMBER &&
            (((parameter_t*)list_get_at(result, 1))->type == STRING ||
             ((parameter_t*)list_get_at(result, 1))->type == WORD))
    {
        // Correct format.
        ppimpl_printf(state, "# %i %s\n",
                      ((parameter_t*)list_get_at(result, 0))->number,
                      ((parameter_t*)list_get_at(result, 1))->string->data);

        // Also update the current input line and filename.
        state->current_line = ((parameter_t*)list_get_at(result, 0))->number;
        if (state->current_filename != NULL)
            bdestroy(state->current_filename);
        state->current_filename = bstrcpy(((parameter_t*)list_get_at(result, 1))->string);
    }
    else
        dhalt(ERR_PP_ASM_LINE_PARAMETERS_INCORRECT, ppimpl_get_location(state));

    ppparam_free(result);
}

static void uline_handle(state_t* state, match_t* match, bool* reprocess)
{
    list_t* result = ppparam_get(state);

    // Ensure the parameter format is correct.
    if (list_size(result) == 2 &&
            ((parameter_t*)list_get_at(result, 0))->type == NUMBER &&
            (((parameter_t*)list_get_at(result, 1))->type == STRING ||
             ((parameter_t*)list_get_at(result, 1))->type == WORD))
    {
        // Correct format.
        ppimpl_printf(state, "#U %i %s\n",
                      ((parameter_t*)list_get_at(result, 0))->number,
                      ((parameter_t*)list_get_at(result, 1))->string->data);
    }
    else
        dhalt(ERR_PP_ASM_ULINE_PARAMETERS_INCORRECT, ppimpl_get_location(state));

    ppparam_free(result);
}

void ppimpl_asm_line_register(state_t* state)
{
    // Register .LINE directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".LINE "));
    match->handler = line_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register .ULINE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".ULINE "));
    match->handler = uline_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #LINE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#LINE "));
    match->handler = line_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #ULINE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#ULINE "));
    match->handler = uline_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);
}

