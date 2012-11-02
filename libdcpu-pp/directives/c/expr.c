#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <bstring.h>
#include <bfile.h>
#include <derr.h>
#include "../c.h"

static state_t* replace_state = NULL;

static uint16_t if_define_replace(bstring define)
{
    // Search through all of the defines to find one where
    // the name matches.
    size_t i = 0;
    match_t* match;
    struct expr* tree;
    for (i = 0; i < list_size(&replace_state->handlers); i++)
    {
        match = list_get_at(&replace_state->handlers, i);
        if (biseq(match->text.ref, define))
        {
            // Found a match.
            if (match->userdata == NULL)
                printf("party time with '%s'.\n", match->text.ref->data);
            tree = expr_parse(match->userdata);
            if (tree == NULL)
                dhalt(ERR_PP_DEFINE_NOT_EXPRESSION, ppimpl_get_location(replace_state));
            return expr_evaluate(tree, &if_define_replace, &dhalt_expression_exit_handler);
        }
    }
    dhalt(ERR_PP_DEFINE_NOT_FOUND, ppimpl_get_location(replace_state));
    return 0;
}

static void if_handle(state_t* state, match_t* match, bool* reprocess)
{
    list_t* result = ppparam_get(state);
    struct expr* expr = NULL;
    uint16_t value;

    // Ensure the parameter format is correct.
    if (list_size(result) == 1 &&
            ((parameter_t*)list_get_at(result, 0))->type == EXPRESSION)
    {
        // Get the expression.
        expr = ((parameter_t*)list_get_at(result, 0))->expr;
        replace_state = state;
        value = expr_evaluate(expr, &if_define_replace, &dhalt_expression_exit_handler);
        replace_state = NULL;
        if (value)
            ppimpl_push_scope(state, true);
        else
            ppimpl_push_scope(state, false);
    }
    else
        dhalt(ERR_PP_C_IF_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    
    ppparam_free(result);
}

static void else_handle(state_t* state, match_t* match, bool* reprocess)
{
    ppimpl_flip_scope(state);
}

static void endif_handle(state_t* state, match_t* match, bool* reprocess)
{
    ppimpl_pop_scope(state);
}

void ppimpl_c_expr_register(state_t* state)
{
    // Register #if directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#if"));
    match->handler = if_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);

    // Register #else directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#else"));
    match->handler = else_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);

    // Register #endif directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#endif"));
    match->handler = endif_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);
}
