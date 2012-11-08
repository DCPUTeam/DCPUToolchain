#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <bstring.h>
#include <bfile.h>
#include <derr.h>
#include "../c.h"
#include "../../ppfind.h"
#include "../../ppimpl.h"

struct substate
{
    state_t* parent;
    state_t* child;
    BFILE* input;
};

static bool include_has(void* userdata)
{
    struct substate* ss = userdata;
    return !bfeof(ss->input);
}

static char include_pop(void* userdata)
{
    struct substate* ss = userdata;
    return (char)bfgetc(ss->input);
}

static void include_push(void* userdata, char value)
{
    struct substate* ss = userdata;
    ppimpl_oprintf(ss->parent, "%c", value);
}

static void include_handle(state_t* state, match_t* match, bool* reprocess)
{
    bstring path;
    BFILE* file;
    list_t* result = ppparam_get(state);
    char* store = malloc(1);
    struct substate* substate = NULL;
    list_t old_handlers;
    list_t old_scopes;

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
        
        // Check that the file was found.
        if (path == NULL)
            dhalt(ERR_PP_C_INCLUDE_FILE_NOT_FOUND, ppimpl_get_location(state));

        // Create the new substate and open file.
        substate = malloc(sizeof(struct substate));
        substate->parent = state;
        substate->input = bfopen(path->data, "r");
        if (substate->input == NULL)
            dhalt(ERR_PP_C_INCLUDE_FILE_NOT_FOUND, ppimpl_get_location(state));

        // Create new preprocessor state.
        substate->child = ppimpl_new(bautofree(path), 1, bautocpy(state->language), include_has, include_pop, include_push, substate);
        
        // Switch out the current list references.
        old_handlers = substate->child->handlers;
        old_scopes = substate->child->scopes;
        substate->child->handlers = substate->parent->handlers;
        substate->child->scopes = substate->parent->scopes;
        
        // Perform preprocessing.
        ppimpl_process(substate->child);
        
        // Switch lists back.
        substate->child->handlers = old_handlers;
        substate->child->scopes = old_scopes;
        
        // Free the child state.
        ppimpl_free(substate->child);
        substate->child = NULL;

        // Close the file.
        bfclose(substate->input);
        
        // Output the tracking line.
        ppimpl_oprintf(state, "# %i %s\n", state->current_line, state->current_filename->data);
        
        // Free the substate container.
        free(substate);
    }
    else
        dhalt(ERR_PP_C_INCLUDE_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    
    free(store);
    ppparam_free(result);
}

void ppimpl_c_include_register(state_t* state)
{
    // Register .INCLUDE directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#include "));
    match->handler = include_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);
}
