///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Implements the functionality for executing policy scripts.
/// @author James Rhodes
///

#include <stdlib.h>
#include "dcpu.h"
#include "policy.h"
#include "policyast.h"
#include "policyexec.h"
#include "parser.h"
#include "assert.h"

static policy_value_t* value_evaluate(policy_state_t* state, policy_value_t* value);

static policy_value_t* function_evaluate(policy_state_t* state, policy_function_call_t* call)
{
    policy_value_t* value_0;
    policy_value_t* value_1;
    policy_value_t* value_2;
    policy_value_t* new_value;
    int int_result;
    list_t list_result;
    list_t* list_copy;
    
    switch (call->function)
    {
        case FUNC_TOTAL:
            assert(call->parameters != NULL && list_size(call->parameters) == 1);
            value_0 = list_get_at(call->parameters, 0);
            assert(value_0->type == TABLE);
            int_result = state->call_total_table(value_0->number);
            new_value = malloc(sizeof(policy_value_t));
            new_value->type = NUMBER;
            new_value->number = int_result;
            return new_value;
        case FUNC_FIELD:
            assert(call->parameters != NULL && list_size(call->parameters) == 3);
            value_0 = list_get_at(call->parameters, 0);
            value_1 = list_get_at(call->parameters, 1);
            value_2 = list_get_at(call->parameters, 2);
            assert(value_0->type == TABLE);
            assert(value_1->type == NUMBER || value_1->type == VARIABLE);
            assert(value_2->type == FIELD);
            value_1 = value_evaluate(state, value_1);
            if (value_1 == NULL)
                return NULL;
            list_result = state->call_field(value_0->number, value_1->number, value_2->number);
            list_copy = malloc(sizeof(list_t));
            memcpy(list_copy, &list_result, sizeof(list_t));
            new_value = malloc(sizeof(policy_value_t));
            new_value->type = LIST;
            new_value->list = list_copy;
            return new_value;
        case FUNC_CODE:
            assert(call->parameters == NULL || list_size(call->parameters) == 0);
            list_result = state->call_code();
            list_copy = malloc(sizeof(list_t));
            memcpy(list_copy, &list_result, sizeof(list_t));
            new_value = malloc(sizeof(policy_value_t));
            new_value->type = LIST;
            new_value->list = list_copy;
            return new_value;
        case FUNC_WORDS:
            new_value = malloc(sizeof(policy_value_t));
            new_value->type = LIST;
            new_value->list = call->parameters;
            return new_value;
        default:
            state->error(bautofree(bfromcstr("unknown function was called.")));
            return NULL;
    }
}

static policy_value_t* value_evaluate(policy_state_t* state, policy_value_t* value)
{
    policy_value_t* new_value;
    policy_variable_t* variable;
    
    switch (value->type)
    {
        case NUMBER:
        case WORD:
        case TABLE:
        case FIELD:
            new_value = malloc(sizeof(policy_value_t));
            memcpy(new_value, value, sizeof(policy_value_t));
            return new_value;
        case LIST:
            new_value = malloc(sizeof(policy_value_t));
            memset(new_value, '\0', sizeof(policy_value_t));
            new_value->type = LIST;
            new_value->list = malloc(sizeof(policy_value_t));
            list_init(new_value->list);
            list_iterator_start(value->list);
            while (list_iterator_hasnext(value->list))
                list_append(new_value->list, list_iterator_next(value->list));
            list_iterator_stop(value->list);
        case VARIABLE:
            list_iterator_start(&state->variables);
            while (list_iterator_hasnext(&state->variables))
            {
                variable = list_iterator_next(&state->variables);
                if (biseq(variable->name, value->string))
                {
                    list_iterator_stop(&state->variables);
                    return value_evaluate(state, variable->value);
                }
            }
            list_iterator_stop(&state->variables);
            
            state->error(bautofree(bformat("referenced variable '%s' was not found.", value->string->data)));
            return NULL;
        case FUNCTION:
            return function_evaluate(state, value->function);
        default:
            assert(false);
            return NULL;
    }
}

