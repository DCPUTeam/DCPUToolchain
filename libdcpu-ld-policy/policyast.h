///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Provides access to the policy script's AST.
/// @author James Rhodes
///

#ifndef __DCPU_LD_POLICY_AST_FIRST_H
#define __DCPU_LD_POLICY_AST_FIRST_H

#include <stdio.h>
#include <simclist.h>
#include <bstring.h>

///
/// @brief The structure that represents a linker policy.
///
typedef struct
{
    int type; //< The type of linker policy, either POLICY_SETTINGS, POLICY_INTERNAL or POLICY_FORMAT.
    bstring name; //< The name of the policy, or NULL if type is POLICY_SETTINGS.
    list_t* instructions; //< The set of instructions associated with this policy, NULL otherwise.
    list_t* settings; //< The set of key-value pairs if type is POLICY_SETTINGS, NULL otherwise.
} policy_t;

///
/// @brief The structure to represent a series of policies.
///
typedef struct
{
    list_t policies; //< A list of POLICY_INTERNAL and POLICY_FORMAT.
    policy_t* settings; //< The POLICY_SETTINGS configuration, or NULL if not provided.
} policies_t;

///
/// @brief A structure representing an AST function call.
///
typedef struct
{
    int function; //< The ID of the function to call, either FUNC_TOTAL, FUNC_FIELD, FUNC_WORDS or FUNC_CODE.
    list_t* parameters; //< A list of policy_value_t parameters, or NULL if there are no parameters.
} policy_function_call_t;

///
/// @brief A structure representing an AST value.
///
typedef struct
{
    int type; //< The type of value, either WORD, NUMBER, VARIABLE, FUNCTION, LIST, TABLE or FIELD.
    union
    {
        int number; //< The numeric value (NUMBER).
        int table; //< The table value (TABLE).
        int field; //< The field value (FIELD).
        bstring string; //< The string value (WORD, VARIABLE).
        policy_function_call_t* function; //< The function call value (FUNCTION).
        list_t* list; //< A list of policy_value_t for values containing other values (LIST).
    };
} policy_value_t;

///
/// @brief A structure representing a variable at execution time.
///
typedef struct
{
    bstring name; //< The name of the variable.
    policy_value_t* value; //< The value of the variable.
} policy_variable_t;

///
/// @brief The structure to represent a policy instructions.
///
typedef struct
{
    int type; //< The instruction type, either INST_WRITE, INST_CHAIN, INST_OFFSET or KEYWORD_FOR.
    policy_value_t* value; //< The main value parameter to this instruction, or NULL if unused.
    bstring for_variable; //< The name of the variable to use in the for loop.
    policy_value_t* for_start; //< The value parameter for the start of the for loop.
    policy_value_t* for_end; //< The value parameter for the end of the for loop.
    list_t* for_instructions; //< The instructions to evaluate in the for loop.
} policy_instruction_t;

///
/// @brief The structure to represent a policy setting.
///
typedef struct
{
    bstring key; //< The name of the setting.
    bstring value; //< The value of the setting.
} policy_setting_t;

///
/// @brief Function signature for when the policy executor encounters an error.
///
/// When this is called back, the handler should use the message to display to the
/// user that there was an error during execution.  After this callback is made,
/// no more instructions are evaluated and thus the result of execution should be
/// considered to be an inconsistent resulting output.
///
/// @note The handler should free the error string with bautodestroy().
///
/// @param error The error message.
///
typedef void (*policy_error_t)(freed_bstring error);

#endif

#ifndef __DCPU_LD_POLICY_AST_SECOND_H
#define __DCPU_LD_POLICY_AST_SECOND_H

// Include the policy execution typedefs.
#include "policyexec.h"

///
/// @brief The structure to represent the state of a policy for execution.
///
/// This structure holds a reference to the policy and a list of function
/// hooks that should be used to evaluate the policy.
///
typedef struct
{
    policy_t* policy; //< The policy this state will evaluate.
    policy_call_offset_t call_offset; //< Call handler for offset() function.
    policy_call_write_t call_write; //< Call handler for write() function.
    policy_call_total_table_t call_total_table; //< Call handler for total(table) function.
    policy_call_total_section_t call_total_section; //< Call handler for total(section) function.
    policy_call_field_t call_field; //< Call handler for field() function.
    policy_call_code_t call_code; //< Call handler for code() function.
    policy_call_section_t call_section; //< Call handler for section() function.
    policy_error_t error; //< Error handler for when execution fails.
    list_t variables; //< A list of local variables (policy_variable_t).
} policy_state_t;

void policy_execute(policies_t* policies, policy_state_t* state);

#endif

///
/// @}
///