///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Defines functions for correctly free'ing policy data.
/// @author James Rhodes
///

#include <stdlib.h>
#include "policyast.h"
#include "policyfree.h"
#include "parser.h"

void free_policy(bool runtime, policy_t* policy)
{
    if (runtime)
        return;
    if (policy->name != NULL)
        bdestroy(policy->name);
    if (policy->instructions != NULL)
    {
        list_iterator_start(policy->instructions);
        while (list_iterator_hasnext(policy->instructions))
            free_policy_instruction(runtime, list_iterator_next(policy->instructions));
        list_iterator_stop(policy->instructions);
        list_destroy(policy->instructions);
        free(policy->instructions);
    }
    if (policy->settings != NULL)
    {
        list_iterator_start(policy->settings);
        while (list_iterator_hasnext(policy->settings))
            free_policy_setting(runtime, list_iterator_next(policy->settings));
        list_iterator_stop(policy->settings);
        list_destroy(policy->settings);
        free(policy->settings);
    }
    free(policy);
}

void free_policies(bool runtime, policies_t* policies)
{
    if (runtime)
        return;
    list_iterator_start(&policies->policies);
    while (list_iterator_hasnext(&policies->policies))
        free_policy(runtime, list_iterator_next(&policies->policies));
    list_iterator_stop(&policies->policies);
    list_destroy(&policies->policies);
    if (policies->settings != NULL)
        free_policy(runtime, policies->settings);
    free(policies);
}

void free_policy_function_call(bool runtime, policy_function_call_t* call)
{
    if (runtime)
        return;
    if (call->parameters != NULL)
    {
        list_iterator_start(call->parameters);
        while (list_iterator_hasnext(call->parameters))
            free_policy_value(runtime, list_iterator_next(call->parameters));
        list_iterator_stop(call->parameters);
        list_destroy(call->parameters);
        free(call->parameters);
    }
    free(call);
}

void free_policy_value(bool runtime, policy_value_t* value)
{
    if ((runtime && !value->runtime) ||
        (!runtime && value->runtime))
        return;
    switch (value->type)
    {
        case NUMBER:
        case TABLE:
        case FIELD:
            break;
        case WORD:
        case VARIABLE:
            bdestroy(value->string);
            break;
        case FUNCTION:
            free_policy_function_call(runtime, value->function);
            break;
        case LIST:
            list_iterator_start(value->list);
            while (list_iterator_hasnext(value->list))
                free_policy_value(runtime, list_iterator_next(value->list));
            list_iterator_stop(value->list);
            list_destroy(value->list);
            free(value->list);
            break;
        default:
            break;
    }
    free(value);
}

void free_policy_variable(bool runtime, policy_variable_t* variable)
{
    if (!runtime)
        return;
    bdestroy(variable->name);
    free_policy_value(runtime, variable->value);
    free(variable);
}

void free_policy_instruction(bool runtime, policy_instruction_t* instruction)
{
    if (runtime)
        return;
    if (instruction->value != NULL)
        free_policy_value(runtime, instruction->value);
    if (instruction->for_variable != NULL)
        bdestroy(instruction->for_variable);
    if (instruction->for_start != NULL)
        free_policy_value(runtime, instruction->for_start);
    if (instruction->for_end != NULL)
        free_policy_value(runtime, instruction->for_end);
    if (instruction->for_instructions != NULL)
    {
        list_iterator_start(instruction->for_instructions);
        while (list_iterator_hasnext(instruction->for_instructions))
            free_policy_instruction(runtime, list_iterator_next(instruction->for_instructions));
        list_iterator_stop(instruction->for_instructions);
        list_destroy(instruction->for_instructions);
        free(instruction->for_instructions);
    }
    free(instruction);
}

void free_policy_setting(bool runtime, policy_setting_t* setting)
{
    if (runtime)
        return;
    bdestroy(setting->key);
    bdestroy(setting->value);
}

void free_policy_state(bool runtime, policy_state_t* state)
{
    if (runtime)
        return;
    list_iterator_start(&state->variables);
    while (list_iterator_hasnext(&state->variables))
        free_policy_instruction(runtime, list_iterator_next(&state->variables));
    list_iterator_stop(&state->variables);
    list_destroy(&state->variables);
    free(state);
}

///
/// @}
///