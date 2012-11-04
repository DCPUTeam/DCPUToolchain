///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Implementation of general policy functionality.
/// @author James Rhodes
///

#include <assert.h>
#include <stdlib.h>
#include "policyast.h"
#include "policyexec.h"
#include "policyfree.h"
#include "parser.h"

policies_t* active_ast; 
extern int yyparse();
extern FILE* yyin, *yyout;

///
/// @brief Loads policies in from a file (or stdin).
///
/// Parses the specified file as a policy list and returns the 
/// resulting policy AST.
///
/// @param input The input file to parse from.
/// @return The policies AST.
///
policies_t* policies_load(FILE* input)
{
    // Set up initial AST structure.
    policies_t* result = malloc(sizeof(policies_t));
    result->settings = NULL;
    list_init(&result->policies);
    
    // Parse into AST.
    yyin = input;
    yyout = NULL;
    active_ast = result;
    yyparse();
    active_ast = NULL;
    
    // Return.
    return result;
}

///
/// @brief Free the set of policies.
///
/// @param policies The policies AST.
///
void policies_free(policies_t* policies)
{
    free_policies(false, policies);
}

///
/// @brief Retrieves a policy based on the name.
///
/// @param policies The policies AST.
/// @param name The name of the policy to find.
/// @return The found policy or NULL.
///
policy_t* policies_get_policy(policies_t* policies, freed_bstring name)
{
    policy_t* policy;
    
    list_iterator_start(&policies->policies);
    while (list_iterator_hasnext(&policies->policies))
    {
        policy = list_iterator_next(&policies->policies);
        if (biseq(policy->name, name.ref))
        {
            bautodestroy(name);
            list_iterator_stop(&policies->policies);
            return policy;
        }
    }
    bautodestroy(name);
    return NULL;
}

///
/// @brief Retrieves the policy settings.
///
/// @param policies The policies AST.
/// @return The settings policy or NULL if none was provided.
///
policy_t* policies_get_settings(policies_t* policies)
{
    return policies->settings;
}

///
/// @brief Retrieves the value associated with the specified setting.
///
/// Returns the value associated with the specified key.  Returns NULL
/// if no such key exists.
///
/// @note The returned bstring is a reference to the value and will be
/// automatically freed when policies_free() is called.
///
/// @param policies The policies AST.
/// @param key The name of the setting.
/// @return The value of the setting.
///
bstring policies_get_setting(policies_t* policies, freed_bstring key)
{
    policy_setting_t* setting = NULL;
    if (policies->settings == NULL)
        return NULL;
    list_iterator_start(policies->settings->settings);
    while (list_iterator_hasnext(policies->settings->settings))
    {
        setting = list_iterator_next(policies->settings->settings);
        if (biseq(setting->key, key.ref))
        {
            list_iterator_stop(policies->settings->settings);
            bautodestroy(key);
            return setting->value;
        }
    }
    list_iterator_stop(policies->settings->settings);
    bautodestroy(key);
    return NULL;
}

///
/// @brief Creates a new execution state for a policy.
///
/// @param policy The policy to create a state from.
/// @return The new policy execution state.
///
policy_state_t* state_from_policy(policy_t* policy)
{
    policy_state_t* state = malloc(sizeof(policy_state_t));
    
    // Set up the new execution state.
    state->policy = policy;
    state->call_code = NULL;
    state->call_field = NULL;
    state->call_offset = NULL;
    state->call_section = NULL;
    state->call_total_section = NULL;
    state->call_total_table = NULL;
    state->call_write = NULL;
    list_init(&state->variables);
    
    return state;
}

///
/// @brief Executes the policy.
///
/// @note This function requires that all of the call_ fields have been
/// set on the execution state.
///
/// @param policies The list of all available policies (used for chain).
/// @param state The policy execution state with all call_ fields set.
///
void state_execute(policies_t* policies, policy_state_t* state)
{
    assert(state->call_code != NULL);
    assert(state->call_field != NULL);
    assert(state->call_offset != NULL);
    assert(state->call_total_table != NULL);
    assert(state->call_write != NULL);
    assert(state->error != NULL);
    
    // Call into the policyexec.c file to do the actual work.
    policy_execute(policies, state);
}

///
/// @brief Free the policy execution state.
///
/// @param state The policy execution state.
///
void state_free(policy_state_t* state)
{
    free_policy_state(false, state);
}

///
/// @}
///