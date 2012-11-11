#include "../c.h"
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
             ((parameter_t*)list_get_at(result, 1))->type == _WORD))
    {
        // Correct format.
        ppimpl_printf(state, "# %i \"%s\"\n",
                      ((parameter_t*)list_get_at(result, 0))->number,
                      ((parameter_t*)list_get_at(result, 1))->string->data);

        // Also update the current input line and filename.
        state->current_line = ((parameter_t*)list_get_at(result, 0))->number;
        if (state->current_filename != NULL)
            bdestroy(state->current_filename);
        state->current_filename = bstrcpy(((parameter_t*)list_get_at(result, 1))->string);
    }
    else
        dhalt(ERR_PP_C_LINE_PARAMETERS_INCORRECT, ppimpl_get_location_prev(state));

    ppparam_free(result);
}

void ppimpl_c_line_register(state_t* state)
{
    // Register #line directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#line "));
    match->handler = line_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);
}
