#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <bstring.h>
#include <bfile.h>
#include <derr.h>
#include "../asm.h"
#include "../../ppfind.h"

static void include_handle(state_t* state, match_t* match, bool* reprocess)
{
    bstring path;
    BFILE* file;
    list_t* result = ppparam_get(state);
    char* store = malloc(1);
    size_t pos = 0;

    // Ensure the parameter format is correct.
    if (list_size(result) == 1 &&
            (((parameter_t*)list_get_at(result, 0))->type == STRING ||
            ((parameter_t*)list_get_at(result, 0))->type == ANGLED_STRING))
    {
        // Get the filename.
        path = ((parameter_t*)list_get_at(result, 0))->string;

        // If the path is in quotes, then find it relative to the local
        // directory.  Otherwise, if it's in angle brackets, search for it.
        if (((parameter_t*)list_get_at(result, 0))->type == ANGLED_STRING ||
            ((parameter_t*)list_get_at(result, 0))->type == STRING)
            path = ppfind_locate(bautocpy(path));

        // Open the specified file.
        file = bfopen(path->data, "r");
        if (file == NULL)
            dhalt(ERR_PP_ASM_INCLUDE_FILE_NOT_FOUND, ppimpl_get_location(state));

        // Copy the data from the file, byte by byte.
        ppimpl_printf(state, "\n# %i %s\n", state->current_line, state->current_filename->data);
        while (!bfeof(file))
        {
            bfread(store, 1, 1, file);
            ppimpl_pprintf(state, pos++, "%c", store[0]);
        }
        ppimpl_printf(state, "\n# %i %s\n", 0, path->data);

        // Close the file.
        bfclose(file);
    }
    else
        dhalt(ERR_PP_ASM_INCLUDE_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    
    free(store);
    ppparam_free(result);
}

void ppimpl_asm_include_register(state_t* state)
{
    // Register .INCLUDE directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".INCLUDE "));
    match->handler = include_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #INCLUDE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#INCLUDE "));
    match->handler = include_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);
}
