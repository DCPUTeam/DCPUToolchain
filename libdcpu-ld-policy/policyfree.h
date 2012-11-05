///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Provides functions for correctly free'ing policy data.
/// @author James Rhodes
///

#ifndef __DCPU_LD_POLICY_FREE_H
#define __DCPU_LD_POLICY_FREE_H

#include <dcpu.h>
#include "policyast.h"

void free_policy(bool runtime, policy_t* policy);
void free_policies(bool runtime, policies_t* policies);
void free_policy_function_call(bool runtime, policy_function_call_t* call);
void free_policy_value(bool runtime, policy_value_t* value);
void free_policy_variable(bool runtime, policy_variable_t* variable);
void free_policy_instruction(bool runtime, policy_instruction_t* instruction);
void free_policy_setting(bool runtime, policy_setting_t* setting);
void free_policy_state(bool runtime, policy_state_t* state);

#endif

///
/// @}
///