#include "../../ppparam.h"
#include <ppparamparser.h>
#include <stdlib.h>
#include <bstring.h>
#include <bfile.h>
#include <derr.h>
#include "../asm.h"

static state_t* replace_state = NULL;


static void skip_to_endln(state_t* state)
{
    char c;
    
    while (ppimpl_has_input(state))
    {
        c = ppimpl_get_input(state);
        if (c == '\n')
            return;
    }
}

static bstring skip_to_endif(state_t* state, bool stop_at_else, bool* stopped_at_else)
{
    char c;
    bool fresh_line = true;
    bstring temp = bfromcstr("");
    bstring temp_output = bfromcstr("");
    bstring output = bfromcstr("");
    int if_open = 1;
    
    
    while (ppimpl_has_input(state))
    {
        c = ppimpl_get_input(state);
        switch(c)
        {
            case '#':
            case '.':
                if (!fresh_line)
                {
                    bconchar(output, c);
                    break;
                }
                bassigncstr(temp_output, "#");
                // first skip spaces
                while (ppimpl_has_input(state))
                {
                    c = ppimpl_get_input(state);
                    bconchar(temp_output, c);
                    if (c != ' ' && c != '\t')
                        break;
                }
                // read pp directive
                bassigncstr(temp, "");
                bconchar(temp, c);
                while (ppimpl_has_input(state))
                {
                    c = ppimpl_get_input(state);
                    bconchar(temp_output, c);
                    if (c == ' ' || c == '\t' || c == '\n')
                        break;
                    bconchar(temp, c);
                }
                
                btolower(temp);
                
                if (biseq(temp, bfromcstr("endif")))
                {
                    if_open--;
                    if (if_open == 0)
                    {
                        if (c != '\n') skip_to_endln(state);
                        *stopped_at_else = false;
                        return output;
                    }
                }
                else if (biseq(temp, bfromcstr("if")))
                {
                    if_open++;
                }
                else if (biseq(temp, bfromcstr("else")) && stop_at_else)
                {
                    if (if_open == 1)
                    {
                        if (c != '\n') skip_to_endln(state);
                        *stopped_at_else = true;
                        return output;
                    }
                }
                bconcat(output, temp_output);
                fresh_line = (c == '\n');
                break;
                
            case '\n':
                fresh_line = true;
                bconchar(output, c);
                break;
            case ' ':
            case '\t':
                bconchar(output, c);
                break;
                
            default:
                fresh_line = false;
                bconchar(output, c);
                break;
        }
        
    }
    
    // no endif was found
    dhalt(ERR_PP_ASM_NO_ENDIF_TO_IF, ppimpl_get_location(state));
}


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
    bstring output;
    bool stopped_at_else;

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
        {
            output = skip_to_endif(state, true, &stopped_at_else);
            if (stopped_at_else)
                skip_to_endif(state, false, &stopped_at_else);
        }
        else
        {
            bassigncstr(output, "");
            skip_to_endif(state, true, &stopped_at_else);
            if (stopped_at_else)
            {
                output = skip_to_endif(state, false, &stopped_at_else);
            }
        }
        
        // print the output to the pre processor input
        ppimpl_printf(state, "%s", output->data);
    }
    else
        dhalt(ERR_PP_ASM_IF_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    
    ppparam_free(result);
}

static void else_handle(state_t* state, match_t* match, bool* reprocess)
{
    // free else encountered
    dhalt(ERR_PP_ASM_ELSE_NO_IF, ppimpl_get_location(state));
}

static void endif_handle(state_t* state, match_t* match, bool* reprocess)
{
    // free else encountered
    dhalt(ERR_PP_ASM_ENDIF_NO_IF, ppimpl_get_location(state));
}

void ppimpl_asm_expr_register(state_t* state)
{
    // Register .IF directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".IF "));
    match->handler = if_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register .ELSE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".ELSE"));
    match->handler = else_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register .ENDIF directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".ENDIF"));
    match->handler = endif_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #IF directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#IF "));
    match->handler = if_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #ELSE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#ELSE"));
    match->handler = else_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #ENDIF directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#ENDIF"));
    match->handler = endif_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);
}