static void instructions_execute(policies_t* policies, policy_state_t* state, list_t* instructions)
{
    policy_state_t* new_state;
    policy_value_t* value;
    policy_value_t* target;
    policy_variable_t* var;
    policy_instruction_t* inst;
    size_t for_remove_at;
    list_t temp;
    assert(instructions != NULL);
    
    list_iterator_start(instructions);
    while (list_iterator_hasnext(instructions))
    {
        inst = list_iterator_next(instructions);
        
        switch (inst->type)
        {
            case INST_CHAIN:
                // Set up a new execution state.
                new_state = malloc(sizeof(policy_state_t));
                assert(inst->value->type == WORD);
                memcpy(new_state, state, sizeof(policy_state_t));
                new_state->policy = policies_get_policy(policies, bautocpy(inst->value->string));
                list_init(&new_state->variables);
                if (new_state->policy == NULL)
                {
                    free(new_state);
                    state->error(bautofree(bfromcstr("policy to chain into not found.")));
                    list_iterator_stop(instructions);
                    return;
                }
                
                // Execute new state.
                instructions_execute(policies, new_state, new_state->policy->instructions);
                
                // Free state.
                free(new_state);
                break;
            case INST_OFFSET:
                value = value_evaluate(state, inst->value);
                if (value->type != NUMBER)
                {
                    free(value);
                    state->error(bautofree(bfromcstr("expected numeric value for parameter to offset().")));
                    list_iterator_stop(instructions);
                    return;
                }
                state->call_offset(value->number);
                free(value);
                break;
            case INST_WRITE:
                value = value_evaluate(state, inst->value);
                if (value == NULL)
                    return;
                if (value->type == LIST)
                {
                    state->call_write(*value->list);
                    free(value);
                }
                else if (value->type == NUMBER)
                {
                    list_init(&temp);
                    list_append(&temp, (void*)value->number);
                    state->call_write(temp);
                    list_destroy(&temp);
                    free(value);
                }
                else
                {
                    free(value);
                    state->error(bautofree(bfromcstr("expected numeric or list value for parameter to write().")));
                    list_iterator_stop(instructions);
                    return;
                }
                break;
            case KEYWORD_FOR:
            {
                // Check to see if the variable already exists.
                list_iterator_start(&state->variables);
                while (list_iterator_hasnext(&state->variables))
                {
                    var = list_iterator_next(&state->variables);
                    if (biseq(var->name, inst->for_variable))
                    {
                        state->error(bautofree(bformat("variable already declared in this scope '%s'.", var->name->data)));
                        list_iterator_stop(&state->variables);
                        return;
                    }
                }
                list_iterator_stop(&state->variables);
                
                // Variable does not exist.  Create it with the
                // start value.
                var = malloc(sizeof(policy_variable_t));
                var->name = bstrcpy(inst->for_variable);
                var->value = value_evaluate(state, inst->for_start);
                
                // Evaluate the end target.
                target = value_evaluate(state, inst->for_end);
                if (target->type != NUMBER)
                {
                    state->error(bautofree(bformat("for loop end must result in numeric value.")));
                    return;
                }
                
                // Check value and if okay, append.
                if (var->value->type != NUMBER)
                {
                    state->error(bautofree(bformat("for loop start / variable '%s' must contain numeric value for iteration.", var->name->data)));
                    return;
                }
                for_remove_at = list_size(&state->variables);
                list_append(&state->variables, var);
                printf("added %s to the list of variables.\n", var->name->data);
                
                // Now perform the loop.
                for (; var->value->number < target->number; var->value->number++)
                    instructions_execute(policies, state, inst->for_instructions);
                
                // Now remove the variable.
                printf("removed %s to the list of variables.\n", var->name->data);
                list_delete_at(&state->variables, for_remove_at);
                free(var);
                break;
            }
            default:
                assert(false);
                break;
        }
    }
    list_iterator_stop(instructions);
}

void policy_execute(policies_t* policies, policy_state_t* state)
{
    instructions_execute(policies, state, state->policy->instructions);
}

///
/// @}
///